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
#include "teensyIn.h"
#include "teensyOut.h"

// Declaring function prototypes in order of definition
static void setupRST();
static void aerInputEncoder_ISR();
static void aerInputC2F_ISR();
static void sendTeensyStatus(TeensyStatus status);

//------------------------------------------------------- Defining Global Variables ------------------------------------------------------ 

static P2TPkt inputBuffer;
std::int8_t inputBinC2F;
std::int8_t inputBinEncoder;

int inputEncoder_dataPins[ENCODER_INPUT_NO_PIN] = {ENCODER_INPUT_BIT_0_PIN, ENCODER_INPUT_BIT_1_PIN, ENCODER_INPUT_BIT_2_PIN};
int inputC2F_dataPins[C2F_INPUT_NO_PIN] = {C2F_INPUT_BIT_0_PIN, C2F_INPUT_BIT_1_PIN, C2F_INPUT_BIT_2_PIN, C2F_INPUT_BIT_3_PIN, C2F_INPUT_BIT_4_PIN};
int outputDecoder_dataPins[DECODER_OUTPUT_NO_PIN] = {DECODER_OUTPUT_BIT_0_PIN, DECODER_OUTPUT_BIT_1_PIN, DECODER_OUTPUT_BIT_2_PIN, DECODER_OUTPUT_BIT_3_PIN,
                            DECODER_OUTPUT_BIT_4_PIN, DECODER_OUTPUT_BIT_5_PIN, DECODER_OUTPUT_BIT_6_PIN, DECODER_OUTPUT_BIT_7_PIN, DECODER_OUTPUT_BIT_8_PIN};

TeensyIn inputEncoder(ENCODER_REQ, ENCODER_ACK, inputEncoder_dataPins, ENCODER_INPUT_NO_PIN, TEENSY_INPUT_ENCODER);
TeensyIn inputC2F(C2F_REQ, C2F_ACK, inputC2F_dataPins, C2F_INPUT_NO_PIN, TEENSY_INPUT_C2F);
TeensyOut outputDecoder(DECODER_REQ, DECODER_ACK, outputDecoder_dataPins, DECODER_OUTPUT_NO_PIN);

DAC dac{DAC_RESET, DAC_A0, DAC_A1};
BiasGen biasGen{BIASGEN_SCK_PIN , BIASGEN_RESET_PIN , BIASGEN_MOSI_PIN, BIASGEN_ENABLE_PIN};

//---------------------------------------------------------------------------------------------------------------------------------------
// "setup" function: 
//---------------------------------------------------------------------------------------------------------------------------------------

void setup() 
{
    setupRST();

    attachInterrupt(digitalPinToInterrupt(ENCODER_REQ), aerInputEncoder_ISR, CHANGE);
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
                biasGen.writeBiasGen(biasGenCommand.biasNo, biasGenCommand.currentValue_binary);
              
                delay(100);
                sendTeensyStatus(TeensyStatus::Success);

                Serial.print("BIASGEN command received. Bias ");
                Serial.print(biasGenCommand.biasNo);
                Serial.print(" set to approx. ");
                Serial.print(biasGen.getBiasGenDecimal(biasGenCommand.currentValue_binary), 6);
                Serial.print(" uA.");
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
                Serial.print(" mV.");
                break;   
            }

            // Request decoder output
            case P2tPktType::P2t_reqOutputDecoder:
            {
                AER_DECODER_OUTPUT_command decoder(inputBuffer);
                outputDecoder.dataWrite(decoder.data);            

                Serial.print("AER command received. Binary value: ");
                Serial.print(decoder.data, BIN);
                break;
            }

            // Get encoder input value
            case P2tPktType::P2t_reqInputEncoder:
            {
                inputBinEncoder = inputEncoder.dataRead();
                Serial.print(inputBinEncoder);
            }
                
            // Get C2F input value
            case P2tPktType::P2t_reqInputC2F:
            {
                inputBinC2F = inputC2F.dataRead();
                Serial.print(inputBinC2F);
            }

            default: 
            {
                sendTeensyStatus(TeensyStatus::UnknownCommand);
                break;  
            }
        } 
   }
};


//---------------------------------------------------------------------------------------------------------------------------------------
// Setup LFSR
//---------------------------------------------------------------------------------------------------------------------------------------

static void setupRST()
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
    if (!inputEncoder.reqRead()) 
    {
        inputEncoder.ackWrite(0);

        if (inputEncoder.reqRead())
        {
            inputEncoder.ackWrite(1);
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------
// AER Input Interrupt Servie Routine (ISR) for C2F input
//---------------------------------------------------------------------------------------------------------------------------------------

static void aerInputC2F_ISR()
{
    if (!inputC2F.reqRead()) 
    {
        inputC2F.ackWrite(0);

        if (inputC2F.reqRead())
        {
            inputC2F.ackWrite(1);
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------
// sendStatus  
//---------------------------------------------------------------------------------------------------------------------------------------

static void sendTeensyStatus(TeensyStatus status)
{
    std::uint8_t teensyStatusBuffer[1] = {(uint8_t) status};
    // usb_serial_write(teensyStatusBuffer, sizeof(teensyStatusBuffer));
}

