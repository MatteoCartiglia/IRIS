#include "dac.h"
#include <Wire.h>



DAC::DAC(const int dac_rst, const int a0, const int a1) {
  _dac_rst = dac_rst;
  _a0 =  a0;
  _a1 = a1;

}

void DAC::setup_dacs(){
  pinMode(_dac_rst, OUTPUT);
  pinMode(_a1, OUTPUT);
  pinMode(_a0, OUTPUT);
  delay(5);
  // set DAC's address pins LOW
  digitalWrite(_a1, LOW);
  digitalWrite(_a0, LOW);
  delay(1);

  //reset the DAC 
  digitalWrite(_dac_rst, HIGH);
  delay(1);
  digitalWrite(_dac_rst, LOW);
  delay(1);
  digitalWrite(_dac_rst, HIGH);
  delay(1);
  Serial.println("Setup DAC complete ");
}

void DAC::write_dacs(uint8_t command_addess, int value){
    uint8_t a = command_addess; 
    uint8_t b = (value >> 8); 
    uint8_t c = value;
    Wire2.beginTransmission(12);                 
    Wire2.write(a);             
    Wire2.write(b);           
    Wire2.write(c);          
    Wire2.endTransmission(); 
    delay(100);
}

void DAC::turn_reference_off(){
// turn off internal reference Voltage                            
    Wire2.beginTransmission(12);                 
    Wire2.write(0x70);             
    Wire2.write(0x00);           
    Wire2.write(0x01);          
    Wire2.endTransmission(); 
    delay(100);
}
