//---------------------------------------------------------------------------------------------------------------------------------------
//
//
// Author: Matteo Cartiglia <camatteo@ini.uzh.ch>
// Last updated: (Ciara Giles-Doran <gciara@student.ethz.ch>)
//---------------------------------------------------------------------------------------------------------------------------------------

#include "aer.h"
#include "constants.h"

//---------------------------------------------------------------------------------------------------------------------------------------
// AER constructor for input events (i.e. events received)
//---------------------------------------------------------------------------------------------------------------------------------------

AER::AER(const int inputReqPin, const int inputAckPin, int inputDataPins[], int inputNumDataPins, byte inputBuffer[] = NULL, int inputDelay = 0, 
        int inputTimestampShift = 0, bool inputActiveLow = true)
{
  _inputReqPin = inputReqPin;
  _inputAckPin = inputAckPin;
  _inputDataPins = inputDataPins;
  _inputNumDataPins = inputNumDataPins;
  _inputBuffer = inputBuffer;
  _inputDelay = inputDelay;
  _inputTimestampShift = inputTimestampShift;
  _inputActiveLow = inputActiveLow;

  _inputIndex = 0;
  _inputT0 = 0;

  pinMode(inputAckPin, OUTPUT);
}

//---------------------------------------------------------------------------------------------------------------------------------------
// AER constructor for output events (i.e. events sent)
//---------------------------------------------------------------------------------------------------------------------------------------

AER::AER(const int outputReqPin, const int outputAckPin, int outputDataPins[], int outputNumDataPins, int outputDelay = 0, bool outputActiveLow = true)
{
  _outputReqPin = outputReqPin;
  _outputAckPin = outputAckPin;
  _outputDataPins = outputDataPins;
  _outputNumDataPins = outputNumDataPins;
  _outputDelay = outputDelay;
  _outputActiveLow = outputActiveLow;

  setupDecoderComms();
}


//---------------------------------------------------------------------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------------------------------------------------------------------

void AER::setupDecoderComms()
{
  for(int i = 0; i < _outputNumDataPins; i++)
  {
    pinMode(_outputDataPins[i], OUTPUT);
  }

  pinMode(SYN_RST_NMDA_PIN, OUTPUT);
  pinMode(SYN_RST_GABGA_PIN, OUTPUT);
  pinMode(_outputReqPin, OUTPUT);
  pinMode(_outputAckPin, INPUT);
  delay(5);

  digitalWrite(SYN_RST_NMDA_PIN, HIGH);
  delay(1);
  digitalWrite(SYN_RST_GABGA_PIN, HIGH);
  delay(1);
  digitalWrite(SYN_RST_NMDA_PIN, LOW);
  delay(1);
  digitalWrite(SYN_RST_GABGA_PIN, LOW);
  delay(1);

  reqWrite(LOW);
  delay(1);
}


//---------------------------------------------------------------------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------------------------------------------------------------------

void AER::setupEncoderComms()
{
  for(int i = 0; i < _inputNumDataPins; i++)
  {
    pinMode(_inputDataPins[i], INPUT);
  }
}


//---------------------------------------------------------------------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------------------------------------------------------------------

void AER::setupC2FComms()
{
  pinMode(_inputAckPin, OUTPUT);
  pinMode(_inputReqPin, INPUT);
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
// 
//---------------------------------------------------------------------------------------------------------------------------------------

bool AER::reqRead() 
{
  return digitalReadFast(_inputReqPin)^_inputActiveLow;
}

//---------------------------------------------------------------------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------------------------------------------------------------------

bool AER::ackRead() 
{
  return digitalReadFast(_outputAckPin)^_outputActiveLow;
}

//---------------------------------------------------------------------------------------------------------------------------------------
//
// NOTE: Works up to 32 data pins
//---------------------------------------------------------------------------------------------------------------------------------------

unsigned int AER::dataRead() 
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

//---------------------------------------------------------------------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------------------------------------------------------------------

bool AER::dataRead_handshake()
{
    bool handshakeStatus = 0;

    if (reqRead())
    {
      handshakeStatus = dataRead();
      ackWrite(1);
    }

    if(!reqRead())
    {
      ackWrite(0);
    }

    return handshakeStatus;
}

//---------------------------------------------------------------------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------------------------------------------------------------------

void AER::reqWrite(bool val) 
{
  digitalWriteFast(_outputReqPin, val^_outputActiveLow);

  if (_outputDelay) 
  {
    delayMicroseconds(_outputDelay);
  }
}


//---------------------------------------------------------------------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------------------------------------------------------------------

void AER::ackWrite(bool val) 
{
  digitalWriteFast(_inputAckPin, val^_inputActiveLow);

  if (_inputDelay) 
  {
    delayMicroseconds(_inputDelay);
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------------------------------------------------------------------

void AER::dataWrite(unsigned int data) 
{
  for (int i=0; i<_outputNumDataPins; i++) 
  {
    digitalWriteFast(_outputDataPins[i], bitRead(data, i)^_outputActiveLow);
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------------------------------------------------------------------

bool AER::dataWrite_handshake(unsigned int data) 
{
  unsigned long t0 = millis();
  bool handshakeStatus = true;

  dataWrite(data);
  reqWrite(1);

  while(!ackRead())
  {
    if (millis() > t0 + AER_WRITE_HANDSHAKE_TIMEOUT)
    {
      handshakeStatus = false;
      break;
    }
  }

  reqWrite(0);
  return handshakeStatus;
}

//---------------------------------------------------------------------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------------------------------------------------------------------

unsigned int AER::record_event() 
{
  unsigned int data = dataRead();
  unsigned int timestamp = (micros() - _inputT0) >> _inputTimestampShift;

  _inputBuffer[_inputIndex++] = ( (data << AER_RECORD_EVENT_LEFT_SHIFT) & AER_RECORD_EVENT_C0 ) | ( (timestamp >> AER_RECORD_EVENT_RIGHT_SHIFT) & AER_RECORD_EVENT_C1);
  _inputBuffer[_inputIndex++] =  timestamp & AER_RECORD_EVENT_C2;

  return data;
}

//---------------------------------------------------------------------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------------------------------------------------------------------

void AER::record_event_manual(unsigned int x) 
{
  unsigned int timestamp = (micros() - _inputT0) >> _inputTimestampShift;

  _inputBuffer[_inputIndex++] = ((x << AER_RECORD_EVENT_LEFT_SHIFT) & AER_RECORD_EVENT_C0) | ((timestamp >> AER_RECORD_EVENT_RIGHT_SHIFT) & AER_RECORD_EVENT_C1);
  _inputBuffer[_inputIndex++] =  timestamp & AER_RECORD_EVENT_C2;
}

//---------------------------------------------------------------------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------------------------------------------------------------------

bool AER::record_event_handshake()
{
  unsigned int handshakeStatus = 0;

  if (reqRead())
  {
    handshakeStatus = record_event();
    ackWrite(1);
  }

  if (!reqRead()) 
  {
    ackWrite(0);
  }

  return handshakeStatus;
}

//---------------------------------------------------------------------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------------------------------------------------------------------

void AER::set_index(int x) 
{
  _inputIndex = x;
}

//---------------------------------------------------------------------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------------------------------------------------------------------

int AER::get_index() 
{
  return _inputIndex;
}

//---------------------------------------------------------------------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------------------------------------------------------------------

void AER::set_t0(int t0) 
{
  _inputT0 = t0;
}