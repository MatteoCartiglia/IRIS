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

class BiasGen {

  // ------------------------------------------ Declaring class constructor and public methods -----------------------------------------

  public:

    // BiasGen constructor
    BiasGen(const int clk, const int cs, const int mosi , const int enable);

    // Setup the bias generator
    void setupBiasGen();

    // Write to the bias generator
    void writeBiasGen(SPIClass SPI, int pin, int address, int value);


  // --------------------------------------------------- Declaring private variables -------------------------------------------------

  private:
    int _clk;
    int _cs;
    int _mosi;
    int _enable;
};

#endif
