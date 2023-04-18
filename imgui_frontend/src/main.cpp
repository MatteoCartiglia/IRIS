//---------------------------------------------------------------------------------------------------------------------------------------
// main.cpp file containing main function and serial port reading operations
//---------------------------------------------------------------------------------------------------------------------------------------

#include <GLFW/glfw3.h>     // Will drag system OpenGL headers
#include <unistd.h>         // UNIX standard function definitions
#include <errno.h>          // Error number definitions
#include <fcntl.h>          // File control definitions
#include <experimental/filesystem>
#include <stdio.h>
#include <string>   
#include <vector>   
#include <cstddef>
#include <iostream>
#include <chrono>
#include <thread>

#include "../imgui/imgui_backend/imgui_impl_opengl3.h"
#include "../imgui/imgui_backend/imgui_impl_glfw.h"
#include "../../teensy_backend/include/datatypes.h"
#include "../imgui/imgui_src/implot.h"
#include "../imgui/imgui_src/implot_internal.h"

#include "../include/serial.h"
#include "../include/guiFunctions.h"
#include "../include/dataFunctions.h"
#include "../include/serialFunctions.h"
#include "../../teensy_backend/include/constants.h"
#include "../../teensy_backend/include/constants_global.h"


//---------------------------------------------------------------------------------------------------------------------------------------
// main: program execution starts here
//---------------------------------------------------------------------------------------------------------------------------------------

