//---------------------------------------------------------------------------------------------------------------------------------------
// Source file initialising PC -> Teensy Comm Packets
//
// Author: Matteo Cartiglia <camatteo@ini.uzh.ch>
// Last updated: 15 JUL 2022 (Ciara Giles-Doran <gciara@student.ethz.ch>)
//---------------------------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include "../include/datatypes.h"

//---------------------------------------------------------------------------------------------------------------------------------------
// DAC Command Packet
//---------------------------------------------------------------------------------------------------------------------------------------

Pkt::Pkt(const DAC_command& dac) : header(static_cast<std::underlying_type<PktType>::type>(PktType::Pkt_setDACvoltage)) 
{
    body[0] = dac.dac_number; 
    body[1] = dac.command_address; 
    body[2] = dac.data >> SERIAL_COMMS_SHIFT;
    body[3] = dac.data & BINARY_255;

};

//---------------------------------------------------------------------------------------------------------------------------------------
// BIASGEN Command Packet
//---------------------------------------------------------------------------------------------------------------------------------------

Pkt::Pkt(const BIASGEN_command& biasGen) : header(static_cast<std::underlying_type<PktType>::type>(PktType::Pkt_setBiasGen)) 
{
    body[0] = biasGen.biasNo >> SERIAL_COMMS_SHIFT; 
    body[1] = biasGen.biasNo & BINARY_255; 
    body[2] = biasGen.currentValue_binary >> SERIAL_COMMS_SHIFT;
    body[3] = biasGen.currentValue_binary & BINARY_255;
};

//---------------------------------------------------------------------------------------------------------------------------------------
// DECODER Command Packet
//---------------------------------------------------------------------------------------------------------------------------------------

Pkt::Pkt(const AER_DECODER_OUTPUT_command& outputDecoder) : header(static_cast<std::underlying_type<PktType>::type>(PktType::Pkt_reqOutputDecoder)) 
{
    body[0] = outputDecoder.data  >> SERIAL_COMMS_SHIFT;
    body[1] = outputDecoder.data & BINARY_255;
};

//---------------------------------------------------------------------------------------------------------------------------------------
// ENCODER Command Packet
//---------------------------------------------------------------------------------------------------------------------------------------

Pkt::Pkt(const ENCODER_INPUT_command& inputEncoder) : header(static_cast<std::underlying_type<PktType>::type>(PktType::P2tRequestAerEncoderOutput)) {};

//---------------------------------------------------------------------------------------------------------------------------------------
// C2F Command Packet
//---------------------------------------------------------------------------------------------------------------------------------------

Pkt::Pkt(const C2F_INPUT_command& inputC2F) : header(static_cast<std::underlying_type<PktType>::type>(PktType::P2tRequestAerC2FOutput)) {};

//---------------------------------------------------------------------------------------------------------------------------------------
// SPI Command Packet
//---------------------------------------------------------------------------------------------------------------------------------------

Pkt::Pkt(const SPI_INPUT_command& spi_command) : header(static_cast<std::underlying_type<PktType>::type>(PktType::Pkt_setSPI)) 
{ 
    body[0] = spi_command.spi_number; 
    body[1] = spi_command.address >> SERIAL_COMMS_SHIFT; 
    body[2] = spi_command.address & BINARY_255;
    body[3] = spi_command.value >> SERIAL_COMMS_SHIFT; 
    body[4] = spi_command.value & BINARY_255;
};

//---------------------------------------------------------------------------------------------------------------------------------------
// AER out Packet
//---------------------------------------------------------------------------------------------------------------------------------------

Aer_Data_Pkt::Aer_Data_Pkt(const AER_out event_buffer[], size_t n) : number_events(n)
{
    for (int i = 0; i < n; i++)
    {
        body[i] = event_buffer[i];
    }
};



//---------------------------------------------------------------------------------------------------------------------------------------
// C2F HANDSHAKE Command Packet
//---------------------------------------------------------------------------------------------------------------------------------------

Pkt::Pkt(const HANDSHAKE_C2F_command& handshakeC2F) : header(static_cast<std::underlying_type<PktType>::type>(PktType::Pkt_handshakeC2F)) {};
Pkt::Pkt(const HANDSHAKE_ENCODER_command& handshakeEncoder) : header(static_cast<std::underlying_type<PktType>::type>(PktType::Pkt_handshakeEncoder)) {};
Pkt::Pkt(const GetAerEncoderOutput& GetAerEncoderOutput) : header(static_cast<std::underlying_type<PktType>::type>(PktType::PktGetAerEncoderOutput)) {};