//---------------------------------------------------------------------------------------------------------------------------------------
// main.cpp file containing main function and ISR interrupt functions
//---------------------------------------------------------------------------------------------------------------------------------------

#include <Arduino.h>
#include <iostream>
#include <vector>

#include "AER_in.h"
#include "constants_global.h"
#include "constants.h"
#include "dac.h"
#include "datatypes.h"
#include "spiConfig.h"
#include "teensyOut.h"
#ifdef TARGET_TEXEL
#include "texel.h"
#endif

// Forward declarations of function prototypes

#ifdef TARGET_ALIVE
static void setupLFSR();
#endif

static void resetChip(uint8_t parameter = ResetTypeDefault);

#ifdef EXISTS_ENCODER
static void aerInputEncoder_ISR();
#endif

#ifdef EXISTS_C2F
static void aerInputC2F_ISR();
#endif

static void sendTeensyStatus(TeensyStatus status);

// Global variables

static Pkt inputBuffer;

#ifdef TARGET_TEXEL
static Texel texel;
static unsigned int readTexelAObus(void *texel) { return static_cast<Texel *>(texel)->readAObus(); }
#endif

#ifdef EXISTS_ENCODER

#ifdef TARGET_SPAIC
int inputEncoder_dataPins[ENCODER_INPUT_NUM_PINS] = {ENCODER_INPUT_BIT_0_PIN, ENCODER_INPUT_BIT_1_PIN, ENCODER_INPUT_BIT_2_PIN, ENCODER_INPUT_BIT_3_PIN};
#endif

#ifdef TARGET_ALIVE
int inputEncoder_dataPins[ENCODER_INPUT_NUM_PINS] = {ENCODER_INPUT_BIT_0_PIN, ENCODER_INPUT_BIT_1_PIN, ENCODER_INPUT_BIT_2_PIN};
#endif

#ifdef TARGET_TEXEL
AER_in inputEncoder(ENCODER_REQ, ENCODER_ACK, readTexelAObus, &texel);
#else
AER_in inputEncoder(ENCODER_REQ, ENCODER_ACK, inputEncoder_dataPins, ENCODER_INPUT_NUM_PINS, ENCODER_DELAY, ENCODER_HANDSHAKE_ACTIVE_LOW, ENCODER_DATA_ACTIVE_LOW);
#endif

int enc_since_blank_milli = 0;
bool enc_aero_flag = true;

#endif

#ifdef EXISTS_C2F
int inputC2F_dataPins[C2F_INPUT_NUM_PINS] = {C2F_INPUT_BIT_0_PIN, C2F_INPUT_BIT_1_PIN, C2F_INPUT_BIT_2_PIN, C2F_INPUT_BIT_3_PIN, C2F_INPUT_BIT_4_PIN};
AER_in inputC2F(C2F_REQ, C2F_ACK, inputC2F_dataPins, C2F_INPUT_NUM_PINS, C2F_DELAY, C2F_HANDSHAKE_ACTIVE_LOW, C2F_DATA_ACTIVE_LOW);
int c2f_since_blank_milli = 0;
bool c2f_aero_flag = true;
#endif

#if defined(EXISTS_OUTPUT_DECODER) && !defined(TARGET_TEXEL)
int outputDecoder_dataPins[DECODER_OUTPUT_NUM_PINS] = {DECODER_OUTPUT_BIT_0_PIN, DECODER_OUTPUT_BIT_1_PIN, DECODER_OUTPUT_BIT_2_PIN, DECODER_OUTPUT_BIT_3_PIN,
                                                     DECODER_OUTPUT_BIT_4_PIN, DECODER_OUTPUT_BIT_5_PIN, DECODER_OUTPUT_BIT_6_PIN, DECODER_OUTPUT_BIT_7_PIN, DECODER_OUTPUT_BIT_8_PIN};
TeensyOut outputDecoder(DECODER_REQ, DECODER_ACK, outputDecoder_dataPins, DECODER_OUTPUT_NUM_PINS, DECODER_DELAY, DECODER_ACTIVE_LOW);
#endif

