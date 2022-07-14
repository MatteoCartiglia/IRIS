//---------------------------------------------------------------------------------------------------------------------------------------
//
//
// Author: Ciara Giles-Doran <gciara@student.ethz.ch>
// Last updated: 
//---------------------------------------------------------------------------------------------------------------------------------------

#include <iostream>
#include "../include/guiFunctions.h"
#include "../include/dataFunctions.h"

//----------------------------------------------- Defining global variables -------------------------------------------------------------

const char *options_chipCore[ALIVE_NO_CORES] = {"Cortical Circuit", "Neural Network"};
const char *options_synapseType[ALIVE_NO_SYNAPSE_TYPES] = {"AMPA", "GABAa", "GABAb", "NMDA"};
const char *options_neuronNumber[ALIVE_NO_NEURONS] = {"1", "2", "3", "4"};
const char *options_neuronNumber_PlasticSynapses[1] = {"All Neurons"};
const char *biasGenHeaderStr[BIASGEN_CATEGORIES] = {"Alpha DPI", "Neurons", "Analogue Synapses", "Digital Synapses", "Synapse Pulse Extension", "Learning Block", "Stop Learning Block", "Current To Frequency", "Buffer"};

#ifdef BIASGEN_SET_TRANSISTOR_TYPE
    const char *options_biasGenTransistorType[2] = {"nFET", "pFET"};
#endif

int selection_chipCore = 0;
int selection_synapseType = 0;
int selection_neuronNumber = 0;
int value_synapseNumber = 0;
int selection_transistorType = 0;

bool selectionChange_chipCore = 0;
bool selectionChange_synapseType  = 0;
bool selectionChange_neuronNumber = 0;
bool selectionChange_synapseNumber = 0;
bool valueChange_DACbias[DAC_CHANNELS_USED] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

std::vector<double> inputEncoder_xValues;
std::vector<int> inputEncoder_yValues;

std::vector<double> inputC2F_xValues;
std::vector<int> inputC2F_yValues;

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

int setupDacWindow(bool show_DAC_config, DAC_command dac[], int serialPort, bool powerOnReset)
{
    int serialDataSent = 0;      
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
        int inputField_BiasValue = static_cast <int>(dac[i].data);
        inputField_BiasValue, valueChange_DACbias[i] = ImGui::InputInt(emptylabel, &inputField_BiasValue);
        dac[i].data = static_cast <std::uint16_t>(checkLimits(inputField_BiasValue, DAC_MAX_VOLTAGE)); 
        ImGui::SameLine();

        // Including units
        ImGui::Text("mV");
        ImGui::SameLine();

        // Adding a update button to write to serial port
        if((ImGui::Button("Update", ImVec2(BUTTON_UPDATE_WIDTH, BUTTON_HEIGHT))) || powerOnReset)
        {
            P2TPkt p2t_pk(dac[i]); 
            write(serialPort, (void *) &p2t_pk, sizeof(p2t_pk));
            serialDataSent++;
        }
        
        ImGui::PopID();
    }

    ImGui::End();
    return serialDataSent;
}

//---------------------------------------------------------------------------------------------------------------------------------------
// setupAerWindow
//---------------------------------------------------------------------------------------------------------------------------------------

int setupAerWindow(bool show_AER_config, int serialPort)
{
    int serialDataSent = 0;      
    ImGui::Begin("AER Packet", &show_AER_config);  

    std::string comboLabel_core_str = " CORE";
    const char *comboLabel_core = comboLabel_core_str.c_str();
    selection_chipCore, selectionChange_chipCore = ImGui::Combo(comboLabel_core, &selection_chipCore, options_chipCore, ALIVE_NO_CORES);

    std::string comboLabel_synapse_str = " SYNAPSE TYPE";
    const char *comboLabel_synapse = comboLabel_synapse_str.c_str();
    selection_synapseType, selectionChange_synapseType = ImGui::Combo(comboLabel_synapse, &selection_synapseType, options_synapseType, ALIVE_NO_SYNAPSE_TYPES);

    std::string comboLabel_neuronNumber_str = " NEURON";
    const char *comboLabel_neuronNumber = comboLabel_neuronNumber_str.c_str();

    // Plastic synapses (NMDA and GABAb) for both cores
    if((selection_synapseType == 3) || (selection_synapseType == 2))
    {
        selection_neuronNumber, selectionChange_neuronNumber = ImGui::Combo(comboLabel_neuronNumber, &selection_neuronNumber, options_neuronNumber_PlasticSynapses, 1);
    }
    // Non-plastic synapses (AMPA and GABAa) for both cores
    else 
    {
        selection_neuronNumber, selectionChange_neuronNumber = ImGui::Combo(comboLabel_neuronNumber, &selection_neuronNumber, options_neuronNumber, ALIVE_NO_NEURONS);
    }

    // Creating different labels for the synapse number to make it clear for the user if AMPA+ or AMPA- synapses have been selected in the NN core
    std::string comboLabel_synapseNumber_str;

    if((selection_chipCore == 1) && (selection_synapseType == 0))
    {
        if(value_synapseNumber < ALIVE_NO_AMPA_SYNAPSES_NN)
        {
            comboLabel_synapseNumber_str = " AMPA+ SYNAPSE NO.";
        }
        else
        {
            comboLabel_synapseNumber_str = " AMPA- SYNAPSE NO.";
        }
    }
    else
    {
        comboLabel_synapseNumber_str = " SYNAPSE NO.";
    }

    const char *comboLabel_synapseNumber = comboLabel_synapseNumber_str.c_str();
    value_synapseNumber, selectionChange_synapseNumber = ImGui::InputInt(comboLabel_synapseNumber, &value_synapseNumber);
    value_synapseNumber = checkLimits_Synapse(value_synapseNumber, selection_synapseType, selection_chipCore);   

    // Adding a "Send" button to write to serial port
    if(ImGui::Button("Send Packet to Teensy", ImVec2(BUTTON_AER_WIDTH, BUTTON_HEIGHT)))
    {
        // Creating the AER packet
        AER_DECODER_OUTPUT_command decoderOutput;
        decoderOutput.data = getAERpacket(selection_chipCore, selection_synapseType, selection_neuronNumber, value_synapseNumber);

        // std::cout << "AER packet: ";
        // printBinaryValue(decoderOutput.data, AER_PACKET_SIZE);

        P2TPkt p2t_pk(decoderOutput); 
        write(serialPort, (void *) &p2t_pk, sizeof(p2t_pk));
        serialDataSent++;
    }
    
    ImGui::End();
    return serialDataSent;
}


