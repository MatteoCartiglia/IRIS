//---------------------------------------------------------------------------------------------------------------------------------------
// Header file for functions related to serial port reading and writing
//---------------------------------------------------------------------------------------------------------------------------------------

#include <unistd.h>         // UNIX standard function definitions
#include <stdio.h>          // Standard input output
#include <termios.h>        // POSIX terminal control definitions
#include <vector>

#include "../../teensy_backend/include/datatypes.h"
#include "../../teensy_backend/include/constants.h"

//---------------------------------------------------------------------------------------------------------------------------------------
// loadBiasValues: Sends the new DAC values to the Teensy 
//---------------------------------------------------------------------------------------------------------------------------------------
void loadBiasValues(DAC_command dac[], int serialPort);

//---------------------------------------------------------------------------------------------------------------------------------------
// loadBiasGenValues: Sends the new BIASGEN values to the Teensy 
//---------------------------------------------------------------------------------------------------------------------------------------
void loadBiasValues(BIASGEN_command bg[], int serialPort);

//---------------------------------------------------------------------------------------------------------------------------------------
// getSerialData: Reads data in serial port and writes entry to Log window
//---------------------------------------------------------------------------------------------------------------------------------------
void getSerialData(int serialPort, bool show_Serial_output, int expectedResponses, int bufferSize);

void  save_events( const std::string& filename, std::vector<AER_out> input_data);

//---------------------------------------------------------------------------------------------------------------------------------------
// getSerialData_Encoder: Reads data in serial port and updates plots displayed
//---------------------------------------------------------------------------------------------------------------------------------------
void getEncoderdata(int serialPort, bool show_PlotData);
void saveEncoderEvents(int serialPort);

void getSerialData_C2F(int serialPort, bool show_PlotData);

//---------------------------------------------------------------------------------------------------------------------------------------
// getHandshakeReturn: Retrieves forced handshake status
//---------------------------------------------------------------------------------------------------------------------------------------
bool getHandshakeReturn(int serialPort);
