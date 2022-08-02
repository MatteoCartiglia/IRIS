#ifndef chips_h
#define chips_h

#include "Arduino.h"
#include "Wire.h"
#include "SPI.h"

class DAC53608 {
  public:
    void begin(const int addr, TwoWire& bus = Wire1);
    void setRegisterPointer(uint8_t reg);
    int16_t read();
    void setDAC(uint8_t channel, uint16_t value);
    void setHighZ(uint8_t channel, uint16_t value); //TODO
    void write16(uint8_t reg, uint16_t d);
    
  private:
    TwoWire* _bus;
    uint8_t  _addr;

};

int16_t setCurrent(void (*dac_function)(uint16_t dac_value), uint16_t (*adc_function)(), int16_t value, int dacMin, int dacMax);

#endif