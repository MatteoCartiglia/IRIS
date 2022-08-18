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
// getBiasValues: Initialises DAC_command array with values from CSV file
//---------------------------------------------------------------------------------------------------------------------------------------
void getBiasValues(DAC_command dac[], const std::string filename);

//---------------------------------------------------------------------------------------------------------------------------------------
// getBiasValues: Initialises BIASGEN_command array with values from CSV file
//---------------------------------------------------------------------------------------------------------------------------------------
void getBiasValues(BIASGEN_command biasGen[], const std::string filename );

//---------------------------------------------------------------------------------------------------------------------------------------
// loadBiasValues: Sends the new DAC values to the Teensy 
//---------------------------------------------------------------------------------------------------------------------------------------
void loadBiasValues(DAC_command dac[], int serialport);

//---------------------------------------------------------------------------------------------------------------------------------------
// loadBiasValues: Sends the new BIASGEN values to the Teensy 
//---------------------------------------------------------------------------------------------------------------------------------------
void loadBiasValues(BIASGEN_command biasGen[], int serialport);

//---------------------------------------------------------------------------------------------------------------------------------------
// readSerialPort: Reads data available at the serial port
//---------------------------------------------------------------------------------------------------------------------------------------
void readSerialPort();

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
// saveBiases: Writes current bias values for DAC to a local file (overloaded function)
//---------------------------------------------------------------------------------------------------------------------------------------
bool saveBiases(const char *filename, DAC_command* command);

//---------------------------------------------------------------------------------------------------------------------------------------
// saveBiases: Writes current bias values for BIASGEN to a local file (overloaded function)
//---------------------------------------------------------------------------------------------------------------------------------------
bool saveBiases(const char *filename, BIASGEN_command* command);

//---------------------------------------------------------------------------------------------------------------------------------------
// getNoFiles: Returns the number of files in the specified directory
//---------------------------------------------------------------------------------------------------------------------------------------
int getNoFiles(char *filepath);

//---------------------------------------------------------------------------------------------------------------------------------------
// getFilepathArray: Returns an array containing the files in the specified directory
//---------------------------------------------------------------------------------------------------------------------------------------
void getFilepathArray(int noFiles, char *filepath, char* biases_filenames[]);

