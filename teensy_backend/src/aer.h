// #ifndef aer_h
// #define aer_h

// #include "Arduino.h"
// #include "lib/mcp/Adafruit_MCP23017.h" // This is an additional #include not part of the original AER library

// #define AER_HANDSHAKE_TIMEOUT     10

// class AER_in {
//   public:
//     AER_in(const int reqPin,
//            const int ackPin,
//            int dataPins[],
//            int numDataPins,
//            byte buff[] = NULL,
//            int d = 0,
//            int timestampShift = 0,
//            bool activeLow = true
//     );
//     bool reqRead();
//     void ackWrite(bool val);

//     unsigned int dataRead();
//     unsigned int dataRead_handshake();
//     unsigned int record_event();
//     void record_event_manual(unsigned int x);
//     unsigned int record_event_handshake();

//     void send_packet();
//     int get_index();
//     void set_index(int x);
//     void set_t0(int t0);

//   private:
//     int _numDataPins;
//     int _reqPin;
//     int _ackPin;
//     int* _dataPins;
//     byte* _buff;
//     int _index;
//     int _t0;
//     int _d;
//     int _timestampShift;
//     bool _activeLow;
// };

// class AER_out {
//   public:
//     AER_out(const int reqPin,
//             const int ackPin,
//             int dataPins[],
//             int numDataPins,
//             int d=0,
//             bool activeLow = true
//     );
//     bool ackRead();
//     void reqWrite(bool val);
//     void dataWrite(unsigned int data);
//     void dataWrite(unsigned int data, Adafruit_MCP23017 mcp);// This is an additional function not part of the original AER library
//     bool dataWrite_handshake(unsigned int data);
//     bool dataWrite_handshake(unsigned int data, Adafruit_MCP23017 mcp);// This is an additional function not part of the original AER library

//   private:
//     int _numDataPins;
//     int _reqPin;
//     int _ackPin;
//     int* _dataPins;
//     int _d;
//     bool _activeLow;
// };

// #endif
