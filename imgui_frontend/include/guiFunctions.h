//---------------------------------------------------------------------------------------------------------------------------------------
// Header file for functions related to graphical user interface (GUI)
//---------------------------------------------------------------------------------------------------------------------------------------

#include <GLFW/glfw3.h>     // Will drag system OpenGL headers
#include <unistd.h>         // UNIX standard function definitions
#include <stdio.h>          // Standard input output
#include <errno.h>          // Error number definitions
#include <string>
#include <vector>
#include <iomanip>          // For formatting system time

#include "../imgui/imgui_backend/imgui_impl_glfw.h"
#include "../imgui/imgui_backend/imgui_impl_opengl3.h"
#include "../imgui/imgui_src/implot.h"
#include "../imgui/imgui_src/implot_internal.h"
#include "../imgui/imgui_src/imgui.h"
#include "serial.h"

#include "../../teensy_backend/include/datatypes.h"
#include "../../teensy_backend/include/constants.h"


//---------------------------------------------------------------------------------------------------------------------------------------
// setupWindow: Defines platform-specific variables, creates GUI window, initialises ImGui and ImPlot contexts and sets up 
//              platform/renderer backends
//---------------------------------------------------------------------------------------------------------------------------------------
GLFWwindow* setupWindow();

//---------------------------------------------------------------------------------------------------------------------------------------
// renderImGui: Completes rendering operations for ImGui and GLFW
//---------------------------------------------------------------------------------------------------------------------------------------
void renderImGui(GLFWwindow* window);

#ifdef EXISTS_DAC
    //---------------------------------------------------------------------------------------------------------------------------------------
    // setupDacWindow: Initialises and updates GUI window displaying DAC values to send
    //---------------------------------------------------------------------------------------------------------------------------------------
    int setupDacWindow(bool show_DAC_config, DAC_command dac[], const Serial& sPort, bool updateValues);
#endif

//---------------------------------------------------------------------------------------------------------------------------------------
// setupAerWindow: Initialises and updates GUI window displaying AER values to send
//---------------------------------------------------------------------------------------------------------------------------------------
void setupAerWindow(bool show_AER_config, const Serial& sPort);


void loadII (bool openLoadPopup, const char *popupLabel, std::vector<AER_DECODER_OUTPUT_command> &II_list);

void ii_stimulate(const Serial& sPort, std::vector<AER_DECODER_OUTPUT_command> &II_list);
#ifdef EXISTS_BIASGEN
    //-----------------------------------------------------------------------------------------------------------------------------------
    // setupBiasGenWindow: Initialises and updates GUI window displaying Bias Generator values to send. 
    //                     #ifdef condition used to define different definition if transistor type option to be displayed and handled
    //-----------------------------------------------------------------------------------------------------------------------------------
    #ifdef BIASGEN_SET_TRANSISTOR_TYPE
        int setupBiasGenWindow(bool show_biasGen_config, BIASGEN_command biasGen[], const Serial& sPort, bool relevantFileRows[][BIASGEN_CHANNELS], 
            std::vector<std::vector<std::vector<int>>> selectionChange_BiasGen, int noRelevantFileRows[], bool updateValues);
    #else
        int setupBiasGenWindow(bool show_biasGen_config, BIASGEN_command biasGen[], const Serial& sPort, bool relevantFileRows[][BIASGEN_CHANNELS], 
                std::vector<std::vector<int>> selectionChange_BiasGen, int noRelevantFileRows[], bool updateValues);
    #endif
#endif


#ifdef EXISTS_SPI1
    //-----------------------------------------------------------------------------------------------------------------------------------
    // setupSPI1Window: Initialises and updates GUI window displaying SPI1 values to send. 
    //                  #ifdef condition used to define different definition if transistor type option to be displayed and handled
    //-----------------------------------------------------------------------------------------------------------------------------------
    int setupSPI1Window(bool show_SPI_config, const Serial& sPort, SPI_INPUT_command spi_command[], int resolution);
#endif

#ifdef EXISTS_SPI2
    //-----------------------------------------------------------------------------------------------------------------------------------
    // setupSPI2Window: Initialises and updates GUI window displaying SPI2 values to send. 
    //                  #ifdef condition used to define different definition if transistor type option to be displayed and handled
    //-----------------------------------------------------------------------------------------------------------------------------------
    int setupSPI2Window(bool show_SPI_config, const Serial& sPort, SPI_INPUT_command spi_command[], int resolution);
#endif

#if defined(EXISTS_BIASGEN) || defined(EXISTS_DAC)
    //-----------------------------------------------------------------------------------------------------------------------------------
    // savePopup: Generic popup to handle bias value saving operations
    //-----------------------------------------------------------------------------------------------------------------------------------
    template <typename T> void savePopup(bool openPopup, const char *popupLabel, T command);

    //-----------------------------------------------------------------------------------------------------------------------------------
    // loadPopup: Generic popup to handle bias value loading operations
    //-----------------------------------------------------------------------------------------------------------------------------------
    template <typename T> void loadPopup(bool openLoadPopup, const char *popupLabel, T command, const Serial& sPort);
#endif


//---------------------------------------------------------------------------------------------------------------------------------------
// updateSerialOutputWindow: Writes serial input to Log window in GUI
//---------------------------------------------------------------------------------------------------------------------------------------
bool updateSerialOutputWindow(bool show_Serial_output, bool logEntry = false, const char* logString = nullptr);

