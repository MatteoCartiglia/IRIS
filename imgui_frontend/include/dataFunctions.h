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

#include "../../teensy_backend/include/datatypes.h"
#include "../include/constants.h"

std::vector<std::vector<std::string>> parseCSV(const std::string& path);

void getDACvalues(DAC_command dac[]);