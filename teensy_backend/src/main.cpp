//---------------------------------------------------------------------------------------------------------------------------------------
//
//
// Author: Matteo Cartiglia <camatteo@ini.uzh.ch>
// Last updated: 
//---------------------------------------------------------------------------------------------------------------------------------------

#include <Arduino.h>
#include <vector>

#include "constants.h"
#include "datatypes.h"
#include "biasGen.h"
#include "dac.h"
#include "aer.h"


// Declaring function prototypes in order of definition
static void setupLFSR();
static void aerInputEncoder_ISR();
static void aerInputC2F_ISR();
static void sendTeensyStatus(TeensyStatus status);

//------------------------------------------------------- Defining Global Variables ------------------------------------------------------ 

static P2TPkt inputBuffer;
bool aerInputEncoder_bufferActive = false;
bool aerInputC2F_bufferActive = false;

int aerInputEncoder_dataPins[AER_ENCODER_INPUT_NO_PIN] = {AER_ENCODER_INPUT_BIT_0_PIN, AER_ENCODER_INPUT_BIT_1_PIN, AER_ENCODER_INPUT_BIT_2_PIN};
int aerInputC2F_dataPins[C2F_INPUT_NO_PIN] = {C2F_INPUT_BIT_0_PIN, C2F_INPUT_BIT_1_PIN, C2F_INPUT_BIT_2_PIN, C2F_INPUT_BIT_3_PIN, C2F_INPUT_BIT_4_PIN};
int aerOutputDecoder_dataPins[AER_DECODER_OUTPUT_NO_PIN] = {AER_DECODER_OUTPUT_BIT_0_PIN, AER_DECODER_OUTPUT_BIT_1_PIN, AER_DECODER_OUTPUT_BIT_2_PIN, AER_DECODER_OUTPUT_BIT_3_PIN,
                            AER_DECODER_OUTPUT_BIT_4_PIN, AER_DECODER_OUTPUT_BIT_5_PIN, AER_DECODER_OUTPUT_BIT_6_PIN, AER_DECODER_OUTPUT_BIT_7_PIN, AER_DECODER_OUTPUT_BIT_8_PIN};

AER aerInputEncoder(AER_ENCODER_REQ, AER_ENCODER_ACK, aerInputEncoder_dataPins, AER_ENCODER_INPUT_NO_PIN, NULL, AER_HANDSHAKE_DELAY, AER_RECORD_EVENT_TSTAMP_SHIFT, false);
AER aerInputC2F(C2F_REQ, C2F_ACK, aerInputC2F_dataPins, C2F_INPUT_NO_PIN, NULL, AER_HANDSHAKE_DELAY, AER_RECORD_EVENT_TSTAMP_SHIFT, false);
AER aerOutputDecoder(AER_DECODER_REQ, AER_DECODER_ACK, aerOutputDecoder_dataPins, AER_DECODER_OUTPUT_NO_PIN, AER_HANDSHAKE_DELAY, false);

DAC dac{DAC_RESET, DAC_A0, DAC_A1};
BiasGen biasGen{BIASGEN_SCK_PIN , BIASGEN_RESET_PIN , BIASGEN_MOSI_PIN, BIASGEN_ENABLE_PIN};

//---------------------------------------------------------------------------------------------------------------------------------------
// "setup" function: 
//---------------------------------------------------------------------------------------------------------------------------------------

void setup() 
{
    // Setting up Encoder comms, C2F comms and Learning Block LFSR
    // NOTE: Decoder, BiasGen and DAC comms implicitly setup when object is created 
    aerInputEncoder.setupEncoderComms();
    aerInputC2F.setupC2FComms();
    setupLFSR();

    attachInterrupt(digitalPinToInterrupt(AER_ENCODER_REQ), aerInputEncoder_ISR, CHANGE);
    attachInterrupt(digitalPinToInterrupt(C2F_REQ), aerInputC2F_ISR, CHANGE);

    dac.join_i2c_bus();
    dac.turn_reference_off();

    Serial.print("********************* Setup complete *********************\n");
}

