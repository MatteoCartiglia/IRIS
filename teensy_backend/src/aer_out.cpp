// #include "aer.h"

// AER_out::AER_out(const int reqPin, const int ackPin, int dataPins[], int numDataPins, int d, bool activeLow) {
//   _numDataPins = numDataPins;
//   _reqPin =  reqPin;
//   _ackPin = ackPin;
//   _dataPins = dataPins;
//   _d = d;
//   _activeLow = activeLow;

//   pinMode(reqPin, OUTPUT);
//   reqWrite(0);
//   for(int i=0; i<numDataPins; i++){
//     pinMode(dataPins[i], OUTPUT);
//   }
// }

// bool AER_out::ackRead() {
//   return digitalReadFast(_ackPin)^_activeLow;
// }

// void AER_out::reqWrite(bool val) {
//   digitalWriteFast(_reqPin, val^_activeLow);
//   if (_d) {
//     delayMicroseconds(_d);
//   }
// }

// void AER_out::dataWrite(unsigned int data) {
//   for (int i=0; i<_numDataPins; i++) {
//     digitalWriteFast(_dataPins[i], bitRead(data, i)^_activeLow);
//   }
// }

// void AER_out::dataWrite(unsigned int data, Adafruit_MCP23017 mcp) {
//   for (int i=0; i<_numDataPins; i++) {
//     mcp.digitalWrite(_dataPins[i], bitRead(data, i)^_activeLow);
//   }
// }

// bool AER_out::dataWrite_handshake(unsigned int data) {
//   unsigned long t0 = millis();
//   bool success = true;

//   dataWrite(data);
//   reqWrite(1);
//   while(!ackRead())
//   {
//     if (millis() > t0 + AER_HANDSHAKE_TIMEOUT)
//     {
//       success = false;
//       break;
//     }
//   }
//   reqWrite(0);

//   return success;
// }

// bool AER_out::dataWrite_handshake(unsigned int data, Adafruit_MCP23017 mcp) {
//   unsigned long t0 = millis();
//   bool success = true;

//   dataWrite(data, mcp);
//   delayMicroseconds(10);
//   reqWrite(1);
//   while (!ackRead())
//   {
//     if (millis() > t0 + AER_HANDSHAKE_TIMEOUT)
//     {
//       success = false;
//       break;
//     }
//   }
//   delayMicroseconds(10);
//   reqWrite(0);
//   while (ackRead())
//   {
//     if (millis() > t0 + AER_HANDSHAKE_TIMEOUT)
//     {
//       success = false;
//       break;
//     }
//   }
//   delayMicroseconds(10);
  
//   return success;
// }
