//---------------------------------------------------------------------------------------------------------------------------------------
// Source file for functions related to graphical user interface (GUI)
//---------------------------------------------------------------------------------------------------------------------------------------

#include <iostream>
#include "../include/guiFunctions.h"
#include "../include/dataFunctions.h"
#include "../include/serial.h"
#include "../../teensy_backend/include/constants.h"
#include "../../teensy_backend/include/constants_global.h"

#include <experimental/filesystem>
#include <chrono>
#include <typeinfo>
#include <thread>

//----------------------------------------------- Defining global variables -------------------------------------------------------------
const char *biasGenHeaderStr[BIASGEN_CATEGORIES] = {"Alpha DPI", "Neurons", "Analogue Synapses", "Digital Synapses", "Synapse Pulse Extension", "Learning Block", "Stop Learning Block", "Current To Frequency", "Buffer"};

#ifdef BIASGEN_SET_TRANSISTOR_TYPE
    const char *options_biasGenTransistorType[2] = {"nFET", "pFET"};
#endif

int selection_chipCore = 0;
int selection_synapseType = 0;
int selection_neuronNumber = 0;
int value_synapseNumber = 0;
int selection_transistorType = 0;
int selection_file = 0;

bool selectionChange_chipCore = 0;
bool selectionChange_synapseType  = 0;
bool selectionChange_neuronNumber = 0;
bool selectionChange_synapseNumber = 0;
bool selectionChange_file = 0;
bool valueChange_DACbias[DAC_CHANNELS_USED] = {};
bool valueChange_SPIbias_1[2] = {0, 0};
bool valueChange_SPIbias_2[2] = {0, 0};
bool valueChange_SaveFilename = false;

bool enableCommsEncoder = false;
bool enableCommsC2F = false;
bool enableCommsAER = false;
bool handshakeStatusEncoder = false;
bool handshakeStatusC2F = false;
bool savingEncoder = false;



std::vector<double> inputEncoder_xValues;
std::vector<int> inputEncoder_yValues;

std::vector<double> inputC2F_xValues;
std::vector<int> inputC2F_yValues;

std::string popupSave_str = "Save Bias Values";
const char *popupSave = popupSave_str.c_str(); 

std::string popupLoad_str = "Load Bias Values";
const char *popupLoad = popupLoad_str.c_str(); 

std::string popupII_str = "Load Input Interface";
const char *popupII = popupII_str.c_str(); 
bool openIIPopup = true;
bool openSavePopup = true;
bool openLoadPopup = true;

std::vector<AER_DECODER_OUTPUT_command> II_list;
int ii_input;
int ii_input_2;
int number_ii_input;
int number_ii_input_2;

