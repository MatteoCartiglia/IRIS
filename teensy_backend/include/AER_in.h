//---------------------------------------------------------------------------------------------------------------------------------------
// Header file for TeensyIn class
//---------------------------------------------------------------------------------------------------------------------------------------

#ifndef AER_IN_H
#define AER_IN_H

#include <Arduino.h>
#include "../include/datatypes.h"

class AER_in
{
    // ------------------------------------------ Declaring class constructor and public methods ----------------------------------------

public:
    //-----------------------------------------------------------------------------------------------------------------------------------
    // Class constructors
    // The first initialises the AER_in object in the case that data is to be read from a set of pins on the Teensy given by inputDataPins[].
    // The second provides a callback function which the class will use in place of its private getInputData() method to be able to read
    // data by any arbitrary, caller defined method in case the data bus in not connected directly to the Teensy.
    // In both cases REQ and ACK must be connected to the Teensy directly.
    // And in both cases, the constructor calls pinMode to set up the pin direction for REQ and ACK, and in the case of the first
    // constructor for the data pins too, despite the fact that this might not be a good thing to do in terms of timing, see Issue #38.
    //-----------------------------------------------------------------------------------------------------------------------------------
    AER_in(int inputReqPin, int inputAckPin, int inputDataPins[], int inputNumDataPins, int inputDelay = 0,
           bool inputHandshakeActiveLow = false, bool inputDataActiveLow = false);
           
    AER_in(int inputReqPin, int inputAckPin, unsigned int (*readCallback)(void *), void *callbackData, int inputDelay = 0,
           bool inputHandshakeActiveLow = false);

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
    void commonConstruction();

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
    unsigned int (*_readCallback)(void *);
    void *_callbackData;

    unsigned long _t0;

    int8_t _inputBufferIndex;
    AER_out _inputEventBuffer[MAX_EVENTS_PER_PACKET] = {};
};

#endif