//---------------------------------------------------------------------------------------------------------------------------------------
//
//
// Author: Matteo Cartiglia <camatteo@ini.uzh.ch>
// Last updated: (Ciara Giles-Doran <gciara@student.ethz.ch>)
//---------------------------------------------------------------------------------------------------------------------------------------

#ifndef TEENSYOUT_H
#define TEENSYOUT_H

#include <Arduino.h>

class TeensyOut
{
    // --------------------------------------------------- Declaring class constructor --------------------------------------------------
    public:

    TeensyOut(const int outputReqPin, const int outputAckPin, int outputDataPins[], int outputNumDataPins, int outputDelay = 0, bool outputActiveLow = false);

    //
    bool dataWrite(unsigned int data);


    // ---------------------------------------------------- Declaring private methods --------------------------------------------------
    private:

    //
    void setupDecoderComms();

    //
    bool ackRead();

    //
    void reqWrite(bool val);
    
    //
    void setOutputData(unsigned int data);


    // --------------------------------------------------- Declaring private variables -------------------------------------------------
    int _outputReqPin;
    int _outputAckPin;
    int* _outputDataPins;
    int _outputNumDataPins;
    int _outputDelay;
    bool _outputActiveLow;

};

#endif