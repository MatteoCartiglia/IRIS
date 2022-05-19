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
#include <experimental/filesystem>
using namespace std;

bool show_dac_config = true;
const int n_ADC = 12;
float ADC_values [n_ADC] = {0};
bool ADC_upload=true;
bool saved_biases = false; 


bool loadBiases(char *filename){
    ifstream inFile;
    inFile.open(filename);
    if (inFile.is_open()) {
        for(int i = 0; i < n_ADC; ++i)
        {
            inFile >> ADC_values[i];
        }
        return true;
    }
    else{
        return false;
    };  
}

bool saveBiases(char *filename){
    ofstream fout(filename);
    if(fout.is_open())
	{
        for (int i = 0; i < n_ADC; i++) 
		{
            fout << ADC_values[i] << ' '; 
		}
        return true;
	}
	else 
	    return false;
}

void showSaveLoadBiases(){

    if (ImGui::Button("Save ADC biases"))
        ImGui::OpenPopup("Saving Biases Menu");

    ImGui::SameLine();
    
    if (ImGui::Button("Load ADC biases"))
        ImGui::OpenPopup("Loading Biases Menu");

    //////////////// SAVE

    if (ImGui::BeginPopupModal("Saving Biases Menu", NULL, ImGuiWindowFlags_MenuBar))
        {
            ImGui::Text("Choose a cool name to save your biases... ");
            static char filename[128] = "Biases/__NAME__.txt";
            ImGui::InputText(" ", filename, IM_ARRAYSIZE(filename));
            ImGui::SameLine();

            ImGui::PushID(0);
            ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(3 / 7.0f, 0.6f, 0.6f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(3 / 7.0f, 0.7f, 0.7f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(3 / 7.0f, 0.8f, 0.8f));
            bool save_button = false;
            save_button = ImGui::Button("Save");
            ImGui::PopStyleColor(3);
            ImGui::PopID();
            if (save_button){
                int saved_biases = saveBiases(filename);
                if (saved_biases == true) {
                    ImGui::CloseCurrentPopup();
                    } 
                else{
                    ImGui::Text("Failed to save the biases! :(");
                    }
            }
            ImGui::PushID(0);
            ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0 / 7.0f, 0.6f, 0.6f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0 / 7.0f, 0.7f, 0.7f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0 / 7.0f, 0.8f, 0.8f));
            bool close_button = false;
            close_button = ImGui::Button("Close");
            ImGui::PopStyleColor(3);
            ImGui::PopID();
            if (close_button)
                ImGui::CloseCurrentPopup();
            ImGui::EndPopup();
        }

    //////////////// LOAD

    if (ImGui::BeginPopupModal("Loading Biases Menu", NULL, ImGuiWindowFlags_MenuBar)){
        
        ImGui::Text("Choose a your favourite set of biases... ");  
        std::string path = "Biases/";


        //// Obtaining the list of files in the Biases/ Directory. This will allow us to display buttons 
        //// And allow the user to choose which file to load. 
        
        /// See how many files we have first
        int n_files = 0; 
        for (const auto& dirEntry: fs::directory_iterator(path)){
            string filename_with_path = dirEntry.path();
            n_files = n_files + 1;
        }

        //// Put filenames into array of chars.
        char* biases_filenames_no_path [n_files];
        char* biases_filenames_with_path [n_files];
        int iter = 0;
        for (const auto& dirEntry: fs::directory_iterator(path)){

            string filename_with_path = dirEntry.path();
            string filename_no_path = filename_with_path.substr(filename_with_path.find('/') + 1, filename_with_path.length() - filename_with_path.find('/'));
            
            biases_filenames_with_path[iter] = new char[filename_with_path.length() + 1];
            strcpy(biases_filenames_with_path[iter], filename_with_path.c_str());
            
            biases_filenames_no_path[iter] = new char[filename_no_path.length() + 1];
            strcpy(biases_filenames_no_path[iter], filename_no_path.c_str());

            iter++;
        }

        // // Now displaying buttons, yay!
        for (int i = 0; i < n_files; i++)
        {
            if (i % 3 != 0)
                ImGui::SameLine();

            ImGui::PushID(i);
            ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(4 / 7.0f, 0.6f, 0.6f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(4 / 7.0f, 0.7f, 0.7f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(4 / 7.0f, 0.8f, 0.8f));

            if (ImGui::Button(biases_filenames_no_path[i])){
                int loaded_biases = loadBiases(biases_filenames_with_path[i]);
                if (loaded_biases == true) {
                    ImGui::CloseCurrentPopup();
                    } 
                else{
                    ImGui::Text("Failed to load the biases! :(");
                    }
            }
            ImGui::PopStyleColor(3);
            ImGui::PopID();
        }

        ImGui::PushID(0);
        ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0 / 7.0f, 0.6f, 0.6f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0 / 7.0f, 0.7f, 0.7f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0 / 7.0f, 0.8f, 0.8f));
        bool close_button = false;
        close_button = ImGui::Button("Close");
        ImGui::PopStyleColor(3);
        ImGui::PopID();
        if (close_button)
            ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }
}

void ShowAdcControlCenter(bool* p_open){
    ImGui::Begin("ADC configuration",&show_dac_config);      
    ImGui::Text("ADC configuration (mV)");
    ImGui::SliderFloat("ADC 1",  &ADC_values[0],  0.0f,  1.8f, "bias = %.3f");
    ImGui::SliderFloat("ADC 2",  &ADC_values[1],  0.0f,  1.8f, "bias = %.3f");
    ImGui::SliderFloat("ADC 3",  &ADC_values[2],  0.0f,  1.8f, "bias = %.3f");
    ImGui::SliderFloat("ADC 4",  &ADC_values[3],  0.0f,  1.8f, "bias = %.3f");
    ImGui::SliderFloat("ADC 5",  &ADC_values[4],  0.0f,  1.8f, "bias = %.3f");
    ImGui::SliderFloat("ADC 6",  &ADC_values[5],  0.0f,  1.8f, "bias = %.3f");
    ImGui::SliderFloat( "ADC 7",  &ADC_values[6],  0.0f,  1.8f, "bias = %.3f");
    ImGui::SliderFloat( "ADC 8",  &ADC_values[7],  0.0f,  1.8f, "bias = %.3f");
    ImGui::SliderFloat( "ADC 9",  &ADC_values[8],  0.0f,  1.8f, "bias = %.3f");
    ImGui::SliderFloat( "ADC 10", &ADC_values[9],  0.0f,  1.8f, "bias = %.3f");
    ImGui::SliderFloat( "ADC 11", &ADC_values[10], 0.0f,  1.8f, "bias = %.3f");
    ImGui::SliderFloat( "ADC 12", &ADC_values[11], 0.0f,  1.8f, "bias = %.3f");
    showSaveLoadBiases();
    ImGui::End();
}


// void showSaveLoadBiasesOld(){
    
//     if (ImGui::Button("Save ADC biases")){
//         int saved_biases = saveBiases();
//         if (saved_biases == true) {
//             ImGui::Text("Successfully saved biases.txt!");
//         } 
//         else{
//             ImGui::Text("Failed to save the biases.txt :(");
//         }
//         saved_biases = 0;
//     }
    
//     ImGui::SameLine();
//     if (ImGui::Button("Load ADC biases")){
//         int loaded_biases = loadBiases();
//         if (loaded_biases == true) {
//             ImGui::Text("Successfully loaded the biases");
//             loaded_biases = 0;
//         } 
//         else{
//             ImGui::Text("Failed to load the biases :(");
//         }
//         loaded_biases = 0;
//     }
// }