//---------------------------------------------------------------------------------------------------------------------------------------
// "main" function: reads in serial communication and 
//---------------------------------------------------------------------------------------------------------------------------------------

void loop() 
{    
   if (usb_serial_available()) 
   {
        // Read the data in the input buffer
        usb_serial_read(&inputBuffer, sizeof(inputBuffer));
        
        // Interpret the input data
        switch ((P2tPktType)inputBuffer.header) 
        {
            // Setup bias generator
            case P2tPktType::P2t_setBiasGen:
            {  
                BIASGEN_command biasGenCommand(inputBuffer);
                biasGen.writeBiasGen(biasGenCommand.address, biasGenCommand.currentValue_binary);
              
                delay(100);
                sendTeensyStatus(TeensyStatus::Success);

                Serial.print("BiasGen command received. Pin ");
                // Serial.print((biasGenCommand.name).c_str());
                Serial.print(biasGenCommand.address);
                Serial.print(" set to ");
                Serial.print(biasGen.getBiasGenDecimal(biasGenCommand.currentValue_binary), 6);
                Serial.print(" uA.\n");

                break;
            }

            // Setup DAC
            case P2tPktType::P2t_setDACvoltage:
            { 
                DAC_command DAC(inputBuffer);
                dac.write_dacs(DAC.command_address, DAC.data); 

                delay(100);
                sendTeensyStatus(TeensyStatus::Success);

                Serial.print("DAC command received. Pin ");
                Serial.print(DAC.command_address);
                Serial.print(" set to ");
                Serial.print(DAC.data);
                Serial.print(" mV.\n");

                break;   
            }

            // Request decoder output
            case P2tPktType::P2t_aerDecoder_reqOutput:
            {
                AER_DECODER_OUTPUT_command aerDecoder(inputBuffer);
                aerOutputDecoder.dataWrite(aerDecoder.data);            // Ask about handshake & req/ack
                break;
            }

            default: 
                sendTeensyStatus(TeensyStatus::UnknownCommand);
                break;  
        } 
   }
    // transmitAnyAerEvents();
} ;


//---------------------------------------------------------------------------------------------------------------------------------------
// Setup LFSR
//---------------------------------------------------------------------------------------------------------------------------------------

static void setupLFSR()
{
  pinMode(LB_LFSR_RST, OUTPUT);
  pinMode(LB_LFSR_CLK, OUTPUT);
  delay(10);

  digitalWrite(LB_LFSR_RST, HIGH);
  delay(0.001);
  digitalWrite(LB_LFSR_RST, LOW);
  delay(1000);
  digitalWrite(LB_LFSR_CLK, LOW);
  delay(1);
}

//---------------------------------------------------------------------------------------------------------------------------------------
// AER Input Interrupt Servie Routine (ISR) for encoder input
//---------------------------------------------------------------------------------------------------------------------------------------

static void aerInputEncoder_ISR()
{
    if (!aerInputEncoder.reqRead()) 
    {
        if (aerInputEncoder_bufferActive  && (aerInputEncoder.get_index() < AER_BUFFER_SIZE) ) 
        { 
            aerInputEncoder.record_event();
        }

        aerInputEncoder.ackWrite(0);

        if (aerInputEncoder.reqRead())
        {
            aerInputEncoder.ackWrite(1);
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------
// AER Input Interrupt Servie Routine (ISR) for C2F input
//---------------------------------------------------------------------------------------------------------------------------------------

static void aerInputC2F_ISR()
{
    if (!aerInputC2F.reqRead()) 
    {
        if (aerInputC2F_bufferActive  && (aerInputC2F.get_index() < AER_BUFFER_SIZE) ) 
        { 
            aerInputC2F.record_event();
        }

        aerInputC2F.ackWrite(0);

        if (aerInputC2F.reqRead())
        {
            aerInputC2F.ackWrite(1);
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------
// sendStatus  
//---------------------------------------------------------------------------------------------------------------------------------------

static void sendTeensyStatus(TeensyStatus status)
{
    std::uint8_t teensyStatusBuffer[1] = {(uint8_t) status};
    usb_serial_write(teensyStatusBuffer, sizeof(teensyStatusBuffer));
}

