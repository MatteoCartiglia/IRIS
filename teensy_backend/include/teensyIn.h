//---------------------------------------------------------------------------------------------------------------------------------------
//
//
// Author: Matteo Cartiglia <camatteo@ini.uzh.ch>
// Last updated: (Ciara Giles-Doran <gciara@student.ethz.ch>)
//---------------------------------------------------------------------------------------------------------------------------------------

#ifndef TEENSYIN_H
#define TEENSYIN_H

#include <Arduino.h>

class TeensyIn 
{
    // --------------------------------------------------- Declaring class constructor --------------------------------------------------
    public:

    TeensyIn(const int inputReqPin, const int inputAckPin, int inputDataPins[], int inputNumDataPins, int inputType, byte inputBuffer[] = NULL, 
        int inputDelay = 0, bool inputActiveLow = false);

    //
    unsigned int dataRead();

    //
    bool reqRead();

    //
    void ackWrite(bool val);


    // ---------------------------------------------------- Declaring private methods --------------------------------------------------
    private:

    //
    void setupEncoderComms();

    //
    void setupC2FComms();

    //
    unsigned int getInputData();


    // --------------------------------------------------- Declaring private variables -------------------------------------------------
    int _inputReqPin;
    int _inputAckPin;
    int* _inputDataPins;
    int _inputNumDataPins;
    byte* _inputBuffer;
    int _inputDelay;
    bool _inputActiveLow;

};

#endif