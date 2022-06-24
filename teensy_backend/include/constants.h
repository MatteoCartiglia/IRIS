//---------------------------------------------------------------------------------------------------------------------------------------
//
//
// Author: Ciara Giles-Doran <gciara@student.ethz.ch>
// Last updated: 
//---------------------------------------------------------------------------------------------------------------------------------------

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <cstdint>
#include <cstddef>

// DAC CONFIGURATION FOR ALIVE
#define DAC_RESET               33
// #define A1                      36
// #define A0                      37

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