//---------------------------------------------------------------------------------------------------------------------------------------
//
//
// Author: 
// Last updated: 
//---------------------------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include "../include/datatypes.h"

P2TPkt::P2TPkt(const DAC_command& dac) : header((std::uint8_t)P2tPktType::P2t_SetDACvoltage) 
{
    body[0] = dac.command_address; 
    body[1] = dac.data >> SERIAL_COMMS_SHIFT;
    body[2] = dac.data & 0xFF;
};

P2TPkt::P2TPkt(const BIASGEN_command& biasGen) : header((std::uint8_t)P2tPktType::P2t_SetBiasGen) 
{
    body[0] = biasGen.address; 
    body[1] = biasGen.currentValue_binary >> SERIAL_COMMS_SHIFT;
    body[2] = biasGen.currentValue_binary & 0xFF;
};

P2TPkt::P2TPkt(const AER_DECODER_OUTPUT_command& aerOutputDecoder) : header((std::uint8_t)P2tPktType::P2t_RequestAerDecoder_Output) 
{
    body[0] = aerOutputDecoder.core; 
    body[1] = aerOutputDecoder.syn_type; 
    body[2] = aerOutputDecoder.configs; 
};

P2TPkt::P2TPkt(const AER_ENCODER_INPUT_command& aerInputEncoder) : header((std::uint8_t)P2tPktType::P2t_AckAerEncoder_Input) 
{
    body[0] = aerInputEncoder.address; 
    body[1] = aerInputEncoder.ts_1ms; 
};