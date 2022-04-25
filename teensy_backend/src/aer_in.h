#ifndef aer_h
#define aer_h

#include "Arduino.h"
#define AER_HANDSHAKE_TIMEOUT     10


class AER_in {
  public:
    AER_in(const int reqPin,
           const int ackPin,
           int dataPins[],
           int numDataPins,
           byte buff[] = NULL,
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

    void send_packet();
    int get_index();
    void set_index(int x);
    void set_t0(int t0);

  private:
    int _numDataPins;
    int _reqPin;
    int _ackPin;
    int* _dataPins;
    byte* _buff;
    int _index;
    int _t0;
    int _d;
    int _timestampShift;
    bool _activeLow;
};
#endif
