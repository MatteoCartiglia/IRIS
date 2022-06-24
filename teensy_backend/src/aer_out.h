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
//     bool dataWrite_handshake(unsigned int data);

//   private:
//     int _numDataPins;
//     int _reqPin;
//     int _ackPin;
//     int* _dataPins;
//     int _d;
//     bool _activeLow;
// };
