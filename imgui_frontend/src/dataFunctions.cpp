//---------------------------------------------------------------------------------------------------------------------------------------
//
//
// Author: Ciara Giles-Doran <gciara@student.ethz.ch>
// Last updated: 
//---------------------------------------------------------------------------------------------------------------------------------------

#include "../include/dataFunctions.h"

//----------------------------------------------- Defining global variables -------------------------------------------------------------

double masterCurrent[BIASGEN_NO_MASTER_CURRENTS] = {BIASGEN_MASTER_CURRENT_0, BIASGEN_MASTER_CURRENT_1, BIASGEN_MASTER_CURRENT_2,
                                                    BIASGEN_MASTER_CURRENT_3, BIASGEN_MASTER_CURRENT_4, BIASGEN_MASTER_CURRENT_5};


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
        biasGen[i].biasNo = std::stoi(parseCSVoutput[i][3]);
        biasGen[i].currentValue_binary = getBiasGenPacket(biasGen[i].currentValue_uV, biasGen[i].transistorType);

        // printf("%s : ", biasGen_BiasName.c_str());
        // printBinaryValue(biasGen[i].currentValue_binary, BIASGEN_PACKET_SIZE);
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

int getBiasGenPacket(float decimalVal, bool transistorType)
{
    if(decimalVal > BIASGEN_MAX_CURRENT)
    {
        fprintf(stderr, "BiasGen cannot generate currents larger than %f uA.\n", BIASGEN_MAX_CURRENT);
        return 0;
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
        fineCurrent = BIASGEN_SCALING_FACTOR*decimalVal/masterCurrent[coarseCurrent];

        // Create 12-bit binary packet
        int binaryVal = (coarseCurrent << BIASGEN_COURSE_SHIFT) | (fineCurrent << BIASGEN_FINE_SHIFT) | (transistorType);

        // printf("Current: %.6f uA \t Decimal value: %d \t Binary: ", decimalVal, binaryVal);
        // printBinaryValue(binaryVal, BIASGEN_PACKET_SIZE);

        return binaryVal;
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------
// getAERpacket
//---------------------------------------------------------------------------------------------------------------------------------------

int getAERpacket(int selection_chipCore, int selection_synapseType, int selection_neuronNumber, int value_synapseNumber)
{
    int chipCore = selection_chipCore << ALIVE_CORE_SHIFT;
    int synapseType = selection_synapseType << ALIVE_SYNAPSE_TYPE_SHIFT;

    // For AMPA synapses, the neuron number is selected and bit 3 is tied to 1
    if(selection_synapseType == 0)
    {
        int neuronNumber = selection_neuronNumber << ALIVE_NEURON_SHIFT;
        int synapseLimit = 1 << ALIVE_AMPA_SHIFT;
        return chipCore | synapseType | neuronNumber | synapseLimit | value_synapseNumber;
    }

    // For GABAa synapses (CC and NN), the neuron number is selected and bits 1-3 are tied to 1
    else if(selection_synapseType == 1)
    {
        int neuronNumber = selection_neuronNumber << ALIVE_NEURON_SHIFT;
        int synapseLimit = ALIVE_GABAa_BITS_1_2_3 << 1;
        return chipCore | synapseType | neuronNumber | synapseLimit | value_synapseNumber;
    }

    // For NN GABAb, bits 4 and 5 of the AER packet are tied to 1 as there are only 16 columns
    else if((selection_synapseType == 2) && (selection_chipCore == 1))
    {
        int neuronNumber = ALIVE_NN_GABAb_BITS_4_5 << ALIVE_NEURON_SHIFT;
        return chipCore | synapseType | neuronNumber | value_synapseNumber;
    }

    // For NMDA synapses (CC and NN) and CC GABGb, all neurons are selected 
    else if((selection_synapseType == 3) || ((selection_synapseType == 2) && (selection_chipCore == 0)))
    {
        return chipCore | synapseType | value_synapseNumber;
    }

    // If the conditions above have not been met, there is an error
    else
    {
        printf("AER packet error. Core: %d\t Synapse Type: %d\n", chipCore, synapseType);
        return 0;
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------
// printBinaryValue
//---------------------------------------------------------------------------------------------------------------------------------------

void printBinaryValue(int decimalVal, int size)
{
    bool binaryNum[size];
    int valueToPrint = decimalVal;

    // Initialising the array
    for(int j = 0; j < size; j++)
    {
        binaryNum[j] = 0;
    }

    // Defining the loop iterator
    int i = size - 1;

    if(valueToPrint > 0)
    {
        while (valueToPrint > 0) 
        {
            // Perform modulo operation and store remainder in binary array
            binaryNum[i] = valueToPrint % 2;

            // Divide value by 2 and increment counter
            valueToPrint = valueToPrint / 2;
            i--;
        }
    }

    // Printing binary array
    for (int k = 0; k < size; k++)
    {
        std::cout << binaryNum[k];
    }

    std::cout << "\n";
}


