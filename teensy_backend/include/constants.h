//---------------------------------------------------------------------------------------------------------------------------------------
//
//
// Author: Ciara Giles-Doran <gciara@student.ethz.ch>
// Last updated: 24 JUN 2022
//---------------------------------------------------------------------------------------------------------------------------------------

#ifndef CONSTANTS_H
#define CONSTANTS_H

// ENVIRONMENT CONDITIONS - UNCOMMENT TO ENABLE
// #define BIASGEN_SEND_POR                1
// #define BIASGEN_SET_TRANSISTOR_TYPE     1

// GUI CONSTANTS
#define BUTTON_HEIGHT                   20
#define BUTTON_UPDATE_WIDTH             100
#define BUTTON_AER_WIDTH                285
#define WINDOW_HEIGHT                   655
#define WINDOW_WIDTH                    1670
#define CLEAR_COLOUR_X                  0.45f
#define CLEAR_COLOUR_Y                  0.55f
#define CLEAR_COLOUR_Z                  0.60f
#define CLEAR_COLOUR_W                  1.00f

// UNIVERSAL CONSTANTS
#define BINARY_8_BIT_SHIFT              8
#define BINARY_255                      0xFF

// PC <-> TEENSY SERIAL COMMUNICATION
#define SERIAL_COMMS_SHIFT              8
#define SERIAL_COMMS_MAX_PKT_LEN        6           // Clarify
#define SERIAl_PORT_NAME                "/dev/ttyACM0"
#define SERIAL_BUFFER_SIZE_BIAS         62
#define SERIAL_BUFFER_SIZE_DAC          44
#define SERIAL_BUFFER_SIZE_PORT_OPEN    34
#define SERIAL_BUFFER_SIZE_AER          44
#define SERIAL_ASCII_SPACE              32

// ALIVE <-> TEENSY COMMUNICATION
#define TEENSY_INPUT_C2F                0
#define TEENSY_INPUT_ENCODER            1
#define TEENSY_INPUT_BUFFER_SIZE        1024
#define TEENSY_OUTPUT_HANDSHAKE_TIMEOUT 10          // microseconds

// BIASGEN 
#define BIASGEN_SLAVE_SPI0              9
#define BIASGEN_MOSI_PIN                11
#define BIASGEN_SCK_PIN                 13
#define BIASGEN_ENABLE_PIN              10
#define BIASGEN_RESET_PIN               41

#define BIASGEN_BIASFILE                "data/BiasValues_BIASGEN.csv"
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

// ALIVE
#define ALIVE_NO_CORES                  2
#define ALIVE_NO_SYNAPSE_TYPES          4
#define ALIVE_NO_NEURONS                4
#define ALIVE_NO_NMDA_SYNAPSES          39          // 40 in total (including value 0)
#define ALIVE_NO_GABAa_SYNAPSES         1           // 2 in total (including value 0)
#define ALIVE_NO_GABAb_SYNAPSES         15          // 16 in total (including value 0)
#define ALIVE_NO_AMPA_SYNAPSES          5           // 6 in total (including value 0)
#define ALIVE_NO_AMPA_SYNAPSES_NN       2           // 3 AMPA+ and 3 AMPA- (including value 0)

#define ALIVE_CORE_SHIFT                8
#define ALIVE_SYNAPSE_TYPE_SHIFT        6
#define ALIVE_NEURON_SHIFT              4
#define ALIVE_NN_GABAb_BITS_4_5         1
#define ALIVE_GABAa_BITS_1_2_3          7
#define ALIVE_AMPA_SHIFT                3
#define ALIVE_PACKET_SIZE               9

// C2F COMMUNICATION
#define C2F_ACK                         0
#define C2F_REQ                         35
#define C2F_INPUT_NO_PIN                5
#define C2F_INPUT_BIT_0_PIN             19
#define C2F_INPUT_BIT_1_PIN             20
#define C2F_INPUT_BIT_2_PIN             21
#define C2F_INPUT_BIT_3_PIN             22
#define C2F_INPUT_BIT_4_PIN             23
#define C2F_INPUT_RANGE                 32

// ENCODER COMMUNICATION
#define ENCODER_ACK                     30
#define ENCODER_REQ                     29
#define ENCODER_INPUT_NO_PIN            3  
#define ENCODER_INPUT_BIT_0_PIN         26
#define ENCODER_INPUT_BIT_1_PIN         27
#define ENCODER_INPUT_BIT_2_PIN         28

// DECODER COMMUNICATION
#define DECODER_ACK                     34
#define DECODER_REQ                     1
#define DECODER_OUTPUT_NO_PIN           9  
#define DECODER_OUTPUT_BIT_0_PIN        18
#define DECODER_OUTPUT_BIT_1_PIN        19
#define DECODER_OUTPUT_BIT_2_PIN        20
#define DECODER_OUTPUT_BIT_3_PIN        21
#define DECODER_OUTPUT_BIT_4_PIN        22
#define DECODER_OUTPUT_BIT_5_PIN        23
#define DECODER_OUTPUT_BIT_6_PIN        24
#define DECODER_OUTPUT_BIT_7_PIN        25
#define DECODER_OUTPUT_BIT_8_PIN        26

// RESET PINS
#define P_RST_PIN                       2 
#define S_RST_PIN                       3 
#define SYN_RST_GABGA_PIN               17   
#define SYN_RST_NMDA_PIN                18

// LEARNING BLOCK LFSR PINS 
#define LB_LFSR_RST                     31
#define LB_LFSR_CLK                     32

// DAC CONFIGURATION
#define DAC_RESET                       33    
#define DAC_A1                          36    
#define DAC_A0                          37    
#define DAC_BINARY_RANGE                65536
#define DAC_REFERENCE                   2500

#define DAC_BIASFILE                    "data/BiasValues_DAC.csv"
#define DAC_CHANNELS_USED               11
#define DAC_COMMAND_WRITE_UPDATE        48      // 0011 0000
#define DAC_MAX_VOLTAGE                 1800    // mV

#endif 