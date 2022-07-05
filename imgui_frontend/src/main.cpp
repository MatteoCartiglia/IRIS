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
#include <string>   //
#include <vector>   //
#include <fstream>  //
#include <sstream>  //
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
bool show_demo_window = false;
bool show_bg_config = false;
bool show_dac_config = true;
bool show_aero = false;
bool AER_init = false;

// Declaration of function prototypes 
std::vector<int> readbias_file(const std::string& path);

//---------------------------------------------------------------------------------------------------------------------------------------
// main
//---------------------------------------------------------------------------------------------------------------------------------------

int main(int, char**)
{
    /* ----------------------------------- Defining Variables ----------------------------------- */

    // Defining BIASGEN variables
    BIASGEN_command bg[MAX_BG_CHANNELS]; 
    bool bg_upload[MAX_BG_CHANNELS];
    int bg_address[MAX_BG_CHANNELS];
    int bg_transistor_type[MAX_BG_CHANNELS];
    int bg_fine_val[MAX_BG_CHANNELS];
    int bg_course_val[MAX_BG_CHANNELS];
    std::vector<int> biases = readbias_file(BIASGEN_POR_FILE);
    static double course_current[6]= {60*pow(10, -6), 460*pow(10, -6), 3.8*pow(10, -3), 30*pow(10, -3), 240*pow(10, -3), 1.9*pow(10, 0)}; //uA

    // Defining DAC variables
    bool DAC_upload[DAC_CHANNELS_USED];
        
    DAC_command dac[DAC_CHANNELS_USED];
    getDACvalues(dac);


    /* ----------------------------------- Initialising Command Variables ----------------------------------- */

    for (uint8_t i=0; i<MAX_BG_CHANNELS; i++) {
        bg[i].address = i;
        bg_address[i] = i;

        bg[i].transistor_type = (bias_type & biases[i]);
        bg_transistor_type[i] = (bias_type & biases[i]);

        bg[i].fine_val = (bias_fine & biases[i]) >>FINE_BIAS_SHIFT;
        bg_fine_val[i] = (bias_fine & biases[i]) >>FINE_BIAS_SHIFT;

        bg[i].course_val = (bias_course & biases[i]) >>COURSE_BIAS_SHIFT ; 
        bg_course_val[i] = (bias_course & biases[i]) >>COURSE_BIAS_SHIFT ; 
    }
   


    /* ----------------------------------- Opening Serial Port ----------------------------------- */
    
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

    /* ----------------------------------- Setup GUI Window ----------------------------------- */
        
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
        if (show_aero)
            setupAerWindow(show_aero, AER_init, serialPort);

        // Setup digital-to-analogue convertor configuration window
        if (show_dac_config)
            setupDacWindow(show_dac_config, DAC_upload, dac, serialPort);

        // Setup the bias generation configuration window 
        if (show_bg_config)
        {
            setupbiasGenWindow(show_bg_config, bg_upload, bg_address, bg_transistor_type, bg_fine_val, bg_course_val, course_current, bg, serialPort);
        }

        // Render the window       
        renderImGui(window);
    }

    /* ----------------------------------- Graceful Shutdown ----------------------------------- */

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();

    ImGui::DestroyContext();
    ImPlot::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
} 

/*---------------------------------------------------------------------------------------------------------------------------------------
* readbias_file
*----------------------------------------------------------------------------------------------------------------------------------------*/

std::vector<int> readbias_file(const std::string& path) 
{
    std::stringstream ss;
    std::ifstream input_file(path);
    std::string value;
    std::vector<int> biases;

    if (!input_file.is_open()) 
    {
        std::cout << "Could not open the file - '" << path << "'" << std::endl;
        exit(EXIT_FAILURE);
    }

    ss << input_file.rdbuf();

    while(std::getline(ss,value,'\n'))
    {
        biases.push_back(stoi(value));
    }

    return biases;
}


