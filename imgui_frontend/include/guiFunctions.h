//---------------------------------------------------------------------------------------------------------------------------------------
//
//
// Author: Ciara Giles-Doran <gciara@student.ethz.ch>
// Last updated: 
//---------------------------------------------------------------------------------------------------------------------------------------

#include <GLFW/glfw3.h>     // Will drag system OpenGL headers
#include <unistd.h>         // UNIX standard function definitions
#include <stdio.h>          // Standard input output
#include <errno.h>          // Error number definitions
#include <string>
#include <vector>

#include "../imgui/imgui_backend/imgui_impl_glfw.h"
#include "../imgui/imgui_backend/imgui_impl_opengl3.h"
#include "../imgui/imgui_src/implot.h"
#include "../imgui/imgui_src/implot_internal.h"

#include "../../teensy_backend/include/datatypes.h"
#include "../include/constants.h"

/****************** Defining function prototypes ************************/

//
GLFWwindow* setupWindow();
void renderImGui(GLFWwindow* window);

//
void setupDacWindow(bool show_dac_config, DAC_command dac[], int serialPort);
void setupBiasGenWindow(bool show_biasGen_config, BIASGEN_command biasGen[], int serialPort, bool relevantFileRows[][BIASGEN_CHANNELS], 
    std::vector<std::vector<int>> selectionChange_BiasGen, int noRelevantFileRows[]);
void setupAerWindow(bool show_aero, bool AER_init, int serialPort);

//
void glfw_error_callback(int error, const char* description);

//
float checkLimits(float value, int maxLimit);
int checkLimits_Synapse(int value, int synapseType, int coreType);