namespace fs = std::experimental::filesystem;





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
#ifdef EXISTS_DAC
void setupDacWindow(bool show_DAC_config, DAC_command dac[], int serialPort, bool updateValues)
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
        if((ImGui::Button("Update", ImVec2(BUTTON_UPDATE_WIDTH, BUTTON_HEIGHT))) || updateValues)
        {
            if (dac[i].data==0) 
            {
            dac[i].data =1;
            }
            Pkt p2t_pk(dac[i]); 
            write(serialPort, (void *) &p2t_pk, sizeof(p2t_pk));
        }

        ImGui::PopID();
    }

    ImGui::NewLine();

    // Loading saved DAC biases

    if (ImGui::Button("Load", ImVec2(ImGui::GetWindowSize().x*0.48, BUTTON_HEIGHT)))
    {
        ImGui::OpenPopup(popupLoad);
    }

    ImGui::SameLine();

    // Save new biases values

    if (ImGui::Button("Save", ImVec2(ImGui::GetWindowSize().x*0.48, BUTTON_HEIGHT)))
    {
        ImGui::OpenPopup(popupSave);
    }

    savePopup(openSavePopup, popupSave, dac);
    loadPopup(openLoadPopup, popupLoad, dac, serialPort);

    ImGui::End();
    
}
#endif
//---------------------------------------------------------------------------------------------------------------------------------------
// setupAerWindow: Initialises and updates GUI window displaying AER values to send
//---------------------------------------------------------------------------------------------------------------------------------------
#ifdef EXISTS_OUTPUT_DECODER
void setupAerWindow(bool show_AER_config, int serialPort)
{
    ImGui::Begin(" Input interface", &show_AER_config);  

    ImGui::NewLine();


    if (ImGui::Button("Load", ImVec2(ImGui::GetWindowSize().x*0.48, BUTTON_HEIGHT)))
    {
        ImGui::OpenPopup(popupII);
    }
    loadII(openIIPopup, popupII, II_list);
    ImGui::NewLine();

    if (ImGui::Button("Start", ImVec2(ImGui::GetWindowSize().x*0.48, BUTTON_HEIGHT)))    
    {
        auto stimulator = std::thread(ii_stimulate, serialPort,  std::ref(II_list));
        
        stimulator.detach();   
    }

    ImGui::InputInt("First input value sending (dec): ", &ii_input);
    ImGui::InputInt("First number of times: ", &number_ii_input);

    ImGui::NewLine();
    ImGui::InputInt("Second input value sending (dec):", &ii_input_2);
    ImGui::InputInt("Second number of times:", &number_ii_input_2);

    ImGui::NewLine();

     AER_DECODER_OUTPUT_command teacher_signal;
     teacher_signal.data = 296;//8;
 
     AER_DECODER_OUTPUT_command input_signal;
     input_signal.data = 448 ;//192;

    if(ImGui::Button("Send Packet to Teensy", ImVec2(ImGui::GetWindowSize().x*0.8, BUTTON_HEIGHT)) || enableCommsAER)
    {
        teacher_signal.data = (uint16_t)ii_input;
        for(int i =0; i<number_ii_input; i++)
        {   
            Pkt p2t_pk(teacher_signal); 
            write(serialPort, (void *) &p2t_pk, sizeof(p2t_pk));
        }
        input_signal.data = (uint16_t)ii_input_2;
        for(int i =0; i<number_ii_input_2; i++)
        {   
            Pkt p2t_pk(input_signal); 
            write(serialPort, (void *) &p2t_pk, sizeof(p2t_pk));
        }
    }
    ImGui::SameLine();

    std::string toggleID_str = "toggleAER";
    const char *toggleID = toggleID_str.c_str();
    toggleButton(toggleID, &enableCommsAER);
    
    ImGui::End();

}
#endif

void ii_stimulate(int serialPort, std::vector<AER_DECODER_OUTPUT_command> &II_list)
{
    std::cout << "Stimulation thread start " << std::endl;

    for (int i=0; i<II_list.size(); i++)
    {

        //std::chrono::high_resolution_clock::time_point start =  std::chrono::high_resolution_clock::now();
        std::this_thread::sleep_for(std::chrono::microseconds(II_list[i].isi*10));
        //std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
        //auto elipsed = start - end;
       // std::cout << "Elipsed: " << elipsed << " Instead of: "<< II_list[i].isi*10 <<std::endl;
        //std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        //std::cout << "Elapsed: " << time_span.count() << " Instead of: "<< II_list[i].isi*10 *1e-6 <<std::endl;

        //std::cout << "Data: " << II_list[i].data <<std::endl;

        Pkt p2t_pk(II_list[i]); 
        write(serialPort, (void *) &p2t_pk, sizeof(p2t_pk));
    }

    std::cout << "Stimulation thread ended " << std::endl;
}


//---------------------------------------------------------------------------------------------------------------------------------------
// setupBiasGenWindow: Initialises and updates GUI window displaying Bias Generator values to send. 
//                     #ifdef condition used to define different definition if transistor type option to be displayed and handled
//---------------------------------------------------------------------------------------------------------------------------------------
#ifdef EXISTS_BIASGEN
#ifdef BIASGEN_SET_TRANSISTOR_TYPE
void setupBiasGenWindow(bool show_biasGen_config, BIASGEN_command biasGen[], int serialPort, bool relevantFileRows[][BIASGEN_CHANNELS], 
    std::vector<std::vector<std::vector<int>>> selectionChange_BiasGen, int noRelevantFileRows[],bool updateValues)
