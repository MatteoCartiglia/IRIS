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
static void aerInputEncoder_ISR();
static void aerInputC2F_ISR();
void sendTeensyStatus(TeensyStatus status);

// void transmitAnyAerEvents();
// elapsedMillis since_blank_milli;

//------------------------------------------------------- Defining Global Variables ------------------------------------------------------ 

static P2TPkt inputBuffer;
bool aerInputEncoder_bufferActive = false;
bool aerInputC2F_bufferActive = false;

int aerInputEncoder_dataPins[AER_ENCODER_INPUT_NO_PIN] = {AER_ENCODER_INPUT_BIT_0_PIN, AER_ENCODER_INPUT_BIT_1_PIN, AER_ENCODER_INPUT_BIT_2_PIN};
int aerInputC2F_dataPins[C2F_INPUT_NO_PIN] = {C2F_INPUT_BIT_0_PIN, C2F_INPUT_BIT_1_PIN, C2F_INPUT_BIT_2_PIN, C2F_INPUT_BIT_3_PIN, C2F_INPUT_BIT_4_PIN};
int aerOutputDecoder_dataPins[AER_DECODER_OUTPUT_NO_PIN] = {AER_DECODER_OUTPUT_BIT_0_PIN, AER_DECODER_OUTPUT_BIT_1_PIN, AER_DECODER_OUTPUT_BIT_2_PIN, AER_DECODER_OUTPUT_BIT_3_PIN,
                            AER_DECODER_OUTPUT_BIT_4_PIN, AER_DECODER_OUTPUT_BIT_5_PIN, AER_DECODER_OUTPUT_BIT_6_PIN, AER_DECODER_OUTPUT_BIT_7_PIN, AER_DECODER_OUTPUT_BIT_8_PIN};

AER aerInputEncoder(AER_ENCODER_REQ, AER_ENCODER_ACK, aerInputEncoder_dataPins, AER_ENCODER_INPUT_NO_PIN, NULL, AER_HANDSHAKE_DELAY, AER_RECORD_EVENT_TSTAMP_SHIFT, false);
AER aerInputC2F(C2F_REQ, C2F_ACK, aerInputC2F_dataPins, C2F_INPUT_NO_PIN, NULL, AER_HANDSHAKE_DELAY, AER_RECORD_EVENT_TSTAMP_SHIFT, false);
AER aerOutputDecoder(AER_DECODER_REQ, AER_DECODER_ACK, aerOutputDecoder_dataPins, AER_DECODER_OUTPUT_NO_PIN, AER_HANDSHAKE_DELAY, false);

DAC dac{DAC_RESET, DAC_A0, DAC_A1};
BiasGen biasGen{BIASGEN_SCK_PIN , BIASGEN_SLAVE_SPI0 , BIASGEN_MOSI_PIN, BIASGEN_ENABLE_PIN};

//---------------------------------------------------------------------------------------------------------------------------------------
// "setup" function: 
//---------------------------------------------------------------------------------------------------------------------------------------

void setup() 
{
    biasGen.setupBiasGen();

    attachInterrupt(digitalPinToInterrupt(AER_ENCODER_REQ), aerInputEncoder_ISR, CHANGE);
    attachInterrupt(digitalPinToInterrupt(C2F_REQ), aerInputC2F_ISR, CHANGE);

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
        //     case P2tPktType::P2tSetBiasGen:{  // BiasGen
        //         Serial.print("Biasgen command recieved \n");
        //         BIASGEN_command BG (rx_buf);
        //         int bg_val = ( (BG.course_val << COURSE_BIAS_SHIFT ) | (BG.fine_val << FINE_BIAS_SHIFT ) | BG.transistor_type );
        //         SPI_events(0, BG.address, bg_val);
        //         Serial.print("Biasgen command done  \n");
        //         // sendStatus(TeensyStatus::Success);
        //         break;
        //     }

        //     case P2tPktType::P2tSetDcVoltage :{  // DAC
        //         DAC_command DAC(rx_buf);
        //         dac.write_dacs(DAC.command_address, DAC.data); 
        //         delay(100);
        //         Serial.print("DAC command recieved \n");
        //         sendStatus(TeensyStatus::Success);
        //         break;   
        //     }

        //     case  P2tPktType::P2tRequestAerOutput: { // aer
        //         // Serial.print("AER activated \n");
        //         aero.set_index(0);
        //         aero.set_t0(micros());
        //         since_blank_milli = 0;
        //         aero_flag = true;

        //         delay(100);
        //         AER_out tmp;
        //         tmp.address = 10;
        //         tmp.ts_1ms = 0b0000000011111111;
        //         P2TPkt aero_pkt(tmp);
        //        // Serial.print(aero_pkt.header);
        //         usb_serial_write((const void*) &aero_pkt, sizeof(aero_pkt));
        //       //  Serial.print("\n");

        //         break;
        //     }

            default: 
                sendTeensyStatus(TeensyStatus::UnknownCommand);
                break;  
        } 
   }
    // transmitAnyAerEvents();
} ;



//---------------------------------------------------------------------------------------------------------------------------------------
// AER Input Interrupt Servie Routine (ISR) for encoder input
//---------------------------------------------------------------------------------------------------------------------------------------

static void aerInputEncoder_ISR()
{
    if (!aerInputEncoder.reqRead()) 
    {
        if (aerInputEncoder_bufferActive  && (aerInputEncoder.get_index() < AER_BUFFER_SIZE) ) 
        { 
            aerInputEncoder.record_event();
        }

        aerInputEncoder.ackWrite(0);

        if (aerInputEncoder.reqRead())
        {
            aerInputEncoder.ackWrite(1);
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------
// AER Input Interrupt Servie Routine (ISR) for C2F input
//---------------------------------------------------------------------------------------------------------------------------------------

static void aerInputC2F_ISR()
{
    if (!aerInputC2F.reqRead()) 
    {
        if (aerInputC2F_bufferActive  && (aerInputC2F.get_index() < AER_BUFFER_SIZE) ) 
        { 
            aerInputC2F.record_event();
        }

        aerInputC2F.ackWrite(0);

        if (aerInputC2F.reqRead())
        {
            aerInputC2F.ackWrite(1);
        }
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------
// sendStatus  
//---------------------------------------------------------------------------------------------------------------------------------------

static void sendTeensyStatus(TeensyStatus status)
{
    std::uint8_t teensyStatusBuffer[1] = {(uint8_t) status};
    usb_serial_write(teensyStatusBuffer, sizeof(teensyStatusBuffer));
}



//interrupt handler run all the time and through out or put into buffer (flag if full) main loop empties this buffer
// sending events to the chip: number 1 without isi number two with buffer and timer interupt 

//---------------------------------------------------------------------------------------------------------------------------------------
// transmitAnyAerEvents  
//---------------------------------------------------------------------------------------------------------------------------------------

// void transmitAnyAerEvents() 
// {
//     if ( aero_flag &&  aero.get_index() ) {
//         for (int i = 0; i < aero.get_index(); ++i) {
//             P2TPkt aero_pkt(aero_buf[i]);
//             usb_serial_write((const void*) &aero_pkt, sizeof(aero_pkt));
//         }
//         since_blank_milli = 0;
//         aero.set_index(0);
//     }
// }