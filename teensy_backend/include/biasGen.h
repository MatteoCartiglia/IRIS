//---------------------------------------------------------------------------------------------------------------------------------------
//
//
// Author: Ciara Giles-Doran <gciara@student.ethz.ch>
// Last updated: 
//---------------------------------------------------------------------------------------------------------------------------------------

#ifndef BIASGEN_H
#define BIASGEN_H

#include <Arduino.h>
#include <SPI.h>
#include "constants.h"

class BiasGen {

  // ------------------------------------------ Declaring class constructor and public methods -----------------------------------------

  public:

    // BiasGen constructor
    BiasGen(const int clk, const int reset, const int mosi , const int enable);

    // Setup the bias generator
    void setupBiasGen();

    // Write to the bias generator
    void writeBiasGen(int address, int value);

    // Get decimal equivalent value of binary packet
    float getBiasGenDecimal(int binaryValue);


  // --------------------------------------------------- Declaring private variables -------------------------------------------------

  private:
    int _clk;
    int _reset;
    int _mosi;
    int _enable;
    float _masterCurrent[BIASGEN_MASTER_CURRENTS_NO] = {0.00006, 0.00046, 0.0038, 0.03, 0.24, 1.9}; // uA
};

#endif
