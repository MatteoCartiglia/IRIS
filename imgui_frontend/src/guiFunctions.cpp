//---------------------------------------------------------------------------------------------------------------------------------------
// Source file for functions related to graphical user interface (GUI)
//
// Author: Ciara Giles-Doran <gciara@student.ethz.ch>
// Last updated: 15 JUL 2022
//---------------------------------------------------------------------------------------------------------------------------------------

#include <iostream>
#include "../include/guiFunctions.h"
#include "../include/dataFunctions.h"
#include "../../teensy_backend/include/constants.h"
#include <experimental/filesystem>
#include <chrono>
#include <thread>
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
bool valueChange_SPIbias_1[2] = {0, 0};
bool valueChange_SPIbias_2[2] = {0, 0};

std::vector<double> inputEncoder_xValues;
std::vector<int> inputEncoder_yValues;

std::vector<double> inputC2F_xValues;
std::vector<int> inputC2F_yValues;

namespace fs = std::experimental::filesystem;


//---------------------------------------------------------------------------------------------------------------------------------------
// saveBiases_dac: Helper fuction to save dac biases
//---------------------------------------------------------------------------------------------------------------------------------------

bool saveBiases_dac(char *filename, DAC_command dac[]){
    std::ofstream fout(filename);
    if(fout.is_open())
    {
        fout << "biasName, value_(mV), address" << '\n'; 
        for (int i = 0; i < DAC_CHANNELS_USED; i++) 
        {
            fout << dac[i].name << ','; 
            fout << dac[i].data << ','; 
            fout << dac[i].command_address; 
            // Not saving correctly. 4 bits command+ 4 bits address?
            fout << '\n'; 

        }
        return true;
    }
    else 
        return false;
}
//---------------------------------------------------------------------------------------------------------------------------------------
// saveBiases_bg: Helper fuction to save BG biases
//---------------------------------------------------------------------------------------------------------------------------------------

bool saveBiases_bg(char *filename, BIASGEN_command bg []){
    std::ofstream fout(filename);
    if(fout.is_open())
    {
        fout << "biasName, value_uA, transistorType, biasNo" << '\n'; 
        for (int i = 0; i < BIASGEN_CHANNELS; i++) 
        {
            fout << bg[i].name << ','; 
            fout << bg[i].currentValue_uA << ','; 
            fout << bg[i].transistorType <<',';
            fout << bg[i].biasNo; 
            fout << '\n'; 

        }
        return true;
    }
    else 
        return false;
}


//---------------------------------------------------------------------------------------------------------------------------------------
// setupWindow: Defines platform-specific variables, creates GUI window, initialises ImGui and ImPlot contexts and sets up 
//              platform/renderer backends
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
// renderImGui: Completes rendering operations for ImGui and GLFW
//---------------------------------------------------------------------------------------------------------------------------------------
void renderImGui(GLFWwindow* window)
{
    int display_w;
    int display_h;
    ImVec4 clear_color = ImVec4(CLEAR_COLOUR_X, CLEAR_COLOUR_Y, CLEAR_COLOUR_Z, CLEAR_COLOUR_W);

    ImGui::Render();

    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);

    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);
}

//---------------------------------------------------------------------------------------------------------------------------------------
// setupDacWindow: Initialises and updates GUI window displaying DAC values to send
//---------------------------------------------------------------------------------------------------------------------------------------

