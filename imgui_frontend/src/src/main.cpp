//Matteo Cartiglia
#include "teensy.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "../../../teensy_backend/src/teensy_interface.h"

#include "../../imgui/implot.h"
#include "../../imgui/implot_internal.h"
//#include "bias_control.cpp"

#include <stdio.h>
#include <stack>         
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <cstddef>

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

#define NUMBER_CHANNELS 8


char port_name[100] = "/dev/cu.usbmodem105661701";
//cu.usbmodem105661701
char biasgen_por_file[100] = "biasgen_por.csv";
bool show_bg_spi_config =true;
bool show_dac_config = true;
bool show_spi_config =true;
bool show_aero =true;


// Biasgen defined masks
#define bias_type 0x0001
#define bias_fine  0x01FE
#define bias_course 0x0E00

#define FINE_BIAS_SHIFT 1
#define COURSE_BIAS_SHIFT 9


// Header of helper fuction. Definition at eof
char port_opened[100] = "Serial port opened\n";
std::vector<int> readbias_file(const std::string& path);
static double course_current[6]= {60*pow(10, -6), 460*pow(10, -6), 3.8*pow(10, -3), 30*pow(10, -3), 240*pow(10, -3), 1.9*pow(10, 0)}; //uA
static void glfw_error_callback(int error, const char* description);

