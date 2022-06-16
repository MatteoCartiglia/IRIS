#ifndef NCS_TEENSY_H
#define NCS_TEENSY_H

#include "teensy_interface.h"

//SPAIC chip values. Other chips need to change these values
//DAC configs for SPIAC
//const int dac_rst = 33; // DAC reset on Teensy pin 33
//const int a1 = 31;      // DAC address A1 on Teensy pin 31
//const int a0 = 30;      // DAC address A0 on Teeensy pin 30

//DAC configs for ALIVE
const int dac_rst = 33; // DAC reset on Teensy pin 33
const int a1 = 36;      // DAC address A1 on Teensy pin 31
const int a0 = 37;      // DAC address A0 on Teeensy pin 30

//SPI configs
bool BIAS_GEN = true;
bool SPI1_ON= false;
bool SPI2_ON= false;

//SPI0
const int slaveSelectPin_SPI_BGEN = 9;
const int BGMOSI = 11;
const int BGSCK = 13;
const int BG_enable = 10;

//SPI1
const int slaveSelectPin_SPI1_CRST = 0 ;
const int CMOSI = 26 ;
const int CSCK = 27 ;

//SPI 2
const int slaveSelectPin_SPI2_VRST = 44 ;
const int VMOSI = 43 ;
const int VSCK = 45 ;

//RESETS
constexpr int prst = 9; //expander
constexpr int srst = 8; //expander

//AER PINS
bool AER_ON = true;

//ALIVE REQ/ACK PINS
constexpr int aero_ack = 30;
constexpr int aero_req = 29;
constexpr int aero_num_data_pins = 3;
int aero_data[aero_num_data_pins] = {26,27,28};
constexpr int aer_delays = 0; // Delay in microseconds after toggling aer handshake output pins

// buffers
constexpr int aero_buf_size = 1024;



#endif 