#else
void setupBiasGenWindow(bool show_biasGen_config, BIASGEN_command biasGen[], int serialPort, bool relevantFileRows[][BIASGEN_CHANNELS], 
        std::vector<std::vector<int>> selectionChange_BiasGen, int noRelevantFileRows[], bool updateValues)
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
                    float inputField_BiasGenValue = biasGen[j].currentValue_uA;
                    inputField_BiasGenValue, selectionChange_BiasGen[i][noRelevantFileRows[i]][0] = ImGui::InputFloat(emptylabel0, &inputField_BiasGenValue, 0.000001, 0, "%.6f", 0);
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
 #endif
                    biasGen[j].currentValue_uA = checkLimits(inputField_BiasGenValue, BIASGEN_MAX_CURRENT); 
                    biasGen[j].currentValue_binary = getBiasGenPacket(biasGen[j].currentValue_uA, biasGen[j].transistorType);
                    ImGui::SameLine();

                    // Including units
                    ImGui::Text("uA");
                    ImGui::SameLine();

                    // Adding a update button to write to serial port
                    if(ImGui::Button("Update", ImVec2(BUTTON_UPDATE_WIDTH, BUTTON_HEIGHT)))
                    {
                        Pkt p2t_pk(biasGen[j]); 
                        write(serialPort, (void *) &p2t_pk, sizeof(p2t_pk));
                    }
                    
                    ImGui::PopID();
                }
            }
        }
    }

    // Initialising values at POR
    if(updateValues)
    {
        for(int k=0; k<BIASGEN_CHANNELS; k++)
        {
            Pkt p2t_pk(biasGen[k]); 
            write(serialPort, (void *) &p2t_pk, sizeof(p2t_pk));
        }
    }

    ImGui::NewLine();

    if (ImGui::Button("Load", ImVec2(ImGui::GetWindowSize().x*0.48, BUTTON_HEIGHT)))
    {
        ImGui::OpenPopup(popupLoad);
    }

    ImGui::SameLine();

    if (ImGui::Button("Save", ImVec2(ImGui::GetWindowSize().x*0.48, BUTTON_HEIGHT)))
    {
        ImGui::OpenPopup(popupSave);
    }

    savePopup(openSavePopup, popupSave, biasGen);
    loadPopup(openSavePopup, popupLoad, biasGen, serialPort);

    ImGui::End();
}
#endif

//---------------------------------------------------------------------------------------------------------------------------------------
// setupSPI1Window: Initialises and updates GUI window displaying SPI1 values to send
//---------------------------------------------------------------------------------------------------------------------------------------
#ifdef EXISTS_SPI1
void setupSPI1Window(bool show_SPI_config, int serialPort, SPI_INPUT_command spi[], int resolution)
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
        Pkt p2t_pk(spi[0]); 
        write(serialPort, (void *) &p2t_pk, sizeof(p2t_pk));
    }
    ImGui::End();
}
#endif

