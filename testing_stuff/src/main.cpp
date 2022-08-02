#include <Arduino.h>
#include <SPI.h>


const int chipSelectPin = 10;

void setup() {

    pinMode(chipSelectPin, OUTPUT);
    SPI.begin();
    SPI.transfer(1);
    SPI.transfer(1);

    SPI1.begin();
    
    SPI1.transfer(1);

    SPI1.transfer(1);

}

void loop() {


}
