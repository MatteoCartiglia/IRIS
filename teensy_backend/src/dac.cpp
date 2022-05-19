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
void DAC::join_i2c_bus(){

  Wire2.begin(); // join i2c bus (address optional for master)
  delay(500);

}

void DAC::write_dacs(uint8_t command_addess, uint16_t value){
    int a = command_addess; 
    int val = (value * FULL_RANGE )/DAC_REF -1 ; 
    int c = val & 0xFF;
    int b = (val >> 8) & 0xFF; 

    Wire2.beginTransmission(12); //
    Wire2.write(a);
    Wire2.write(b);
    Wire2.write(c);
    Wire2.endTransmission();


    delay(100);
    Serial.print("DAC command recived \n");

}

void DAC::turn_reference_off(){
// turn off internal reference Voltage  


    Wire2.beginTransmission(12);                 
    Wire2.write(0x70);             
    Wire2.write(0x00);           
    Wire2.write(0x00);          
    Wire2.endTransmission(); 
    delay(100);
}
