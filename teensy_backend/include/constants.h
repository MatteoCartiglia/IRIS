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

// AER COMMS
#define AER_WRITE_HANDSHAKE_TIMEOUT     10
#define AER_RECORD_EVENT_LEFT_SHIFT     5
#define AER_RECORD_EVENT_RIGHT_SHIFT    8
#define AER_RECORD_EVENT_C0             0xE0
#define AER_RECORD_EVENT_C1             0x1F
#define AER_RECORD_EVENT_C2             0xFF






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

// RESETS
#define PRST                    9 // expander
#define SRST                    8 // expander

// AER PINS
#define AER_ON                  1
#define AER_DELAYS              0 // Delay in microseconds after toggling AER handshake output pins

#define AERO_ACK                30
#define AERO_REQ                29
#define AERO_NUM_DATA_PINS      3
#define AERO_DURATION           15
#define AERO_BUFF_SIZE          1024


#endif 