#ifdef EXISTS_DAC
DAC dac{DAC_RESET, DAC_A0, DAC_A1};
#endif

#if defined(EXISTS_BIASGEN) && !defined(TARGET_TEXEL)
SPIConfig biasGen{BIASGEN_SCK_PIN, BIASGEN_RESET_PIN, BIASGEN_MOSI_PIN, BIASGEN_ENABLE_PIN, 0};
#endif

#ifdef EXISTS_SPI1
SPIConfig spi1{SPI1_SCK_PIN, SPI1_RESET_PIN, SPI1_MOSI_PIN, 1};
#endif

#ifdef EXISTS_SPI2
SPIConfig spi2{SPI2_SCK_PIN, SPI2_RESET_PIN, SPI2_MOSI_PIN, 2};
#endif

//---------------------------------------------------------------------------------------------------------------------------------------
// "setup" function:
//---------------------------------------------------------------------------------------------------------------------------------------

void setup()
{
#ifdef TARGET_ALIVE
    setupLFSR();
#endif

#ifdef TARGET_TEXEL
    texel.setup();
#endif

#ifdef EXISTS_SPI1
    spi1.setupSPI();
#endif

#ifdef EXISTS_SPI2
    spi2.setupSPI();
#endif

#ifdef EXISTS_ENCODER
    attachInterrupt(digitalPinToInterrupt(ENCODER_REQ), aerInputEncoder_ISR, CHANGE);
#endif

#ifdef EXISTS_C2F
    attachInterrupt(digitalPinToInterrupt(C2F_REQ), aerInputC2F_ISR, CHANGE);
#endif

#ifdef EXISTS_DAC
    dac.join_I2C_bus();
    dac.turnReferenceOff();
#endif

    delay(5000);

#if defined(EXISTS_BIASGEN) && !defined(TARGET_TEXEL)
    biasGen.setupBG();
#endif

    resetChip();

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
            
#ifdef EXISTS_DAC
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
#endif

#ifdef EXISTS_BIASGEN
        // Setup bias generator
        case PktType::Pkt_setBiasGen:
        {
            BIASGEN_command biasGenCommand(inputBuffer);
#ifdef TARGET_TEXEL
            texel.setBias(biasGenCommand.biasNo, biasGenCommand.currentValue_binary);
#else
            biasGen.writeSPI(biasGenCommand.biasNo, biasGenCommand.currentValue_binary);
#endif
            delay(100);
            sendTeensyStatus(TeensyStatus::Success);
            break;
        }
#endif

#ifdef EXISTS_OUTPUT_DECODER
        // Request decoder output
        case PktType::Pkt_reqOutputDecoder:
        {
            AER_DECODER_OUTPUT_command decoder(inputBuffer);
#ifdef TARGET_TEXEL
            texel.write(decoder.data);
#else
            outputDecoder.dataWrite(decoder.data);
#endif
            //Serial.print("AER command received. Binary value: ");
            //Serial.print(decoder.data, BIN);
            break;
        }
#endif

#ifdef EXISTS_ENCODER
        // Get encoder input value
        case PktType::P2tRequestAerEncoderOutput:
        {
            // Serial.println(inputEncoder.saving_flag);

            // Serial.println("Toggling saving encoder /n");
            inputEncoder.setBufferIndex(0);
            inputEncoder.set_t0(micros());
            inputEncoder.toggle_saving_flag();
            // Serial.println(inputEncoder.saving_flag);

            /*
            delay(100);
            AER_out tmp;
            tmp.address = 10;
            tmp.ts_1ms = 0b0000000011111111;
            P2TPkt aero_pkt(tmp);
           // Serial.print(aero_pkt.header);
            usb_serial_write((const void*) &aero_pkt, sizeof(aero_pkt));
            */
            break;
        }
#endif

#ifdef EXISTS_ENCODER
        case PktType::PktGetAerEncoderOutput:
        {
            //  Serial.println("Get number of events");
            // Serial.println(inputEncoder.getBufferIndex(), DEC);

            if (inputEncoder.saving_flag)
            {
                // Serial.println("");
                // Serial.print("Ev: ");
                // Serial.println(inputEncoder.getBufferIndex(), DEC);

                inputEncoder.sendEventBuffer();
            }
            break;
        }
        case PktType::Pkt_handshakeEncoder:
        {
            inputEncoder.handshake();
            break;
        }
#endif

#ifdef EXISTS_C2F
        // Get C2F input value
        case PktType::P2tRequestAerC2FOutput:
        {
            c2f_aero_flag = true;

            break;
        }

        case PktType::Pkt_handshakeC2F:
        {
            inputC2F.handshake();
            break;
        }
#endif

        // Setup SPIs
        case PktType::Pkt_setSPI:
        {
            // Teensy 4.1 supports 3 SPIs. SPI0 is always for BiasGen. SPI1/2 are optional and user dependant.
            SPI_INPUT_command spiCommand(inputBuffer);

#ifdef EXISTS_SPI1
            if (spiCommand.spi_number == 1)
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

        case PktType::PktResetChip:
        {
            RESET_command resetCommand(inputBuffer);
            resetChip(resetCommand.parameter);
        }

        default:
        {

            break;
        }
        }
    }
};

