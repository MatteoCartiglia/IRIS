//---------------------------------------------------------------------------------------------------------------------------------------
//
//
// Author: Ciara Giles-Doran <gciara@student.ethz.ch>
// Last updated: 
//---------------------------------------------------------------------------------------------------------------------------------------

#include <iostream>
#include "../include/guiFunctions.h"
#include "../include/constants.h"

//----------------------------------------------- Defining global variables -------------------------------------------------------------

const char *options_chipCore[AER_NO_CORES] = {"Neural Network", "Cortical Circuit"};
const char *options_synapseType[AER_NO_SYNAPSE_TYPES] = {"NMDA", "GABAa", "GABAb", "AMPA"};
const char *options_neuronNumber[AER_NO_NEURONS] = {"1", "2", "3", "4"};
const char *options_neuronNumber_PlasticSynapses[1] = {"All Neurons"};
const char *biasGenHeaderStr[BIASGEN_CATEGORIES] = {"Alpha DPI", "Neurons", "Analogue Synapses", "Digital Synapses", "Synapse Pulse Extension", "Learning Block", "Stop Learning Block", "Current To Frequency", "Buffer"};

int selection_chipCore = 0;
int selection_synapseType = 0;
int selection_neuronNumber = 0;
int value_synapseNumber = 0;

bool selectionChange_chipCore = 0;
bool selectionChange_synapseType  = 0;
bool selectionChange_neuronNumber = 0;
bool selectionChange_synapseNumber = 0;
bool valueChange_DACbias[DAC_CHANNELS_USED] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

//---------------------------------------------------------------------------------------------------------------------------------------
// setupGLFW 
//---------------------------------------------------------------------------------------------------------------------------------------

GLFWwindow* setupWindow()
{
    glfwSetErrorCallback(glfw_error_callback);

    if (!glfwInit())
    {
        fprintf(stderr, "GLFW initialization failed.\n");
        return NULL;
    }

    //------------------------------------------------Defining Platform-Specific Variables --------------------------------------------
        
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char* glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);

#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150 --> MC using this
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac

#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    //----------------------------------------------- Creating Window With Graphics Context ---------------------------------------------

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "ALIVE Testing Interface", NULL, NULL);

    if (!window)
    {
        glfwTerminate();
        fprintf(stderr, "Error creating window.\n");
        return NULL;
    }
        
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); 

    //------------------------------------------------------- Setting up ImGui ----------------------------------------------------------

    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    //---------------------------------------------- Setting up Platform/Renderer backends ----------------------------------------------

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    
    return window;
}


//---------------------------------------------------------------------------------------------------------------------------------------
// renderImGui
//----------------------------------------------------------------------------------------------------------------------------------------
void renderImGui(GLFWwindow* window)
{
    int display_w;
    int display_h;
    ImVec4 clear_color = ImVec4(CLEAR_COLOUR_X, CLEAR_COLOUR_Y, CLEAR_COLOUR_Z, CLEAR_COLOUR_W);

    // Rendering
    ImGui::Render();

    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);

    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);
}

//---------------------------------------------------------------------------------------------------------------------------------------
// setupDacWindow
//----------------------------------------------------------------------------------------------------------------------------------------

void setupDacWindow(bool show_DAC_config, DAC_command dac[], int serialPort)
{
    ImGui::Begin("Test Structure Biases [DAC Values]", &show_DAC_config);      

    for(int i=0; i<DAC_CHANNELS_USED; i++)
    {
        // Using push/pop ID to create unique identifiers for widgets with the same label
        ImGui::PushID(i);

        // Labelling the DAC input channel
        ImGui::Text(dac[i].name.c_str());
        ImGui::SameLine();

        // Setting an invisible label for the input field
        std::string emptylabel_str = "##";
        const char *emptylabel = emptylabel_str.c_str(); 
        
        // Adding an input field for changing bias value
        ImGui::PushItemWidth(120);
        int inputField_BiasValue = dac[i].data;
        dac[i].data, valueChange_DACbias[i] = ImGui::InputInt(emptylabel, &inputField_BiasValue);
        dac[i].data = checkLimits(dac[i].data, DAC_MAX_VOLTAGE); 
        ImGui::SameLine();

        // Including units
        ImGui::Text("mV");
        ImGui::SameLine();

        // Adding a update button to write to serial port
        if(ImGui::Button("Update", ImVec2(BUTTON_UPDATE_WIDTH, BUTTON_HEIGHT)))
        {
            P2TPkt p2t_pk(dac[i]); 
            write(serialPort, (void *) &p2t_pk, sizeof(p2t_pk));
        }
        
        ImGui::PopID();
    }

    ImGui::End();
}

//---------------------------------------------------------------------------------------------------------------------------------------
// setupAerWindow
//---------------------------------------------------------------------------------------------------------------------------------------

