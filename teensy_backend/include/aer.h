//---------------------------------------------------------------------------------------------------------------------------------------
//
//
// Author: Matteo Cartiglia <camatteo@ini.uzh.ch>
// Last updated: (Ciara Giles-Doran <gciara@student.ethz.ch>)
//---------------------------------------------------------------------------------------------------------------------------------------

#ifndef AER_H
#define AER_H

#include <Arduino.h>

class AER 
{
  // --------------------------------------------------- Declaring class constructors --------------------------------------------------
  public:

    AER(const int inputReqPin, const int inputAckPin, int inputDataPins[], int inputNumDataPins, byte inputBuffer[] = NULL, int inputDelay = 0, 
        int inputTimestampShift = 0, bool inputActiveLow = true);

    AER(const int outputReqPin, const int outputAckPin, int outputDataPins[], int outputNumDataPins, int outputDelay = 0, bool outputActiveLow = true);

    
    // -------------------------------------------------- Declaring "setting up" methods ----------------------------------------------

    //
    void setupDecoderComms();

    //
    void setupEncoderComms();

    //
    void setupC2FComms();


    // ------------------------------------------------ Declaring AER read public methods ---------------------------------------------

    //
    bool reqRead();

    //
    bool ackRead();

    //
    unsigned int dataRead();

    //
    bool dataRead_handshake();


    // --------------------------------------------------- Declaring AER write public methods ----------------------------------------

    //
    void reqWrite(bool val);
    
    //
    void ackWrite(bool val);

    //
    void dataWrite(unsigned int data);

    //
    bool dataWrite_handshake(unsigned int data);


    // -------------------------------------------------- Declaring AER record public methods ------------------------------------------

    //
    unsigned int record_event();

    //
    void record_event_manual(unsigned int x);

    //
    bool record_event_handshake();


    // ----------------------------------------------------- Declaring getters and setters --------------------------------------------

    //
    int get_index();

    //
    void set_index(int x);

    //
    void set_t0(int t0);
 

  // --------------------------------------------------- Declaring private variables -------------------------------------------------
  private:

  // Input variables
  int _inputReqPin;
  int _inputAckPin;
  int* _inputDataPins;
  int _inputNumDataPins;
  byte* _inputBuffer;
  int _inputDelay;
  int _inputTimestampShift;
  bool _inputActiveLow;
  int _inputIndex;
  int _inputT0;

  // Output variables
  int _outputReqPin;
  int _outputAckPin;
  int* _outputDataPins;
  int _outputNumDataPins;
  int _outputDelay;
  bool _outputActiveLow;

};

#endif
