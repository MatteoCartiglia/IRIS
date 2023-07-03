//---------------------------------------------------------------------------------------------------------------------------------------
// Header file for TeensyOut class
//---------------------------------------------------------------------------------------------------------------------------------------

#ifndef TEENSYOUT_H
#define TEENSYOUT_H

#include <Arduino.h>
#include "../../teensy_backend/include/constants.h"

class TeensyOut
{
    // ------------------------------------------ Declaring class constructor and public methods ----------------------------------------

    public:

    //-----------------------------------------------------------------------------------------------------------------------------------
    // Class constructor; initialises the TeensyOut object and sets up the relevant pins on Teensy
    //-----------------------------------------------------------------------------------------------------------------------------------
    TeensyOut(int outputReqPin, int outputAckPin, const int outputDataPins[], int outputNumDataPins, int outputDelay = 0, bool outputActiveLow = false);

    //----------------------------------------------------------------------------------------------------------------------------------
    // dataWrite: Executes REQ/ACK handshake and writes output to ALIVE
    //----------------------------------------------------------------------------------------------------------------------------------
    bool dataWrite(unsigned int data);


    // ---------------------------------------------------- Declaring private methods --------------------------------------------------

    private:

    //----------------------------------------------------------------------------------------------------------------------------------
    // setupPins: Sets up the relevant pins for communication
    //----------------------------------------------------------------------------------------------------------------------------------
    void setupPins();

    //----------------------------------------------------------------------------------------------------------------------------------
    // ackRead: Reads ACK pin state
    //---------------------------------------------------------------------------------------------------------------------------------- 
    bool ackRead();

    //----------------------------------------------------------------------------------------------------------------------------------
    // reqWrite: Writes to REQ pin
    //----------------------------------------------------------------------------------------------------------------------------------
    void reqWrite(bool val);
    
    //----------------------------------------------------------------------------------------------------------------------------------
    // setOutputData: Write data to output pins
    //----------------------------------------------------------------------------------------------------------------------------------
    void setOutputData(unsigned int data);


    // --------------------------------------------------- Declaring private variables -------------------------------------------------

    int _outputReqPin;
    int _outputAckPin;
    const int* _outputDataPins;
    int _outputNumDataPins;
    int _outputDelay;
    bool _outputActiveLow;
};

#endif