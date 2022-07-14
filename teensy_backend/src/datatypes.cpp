//---------------------------------------------------------------------------------------------------------------------------------------
//
//
// Author: 
// Last updated: 
//---------------------------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include "../include/datatypes.h"

P2TPkt::P2TPkt(const DAC_command& dac) : header(static_cast<std::underlying_type<P2tPktType>::type>(P2tPktType::P2t_setDACvoltage)) 
{
    body[0] = dac.command_address; 
    body[1] = dac.data >> SERIAL_COMMS_SHIFT;
    body[2] = dac.data & BINARY_255;
};

P2TPkt::P2TPkt(const BIASGEN_command& biasGen) : header(static_cast<std::underlying_type<P2tPktType>::type>(P2tPktType::P2t_setBiasGen)) 
{
    body[0] = biasGen.biasNo; 
    body[1] = biasGen.currentValue_binary >> SERIAL_COMMS_SHIFT;
    body[2] = biasGen.currentValue_binary & BINARY_255;
};

P2TPkt::P2TPkt(const AER_DECODER_OUTPUT_command& outputDecoder) : header(static_cast<std::underlying_type<P2tPktType>::type>(P2tPktType::P2t_reqOutputDecoder)) 
{
    body[0] = outputDecoder.data  >> SERIAL_COMMS_SHIFT;
    body[1] = outputDecoder.data & BINARY_255;
};

P2TPkt::P2TPkt(const ENCODER_INPUT_command& inputEncoder) : header(static_cast<std::underlying_type<P2tPktType>::type>(P2tPktType::P2t_reqInputEncoder)) {};
P2TPkt::P2TPkt(const C2F_INPUT_command& inputC2F) : header(static_cast<std::underlying_type<P2tPktType>::type>(P2tPktType::P2t_reqInputC2F)) {};