//---------------------------------------------------------------------------------------------------------------------------------------
// Header file for generalised spiConfig class
// NOTE: Cannot create an SPI class because SPI.h doesn't have a constructur
//
// Author: Matteo Cartiglia <camatteo@ini.uzh.ch>
// Last updated: 16 AUG 2022 (Ciara Giles-Doran)
//---------------------------------------------------------------------------------------------------------------------------------------

#ifndef SPICONFIG_H
#define SPICONFIG_H

#include <Arduino.h>
#include <SPI.h>
#include "constants.h"
#include "constants_global.h"

class SPIConfig {

  // ------------------------------------------ Declaring class constructor and public methods -----------------------------------------

  public:

    //----------------------------------------------------------------------------------------------------------------------------------
    // Class constructor; initialises the basic SPI object and sets up the relevant pins on Teensy
    //----------------------------------------------------------------------------------------------------------------------------------
    SPIConfig(const int clk, const int reset, const int mosi, int SPInumber ); // SPIs

    //----------------------------------------------------------------------------------------------------------------------------------
    // Class constructor; initialises the BiasGen object and sets up the relevant pins on Teensy
    //----------------------------------------------------------------------------------------------------------------------------------
    SPIConfig(const int clk, const int reset, const int mosi, const int enable, int SPInumber ); // biasgen has enable

    //----------------------------------------------------------------------------------------------------------------------------------
    // setupSPI: Sets up the relevant pins on Teensy
    //----------------------------------------------------------------------------------------------------------------------------------
    void setupSPI();
    void setupBG();

    //----------------------------------------------------------------------------------------------------------------------------------
    // resetSPI: Executes the SPI reset pattern 
    //----------------------------------------------------------------------------------------------------------------------------------
    void resetSPI();

    //----------------------------------------------------------------------------------------------------------------------------------
    // writeSPI: Writes values to the SPI specified
    //----------------------------------------------------------------------------------------------------------------------------------
    void writeSPI(int address, int value);

    //----------------------------------------------------------------------------------------------------------------------------------
    // getBiasGenDecimal: Converts the 12-bit binary value sent to Bias Generator into its approx. decimal equivalent 
    //----------------------------------------------------------------------------------------------------------------------------------
    float getBiasGenDecimal(int binaryValue);


  // --------------------------------------------------- Declaring private variables --------------------------------------------------

  private:
    int _clk;
    int _reset;
    int _mosi;
    int _enable = 0;
    int _SPInumber;
    #ifdef EXISTS_BIASGEN
    double _masterCurrent[BIASGEN_NO_MASTER_CURRENTS] = {BIASGEN_MASTER_CURRENT_0, BIASGEN_MASTER_CURRENT_1, BIASGEN_MASTER_CURRENT_2,
                                                      BIASGEN_MASTER_CURRENT_3, BIASGEN_MASTER_CURRENT_4, BIASGEN_MASTER_CURRENT_5}; // uA
    #endif
   };

#endif
