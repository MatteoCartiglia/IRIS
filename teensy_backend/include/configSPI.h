#ifndef configSPI_H
#define configSPI_H

#include <Arduino.h>
#include <SPI.h>
#include "constants.h"


class configSPI {


  public:

    configSPI(const int clk, const int reset, const int mosi, int SPInumber ); // SPIs
    configSPI(const int clk, const int reset, const int mosi, const int enable, int SPInumber ); // biasgen has enable

    void setupSPI();

    void resetSPI();

    void writeSPI(int address, int value);

    float getBiasGenDecimal(int binaryValue);



  private:
    int _clk;
    int _reset;
    int _mosi;
    int _enable = 0;
    int _SPInumber;
    double _masterCurrent[BIASGEN_NO_MASTER_CURRENTS] = {BIASGEN_MASTER_CURRENT_0, BIASGEN_MASTER_CURRENT_1, BIASGEN_MASTER_CURRENT_2,
                                                          BIASGEN_MASTER_CURRENT_3, BIASGEN_MASTER_CURRENT_4, BIASGEN_MASTER_CURRENT_5}; // uA

   };

#endif
