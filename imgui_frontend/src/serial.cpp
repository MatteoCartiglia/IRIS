//---------------------------------------------------------------------------------------------------------------------------------------
// Source file for functions related to serial port reading and writing
//---------------------------------------------------------------------------------------------------------------------------------------

#include "../include/serial.h"
#include "../include/guiFunctions.h"
#include <chrono>
#include <iostream>
#include <thread>
#include <fstream>

std::vector<AER_out> input_data;
std::string popupSave_str_encoder = "Save events";
const char *popupSave_encoder = popupSave_str_encoder.c_str();
bool savingEnc = false;

void save_events(const std::string &filename, std::vector<AER_out> input_data);

//---------------------------------------------------------------------------------------------------------------------------------------
// loadBiasValues: Sends the new DAC values to the Teensy
//---------------------------------------------------------------------------------------------------------------------------------------
#ifdef EXISTS_DAC

void loadBiasValues(DAC_command dac[], int serialPort)
{
    for (int i = 0; i < DAC_CHANNELS_USED; i++)
    {
        if (dac[i].data == 0)
        {
            dac[i].data = 1;
        }
        Pkt p2t_pk(dac[i]);
        write(serialPort, (void *)&p2t_pk, sizeof(p2t_pk));
        std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::microseconds(100));
    }
}
#endif

//---------------------------------------------------------------------------------------------------------------------------------------
// loadBiasGenValues: Sends the new BIASGEN values to the Teensy
//---------------------------------------------------------------------------------------------------------------------------------------
#ifdef EXISTS_BIASGEN

void loadBiasValues(BIASGEN_command bg[], int serialPort)
{
    for (int i = 0; i < BIASGEN_CHANNELS; i++)
    {
        Pkt p2t_pk(bg[i]);
        write(serialPort, (void *)&p2t_pk, sizeof(p2t_pk));
        std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::microseconds(100));
    }
}
#endif
//---------------------------------------------------------------------------------------------------------------------------------------
// getSerialData: Reads data in serial port and writes entry to Log window
//---------------------------------------------------------------------------------------------------------------------------------------

void getSerialData(int serialPort, bool show_Serial_output, int expectedResponses, int bufferSize)
{
    int serialReadBytes = 0;

    while (expectedResponses > 0)
    {
        char serialReadBuffer[bufferSize];
        std::fill(serialReadBuffer, serialReadBuffer + bufferSize, SERIAL_ASCII_SPACE);

        serialReadBytes = read(serialPort, &serialReadBuffer, bufferSize);

        if ((serialReadBytes != 0) && (serialReadBytes != -1))
        {
            updateSerialOutputWindow(show_Serial_output, true, serialReadBuffer);
        }
        else
        {
            printf("getSerialData: Error reading serial port. Serial read byte: %d\n", serialReadBytes);
        }

        expectedResponses--;
    }

    tcflush(serialPort, TCIFLUSH);
}

//---------------------------------------------------------------------------------------------------------------------------------------
// getEncoderdata: Reads data in serial port and updates plots displayed
//---------------------------------------------------------------------------------------------------------------------------------------
void getEncoderdata(int serialPort, bool show_PlotData)
{
    // int serialReadBytes = 0;
    // std::this_thread::sleep_until(std::chrono::system_clock::now()+ std::chrono::microseconds(100) );

    ImGui::Begin("Encoder Output");

    if (ImGui::Button("Handshake: Encoder", ImVec2(ImGui::GetWindowSize().x * 0.48, BUTTON_HEIGHT)))
    {
        HANDSHAKE_ENCODER_command handshakeEncoder;
        Pkt p2tpk_HandshakeEncoder(handshakeEncoder);
        write(serialPort, (void *)&p2tpk_HandshakeEncoder, sizeof(p2tpk_HandshakeEncoder));
    }

    ImGui::SameLine();
    ImGui::Text(" Output: Encoder ");
    ImGui::SameLine();

    if (ImGui::Button("Save"))
    {
        ENCODER_INPUT_command Enable_Encoder;
        Pkt p2tpk_Enable_encoder(Enable_Encoder);
        write(serialPort, (void *)&p2tpk_Enable_encoder, sizeof(p2tpk_Enable_encoder));
        // handshakeStatusEncoder = getHandshakeReturn(serialPort);
        savingEnc = !savingEnc;
        // serialDataSent++;
    }
    if (savingEnc)
    {
        GetAerEncoderOutput trasmit;
        Pkt p2t_pk(trasmit);
        write(serialPort, (void *)&p2t_pk, sizeof(p2t_pk));
        Aer_Data_Pkt aer_data;
        std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::microseconds(500));

        int serialReadBytes = read(serialPort, (void *)&aer_data, sizeof(aer_data));
        // std::cout << "retval: " << serialReadBytes <<std::endl;
        if (serialReadBytes == -1)
        {
            std::cout << "Error= -1" << std::endl;
        }
        else if (serialReadBytes == 0)
        {
            std::cout << "Error = no data read" << std::endl;
        }
        else
        {
            //  assert(serialReadBytes == (aer_data.number_events * sizeof(AER_out)) +sizeof(aer_data.number_events));
            // if (aer_data.number_events > MAX_EVENTS_PER_PACKET)

            std::cout << " number_events: " << static_cast<int16_t>(aer_data.number_events) << std::endl;

            if (aer_data.number_events > 0 && aer_data.number_events < MAX_EVENTS_PER_PACKET)
            {
                for (int j = 0; j < (aer_data.number_events); j++)
                {
                    input_data.push_back(aer_data.body[j]);
                    std::cout << "Data: " << aer_data.body[j].data << " Ts: " << aer_data.body[j].timestamp << std::endl;
                }
            }
        }
    }
    ImGui::SameLine();
    ImGui::Checkbox("Saving: ", &savingEnc);
    ImGui::NewLine();

    if (ImGui::Button("Save to file"))
    {
        save_events(ENCODER_INPUT_SAVE_FILENAME_CSV, input_data);
        input_data.clear();
    }
    // Flush the serial port
    tcflush(serialPort, TCIFLUSH);

    ImGui::End();
}

void save_events(const std::string &filename, std::vector<AER_out> input_data)
{
    std::ofstream file(filename, std::ios::out | std::ios::app);
    if (file.is_open())
    {
        file << "data, ts (in micros)" << '\n';

        for (unsigned i = 0; i < input_data.size(); i++)
        {
            file << input_data[i].data << ',';
            file << input_data[i].timestamp;
            file << '\n';
        }

        input_data.clear();
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------
// getHandshakeReturn: Retrieves forced handshake status ??? buggy
//---------------------------------------------------------------------------------------------------------------------------------------

bool getHandshakeReturn(int serialPort)
{
    int serialReadBytes = 0;
    int serialRead = false;

    serialReadBytes = read(serialPort, &serialRead, 1); // buggy

    if ((serialReadBytes != 0) && (serialReadBytes != -1))
    {
        tcflush(serialPort, TCIFLUSH);
        return char(serialRead);
    }
    else
    {
        printf("getHandshakeReturn: Error reading serial port. Serial read byte: %d\n", serialReadBytes);
        tcflush(serialPort, TCIFLUSH);
        return false;
    }
}