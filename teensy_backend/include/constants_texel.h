#ifndef CONSTANTS_TEXEL_H
#define CONSTANTS_TEXEL_H

// INTERFACE CONFIGURATION - UNCOMMENT AS REQUIRED

//#define EXISTS_BIASGEN
//#define EXISTS_ENCODER
//#define EXISTS_OUTPUT_DECODER


#define SERIAl_PORT_NAME "/dev/ttyACM0"


// RESET PINS

#define RESET_PIN   33
#define REGRST_PIN  41
#define SYNRST_PIN  40


// ENCODER
// Texel -> Teensy => AO bus via MCP23017 I²C bus
// AOREQ and AOACK directly connected to Teensy

#define ENCODER_INPUT_SAVE_FILENAME_CSV "../imgui_frontend/data/recordings/untitled.csv"

#define ENCODER_REQ  2
#define ENCODER_ACK  3
#define ENCODER_SCL 19
#define ENCODER_SDA 18


// DECODER
// Teensy -> Texel => AI bus

#define INPUT_INTERFACE_FILENAME_LOAD "data/IIValues/"

#define DECODER_REQ 4
#define DECODER_ACK 5
#define DECODER_OUTPUT_NUM_PINS 32
#define DECODER_OUTPUT_BIT_0_PIN     0
#define DECODER_OUTPUT_BIT_1_PIN     1
#define DECODER_OUTPUT_BIT_2_PIN    14
#define DECODER_OUTPUT_BIT_3_PIN    15
#define DECODER_OUTPUT_BIT_4_PIN    16
#define DECODER_OUTPUT_BIT_5_PIN    17
#define DECODER_OUTPUT_BIT_6_PIN    20
#define DECODER_OUTPUT_BIT_7_PIN    21
#define DECODER_OUTPUT_BIT_8_PIN    22
#define DECODER_OUTPUT_BIT_9_PIN    23
#define DECODER_OUTPUT_BIT_10_PIN   24
#define DECODER_OUTPUT_BIT_11_PIN   25
#define DECODER_OUTPUT_BIT_12_PIN   26
#define DECODER_OUTPUT_BIT_13_PIN   27
#define DECODER_OUTPUT_BIT_14_PIN    6
#define DECODER_OUTPUT_BIT_15_PIN    7
#define DECODER_OUTPUT_BIT_16_PIN    8
#define DECODER_OUTPUT_BIT_17_PIN    9
#define DECODER_OUTPUT_BIT_18_PIN   10
#define DECODER_OUTPUT_BIT_19_PIN   11
#define DECODER_OUTPUT_BIT_20_PIN   12
#define DECODER_OUTPUT_BIT_21_PIN   13
#define DECODER_OUTPUT_BIT_22_PIN   32
#define DECODER_OUTPUT_BIT_23_PIN   28
#define DECODER_OUTPUT_BIT_24_PIN   30
#define DECODER_OUTPUT_BIT_25_PIN   31
#define DECODER_OUTPUT_BIT_26_PIN   34
#define DECODER_OUTPUT_BIT_27_PIN   35
#define DECODER_OUTPUT_BIT_28_PIN   36
#define DECODER_OUTPUT_BIT_29_PIN   37
#define DECODER_OUTPUT_BIT_30_PIN   38
#define DECODER_OUTPUT_BIT_31_PIN   39
#define DECODER_ACTIVE_LOW false // TEXEL is ACTIVE HIGH ???
#define DECODER_DELAY 10 // ???


// There is one free pin, pin 29, that might be useful for debugging or
// something that is not connected (NC).

#define NC_PIN_29   29


// BIASGEN
#define BIASGEN_BIASFILE "data/defaultBiasValues/TEXEL_BIASGEN_POR.csv"
#define BIASGEN_FILENAME_LOAD "data/customBiasValues/BIASGEN/"

#endif
