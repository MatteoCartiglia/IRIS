//---------------------------------------------------------------------------------------------------------------------------------------
//
//
// Author: Ciara Giles-Doran <gciara@student.ethz.ch>
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
    int noFileLines = getFileLines(path);

    // Iterating over all lines in the csv file
    for(int i = 0; i <noFileLines; i++)
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
// getFileLines
//---------------------------------------------------------------------------------------------------------------------------------------

int getFileLines(const std::string& path)
{
    int noRows = 0;
    std::ifstream input_file(path);
    std::string line;

    while(std::getline(input_file, line))
    {
        noRows++;
    }    

    return noRows;
}

//---------------------------------------------------------------------------------------------------------------------------------------
// getDACvalues
//---------------------------------------------------------------------------------------------------------------------------------------

void getDACvalues(DAC_command dac[])
{  
    std::vector<std::vector<std::string>> parseCSVoutput = parseCSV(DAC_BIASFILE);

    for (int i = 0; i < (int) parseCSVoutput.size(); i++)
    {
        std::string dacBiasName = "                       ";

        for(int j = 0; j < (int) parseCSVoutput[i][0].length(); j++)
        {
            dacBiasName[j] = parseCSVoutput[i][0][j];
        }

        dac[i].name = dacBiasName;
        dac[i].data = (uint16_t) std::stoi(parseCSVoutput[i][1]);
        dac[i].command_address = DAC_COMMAND_WRITE_UPDATE | (int) std::stoi(parseCSVoutput[i][2]);
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------
// getSPIvalues
//---------------------------------------------------------------------------------------------------------------------------------------

void getBiasGenValues(BIASGEN_command biasGen[])
{
    std::vector<std::vector<std::string>> parseCSVoutput = parseCSV(BIASGEN_BIASFILE);

    for (int i = 0; i < (int) parseCSVoutput.size(); i++)
    {
        std::string biasGen_BiasName = "                       ";

        for(int j = 0; j < (int) parseCSVoutput[i][0].length(); j++)
        {
            biasGen_BiasName[j] = parseCSVoutput[i][0][j];
        }
        
        biasGen[i].name = biasGen_BiasName;
        biasGen[i].currentValue_uV = std::stof(parseCSVoutput[i][1]);
        // biasGen[i].course_val = 0;
        // biasGen[i].fine_val = 0;
        // biasGen[i].transistor_type = 0;
        // biasGen[i].address = 0;
    }
}

int decimalToBinary(int decimalVal)
{

}

//---------------------------------------------------------------------------------------------------------------------------------------
// getRelevantFileRows
//---------------------------------------------------------------------------------------------------------------------------------------

int getRelevantFileRows_BiasGen(std::string substring, BIASGEN_command biasGen[], bool relevantFileRows[], int fileRows)
{
    int counter = 0;

    for(int i = 0; i < fileRows; i++)
    {
        if (biasGen[i].name.find(substring) != std::string::npos) 
        {
            relevantFileRows[i] = 1;
            counter++;
        }
        else
        {
            relevantFileRows[i] = 0;
        }
    }

    return counter;
}