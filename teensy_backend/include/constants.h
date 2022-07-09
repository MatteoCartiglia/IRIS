//---------------------------------------------------------------------------------------------------------------------------------------
//
//
// Author: Ciara Giles-Doran <gciara@student.ethz.ch>
// Last updated: 24 JUN 2022
//---------------------------------------------------------------------------------------------------------------------------------------

#ifndef CONSTANTS_H
#define CONSTANTS_H

// SERIAL COMMS
#define SERIAL_MAX_PKT_LEN              6

// BIASGEN COMMS
#define BIASGEN_PACKET_LENGTH           12
#define BIASGEN_SLAVE_SPI0              9
#define BIASGEN_MOSI_PIN                11
#define BIASGEN_SCK_PIN                 13
#define BIASGEN_ENABLE_PIN              10

// AER COMMUNICATION
#define AER_WRITE_HANDSHAKE_TIMEOUT     10      // microseconds
#define AER_HANDSHAKE_DELAY             0       // microseconds
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

#define AER_RECORD_EVENT_LEFT_SHIFT     5       // Clarify
#define AER_RECORD_EVENT_RIGHT_SHIFT    8       // Clarify
#define AER_RECORD_EVENT_TSTAMP_SHIFT   10      // Clarify
#define AER_RECORD_EVENT_C0             0xE0    // Clarify
#define AER_RECORD_EVENT_C1             0x1F    // Clarify
#define AER_RECORD_EVENT_C2             0xFF    // Clarify

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

// DAC CONFIGURATION FOR ALIVE
#define DAC_RESET                       33    
#define DAC_A1                          36    
#define DAC_A0                          37    
#define FULL_RANGE                      65536   // Clarify
#define DAC_REF                         2500    // Clarify

// PC -> TEENSY SERIAL COMMUNICATION
#define SERIAL_COMMS_SHIFT              8
#define MAX_PKT_LEN                     6       // Clarify




#define AERO_INPUT_DURATION           15
#define AER_ON                  1




#endif 