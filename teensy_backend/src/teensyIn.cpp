//---------------------------------------------------------------------------------------------------------------------------------------
//
//
// Author: Matteo Cartiglia <camatteo@ini.uzh.ch>
// Last updated: (Ciara Giles-Doran <gciara@student.ethz.ch>)
//---------------------------------------------------------------------------------------------------------------------------------------

#include "teensyIn.h"
#include "constants.h"
#include "datatypes.h"

//---------------------------------------------------------------------------------------------------------------------------------------
// TeensyIn constructor
//---------------------------------------------------------------------------------------------------------------------------------------

TeensyIn::TeensyIn(const int inputReqPin, const int inputAckPin, int inputDataPins[], int inputNumDataPins, int inputDelay, bool inputActiveLow)
{
  _inputReqPin = inputReqPin;
  _inputAckPin = inputAckPin;
  _inputDataPins = inputDataPins;
  _inputNumDataPins = inputNumDataPins;
  _inputDelay = inputDelay;
  _inputActiveLow = inputActiveLow;

  resetBuffer();
  setupPins();
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


//----------------------------------------------------------------------------------------------------------------------------------
// getBufferIndex: Retreives the current index of the buffer
//----------------------------------------------------------------------------------------------------------------------------------

int TeensyIn::getBufferIndex()
{
  return _inputBufferIndex;
}


//----------------------------------------------------------------------------------------------------------------------------------
// recordEvent: Records ALIVE output events as they occur
//----------------------------------------------------------------------------------------------------------------------------------
void TeensyIn::recordEvent()
{
  outputALIVE newEvent;
  newEvent.data = dataRead();
  newEvent.timestamp = 0;

  _inputEventBuffer[1 + _inputBufferIndex++] = newEvent;
}

//----------------------------------------------------------------------------------------------------------------------------------
// sendEventBuffer: Sends ALIVE output data saved in buffer to Teensy
//----------------------------------------------------------------------------------------------------------------------------------
void TeensyIn::sendEventBuffer()
{
  _inputEventBuffer[0].data = _inputBufferIndex - 1;

  for(int i = 0; i < EVENT_BUFFER_SIZE; i++)
  {
    Serial.print(_inputEventBuffer[i].data);
    Serial.print(_inputEventBuffer[i].timestamp);
  }

  resetBuffer();
}


//----------------------------------------------------------------------------------------------------------------------------------
// setupPins: Sets up the relevant pins for communication
//----------------------------------------------------------------------------------------------------------------------------------

void TeensyIn::setupPins()
{
  pinMode(_inputReqPin, INPUT);
  pinMode(_inputAckPin, OUTPUT);

  for(int i = 0; i < _inputNumDataPins; i++)
  {
    pinMode(_inputDataPins[i], INPUT);
  }
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


//--------------------------------------------------------------------------------------------------------------------------------------
// resetBuffer: (Re)Initialises the ALIVE output buffer and (re)sets buffer index counter variable
//--------------------------------------------------------------------------------------------------------------------------------------

void TeensyIn::resetBuffer()
{
  for(int i = 0; i < EVENT_BUFFER_SIZE; i++)
  {
    _inputEventBuffer[i].data = 0;
    _inputEventBuffer[i].timestamp = 0;
  }
  _inputBufferIndex = 0;
}