struct ScrollingBuffer {
    int MaxSize;
    int Offset;
    ImVector<ImVec2> Data;
    ScrollingBuffer(int max_size = 2000) {
        MaxSize = max_size;
        Offset  = 0;
        Data.reserve(MaxSize);
    }
    void AddPoint(float x, float y) {
        if (Data.size() < MaxSize)
            Data.push_back(ImVec2(x,y));
        else {
            Data[Offset] = ImVec2(x,y);
            Offset =  (Offset + 1) % MaxSize;
        }
    }
    void Erase() {
        if (Data.size() > 0) {
            Data.shrink(0);
            Offset  = 0;
        }
    }
};


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
    ImPlot::CreateContext();

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Our state
    bool show_demo_window = true;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Bias Gen controls
    std::vector<int> biases = readbias_file(biasgen_por_file);

    BIASGEN_command bg[MAX_BG_CHANNELS];

    bool bg_upload[MAX_BG_CHANNELS];
    int bg_address[MAX_BG_CHANNELS];
    int bg_transistor_type[MAX_BG_CHANNELS];
    int bg_fine_val[MAX_BG_CHANNELS];
    int bg_course_val[MAX_BG_CHANNELS];

    for (uint8_t i=0; i<MAX_BG_CHANNELS; i++) {
        bg[i].address= i;
        bg_address[i] = i;

        bg[i].transistor_type= (bias_type & biases[i]);
        bg_transistor_type[i] = (bias_type & biases[i]);

        bg[i].fine_val= (bias_fine & biases[i]) >>FINE_BIAS_SHIFT;
        bg_fine_val[i] = (bias_fine & biases[i]) >>FINE_BIAS_SHIFT;

        bg[i].course_val = (bias_course & biases[i]) >>COURSE_BIAS_SHIFT ; 
        bg_course_val[i] = (bias_course & biases[i]) >>COURSE_BIAS_SHIFT ; 
        }
        

    // Dac controls
    DAC_command  dac[MAX_DAC_CHANNEL];
    int DACvalue[MAX_DAC_CHANNEL];


    for (int i=0; i<MAX_DAC_CHANNEL; i++) {

        dac[i].dac_number = (uint8_t) 0;
        dac[i].data = (uint16_t) 0;
        DACvalue[i] = 500;
        dac[i].command_address = DAC_COMMAND << DAC_COMMAND_SHIFT | i ;
        }
        
    bool DAC_upload[MAX_DAC_CHANNEL];

    // SPI controls
    int spi_number=0, spi_value=0, spi_address=0;
    SPI_command spi;
    SPI_command last_spi_command;
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
            ImPlot::ShowDemoWindow(&show_demo_window);

        
       /* if (show_aero)
        { 
            ImGui::BulletText("Move your mouse to change the data!");
            ImGui::BulletText("This example assumes 60 FPS. Higher FPS requires larger buffer size.");
            static ScrollingBuffer sdata1, sdata2;
            ImVec2 mouse = ImGui::GetMousePos();
            static float t = 0;
            t += ImGui::GetIO().DeltaTime;
            sdata1.AddPoint(t, mouse.x * 0.0005f);
            sdata2.AddPoint(t, mouse.y * 0.0005f);

            static float history = 10.0f;
            ImGui::SliderFloat("History",&history,1,30,"%.1f s");
      

            static ImPlotAxisFlags flags = ImPlotAxisFlags_NoTickLabels;

            if (

                ImPlot::BeginPlot("##Scrolling", ImVec2(-1,150))) {
               // for (int channel = 0;  channel <NUMBER_CHANNELS; channel++ ){

                    ImPlot::SetupAxes(NULL, NULL, flags, flags);
                    ImPlot::SetupAxisLimits(ImAxis_X1,t - history, t, ImGuiCond_Always);
                    ImPlot::SetupAxisLimits(ImAxis_Y1,0,1);
                    ImPlot::PlotShaded("Mouse X", &sdata1.Data[0].x, &sdata1.Data[0].y, sdata1.Data.size(), -INFINITY, sdata1.Offset, 2 * sizeof(float));
                    ImPlot::PlotLine("Mouse Y", &sdata2.Data[0].x, &sdata2.Data[0].y, sdata2.Data.size(), sdata2.Offset, 2*sizeof(float));
                    
                    //}
                    ImPlot::EndPlot();

            }

            
                /* ImGui::BulletText("Live plotting of AER events");
            static ScrollingBuffer sdata1, sdata2;
            ImVec2 mouse = ImGui::GetMousePos();
            static float t = 0;
            t += ImGui::GetIO().DeltaTime;
            sdata1.AddPoint(t, mouse.x * 0.0005f);
            sdata2.AddPoint(t, mouse.y * 0.0005f);

            static float history = 10.0f;
            ImGui::SliderFloat("History",&history,1,30,"%.1f s");
  
            static ImPlotAxisFlags flags = ImPlotAxisFlags_NoTickLabels;

            if (ImGui::BeginTable(" Events Table", 2, flags, ImVec2(-1,0))) {
                ImGui::TableSetupColumn(" Channel Id", ImGuiTableColumnFlags_WidthFixed, 75.0f);
                ImGui::TableSetupColumn("Events");
                ImGui::TableHeadersRow();
                ImPlot::PushColormap(ImPlotColormap_Cool);
                
                for (int channel = 0; channel < NUMBER_CHANNELS; channel++) {

                    ImGui::TableNextRow();

                    srand(channel);
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("Channel n %d", channel + 1);
                    ImGui::TableSetColumnIndex(1);
                    ImGui::PushID(channel);
                    ImPlot::SetNextMarkerStyle(1, 2, ImVec4(0,0,0,0), 3);
                    ImGui::PopID();
                }
                ImPlot::PopColormap();
                ImGui::EndTable();
            }
            ImGui::End();*/
            
        }


        //Dac config window
        if (show_dac_config)
        {   
            ImGui::Begin("DAC configuration",&show_dac_config);      
            ImGui::Text("DAC configuration (mV)");      
            for(int i=0; i<MAX_DAC_CHANNEL; i++){
                std::string label = "##_";
                label += std::to_string(i);         
                const char *label_c = label.c_str();       
                std::string labelb =label +  "but";
                const char *label_b = labelb.c_str(); 
                ImGui::Text("Dac number %d", i);
                ImGui::SameLine();
                ImGui::SliderInt(label_c, &DACvalue[i],0,1800);
                ImGui::SameLine();
                *&DAC_upload[i] =  ImGui::Button(label_b);
                // Upload DAC statee
                if (*&DAC_upload[i]){
                
                    dac[i].data = (uint16_t) DACvalue[i];
                    P2TPkt p2t_pk(dac[i]); 
                    write(serial_port, (void *) &p2t_pk, sizeof(p2t_pk));
                    int retval = read(serial_port, &read_buf, sizeof(read_buf)); // in while loop? 
                    DAC_upload[i] = false;                
                }
            }
            ImGui::End();
        }
        //Bias Gen config window
        if (show_bg_spi_config)
        {
            ImGui::Begin("Bias Generator interface",&show_bg_spi_config);
            ImGui::TextWrapped("Course value 0-6 - Fine value 0-255 - P or N type. Currents are in uAmps ");

            for(int i=0; i<MAX_BG_CHANNELS; i++){
                float current = course_current[(int)bg_course_val[i]* bg_fine_val[i]/256 ];
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
                ImGui::SliderInt(c_label, &bg_course_val[i],0,5);
                ImGui::SameLine();
                ImGui::PushItemWidth(100);
                ImGui::SliderInt(f_label, &bg_fine_val[i], 0,255);  
                ImGui::SameLine();
                ImGui::PushItemWidth(50);
                ImGui::SliderInt(t_label,  &bg_transistor_type[i] ,0, 1); 
                ImGui::SameLine();
                *&bg_upload[i] =  ImGui::Button(b_label);
                if (*&bg_upload[i]){
                    //BIASGEN course 3bits - fine value 8bits - P or N type 1 Bit
                    bg[i].address = (uint16_t)bg_address[i];
                    bg[i].course_val = (uint8_t)bg_course_val[i];
                    bg[i].fine_val = (uint8_t)bg_fine_val[i];
                    bg[i].transistor_type = (uint8_t)bg_transistor_type[i];

                    P2TPkt p2t_pkt(bg[i]); 
                    write(serial_port,(void *) &p2t_pkt, sizeof(p2t_pkt));
                    //printf(p2t_pkt.header)

                    std::printf("BG header, body.addess: %d, %d \n", p2t_pkt.header, p2t_pkt.body[0]);
                    //std::printf("course val, fine val: %d, %d\n", bg[i].course_val,bg[i].fine_val);
            


                 //   int retval = read(serial_port, read_buf, sizeof(read_buf));
                 //   catch_retval (retval, read_buf);
                    bg_upload[i] = false;
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
            ImGui::SliderInt("SPI number: ",  &spi_number, 1,2);                 
            ImGui::SliderInt("SPI register: ", &spi_address,0,255);
            ImGui::SliderInt("SPI value: ", &spi_value,0, 255);
            ImGui::Checkbox("Upload", &SPI_upload);

            ImGui::Text("Last SPI values: %d, %d, %d ",  last_spi_command.spi_number,  last_spi_command.address, last_spi_command.value);
            ImGui::End();
        }

        // Upload SPI state
        if (SPI_upload){
            spi.spi_number = (uint8_t)spi_number;
            spi.address = (uint8_t)spi_address;
            spi.value = (uint16_t)spi_value;
            last_spi_command = spi;
            
            P2TPkt p2t_pkt(spi); 
            write(serial_port, (void *) &p2t_pkt, sizeof(p2t_pkt));
            std::printf("header, body: %d, %d \n", p2t_pkt.header, p2t_pkt.body);

          //  int retval = read(serial_port, read_buf, sizeof(read_buf));
         //   catch_retval (retval, read_buf);
            SPI_upload = false;
            }
        
        // 8 address + 12 data ; 12 bit data 11:9 (6 values) Course 8:1 fine value [0] 1p or 0n 
        // 64 biases
        
       
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


    ImPlot::DestroyContext();


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