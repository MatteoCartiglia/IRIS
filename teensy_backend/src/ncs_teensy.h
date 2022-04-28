#include "teensy_interface.h"

//SPAIC chip values. Other chips need to change these values
//DAC configs for SPIAC
const int dac_rst = 33; // DAC reset on Teensy pin 33
const int a1 = 31;      // DAC address A1 on Teensy pin 31
const int a0 = 30;      // DAC address A0 on Teeensy pin 30

//DAC configs for ALIVE
//const int dac_rst = 33; // DAC reset on Teensy pin 33
//const int a1 = 36;      // DAC address A1 on Teensy pin 31
//const int a0 = 37;      // DAC address A0 on Teeensy pin 30

//SPI configs
bool BIAS_GEN = true;
bool SPI1_ON= true;
bool SPI2_ON= true;

//SPI0
const int slaveSelectPin_SPI_BGEN = 10;
const int BGRST = 9;
const int BGMOSI = 11;
const int BGSCK = 13;

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
constexpr int aero_ack = 4;
constexpr int aero_req = 5;
constexpr int aero_num_data_pins = 3;
int aero_data[aero_num_data_pins] = {9, 8, 7};
constexpr int aer_delays = 0; // Delay in microseconds after toggling aer handshake output pins

// buffers
P2TPkt rx_buf;

constexpr int tx_buf_size = 1024;
uint8_t tx_buf[tx_buf_size];
uint8_t aero_buf[tx_buf_size - 2];

uint8_t msg_buf[0];