#ifdef TARGET_ALIVE
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
#endif

//---------------------------------------------------------------------------------------------------------------------------------------
// resetChip: Sets up and executes chip reset pattern
//---------------------------------------------------------------------------------------------------------------------------------------

static void resetChip(uint8_t parameter)
{
#ifdef TARGET_TEXEL
    texel.reset(parameter);
    return;
#endif

#ifdef EXISTS_PSRESET
    pinMode(P_RST_PIN, OUTPUT);
    pinMode(S_RST_PIN, OUTPUT);

    digitalWrite(P_RST_PIN, LOW);
    delay(100);
    digitalWrite(S_RST_PIN, LOW);
    delay(100);

    digitalWrite(P_RST_PIN, HIGH);
    delay(100);
    digitalWrite(S_RST_PIN, HIGH);
    delay(100);
#endif

#ifdef TARGET_ALIVE
    pinMode(SYN_RST_NMDA_PIN, OUTPUT);
    pinMode(SYN_RST_GABGA_PIN, OUTPUT);
    pinMode(LB_LFSR_RST, OUTPUT);
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
#endif
}

#ifdef EXISTS_ENCODER
//---------------------------------------------------------------------------------------------------------------------------------------
// AER Input Interrupt Servie Routine (ISR) for encoder input
//---------------------------------------------------------------------------------------------------------------------------------------
void aerInputEncoder_ISR()
{
    if (inputEncoder.reqRead())
    {
        if (inputEncoder.saving_flag && (inputEncoder.getBufferIndex() < int(MAX_EVENTS_PER_PACKET)))
        {
            inputEncoder.recordEvent();
        }
        inputEncoder.ackWrite(1);
    }

    else if (!inputEncoder.reqRead())
    {

        inputEncoder.ackWrite(0);
    }
}
#endif

#ifdef EXISTS_C2F
//---------------------------------------------------------------------------------------------------------------------------------------
// AER Input Interrupt Servie Routine (ISR) for C2F input
//---------------------------------------------------------------------------------------------------------------------------------------
static void aerInputC2F_ISR()
{
    if (!inputC2F.reqRead())
    {
        if (inputC2F.getBufferIndex() < int(MAX_PKT_BODY_LEN))
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
#endif

//---------------------------------------------------------------------------------------------------------------------------------------
// sendTeensyStatus: Sends Teensy Status update to PC
//---------------------------------------------------------------------------------------------------------------------------------------

static void sendTeensyStatus(TeensyStatus status)
{
    // std::uint8_t teensyStatusBuffer[1] = {(uint8_t) status};
    // usb_serial_write(teensyStatusBuffer, sizeof(teensyStatusBuffer));
}
