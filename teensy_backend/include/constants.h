//---------------------------------------------------------------------------------------------------------------------------------------
//
//
// Author: Ciara Giles-Doran <gciara@student.ethz.ch>
// Last updated: 24 JUN 2022
//---------------------------------------------------------------------------------------------------------------------------------------

#ifndef CONSTANTS_H
#define CONSTANTS_H

// UNIVERSAL CONSTANTS
#define BINARY_8_BIT_SHIFT              8
#define BINARY_255                      0xFF

// PC <-> TEENSY SERIAL COMMUNICATION
#define SERIAL_COMMS_SHIFT              8
#define SERIAL_COMMS_MAX_PKT_LEN        6           // Clarify

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

// AER
#define AER_WRITE_HANDSHAKE_TIMEOUT     10          // microseconds
#define AER_HANDSHAKE_DELAY             0           // microseconds
#define AER_BUFFER_SIZE                 1024

#define AER_ENCODER_ACK                 30
#define AER_ENCODER_REQ                 29
#define AER_ENCODER_INPUT_NO_PIN        3  
#define AER_ENCODER_INPUT_BIT_0_PIN     26
#define AER_ENCODER_INPUT_BIT_1_PIN     27
#define AER_ENCODER_INPUT_BIT_2_PIN     28

#define AER_DECODER_ACK                 34
#define AER_DECODER_REQ                 1
#define AER_DECODER_OUTPUT_NO_PIN       9  
#define AER_DECODER_OUTPUT_BIT_0_PIN    18
#define AER_DECODER_OUTPUT_BIT_1_PIN    19
#define AER_DECODER_OUTPUT_BIT_2_PIN    20
#define AER_DECODER_OUTPUT_BIT_3_PIN    21
#define AER_DECODER_OUTPUT_BIT_4_PIN    22
#define AER_DECODER_OUTPUT_BIT_5_PIN    23
#define AER_DECODER_OUTPUT_BIT_6_PIN    24
#define AER_DECODER_OUTPUT_BIT_7_PIN    25
#define AER_DECODER_OUTPUT_BIT_8_PIN    26

#define AER_RECORD_EVENT_LEFT_SHIFT     5           // Clarify
#define AER_RECORD_EVENT_RIGHT_SHIFT    8           // Clarify
#define AER_RECORD_EVENT_TSTAMP_SHIFT   10          // Clarify
#define AER_RECORD_EVENT_C0             0xE0        // Clarify
#define AER_RECORD_EVENT_C1             0x1F        // Clarify
#define AER_RECORD_EVENT_C2             0xFF        // Clarify

#define AER_NO_CORES                    2
#define AER_NO_SYNAPSE_TYPES            4
#define AER_NO_NEURONS                  4
#define AER_NO_NMDA_SYNAPSES            40
#define AER_NO_GABAa_SYNAPSES           2
#define AER_NO_GABAb_SYNAPSES           16
#define AER_NO_AMPA_SYNAPSES            6
#define AER_NO_AMPA_SYNAPSES_NN         3           // 3 AMPA+ and 3 AMPA-

#define AER_CORE_SHIFT                  8
#define AER_SYNAPSE_TYPE_SHIFT          6
#define AER_NEURON_SHIFT                4
#define AER_NN_GABAb_BITS_4_5           1
#define AER_GABAa_BITS_1_2_3            7
#define AER_AMPA_SHIFT                  3
#define AER_PACKET_SIZE                 9

// C2F COMMUNICATION
#define C2F_ACK                         0
#define C2F_REQ                         35
#define C2F_INPUT_NO_PIN                5
#define C2F_INPUT_BIT_0_PIN             19
#define C2F_INPUT_BIT_1_PIN             20
#define C2F_INPUT_BIT_2_PIN             21
#define C2F_INPUT_BIT_3_PIN             22
#define C2F_INPUT_BIT_4_PIN             23

// RESET (OUTPUT PINS)
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

// SERIAL PORT
#define PORT_NAME                       "/dev/ttyACM0"
                                        //"/dev/cu.usbmodem105688601";  
                                        //"cu.usbmodem105688601";           ALIVE non soldered board
                                        //"/cu.usbmodem105661701";          ALIVE BOARD

// GUI CONSTANTS
#define BUTTON_HEIGHT                   20
#define BUTTON_UPDATE_WIDTH             100
#define BUTTON_AER_WIDTH                285
#define WINDOW_HEIGHT                   800
#define WINDOW_WIDTH                    1100
#define CLEAR_COLOUR_X                  0.45f
#define CLEAR_COLOUR_Y                  0.55f
#define CLEAR_COLOUR_Z                  0.60f
#define CLEAR_COLOUR_W                  1.00f

#endif 