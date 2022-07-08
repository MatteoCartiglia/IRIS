//---------------------------------------------------------------------------------------------------------------------------------------
//
//
// Author: Matteo Cartiglia <camatteo@ini.uzh.ch>
// Last updated: 
//---------------------------------------------------------------------------------------------------------------------------------------

#include <Arduino.h>

#include "constants.h"
#include "datatypes.h"
#include "biasGen.h"
#include "dac.h"
#include "aer.h"


// Declaring function prototypes in order of definition


static void aeroISR();
void sendStatus(TeensyStatus msg);
void transmitAnyAerEvents();

// Defining global variables

uint8_t msg_buf[1];

elapsedMicros since_blank_micro;
elapsedMillis since_blank_milli;

int AERO_DATA[AERO_NUM_DATA_PINS] = {26,27,28};
AER_out aero_buf[AERO_BUFF_SIZE];
AER_in aero(AERO_REQ,
            AERO_ACK,
            AERO_DATA,
            AERO_NUM_DATA_PINS,
            aero_buf,
            AER_DELAYS,
            10,
            false);

//------------------------------------------------------- Defining Global Variables ------------------------------------------------------ 

static bool aerOUT;
static P2TPkt inputBuffer;

DAC dac{DAC_RESET, DAC_A0, DAC_A1};
BiasGen biasGen{BIASGEN_SCK_PIN , BIASGEN_SLAVE_SPI0 , BIASGEN_MOSI_PIN, BIASGEN_ENABLE_PIN};

//---------------------------------------------------------------------------------------------------------------------------------------
// "setup" function: 
//---------------------------------------------------------------------------------------------------------------------------------------

void setup() 
{
    biasGen.setupBiasGen();

    aerOUT = false;
    attachInterrupt(digitalPinToInterrupt(AERO_REQ), aeroISR, CHANGE);

    dac.setup_dacs();
    dac.join_i2c_bus();
    dac.turn_reference_off();

    Serial.print("********************* Setup complete *********************\n");
}

//---------------------------------------------------------------------------------------------------------------------------------------
// "main" function: reads in serial communication and 
//---------------------------------------------------------------------------------------------------------------------------------------

void loop() 
{    
   if (usb_serial_available()) 
   {
        // Read the data in the input buffer
        usb_serial_read(&inputBuffer, sizeof(inputBuffer));

        // Interpret the input data
        switch ((P2tPktType)inputBuffer.header) 
        {
               


            case P2tPktType::P2tSetBiasGen:{  // BiasGen
                Serial.print("Biasgen command recieved \n");
                BIASGEN_command BG (rx_buf);
                int bg_val = ( (BG.course_val << COURSE_BIAS_SHIFT ) | (BG.fine_val << FINE_BIAS_SHIFT ) | BG.transistor_type );
                SPI_events(0, BG.address, bg_val);
                Serial.print("Biasgen command done  \n");
                // sendStatus(TeensyStatus::Success);
                break;
            }

            case P2tPktType::P2tSetDcVoltage :{  // DAC
                DAC_command DAC(rx_buf);
                dac.write_dacs(DAC.command_address, DAC.data); 
                delay(100);
                Serial.print("DAC command recieved \n");
                sendStatus(TeensyStatus::Success);
                break;   
            }

            case P2tPktType::P2tSetSPI: { // SPIs
                Serial.print("SPI command recieved \n");
                SPI_command SPI  (rx_buf);
                SPI_events(SPI.spi_number , SPI.address, SPI.value);
                // sendStatus(TeensyStatus::Success);
                break;
            }

            case  P2tPktType::P2tRequestAerOutput: { // aer
                // Serial.print("AER activated \n");
                aero.set_index(0);
                aero.set_t0(micros());
                since_blank_milli = 0;
                aero_flag = true;

                delay(100);
                AER_out tmp;
                tmp.address = 10;
                tmp.ts_1ms = 0b0000000011111111;
                P2TPkt aero_pkt(tmp);
               // Serial.print(aero_pkt.header);
                usb_serial_write((const void*) &aero_pkt, sizeof(aero_pkt));
              //  Serial.print("\n");

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



//---------------------------------------------------------------------------------------------------------------------------------------
// aeroISR  
//---------------------------------------------------------------------------------------------------------------------------------------

static void aeroISR()
{
    if (!aero.reqRead()) {
        if (aero_flag  && (aero.get_index() < AERO_BUFF_SIZE) ) { 
            aero.record_event();
        }
        aero.ackWrite(0);
        if (aero.reqRead()){
            aero.ackWrite(1);
        }
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------
// sendStatus  
//---------------------------------------------------------------------------------------------------------------------------------------

void sendStatus(TeensyStatus msg) 
{
  msg_buf[0] = (uint8_t) msg;
  usb_serial_write(msg_buf, sizeof(msg_buf));
} // Success is 0


//interrupt handler run all the time and through out or put into buffer (flag if full) main loop empties this buffer
// sending events to the chip: number 1 without isi number two with buffer and timer interupt 

//---------------------------------------------------------------------------------------------------------------------------------------
// transmitAnyAerEvents  
//---------------------------------------------------------------------------------------------------------------------------------------

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