//---------------------------------------------------------------------------------------------------------------------------------------
// updatePlotWindow: Initialises and updates GUI window displaying live output from ALIVE
//---------------------------------------------------------------------------------------------------------------------------------------
// void updatePlotWindow_Encoder(bool updatePlot, long timeStamp, double value, int serialPort);
// void updatePlotWindow_C2F(bool updatePlot, long timeStamp, double value, int serialPort);
bool getHandshakeStatus(int inputType);


void setupResetWindow(bool show_reset_config, const Serial& sPort);  
// int setupResetWindow(bool show_reset_config, int serialPort);  


//---------------------------------------------------------------------------------------------------------------------------------------
// checkLimits: Checks the user input values do not go out of range and updates the value accordingly
//---------------------------------------------------------------------------------------------------------------------------------------
float checkLimits(float value, float maxLimit, float minValue = 0);

//---------------------------------------------------------------------------------------------------------------------------------------
// checkLimits_Synapse: Calls the checkLimits function using the predefined limits for each type of synapse
//---------------------------------------------------------------------------------------------------------------------------------------
int checkLimits_Synapse(int value, int synapseType);

//---------------------------------------------------------------------------------------------------------------------------------------
// glfw_error_callback: Prints GLFW callback error to terminal
//---------------------------------------------------------------------------------------------------------------------------------------
void glfw_error_callback(int error, const char* description);

//---------------------------------------------------------------------------------------------------------------------------------------
// toggleButton: Implementation of toggle button mostly copied from ImGui user forum (https://github.com/ocornut/imgui/issues/1537)
//---------------------------------------------------------------------------------------------------------------------------------------
void toggleButton(const char* str_id, bool* v);

//-----------------------------------------------------------------------------
// Log struct directly copied from imgui_demo.cpp - CLEANUP NEEDED
//-----------------------------------------------------------------------------

struct Log
{
    ImGuiTextBuffer     Buf;
    ImGuiTextFilter     Filter;
    ImVector<int>       LineOffsets; // Index to lines offset. We maintain this with AddLog() calls.
    bool                AutoScroll;  // Keep scrolling if already at the bottom.

    Log()
    {
        AutoScroll = true;
        Clear();
    }

    void    Clear()
    {
        Buf.clear();
        LineOffsets.clear();
        LineOffsets.push_back(0);
    }

    void    AddLog(const char* fmt, ...) IM_FMTARGS(2)
    {
        int old_size = Buf.size();
        va_list args;
        va_start(args, fmt);
        Buf.appendfv(fmt, args);
        va_end(args);
        for (int new_size = Buf.size(); old_size < new_size; old_size++)
            if (Buf[old_size] == '\n')
                LineOffsets.push_back(old_size + 1);
    }

    void    Draw(const char* title, bool* p_open = NULL)
    {
        if (!ImGui::Begin(title, p_open))
        {
            ImGui::End();
            return;
        }

        // Options menu
        if (ImGui::BeginPopup("Options"))
        {
            ImGui::Checkbox("Auto-scroll", &AutoScroll);
            ImGui::EndPopup();
        }

        // Main window
        if (ImGui::Button("Options"))
            ImGui::OpenPopup("Options");
        ImGui::SameLine();
        bool clear = ImGui::Button("Clear");
        ImGui::SameLine();
        bool copy = ImGui::Button("Copy");
        ImGui::SameLine();
        Filter.Draw("Filter", -100.0f);

        ImGui::Separator();
        ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

        if (clear)
            Clear();
        if (copy)
            ImGui::LogToClipboard();

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        const char* buf = Buf.begin();
        const char* buf_end = Buf.end();
        if (Filter.IsActive())
        {
            // In this example we don't use the clipper when Filter is enabled.
            // This is because we don't have a random access on the result on our filter.
            // A real application processing logs with ten of thousands of entries may want to store the result of
            // search/filter.. especially if the filtering function is not trivial (e.g. reg-exp).
            for (int line_no = 0; line_no < LineOffsets.Size; line_no++)
            {
                const char* line_start = buf + LineOffsets[line_no];
                const char* line_end = (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
                if (Filter.PassFilter(line_start, line_end))
                    ImGui::TextUnformatted(line_start, line_end);
            }
        }
        else
        {
            // The simplest and easy way to display the entire buffer:
            //   ImGui::TextUnformatted(buf_begin, buf_end);
            // And it'll just work. TextUnformatted() has specialization for large blob of text and will fast-forward
            // to skip non-visible lines. Here we instead demonstrate using the clipper to only process lines that are
            // within the visible area.
            // If you have tens of thousands of items and their processing cost is non-negligible, coarse clipping them
            // on your side is recommended. Using ImGuiListClipper requires
            // - A) random access into your data
            // - B) items all being the  same height,
            // both of which we can handle since we an array pointing to the beginning of each line of text.
            // When using the filter (in the block of code above) we don't have random access into the data to display
            // anymore, which is why we don't use the clipper. Storing or skimming through the search result would make
            // it possible (and would be recommended if you want to search through tens of thousands of entries).
            ImGuiListClipper clipper;
            clipper.Begin(LineOffsets.Size);
            while (clipper.Step())
            {
                for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++)
                {
                    const char* line_start = buf + LineOffsets[line_no];
                    const char* line_end = (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
                    ImGui::TextUnformatted(line_start, line_end);
                }
            }
            clipper.End();
        }
        ImGui::PopStyleVar();

        if (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
            ImGui::SetScrollHereY(1.0f);

        ImGui::EndChild();
        ImGui::End();
    }
};
