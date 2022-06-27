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

#include "../imgui/imgui_backend/imgui_impl_glfw.h"
#include "../imgui/imgui_backend/imgui_impl_opengl3.h"
#include "../imgui/imgui_src/implot.h"
#include "../imgui/imgui_src/implot_internal.h"

#include "../../teensy_backend/include/datatypes.h"

// Defining function prototypes
GLFWwindow* setupWindow();
void setupImGuiContext(GLFWwindow* window);
void renderImGui(GLFWwindow* window);

void setupDacWindow(bool show_dac_config, int DACvalue[], bool DAC_upload, DAC_command dac[], int serialPort);
void setupAerWindow(bool show_aero, bool AER_init, int serialPort);
void setupbiasGenWindow(bool show_bg_config, bool bg_upload[], int bg_address[], int bg_transistor_type[], int bg_fine_val[], int bg_course_val[], double course_current[], BIASGEN_command bg[], int serialPort);

static void glfw_error_callback(int error, const char* description);
const char* getGlslVersion();