int setupDacWindow(bool show_DAC_config, DAC_command dac[], int serialPort, bool powerOnReset)
{
    int serialDataSent = 0;      
    ImGui::Begin("Test Structure Biases [DAC Values]", &show_DAC_config);

// Save and Load new biases 
    if (ImGui::Button("Save", ImVec2(BUTTON_UPDATE_WIDTH, BUTTON_HEIGHT)))
        ImGui::OpenPopup("Saving DAC Menu");
    bool opened_save = true;
    if (ImGui::BeginPopupModal("Saving DAC Menu", &opened_save))
        {
            ImGui::Text("Name of biases");
            static char filename[128] = "data/DAC_biases/NAME_dac.csv";
            ImGui::InputText(" ", filename, IM_ARRAYSIZE(filename));
            ImGui::SameLine();
            if (ImGui::Button("Save DAC values", ImVec2(BUTTON_UPDATE_WIDTH, BUTTON_HEIGHT)))
            {
                int saved_biases = saveBiases_dac(filename, dac);
                ImGui::CloseCurrentPopup();
            } 
        if (ImGui::Button("Close"))
            (ImGui::CloseCurrentPopup());
        ImGui::EndPopup(); 
        }
    ImGui::SameLine();
    // Loading DAC biases
    if (ImGui::Button("Load", ImVec2(BUTTON_UPDATE_WIDTH, BUTTON_HEIGHT)))
            ImGui::OpenPopup("Loading DAC Menu" );
    std::string path = "data/DAC_biases/";
    bool opened_load = true;
    if (ImGui::BeginPopupModal("Loading DAC Menu", &opened_load))
    {
        ImGui::Text("Select biases to load ");  
        /// See how many files are in the directory
        int n_files = 0; 
        for (const auto& dirEntry: fs::directory_iterator(path))
        {
            std::string filename_with_path = dirEntry.path();
            n_files++;
        }
        //// Put filenames into array of chars.
        char* biases_filenames_no_path [n_files];
        char* biases_filenames_with_path [n_files];
        int iter = 0;
        for (const auto& dirEntry: fs::directory_iterator(path))
        {
            std::string filename_with_path = dirEntry.path();
            std::string filename_no_path = filename_with_path.substr(filename_with_path.find('/') + 1, filename_with_path.length() - filename_with_path.find('/'));
            
            biases_filenames_with_path[iter] = new char[filename_with_path.length() + 1];
            strcpy(biases_filenames_with_path[iter], filename_with_path.c_str());
            
            biases_filenames_no_path[iter] = new char[filename_no_path.length() + 1];
            strcpy(biases_filenames_no_path[iter], filename_no_path.c_str());

            iter++;
        }
        for (int i = 0; i < n_files; i++)
        {
            ImGui::PushID(i);

            if (i % 3 != 0)
                ImGui::SameLine();
            if (ImGui::Button(biases_filenames_no_path[i])){
                getDACvalues(dac, (const std::string) biases_filenames_with_path[i]);
                powerOnReset= true;
//                loadDACvalues(dac, serialPort);
                ImGui::CloseCurrentPopup;
            }
            ImGui::PopID();
        }    
        ImGui::NewLine();
        ImGui::NewLine();
        if (ImGui::Button("Close"))
            (ImGui::CloseCurrentPopup());
        ImGui::EndPopup(); 
    }



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
        if((ImGui::Button("Update", ImVec2(BUTTON_UPDATE_WIDTH, BUTTON_HEIGHT))) ||powerOnReset )
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
// setupAerWindow: Initialises and updates GUI window displaying AER values to send
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
    value_synapseNumber = checkLimits_Synapse(value_synapseNumber, selection_synapseType);   

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
// setupBiasGenWindow: Initialises and updates GUI window displaying Bias Generator values to send. 
//                     #ifdef condition used to define different definition if transistor type option to be displayed and handled
//---------------------------------------------------------------------------------------------------------------------------------------

#ifdef BIASGEN_SET_TRANSISTOR_TYPE
int setupBiasGenWindow(bool show_biasGen_config, BIASGEN_command biasGen[], int serialPort, bool relevantFileRows[][BIASGEN_CHANNELS], 
    std::vector<std::vector<std::vector<int>>> selectionChange_BiasGen, int noRelevantFileRows[],bool powerOnReset)
#else
int setupBiasGenWindow(bool show_biasGen_config, BIASGEN_command biasGen[], int serialPort, bool relevantFileRows[][BIASGEN_CHANNELS], 
        std::vector<std::vector<int>> selectionChange_BiasGen, int noRelevantFileRows[], bool powerOnReset)
#endif
{
    int serialDataSent = 0;      
    ImGui::Begin("Bias Generator Configuration", &show_biasGen_config);


    // Save and Load new biases 
    if (ImGui::Button("Save", ImVec2(BUTTON_UPDATE_WIDTH, BUTTON_HEIGHT)))
        ImGui::OpenPopup("Saving biasgen Menu");
    bool opened_save = true;
    if (ImGui::BeginPopupModal("Saving biasgen Menu", &opened_save))
        {
            ImGui::Text("Name of biases");
            static char filename[128] = "data/BIASGEN_biases/NAME_dac.csv";
            ImGui::InputText(" ", filename, IM_ARRAYSIZE(filename));
            ImGui::SameLine();
            if (ImGui::Button("Save BIASGEN values", ImVec2(BUTTON_UPDATE_WIDTH, BUTTON_HEIGHT)))
            {
                int saved_biases = saveBiases_bg(filename, biasGen);
                ImGui::CloseCurrentPopup();
            } 
        if (ImGui::Button("Close"))
            (ImGui::CloseCurrentPopup());
        ImGui::EndPopup(); 
        }
    ImGui::SameLine();
    // Loading DAC biases
    if (ImGui::Button("Load", ImVec2(BUTTON_UPDATE_WIDTH, BUTTON_HEIGHT)))
            ImGui::OpenPopup("Loading BG Menu" );
    std::string path = "data/BIASGEN_biases/";
    bool opened_load = true;
        if (ImGui::BeginPopupModal("Loading BG Menu", &opened_load))
        {
            ImGui::Text("Select biases to load ");  
            /// See how many files are in the directory
            int n_files = 0; 
            for (const auto& dirEntry: fs::directory_iterator(path))
            {
                std::string filename_with_path = dirEntry.path();
                n_files++;
            }
            //// Put filenames into array of chars.
            char* biases_filenames_no_path [n_files];
            char* biases_filenames_with_path [n_files];
            int iter = 0;
            for (const auto& dirEntry: fs::directory_iterator(path))
            {
                std::string filename_with_path = dirEntry.path();
                std::string filename_no_path = filename_with_path.substr(filename_with_path.find('/') + 1, filename_with_path.length() - filename_with_path.find('/'));
                biases_filenames_with_path[iter] = new char[filename_with_path.length() + 1];
                strcpy(biases_filenames_with_path[iter], filename_with_path.c_str());
                biases_filenames_no_path[iter] = new char[filename_no_path.length() + 1];
                strcpy(biases_filenames_no_path[iter], filename_no_path.c_str());
                iter++;
            }
            for (int i = 0; i < n_files; i++)
            {
                ImGui::PushID(i);

                if (i % 3 != 0)
                    ImGui::SameLine();
                if (ImGui::Button(biases_filenames_no_path[i]))
                {

                    getBiasGenValues(biasGen, biases_filenames_no_path[i]);
                    powerOnReset = true;
                   // loadBiasGenValues(biasGen, serialPort);
                    ImGui::CloseCurrentPopup;

                }
                ImGui::PopID();
            }    
            ImGui::NewLine();
            ImGui::NewLine();

            if (ImGui::Button("Close"))
                (ImGui::CloseCurrentPopup());
            ImGui::EndPopup(); 
        }    

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
                    float inputField_BiasGenValue = biasGen[j].currentValue_uA;
                    inputField_BiasGenValue, selectionChange_BiasGen[i][noRelevantFileRows[i]] = ImGui::InputFloat(emptylabel0, &inputField_BiasGenValue, 0.000001, 0, "%.6f", 0);
                    biasGen[j].currentValue_uA = checkLimits(inputField_BiasGenValue, BIASGEN_MAX_CURRENT); 
                    biasGen[j].currentValue_binary = getBiasGenPacket(biasGen[j].currentValue_uA, biasGen[j].transistorType);
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

    ImGui::End();
    return serialDataSent;
}


// SPI CONTROL WINDOW

int setupSPI1Window(bool show_SPI_config, int serialPort, SPI_INPUT_command spi[], int resolution)
{
    int serialDataSent = 0;  
   
    ImGui::Begin("Configurations of SPI 1:", &show_SPI_config);
    
    ImGui::Text("Value: ");
    ImGui::SameLine();
    
    // Setting an invisible label for the input field
    std::string emptylabel_str_v1 = "##_value1";
    const char *emptylabel_v1 = emptylabel_str_v1.c_str();

    ImGui::PushItemWidth(120);
    int inputField_value = static_cast <int>(spi[0].value);
    inputField_value,valueChange_SPIbias_1[0] = ImGui::InputInt(emptylabel_v1, &inputField_value);
    spi[0].value = static_cast <std::uint16_t>(checkLimits(inputField_value, resolution));
    
    ImGui::Text(" Address:");
    ImGui::SameLine();

    // Setting an invisible label for the input field
    std::string emptylabel_str_a1 = "##_";
    const char *emptylabel_a1 = emptylabel_str_a1.c_str();

    ImGui::PushItemWidth(120);
    int inputField_add = static_cast <int>(spi[0].address);
    inputField_add,valueChange_SPIbias_1[1] = ImGui::InputInt(emptylabel_a1, &inputField_add);
    spi[0].address = static_cast <std::uint16_t>(checkLimits(inputField_add, resolution));
    ImGui::SameLine();
    
    if(ImGui::Button("Update", ImVec2(BUTTON_UPDATE_WIDTH, BUTTON_HEIGHT)))  
    {
        P2TPkt p2t_pk(spi[0]); 
        write(serialPort, (void *) &p2t_pk, sizeof(p2t_pk));
        serialDataSent++;
    }
    ImGui::End();
    return serialDataSent;
}


int setupSPI2Window(bool show_SPI_config, int serialPort, SPI_INPUT_command spi[], int resolution)
{
    int serialDataSent = 0;  
   
    ImGui::Begin("Configurations of SPI 2", &show_SPI_config);
    
    ImGui::Text("Value: ");
    ImGui::SameLine();
    
    // Setting an invisible label for the input field
    std::string emptylabel_str_v2 = "##_3";
    const char *emptylabel_v2 = emptylabel_str_v2.c_str();

    ImGui::PushItemWidth(120);
    int inputField_value = static_cast <int>(spi[0].value);
    inputField_value,valueChange_SPIbias_2[0] = ImGui::InputInt(emptylabel_v2, &inputField_value);
    spi[0].value = static_cast <std::uint16_t>(checkLimits(inputField_value, resolution));
    
    ImGui::Text(" Address:");
    ImGui::SameLine();

    // Setting an invisible label for the input field
    std::string emptylabels_str_a2 = "##_4";
    const char *emptylabels_a2 = emptylabels_str_a2.c_str();

    ImGui::PushItemWidth(120);

    int inputField_add = static_cast <int>(spi[0].address);
    inputField_add,valueChange_SPIbias_2[1] = ImGui::InputInt(emptylabels_a2, &inputField_add);
    spi[0].address = static_cast <std::uint16_t>(checkLimits(inputField_add, resolution));
    ImGui::SameLine();
    
    if(ImGui::Button("Update", ImVec2(BUTTON_UPDATE_WIDTH, BUTTON_HEIGHT)))  
    {
        P2TPkt p2t_pk(spi[0]); 
        write(serialPort, (void *) &p2t_pk, sizeof(p2t_pk));
        serialDataSent++;
    }
    ImGui::End();
    return serialDataSent;
}


//---------------------------------------------------------------------------------------------------------------------------------------
// updateSerialOutputWindow: Writes serial input to Log window in GUI
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
    
    // Call in the regular Log helper (which will Begin() into the same window)
    log.Draw("Log", &show_Serial_output);
    ImGui::End();

    return logEntryUpdate;
}