//---------------------------------------------------------------------------------------------------------------------------------------
// setupSPI2Window: Initialises and updates GUI window displaying SPI2 values to send
//---------------------------------------------------------------------------------------------------------------------------------------
#ifdef EXISTS_SPI2
void setupSPI2Window(bool show_SPI_config, int serialPort, SPI_INPUT_command spi[], int resolution)
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
        Pkt p2t_pk(spi[0]); 
        write(serialPort, (void *) &p2t_pk, sizeof(p2t_pk));
    }
    ImGui::End();
}
#endif
//--------------------------------------------------------------------------------------------------------------------------------------
// savePopup: Generic popup to handle bias value saving operations
//--------------------------------------------------------------------------------------------------------------------------------------
template <typename T> void savePopup(bool openPopup, const char *popupLabel, T command)
{
    char filename[128];
    if (ImGui::BeginPopupModal(popupLabel, &openPopup))
    {
        if(typeid(command).hash_code() == typeid(DAC_command*).hash_code())
        {
            char filename_dac[128] = "data/customBiasValues/DAC/untitled.csv"; 
            strncpy(filename,  filename_dac, 128);

        }
        else if(typeid(command).hash_code() == typeid(BIASGEN_command*).hash_code())
        {
            char filename_bg[128] = "data/customBiasValues/BIASGEN/untitled.csv"; 
            strncpy(filename,  filename_bg, 128);
        }
 
        ImGui::NewLine();
        ImGui::Text("Filename: ");
        ImGui::SameLine();
        filename, valueChange_SaveFilename = ImGui::InputText(" ", filename, IM_ARRAYSIZE(filename));
        ImGui::NewLine();
        if (ImGui::Button("Close", ImVec2(ImGui::GetWindowSize().x*0.48, BUTTON_HEIGHT)))
        {
            ImGui::CloseCurrentPopup();
        }
        
        ImGui::SameLine();

        if (ImGui::Button("Save", ImVec2(ImGui::GetWindowSize().x*0.48, BUTTON_HEIGHT)))
        {
            saveBiases(filename, command);
            ImGui::CloseCurrentPopup();   
        }

        ImGui::EndPopup(); 
    }
}


//--------------------------------------------------------------------------------------------------------------------------------------
// loadPopup: Generic popup to handle bias value loading operations
//--------------------------------------------------------------------------------------------------------------------------------------
template <typename T> void loadPopup(bool openLoadPopup, const char *popupLabel, T command, int serialPort)
{
    char *filepath;
    std::string comboLabel_loadFiles_str = "##";
    const char *comboLabel_loadFiles = comboLabel_loadFiles_str.c_str();

    #ifdef EXISTS_DAC

    if(typeid(command).hash_code() == typeid(DAC_command*).hash_code())
    {
        filepath = DAC_FILENAME_LOAD;
    }
    #endif
    #ifdef EXISTS_BIASGEN

    if(typeid(command).hash_code() == typeid(BIASGEN_command*).hash_code())
    {
        filepath = BIASGEN_FILENAME_LOAD;
    }
    #endif


    if (ImGui::BeginPopupModal(popupLabel, &openLoadPopup))
    {
        // Get the files in the specified directory 

        int noFiles = getNoFiles(filepath);
        char* biases_filenames[noFiles];
        getFilepathArray(noFiles, filepath, biases_filenames);

        // Select file from comboBox
        
        ImGui::NewLine();
        ImGui::Text("Select file: ");  
        ImGui::SameLine();
        selection_file, selectionChange_file = ImGui::Combo(comboLabel_loadFiles, &selection_file, biases_filenames, noFiles);

        ImGui::NewLine();

        if (ImGui::Button("Close", ImVec2(ImGui::GetWindowSize().x*0.48, BUTTON_HEIGHT)))
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("Load", ImVec2(ImGui::GetWindowSize().x*0.48, BUTTON_HEIGHT)))
        {
            getBiasValues(command, (const std::string) biases_filenames[selection_file]);
            loadBiasValues(command, serialPort);
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup(); 
    }
}

