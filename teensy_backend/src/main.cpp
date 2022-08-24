//---------------------------------------------------------------------------------------------------------------------------------------
// main.cpp file containing main function and ISR interrupt functions
//
// Author: Ciara Giles-Doran <gciara@student.ethz.ch>
// Last updated: 15 JUL 2022
//---------------------------------------------------------------------------------------------------------------------------------------

#include <Arduino.h>
#include <vector>
#include <iostream>

#include "constants.h"
#include "datatypes.h"
#include "spiConfig.h"

#include "dac.h"
#include "teensyIn.h"
#include "teensyOut.h"

// Declaring function prototypes in order of definition
static void setupLFSR();
static void resetChip();
static void aerInputEncoder_ISR();
static void aerInputC2F_ISR();
static void sendTeensyStatus(TeensyStatus status);


//------------------------------------------------------- Defining Global Variables ------------------------------------------------------ 

static Pkt inputBuffer; // missnomer. Input command would be more appropriate


#ifdef EXISTS_INPUT_ENCODER
    int inputEncoder_dataPins[ENCODER_INPUT_NO_PIN] = {ENCODER_INPUT_BIT_0_PIN, ENCODER_INPUT_BIT_1_PIN, ENCODER_INPUT_BIT_2_PIN};
    TeensyIn inputEncoder(ENCODER_REQ, ENCODER_ACK, inputEncoder_dataPins, ENCODER_INPUT_NO_PIN, ENCODER_DELAY, ENCODER_ACTIVE_LOW);
#endif

#ifdef EXISTS_INPUT_C2F
    int inputC2F_dataPins[C2F_INPUT_NO_PIN] = {C2F_INPUT_BIT_0_PIN, C2F_INPUT_BIT_1_PIN, C2F_INPUT_BIT_2_PIN, C2F_INPUT_BIT_3_PIN, C2F_INPUT_BIT_4_PIN};
    TeensyIn inputC2F(C2F_REQ, C2F_ACK, inputC2F_dataPins, C2F_INPUT_NO_PIN, C2F_DELAY, C2F_ACTIVE_LOW);
#endif

#ifdef EXISTS_OUTPUT_DECODER
    int outputDecoder_dataPins[DECODER_OUTPUT_NO_PIN] = {DECODER_OUTPUT_BIT_0_PIN, DECODER_OUTPUT_BIT_1_PIN, DECODER_OUTPUT_BIT_2_PIN, DECODER_OUTPUT_BIT_3_PIN,
                                DECODER_OUTPUT_BIT_4_PIN, DECODER_OUTPUT_BIT_5_PIN, DECODER_OUTPUT_BIT_6_PIN, DECODER_OUTPUT_BIT_7_PIN, DECODER_OUTPUT_BIT_8_PIN};
    TeensyOut outputDecoder(DECODER_REQ, DECODER_ACK, outputDecoder_dataPins, DECODER_OUTPUT_NO_PIN, DECODER_DELAY, DECODER_ACTIVE_LOW);
#endif

#ifdef EXISTS_DAC
    DAC dac{DAC_RESET, DAC_A0, DAC_A1};
#endif

#ifdef EXISTS_BIASGEN
    SPIConfig biasGen{BIASGEN_SCK_PIN , BIASGEN_RESET_PIN , BIASGEN_MOSI_PIN,  BIASGEN_ENABLE_PIN, 0};
#endif

#ifdef EXISTS_SPI1
    SPIConfig spi1{SPI1_SCK_PIN , SPI1_RESET_PIN , SPI1_MOSI_PIN, 1};
#endif

#ifdef EXISTS_SPI2
    SPIConfig spi2{SPI2_SCK_PIN ,SPI2_RESET_PIN , SPI2_MOSI_PIN, 2};
#endif


//---------------------------------------------------------------------------------------------------------------------------------------
// "setup" function: 
//---------------------------------------------------------------------------------------------------------------------------------------