//---------------------------------------------------------------------------------------------------------------------------------------
// setupbiasGenWindow
//---------------------------------------------------------------------------------------------------------------------------------------
#ifdef BIASGEN_SET_TRANSISTOR_TYPE
int setupBiasGenWindow(bool show_biasGen_config, BIASGEN_command biasGen[], int serialPort, bool relevantFileRows[][BIASGEN_CHANNELS], 
    std::vector<std::vector<std::vector<int>>> selectionChange_BiasGen, int noRelevantFileRows[], bool powerOnReset)
#else
int setupBiasGenWindow(bool show_biasGen_config, BIASGEN_command biasGen[], int serialPort, bool relevantFileRows[][BIASGEN_CHANNELS], 
        std::vector<std::vector<int>> selectionChange_BiasGen, int noRelevantFileRows[], bool powerOnReset)
#endif
{
    int serialDataSent = 0;      
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
                    std::string emptylabel0_str = "##0";
                    const char *emptylabel0 = emptylabel0_str.c_str(); 

#ifdef BIASGEN_SET_TRANSISTOR_TYPE
                    // Adding an input field for changing bias value
                    ImGui::PushItemWidth(150);
                    float inputField_BiasGenValue = biasGen[j].currentValue_uV;
                    inputField_BiasGenValue, selectionChange_BiasGen[i][noRelevantFileRows[i]][0] = ImGui::InputFloat(emptylabel0, &inputField_BiasGenValue, 0.000001, 0, "%.6f", 0);
                    biasGen[j].currentValue_uV = checkLimits(inputField_BiasGenValue, BIASGEN_MAX_CURRENT); 
                    ImGui::SameLine();

                    std::string emptylabel1_str = "##1";
                    const char *emptylabel1 = emptylabel1_str.c_str(); 

                    ImGui::PushItemWidth(100);
                    selection_transistorType = biasGen[j].transistorType;
                    selection_transistorType, selectionChange_BiasGen[i][noRelevantFileRows[i]][1] = ImGui::Combo(emptylabel1, &selection_transistorType, options_biasGenTransistorType, 2);
                    biasGen[j].transistorType = selection_transistorType;
                    ImGui::SameLine();
#else
                    // Adding an input field for changing bias value
                    ImGui::PushItemWidth(260);
                    float inputField_BiasGenValue = biasGen[j].currentValue_uV;
                    inputField_BiasGenValue, selectionChange_BiasGen[i][noRelevantFileRows[i]] = ImGui::InputFloat(emptylabel0, &inputField_BiasGenValue, 0.000001, 0, "%.6f", 0);
                    biasGen[j].currentValue_uV = checkLimits(inputField_BiasGenValue, BIASGEN_MAX_CURRENT); 
                    ImGui::SameLine();
#endif
                    // Including units
                    ImGui::Text("uA");
                    ImGui::SameLine();

                    // Adding a update button to write to serial port
                    if(ImGui::Button("Update", ImVec2(BUTTON_UPDATE_WIDTH, BUTTON_HEIGHT)))
                    {
                        P2TPkt p2t_pk(biasGen[j]); 
                        write(serialPort, (void *) &p2t_pk, sizeof(p2t_pk));
                        serialDataSent++;
                    }
                    
                    ImGui::PopID();
                }
            }
        }
    }

