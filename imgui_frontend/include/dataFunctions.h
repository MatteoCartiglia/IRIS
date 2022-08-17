//---------------------------------------------------------------------------------------------------------------------------------------
// Header file for functions related to file handling and data conversion operations
//
// Author: Ciara Giles-Doran <gciara@student.ethz.ch>
// Last updated: 15 JUL 2022
//---------------------------------------------------------------------------------------------------------------------------------------

#include <vector>
#include <string>
#include <sstream>
#include <fstream>          // Includes functions to write to file type
#include <iostream>
#include <errno.h>          // Error number definitions
#include <cstddef>          // Includes byte data type

#include "../../teensy_backend/include/datatypes.h"
#include "../../teensy_backend/include/constants.h"


//---------------------------------------------------------------------------------------------------------------------------------------
// parseCSV: Parses CSV files containing POR bias value for the DAC and Bias Generator
//---------------------------------------------------------------------------------------------------------------------------------------
std::vector<std::vector<std::string>> parseCSV(const std::string& path);

//---------------------------------------------------------------------------------------------------------------------------------------
// getDACvalues: Initialises DAC_command array with values from CSV file
//---------------------------------------------------------------------------------------------------------------------------------------
void getDACvalues(DAC_command dac[], const std::string filename);

//---------------------------------------------------------------------------------------------------------------------------------------
// loadDACvalues: 
//---------------------------------------------------------------------------------------------------------------------------------------
int loadDACvalues(DAC_command dac[], int serialport);

//---------------------------------------------------------------------------------------------------------------------------------------
// getBiasGenValues: Initialises BIASGEN_command array with values from CSV file
//---------------------------------------------------------------------------------------------------------------------------------------
void getBiasGenValues(BIASGEN_command biasGen[], const std::string filename );

//---------------------------------------------------------------------------------------------------------------------------------------
// loadBiasGenValues: 
//---------------------------------------------------------------------------------------------------------------------------------------
int loadBiasGenValues(BIASGEN_command biasGen[], int serialport);

//---------------------------------------------------------------------------------------------------------------------------------------
// getFileLines: Retrieves the number of lines in a given file
//---------------------------------------------------------------------------------------------------------------------------------------
int getFileLines(const std::string& path);

//---------------------------------------------------------------------------------------------------------------------------------------
// getRelevantFileRows_BiasGen: Retrieves the number of file lines containing the specified substring 
//---------------------------------------------------------------------------------------------------------------------------------------
int getRelevantFileRows_BiasGen(std::string substring, BIASGEN_command biasGen[], bool relevantFileRows[], int fileRows);

//---------------------------------------------------------------------------------------------------------------------------------------
// getBiasGenPacket: Converts the bias voltage value into the equivalent binary value to send to the Bias Generator
//---------------------------------------------------------------------------------------------------------------------------------------
int getBiasGenPacket(float decimalVal, bool transistorType);

//---------------------------------------------------------------------------------------------------------------------------------------
// getAERpacket: Generates the equivalent AER binary value for the user-selected options
//---------------------------------------------------------------------------------------------------------------------------------------
int getAERpacket(int selection_chipCore, int selection_synapseType, int selection_neuronNumber, int value_synapseNumber);

//---------------------------------------------------------------------------------------------------------------------------------------
// printBinaryValue: Prints the given decimal value in binary to the terminal
//---------------------------------------------------------------------------------------------------------------------------------------
void printBinaryValue(int decimalVal, int size);

//---------------------------------------------------------------------------------------------------------------------------------------
// saveToCSV: Creates, writes and appends the give values to the specified CSV file
//---------------------------------------------------------------------------------------------------------------------------------------
void saveToCSV(long valuesToSave[], int arraySize, const std::string& filename);

//---------------------------------------------------------------------------------------------------------------------------------------
// saveBiases: Helper fuction to save current bias values for DAC (overloaded function)
//---------------------------------------------------------------------------------------------------------------------------------------
bool saveBiases(const char *filename, DAC_command* command);

//---------------------------------------------------------------------------------------------------------------------------------------
// saveBiases: Helper fuction to save current bias values for BIASGEN (overloaded function)
//---------------------------------------------------------------------------------------------------------------------------------------
bool saveBiases(const char *filename, BIASGEN_command* command);


