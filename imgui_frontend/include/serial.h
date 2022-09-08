//---------------------------------------------------------------------------------------------------------------------------------------
// Header file for functions related to serial port reading and writing
//
// Author: Ciara Giles-Doran <gciara@student.ethz.ch>
// Last updated:
//---------------------------------------------------------------------------------------------------------------------------------------

#include <unistd.h>         // UNIX standard function definitions
#include <stdio.h>          // Standard input output
#include <termios.h>        // POSIX terminal control definitions

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

//---------------------------------------------------------------------------------------------------------------------------------------
// getSerialData_Plots: Reads data in serial port and updates plots displayed
//---------------------------------------------------------------------------------------------------------------------------------------
void getSerialData_Plots(int serialPort, bool show_PlotData);

//---------------------------------------------------------------------------------------------------------------------------------------
// getHandshakeReturn: Retrieves forced handshake status
//---------------------------------------------------------------------------------------------------------------------------------------
bool getHandshakeReturn(int serialPort);