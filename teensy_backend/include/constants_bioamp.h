#ifndef CONSTANTS_BIOAMP_H
#define CONSTANTS_BIOAMP_H

// INTERFACE CONFIGURATION - UNCOMMENT AS REQUIRED

#define EXISTS_BIASGEN
#define EXISTS_SPI1

#define EXISTS_ENCODER_1
#define EXISTS_ENCODER_2


#define SERIAl_PORT_NAME "/dev/ttyACM0"

// tty.usbmodem116666801 usbmodem105661701  usbmodem119616901
// tty.usbmodem116666801
// "/dev/cu.usbmodem130123101"

#ifdef EXISTS_BIASGEN
    #define BIASGEN_MOSI_PIN                11
    #define BIASGEN_SCK_PIN                 13
    #define BIASGEN_ENABLE_PIN              10
    #define BIASGEN_RESET_PIN               41
    // ENVIRONMENT CONDITIONS - UNCOMMENT TO ENABLE
    // #define BIASGEN_SEND_POR                1
    // #define BIASGEN_SET_TRANSISTOR_TYPE     1
#endif

// SPI1 
#ifdef EXISTS_SPI1
    #define SPI1_MOSI_PIN                   11
    #define SPI1_SCK_PIN                    13
    #define SPI1_ENABLE_PIN                 10
    #define SPI1_RESET_PIN                  41
#endif

// DECODER
// Teensy -> BioAMP => SPI -? AERC and BIASGEN

//???


// ENCODER
// BioAmp -> Teensy => Digital Pins

// ENCODER COMMUNICATION
#define ENCODER_INPUT_SAVE_FILENAME_CSV "../imgui_frontend/data/recordings/untitled.csv"

#ifdef EXISTS_ENCODER_1
    #define ENCODER_ACK 24
    #define ENCODER_REQ 25
    #define ENCODER_INPUT_NO_PIN 5
    #define ENCODER_INPUT_BIT_0_PIN 26
    #define ENCODER_INPUT_BIT_1_PIN 27
    #define ENCODER_INPUT_BIT_2_PIN 28
    #define ENCODER_INPUT_BIT_3_PIN 29
    #define ENCODER_INPUT_BIT_4_PIN 30
    #define ENCODER_HANDSHAKE_ACTIVE_LOW true
    #define ENCODER_DATA_ACTIVE_LOW false
    #define ENCODER_DELAY 50
#endif

#ifdef EXISTS_ENCODER_2
    #define ENCODER_ACK 6
    #define ENCODER_REQ 5
    #define ENCODER_INPUT_NO_PIN 5
    #define ENCODER_INPUT_BIT_0_PIN 4
    #define ENCODER_INPUT_BIT_1_PIN 3
    #define ENCODER_INPUT_BIT_2_PIN 2
    #define ENCODER_INPUT_BIT_3_PIN 1
    #define ENCODER_INPUT_BIT_4_PIN 0
#endif

// RESET PINS
#define P_RST_PIN 22
#define S_RST_PIN 23

// DELAY CONTROL
#define DLY_CTL_0 20
#define DLY_CTL_1 21

//Modify this for Bioamp
// BIASGEN
#define BIASGEN_BIASFILE "../imgui_frontend/data/defaultBiasValues/BIOAMP_BIASGEN_POR.csv"
#define BIASGEN_FILENAME_LOAD "../imgui_frontend/data/customBiasValues/BIASGEN/"

#endif

// // DECODER COMMUNICATION -- INPUT INTERFACE
// #define INPUT_INTERFACE_FILENAME_LOAD "data/IIValues/"
// #ifdef EXISTS_OUTPUT_DECODER
// #define DECODER_ACK 34
// #define DECODER_REQ 1
// #define DECODER_OUTPUT_NO_PIN 9
// #define DECODER_OUTPUT_BIT_0_PIN 4
// #define DECODER_OUTPUT_BIT_1_PIN 5
// #define DECODER_OUTPUT_BIT_2_PIN 6
// #define DECODER_OUTPUT_BIT_3_PIN 7
// #define DECODER_OUTPUT_BIT_4_PIN 8
// #define DECODER_OUTPUT_BIT_5_PIN 9
// #define DECODER_OUTPUT_BIT_6_PIN 14
// #define DECODER_OUTPUT_BIT_7_PIN 15
// #define DECODER_OUTPUT_BIT_8_PIN 16
// #define DECODER_ACTIVE_LOW false // ALIVE is ACTIVE HIGH
// #define DECODER_DELAY 10
// #endif