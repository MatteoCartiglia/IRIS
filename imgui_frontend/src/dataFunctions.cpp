//---------------------------------------------------------------------------------------------------------------------------------------
// Source file for functions related to file handling and data conversion operations
//---------------------------------------------------------------------------------------------------------------------------------------

#include "../include/dataFunctions.h"
#include "../include/guiFunctions.h"
#include <experimental/filesystem>
#include <chrono>
#include <bitset>

namespace fs = std::experimental::filesystem;

//----------------------------------------------- Defining global variables -------------------------------------------------------------
#ifdef EXISTS_BIASGEN
double masterCurrent[BIASGEN_NO_MASTER_CURRENTS] = {BIASGEN_MASTER_CURRENT_0, BIASGEN_MASTER_CURRENT_1, BIASGEN_MASTER_CURRENT_2,
                                                    BIASGEN_MASTER_CURRENT_3, BIASGEN_MASTER_CURRENT_4, BIASGEN_MASTER_CURRENT_5};

#endif
//---------------------------------------------------------------------------------------------------------------------------------------
// parseCSV: Parses CSV files containing POR bias value for the DAC and Bias Generator
//---------------------------------------------------------------------------------------------------------------------------------------

std::vector<std::vector<std::string>> parseCSV(const std::string& path)
{
    std::vector<std::vector<std::string>> parsedCSV;

    try {
        if(fs::exists(path)) 
        {
            std::ifstream input_file(path);
            std::string line;
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
        else
        {
            throw std::runtime_error("Error parsing CSV file.");
        }
    }

    catch(std::exception exception) {
        printf("Error parsing CSV file. \t\t Error %i; '%s'\n", errno, strerror(errno));
        return parsedCSV;
    }
    
}



//---------------------------------------------------------------------------------------------------------------------------------------
// getDACvalues: Initialises DAC_command array with values from CSV file
//---------------------------------------------------------------------------------------------------------------------------------------
#ifdef EXISTS_DAC
void getBiasValues(DAC_command dac[], const std::string filename = DAC_BIASFILE )
{
    try {
        if(fs::exists(filename)) 
        {
            std::vector<std::vector<std::string>> parseCSVoutput = parseCSV(filename);

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
                dac[i].command_address =DAC_COMMAND_WRITE_UPDATE << DAC_COMMAND_WRITE_SHIFT| (int) std::stoi(parseCSVoutput[i][2]);
            }
        }
        else
        {
            throw std::runtime_error("Error reading BiasGen file.");
        }
    }
    catch(std::exception exception) {
        printf("Error reading BiasGen file. \t\t Error %i; '%s'\n", errno, strerror(errno));
    }
}
#endif

//---------------------------------------------------------------------------------------------------------------------------------------
// getBiasGenValues: Initialises BIASGEN_command array with values from CSV file
//---------------------------------------------------------------------------------------------------------------------------------------
#ifdef EXISTS_BIASGEN
void getBiasValues(BIASGEN_command biasGen[], const std::string filename)
{
    try {
        if(fs::exists(filename))
        {
            std::vector<std::vector<std::string>> parseCSVoutput = parseCSV(filename);

            for (int i = 0; i < (int) parseCSVoutput.size(); i++)
            {
                // Defining an string of spaces for consistent sizing in GUI
                std::string biasGen_BiasName = "                       ";

                for(int j = 0; j < (int) parseCSVoutput[i][0].length(); j++)
                {
                    biasGen_BiasName[j] = parseCSVoutput[i][0][j];
                }
                
                biasGen[i].name = biasGen_BiasName;
                biasGen[i].currentValue_uA = std::stof(parseCSVoutput[i][1]);
                biasGen[i].transistorType = (bool)std::stoi(parseCSVoutput[i][2]);
                biasGen[i].biasNo = std::stoi(parseCSVoutput[i][3]);
                biasGen[i].currentValue_binary = getBiasGenPacket(biasGen[i].currentValue_uA, biasGen[i].transistorType);
            }
        }
        else
        {
            throw std::runtime_error("Error reading BiasGen file.");
        }
    }

    catch(std::exception exception) {
        printf("Error reading BiasGen file. \t\t Error %i; '%s'\n", errno, strerror(errno));
    }
}
#endif