void setupAerWindow(bool show_aero, bool AER_init, int serialPort)
{
    ImGui::Begin("AER Packet", &show_aero);  

    std::string comboLabel_core_str = "   CORE";
    const char *comboLabel_core = comboLabel_core_str.c_str();
    selection_chipCore, selectionChange_chipCore = ImGui::Combo(comboLabel_core, &selection_chipCore, options_chipCore, AER_NO_CORES);

    std::string comboLabel_synapse_str = "   SYNAPSE TYPE";
    const char *comboLabel_synapse = comboLabel_synapse_str.c_str();
    selection_synapseType, selectionChange_synapseType = ImGui::Combo(comboLabel_synapse, &selection_synapseType, options_synapseType, AER_NO_SYNAPSE_TYPES);

    std::string comboLabel_neuronNumber_str = "   NEURON";
    const char *comboLabel_neuronNumber = comboLabel_neuronNumber_str.c_str();

    // Plastic NMDA synapses for both cores
    if((selection_synapseType == 0) || (selection_synapseType == 2))
    {
        selection_neuronNumber, selectionChange_neuronNumber = ImGui::Combo(comboLabel_neuronNumber, &selection_neuronNumber, options_neuronNumber_PlasticSynapses, 1);
    }
    // Non-plastic synapses (AMPA and GABAa) for both cores
    else 
    {
        selection_neuronNumber, selectionChange_neuronNumber = ImGui::Combo(comboLabel_neuronNumber, &selection_neuronNumber, options_neuronNumber, AER_NO_NEURONS);
    }

    std::string comboLabel_synapseNumber_str = "   SYNAPSE NO.";
    const char *comboLabel_synapseNumber = comboLabel_synapseNumber_str.c_str();
    value_synapseNumber, selectionChange_synapseNumber = ImGui::InputInt(comboLabel_synapseNumber, &value_synapseNumber);
    value_synapseNumber = checkLimits_Synapse(value_synapseNumber, selection_synapseType, selection_chipCore);   

    // Adding a "Send" button to write to serial port
    ImGui::Button("Send Packet to Teensy", ImVec2(BUTTON_AER_WIDTH, BUTTON_HEIGHT));
    ImGui::End();
}


//---------------------------------------------------------------------------------------------------------------------------------------
// setupbiasGenWindow
//----------------------------------------------------------------------------------------------------------------------------------------

void setupBiasGenWindow(bool show_biasGen_config, BIASGEN_command biasGen[], int serialPort, bool relevantFileRows[][BIASGEN_CHANNELS], 
        std::vector<std::vector<int>> selectionChange_BiasGen, int noRelevantFileRows[])
{
    ImGui::Begin("Bias Generator Configuration", &show_biasGen_config);

    for(int i = 0; i < BIASGEN_CATEGORIES; i++)
    {
        if(ImGui::CollapsingHeader(biasGenHeaderStr[i]))
        {
            for(int j=0; j<BIASGEN_CHANNELS; j++)
            {
                if(relevantFileRows[i][j] == 1)
                {
                    // Using push/pop ID to create unique identifiers for widgets with the same label
                    ImGui::PushID(j);

                    // Labelling the DAC input channel
                    ImGui::Text(biasGen[j].name.c_str());
                    ImGui::SameLine();

                    // Setting an invisible label for the input field
                    std::string emptylabel_str = "##";
                    const char *emptylabel = emptylabel_str.c_str(); 
                    
                    // Adding an input field for changing bias value
                    ImGui::PushItemWidth(150);
                    biasGen[j].currentValue_uV, selectionChange_BiasGen[i][noRelevantFileRows[i]] = ImGui::InputFloat(emptylabel, &biasGen[j].currentValue_uV, 0.000001, 0, "%.6f", 0);
                    biasGen[j].currentValue_uV = checkLimits(biasGen[j].currentValue_uV, BIASGEN_MAX_VOLTAGE); 
                    ImGui::SameLine();

                    // Including units
                    ImGui::Text("uA");
                    ImGui::SameLine();

                    // Adding a update button to write to serial port
                    if(ImGui::Button("Update", ImVec2(BUTTON_UPDATE_WIDTH, BUTTON_HEIGHT)))
                    {
                        P2TPkt p2t_pk(biasGen[j]); 
                        write(serialPort, (void *) &p2t_pk, sizeof(p2t_pk));
                    }
                    
                    ImGui::PopID();
                }
            }
        }
    }

    ImGui::End();
}


//---------------------------------------------------------------------------------------------------------------------------------------
// glfw_error_callback
//----------------------------------------------------------------------------------------------------------------------------------------
void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

//---------------------------------------------------------------------------------------------------------------------------------------
// checkLimits
//----------------------------------------------------------------------------------------------------------------------------------------

float checkLimits(float value, int maxLimit)
{
    if(value > maxLimit)
    {
        value = maxLimit;
    }
    else if (value < 0)
    {
        value = 0;
    }

    return value;
}

//---------------------------------------------------------------------------------------------------------------------------------------
// checkLimits_Synapse
//---------------------------------------------------------------------------------------------------------------------------------------

int checkLimits_Synapse(int value, int synapseType, int coreType)
{
    // Call checkLimits with correct maxLimit parameter

    if(selection_synapseType == 0)
    {
        value = checkLimits(value, AER_NO_NMDA_SYNAPSES);
    }
    else if (selection_synapseType == 1)
    {
        value = checkLimits(value, AER_NO_GABAa_SYNAPSES);
    }
    else if (selection_synapseType == 2)
    {
        value = checkLimits(value, AER_NO_GABAb_SYNAPSES);
    }
    else if ((selection_synapseType == 3) && (coreType == 0))
    {
        value = checkLimits(value, AER_NO_AMPA_SYNAPSES_CC);
    }
    else if ((selection_synapseType == 3) && (coreType == 1))
    {
        value = checkLimits(value, AER_NO_AMPA_SYNAPSES_NN);
    }

    return value;
}