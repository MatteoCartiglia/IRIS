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

// Fields in packets to and from chip

constexpr uint32_t TEXEL_AE_CORE_MASK       = 0x80000000;
constexpr uint32_t TEXEL_AE_REGSPK_MASK     = 0x40000000;
constexpr uint32_t TEXEL_AE_RW_MASK         = 0x20000000;
constexpr uint32_t TEXEL_AE_REGADDR_MASK    = 0x0000003F;
constexpr uint32_t TEXEL_AE_REGDATA_MASK    = 0x1FFFFFC0;
constexpr uint32_t TEXEL_AE_REGDATA_SHIFT   = 6;
constexpr uint32_t TEXEL_AEI_SPKUNUSED_MASK = 0x1FFFE000;
constexpr uint32_t TEXEL_AEI_SPKADDR_MASK   = 0x00001FFF;
constexpr uint32_t TEXEL_AEI_SPKADDRX_MASK  = 0x0000000F;
constexpr uint32_t TEXEL_AEI_SPKADDRY_MASK  = 0x00001FF0;
constexpr uint32_t TEXEL_AEI_SPKADDRY_SHIFT = 4;
constexpr uint32_t TEXEL_AEO_SPKUNUSED_MASK = 0x1FFFFF80;
constexpr uint32_t TEXEL_AEO_SPKADDR_MASK   = 0x0000007F;
constexpr uint32_t TEXEL_AEO_SPKADDRX_MASK  = 0x0000000F;
constexpr uint32_t TEXEL_AEO_SPKADDRY_MASK  = 0x00000070;
constexpr uint32_t TEXEL_AEO_SPKADDRY_SHIFT = 4;

constexpr uint32_t TEXEL_AE_CORE_1          = 0x00000000;
constexpr uint32_t TEXEL_AE_CORE_2          = 0x10000000;

constexpr uint32_t TEXEL_AE_TYPE_MASK       = 0x60000000;
constexpr uint32_t TEXEL_AE_REGWRITE        = 0x60000000;
constexpr uint32_t TEXEL_AE_REGREAD         = 0x40000000;
constexpr uint32_t TEXEL_AE_SPIKE           = 0x00000000;

// ENCODER
// Texel -> Teensy => AO bus via MCP23017 I²C bus
// AOREQ and AOACK directly connected to Teensy

#define ENCODER_INPUT_SAVE_FILENAME_CSV "../imgui_frontend/data/recordings/untitled.csv"

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
