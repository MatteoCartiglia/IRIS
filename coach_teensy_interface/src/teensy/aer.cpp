#include "aer.h"

AER_in::AER_in(const int reqPin, const int ackPin, int dataPins[], int numDataPins, byte buff[], int d, int timestampShift, bool activeLow) {
  _numDataPins = numDataPins;
  _reqPin =  reqPin;
  _ackPin = ackPin;
  _dataPins = dataPins;
  _buff = buff;
  _index = 0;
  _t0 = 0;
  _d = d;
  _timestampShift = timestampShift;
  _activeLow = activeLow;

  pinMode(ackPin, OUTPUT);
}

bool AER_in::reqRead() {
  return digitalReadFast(_reqPin)^_activeLow;
}

void AER_in::ackWrite(bool val) {
  digitalWriteFast(_ackPin, val^_activeLow);
  if (_d) {
    delayMicroseconds(_d);
  }
}

unsigned int AER_in::dataRead() { //works up to 32 data pins
  unsigned int x=0;
  for (int i=0; i<_numDataPins; i++) {
    x |= digitalReadFast(_dataPins[i]) << i;
  }
  if (_activeLow) {
    return ~x;
  }
  else {
    return x;
  }
}

unsigned int AER_in::send_packet()
{
    unsigned int x = 0;
    if (reqRead()){
      x = dataRead();
      ackWrite(1);
    }
    if(!reqRead()){
      ackWrite(0);
    }
    return x;
}

unsigned int AER_in::record_event() {
  unsigned int x = dataRead();
  unsigned int timestamp = (micros() - _t0) >> _timestampShift;

  _buff[_index++] = ((x<<5) & 0xE0) | ((timestamp>>8) & 0x1F);
  _buff[_index++] =  timestamp & 0xFF;

  return x;
}

void AER_in::record_event_manual(unsigned int x) {
  unsigned int timestamp = (micros() - _t0) >> _timestampShift;

  _buff[_index++] = ((x<<5) & 0xE0) | ((timestamp >> 8) & 0x1F);
  _buff[_index++] =  timestamp & 0xFF;
}

unsigned int AER_in::record_event_handshake()
{
  unsigned int x = 0;
  if (reqRead()){
    x = record_event();
    ackWrite(1);
  }
  if (!reqRead()) {
    ackWrite(0);
  }
  return x;
}

void AER_in::send_packet() {
  set_t0(micros() >> _timestampShift & 0xFFFF);
  usb_serial_write(_buff, _index);
  set_index(0);
}

void AER_in::set_index(int x) {
  _index = x;
}

int AER_in::get_index() {
  return _index;
}

void AER_in::set_t0(int t0) {
  _t0 = t0;
}


AER_out::AER_out(const int reqPin, const int ackPin, int dataPins[], int numDataPins, int d, bool activeLow) {
  _numDataPins = numDataPins;
  _reqPin =  reqPin;
  _ackPin = ackPin;
  _dataPins = dataPins;
  _d = d;
  _activeLow = activeLow;

  pinMode(reqPin, OUTPUT);
  reqWrite(0);
  for(int i=0; i<numDataPins; i++){
    pinMode(dataPins[i], OUTPUT);
  }
}

bool AER_out::ackRead() {
  return digitalReadFast(_ackPin)^_activeLow;
}

void AER_out::reqWrite(bool val) {
  digitalWriteFast(_reqPin, val^_activeLow);
  if (_d) {
    delayMicroseconds(_d);
  }
}

void AER_out::dataWrite(unsigned int data) {
  for (int i=0; i<_numDataPins; i++) {
    digitalWriteFast(_dataPins[i], bitRead(data, i)^_activeLow);
  }
}

void AER_out::dataWrite(unsigned int data, Adafruit_MCP23017 mcp) {
  for (int i=0; i<_numDataPins; i++) {
    mcp.digitalWrite(_dataPins[i], bitRead(data, i)^_activeLow);
  }
}

bool AER_out::dataWrite_handshake(unsigned int data) {
  unsigned long t0 = millis();
  bool success = true;

  dataWrite(data);
  reqWrite(1);
  while(!ackRead())
  {
    if (millis() > t0 + AER_HANDSHAKE_TIMEOUT)
    {
      success = false;
      break;
    }
  }
  reqWrite(0);

  return success;
}

bool AER_out::dataWrite_handshake(unsigned int data, Adafruit_MCP23017 mcp) {
  unsigned long t0 = millis();
  bool success = true;

  dataWrite(data, mcp);
  delayMicroseconds(10);
  reqWrite(1);
  while (!ackRead())
  {
    if (millis() > t0 + AER_HANDSHAKE_TIMEOUT)
    {
      success = false;
      break;
    }
  }
  delayMicroseconds(10);
  reqWrite(0);
  while (ackRead())
  {
    if (millis() > t0 + AER_HANDSHAKE_TIMEOUT)
    {
      success = false;
      break;
    }
  }
  delayMicroseconds(10);
  
  return success;
}