void setup() 
{
    resetChip();        
    setupLFSR();

#ifdef EXISTS_BIASGEN
    biasGen.setupSPI();
    biasGen.resetSPI();
#endif

#ifdef EXISTS_SPI1
    spi1.setupSPI();
    spi1.resetSPI();
#endif

#ifdef EXISTS_SPI2
    spi2.setupSPI();
    spi2.resetSPI();
#endif

#ifdef EXISTS_INPUT_ENCODER
    attachInterrupt(digitalPinToInterrupt(ENCODER_REQ), aerInputEncoder_ISR, CHANGE);
#endif

#ifdef EXISTS_INPUT_C2F
    attachInterrupt(digitalPinToInterrupt(C2F_REQ), aerInputC2F_ISR, CHANGE);
#endif

#ifdef EXISTS_DAC
    dac.join_I2C_bus();
    dac.turnReferenceOff();
#endif

    Serial.print("********************* Setup complete *********************\n");
}


//---------------------------------------------------------------------------------------------------------------------------------------
// "main" function: reads and writes PC <-> Teensy serial communication
//---------------------------------------------------------------------------------------------------------------------------------------

void loop() 
{    
   if (usb_serial_available()) 
   {
        // Read the data in the input buffer
        usb_serial_read(&inputBuffer, sizeof(inputBuffer));
               
        // Interpret the input data
        switch ((PktType)inputBuffer.header) 
        {
            // Setup DAC
            case PktType::Pkt_setDACvoltage:
            { 
                DAC_command DAC(inputBuffer);
                dac.writeDAC(DAC.command_address, DAC.data); 

                delay(100);
                sendTeensyStatus(TeensyStatus::Success);

                Serial.print("DAC command received. Pin ");
                Serial.print(DAC.command_address);
                Serial.print(" set to ");
                Serial.print(DAC.data);
                Serial.print(" mV.");
                break;   
            }

            // Setup bias generator
            case PktType::Pkt_setBiasGen:
            {  
                BIASGEN_command biasGenCommand(inputBuffer);
                biasGen.writeSPI(biasGenCommand.biasNo, biasGenCommand.currentValue_binary);
                delay(100);
                sendTeensyStatus(TeensyStatus::Success);
                Serial.print("BIASGEN command received. Bias ");
                Serial.print(biasGenCommand.biasNo);
                Serial.print(" set to approx. ");
                Serial.print(biasGen.getBiasGenDecimal(biasGenCommand.currentValue_binary), 6);
                Serial.print(" uA.");
                break;
            }

            // Request decoder output
            case PktType::Pkt_reqOutputDecoder:
            {
                AER_DECODER_OUTPUT_command decoder(inputBuffer);
                outputDecoder.dataWrite(decoder.data);            

                Serial.print("AER command received. Binary value: ");
                Serial.print(decoder.data, BIN);
                break;
            }

            // Get encoder input value
            case PktType::Pkt_reqInputEncoder:
            {
                inputEncoder.sendEventBuffer();
                break;
            }

            // Get C2F input value
            case PktType::Pkt_reqInputC2F:
            {
                inputC2F.sendEventBuffer();
                break;
            }

            // Setup SPIs
            case PktType::Pkt_setSPI:
            {
                // Teensy 4.1 supports 3 SPIs. SPI0 is always for BiasGen. SPI1/2 are optional and user dependant.
                SPI_INPUT_command spiCommand(inputBuffer);

#ifdef EXISTS_SPI1
                if (spiCommand.spi_number == 1 )
                {
                    Serial.print("SPI command received. Spi number 1, address ");
                    Serial.print(spiCommand.address);
                    Serial.print("Value ");
                    Serial.print(spiCommand.value);
                    spi1.writeSPI(spiCommand.address, spiCommand.value);
                }
#endif

#ifdef EXISTS_SPI2
                if (spiCommand.spi_number == 2)
                {
                    spi2.writeSPI(spiCommand.address, spiCommand.value);
                    Serial.print("SPI command received: Spi number 2, address ");
                    Serial.print(spiCommand.address);
                    Serial.print("Value: ");
                    Serial.print(spiCommand.value);
                }
#endif

                delay(100);
                sendTeensyStatus(TeensyStatus::Success);
                break;
            }

            case PktType::Pkt_handshakeC2F:
            {
                inputC2F.handshake();
                break;
            }
                        
            case PktType::Pkt_handshakeEncoder:
            {
                inputEncoder.handshake();
                break;
            }

            default: 
            {
               // sendTeensyStatus(TeensyStatus::UnknownCommand);
                break;  
            }
        } 
   }
};


