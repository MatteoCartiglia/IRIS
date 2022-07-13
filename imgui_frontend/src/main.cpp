//---------------------------------------------------------------------------------------------------------------------------------------
//
//
// Author: Matteo Cartiglia <camatteo@ini.uzh.ch>
// Last updated: 
//---------------------------------------------------------------------------------------------------------------------------------------

#include <GLFW/glfw3.h>     // Will drag system OpenGL headers
#include <unistd.h>         // UNIX standard function definitions
#include <errno.h>          // Error number definitions
#include <termios.h>        // POSIX terminal control definitions
#include <fcntl.h>          // File control definitions
#include <stdio.h>
#include <string>   
#include <vector>   
#include <cstddef>
#include <iostream>

#include "../imgui/imgui_backend/imgui_impl_opengl3.h"
#include "../imgui/imgui_backend/imgui_impl_glfw.h"
#include "../../teensy_backend/include/datatypes.h"
#include "../imgui/imgui_src/implot.h"
#include "../imgui/imgui_src/implot_internal.h"

#include "../include/guiFunctions.h"
#include "../include/dataFunctions.h"
#include "../../teensy_backend/include/constants.h"

//----------------------------------------------------- Defining Function Prototypes ----------------------------------------------------- 
void getSerialData(int serialPort, int expectedResponses, int bufferSize);


//------------------------------------------------------- Defining Global Variables ------------------------------------------------------ 
bool show_BiasGen_config = true;
bool show_DAC_config = true;
bool show_AER_config = true;
bool show_Serial_output = true;
bool powerOnReset = true;

//---------------------------------------------------------------------------------------------------------------------------------------
// main
//---------------------------------------------------------------------------------------------------------------------------------------

