//Matteo Cartiglia
#include "teensy.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "../../../teensy_backend/src/teensy_interface.h"


#include <stdio.h>
#include <stack>         
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>

#include <chrono>
#include <thread>
 
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include <iomanip>
#include <list>
#include <map>


uint8_t read_buf[1];
//User defined inputs. 
#define MAX_BG_CHANNELS 37
#define MAX_DAC_CHANNEL 12
#define DAC_COMMAND 3
#define DAC_COMMAND_SHIFT 4

char port_name[100] = "/dev/cu.usbmodem105688601";
char biasgen_por_file[100] = "biasgen_por.csv";
bool show_bg_spi_config =true;
bool show_dac_config = true;
bool show_spi_config =false;

// Biasgen defined masks
#define bias_type 0x0001
#define bias_fine  0x01FE
#define bias_course 0x0E00

// Header of helper fuction. Definition at eof
char port_opened[100] = "Serial port opened\n";
std::vector<int> readbias_file(const std::string& path);
static double course_current[6]= {60*pow(10, -6), 460*pow(10, -6), 3.8*pow(10, -3), 30*pow(10, -3), 240*pow(10, -3), 1.9*pow(10, 0)}; //uA
static void glfw_error_callback(int error, const char* description);

int main(int, char**)
{
    int serial_port = open(port_name, O_RDWR);
    // Check for errors
    if (serial_port < 0) {
        printf("Error %i from open: %s\n", errno, strerror(errno));
    }
    else {
            printf("Port opened successfuly");
            write(serial_port, port_opened, sizeof(port_opened));
    }
    
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char* glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150 --> MCusing this
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

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(1280, 720, "NCS teensy interface", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Our state
    bool show_demo_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Bias Gen controls
    std::vector<int> biases = readbias_file(biasgen_por_file);

    BIASGEN_command *bg[MAX_BG_CHANNELS];

    bool bg_upload[MAX_BG_CHANNELS];
    for (uint8_t i=0; i<MAX_BG_CHANNELS; i++) {
        bg[i]->address= i;
        bg[i]->transistor_type= (bias_type & biases[i]);
        bg[i]->fine_val= (bias_fine & biases[i]) >>1;
        bg[i]->course_val = (bias_course & biases[i]) >>9 ; 
        }
        

    // Dac controls
    DAC_command  *dac[MAX_DAC_CHANNEL];
    for (int i=0; i<MAX_DAC_CHANNEL; i++) {

        dac[i]->dac_number = (uint16_t) 0;
        dac[i]->data = (uint16_t) 0;
        dac[i]->command_address = DAC_COMMAND << DAC_COMMAND_SHIFT | i ;
        }
        
    bool DAC_upload=false;

    // SPI controls

    SPI_command *spi;
    SPI_command *last_spi_command;

    bool SPI_upload=false;


    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);


        //Dac config window
        if (show_dac_config)
        {   
            ImGui::Begin("DAC configuration",&show_dac_config);      
            ImGui::Text("DAC configuration (mV)");      
            ImGui::SliderInt("DAC 0 value: ", (int *) dac[0]->data, 0,1800);
            ImGui::SliderInt("DAC 1 value: ", (int *)dac[1]->data,0,1800);
            ImGui::SliderInt("DAC 2 value: ",(int *) dac[2]->data,0,1800);
            ImGui::SliderInt("DAC 3 value: ",(int *) dac[3]->data,0,1800);
            ImGui::SliderInt("DAC 4 value: ",(int *) dac[4]->data,0,1800);
            ImGui::SliderInt("DAC 5 value: ",(int *) dac[5]->data,0,1800);
            ImGui::SliderInt("DAC 6 value: ",(int *) dac[6]->data,0,1800);
            ImGui::SliderInt("DAC 7 value: ",(int *) dac[7]->data,0,1800);
            ImGui::SliderInt("DAC 8 value: ",(int *) dac[8]->data,0,1800);
            ImGui::SliderInt("DAC 9 value: ", (int *)dac[9]->data,0,1800);
            ImGui::SliderInt("DAC 10 value: ",(int *) dac[10]->data,0,1800);
            ImGui::SliderInt("DAC 11 value: ",(int *) dac[11]->data,0,1800);
            ImGui::Checkbox("Upload", &DAC_upload);
            ImGui::End();
        }
        //Bias Gen config window
        if (show_bg_spi_config)
        {
            ImGui::Begin("Bias Generator interface",&show_bg_spi_config);
            ImGui::TextWrapped("Course value 0-6 - Fine value 0-255 - P or N type. Currents are in uAmps ");

            for(int i=0; i<MAX_BG_CHANNELS; i++){
                float current = course_current[(int) bg[i]->course_val* bg[i]->fine_val/256 ];
                ImGui::Text("Bias Number: %d, Approx Current: %f uA", i, current);
                std::string label = "##_";
                label += std::to_string(i);
                std::string label_course, label_fine,label_type,label_but;
                label_course = label + "course";
                label_fine = label + "fine";
                label_type = label + "type";
                label_but = label + "but";

                const char *c_label = label_course.c_str();
                const char *f_label = label_fine.c_str();
                const char *t_label = label_type.c_str();
                const char *b_label = label_but.c_str();
                ImGui::PushItemWidth(100);
                ImGui::SliderInt(c_label, (int *) bg[i]->course_val,0,5);
                ImGui::SameLine();
                ImGui::PushItemWidth(100);
                ImGui::SliderInt(f_label, (int *) bg[i]->fine_val, 0,255);  
                ImGui::SameLine();
                ImGui::PushItemWidth(50);
                ImGui::SliderInt(t_label, (int *) bg[i]->transistor_type,0, 1); 
                ImGui::SameLine();
                *&bg_upload[i] =  ImGui::Button(b_label);
                if (*&bg_upload[i]){
                    //BIASGEN course 3bits - fine value 8bits - P or N type 1 Bit
                    printf("%d, %d, %d, %d", bg[i]->address , bg[i]->course_val, bg[i]->fine_val, bg[i]->transistor_type);

                    P2TPkt p2t_pkt(*bg[i]); 
                    write(serial_port, p2t_pkt, sizeof(p2t_pkt));
                    int retval = read(serial_port, read_buf, sizeof(read_buf));
                    catch_retval (retval, read_buf);
                    &bg_upload[i] = false;
                        }
                }
            ImGui::End();
        }

        // SPI 1 and 2 Config window. Only valid for SPAIC (i think)
        if (show_spi_config)
        {
            ImGui::Begin("SPI interface",&show_spi_config);
            ImGui::SameLine();
            ImGui::Text("SPI 1 for V SPI and 2 for C SPI ");  
            ImGui::SliderInt("SPI number: ",  (int *) spi->spi_number, 1,2);                 
            ImGui::SliderInt("SPI register: ",(int *) spi->address,0,255);
            ImGui::SliderInt("SPI value: ", (int *) spi->value,0,255);
            ImGui::Checkbox("Upload", &SPI_upload);

            ImGui::Text("Last SPI values: %d, %d, %d ",  last_spi_command->spi_number,  last_spi_command->address, last_spi_command->value);
            ImGui::End();
        }

        // Upload SPI state
        if (SPI_upload){
            last_spi_command = spi;
            P2TPkt p2t_pkt(*spi); 
            write(serial_port, p2t_pkt, sizeof(p2t_pkt));
            int retval = read(serial_port, read_buf, sizeof(read_buf));
            catch_retval (retval, read_buf);
            SPI_upload = false;
            }
        
        // 8 address + 12 data ; 12 bit data 11:9 (6 values) Course 8:1 fine value [0] 1p or 0n 
        // 64 biases
        
        // Upload DAC states
        if (DAC_upload){
          
            for(u_int8_t i=0; i<MAX_DAC_CHANNEL; i++){

                P2TPkt p2t_pkt(*dac[i]); 
                write(serial_port, p2t_pkt, sizeof(p2t_pkt));
                int retval = read(serial_port, read_buf, sizeof(read_buf)); // in while loop?
                catch_retval (retval, read_buf);
            }
            DAC_upload = false;
        }
       
        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
std::vector<int> readbias_file(const std::string& path) {
    std::stringstream ss;
    std::ifstream input_file(path);
    if (!input_file.is_open()) {
        std::cout << "Could not open the file - '"
             << path << "'" << std::endl;
        exit(EXIT_FAILURE);
    }
    ss << input_file.rdbuf();
    std::string value;
    std::vector<int> biases;

    while(std::getline(ss,value,'\n')){
        biases.push_back(stoi(value));
    }
    return biases;
}



static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

/*
void catch_retval (int retval, uint8_t read_buf) :{
 if (retval != sizeof(read_buf))
    {
        if (retval == 0) {                       
            fprintf("EOF read.. big mistake" );
            exit(-1);
        }
        if (retval == -1) {
            int err = errno;
            fprintf(stderr, "Error number: %d\n", err);
            exit(err);
        }
    }
    if (read_buf != uint8_t(TeensyStatus::Success) {
            fprintf("EOF read.. big mistake" );
    }
}*/