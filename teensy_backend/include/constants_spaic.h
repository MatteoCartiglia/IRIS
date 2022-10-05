#ifndef CONSTANTS_H
#define CONSTANTS_H

// INTERFACE CONFIGURATION - UNCOMMENT AS REQUIRED
#define EXISTS_BIASGEN                   
#define EXISTS_SPI1                      
#define EXISTS_SPI2                     
#define EXISTS_ENCODER                  
//#define EXISTS_C2F                     
//#define EXISTS_OUTPUT_DECODER                    
#define EXISTS_DAC       
//#define EXISTS_ALIVE      
#define EXISTS_SPAIC      

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
#define SERIAl_PORT_NAME                    "/dev/cu.usbmodem105668801"  //SPAICs Number
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


// BIASGEN 
#ifdef EXISTS_BIASGEN
    #define BIASGEN_MOSI_PIN                11
    #define BIASGEN_SCK_PIN                 13
    #define BIASGEN_ENABLE_PIN              10
    #define BIASGEN_RESET_PIN               9
    
    #define BIASGEN_BIASFILE                "data/defaultBiasValues/SPAIC_BIASGEN_POR.csv"
    #define BIASGEN_FILENAME_LOAD           "data/customBiasValues/BIASGEN/"

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
    #define BIASGEN_COURSE_SHIFT            9
    #define BIASGEN_FINE_SHIFT              1
    #define BIASGEN_PACKET_SIZE             12

    // ENVIRONMENT CONDITIONS - UNCOMMENT TO ENABLE
    // #define BIASGEN_SEND_POR                1
    // #define BIASGEN_SET_TRANSISTOR_TYPE     1
#endif

// SPI1 
#ifdef EXISTS_SPI1
    #define SPI1_MOSI_PIN                   11
    #define SPI1_SCK_PIN                    13
    #define SPI1_ENABLE_PIN                 10
    #define SPI1_RESET_PIN                  9
    #define SPI1_RESOLUTION                 255
#endif


// SPI2
#ifdef EXISTS_SPI2
    #define SPI2_MOSI_PIN                   43
    #define SPI2_SCK_PIN                    45
    #define SPI2_RESET_PIN                  44
    #define SPI2_RESOLUTION                 255
#endif


// ENCODER COMMUNICATION
#ifdef EXISTS_ENCODER
    #define ENCODER_ACK                     29
    #define ENCODER_REQ                     28
    #define ENCODER_INPUT_NO_PIN            5  
    #define ENCODER_INPUT_BIT_0_PIN         4
    #define ENCODER_INPUT_BIT_1_PIN         5
    #define ENCODER_INPUT_BIT_2_PIN         6
    #define ENCODER_INPUT_BIT_3_PIN         7
    #define ENCODER_INPUT_BIT_4_PIN         8
    #define ENCODER_INPUT_SAVE_FILENAME_CSV "../imgui_frontend/data/savedOutput/Encoder/SPAIC_outputEncoder.csv"
    #define ENCODER_HANDSHAKE_ACTIVE_LOW    true
    #define ENCODER_DATA_ACTIVE_LOW         false
    #define ENCODER_DELAY                   10000
#endif

// #if defined(EXISTS_INPUT_ENCODER) || defined(EXISTS_INPUT_C2F)
//     #define EVENT_BUFFER_SIZE               20
// #endif

// RESET PINS
#define P_RST_PIN                           3 
#define S_RST_PIN                           2 


// DAC CONFIGURATION
#ifdef EXISTS_DAC
    #define DAC_RESET                       33    
    #define DAC_A1                          31    
    #define DAC_A0                          30    
    #define DAC_BINARY_RANGE                65536 
    #define DAC_REFERENCE                   2500
    #define DAC_CHANNELS_USED               6
    #define DAC_COMMAND_WRITE_UPDATE        3      // 0011 
    #define DAC_COMMAND_WRITE_SHIFT         4     
    #define DAC_MAX_VOLTAGE                 1800    // mV

    #define DAC_BIASFILE                    "data/defaultBiasValues/SPAIC_DAC_POR.csv"
    #define DAC_FILENAME_LOAD               "data/customBiasValues/DAC/"

#endif
#endif 