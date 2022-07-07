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

#include "../include/constants.h"
#include "../include/guiFunctions.h"
#include "../include/dataFunctions.h"


// Defining global variables 

bool show_BiasGen_Config = true;
bool show_DAC_Config = true;
bool show_AER_Config = true;

bool show_demo_window = false;
bool AER_init = true;

//---------------------------------------------------------------------------------------------------------------------------------------
// main
//---------------------------------------------------------------------------------------------------------------------------------------

int main(int, char**)
{
    //----------------------------------- Defining & Initialising Variables ----------------------------------- 

    BIASGEN_command biasGen[BIASGEN_CHANNELS];
    DAC_command dac[DAC_CHANNELS_USED];

    std::string substring[BIASGEN_CATEGORIES] = {"DE_", "NEUR_", "SYN_A", "SYN_D", "PWEXT", "LB_", "ST_", "C2F_", "BUFFER_"};
    bool relevantFileRows[BIASGEN_CATEGORIES][BIASGEN_CHANNELS];
    int noRelevantFileRows[BIASGEN_CATEGORIES];
    std::vector<std::vector<int>> valueChange_BiasGen;

    getDACvalues(dac);
    getBiasGenValues(biasGen);

    // Creating a vector of boolean vectors to hold the bias value change variables for each bias per category
    for(int i = 0; i < BIASGEN_CATEGORIES; i++)
    {
        std::vector<int> valueChange_BiasGen_Category;
        noRelevantFileRows[i] = getRelevantFileRows_BiasGen(substring[i], biasGen, relevantFileRows[i], BIASGEN_CHANNELS);

        // Resizing the vector to prevent malloc errors
        valueChange_BiasGen_Category.resize(noRelevantFileRows[i]);

        for(int j = 0; j < noRelevantFileRows[i]; j++)
        {
            valueChange_BiasGen_Category.push_back(0);
        }
        
        valueChange_BiasGen.push_back(valueChange_BiasGen_Category);
    }
 

    //----------------------------------- Opening Serial Port ----------------------------------- 
    
    int serialPort = open(PORT_NAME, O_RDWR);

    if (serialPort < 0) 
    {
        printf("Error %i from open: %s\n", errno, strerror(errno));
    }
    else 
    {
        char port_opened[100] = "Serial port opened successfully. \n";
        printf("Port opened successfuly");
        write(serialPort, port_opened, sizeof(port_opened));
    }

    //----------------------------------- Setup GUI Window -----------------------------------
        
    GLFWwindow* window = setupWindow();

    // Keep window open until the 'X' button is pressed
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Setup demo window
        if (show_demo_window)
        {
            ImGui::ShowDemoWindow(&show_demo_window);
            ImPlot::ShowDemoWindow(&show_demo_window);
        }
        
        // Setup AER event logging window
        if (show_AER_Config)
        {
            setupAerWindow(show_AER_Config, AER_init, serialPort);
        }

        // Setup digital-to-analogue convertor configuration window
        if (show_DAC_Config)
        {
            setupDacWindow(show_DAC_Config, dac, serialPort);
        }

        // Setup the bias generation configuration window 
        if (show_BiasGen_Config)
        {
            setupBiasGenWindow(show_BiasGen_Config, biasGen, serialPort, relevantFileRows, valueChange_BiasGen, noRelevantFileRows);
        }

        // Render the window       
        renderImGui(window);
    }

    //----------------------------------- Graceful Shutdown -----------------------------------

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();

    ImGui::DestroyContext();
    ImPlot::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
} 