//---------------------------------------------------------------------------------------------------------------------------------------
// updatePlotWindow: Initialises and updates GUI window displaying live output from ALIVE
//---------------------------------------------------------------------------------------------------------------------------------------

void updatePlotWindow(bool updatePlot, long timeStamp, double value, int inputType)
{
    long valuesToSave[2] = {timeStamp, long(value)};
    double timeStamp_s = timeStamp/1000;

    ImGui::Begin("ALIVE Output", &updatePlot);  

    // Updating the data vectors
    if(inputType == TEENSY_INPUT_ENCODER)
    {
        inputEncoder_xValues.push_back(timeStamp_s);
        inputEncoder_yValues.push_back(value);
    }
    else if(inputType == TEENSY_INPUT_C2F)
    {
        inputC2F_xValues.push_back(timeStamp_s);
        inputC2F_yValues.push_back(value);
    }

    // Converting the data vectors to arrays
    double yArray[inputEncoder_yValues.size()];
    double xArray[inputEncoder_xValues.size()];

    for(int i = 0; i < int(inputEncoder_xValues.size()); i++)
    {
        xArray[i] = inputEncoder_xValues[i];
        yArray[i] = inputEncoder_yValues[i];
    }

    //
    if((inputType == TEENSY_INPUT_ENCODER) && (ImPlot::BeginPlot("Encoder Output: Firing Neuron Number")))
    {
        ImPlot::GetStyle().UseLocalTime;
        ImPlot::GetStyle().Use24HourClock;

        ImPlot::SetupAxis(ImAxis_X1, "Time", ImPlotAxisFlags_Time);
        ImPlot::SetupAxis(ImAxis_Y1, "Firing Neuron Number");
        ImPlot::SetupAxisLimits(ImAxis_Y1, 0, ALIVE_NO_NEURONS + 1, 1);
        ImPlot::SetupAxisLimits(ImAxis_X1, timeStamp_s - 25, timeStamp_s + 5, 1);
        ImPlot::PlotScatter("###", xArray, yArray, inputEncoder_yValues.size());
        ImPlot::EndPlot();
        saveToCSV(valuesToSave, 2, ENCODER_INPUT_SAVE_FILENAME_CSV);
    }

    if((inputType == TEENSY_INPUT_C2F) && (ImPlot::BeginPlot("C2F Output: Current Number")))
    {
        ImPlot::SetupAxis(ImAxis_X1, "Time", ImPlotAxisFlags_Time);
        ImPlot::SetupAxis(ImAxis_Y1, "Current Number");
        ImPlot::SetupAxisLimits(ImAxis_Y1, 0, C2F_INPUT_RANGE, 1);
        ImPlot::SetupAxisLimits(ImAxis_X1, timeStamp_s - 25, timeStamp_s + 5, 1);
        ImPlot::PlotScatter("###", xArray, yArray, inputEncoder_yValues.size());
        ImPlot::EndPlot();
        saveToCSV(valuesToSave, 2, C2F_INPUT_SAVE_FILENAME_CSV);
    }

    ImGui::End();
}


//---------------------------------------------------------------------------------------------------------------------------------------
// checkLimits: Checks the user input values do not go out of range 
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
// checkLimits_Synapse: Calls the checkLimits function using the predefined limits for each type of synapse
//---------------------------------------------------------------------------------------------------------------------------------------

int checkLimits_Synapse(int value, int synapseType)
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


//---------------------------------------------------------------------------------------------------------------------------------------
// glfw_error_callback: Prints GLFW callback error to terminal
//---------------------------------------------------------------------------------------------------------------------------------------

void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

