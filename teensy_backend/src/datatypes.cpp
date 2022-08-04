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

P2TPkt::P2TPkt(const DAC_command& dac) : header(static_cast<std::underlying_type<P2tPktType>::type>(P2tPktType::P2t_setDACvoltage)) 
{
    body[0] = dac.dac_number; 
    body[1] = dac.command_address; 
    body[2] = dac.data >> SERIAL_COMMS_SHIFT;
    body[3] = dac.data & BINARY_255;

};
//---------------------------------------------------------------------------------------------------------------------------------------
// BIASGEN Command Packet
//---------------------------------------------------------------------------------------------------------------------------------------

P2TPkt::P2TPkt(const BIASGEN_command& biasGen) : header(static_cast<std::underlying_type<P2tPktType>::type>(P2tPktType::P2t_setBiasGen)) 
{
    body[0] = biasGen.biasNo  >> SERIAL_COMMS_SHIFT; 
    body[1] = biasGen.biasNo & BINARY_255; 
    body[2] = biasGen.currentValue_binary >> SERIAL_COMMS_SHIFT;
    body[3] = biasGen.currentValue_binary & BINARY_255;
};

//---------------------------------------------------------------------------------------------------------------------------------------
// DECODER Command Packet
//---------------------------------------------------------------------------------------------------------------------------------------

P2TPkt::P2TPkt(const AER_DECODER_OUTPUT_command& outputDecoder) : header(static_cast<std::underlying_type<P2tPktType>::type>(P2tPktType::P2t_reqOutputDecoder)) 
{
    body[0] = outputDecoder.data  >> SERIAL_COMMS_SHIFT;
    body[1] = outputDecoder.data & BINARY_255;
};

//---------------------------------------------------------------------------------------------------------------------------------------
// ENCODER Command Packet
//---------------------------------------------------------------------------------------------------------------------------------------

P2TPkt::P2TPkt(const ENCODER_INPUT_command& inputEncoder) : header(static_cast<std::underlying_type<P2tPktType>::type>(P2tPktType::P2t_reqInputEncoder)) {};

//---------------------------------------------------------------------------------------------------------------------------------------
// C2F Command Packet
//---------------------------------------------------------------------------------------------------------------------------------------

P2TPkt::P2TPkt(const C2F_INPUT_command& inputC2F) : header(static_cast<std::underlying_type<P2tPktType>::type>(P2tPktType::P2t_reqInputC2F)) {};


P2TPkt::P2TPkt(const SPI_INPUT_command& spi_command) : header(static_cast<std::underlying_type<P2tPktType>::type>(P2tPktType::P2t_setSPI)) 
{ 
    body[0] = spi_command.spi_number; 
    body[1] = spi_command.address >> SERIAL_COMMS_SHIFT; 
    body[2] = spi_command.address & BINARY_255;
    body[3] = spi_command.value >> SERIAL_COMMS_SHIFT; 
    body[4] = spi_command.value & BINARY_255;

};