//---------------------------------------------------------------------------------------------------------------------------------------
// getIIValues: Gets the input interface values from a file
//---------------------------------------------------------------------------------------------------------------------------------------
void getIIValues(const std::string filename, std::vector<AER_DECODER_OUTPUT_command> &II_list)
{
    try {
        if(fs::exists(filename))
        {
            std::vector<std::vector<std::string>> parseCSVoutput = parseCSV(filename);
            AER_DECODER_OUTPUT_command tmp;

            for (int i = 0; i < (int) parseCSVoutput.size(); i++)
            {   
                tmp.data = std::stof(parseCSVoutput[i][0]);
                tmp.isi = std::stof(parseCSVoutput[i][1]);
                II_list.push_back(tmp);

                std::cout << "Data: " << std::bitset<16> (II_list[i].data) << " ISI: " << II_list[i].isi << std::endl;
            }
        }
        else
        {
            throw std::runtime_error("Error reading II file.");
        }
    }
    
    catch(std::exception exception) {
        printf("Error reading II file. \t\t Error %i; '%s'\n", errno, strerror(errno));
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------
// getFileLines: Retrieves the number of lines in a given file
//---------------------------------------------------------------------------------------------------------------------------------------

int getFileLines(const std::string& path)
{
    try 
    {
        if(fs::exists(path))
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

        else {
            throw std::runtime_error("Error accessing filepath provided.");
        }
    }

    catch(std::exception exception) {
        printf("Error accessing filepath provided. \t\t Error %i; '%s'\n", errno, strerror(errno));
        return -1;
    }

}


//---------------------------------------------------------------------------------------------------------------------------------------
// getRelevantFileRows_BiasGen: Retrieves the number of file lines containing the specified substring 
//---------------------------------------------------------------------------------------------------------------------------------------
#ifdef EXISTS_BIASGEN
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
#endif
//---------------------------------------------------------------------------------------------------------------------------------------
// getBiasGenPacket: Converts the bias voltage value into the equivalent binary value to send to the Bias Generator
//---------------------------------------------------------------------------------------------------------------------------------------
#ifdef EXISTS_BIASGEN

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
#endif

//---------------------------------------------------------------------------------------------------------------------------------------
// printBinaryValue: Prints the given decimal value in binary to the terminal
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


//---------------------------------------------------------------------------------------------------------------------------------------
// saveToCSV: Creates, writes and appends the give values to the specified CSV file
//---------------------------------------------------------------------------------------------------------------------------------------

void saveToCSV(long valuesToSave[], int arraySize, const std::string& filename)
{
    try
    {
        std::ofstream file(filename, std::ios::out | std::ios::app);

        if(file.good() && file.is_open())
        {
            for(int i = 0; i < arraySize; i++)
            {
                file << valuesToSave[i];

                if(i != arraySize - 1)
                {
                    file << ",";
                }
                if(i == arraySize - 1)
                {
                    file << "\n";
                }
            }
            
            file.close();
        }
        else
        {
            throw std::runtime_error("Error saving to CSV file.");
        }
    }

    catch(std::exception exception) {
        printf("Error saving to CSV file. \t\t Error %i; '%s'\n", errno, strerror(errno));
    }

}


//---------------------------------------------------------------------------------------------------------------------------------------
// saveBiases: Helper fuction to save current bias values for DAC (overloaded function)
//---------------------------------------------------------------------------------------------------------------------------------------
#ifdef EXISTS_DAC

bool saveBiases(const char *filename, DAC_command* command)
{
    try {
        std::ofstream fout(filename);

        if(fout.good() && fout.is_open())
        {
            fout << "biasName, value_(mV), address" << '\n'; 

            for (int i = 0; i < DAC_CHANNELS_USED; i++) 
            {
                fout << command[i].name << ','; 
                fout << command[i].data << ','; 
                fout << int(command[i].command_address); 
                fout << '\n'; 
            }
            
            return true;
        }
        else {
            throw std::runtime_error("Error saving DAC biases.");
        }
    }

    catch(std::exception exception) {
        printf("Error saving DAC biases. \t\t Error %i; '%s'\n", errno, strerror(errno));
        return false;
    }
}

#endif
//---------------------------------------------------------------------------------------------------------------------------------------
// saveBiases: Helper fuction to save current bias values for BIASGEN (overloaded function)
//---------------------------------------------------------------------------------------------------------------------------------------

#ifdef EXISTS_BIASGEN
bool saveBiases(const char *filename, BIASGEN_command* command)
{
    try {
        std::ofstream fout(filename);

        if(fout.good() && fout.is_open())
        {
            fout << "biasName, value_uA, transistorType, biasNo" << '\n'; 

            for (int i = 0; i < BIASGEN_CHANNELS; i++) 
            {
                fout << command[i].name << ','; 
                fout << command[i].currentValue_uA << ','; 
                fout << command[i].transistorType <<',';
                fout << command[i].biasNo; 
                fout << '\n'; 
            }

            return true;
        }
        else {
            throw std::runtime_error("Error saving BiasGen biases.");
        }
    }

    catch(std::exception exception) {
        printf("Error saving BiasGen biases. \t\t Error %i; '%s'\n", errno, strerror(errno));
        return false;
    }
}
#endif

//---------------------------------------------------------------------------------------------------------------------------------------
// getNoFiles: Returns the number of files in the specified directory
//---------------------------------------------------------------------------------------------------------------------------------------

int getNoFiles(char *filepath)
{
    try 
    {
        if(fs::exists(filepath))
        {
            int fileCounter = 0;

            for (const auto& dirEntry: fs::directory_iterator(filepath))
            {
                std::string filename_with_path = dirEntry.path();
                fileCounter++;
            }

            return fileCounter;
        }
        else
        {
            throw std::runtime_error("Error accessing specified directory.");
        }
    }

    catch(std::exception exception) {
        printf("Error accessing specified directory. \t\t Error %i; '%s'\n", errno, strerror(errno));
        return -1;
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------
// getFilepathArray: Returns an array containing the files in the specified directory
//---------------------------------------------------------------------------------------------------------------------------------------

void getFilepathArray(int noFiles, char *filepath, char* biases_filenames[])
{
    try {
        if(fs::exists(filepath))
        {
            int index = 0;

            for (const auto& dirEntry: fs::directory_iterator(filepath))
            {
                std::string filename = dirEntry.path();
                
                biases_filenames[index] = new char[filename.length() + 1];
                strcpy(biases_filenames[index], filename.c_str());

                index++;
            }
        }
        else
        {
            throw std::runtime_error("Error accessing specified directory.");
        }
    }

    catch(std::exception exception) {
        printf("Error accessing specified directory. \t\t Error %i; '%s'\n", errno, strerror(errno));
    }
}
