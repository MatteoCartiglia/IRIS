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
#define SERIAL_BUFFER_SIZE_AER              44
#define SERIAL_ASCII_SPACE                  32

// CHIP <-> TEENSY COMMUNICATION
#define TEENSY_INPUT_C2F                    0
#define TEENSY_INPUT_ENCODER                1
#define TEENSY_INPUT_BUFFER_SIZE            1024
#define TEENSY_OUTPUT_HANDSHAKE_TIMEOUT     10          // microseconds

// TEENSY -> HOST 
#define  MAX_EVENTS_PER_PACKET              256

#endif 