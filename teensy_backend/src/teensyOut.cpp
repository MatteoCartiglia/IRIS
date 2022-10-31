//---------------------------------------------------------------------------------------------------------------------------------------
// Source file for TeensyOut class
//---------------------------------------------------------------------------------------------------------------------------------------

#include "teensyOut.h"
#include "constants.h"
#include "constants_global.h"

//---------------------------------------------------------------------------------------------------------------------------------------
// Class constructor; initialises the TeensyOut object and sets up the relevant pins on Teensy
//---------------------------------------------------------------------------------------------------------------------------------------

TeensyOut::TeensyOut(const int outputReqPin, const int outputAckPin, int outputDataPins[], int outputNumDataPins, int outputDelay, bool outputActiveLow)
{
  _outputReqPin = outputReqPin;
  _outputAckPin = outputAckPin;
  _outputDataPins = outputDataPins;
  _outputNumDataPins = outputNumDataPins;
  _outputDelay = outputDelay;
  _outputActiveLow = outputActiveLow;

  setupPins();
}

//---------------------------------------------------------------------------------------------------------------------------------------
// dataWrite: Executes REQ/ACK handshake and writes output to ALIVE
//---------------------------------------------------------------------------------------------------------------------------------------

bool TeensyOut::dataWrite(unsigned int data) 
{
  unsigned long t0 = millis();
  bool handshakeStatus = true;

  setOutputData(data);
  reqWrite(1);

  while(!ackRead())
  {
    if (millis() > t0 + TEENSY_OUTPUT_HANDSHAKE_TIMEOUT)
    {
      handshakeStatus = false;
      break;
    }
  }

  reqWrite(0);

  return handshakeStatus;
}

//---------------------------------------------------------------------------------------------------------------------------------------
// setupPins: Sets up the relevant pins for communication
//---------------------------------------------------------------------------------------------------------------------------------------

void TeensyOut::setupPins()
{
  pinMode(_outputReqPin, OUTPUT);
  pinMode(_outputAckPin, INPUT);

  for(int i = 0; i < _outputNumDataPins; i++)
  {
    pinMode(_outputDataPins[i], OUTPUT);
  }
}


//---------------------------------------------------------------------------------------------------------------------------------------
// ackRead: Reads ACK pin state
//---------------------------------------------------------------------------------------------------------------------------------------

bool TeensyOut::ackRead() 
{
  return digitalReadFast(_outputAckPin)^_outputActiveLow;
}


//---------------------------------------------------------------------------------------------------------------------------------------
// reqWrite: Writes to REQ pin
//---------------------------------------------------------------------------------------------------------------------------------------

void TeensyOut::reqWrite(bool val) 
{
  digitalWriteFast(_outputReqPin, val^_outputActiveLow);

  if (_outputDelay) 
  {
    delayMicroseconds(_outputDelay);
  }
}


//---------------------------------------------------------------------------------------------------------------------------------------
// setOutputData: Write data to output pins
//---------------------------------------------------------------------------------------------------------------------------------------

void TeensyOut::setOutputData(unsigned int data) 
{
  for (int i=0; i<_outputNumDataPins; i++) 
  {
    bool bit = bitRead(data, i)^_outputActiveLow;
    digitalWriteFast(_outputDataPins[i], bit);
  }
}
