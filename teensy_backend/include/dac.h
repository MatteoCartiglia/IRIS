//---------------------------------------------------------------------------------------------------------------------------------------
//
//
// Author: Matteo Cartiglia <camatteo@ini.uzh.ch>
// Last updated: 24 JUN 2022 (Ciara Giles-Doran)
//---------------------------------------------------------------------------------------------------------------------------------------

#ifndef DAC_H
#define DAC_H

#include "Arduino.h"

class DAC {

  // ------------------------------------------ Declaring class constructor and public methods -----------------------------------------

  public:

    // DAC constructor
    DAC(const int dac_rst, const int a0, const int a1);

    // Setup the DAC
    void setup_dacs();

    // Join the i2c bus 
    void join_i2c_bus();

    // Write to the DAC
    void write_dacs(uint8_t address, uint16_t value);

    // Turn off the internal reference voltage
    void turn_reference_off();


  // --------------------------------------------------- Declaring private variables -------------------------------------------------

  private:
    int _dac_rst;
    int _a0;
    int _a1;
};

#endif
