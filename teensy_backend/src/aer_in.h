#ifndef aer_h
#define aer_h
#include "teensy_interface.h"
#include "Arduino.h"
#define AER_HANDSHAKE_TIMEOUT     10


class AER_in {
  public:
    AER_in(const int reqPin,
           const int ackPin,
           int dataPins[],
           int numDataPins,
           AER_out buff[] = NULL,
           int d = 0,
           int timestampShift = 0,
           bool activeLow = true
    );
    bool reqRead();
    void ackWrite(bool val);

    unsigned int dataRead();
    unsigned int dataRead_handshake();
    unsigned int record_event();
    void record_event_manual(unsigned int x);
    unsigned int record_event_handshake();

    int get_index();
    void set_index(int x);
    void set_t0(int t0);

  private:
    int _numDataPins;
    int _reqPin;
    int _ackPin;
    int* _dataPins;
    AER_out* _buff;
    int _index;
    int _t0;
    int _d;
    int _timestampShift;
    bool _activeLow;
};

static inline AER_out makeAerO(unsigned int addr, unsigned int ts)
{
  AER_out event;
  event.address = 0;
  event.address =(uint8_t) addr;
  event.ts_1ms =(uint16_t) ts/1000;
  Serial.print("\n makeAerO addr: ");
  Serial.print(event.address,BIN);
  return event;
}

#endif
