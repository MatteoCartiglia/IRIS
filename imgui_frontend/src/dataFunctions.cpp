//---------------------------------------------------------------------------------------------------------------------------------------
//
//
// Author: 
// Last updated: 
//---------------------------------------------------------------------------------------------------------------------------------------

#include "../include/dataFunctions.h"
#include "../include/constants.h"

//---------------------------------------------------------------------------------------------------------------------------------------
// parseCSV 
//---------------------------------------------------------------------------------------------------------------------------------------

std::vector<std::vector<std::string>> parseCSV(const std::string& path)
{
    std::ifstream input_file(path);
    std::string line;
    std::vector<std::vector<std::string>> parsedCSV;

    // Iterating over all lines in the csv file
    for(int i = 0; i <= DAC_CHANNELS_USED; i++)
    {
        std::getline(input_file, line);
        std::stringstream lineStream(line);
        std::string cellValue;
        std::vector<std::string> parsedRow;

        // Excluding the first line with column titles
        if(i > 0)
        {
            // Iterating over the values per row
            while(std::getline(lineStream, cellValue, ','))
            {
                parsedRow.push_back(cellValue);
            }

            parsedCSV.push_back(parsedRow);
        }
    }

    return parsedCSV;
}

//---------------------------------------------------------------------------------------------------------------------------------------
// getDACvalues
//---------------------------------------------------------------------------------------------------------------------------------------

void getDACvalues(DAC_command dac[])
{  
    std::vector<std::vector<std::string>> parseCSVoutput = parseCSV(DAC_BIASFILE);

    for (int i = 0; i < (int) parseCSVoutput.size(); i++)
    {
        std::string dacBiasName = "                    ";

        for(int j = 0; j < (int) parseCSVoutput[i][0].length(); j++)
        {
            dacBiasName[j] = parseCSVoutput[i][0][j];
        }

        dac[i].name = dacBiasName;
        dac[i].data = (uint16_t) std::stoi(parseCSVoutput[i][1]);
        dac[i].command_address = (DAC_COMMAND << DAC_COMMAND_SHIFT) | (int) std::stoi(parseCSVoutput[i][2]);
    }
}
