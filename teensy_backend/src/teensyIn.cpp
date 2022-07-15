//---------------------------------------------------------------------------------------------------------------------------------------
//
//
// Author: Matteo Cartiglia <camatteo@ini.uzh.ch>
// Last updated: (Ciara Giles-Doran <gciara@student.ethz.ch>)
//---------------------------------------------------------------------------------------------------------------------------------------

#include "teensyIn.h"
#include "constants.h"

//---------------------------------------------------------------------------------------------------------------------------------------
// TeensyIn constructor
//---------------------------------------------------------------------------------------------------------------------------------------

TeensyIn::TeensyIn(const int inputReqPin, const int inputAckPin, int inputDataPins[], int inputNumDataPins, int inputType, byte inputBuffer[], 
        int inputDelay, bool inputActiveLow)
{
  _inputReqPin = inputReqPin;
  _inputAckPin = inputAckPin;
  _inputDataPins = inputDataPins;
  _inputNumDataPins = inputNumDataPins;
  _inputBuffer = inputBuffer;
  _inputDelay = inputDelay;
  _inputActiveLow = inputActiveLow;

  pinMode(_inputAckPin, OUTPUT);
  pinMode(_inputReqPin, INPUT);

  if(inputType == TEENSY_INPUT_C2F)
  {
    setupC2FComms();
  }
  else if(inputType == TEENSY_INPUT_ENCODER)
  {
    setupEncoderComms();
  }
  else
  {
    printf("Teensy input setup unsuccessful. Unknown input type.");
  }
}


//---------------------------------------------------------------------------------------------------------------------------------------
// dataRead: Executes REQ/ACK handshake and retrieves input from ALIVE
//---------------------------------------------------------------------------------------------------------------------------------------

unsigned int TeensyIn::dataRead()
{
    unsigned int inputData = 0;

    if (reqRead())
    {
      inputData = getInputData();
      ackWrite(1);
    }

    if(!reqRead())
    {
      ackWrite(0);
    }

    return inputData;
}


//---------------------------------------------------------------------------------------------------------------------------------------
// reqRead: Reads REQ pin state
//---------------------------------------------------------------------------------------------------------------------------------------

bool TeensyIn::reqRead() 
{
  return digitalReadFast(_inputReqPin)^_inputActiveLow;
}


//---------------------------------------------------------------------------------------------------------------------------------------
// ackWrite: Writes to ACK pin
//---------------------------------------------------------------------------------------------------------------------------------------

void TeensyIn::ackWrite(bool val) 
{
  digitalWriteFast(_inputAckPin, val^_inputActiveLow);

  if (_inputDelay) 
  {
    delayMicroseconds(_inputDelay);
  }
}


//---------------------------------------------------------------------------------------------------------------------------------------
// setupEncoderComms: Sets up the relevant pins for Encoder comms on Teensy
//---------------------------------------------------------------------------------------------------------------------------------------

void TeensyIn::setupEncoderComms()
{
  for(int i = 0; i < _inputNumDataPins; i++)
  {
    pinMode(_inputDataPins[i], INPUT);
  }
}


//---------------------------------------------------------------------------------------------------------------------------------------
// setupC2FComms: Sets up the relevant pins for C2F comms on Teensy
//---------------------------------------------------------------------------------------------------------------------------------------

void TeensyIn::setupC2FComms()
{
  pinMode(P_RST_PIN, OUTPUT);
  pinMode(S_RST_PIN, OUTPUT);
  delay(5);

  digitalWrite(P_RST_PIN, LOW);
  delay(1);
  digitalWrite(S_RST_PIN, LOW);
  delay(1);
  digitalWrite(P_RST_PIN, HIGH);
  delay(1);
  digitalWrite(S_RST_PIN, HIGH);
  delay(1);
}


//---------------------------------------------------------------------------------------------------------------------------------------
// getInputData: Retrieves input from ALIVE
//---------------------------------------------------------------------------------------------------------------------------------------

unsigned int TeensyIn::getInputData() 
{
  unsigned int inputData = 0;

  for (int i = 0; i < _inputNumDataPins; i++) 
  {
    inputData |= digitalReadFast(_inputDataPins[i]) << i;
  }

  if (_inputActiveLow) 
  {
    return ~inputData;
  }
  else 
  {
    return inputData;
  }
}