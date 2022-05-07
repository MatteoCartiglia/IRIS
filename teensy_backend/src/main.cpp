#include <Arduino.h>
#include <SPI.h>
#include "ncs_teensy.h"
#include "dac.h"
#include "aer_in.h"
#include "teensy_interface.h"


void spi_controller(SPIClass SPI, int cs, int address, int value);
void SPI_events(int spi_number, int address, int value);
void spi_setup(int clk, int cs, int mosi );
void sendStatus(TeensyStatus msg);
void aeroISR();

DAC dac{dac_rst,
    a0, 
    a1
};


AER_in aero(aero_req,
            aero_ack,
            aero_data,
            aero_num_data_pins,
            aero_buf,
            aer_delays,
            
            10,
            false);

//bool aero_flag = false;
//uint32_t aero_duration = 0;
//constexpr int aer_delays = 0; // Delay in microseconds after toggling aer handshake output pins


void setup() {
    dac.setup_dacs();
    if (BIAS_GEN) spi_setup(BGSCK , slaveSelectPin_SPI_BGEN , BGMOSI);
    if (SPI1_ON) spi_setup(CSCK , slaveSelectPin_SPI1_CRST , CMOSI);
    if (SPI2_ON) spi_setup(VSCK , slaveSelectPin_SPI2_VRST , VMOSI);
    Serial.println("Setup");
}

void loop() {
   if (usb_serial_available()) {
        usb_serial_read(&rx_buf, sizeof(rx_buf)); // in bytes. 1 byte is 8 bits.

        switch ((P2tPktType)rx_buf.header) {

            case P2tPktType::P2tReset: { // reset
                Serial.println("Reset recieved");
                sendStatus(TeensyStatus::Success);
                    break;
                    }
            case P2tPktType::P2tSetBiasGen: { // SPIs
                Serial.print("Biasgen command recieved \n");
                BIASGEN_command BG ( *const_cast<const P2TPkt*> (&rx_buf));
                    break;
                    }       
            case P2tPktType::P2tSetSPI: { // SPIs
                Serial.print("SPI command recieved \n");
                SPI_command SPI ( *const_cast<const P2TPkt*> (&rx_buf));
                SPI_events(SPI.spi_number , SPI.address, SPI.value);

                    break;
                    } 
            case P2tPktType::P2tSetDcVoltage : { // DAC

                Serial.print("DAC command recived \n");
                DAC_command DAC( *const_cast<const P2TPkt*> (&rx_buf));
                dac.write_dacs(DAC.command_address, DAC.data); 

                sendStatus(TeensyStatus::Success);

                break;
                    } 
            case  P2tPktType::P2tRequestAerOutput: { // aer
              //  AER_out(rx_buf);
                Serial.print("aer");
                Serial.print("\n");
                break;
                    }
            case P2tPktType::P2tGetTeensySN: { // sn
                break;       
                } 

             case P2tPktType::P2tSendEvents: { // II     
                break;     
                } 
           } 
        }
} 
//interrupt handler run all the time and through out or put into buffer (flag if full) main loop empties this buffer
// sending events to the chip: number 1 without isi number two with buffer and timer interupt 


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

void spi_setup(int clk, int cs, int mosi ){ 

    pinMode(clk, OUTPUT);
    pinMode(cs, OUTPUT);
    pinMode(mosi, OUTPUT);
    digitalWrite(clk, LOW);
    delay(1);
    digitalWrite(cs, HIGH);
    delay(1);
    digitalWrite(mosi, LOW);
    delay(1);

}   

// AER
/*void aeroISR() {
  if (!aero.reqRead()) {
    if (aero.get_index() >= tx_buf_size - 2) { // buffer full
      setHeader(5, aero.get_index());
      for (int i = 0; i < aero.get_index(); i++)
      {
        tx_buf[2+i] = aero_buf[i];
      }
      usb_serial_write(tx_buf, 2 + aero.get_index());
      aero.set_index(0);
    }
    aero.record_event();
    aero.ackWrite(0);
  }
  if (aero.reqRead()) {
    aero.ackWrite(1);
  }
}*/



// TEENSY 2 PC USB message
void sendStatus(TeensyMessage msg) {
  msg_buf[0] = (uint8_t) msg;
  usb_serial_write(msg_buf, sizeof(msg_buf));
} // Success is 0
