//---------------------------------------------------------------------------------------------------------------------------------------
//
//
// Author: 
// Last updated: 
//---------------------------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include "../include/datatypes.h"

P2TPkt::P2TPkt() : header((std::uint8_t)P2tPktType::P2t_emptyBuffer){};


P2TPkt::P2TPkt(const DAC_command& dac) : header(static_cast<std::underlying_type<P2tPktType>::type>(P2tPktType::P2t_setDACvoltage)) 
{
    body[0] = dac.command_address; 
    body[1] = dac.data >> SERIAL_COMMS_SHIFT;
    body[2] = dac.data & BINARY_255;
};

P2TPkt::P2TPkt(const BIASGEN_command& biasGen) : header(static_cast<std::underlying_type<P2tPktType>::type>(P2tPktType::P2t_setBiasGen)) 
{
    body[0] = biasGen.address; 
    body[1] = biasGen.currentValue_binary >> SERIAL_COMMS_SHIFT;
    body[2] = biasGen.currentValue_binary & BINARY_255;
};

P2TPkt::P2TPkt(const AER_DECODER_OUTPUT_command& aerOutputDecoder) : header(static_cast<std::underlying_type<P2tPktType>::type>(P2tPktType::P2t_aerDecoder_reqOutput)) 
{
    body[0] = aerOutputDecoder.data  >> SERIAL_COMMS_SHIFT;
    body[1] = aerOutputDecoder.data & BINARY_255;
};

P2TPkt::P2TPkt(const AER_ENCODER_INPUT_command& aerInputEncoder) : header(static_cast<std::underlying_type<P2tPktType>::type>(P2tPktType::P2t_aerEncoder_ackInput)) 
{
    body[0] = aerInputEncoder.address; 
    body[1] = aerInputEncoder.ts_1ms; 
};