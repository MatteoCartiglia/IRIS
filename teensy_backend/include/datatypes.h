//---------------------------------------------------------------------------------------------------------------------------------------
//
//
// Author: Ciara Giles-Doran <gciara@student.ethz.ch>
// Last updated: 
//---------------------------------------------------------------------------------------------------------------------------------------

#ifndef DATATYPES_H
#define DATATYPES_H

#include <cstdint>
#include <string>
#include "constants.h"

static constexpr std::size_t MAX_PKT_BODY_LEN = SERIAL_COMMS_MAX_PKT_LEN - sizeof(std::uint8_t);

struct DAC_command;
struct BIASGEN_command;
struct AER_DECODER_OUTPUT_command;
struct AER_ENCODER_INPUT_command;

//---------------------------------------------------------------------------------------------------------------------------------------
// ENUMERATED DATATYPES
//---------------------------------------------------------------------------------------------------------------------------------------

enum class TeensyStatus 
{
    UnknownCommand                  = 0,
    Success                         = 1,
};

enum class P2tPktType 
{
    P2t_setDACvoltage               = 1U,
    P2t_setBiasGen                  = 2U,
    P2t_aerDecoder_reqOutput        = 3U
};

//---------------------------------------------------------------------------------------------------------------------------------------
// STRUCTURED DATATYPES
//---------------------------------------------------------------------------------------------------------------------------------------

struct P2TPkt 
{
    P2TPkt(){};
    P2TPkt(const DAC_command& dac);    
    P2TPkt(const BIASGEN_command& biasGen);
    P2TPkt(const AER_DECODER_OUTPUT_command& aerOutputDecoder);

    std::uint8_t header;                          // Packet length encoded in header excludes size of header
    std::uint8_t body[MAX_PKT_BODY_LEN];

}__attribute__ ((packed));

struct DAC_command
{
    DAC_command() {};
    DAC_command (const P2TPkt& pkt) : command_address(pkt.body[0]), data((pkt.body[1] << SERIAL_COMMS_SHIFT) | pkt.body[2]) {};

    std::string name;
    std::uint8_t command_address; 
    std::uint16_t data;
};

struct BIASGEN_command
{ 
    BIASGEN_command() {};
    BIASGEN_command ( const P2TPkt& pkt) : biasNo(pkt.body[0]), currentValue_binary(( pkt.body[1] << SERIAL_COMMS_SHIFT) | pkt.body[2]) {};

    std::uint8_t biasNo;
    std::uint16_t currentValue_binary;
    std::string name;
    double currentValue_uV;
    bool transistorType;
};

struct AER_DECODER_OUTPUT_command
{
    AER_DECODER_OUTPUT_command() {};
    AER_DECODER_OUTPUT_command (const P2TPkt& pkt) : data((pkt.body[0] << SERIAL_COMMS_SHIFT) | pkt.body[1]) {};

    std::uint16_t data;
};

#endif
