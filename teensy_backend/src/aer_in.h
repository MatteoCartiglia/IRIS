//---------------------------------------------------------------------------------------------------------------------------------------
//
//
// Author: Matteo Cartiglia <camatteo@ini.uzh.ch>
// Last updated: 24 JUN 2022 (Ciara Giles-Doran)
//---------------------------------------------------------------------------------------------------------------------------------------

#ifndef aer_h
#define aer_h

#include "datatypes.h"
#include "Arduino.h"

class AER_in {

  /* Declaring class constructor and public methods */
  
  public:

    AER_in(const int reqPin,          // REQUEST PIN
           const int ackPin,          // ACKNOWLEDGE PIN
           int dataPins[],            // DATA PINS
           int numDataPins,           // NUMBER OF DATA PINS
           AER_out buff[] = NULL,     //
           int d = 0,                 //
           int timestampShift = 0,    //
           bool activeLow = true      //
    );

    /*  */
    bool reqRead();

    /*  */
    void ackWrite(bool val);

    /*  */
    unsigned int dataRead();

    /*  */
    unsigned int dataRead_handshake();

    /*  */
    unsigned int record_event();

    /*  */
    void record_event_manual(unsigned int x);

    /*  */
    unsigned int record_event_handshake();

    /*  */
    int get_index();

    /*  */
    void set_index(int x);

    /*  */
    void set_t0(int t0);


  /* Declaring private variables */

  private:
    int _reqPin;
    int _ackPin;
    int* _dataPins;
    int _numDataPins;
    AER_out* _buff;
    int _d;
    int _timestampShift;
    bool _activeLow;

    int _index;
    int _t0;
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
