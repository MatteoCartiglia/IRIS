#ifndef CONSTANTS_BIOAMP_H
#define CONSTANTS_BIOAMP_H

#define SERIAl_PORT_NAME "/dev/ttyACM0"

// DECODER
// Teensy -> BioAMP => SPI -? AERC and BIASGEN

#define BIASGEN_MOSI_PIN                11
#define BIASGEN_SCK_PIN                 13
#define BIASGEN_ENABLE_PIN              10
#define BIASGEN_RESET_PIN               41


#define SPI1_MOSI_PIN                   11
#define SPI1_SCK_PIN                    13
#define SPI1_ENABLE_PIN                 10
#define SPI1_RESET_PIN                  41

// ENCODER
// BioAmp -> Teensy => Digital Pins

// ENCODER COMMUNICATION
#define ENCODER_INPUT_SAVE_FILENAME_CSV "../imgui_frontend/data/recordings/untitled.csv"

#define ENCODER1_ACK 24
#define ENCODER1_REQ 25
#define ENCODER1_INPUT_NO_PIN 5

//Not sue if these are relevant or not in current context
#define ENCODER1_HANDSHAKE_ACTIVE_LOW true
#define ENCODER1_DATA_ACTIVE_LOW false
#define ENCODER1_DELAY 50

constexpr int ENCODER1_bus_pins[ENCODER1_INPUT_NO_PIN] = {26, 27, 28, 29, 30};

#define ENCODER2_ACK 6
#define ENCODER2_REQ 5
#define ENCODER2_INPUT_NO_PIN 5

//Not sue if these are relevant or not in current context
#define ENCODER2_HANDSHAKE_ACTIVE_LOW true
#define ENCODER2_DATA_ACTIVE_LOW false
#define ENCODER2_DELAY 50

constexpr int ENCODER2_bus_pins[ENCODER2_INPUT_NO_PIN] = {4, 3, 2, 1, 0};


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

  

// RESET PINS
#define P_RST_PIN 22
#define S_RST_PIN 23

// DELAY Control Bits
constexpr int DLY_CTL_pins[] ={20,21};

// BIASGEN
#define BIASGEN_BIASFILE "../imgui_frontend/data/defaultBiasValues/BIOAMP_BIASGEN_POR.csv"
#define BIASGEN_FILENAME_LOAD "../imgui_frontend/data/customBiasValues/BIASGEN/"

#endif
