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
#define EXISTS_SPAIC      

#define SERIAl_PORT_NAME                    "/dev/cu.usbmodem105661701" 


// BIASGEN 
#define BIASGEN_BIASFILE                "data/defaultBiasValues/SPAIC_BIASGEN_POR.csv"
#define BIASGEN_FILENAME_LOAD           "data/customBiasValues/BIASGEN/"

#ifdef EXISTS_BIASGEN
    #define BIASGEN_MOSI_PIN                11
    #define BIASGEN_SCK_PIN                 13
    #define BIASGEN_ENABLE_PIN              10
    #define BIASGEN_RESET_PIN               9
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
#endif


// SPI2
#ifdef EXISTS_SPI2
    #define SPI2_MOSI_PIN                   43
    #define SPI2_SCK_PIN                    45
    #define SPI2_RESET_PIN                  44
#endif


// ENCODER COMMUNICATION
#define ENCODER_INPUT_SAVE_FILENAME_CSV "../imgui_frontend/data/savedOutput/Encoder/SPAIC_outputEncoder.csv"

#ifdef EXISTS_ENCODER
    #define ENCODER_ACK                     29
    #define ENCODER_REQ                     28
    #define ENCODER_INPUT_NO_PIN            5  
    #define ENCODER_INPUT_BIT_0_PIN         4
    #define ENCODER_INPUT_BIT_1_PIN         5
    #define ENCODER_INPUT_BIT_2_PIN         6
    #define ENCODER_INPUT_BIT_3_PIN         7
    #define ENCODER_INPUT_BIT_4_PIN         8
    #define ENCODER_HANDSHAKE_ACTIVE_LOW    true
    #define ENCODER_DATA_ACTIVE_LOW         false
    #define ENCODER_DELAY                   10000
#endif

// RESET PINS
#define P_RST_PIN                           3 
#define S_RST_PIN                           2 

// DAC CONFIGURATION

#define DAC_BIASFILE                    "data/defaultBiasValues/SPAIC_DAC_POR.csv"
#define DAC_FILENAME_LOAD               "data/customBiasValues/DAC/"

#ifdef EXISTS_DAC
    #define DAC_RESET                       33    
    #define DAC_A1                          31    
    #define DAC_A0                          30    
#endif

#endif
