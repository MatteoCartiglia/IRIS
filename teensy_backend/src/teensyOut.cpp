//---------------------------------------------------------------------------------------------------------------------------------------
// Source file for TeensyOut class
//
// Author: Matteo Cartiglia <camatteo@ini.uzh.ch>
// Last updated: 15 JUL 2022 (Ciara Giles-Doran <gciara@student.ethz.ch>)
//---------------------------------------------------------------------------------------------------------------------------------------

#include "teensyOut.h"
#include "constants.h"

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

  setupDecoderComms();
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
// setupDecoderComms: Sets up the relevant pins for Decoder comms on Teensy
//---------------------------------------------------------------------------------------------------------------------------------------

void TeensyOut::setupDecoderComms()
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

      bool bit =bitRead(data, i)^_outputActiveLow;

      Serial.print(bit);
      Serial.print(" ");

    digitalWriteFast(_outputDataPins[i], bit);
  }
}
