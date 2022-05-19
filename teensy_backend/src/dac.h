#ifndef dac_h
#define dac_h

#include "Arduino.h"
#define FULL_RANGE 65536
#define DAC_REF 2500


class DAC {
  public:
    DAC(   const int dac_rst,
           const int a0,
           const int a1
    );
    void join_i2c_bus();
    void setup_dacs();
    bool reset_dacs();
    void write_dacs(uint8_t address, uint16_t value);
    void turn_reference_off();


  private:
    int _dac_rst;
    int _a0;
    int _a1;

};
#endif