int main(int, char**)
{
    //-------------------------------------------------- Defining & Initialising Variables ---------------------------------------------- 

    const char* logString;
    bool logEntry = false;

    int serialPort;
    char serialPortOpenStr[SERIAL_BUFFER_SIZE_PORT_OPEN] = {"Serial port opened successfully.\n"};
    int expectedResponses = 0;
    struct termios SerialPortSettings;

    BIASGEN_command biasGen[BIASGEN_CHANNELS];
    DAC_command dac[DAC_CHANNELS_USED];

    std::string substring[BIASGEN_CATEGORIES] = {"DE_", "NEUR_", "SYN_A", "SYN_D", "PWEXT", "LB_", "ST_", "C2F_", "BUFFER_"};
    bool relevantFileRows[BIASGEN_CATEGORIES][BIASGEN_CHANNELS];
    int noRelevantFileRows[BIASGEN_CATEGORIES];

    getDACvalues(dac);
    getBiasGenValues(biasGen);

#ifdef BIASGEN_SET_TRANSISTOR_TYPE
    std::vector<std::vector<std::vector<int>>> valueChange_BiasGen;
    int biasGenNoValueChanges = 2;

    for(int i = 0; i < BIASGEN_CATEGORIES; i++)
    {
        std::vector<int> valueChange_currentTransistorType;

        // Resizing the vector to prevent malloc errors
        valueChange_currentTransistorType.resize(biasGenNoValueChanges);

        noRelevantFileRows[i] = getRelevantFileRows_BiasGen(substring[i], biasGen, relevantFileRows[i], BIASGEN_CHANNELS);
        std::vector<std::vector<int>> valueChange_BiasGen_Category;
        valueChange_BiasGen_Category.resize(noRelevantFileRows[i]);

        for(int j = 0; j <= noRelevantFileRows[i]; j++)
        {
            for(int k = 0; k < biasGenNoValueChanges; k++)
            {
                valueChange_currentTransistorType.push_back(0);
            }

            valueChange_BiasGen_Category.push_back(valueChange_currentTransistorType);
        }
        
        valueChange_BiasGen.push_back(valueChange_BiasGen_Category);
    }

#else
    std::vector<std::vector<int>> valueChange_BiasGen;

    // Creating a vector of boolean vectors to hold the bias value change variables for each bias per category
    for(int i = 0; i < BIASGEN_CATEGORIES; i++)
    {
        std::vector<int> valueChange_BiasGen_Category;
        noRelevantFileRows[i] = getRelevantFileRows_BiasGen(substring[i], biasGen, relevantFileRows[i], BIASGEN_CHANNELS);
        
        // Resizing the vector to prevent malloc errors
        valueChange_BiasGen_Category.resize(noRelevantFileRows[i]);

        for(int j = 0; j <= noRelevantFileRows[i]; j++)
        {
            valueChange_BiasGen_Category.push_back(0);
        }
        
        valueChange_BiasGen.push_back(valueChange_BiasGen_Category);
    }
 #endif

    //------------------------------------------------------ Opening Serial Port ------------------------------------------------------
    
    serialPort = open(SERIAl_PORT_NAME, O_RDWR);
    tcgetattr(serialPort, &SerialPortSettings);         // Get the current attributes of the Serial port */
    cfsetispeed(&SerialPortSettings,B19200);            // Set Read  Speed as 19200                     
    cfsetospeed(&SerialPortSettings,B19200);            // Set Write Speed as 19200


    if (serialPort < 0) 
    {
        printf("Error opening serial port. Error: %i %s\n", errno, strerror(errno));
    }
    else 
    {
        logString = serialPortOpenStr;
        logEntry = true;
    }

    //------------------------------------------------------- Setup GUI Window ------------------------------------------------------- 
        
    GLFWwindow* window = setupWindow();

    // Keep window open until the 'X' button is pressed
    while (!glfwWindowShouldClose(window))
    {
        // Flush serial input buffer
        tcflush(serialPort, TCIFLUSH);

        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Setup the window to show ALIVE output values
        if(show_Serial_output)
        {
            logEntry = updateSerialOutputWindow(show_Serial_output, logEntry, logString);      
        }
       
        // Setup AER event logging window
        if (show_AER_config)
        {
            expectedResponses = setupAerWindow(show_AER_config, serialPort);

            if((expectedResponses > 0) && (show_Serial_output))
            {
                getSerialData(serialPort, expectedResponses, SERIAL_BUFFER_SIZE_AER);
                expectedResponses = 0;
            }
        }

        // Setup digital-to-analogue convertor configuration window
        if (show_DAC_config)
        {
            expectedResponses = setupDacWindow(show_DAC_config, dac, serialPort, powerOnReset);

            if((expectedResponses > 0) && (show_Serial_output))
            {
                getSerialData(serialPort, expectedResponses, SERIAL_BUFFER_SIZE_DAC);
                expectedResponses = 0;
            }
        }

        // Setup the bias generation configuration window 
        if (show_BiasGen_config)
        {
            expectedResponses = setupBiasGenWindow(show_BiasGen_config, biasGen, serialPort, relevantFileRows, valueChange_BiasGen, noRelevantFileRows, powerOnReset);

            if((expectedResponses > 0) && (show_Serial_output))
            {
                getSerialData(serialPort, expectedResponses, SERIAL_BUFFER_SIZE_BIAS);
                expectedResponses = 0;
            }
        }

        // Render the window       
        renderImGui(window);

        //
        powerOnReset = false;     

        //
        sleep(0.25);  
    }

    //-----------------------------------------------------  Graceful Shutdown ----------------------------------------------------- 

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();

    ImGui::DestroyContext();
    ImPlot::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
} 


//---------------------------------------------------------------------------------------------------------------------------------------
// getSerialData
//---------------------------------------------------------------------------------------------------------------------------------------

void getSerialData(int serialPort, int expectedResponses, int bufferSize)
{
    int serialReadBytes = 0;

    while(expectedResponses > 0)
    {
        char serialReadBuffer[bufferSize];
        std::fill(serialReadBuffer, serialReadBuffer + bufferSize, SERIAL_ASCII_SPACE);

        serialReadBytes = read(serialPort, &serialReadBuffer, bufferSize);
        
        if((serialReadBytes != 0) && (serialReadBytes != -1))
        {
            updateSerialOutputWindow(show_Serial_output, true, serialReadBuffer);
        }
        else
        {
            printf("Error reading serial port. Serial read byte: %d\n", serialReadBytes);
        }

        expectedResponses--;
    }
    
    tcflush(serialPort, TCIFLUSH);
}
