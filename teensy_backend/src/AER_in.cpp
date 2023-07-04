//---------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------
#include <Arduino.h>

#include "AER_in.h"
#include "constants.h"
#include "datatypes.h"
#include <chrono>

bool startRecording = false;

AER_in::AER_in(int inputReqPin, int inputAckPin, int inputDataPins[], int inputNumDataPins, int inputDelay, bool inputHandshakeActiveLow, bool inputDataActiveLow)
{
  _inputReqPin = inputReqPin;
  _inputAckPin = inputAckPin;
  _inputDataPins = inputDataPins;
  _inputNumDataPins = inputNumDataPins;
  _inputDelay = inputDelay;
  _inputHandshakeActiveLow = inputHandshakeActiveLow;
  _inputDataActiveLow = inputDataActiveLow;
  _readCallback = NULL;
  _callbackData = NULL;

  commonConstruction();
}

AER_in::AER_in(int inputReqPin, int inputAckPin, unsigned int (*readCallback)(void *), void *callbackData, int inputDelay, bool inputHandshakeActiveLow)
{
    _inputReqPin = inputReqPin;
    _inputAckPin = inputAckPin;
    _inputDataPins = NULL;
    _inputNumDataPins = 0;
    _inputDelay = inputDelay;
    _inputHandshakeActiveLow = inputHandshakeActiveLow;
    _readCallback = readCallback;
    _callbackData = callbackData;
    
    commonConstruction();
}

void AER_in::commonConstruction()
{
    saving_flag = false;
    _t0 = 0;
    _inputBufferIndex = 0;
    resetBuffer();
    setupPins();
}

//---------------------------------------------------------------------------------------------------------------------------------------
// reqRead: Reads REQ pin state
//---------------------------------------------------------------------------------------------------------------------------------------

bool AER_in::reqRead()
{
  return digitalReadFast(_inputReqPin) ^ _inputHandshakeActiveLow;
}

//---------------------------------------------------------------------------------------------------------------------------------------
// ackWrite: Writes to ACK pin
//---------------------------------------------------------------------------------------------------------------------------------------

void AER_in::ackWrite(bool val)
{

  if (_inputDelay)
  {
    delayMicroseconds(_inputDelay);
    digitalWriteFast(_inputAckPin, val ^ _inputHandshakeActiveLow);
  }
  else
  {
    digitalWriteFast(_inputAckPin, val ^ _inputHandshakeActiveLow);
  }
}

//----------------------------------------------------------------------------------------------------------------------------------
// getBufferIndex: Retreives the current index of the buffer
//----------------------------------------------------------------------------------------------------------------------------------

int AER_in::getBufferIndex()
{
  return _inputBufferIndex;
}

//----------------------------------------------------------------------------------------------------------------------------------
// setBufferIndex: Set the index of the buffer
//----------------------------------------------------------------------------------------------------------------------------------

void AER_in::setBufferIndex(int x)
{
  _inputBufferIndex = x;
}

//----------------------------------------------------------------------------------------------------------------------------------
// recordEvent: Records ALIVE output events as they occur
//----------------------------------------------------------------------------------------------------------------------------------
void AER_in::recordEvent()
{

  AER_out newEvent;
  if (!_readCallback)
      newEvent.data = getInputData();
  else
      newEvent.data = _readCallback(_callbackData);

  newEvent.timestamp = (micros() - _t0);
  /*Serial.print("Data & ts ");
  Serial.print(newEvent.data, DEC);
  Serial.print(" ");
  Serial.println(newEvent.timestamp, DEC); */
  _inputEventBuffer[_inputBufferIndex++] = newEvent;
}

//----------------------------------------------------------------------------------------------------------------------------------
// sendEventBuffer: Sends ALIVE output data saved in buffer to Teensy
//----------------------------------------------------------------------------------------------------------------------------------
void AER_in::sendEventBuffer()
{
  // Serial.print("index: ");
  // Serial.print(_inputBufferIndex);

  Aer_Data_Pkt pkt_out(_inputEventBuffer, _inputBufferIndex);
  //Serial.println("Num of events: ");
  //Serial.println(pkt_out.number_events, DEC);
  usb_serial_write((const void *)&pkt_out, sizeof(pkt_out));

  resetBuffer();
}

//----------------------------------------------------------------------------------------------------------------------------------
// handshake: Executes REQ/ACK handshake between Teensy and ALIVE
//----------------------------------------------------------------------------------------------------------------------------------
void AER_in::handshake()
{
  if (_inputHandshakeActiveLow)
  {
    if (!reqRead())
    {

      ackWrite(0);
    }

    else if (reqRead())
    {
      ackWrite(1);
    }
  }

  else if (!_inputHandshakeActiveLow)
  {
    if (!reqRead())
    {
      ackWrite(0);
    }

    else if (reqRead())
    {
      ackWrite(1);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------------------
// setupPins: Sets up the relevant pins for communication
//----------------------------------------------------------------------------------------------------------------------------------

void AER_in::setupPins()
{
  pinMode(_inputReqPin, INPUT);
  pinMode(_inputAckPin, OUTPUT);

  for (int i = 0; i < _inputNumDataPins; i++)
  {
    pinMode(_inputDataPins[i], INPUT);
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------
// getInputData: Retrieves input from ALIVE
//---------------------------------------------------------------------------------------------------------------------------------------

unsigned int AER_in::getInputData()
{
  unsigned int inputData = 0;
  for (int i = 0; i < _inputNumDataPins; i++)
  {
    inputData |= digitalReadFast(_inputDataPins[i]) << i;
  }

  if (_inputDataActiveLow)
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

void AER_in::resetBuffer()
{
  for (int i = 0; i < int(MAX_EVENTS_PER_PACKET); i++)
  {
    _inputEventBuffer[i].data = 0;
    _inputEventBuffer[i].timestamp = 0;
  }
  _inputBufferIndex = 0;
}
//--------------------------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------------------------

void AER_in::set_t0(int t0)
{
  _t0 = t0;
}

//--------------------------------------------------------------------------------------------------------------------------------------
// toggle_saving_flag : enable/disable saving flag
//--------------------------------------------------------------------------------------------------------------------------------------

void AER_in::toggle_saving_flag()
{
  bool new_saving_flag;
  if (saving_flag)
  {
    new_saving_flag = 0;
  }
  if (saving_flag == 0)
  {
    new_saving_flag = 1;
  }
  saving_flag = new_saving_flag;
}