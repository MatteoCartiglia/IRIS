#ifndef CONSTANTS_BIOAMP_H
#define CONSTANTS_BIOAMP_H

#define SERIAl_PORT_NAME "/dev/ttyACM0"

#define BIASGEN_MOSI_PIN                11
#define BIASGEN_SCK_PIN                 13
#define BIASGEN_ENABLE_PIN              10
#define BIASGEN_RESET_PIN               41


#define SPI1_MOSI_PIN                   11
#define SPI1_SCK_PIN                    13
#define SPI1_ENABLE_PIN                 10
#define SPI1_RESET_PIN                  41


// DECODER
// Teensy -> BioAMP => SPI -? AERC and BIASGEN

//???


// ENCODER
// BioAmp -> Teensy => Digital Pins

// ENCODER COMMUNICATION
#define ENCODER_INPUT_SAVE_FILENAME_CSV "../imgui_frontend/data/recordings/untitled.csv"

// #ifdef EXISTS_ENCODER_1
//     #define ENCODER_ACK 24
//     #define ENCODER_REQ 25
//     #define ENCODER_INPUT_NO_PIN 5
//     #define ENCODER_INPUT_BIT_0_PIN 26
//     #define ENCODER_INPUT_BIT_1_PIN 27
//     #define ENCODER_INPUT_BIT_2_PIN 28
//     #define ENCODER_INPUT_BIT_3_PIN 29
//     #define ENCODER_INPUT_BIT_4_PIN 30
//     #define ENCODER_HANDSHAKE_ACTIVE_LOW true
//     #define ENCODER_DATA_ACTIVE_LOW false
//     #define ENCODER_DELAY 50
// #endif

// #ifdef EXISTS_ENCODER_2
//     #define ENCODER_ACK 6
//     #define ENCODER_REQ 5
//     #define ENCODER_INPUT_NO_PIN 5
//     #define ENCODER_INPUT_BIT_0_PIN 4
//     #define ENCODER_INPUT_BIT_1_PIN 3
//     #define ENCODER_INPUT_BIT_2_PIN 2
//     #define ENCODER_INPUT_BIT_3_PIN 1
//     #define ENCODER_INPUT_BIT_4_PIN 0
// #endif

constexpr int ENCODER_1_pins[] = {24, 25, 5, 26, 27, 28, 29, 30, true, false, 50};
constexpr int ENCODER_2_pins[] = {6, 5, 5, 4, 3, 2, 1, 0};
  

// RESET PINS
#define P_RST_PIN 22
#define S_RST_PIN 23

// DELAY Control Bits
constexpr int DLY_CTL_pins[] ={20,21};

// BIASGEN
#define BIASGEN_BIASFILE "../imgui_frontend/data/defaultBiasValues/BIOAMP_BIASGEN_POR.csv"
#define BIASGEN_FILENAME_LOAD "../imgui_frontend/data/customBiasValues/BIASGEN/"

#endif
