#ifndef dac_h
#define dac_h

#include "Arduino.h"

class DAC {
  public:
    DAC(   const int dac_rst,
           const int a0,
           const int a1
    );

    void setup_dacs();
    bool reset_dacs();
    void write_dacs(int a, int b, int c);
    void turn_reference_off();


  private:
    int _dac_rst;
    int _a0;
    int _a1;

};
#endif
