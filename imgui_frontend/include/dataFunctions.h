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

#include "../../teensy_backend/include/datatypes.h"
#include "../include/constants.h"

std::vector<std::vector<std::string>> parseCSV(const std::string& path);

int getFileLines(const std::string& path);
void getDACvalues(DAC_command dac[]);

void getBiasGenValues(BIASGEN_command biasGen[]);
void getRelevantFileRows_BiasGen(std::string substring, BIASGEN_command biasGen[], bool relevantFileRows[], int fileRows);

int decimalToBinary(int decimalVal);