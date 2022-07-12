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
#include "../../teensy_backend/include/constants.h"

//
std::vector<std::vector<std::string>> parseCSV(const std::string& path);

//
void getDACvalues(DAC_command dac[]);
void getBiasGenValues(BIASGEN_command biasGen[]);

//
int getFileLines(const std::string& path);
int getRelevantFileRows_BiasGen(std::string substring, BIASGEN_command biasGen[], bool relevantFileRows[], int fileRows);

//
int getBiasGenPacket(float decimalVal, bool transistorType);
int getAERpacket(int selection_chipCore, int selection_synapseType, int selection_neuronNumber, int value_synapseNumber);

//
void printBinaryValue(int decimalVal, int size);
