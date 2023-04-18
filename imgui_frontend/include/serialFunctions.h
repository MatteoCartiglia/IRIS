//---------------------------------------------------------------------------------------------------------------------------------------
// Header file for functions related to serial port reading and writing
//---------------------------------------------------------------------------------------------------------------------------------------

#include <unistd.h>         // UNIX standard function definitions
#include <stdio.h>          // Standard input output
#include <termios.h>        // POSIX terminal control definitions
#include <vector>

#include "../../teensy_backend/include/datatypes.h"
#include "../../teensy_backend/include/constants.h"
#include "serial.h"

void save_events( const std::string& filename, std::vector<AER_out> input_data);

//---------------------------------------------------------------------------------------------------------------------------------------
// getSerialData_Encoder: Reads data in serial port and updates plots displayed
//---------------------------------------------------------------------------------------------------------------------------------------
void getEncoderdata(int serialPort, bool show_PlotData);

void getSerialData_C2F(int serialPort, bool show_PlotData);

//---------------------------------------------------------------------------------------------------------------------------------------
// getHandshakeReturn: Retrieves forced handshake status
//---------------------------------------------------------------------------------------------------------------------------------------
bool getHandshakeReturn(int serialPort);

#ifdef EXISTS_DAC
    //----------------------------------------------------------------------------------------------------------------------------------
    // writeBiasValues: Sends the new DAC values to the Teensy 
    //----------------------------------------------------------------------------------------------------------------------------------
    void writeBiasValues(DAC_command dac[], const Serial& sPort);
#endif

#ifdef EXISTS_BIASGEN
    //----------------------------------------------------------------------------------------------------------------------------------
    // writeBiasValues: Sends the new BIASGEN values to the Teensy 
    //----------------------------------------------------------------------------------------------------------------------------------
    void writeBiasValues(BIASGEN_command bg[], const Serial& sPort);
#endif
