#ifndef CONSTANTS_GLOBAL_H
#define CONSTANTS_GLOBAL_H

// GUI CONSTANTS
#define BUTTON_HEIGHT                       20
#define BUTTON_UPDATE_WIDTH                 100
#define BUTTON_TOGGLE_WIDTH                 50
#define WINDOW_HEIGHT                       750
#define WINDOW_WIDTH                        1670
#define CLEAR_COLOUR_X                      0.45f
#define CLEAR_COLOUR_Y                      0.55f
#define CLEAR_COLOUR_Z                      0.60f
#define CLEAR_COLOUR_W                      1.00f

// UNIVERSAL CONSTANTS
#define BINARY_8_BIT_SHIFT                  8
#define BINARY_255                          0xFF

// PC <-> TEENSY SERIAL COMMUNICATION
#define SERIAL_COMMS_SHIFT                  8
#define SERIAL_COMMS_MAX_PKT_LEN            20           // Clarify
#define SERIAL_BUFFER_SIZE_BIAS             62
#define SERIAL_BUFFER_SIZE_DAC              44
#define SERIAL_BUFFER_SIZE_PORT_OPEN        34
#define SERIAL_BUFFER_SIZE_PORT_ERROR       41
#define SERIAL_BUFFER_SIZE_AER              44

// CHIP <-> TEENSY COMMUNICATION
#define TEENSY_INPUT_C2F                    0
#define TEENSY_INPUT_ENCODER                1
#define TEENSY_INPUT_BUFFER_SIZE            1024
#define TEENSY_OUTPUT_HANDSHAKE_TIMEOUT     10          // microseconds

// TEENSY -> HOST 
#define  MAX_EVENTS_PER_PACKET              256

// General BiasGen configs
#define BIASGEN_SCALING_FACTOR          256
#define BIASGEN_COURSE_SHIFT            9
#define BIASGEN_FINE_SHIFT              1
#define BIASGEN_PACKET_SIZE             12
#define BIASGEN_CHANNELS                54
#define BIASGEN_CATEGORIES              9

#define BIASGEN_MAX_CURRENT             1.9         // uA
#define BIASGEN_NO_MASTER_CURRENTS      6
#define BIASGEN_MASTER_CURRENT_0        0.00006     // uA
#define BIASGEN_MASTER_CURRENT_1        0.00046     // uA
#define BIASGEN_MASTER_CURRENT_2        0.00380     // uA
#define BIASGEN_MASTER_CURRENT_3        0.03000     // uA
#define BIASGEN_MASTER_CURRENT_4        0.24000     // uA
#define BIASGEN_MASTER_CURRENT_5        1.90000     // uA
#define BIASGEN_SCALING_FACTOR          256

// General DAC configs
#define DAC_BINARY_RANGE                65536 
#define DAC_REFERENCE                   2500
#define DAC_CHANNELS_USED               11
#define DAC_COMMAND_WRITE_UPDATE        3      // 0011 
#define DAC_COMMAND_WRITE_SHIFT         4     
#define DAC_MAX_VOLTAGE                 1800    // mV
#define DAC_ADDRESS_I2C                 12


// General SPI configs
#define SPI2_RESOLUTION                 255
#define SPI1_RESOLUTION                 255

#endif 