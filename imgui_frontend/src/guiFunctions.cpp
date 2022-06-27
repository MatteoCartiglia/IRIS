//---------------------------------------------------------------------------------------------------------------------------------------
//
//
// Author: Ciara Giles-Doran <gciara@student.ethz.ch>
// Last updated: 
//---------------------------------------------------------------------------------------------------------------------------------------

#include "../include/guiFunctions.h"
#include "../include/constants.h"

// Defining global variables
const char* glsl_version;

//---------------------------------------------------------------------------------------------------------------------------------------
// setupGLFW 
//---------------------------------------------------------------------------------------------------------------------------------------

GLFWwindow* setupWindow()
{
    glfwSetErrorCallback(glfw_error_callback);

    if (!glfwInit())
    {
        printf("Error %i: %s\n", errno, strerror(errno));
        return 0;
    }
        
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);

#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150 --> MC using this
    glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac

#else
    // GL 3.0 + GLSL 130
    glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(1280, 720, "ALIVE Teensy Testing Interface", NULL, NULL);

    if (window == NULL)
    {
        printf("Error %i: %s\n", errno, strerror(errno));
        return 0;
    }
        
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); 
    setupImGuiContext(window);
    
    return window;
}

/*---------------------------------------------------------------------------------------------------------------------------------------
* setupImGuiContext
*----------------------------------------------------------------------------------------------------------------------------------------*/

void setupImGuiContext(GLFWwindow* window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(getGlslVersion());
}

/*---------------------------------------------------------------------------------------------------------------------------------------
* renderImGui
*----------------------------------------------------------------------------------------------------------------------------------------*/
void renderImGui(GLFWwindow* window)
{
    int display_w;
    int display_h;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Rendering
    ImGui::Render();
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);
}

/*---------------------------------------------------------------------------------------------------------------------------------------
* setupDacWindow
*----------------------------------------------------------------------------------------------------------------------------------------*/
void setupDacWindow(bool show_dac_config, int DACvalue[], bool DAC_upload, DAC_command dac[], int serialPort)
{
    ImGui::Begin("DAC Configuration", &show_dac_config);      
    ImGui::Text("DAC Configuration (mV)");      

    for(int i=0; i<MAX_DAC_CHANNEL; i++)
    {
        std::string label = "##_";
        label += std::to_string(i);         
        const char *label_c = label.c_str();       
        std::string labelb =label +  "but";
        const char *label_b = labelb.c_str(); 

        ImGui::Text("DAC Number %d", i);
        ImGui::SameLine();
        ImGui::SliderInt(label_c, &DACvalue[i], 0, 1800);
        ImGui::SameLine();

        *&DAC_upload[&i] =  ImGui::Button(label_b);

        // Upload DAC state

        if (*&DAC_upload[&i])
        {
            dac[i].data = (uint16_t) DACvalue[i];
            P2TPkt p2t_pk(dac[i]); 

            write(serialPort, (void *) &p2t_pk, sizeof(p2t_pk));
            DAC_upload[&i] = false;                
        }
    }

    ImGui::End();
}

/*---------------------------------------------------------------------------------------------------------------------------------------
* setupAerWindow
*----------------------------------------------------------------------------------------------------------------------------------------*/
void setupAerWindow(bool show_aero, bool AER_init, int serialPort)
{
    static P2TPkt rx_buf;
    static AER_out aero;
    int retval = 0;

    ImGui::Begin("AER Events", &show_aero);   
    *&AER_init =  ImGui::Button("AER Start Logging");

    if (*&AER_init)
    {
        P2TPkt p2t_pkt(aero); 
        write(serialPort, (void *) &p2t_pkt, sizeof(p2t_pkt));

        while( retval == 0)
            retval = read(serialPort, &rx_buf, sizeof(rx_buf)); // in while loop?

        std::printf("\n retval: %d \n", retval) ;
        std::printf("\n rx_buf head: %d \n", rx_buf.header) ;
        std::printf("\n rx_buf body[0]: %d \n", rx_buf.body[0]);
        std::printf("\n rx_buf body[1]: %d \n", rx_buf.body[1]);
        std::printf("\n rx_buf body[2]: %d \n", rx_buf.body[2]);
        std::printf("\n rx_buf body[3]: %d \n", rx_buf.body[3]);
        std::printf("\n rx_buf body[4]: %d \n", rx_buf.body[4]);
        std::printf("\n rx_buf body[5]: %d \n", rx_buf.body[5]);
    }

    if ((P2tPktType)rx_buf.header == P2tPktType::P2tRequestAerOutput)
    {
        std::printf("\n %d \n", (P2tPktType)rx_buf.header) ;
        AER_out aero (rx_buf);
        std::printf("\n Add: %d \n", aero.address) ;
        std::printf("\n Ts:%d \n", aero.ts_1ms) ;
    }
    ImGui::End();
}


/*---------------------------------------------------------------------------------------------------------------------------------------
* setupbiasGenWindow
*----------------------------------------------------------------------------------------------------------------------------------------*/

void setupbiasGenWindow(bool show_bg_config, bool bg_upload[], int bg_address[], int bg_transistor_type[], int bg_fine_val[], int bg_course_val[], 
    double course_current[], BIASGEN_command bg[], int serialPort)
{
    ImGui::Begin("Bias Generator interface",&show_bg_config);
    ImGui::TextWrapped("Course value 0-6 - Fine value 0-255 - P or N type. Currents are in uAmps ");

    for(int i=0; i<MAX_BG_CHANNELS; i++)
    {
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

        if (*&bg_upload[i])
        {
            //BIASGEN course 3bits - fine value 8bits - P or N type 1 Bit
            bg[i].address = (uint16_t)bg_address[i];
            bg[i].course_val = (uint8_t)bg_course_val[i];
            bg[i].fine_val = (uint8_t)bg_fine_val[i];
            bg[i].transistor_type = (uint8_t)bg_transistor_type[i];

            P2TPkt p2t_pkt(bg[i]); 
            write(serialPort, (void *) &p2t_pkt, sizeof(p2t_pkt));
            bg_upload[i] = false;
        }
    }   

    ImGui::End();
}


/*---------------------------------------------------------------------------------------------------------------------------------------
* glfw_error_callback
*----------------------------------------------------------------------------------------------------------------------------------------*/
static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW error %d: %s\n", error, description);
}


/*---------------------------------------------------------------------------------------------------------------------------------------
* getGlslVersion
*----------------------------------------------------------------------------------------------------------------------------------------*/
const char* getGlslVersion()
{
    return glsl_version;
}