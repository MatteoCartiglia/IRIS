//---------------------------------------------------------------------------------------------------------------------------------------
//
//
// Author: Matteo Cartiglia <camatteo@ini.uzh.ch>
// Last updated: 24 JUN 2022 (Ciara Giles-Doran)
//---------------------------------------------------------------------------------------------------------------------------------------

#include <Wire.h>

#include "constants.h"
#include "dac.h"

//---------------------------------------------------------------------------------------------------------------------------------------
// DAC constructor: initialises the DAC object
//---------------------------------------------------------------------------------------------------------------------------------------

DAC::DAC(const int dac_rst, const int a0, const int a1) 
{
  _dac_rst = dac_rst;
  _a0 =  a0;
  _a1 = a1;

  setup_dacs();
}

//---------------------------------------------------------------------------------------------------------------------------------------
// Setup the DAC
//---------------------------------------------------------------------------------------------------------------------------------------

void DAC::setup_dacs()
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
  delay(1);
  digitalWrite(_dac_rst, LOW);
  delay(1);
  digitalWrite(_dac_rst, HIGH);
  delay(1);

  Serial.println("DAC setup complete.");
}

//---------------------------------------------------------------------------------------------------------------------------------------
// Join the I2C bus
//---------------------------------------------------------------------------------------------------------------------------------------

void DAC::join_i2c_bus()
{
  Wire2.begin();
  delay(500);
}

//---------------------------------------------------------------------------------------------------------------------------------------
// Write to the DAC 
//---------------------------------------------------------------------------------------------------------------------------------------

void DAC::write_dacs(uint8_t command_addess, uint16_t value)
{
    int commandAddess = command_addess; 

    // Scaling decimal value to DAC binary value 
    int dacData = (value * DAC_BINARY_RANGE) / DAC_REFERENCE -1 ; 

    // Const
    int dacDataHighByte = (dacData >> BINARY_8_BIT_SHIFT) & BINARY_255; 
    int dacDataLowByte = dacData & BINARY_255;

    Wire2.beginTransmission(12);
    Wire2.write(commandAddess);
    Wire2.write(dacDataHighByte);
    Wire2.write(dacDataLowByte);
    Wire2.endTransmission();
}

//---------------------------------------------------------------------------------------------------------------------------------------
// Turn off the internal reference voltage
//---------------------------------------------------------------------------------------------------------------------------------------

void DAC::turn_reference_off()
{
    Wire2.beginTransmission(12);                 
    Wire2.write(0x70);             
    Wire2.write(0x00);           
    Wire2.write(0x00);          
    Wire2.endTransmission(); 
    delay(100);
}
