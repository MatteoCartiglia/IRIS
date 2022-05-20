#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <stack>         
#include <iostream>
#include <string>
#include <fstream> 
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include <sstream>
#include <iomanip>
#include <list>
#include "bias_control.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>



void show_dac_controls(bool* p_open, int serial_port){

            ImGui::Begin("DAC configuration",&p_open);      
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

                //showSaveLoadBiases();


                // Upload DAC statee
                if (*&DAC_upload[i]){
                
                    dac[i].data = (uint16_t) DACvalue[i];
                    P2TPkt p2t_pk(dac[i]); 
                    write(serial_port, (void *) &p2t_pk, sizeof(p2t_pk));
                    int retval = read(serial_port, &read_buf, sizeof(read_buf)); // in while loop? 
                    //std::printf("\n read: %d, %d , %d ,%d \n", &read_buf, read_buf, *read_buf, retval);

                    // catch_retval (retval, read_buf);
                    DAC_upload[i] = false;                
                }
            }
            ImGui::End();
}