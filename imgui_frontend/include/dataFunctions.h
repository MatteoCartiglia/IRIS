//---------------------------------------------------------------------------------------------------------------------------------------
//
//
// Author: Ciara Giles-Doran <gciara@student.ethz.ch>
// Last updated: 
//---------------------------------------------------------------------------------------------------------------------------------------

#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <errno.h>          // Error number definitions
#include <cstddef>          // Includes byte data type

#include "../../teensy_backend/include/datatypes.h"

//
std::vector<std::vector<std::string>> parseCSV(const std::string& path);

//
void getDACvalues(DAC_command dac[]);
void getBiasGenValues(BIASGEN_command biasGen[]);

//
int getFileLines(const std::string& path);
int getRelevantFileRows_BiasGen(std::string substring, BIASGEN_command biasGen[], bool relevantFileRows[], int fileRows);

//
void getAERpacket();
void getBiasGenPacket(float decimalVal, int transistorType, int binaryVal);
void getDACpacket();

//
void printBinaryValue(int decimalVal, int size);
