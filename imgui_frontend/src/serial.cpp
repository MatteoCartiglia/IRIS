//---------------------------------------------------------------------------------------------------------------------------------------
// Source file for functions related to serial port reading and writing
//
// Author: Ciara Giles-Doran <gciara@student.ethz.ch>
// Last updated: 
//---------------------------------------------------------------------------------------------------------------------------------------

#include "../include/serial.h"
#include "../include/guiFunctions.h"
#include <chrono>
#include <thread>

//---------------------------------------------------------------------------------------------------------------------------------------
// loadBiasValues: Sends the new DAC values to the Teensy 
//---------------------------------------------------------------------------------------------------------------------------------------

void loadBiasValues(DAC_command dac[], int serialPort )
{
    for (int i = 0; i< DAC_CHANNELS_USED; i++)
    {
        Pkt p2t_pk(dac[i]); 
        write(serialPort, (void *) &p2t_pk, sizeof(p2t_pk));
        std::this_thread::sleep_until(std::chrono::system_clock::now()+ std::chrono::microseconds(100) );
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------
// loadBiasGenValues: Sends the new BIASGEN values to the Teensy 
//---------------------------------------------------------------------------------------------------------------------------------------
void loadBiasValues(BIASGEN_command bg[], int serialPort)
{
    for (int i = 0; i< BIASGEN_CHANNELS; i++)
    {
        Pkt p2t_pk(bg[i]); 
        write(serialPort, (void *) &p2t_pk, sizeof(p2t_pk));
        std::this_thread::sleep_until(std::chrono::system_clock::now()+ std::chrono::microseconds(100) );

    }
}

//---------------------------------------------------------------------------------------------------------------------------------------
// getSerialData: Reads data in serial port and writes entry to Log window
//---------------------------------------------------------------------------------------------------------------------------------------

void getSerialData(int serialPort, bool show_Serial_output, int expectedResponses, int bufferSize)
{
    int serialReadBytes = 0;

    while(expectedResponses > 0)
    {
        char serialReadBuffer[bufferSize];
        std::fill(serialReadBuffer, serialReadBuffer + bufferSize, SERIAL_ASCII_SPACE);

        serialReadBytes = read(serialPort, &serialReadBuffer, bufferSize);
        
        if((serialReadBytes != 0) && (serialReadBytes != -1))
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
// getSerialData_Plots: Reads data in serial port and updates plots displayed
//---------------------------------------------------------------------------------------------------------------------------------------
void getSerialData_Plots(int serialPort, bool show_PlotData)
{
    long time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    int serialReadBytes = 0;

    // -------------------------------------------- Get Encoder Output & Update Plots ---------------------------------------------

    if(getHandshakeStatus(TEENSY_INPUT_ENCODER))
    {
        updatePlotWindow_Encoder(show_PlotData, time_ms, 0, serialPort);
    }

    else if (!getHandshakeStatus(TEENSY_INPUT_ENCODER))
    {
        updatePlotWindow_Encoder(show_PlotData, time_ms, 0, serialPort);
    }


    // ---------------------------------------------- Get ALIVE C2F Output & Update Plots -----------------------------------------------

    if(getHandshakeStatus(TEENSY_INPUT_C2F))
    {
        updatePlotWindow_C2F(show_PlotData, time_ms, 0, serialPort);
       
    }

    else if(!getHandshakeStatus(TEENSY_INPUT_C2F))
    {
        updatePlotWindow_C2F(show_PlotData, time_ms, 0, serialPort);
    }
  

    // Flush the serial port
    tcflush(serialPort, TCIFLUSH);
}


//---------------------------------------------------------------------------------------------------------------------------------------
// getHandshakeReturn: Retrieves forced handshake status
//---------------------------------------------------------------------------------------------------------------------------------------

bool getHandshakeReturn(int serialPort)
{
    int serialReadBytes = 0;
    int serialRead = false;

    serialReadBytes = read(serialPort, &serialRead, 1);
    
    if((serialReadBytes != 0) && (serialReadBytes != -1))
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