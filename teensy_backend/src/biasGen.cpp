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

BiasGen::BiasGen(const int clk, const int reset, const int mosi , const int enable)
{
    _clk = clk;
    _reset = reset;
    _mosi = mosi;
    _enable = enable;

    setupBiasGen();
}

//---------------------------------------------------------------------------------------------------------------------------------------
// Setup the bias generator
//---------------------------------------------------------------------------------------------------------------------------------------

void BiasGen::setupBiasGen()
{
    pinMode(_enable, OUTPUT);
    pinMode(_clk, OUTPUT);
    pinMode(_reset, OUTPUT);
    pinMode(_mosi, OUTPUT);
    delay(5);

    digitalWrite(_reset, LOW);          // Clarify reset pattern
    delay(1);
    digitalWrite(_mosi, LOW);
    delay(1);
    digitalWrite(_clk, LOW);
    delay(1);
    digitalWrite(_reset, HIGH);
    delay(1);
    digitalWrite(_enable, HIGH);
    delay(1);
    digitalWrite(_reset, LOW);
    delay(1);
}

//---------------------------------------------------------------------------------------------------------------------------------------
// Write to the bias generator 
//---------------------------------------------------------------------------------------------------------------------------------------

void BiasGen::writeBiasGen(int address, int value)
{
    SPI.begin();
    SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));

    uint8_t add = (address  >> 8 ) & 0xFF ; 
    uint8_t add2 = address & 0xFF ; 
    uint8_t val = (value  >> 8 ) & 0xFF ; 
    uint8_t val2 = value  & 0xFF ; 

    digitalWrite(BIASGEN_SLAVE_SPI0, LOW);
    delay(100);
    digitalWrite(BIASGEN_SLAVE_SPI0, HIGH);
    delay(100); 
    digitalWrite(BIASGEN_SLAVE_SPI0, LOW);
    delay(100);

    SPI.transfer(add);
    SPI.transfer(add2);
    SPI.transfer(val);
    SPI.transfer(val2);
    SPI.endTransaction();
}

//---------------------------------------------------------------------------------------------------------------------------------------
// getBiasGenDecimal
//---------------------------------------------------------------------------------------------------------------------------------------
float BiasGen::getBiasGenDecimal(int binaryValue)
{
    // Shifting value to right by 1 to remove transistor type value (not relevant for decimal calculation)
    int binaryValShifted = binaryValue >> 1;
    int binaryCoarseVal = binaryValue >> (BIASGEN_SHIFT_COURSE);
    int binaryFineVal = binaryValShifted - binaryCoarseVal;

    // Serial.print("\t");
    // Serial.print(binaryValue);
    // Serial.print("\t");
    // Serial.print(binaryCoarseVal);
    // Serial.print("\t");
    // Serial.print(binaryFineVal);
    // Serial.print("\t");

    float fineCurrent = (binaryFineVal*_masterCurrent[binaryCoarseVal])/BIASGEN_MULTIPL;

    return fineCurrent;
}