//---------------------------------------------------------------------------------------------------------------------------------------
// Source file for DAC class
//---------------------------------------------------------------------------------------------------------------------------------------

#include "dac.h"
#include "constants.h"
#include "constants_global.h"

//----------------------------------------------------------------------------------------------------------------------------------
// Class constructor; initialises the BiasGen object and sets up the relevant pins on Teensy
//----------------------------------------------------------------------------------------------------------------------------------

DAC::DAC(const int dac_rst, const int a0, const int a1) 
{
  _dac_rst = dac_rst;
  _a0 =  a0;
  _a1 = a1;

  setupDAC();
}


//---------------------------------------------------------------------------------------------------------------------------------------
// setupDAC: Sets up the relevant pin modes on Teensy
//---------------------------------------------------------------------------------------------------------------------------------------

void DAC::setupDAC()
{
  pinMode(_dac_rst, OUTPUT);
  pinMode(_a1, OUTPUT);
  pinMode(_a0, OUTPUT);
  delay(5);

  // Set DAC's address pins LOW
  digitalWrite(_a1, LOW);
  digitalWrite(_a0, LOW);
  delay(1);

  // Reset the DAC 
  digitalWrite(_dac_rst, HIGH);
  delay(10);
  digitalWrite(_dac_rst, LOW);
  delay(10);
  digitalWrite(_dac_rst, HIGH);
  delay(10);

  Serial.println("DAC setup complete.");
}


//---------------------------------------------------------------------------------------------------------------------------------------
// join_I2C_bus: Joins the I2C bus
//---------------------------------------------------------------------------------------------------------------------------------------

void DAC::join_I2C_bus()
{
  Wire2.begin();
  delay(500);
}


//---------------------------------------------------------------------------------------------------------------------------------------
// writeDAC: Writes values to the DAC
//---------------------------------------------------------------------------------------------------------------------------------------

void DAC::writeDAC(uint8_t command_addess, uint16_t value)
{
    int commandAddess = command_addess; 

    // Scaling decimal value to DAC binary value 
    uint16_t dacData = (value * DAC_BINARY_RANGE) / DAC_REFERENCE - 1 ; 
    int dacDataLowByte = dacData & BINARY_255;
    int dacDataHighByte = (dacData >> BINARY_8_BIT_SHIFT) & BINARY_255; 

    Wire2.beginTransmission(12);
    Wire2.write(commandAddess);
    Wire2.write(dacDataHighByte);
    Wire2.write(dacDataLowByte);
    Wire2.endTransmission();
    delay(100);

}


//---------------------------------------------------------------------------------------------------------------------------------------
// turnReferenceOff: Turns off the internal reference voltage
//---------------------------------------------------------------------------------------------------------------------------------------

void DAC::turnReferenceOff()
{
    Wire2.beginTransmission(12);                 
    Wire2.write(0x70);             
    Wire2.write(0x00);           
    Wire2.write(0x00);          
    Wire2.endTransmission(); 
    delay(100);
}