void loadII (bool openLoadPopup, const char *popupLabel, std::vector<AER_DECODER_OUTPUT_command>  &II_list)
{
    if (ImGui::BeginPopupModal(popupLabel, &openLoadPopup))
    { 
        char *filepath;  
        std::string comboLabel_loadFiles_str = "##";
        const char *comboLabel_loadFiles = comboLabel_loadFiles_str.c_str();

        filepath = INPUT_INTERFACE_FILENAME_LOAD;
        int noFiles = getNoFiles(filepath);
        char* biases_filenames[noFiles];
        getFilepathArray(noFiles, filepath, biases_filenames);

        // Select file from comboBox
        
        ImGui::NewLine();
        ImGui::Text("Select file: ");  
        ImGui::SameLine();
        selection_file, selectionChange_file = ImGui::Combo(comboLabel_loadFiles, &selection_file, biases_filenames, noFiles);
        ImGui::NewLine();

        if (ImGui::Button("Close", ImVec2(ImGui::GetWindowSize().x*0.48, BUTTON_HEIGHT)))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Load", ImVec2(ImGui::GetWindowSize().x*0.48, BUTTON_HEIGHT)))
        {   II_list.clear(); 
            getIIValues((const std::string) biases_filenames[selection_file], II_list );
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup(); 

    }


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
#ifdef EXISTS_C2F
void updatePlotWindow_C2F(bool updatePlot, long timeStamp, double value, int serialPort)
{
    long valuesToSave[2] = {timeStamp, long(value)};
    double timeStamp_s = timeStamp/1000;
        
    ImGui::Begin("ALIVE Output", &updatePlot);  

    inputC2F_xValues.push_back(timeStamp_s);
    inputC2F_yValues.push_back(value);

    // Converting the data vectors to arrays
    double yArray[inputC2F_yValues.size()];
    double xArray[inputC2F_xValues.size()];

    for(int i = 0; i < int(inputC2F_xValues.size()); i++)
    {
        xArray[i] = inputC2F_xValues[i];
        yArray[i] = inputC2F_yValues[i];
    }

    if(ImPlot::BeginPlot("C2F Output: Current Number"))
    {
        ImPlot::GetStyle().UseLocalTime;
        ImPlot::GetStyle().Use24HourClock;

        ImPlot::SetupAxis(ImAxis_X1, "Time", ImPlotAxisFlags_Time);
        ImPlot::SetupAxis(ImAxis_Y1, "Current Number");
        ImPlot::SetupAxisLimits(ImAxis_Y1, 0, C2F_INPUT_RANGE, 1);
        ImPlot::SetupAxisLimits(ImAxis_X1, timeStamp_s - 25, timeStamp_s + 5, 1);
        ImPlot::PlotScatter("###", xArray, yArray, inputC2F_yValues.size());
        ImPlot::EndPlot();
        saveToCSV(valuesToSave, 2, C2F_INPUT_SAVE_FILENAME_CSV);

        ImGui::NewLine();

        if(ImGui::Button("Handshake: C2F", ImVec2(ImGui::GetWindowSize().x*0.48, BUTTON_HEIGHT)))
        {
            HANDSHAKE_C2F_command handshakeC2F;
            Pkt p2tpk_HandshakeC2F(handshakeC2F); 
            
            write(serialPort, (void *) &p2tpk_HandshakeC2F, sizeof(p2tpk_HandshakeC2F));
            handshakeStatusC2F = getHandshakeReturn(serialPort);
        }

        ImGui::SameLine();
        ImGui::Text("               ALIVE Output: C2F ");  
        ImGui::SameLine();

        std::string toggleID_str = "toggleC2F";
        const char *toggleID = toggleID_str.c_str();
        toggleButton(toggleID, &enableCommsC2F);

        // if(enableCommsC2F)
        // {

        // }
    }
    
    ImGui::End();
}
#endif


void updatePlotWindow_Encoder(bool updatePlot, long timeStamp, double value, int serialPort)
{
   /*  long valuesToSave[2] = {timeStamp, long(value)};
    double timeStamp_s = timeStamp/1000;

    ImGui::Begin("ALIVE Output", &updatePlot);  

    inputEncoder_xValues.push_back(timeStamp_s);
    inputEncoder_yValues.push_back(value);

    // Converting the data vectors to arrays
    double yArray[inputEncoder_yValues.size()];
    double xArray[inputEncoder_xValues.size()];

    for(int i = 0; i < int(inputEncoder_xValues.size()); i++)
    {
        xArray[i] = inputEncoder_xValues[i];
        yArray[i] = inputEncoder_yValues[i];
    }

    if(ImPlot::BeginPlot("Encoder Output: Firing Neuron Number"))
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

        ImGui::NewLine();
        

        if(ImGui::Button("Handshake: Encoder", ImVec2(ImGui::GetWindowSize().x*0.48, BUTTON_HEIGHT)))
        {
            HANDSHAKE_ENCODER_command handshakeEncoder;
            Pkt p2tpk_HandshakeEncoder(handshakeEncoder); 
            
            write(serialPort, (void *) &p2tpk_HandshakeEncoder, sizeof(p2tpk_HandshakeEncoder));
            handshakeStatusEncoder = getHandshakeReturn(serialPort);
        }

        ImGui::SameLine();
        ImGui::Text("ALIVE Output: Encoder ");  
        ImGui::SameLine();

        if (ImGui::Button( "Save"))
        {
            ENCODER_INPUT_command Enable_Encoder;
            Pkt p2tpk_Enable_encoder(Enable_Encoder);
            write(serialPort, (void *) &p2tpk_Enable_encoder, sizeof(p2tpk_Enable_encoder));
           // handshakeStatusEncoder = getHandshakeReturn(serialPort);
            savingEncoder = !savingEncoder;
            //serialDataSent++;
        }
        ImGui::SameLine();
        ImGui::Checkbox("Saving: ", &savingEncoder);
        ImGui::NewLine();
    }  

    ImGui::End();*/
}


bool getHandshakeStatus(int inputType)
{
    if(inputType == TEENSY_INPUT_ENCODER)
    {
        return handshakeStatusEncoder;
    }
    else if(inputType == TEENSY_INPUT_C2F)
    {
        return handshakeStatusC2F;
    }
    else
    {
        printf("Error: Input type not recognised.\n");
        return false;
    }
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
// glfw_error_callback: Prints GLFW callback error to terminal
//---------------------------------------------------------------------------------------------------------------------------------------

void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}


//---------------------------------------------------------------------------------------------------------------------------------------
// toggleButton: Implementation of toggle button directly copied from ImGui user forum (https://github.com/ocornut/imgui/issues/1537)
//---------------------------------------------------------------------------------------------------------------------------------------

void toggleButton(const char* str_id, bool* v)
{
    ImVec2 p = ImGui::GetCursorScreenPos();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    float height = BUTTON_HEIGHT; //ImGui::GetFrameHeight();
    float width = BUTTON_TOGGLE_WIDTH;
    float radius = height * 0.50f;

    ImGui::InvisibleButton(str_id, ImVec2(width, height));
    if (ImGui::IsItemClicked())
        *v = !*v;

    float t = *v ? 1.0f : 0.0f;

    ImGuiContext& g = *GImGui;
    float ANIM_SPEED = 0.08f;
    if (g.LastActiveId == g.CurrentWindow->GetID(str_id))// && g.LastActiveIdTimer < ANIM_SPEED)
    {
        float t_anim = ImSaturate(g.LastActiveIdTimer / ANIM_SPEED);
        t = *v ? (t_anim) : (1.0f - t_anim);
    }

    ImU32 col_bg;
    if (ImGui::IsItemHovered())
        col_bg = ImGui::GetColorU32(ImLerp(ImVec4(0.78f, 0.78f, 0.78f, 1.0f), ImVec4(0.64f, 0.83f, 0.34f, 1.0f), t));
    else
        col_bg = ImGui::GetColorU32(ImLerp(ImVec4(0.85f, 0.85f, 0.85f, 1.0f), ImVec4(0.56f, 0.83f, 0.26f, 1.0f), t));

    draw_list->AddRectFilled(p, ImVec2(p.x + width, p.y + height), col_bg, height * 0.5f);
    draw_list->AddCircleFilled(ImVec2(p.x + radius + t * (width - radius * 2.0f), p.y + radius), radius - 1.5f, IM_COL32(255, 255, 255, 255));
}

int setupresetWindow(bool show_reset_config, int serialPort)
{
    int serialDataSent = 0;

    ImGui::Begin("Reset window", &show_reset_config);
    

    if(ImGui::Button("Reset", ImVec2(BUTTON_UPDATE_WIDTH, BUTTON_HEIGHT)))  
    {
        RESET_command reset;
        Pkt p2t_pk(reset); 
        write(serialPort, (void *) &p2t_pk, sizeof(p2t_pk));
        serialDataSent++;
    }
    ImGui::End();
    return serialDataSent;
}