int main(int, char**)
{
    //----------------------------------------------- Defining & Initialising BiasGen Variables -----------------------------------------

#ifdef EXISTS_BIASGEN
    bool show_BiasGen_config = true;

    BIASGEN_command biasGen[BIASGEN_CHANNELS];
    getBiasValues(biasGen,  BIASGEN_BIASFILE);

    std::string substring[BIASGEN_CATEGORIES] = {"DE_", "NEUR_", "SYN_A", "SYN_D", "PWEXT", "LB_", "C2F_", "BUFFER_"};
    bool relevantFileRows[BIASGEN_CATEGORIES][BIASGEN_CHANNELS];
    int noRelevantFileRows[BIASGEN_CATEGORIES];
    bool updateValues_BiasGen = false;

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
#else    
    bool show_BiasGen_config = false;
#endif

    //------------------------------------------------ Defining & Initialising SPI Variables ------------------------------------------
#ifdef EXISTS_SPI1
    bool show_SPI1_config = true;
    SPI_INPUT_command spi_command[1];
    spi_command[0].spi_number = 1;
    spi_command[0].value = 100;
    spi_command[0].address = 200;
#endif

#ifdef EXISTS_SPI2
    bool show_SPI2_config = true;
    SPI_INPUT_command spi2_command[1];
    spi2_command[0].spi_number = 2;
    spi2_command[0].value = 1;
    spi2_command[0].address = 200;
#endif

    //--------------------------------------------- Defining & Initialising Decoder Variables -----------------------------------------

#ifdef EXISTS_OUTPUT_DECODER
    bool show_AER_config = true;
#endif

    //------------------------------------------------ Defining & Initialising DAC Variables ------------------------------------------

#ifdef EXISTS_DAC
    bool show_DAC_config = true;
    bool updateValues_DAC = true;
    DAC_command dac[DAC_CHANNELS_USED];
    getBiasValues(dac, DAC_BIASFILE);
#endif
    
#ifdef EXISTS_ENCODER
    bool show_Encoder = true;

#endif
#ifdef EXISTS_C2F
    bool show_C2F = true;

#endif

    //--------------------------------------------- Defining & Initialising All Other Variables -------------------------------------- 
    
    bool show_Serial_output = true;
           
    auto time = std::time(nullptr);
    auto time_tm = *std::localtime(&time);
    std::ostringstream outputTimeString;
    outputTimeString << std::put_time(&time_tm, "_%d_%m_%H_%M");
    auto timeString = outputTimeString.str();

    const char* logString;
    bool logEntry = false;

    char serialPortOpenStr[SERIAL_BUFFER_SIZE_PORT_OPEN] = {"Serial port opened successfully.\n"};
    char serialPortErrorStr[SERIAL_BUFFER_SIZE_PORT_ERROR] = {"Serial port NOT opened. No device found."};
    int expectedResponses = 0;

    //--------------------------------------------------------- Opening Serial Port ------------------------------------------------------
    
    Serial sPort;
    int serialPort = sPort.getFileDescriptor();
    logString = (serialPort != -1)? serialPortOpenStr : serialPortErrorStr;
    logEntry = true;

    //----------------------------------------------------------- Setup GUI Window ------------------------------------------------------- 
        
    GLFWwindow* window = setupWindow();
    ImGuiIO& io = ImGui::GetIO(); (void)io;


    // Keep window open until the 'X' button is pressed
    while (!glfwWindowShouldClose(window))
    {
        // Flush serial input buffer
        tcflush(sPort.getFileDescriptor(), TCIFLUSH);

        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        io.DeltaTime = 1.0f/60.0f;

        ImGui::NewFrame();
        
        setupResetWindow(1, sPort);     

        // Setup the window to show output values
        if(show_Serial_output)
        {
            logEntry = updateSerialOutputWindow(show_Serial_output, logEntry, logString);      
        }
#ifdef EXISTS_OUTPUT_DECODER
        // Setup AER event logging window
        if (show_AER_config)
        {
            setupAerWindow(show_AER_config, sPort);
        }
#endif
        // Setup digital-to-analogue convertor configuration window - ok!
#ifdef EXISTS_DAC
        if (show_DAC_config)
        {
            expectedResponses = setupDacWindow(show_DAC_config, dac, sPort, updateValues_DAC);
            char logEntry[SERIAL_BUFFER_SIZE_DAC];
            
            while(expectedResponses > 0)
            {

                sPort.readSerialPort(expectedResponses, SERIAL_BUFFER_SIZE_DAC, logEntry);
                updateSerialOutputWindow(show_Serial_output, true, logEntry);

                expectedResponses--;
            }

            tcflush(sPort.getFileDescriptor(), TCIFLUSH);
        }
#endif

        // Setup the bias generation configuration window 
#ifdef EXISTS_BIASGEN
        if (show_BiasGen_config)
        {
            expectedResponses = setupBiasGenWindow(show_BiasGen_config, biasGen, sPort, relevantFileRows, valueChange_BiasGen, noRelevantFileRows, updateValues_BiasGen);
            char logEntry[SERIAL_BUFFER_SIZE_BIAS];

            while(expectedResponses > 0)
            {
                sPort.readSerialPort(expectedResponses, SERIAL_BUFFER_SIZE_BIAS, logEntry);
                updateSerialOutputWindow(show_Serial_output, true, logEntry);

                expectedResponses--;
            }
            
            tcflush(sPort.getFileDescriptor(), TCIFLUSH);
        }
#endif

       // Setup the SPI1 configuration window 
#ifdef EXISTS_SPI1
        if (show_SPI1_config)
        {
            expectedResponses = setupSPI1Window(show_SPI1_config, sPort, spi_command, SPI1_RESOLUTION);
            char logEntry[SPI1_RESOLUTION];

            while(expectedResponses > 0)
            {
                sPort.readSerialPort(expectedResponses, SPI1_RESOLUTION, logEntry);
                updateSerialOutputWindow(show_Serial_output, true, logEntry);

                expectedResponses--;
            }
            
            tcflush(sPort.getFileDescriptor(), TCIFLUSH);
        }
#endif

       // Setup the SPI2 configuration window 
#ifdef EXISTS_SPI2

        if (show_SPI2_config)
        {
            expectedResponses = setupSPI2Window(show_SPI2_config, sPort, spi2_command, SPI2_RESOLUTION);
            char logEntry[SPI2_RESOLUTION];

            while(expectedResponses > 0)
            {
                sPort.readSerialPort(expectedResponses, SPI2_RESOLUTION, logEntry);
                updateSerialOutputWindow(show_Serial_output, true, logEntry);

                expectedResponses--;
            }
            
            tcflush(sPort.getFileDescriptor(), TCIFLUSH);
        }
#endif




#ifdef EXISTS_C2F
  // Encoder outputs
        if(show_C2F)
        {
            getSerialData_C2F(serialPort, show_C2F);
            
        }
#endif
#ifdef EXISTS_ENCODER

  // Encoder outputs
        if(show_Encoder)
        {
            getEncoderdata(serialPort, show_Encoder);
            
        }
#endif
        // Render the window       
        renderImGui(window);
#ifdef EXISTS_DAC
        updateValues_DAC = false;
#endif
  #ifdef EXISTS_BIASGEN
       
        updateValues_BiasGen  = false;  
 #endif
  
        // sleep(0.25);  
    }

    //---------------------------------------------------------- Graceful Shutdown ------------------------------------------------------- 

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();

    ImGui::DestroyContext();
    ImPlot::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    //-------------------------------------------------------- Saving Files Correctly ---------------------------------------------------- 

    /*if(std::filesystem::exists(C2F_INPUT_SAVE_FILENAME_CSV))
    {
        std::string newName = C2F_INPUT_SAVE_FILENAME + timeString + ".csv";
        rename(C2F_INPUT_SAVE_FILENAME_CSV, newName.c_str());
    }

    if(std::filesystem::exists(ENCODER_INPUT_SAVE_FILENAME_CSV))
    {
        std::string newName = ENCODER_INPUT_SAVE_FILENAME + timeString + ".csv";;
        rename(ENCODER_INPUT_SAVE_FILENAME_CSV, newName.c_str());
    }*/

    return 0;
} 