#ifdef BIASGEN_SEND_POR

    // Initialising values at POR
    if(powerOnReset)
    {
        for(int k=0; k<BIASGEN_CHANNELS; k++)
        {
            P2TPkt p2t_pk(biasGen[k]); 
            write(serialPort, (void *) &p2t_pk, sizeof(p2t_pk));
            serialDataSent++;
        }
    }
#endif

    ImGui::End();
    return serialDataSent;
}


//---------------------------------------------------------------------------------------------------------------------------------------
// updateSerialOutputWindow
//---------------------------------------------------------------------------------------------------------------------------------------

bool updateSerialOutputWindow(bool show_Serial_output, bool logEntry, const char* logString)
{
    static Log log;
    bool logEntryUpdate = logEntry;

    std::time_t myTime = time(NULL);
    char timeBuffer[20];
    strftime(timeBuffer, 20, "%F %R", localtime(&myTime)); 

    ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
    ImGui::Begin("Log", &show_Serial_output);

    if(logEntry)
    {
        log.AddLog("[%s] %s\n", timeBuffer, logString);
        logEntryUpdate = false;
    }
    
    // Actually call in the regular Log helper (which will Begin() into the same window as we just did)
    log.Draw("Log", &show_Serial_output);
    ImGui::End();

    return logEntryUpdate;
}

//---------------------------------------------------------------------------------------------------------------------------------------
// updatePlotWindow
//---------------------------------------------------------------------------------------------------------------------------------------
void updatePlotWindow(bool updatePlot, double timeStamp, double value, int inputType)
{
    ImGui::Begin("ALIVE Output", &updatePlot);  

    // Updating the data vectors
    if(inputType == TEENSY_INPUT_ENCODER)
    {
        inputEncoder_xValues.push_back(timeStamp);
        inputEncoder_yValues.push_back(value);
    }
    else if(inputType == TEENSY_INPUT_C2F)
    {
        inputC2F_xValues.push_back(timeStamp);
        inputC2F_yValues.push_back(value);
    }

    // Converting the data vectors to arrays
    double yArray[inputEncoder_yValues.size()];
    double xArray[inputEncoder_xValues.size()];

    for(int i = 0; i < inputEncoder_xValues.size(); i++)
    {
        xArray[i] = inputEncoder_xValues[i];
        yArray[i] = inputEncoder_yValues[i];
    }

    //
    if(ImPlot::BeginPlot("Encoder Output: Firing Neuron Number"))
    {
        ImPlot::SetupAxis(ImAxis_X1, "Time", ImPlotAxisFlags_Time);
        ImPlot::SetupAxis(ImAxis_Y1, "Firing Neuron Number");
        ImPlot::SetupAxisLimits(ImAxis_Y1, 0, ALIVE_NO_NEURONS + 1, 1);
        ImPlot::SetupAxisLimits(ImAxis_X1, timeStamp - 25, timeStamp + 5, 1);
        ImPlot::PlotScatter("###", xArray, yArray, inputEncoder_yValues.size());
        ImPlot::EndPlot();
    }

    if(ImPlot::BeginPlot("C2F Output: Current Number"))
    {
        ImPlot::SetupAxis(ImAxis_X1, "Time", ImPlotAxisFlags_Time);
        ImPlot::SetupAxis(ImAxis_Y1, "Current Number");
        ImPlot::SetupAxisLimits(ImAxis_Y1, 0, C2F_INPUT_RANGE, 1);
        ImPlot::SetupAxisLimits(ImAxis_X1, timeStamp - 25, timeStamp + 5, 1);
        ImPlot::PlotScatter("###", xArray, yArray, inputEncoder_yValues.size());
        ImPlot::EndPlot();
    }

    ImGui::End();
}

//---------------------------------------------------------------------------------------------------------------------------------------
// glfw_error_callback
//---------------------------------------------------------------------------------------------------------------------------------------
void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

//---------------------------------------------------------------------------------------------------------------------------------------
// checkLimits
//---------------------------------------------------------------------------------------------------------------------------------------

float checkLimits(float value, float maxLimit, float minValue)
{
    if(value > maxLimit)
    {
        value = maxLimit;
    }
    else if(value < minValue)
    {
        value = minValue;
    }

    return value;
}

//---------------------------------------------------------------------------------------------------------------------------------------
// checkLimits_Synapse
//---------------------------------------------------------------------------------------------------------------------------------------

int checkLimits_Synapse(int value, int synapseType, int coreType)
{
    // Call checkLimits with correct maxLimit parameter

    if(selection_synapseType == 3)
    {
        value = checkLimits(value, ALIVE_NO_NMDA_SYNAPSES);
    }
    else if (selection_synapseType == 1)
    {
        value = checkLimits(value, ALIVE_NO_GABAa_SYNAPSES);
    }
    else if (selection_synapseType == 2)
    {
        value = checkLimits(value, ALIVE_NO_GABAb_SYNAPSES);
    }
    else if (selection_synapseType == 0)
    {
        value = checkLimits(value, ALIVE_NO_AMPA_SYNAPSES);
    }

    return value;
}