//---------------------------------------------------------------------------------------------------------------------------------------
// setupLFSR: Sets up Linear Feedback Shift Register
//---------------------------------------------------------------------------------------------------------------------------------------

static void setupLFSR()
{
  pinMode(LB_LFSR_CLK, OUTPUT);
  delay(100);
  digitalWrite(LB_LFSR_CLK, LOW);
  delay(100);
}

//---------------------------------------------------------------------------------------------------------------------------------------
// resetChip: Sets up and executes chip reset pattern
//---------------------------------------------------------------------------------------------------------------------------------------

static void resetChip()
{
  pinMode(P_RST_PIN, OUTPUT);
  pinMode(S_RST_PIN, OUTPUT);
  pinMode(SYN_RST_NMDA_PIN, OUTPUT);
  pinMode(SYN_RST_GABGA_PIN, OUTPUT);
  pinMode(LB_LFSR_RST, OUTPUT);
  delay(100);

  digitalWrite(P_RST_PIN, LOW);
  delay(100);
  digitalWrite(S_RST_PIN, LOW);
  delay(100);

  digitalWrite(P_RST_PIN, HIGH);
  delay(100);
  digitalWrite(S_RST_PIN, HIGH);
  delay(100);

  digitalWrite(SYN_RST_NMDA_PIN, HIGH);
  delay(100);
  digitalWrite(SYN_RST_GABGA_PIN, HIGH);
  delay(100);

  digitalWrite(SYN_RST_NMDA_PIN, LOW);
  delay(100);
  digitalWrite(SYN_RST_GABGA_PIN, LOW);
  delay(100);

  digitalWrite(LB_LFSR_RST, HIGH);
  delay(100);
  digitalWrite(LB_LFSR_RST, LOW);
  delay(1000);
}


//---------------------------------------------------------------------------------------------------------------------------------------
// AER Input Interrupt Servie Routine (ISR) for encoder input
//---------------------------------------------------------------------------------------------------------------------------------------

static void aerInputEncoder_ISR()
{
    if (!inputEncoder.reqRead()) 
    {        
        if(inputEncoder.getBufferIndex() < MAX_PKT_BODY_LEN)
        {
            inputEncoder.recordEvent();
        }

        inputEncoder.ackWrite(0);
    }

    else if (inputEncoder.reqRead())
    {
        inputEncoder.ackWrite(1);
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------
// AER Input Interrupt Servie Routine (ISR) for C2F input
//---------------------------------------------------------------------------------------------------------------------------------------

static void aerInputC2F_ISR()
{
    if (!inputC2F.reqRead()) 
    {        
        if(inputC2F.getBufferIndex() < MAX_PKT_BODY_LEN)
        {
            inputC2F.recordEvent();
        }
        inputC2F.ackWrite(0);
    }

    else if (inputC2F.reqRead())
    {      
        inputC2F.ackWrite(1);
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------
// sendTeensyStatus: Sends Teensy Status update to PC 
//---------------------------------------------------------------------------------------------------------------------------------------

static void sendTeensyStatus(TeensyStatus status)
{
    std::uint8_t teensyStatusBuffer[1] = {(uint8_t) status};
    // usb_serial_write(teensyStatusBuffer, sizeof(teensyStatusBuffer));
}

