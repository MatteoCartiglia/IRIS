
//---------------------------------------------------------------------------------------------------------------------------------------
// Source file for generalised SPIConfig class
//---------------------------------------------------------------------------------------------------------------------------------------

#include "spiConfig.h"
#include "constants.h"

//----------------------------------------------------------------------------------------------------------------------------------
// Class constructor; initialises the basic SPI object and sets up the relevant pins on Teensy
//----------------------------------------------------------------------------------------------------------------------------------

SPIConfig::SPIConfig(const int clk, const int reset, const int mosi, int SPInumber )
{
    _clk = clk;
    _reset = reset;
    _mosi = mosi;
    _SPInumber = SPInumber;    
}
//----------------------------------------------------------------------------------------------------------------------------------
// Class constructor; initialises the BiasGen object and sets up the relevant pins on Teensy
//----------------------------------------------------------------------------------------------------------------------------------
SPIConfig::SPIConfig(const int clk, const int reset, const int mosi,const int enable, int SPInumber )
{
    _clk = clk;
    _reset = reset;
    _mosi = mosi;
    _enable = enable;
    _SPInumber = SPInumber;    
}

//---------------------------------------------------------------------------------------------------------------------------------------
// setupSPI: sets up the relevant pins on Teensy
//---------------------------------------------------------------------------------------------------------------------------------------

void SPIConfig::setupBG()
{
    pinMode(_reset, OUTPUT);
   
    pinMode(_clk, OUTPUT); 
    pinMode(_mosi, OUTPUT);
    pinMode(_enable, OUTPUT);
  

    delay(10);
    
    digitalWrite(_mosi, LOW);
    delay(10);
    digitalWrite(_clk, LOW);
    delay(10);
    digitalWrite(_enable, LOW);
    delay(10);

    resetSPI();
    delay(10);
    digitalWrite(_enable, HIGH);
    delay(10);



}

void SPIConfig::setupSPI()
{
    pinMode(_clk, OUTPUT);
    pinMode(_reset, OUTPUT);
    pinMode(_mosi, OUTPUT); 

    digitalWrite(_mosi, LOW);
    delay(10);
    digitalWrite(_clk, LOW);
    delay(10);
    
    resetSPI();
}


//----------------------------------------------------------------------------------------------------------------------------------
// resetSPI: Executes the SPI reset pattern 
//----------------------------------------------------------------------------------------------------------------------------------

void SPIConfig::resetSPI()
{
    digitalWrite(_reset, LOW);          
    delay(10);
    digitalWrite(_reset, HIGH);
    delay(10);
    digitalWrite(_reset, LOW);          

}

//----------------------------------------------------------------------------------------------------------------------------------
// writeSPI: Writes values to the SPI specified
//----------------------------------------------------------------------------------------------------------------------------------

void SPIConfig::writeSPI(int address, int value )
{
    uint8_t add = (address  >> 8 ) & 0xFF ; 
    uint8_t add2 = address & 0xFF ; 
    uint8_t val = (value  >> 8 ) & 0xFF ; 
    uint8_t val2 = value  & 0xFF ; 

    if (_SPInumber == 0){ 
        SPI.begin();
        SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
        SPI.transfer(add);
        SPI.transfer(add2);
        SPI.transfer(val);
        SPI.transfer(val2);
        SPI.endTransaction();
    }

    if (_SPInumber == 1){ 
        SPI1.begin();
        SPI1.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
        SPI1.transfer(add2);
        SPI1.transfer(val2);
        SPI1.endTransaction();
    }

    if (_SPInumber == 2){ 
        SPI2.begin();
        SPI2.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
        SPI2.transfer(add2);
        SPI2.transfer(val2);
        SPI2.endTransaction();
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------
// getBiasGenDecimal: Converts the 12-bit binary value sent to Bias Generator into its approx. decimal equivalent 
//---------------------------------------------------------------------------------------------------------------------------------------
#ifdef EXISTS_BIASGEN
float SPIConfig::getBiasGenDecimal(int binaryValue)
{
    int binaryCoarseVal = binaryValue >> (BIASGEN_COURSE_SHIFT);

    // Shifting value to right by 1 to remove transistor type value (not relevant for decimal calculation)
    int binaryValShifted = binaryValue >> 1;
    int binaryFineVal = binaryValShifted - (binaryCoarseVal << (BIASGEN_COURSE_SHIFT - 1));
    float fineCurrent = (binaryFineVal*_masterCurrent[binaryCoarseVal])/BIASGEN_SCALING_FACTOR;

    return fineCurrent;
}
#endif