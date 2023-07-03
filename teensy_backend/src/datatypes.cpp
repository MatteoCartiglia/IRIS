//---------------------------------------------------------------------------------------------------------------------------------------
// Source file initialising PC -> Teensy Comm Packets
//---------------------------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include "../include/datatypes.h"

//---------------------------------------------------------------------------------------------------------------------------------------
// DAC Command Packet
//---------------------------------------------------------------------------------------------------------------------------------------

Pkt::Pkt(const DAC_command &dac) : header(static_cast<std::underlying_type<PktType>::type>(PktType::Pkt_setDACvoltage))
{
    body[0] = dac.dac_number;
    body[1] = dac.command_address;
    body[2] = dac.data >> 8;
    body[3] = dac.data & 0xFF;
};

//---------------------------------------------------------------------------------------------------------------------------------------
// BIASGEN Command Packet
//---------------------------------------------------------------------------------------------------------------------------------------

Pkt::Pkt(const BIASGEN_command &biasGen) : header(static_cast<std::underlying_type<PktType>::type>(PktType::Pkt_setBiasGen))
{
    body[0] = biasGen.biasNo >> 8;
    body[1] = biasGen.biasNo & 0xFF;
    body[2] = biasGen.currentValue_binary >> 8;
    body[3] = biasGen.currentValue_binary & 0xFF;
    body[4] = biasGen.transistorType;
};

//---------------------------------------------------------------------------------------------------------------------------------------
// DECODER Command Packet
//---------------------------------------------------------------------------------------------------------------------------------------

Pkt::Pkt(const AER_DECODER_OUTPUT_command &outputDecoder) : header(static_cast<std::underlying_type<PktType>::type>(PktType::Pkt_reqOutputDecoder))
{
    body[0] = outputDecoder.data >> 24;
    body[1] = outputDecoder.data >> 16;
    body[2] = outputDecoder.data >> 8;
    body[3] = outputDecoder.data & 0xFF;
};

//---------------------------------------------------------------------------------------------------------------------------------------
// ENCODER Command Packet
//---------------------------------------------------------------------------------------------------------------------------------------

Pkt::Pkt(const ENCODER_INPUT_command &inputEncoder) : header(static_cast<std::underlying_type<PktType>::type>(PktType::P2tRequestAerEncoderOutput)){};

//---------------------------------------------------------------------------------------------------------------------------------------
// C2F Command Packet
//---------------------------------------------------------------------------------------------------------------------------------------

Pkt::Pkt(const C2F_INPUT_command &inputC2F) : header(static_cast<std::underlying_type<PktType>::type>(PktType::P2tRequestAerC2FOutput)){};

//---------------------------------------------------------------------------------------------------------------------------------------
// SPI Command Packet
//---------------------------------------------------------------------------------------------------------------------------------------

Pkt::Pkt(const SPI_INPUT_command &spi_command) : header(static_cast<std::underlying_type<PktType>::type>(PktType::Pkt_setSPI))
{
    body[0] = spi_command.spi_number;
    body[1] = spi_command.address >> 8;
    body[2] = spi_command.address & 0xFF;
    body[3] = spi_command.value >> 8;
    body[4] = spi_command.value & 0xFF;
};

//---------------------------------------------------------------------------------------------------------------------------------------
// AER out Packet
//---------------------------------------------------------------------------------------------------------------------------------------

Aer_Data_Pkt::Aer_Data_Pkt(AER_out event_buffer[], int8_t n) : number_events(n)
{
    for (int i = 0; i < n; i++)
    {
        body[i] = event_buffer[i];
    }
};

//---------------------------------------------------------------------------------------------------------------------------------------
// Command Packet
//---------------------------------------------------------------------------------------------------------------------------------------

Pkt::Pkt(const HANDSHAKE_C2F_command &handshakeC2F) : header(static_cast<std::underlying_type<PktType>::type>(PktType::Pkt_handshakeC2F)){};
Pkt::Pkt(const HANDSHAKE_ENCODER_command &handshakeEncoder) : header(static_cast<std::underlying_type<PktType>::type>(PktType::Pkt_handshakeEncoder)){};
Pkt::Pkt(const GetAerEncoderOutput &GetAerEncoderOutput) : header(static_cast<std::underlying_type<PktType>::type>(PktType::PktGetAerEncoderOutput)){};

Pkt::Pkt(const RESET_command &reset) : header(static_cast<std::underlying_type<PktType>::type>(PktType::PktResetChip))
{
    body[0] = reset.parameter;
};
