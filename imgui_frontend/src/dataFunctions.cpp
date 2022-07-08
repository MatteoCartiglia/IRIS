//---------------------------------------------------------------------------------------------------------------------------------------
//
//
// Author: Ciara Giles-Doran <gciara@student.ethz.ch>
// Last updated: 
//---------------------------------------------------------------------------------------------------------------------------------------

#include "../include/dataFunctions.h"
#include "../include/constants.h"

//----------------------------------------------- Defining global variables -------------------------------------------------------------

float masterCurrent[BIASGEN_NO_MASTER_CURRENTS] = {0.00006, 0.00046, 0.0038, 0.03, 0.24, 1.9}; // uA


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
// getDACvalues
//---------------------------------------------------------------------------------------------------------------------------------------

void getDACvalues(DAC_command dac[])
{  
    std::vector<std::vector<std::string>> parseCSVoutput = parseCSV(DAC_BIASFILE);

    for (int i = 0; i < (int) parseCSVoutput.size(); i++)
    {
        // Defining an string of spaces for consistent sizing in GUI
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
// getBiasGenValues
//---------------------------------------------------------------------------------------------------------------------------------------

void getBiasGenValues(BIASGEN_command biasGen[])
{
    std::vector<std::vector<std::string>> parseCSVoutput = parseCSV(BIASGEN_BIASFILE);

    for (int i = 0; i < (int) parseCSVoutput.size(); i++)
    {
        // Defining an string of spaces for consistent sizing in GUI
        std::string biasGen_BiasName = "                       ";

        for(int j = 0; j < (int) parseCSVoutput[i][0].length(); j++)
        {
            biasGen_BiasName[j] = parseCSVoutput[i][0][j];
        }
        
        biasGen[i].name = biasGen_BiasName;
        biasGen[i].currentValue_uV = std::stof(parseCSVoutput[i][1]);
        biasGen[i].transistorType = std::stoi(parseCSVoutput[i][2]);
        biasGen[i].address = std::stoi(parseCSVoutput[i][3]);

        //
        getBiasGenPacket(biasGen[i].currentValue_uV, biasGen[i].transistorType, biasGen[i].currentValue_binary);
    }
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

//---------------------------------------------------------------------------------------------------------------------------------------
// getBiasGenPacket
//---------------------------------------------------------------------------------------------------------------------------------------

void getBiasGenPacket(float decimalVal, int transistorType, int binaryVal)
{
    if(decimalVal > BIASGEN_MAX_CURRENT)
    {
        fprintf(stderr, "BiasGen cannot generate currents larger than %f uA.\n", BIASGEN_MAX_CURRENT);
    }
    else
    {
        int fineCurrent;
        int coarseCurrent;

        // Calculating the coarse current value
        for(int i = 0; i < BIASGEN_NO_MASTER_CURRENTS; i++)
        {
            if(masterCurrent[i] > decimalVal)
            {
                coarseCurrent = i;
                break;
            }
        }

        // Calculating the fine current value: I_target = I_coarse*I_fine / 256
        fineCurrent = BIASGEN_MULTIPL_FACTOR*decimalVal/masterCurrent[coarseCurrent];

        // Create 12-bit binary packet
        binaryVal = (coarseCurrent << BIASGEN_COURSE_BIAS_SHIFT) | (fineCurrent << BIASGEN_FINE_BIAS_SHIFT) | (transistorType);

        // printf("Current: %.6f uA \t Decimal value: %d \t Binary: ", decimalVal, binaryVal);
        // printBinaryValue(binaryVal, BIASGEN_PACKET_SIZE);
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------
// printBinaryValue
//---------------------------------------------------------------------------------------------------------------------------------------

void printBinaryValue(int decimalVal, int size)
{
    bool binaryNum[size];
    int valueToPrint = decimalVal;

    // Defining the loop iterator
    int i = 0;

    if(valueToPrint > 0)
    {
        while (valueToPrint > 0) 
        {
            // Perform modulo operation and store remainder in binary array
            binaryNum[i] = valueToPrint % 2;

            // Divide value by 2 and increment counter
            valueToPrint = valueToPrint / 2;
            i++;
        }
    }
    else
    {
        for(i = 0; i < size; i++)
        {
            binaryNum[i] = 0;
        }
    }


    // Printing binary array in reverse order
    for (int j = i - 1; j >= 0; j--)
    {
        std::cout << binaryNum[j];
    }

    std::cout << "\n";
}


