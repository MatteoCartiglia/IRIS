#ifndef CONSTANTS_TEXEL_H
#define CONSTANTS_TEXEL_H

//#define EXISTS_BIASGEN

#define EXISTS_ENCODER
#define EXISTS_OUTPUT_DECODER

#define SERIAl_PORT_NAME "/dev/ttyACM0"


// RESET PINS - all active low

#define RESET_PIN   33
#define REGRST_PIN  41
#define SYNRST_PIN  40

// Each of the resets should be held low for 100µs to be effective.

#define RESET_DELAY_US  100


// ENCODER
// Texel -> Teensy => AO bus via MCP23017 I²C bus
// AOREQ and AOACK directly connected to Teensy

#define ENCODER_INPUT_SAVE_FILENAME_CSV "./untitled.csv"

#define ENCODER_REQ  2
#define ENCODER_ACK  3
#define ENCODER_SCL 19
#define ENCODER_SDA 18
#define U8_I2C_ADDRESS 0b000
#define U9_I2C_ADDRESS 0b001

// DECODER
// Teensy -> Texel => AI bus
// Data, req and ack are active high.

#define INPUT_INTERFACE_FILENAME_LOAD "data/IIValues/"

#define DECODER_REQ 4
#define DECODER_ACK 5

constexpr int AI_bus_pins[] = {0, 1, 14, 15, 16, 17, 20, 21, 22, 23, 24, 25, 26, 27,  6,  7,
                               8, 9, 10, 11, 12, 13, 32, 28, 30, 31, 34, 35, 36, 37, 38, 39};
                               


// There is one free pin, pin 29, that might be useful for debugging or
// something that is not connected (NC).

#define NC_PIN_29   29


// BIASGEN
#define BIASGEN_BIASFILE "data/defaultBiasValues/TEXEL_BIASGEN_POR.csv"
#define BIASGEN_FILENAME_LOAD "data/customBiasValues/BIASGEN/"

#endif
