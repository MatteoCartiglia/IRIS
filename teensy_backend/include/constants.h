#ifndef CONSTANTS_H
#define CONSTANTS_H

// INTERFACE CONFIGURATION - UNCOMMENT AS REQUIRED
#define EXISTS_BIASGEN                   
//#define EXISTS_SPI1                      
//#define EXISTS_SPI2                     
#define EXISTS_ENCODER                  
//#define EXISTS_C2F                     
#define EXISTS_OUTPUT_DECODER                    
#define EXISTS_DAC      

#define EXISTS_ALIVE
//#define EXISTS_SPAIC
#define SERIAl_PORT_NAME                    "/dev/cu.usbmodem105661701" 


// BIASGEN 
#ifdef EXISTS_BIASGEN
    #define BIASGEN_MOSI_PIN                11
    #define BIASGEN_SCK_PIN                 13
    #define BIASGEN_ENABLE_PIN              10
    #define BIASGEN_RESET_PIN               41
    #define BIASGEN_BIASFILE                "data/defaultBiasValues/ALIVE_BIASGEN_POR.csv"
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

// C2F COMMUNICATION
    #define C2F_INPUT_RANGE                 32
    #define C2F_INPUT_SAVE_FILENAME_CSV     "../imgui_frontend/data/savedOutput/C2F/C2F.csv"
#ifdef EXISTS_C2F
    #define C2F_ACK                         0
    #define C2F_REQ                         35
    #define C2F_INPUT_NO_PIN                5
    #define C2F_INPUT_BIT_0_PIN             19
    #define C2F_INPUT_BIT_1_PIN             20
    #define C2F_INPUT_BIT_2_PIN             21
    #define C2F_INPUT_BIT_3_PIN             22
    #define C2F_INPUT_BIT_4_PIN             23
    #define C2F_INPUT_RANGE                 32
    #define C2F_INPUT_SAVE_FILENAME_CSV     "../imgui_frontend/data/savedOutput/C2F/C2F.csv"
    #define C2F_INPUT_SAVE_FILENAME         "../imgui_frontend/data/savedOutput/C2F/C2F"
    #define C2F_HANDSHAKE_ACTIVE_LOW        true
    #define C2F_DATA_ACTIVE_LOW             false
    #define C2F_DELAY                       1
#endif

// ENCODER COMMUNICATION
#ifdef EXISTS_ENCODER
    #define ENCODER_ACK                     30
    #define ENCODER_REQ                     29
    #define ENCODER_INPUT_NO_PIN            3  
    #define ENCODER_INPUT_BIT_0_PIN         26
    #define ENCODER_INPUT_BIT_1_PIN         27
    #define ENCODER_INPUT_BIT_2_PIN         28
    #define ENCODER_INPUT_SAVE_FILENAME_CSV "../imgui_frontend/data/savedOutput/Encoder/ALIVE_outputEncoder.csv"
    #define ENCODER_HANDSHAKE_ACTIVE_LOW    true
    #define ENCODER_DATA_ACTIVE_LOW         false
    #define ENCODER_DELAY                   2
#endif

// #if defined(EXISTS_INPUT_ENCODER) || defined(EXISTS_INPUT_C2F)
//     #define EVENT_BUFFER_SIZE               20
// #endif

// DECODER COMMUNICATION -- INPUT INTERFACE
#ifdef EXISTS_OUTPUT_DECODER
    #define DECODER_ACK                     34
    #define DECODER_REQ                     1
    #define DECODER_OUTPUT_NO_PIN           9  
    #define DECODER_OUTPUT_BIT_0_PIN        4
    #define DECODER_OUTPUT_BIT_1_PIN        5
    #define DECODER_OUTPUT_BIT_2_PIN        6
    #define DECODER_OUTPUT_BIT_3_PIN        7
    #define DECODER_OUTPUT_BIT_4_PIN        8
    #define DECODER_OUTPUT_BIT_5_PIN        9
    #define DECODER_OUTPUT_BIT_6_PIN        14
    #define DECODER_OUTPUT_BIT_7_PIN        15
    #define DECODER_OUTPUT_BIT_8_PIN        16

    #define DECODER_ACTIVE_LOW              false // ALIVE is ACTIVE HIGH
    #define DECODER_DELAY                   10
    #define INPUT_INTERFACE_FILENAME_LOAD   "data/IIValues/"
#endif

// RESET PINS
#define P_RST_PIN                           2 
#define S_RST_PIN                           3 
#define SYN_RST_GABGA_PIN                   17   
#define SYN_RST_NMDA_PIN                    18

// LEARNING BLOCK LFSR PINS 
#define LB_LFSR_RST                         31
#define LB_LFSR_CLK                         32

// DAC CONFIGURATION
#ifdef EXISTS_DAC
    #define DAC_RESET                       33    
    #define DAC_A1                          36    
    #define DAC_A0                          37    
    #define DAC_BINARY_RANGE                65536 
    #define DAC_REFERENCE                   2500
    #define DAC_CHANNELS_USED               11
    #define DAC_COMMAND_WRITE_UPDATE        3      // 0011 
    #define DAC_COMMAND_WRITE_SHIFT         4     
    #define DAC_MAX_VOLTAGE                 1800    // mV

    #define DAC_BIASFILE                    "data/defaultBiasValues/ALIVE_DAC_POR.csv"
    #define DAC_FILENAME_LOAD               "data/customBiasValues/DAC/"
#endif

#endif 