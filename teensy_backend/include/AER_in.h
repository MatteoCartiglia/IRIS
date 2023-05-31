//---------------------------------------------------------------------------------------------------------------------------------------
// Header file for TeensyIn class
//---------------------------------------------------------------------------------------------------------------------------------------

#ifndef AER_in_H
#define AER_in_H

#include <Arduino.h>
#include "../include/datatypes.h"

class AER_in
{
    // ------------------------------------------ Declaring class constructor and public methods ----------------------------------------

public:
    //-----------------------------------------------------------------------------------------------------------------------------------
    // Class constructor; initialises the AER_in object and sets up the relevant pins on Teensy
    //-----------------------------------------------------------------------------------------------------------------------------------
    AER_in(int inputReqPin, int inputAckPin, int inputDataPins[], int inputNumDataPins, int inputDelay = 0,
           bool inputActiveLow = false, bool inputDataActiveLow = false);

    //----------------------------------------------------------------------------------------------------------------------------------
    // reqRead: Reads REQ pin state
    //----------------------------------------------------------------------------------------------------------------------------------
    bool reqRead();

    //----------------------------------------------------------------------------------------------------------------------------------
    // ackWrite: Writes to ACK pin
    //----------------------------------------------------------------------------------------------------------------------------------
    void ackWrite(bool val);

    //----------------------------------------------------------------------------------------------------------------------------------
    // getBufferIndex: Retreives the current index of the buffer
    //----------------------------------------------------------------------------------------------------------------------------------
    int getBufferIndex();

    //----------------------------------------------------------------------------------------------------------------------------------
    // setBufferIndex: Retreives the current index of the buffer
    //----------------------------------------------------------------------------------------------------------------------------------
    void setBufferIndex(int x);

    //----------------------------------------------------------------------------------------------------------------------------------
    // recordEvent: Records ALIVE output events as they occur
    //----------------------------------------------------------------------------------------------------------------------------------
    void recordEvent();

    //----------------------------------------------------------------------------------------------------------------------------------
    // sendEventBuffer: Sends ALIVE output data saved in buffer to Teensy
    //----------------------------------------------------------------------------------------------------------------------------------
    void sendEventBuffer();

    //----------------------------------------------------------------------------------------------------------------------------------
    // handshake: Executes REQ/ACK handshake between Teensy and ALIVE
    //----------------------------------------------------------------------------------------------------------------------------------
    void handshake();

    //----------------------------------------------------------------------------------------------------------------------------------
    // set_t0 : Sets the initial time
    //----------------------------------------------------------------------------------------------------------------------------------
    void set_t0(int t0);

    //----------------------------------------------------------------------------------------------------------------------------------
    // toggle_saving_flag : toogle the saving flag
    //----------------------------------------------------------------------------------------------------------------------------------
    void toggle_saving_flag();
    bool saving_flag;

    // ---------------------------------------------------- Declaring private methods --------------------------------------------------
private:
    //----------------------------------------------------------------------------------------------------------------------------------
    // setupPins: Sets up the relevant pins for communication
    //----------------------------------------------------------------------------------------------------------------------------------
    void setupPins();

    //----------------------------------------------------------------------------------------------------------------------------------
    // getInputData: Retrieves input from ALIVE
    //----------------------------------------------------------------------------------------------------------------------------------
    unsigned int getInputData();

    //----------------------------------------------------------------------------------------------------------------------------------
    // resetBuffer: (Re)Initialises the ALIVE output buffer and (re)sets buffer index counter variable
    //----------------------------------------------------------------------------------------------------------------------------------
    void resetBuffer();

    // --------------------------------------------------- Declaring private variables -------------------------------------------------

    int _inputReqPin;
    int _inputAckPin;
    int *_inputDataPins;
    int _inputNumDataPins;
    int _inputDelay;
    bool _inputHandshakeActiveLow;
    bool _inputDataActiveLow;

    unsigned long _t0;

    int8_t _inputBufferIndex;
    AER_out _inputEventBuffer[MAX_EVENTS_PER_PACKET] = {};
};

#endif