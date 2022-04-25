#include <Arduino.h>
#include <SPI.h>
#include "ncs_teensy.h"
#include "dac.h"

void spi_controller(SPIClass SPI, int cs, int address, int value);
void SPI_events(int spi_number, int address, int value);

DAC dac{dac_rst,
    a0, 
    a1
};


void setup() {


}

void loop() {

  
}



void SPI_events(int spi_number, int address, int value)
{
  switch(spi_number)
  {
    case 0:
        spi_controller(SPI, slaveSelectPin_SPI_BGEN, address, value);
    case 1:
        spi_controller(SPI1, slaveSelectPin_SPI1_CRST,address, value);
    case 2:
        spi_controller(SPI2, slaveSelectPin_SPI2_VRST, address, value);

    default:
    break;
  }

}

//cannot create an SPI class because it SPI.h doesn't have a constructur
void spi_controller(SPIClass SPI, int cs, int address, int value)
{
    SPI.begin();
    SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
    digitalWrite(cs,LOW);
    SPI.transfer(address);
    SPI.transfer(value);
    digitalWrite(cs,HIGH);
    SPI.endTransaction();

}