#include "aer_in.h"
#include "teensy_interface.h"

AER_in::AER_in(const int reqPin, const int ackPin, int dataPins[], int numDataPins, AER_out buff[], int d, int timestampShift, bool activeLow) {
  _numDataPins = numDataPins;
  _reqPin =  reqPin;
  _ackPin = ackPin;
  _dataPins = dataPins;
  _buff = buff;
  _index = 0;
  _t0 = 0;
  _d = d; //delay
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

unsigned int AER_in::dataRead_handshake()
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

unsigned int AER_in::record_event() {
  unsigned int address = dataRead();
  unsigned int timestamp = (micros() - _t0) >> _timestampShift;
  _buff[_index++] = makeAerO(address, timestamp);
  return address;
}


void AER_in::record_event_manual(unsigned int address) {
  unsigned int timestamp_1 = (micros() - _t0) >> _timestampShift;

  _buff[_index++] = makeAerO(address, timestamp_1);
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
