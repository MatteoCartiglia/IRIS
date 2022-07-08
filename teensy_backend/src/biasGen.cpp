//---------------------------------------------------------------------------------------------------------------------------------------
//
//
// Author: Ciara Giles-Doran <gciara@student.ethz.ch>
// Last updated: 
//---------------------------------------------------------------------------------------------------------------------------------------

#include "biasGen.h"
#include "constants.h"

//---------------------------------------------------------------------------------------------------------------------------------------
// BiasGen constructor: initialises the BiasGen object
//---------------------------------------------------------------------------------------------------------------------------------------

BiasGen::BiasGen(const int clk, const int cs, const int mosi , const int enable)
{
    _clk = clk;
    _cs = cs;
    _mosi = mosi;
    _enable = enable;
}

//---------------------------------------------------------------------------------------------------------------------------------------
// Setup the bias generator
//---------------------------------------------------------------------------------------------------------------------------------------

void BiasGen::setupBiasGen()
{
    pinMode(_enable, OUTPUT);
    pinMode(_clk, OUTPUT);
    pinMode(_cs, OUTPUT);
    pinMode(_mosi, OUTPUT);

    digitalWrite(_clk, LOW);
    delay(1);
    digitalWrite(_cs, HIGH);
    delay(1);
    digitalWrite(_mosi, LOW);
    delay(1);
    digitalWrite(_enable, HIGH);
    delay(1);
}

//---------------------------------------------------------------------------------------------------------------------------------------
// Write to the bias generator 
//---------------------------------------------------------------------------------------------------------------------------------------

void BiasGen::writeBiasGen(SPIClass SPI, int pin, int address, int value)
{
    SPI.begin();
    SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));

    uint8_t add = (address  >> 8 ) & 0xFF ; 
    uint8_t add2 = address & 0xFF ; 
    uint8_t val = (value  >> 8 ) & 0xFF ; 
    uint8_t val2 = value  & 0xFF ; 

    digitalWrite(pin, LOW);
    delay(100);
    digitalWrite(pin, HIGH);
    delay(100); 
    digitalWrite(pin, LOW);
    delay(100);

    SPI.transfer(add);
    SPI.transfer(add2);
    SPI.transfer(val);
    SPI.transfer(val2);
    SPI.endTransaction();
}