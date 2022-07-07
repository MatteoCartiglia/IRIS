//---------------------------------------------------------------------------------------------------------------------------------------
//
//
// Author: Ciara Giles-Doran <gciara@student.ethz.ch>
// Last updated: 24 JUN 2022
//---------------------------------------------------------------------------------------------------------------------------------------

#ifndef CONSTANTS_H
#define CONSTANTS_H

//
#define AER_DECODER_BIT_0_PIN       18
#define AER_DECODER_BIT_1_PIN       19
#define AER_DECODER_BIT_2_PIN       20
#define AER_DECODER_BIT_3_PIN       21
#define AER_DECODER_BIT_4_PIN       22
#define AER_DECODER_BIT_5_PIN       23
#define AER_DECODER_BIT_6_PIN       24
#define AER_DECODER_BIT_7_PIN       25
#define AER_DECODER_BIT_8_PIN       26

// DAC CONFIGURATION FOR ALIVE
#define DAC_RESET               33
#define DAC_A1                  36
#define DAC_A0                  37
#define FULL_RANGE              65536
#define DAC_REF                 2500

// SPI CONFIGURATION
#define BIAS_GEN                1
#define SPI1_ON                 0
#define SPI2_ON                 0

// SPI 0 CONFIGURATION
#define SLAVE_SPI0_BGEN         9
#define BG_MOSI                 11
#define BG_SCK                  13
#define BG_ENABLE               10

// SPI 1 CONFIGURATION
#define SLAVE_SPI1_CRST         0
#define C_MOSI                  26
#define C_SCK                   27

// SPI 2 CONFIGURATION
#define SLAVE_SPI2_VRST         44
#define V_MOSI                  43
#define V_SCK                   45

// RESETS
#define PRST                    9 // expander
#define SRST                    8 // expander

// AER PINS
#define AER_ON                  1
#define AER_DELAYS              0 // Delay in microseconds after toggling AER handshake output pins
#define AER_HANDSHAKE_TIMEOUT   10

#define AERO_ACK                30
#define AERO_REQ                29
#define AERO_NUM_DATA_PINS      3
#define AERO_DURATION           15
#define AERO_BUFF_SIZE          1024

// BIAS GENERATION
#define FINE_BIAS_SHIFT         1
#define COURSE_BIAS_SHIFT       9

// PACKET CONSTANTS
#define MAX_PKT_LEN             6
#define PKT_HDR_PKT_TYPE_SHIFT  0

#endif 