//---------------------------------------------------------------------------------------------------------------------------------------
// Header file for BiasGen class
//
// Author: Ciara Giles-Doran <gciara@student.ethz.ch>
// Last updated: 15 JUL 2022
//---------------------------------------------------------------------------------------------------------------------------------------

#ifndef BIASGEN_H
#define BIASGEN_H

#include <Arduino.h>
#include <SPI.h>
#include "constants.h"

class BiasGen {

  // ------------------------------------------ Declaring class constructor and public methods -----------------------------------------

  public:

    //----------------------------------------------------------------------------------------------------------------------------------
    // Class constructor; initialises the BiasGen object and sets up the relevant pins on Teensy
    //----------------------------------------------------------------------------------------------------------------------------------
    BiasGen(const int clk, const int reset, const int mosi , const int enable);

    //----------------------------------------------------------------------------------------------------------------------------------
    // setupBiasGen: Sets up the relevant pins on Teensy
    //----------------------------------------------------------------------------------------------------------------------------------
    void setupBiasGen();

    //----------------------------------------------------------------------------------------------------------------------------------
    // writeBiasGen: Writes values to the bias generator
    //----------------------------------------------------------------------------------------------------------------------------------
    void writeBiasGen(int address, int value);

    //----------------------------------------------------------------------------------------------------------------------------------
    // getBiasGenDecimal: Converts the 12-bit binary value sent to Bias Generator into its approx. decimal equivalent 
    //----------------------------------------------------------------------------------------------------------------------------------
    float getBiasGenDecimal(int binaryValue);


  // --------------------------------------------------- Declaring private variables --------------------------------------------------

  private:
    int _clk;
    int _reset;
    int _mosi;
    int _enable;
    double _masterCurrent[BIASGEN_NO_MASTER_CURRENTS] = {BIASGEN_MASTER_CURRENT_0, BIASGEN_MASTER_CURRENT_1, BIASGEN_MASTER_CURRENT_2,
                                                          BIASGEN_MASTER_CURRENT_3, BIASGEN_MASTER_CURRENT_4, BIASGEN_MASTER_CURRENT_5}; // uA
};

#endif
