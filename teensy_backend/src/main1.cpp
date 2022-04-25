/*#include <Arduino.h>
#include <Wire.h>
#include "ncs_teensy.h"
#include "aer_in.h"
#include "dac.h"
#include "usb_serial.h"
#include <SPI.h>


AER_in aero(aero_req,
            aero_ack,
            aero_data,
            aero_num_data_pins,
            aero_buf,
            aer_delays,
            10,
            false);

DAC dac( dac_rst, 
          a0, 
          a1);

SPIClass SPIxx;
SPIClass SPI11();
SPIClass SPI22();


SPI_control SPI_BG( slaveSelectPin_SPI_BGEN, 
          BGMOSI,
          BGSCK,
          SPI,
          BGRST );

SPI_control SPI1( slaveSelectPin_SPI1_CRST, 
          CMOSI,
          CSCK,
          SPI11

          );

SPI_control SPI2( slaveSelectPin_SPI2_VRST,  
          VMOSI,
          VSCK,
          SPI22
          );
          


void setup() {

  dac.setup_dacs();
  SPI_BG.setup_spi();
  if (SPI1_ON)
    SPI1.setup_spi();
  if (SPI2_ON)
    SPI2.setup_spi();
    
  dac.turn_reference_off();

  digitalWrite(13, HIGH); // LED on TEENSY

  Serial.println("All_setup");

}
void loop() {
  delay(100);

  SPI.begin();
  SPI1.begin();
  SPI2.begin();

  if (usb_serial_available()) {
    usb_serial_read(rx_buf, rx_buf_size);

 while (Serial.available()) {
        delay(100);  
          if (Serial.available() >0) {
        char c = Serial.read();
        readString += c;}
        }
        
    if (readString.length() >0){

        Serial.println(readString);

    String commandtype = readString.substring(0, 3);

    String spi_n = readString.substring(0, 1);
    String adress = readString.substring(1, 4);
    String value = readString.substring(4, 7);

    int n1 = spi_n.toInt();
    int n2 = adress.toInt();
    int n3 = value.toInt();
    
    spi_write(n1,n2, n3);
    readString="";
 
  }
}



void decode_command( string readString ){
    String commandtype = readString.substring(0, 3);


    if (commandtype == "DAC_") {
      String address = readString.substring(4, 5);
      String value = readString.substring(6, 9);
    }
    if (commandtype == "BGE_") {
      String address = readString.substring(4, 6);
      String value = readString.substring(7, 9);

    }


}

//bsgentype (binary) 8 address 

//DAC 4 command 4 address 16 value
/*            ssp << std::setw(1) << std::setfill('0') << SPInumber;
            SPI_port += ssp.str();
            ssp1 << std::setw(3) << std::setfill('0') << SPIaddess;
            SPI_port += ssp1.str();
            ssp2 << std::setw(3) << std::setfill('0') << SPIvalue;
            SPI_port += ssp2.str();
            const char *spi_label = SPI_port.c_str();

                 std::string DAC_port = "DAC_";
                printf("\n DAC number: %02d with value : %04d \n", i,DACvalue[i]);
                ss << std::setw(2) << std::setfill('0') << i;
                DAC_port += ss.str();;
                ss1 << std::setw(4) << std::setfill('0') << DACvalue[i];
                DAC_port += ss1.str();
                const char *dac_label = DAC_port.c_str();
                printf(dac_label);
                write(serial_port, dac_label, sizeof(dac_label));

                AER IN: REQ/ACK address is 16bits
                spik 

                */