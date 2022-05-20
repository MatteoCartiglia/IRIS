#include <Arduino.h>
#include <SPI.h>
#include "ncs_teensy.h"
#include "dac.h"
#include "aer_in.h"
#include "teensy_interface.h"
#include <Wire.h>


static P2TPkt rx_buf;
static bool aero_flag;
AER_out aero_buf[aero_buf_size];
uint8_t msg_buf[1];


void spi_controller(SPIClass SPI, int cs, int address, int value);
void SPI_events(int spi_number, int address, int value);
void spi_setup(int clk, int cs, int mosi );

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
int aero_duration = 15; 

elapsedMicros since_blank_micro;
elapsedMillis since_blank_milli;


//interrupt handler run all the time and through out or put into buffer (flag if full) main loop empties this buffer
// sending events to the chip: number 1 without isi number two with buffer and timer interupt 



void transmitAnyAerEvents() 
{
    if ( aero_flag &&  aero.get_index() ) {
        for (int i = 0; i < aero.get_index(); ++i) {
            P2TPkt aero_pkt(aero_buf[i]);
            usb_serial_write((const void*) &aero_pkt, sizeof(aero_pkt));
        }
        since_blank_milli = 0;
        aero.set_index(0);
    }
}


void SPI_events(int spi_number, int address, int value)
{
  switch(spi_number)
  {
    case 0:
        bg_controller(SPI, slaveSelectPin_SPI_BGEN, address, value);
    case 1:
        spi_controller(SPI1, slaveSelectPin_SPI1_CRST,address, value);
    case 2:
        spi_controller(SPI2, slaveSelectPin_SPI2_VRST, address, value);

    default:
    break;
  }

}
void bg_controller (SPIClass SPI, int cs, int address, int value)
{
    SPI.begin();
    SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
    digitalWrite(cs,LOW);
    uint8_t add = (address  >> 8 ) & 0xFF ; 
    uint8_t add2 = address  & 0xFF ; 
    uint8_t val = (value  >> 8 ) & 0xFF ; 
    uint8_t val2 = value  & 0xFF ; 

    SPI.transfer(add);
    SPI.transfer(add2);
    SPI.transfer(val);
    SPI.transfer(val2);

    digitalWrite(cs,HIGH);
    SPI.endTransaction();

    
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
static void aeroISR()
{
    if (!aero.reqRead()) {
        if (aero_flag  && (aero.get_index() < aero_buf_size) ) { 
            aero.record_event();
        }
        aero.ackWrite(0);
        if (aero.reqRead()){
            aero.ackWrite(1);
        }
    }
}


// TEENSY 2 PC USB message
void sendStatus(TeensyStatus msg) {
  msg_buf[0] = (uint8_t) msg;
  usb_serial_write(msg_buf, sizeof(msg_buf));
} // Success is 0


void setup() {

    if (BIAS_GEN) spi_setup(BGSCK , slaveSelectPin_SPI_BGEN , BGMOSI);
    if (SPI1_ON) spi_setup(CSCK , slaveSelectPin_SPI1_CRST , CMOSI);
    if (SPI2_ON) spi_setup(VSCK , slaveSelectPin_SPI2_VRST , VMOSI);
    
    aero_flag = false;
    attachInterrupt(digitalPinToInterrupt(aero_req), aeroISR, CHANGE);

    dac.setup_dacs();
    dac.join_i2c_bus();
    dac.turn_reference_off();
    Serial.print("SETUP COMPLETE!");

    }


void loop() {
   if (usb_serial_available()) {
        usb_serial_read(&rx_buf, sizeof(rx_buf)); // in bytes. 1 byte is 8 bits.

        switch ((P2tPktType)rx_buf.header) {

            case P2tPktType::P2tReset:{ // reset
                Serial.print("Reset recieved");
               // sendStatus(TeensyStatus::Success);
                break;
            }
                
               
            case P2tPktType::P2tSetBiasGen:{  // BiasGen
                Serial.print("Biasgen command recieved \n");
                BIASGEN_command BG (rx_buf);
                Serial.print(BG.course_val);
                Serial.print(BG.fine_val);
                Serial.print(BG.address);
                Serial.print("\n");
                int bg_val = ( BG.course_val <<9 & BG.fine_val << 1  & BG.transistor_type);
                SPI_events(0,BG.address,bg_val );
                Serial.print("Biasgen command done  \n");

              //  sendStatus(TeensyStatus::Success);
                break;
            }
            case P2tPktType::P2tSetDcVoltage :{  // DAC
                DAC_command DAC( rx_buf);
                dac.write_dacs(DAC.command_address, DAC.data); 
                delay(100);
                Serial.print("DAC command recived \n");
                sendStatus(TeensyStatus::Success);
                break;   
            }
            case P2tPktType::P2tSetSPI: { // SPIs
                Serial.print("SPI command recieved \n");
                SPI_command SPI  (rx_buf);
                SPI_events(SPI.spi_number , SPI.address, SPI.value);
              //  sendStatus(TeensyStatus::Success);
                break;
            }
            case  P2tPktType::P2tRequestAerOutput: { // aer
  
                aero.set_index(0);
                aero.set_t0(micros());
                since_blank_milli = 0;
                aero_flag = true;
                break;
            }
            case P2tPktType::P2tGetTeensySN:{  // SN
                break;       
            } 

            case P2tPktType::P2tSendEvents:{ // II     
                break;     
            }   
            default: 
                sendStatus(TeensyStatus::UnknownCommand);
                break;  
        } 
   }
    transmitAnyAerEvents();
} ;
