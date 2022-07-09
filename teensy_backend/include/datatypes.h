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

static constexpr std::size_t MAX_PKT_BODY_LEN = MAX_PKT_LEN - sizeof(std::uint8_t);

struct DAC_command;
struct BIASGEN_command;
struct AER_DECODER_OUTPUT_command;
struct AER_ENCODER_INPUT_command;
struct AER_C2F_INPUT_command;

//---------------------------------------------------------------------------------------------------------------------------------------
// ENUMERATED DATATYPES
//---------------------------------------------------------------------------------------------------------------------------------------

enum class TeensyStatus 
{
    UnknownCommand                  = -1,
    Success                         = 0,
};

enum class P2tPktType 
{
    P2t_SetDACvoltage               = 0U,
    P2t_SetBiasGen                  = 1U,
    P2t_RequestAerDecoder_Output    = 2U,
    P2t_AckAerEncoder_Input         = 3U,
    P2t_AckAerC2f_Input             = 4U
};

//---------------------------------------------------------------------------------------------------------------------------------------
// STRUCTURED DATATYPES
//---------------------------------------------------------------------------------------------------------------------------------------

struct P2TPkt 
{
    P2TPkt(const DAC_command& dac) ;
    P2TPkt(const BIASGEN_command& biasGen);
    P2TPkt(const AER_DECODER_OUTPUT_command& aerOutputDecoder);
    P2TPkt(const AER_ENCODER_INPUT_command& aerInputEncoder);
    P2TPkt(const AER_C2F_INPUT_command& aerInputC2F);
    P2TPkt() {}

    std::uint8_t header; // Packet length encoded in header excludes size of header
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
    BIASGEN_command ( const P2TPkt& pkt) : address(pkt.body[0]), currentValue_binary(( pkt.body[1] << SERIAL_COMMS_SHIFT) | pkt.body[2]) {};

    std::uint8_t address;
    std::uint16_t currentValue_binary;
    std::string name;
    float currentValue_uV;
    std::uint8_t transistorType;
};

struct AER_DECODER_OUTPUT_command
{
    AER_DECODER_OUTPUT_command() {};
    AER_DECODER_OUTPUT_command (const P2TPkt& pkt) : core(pkt.body[0]), syn_type(pkt.body[1]), configs(pkt.body[2]) {};

    std::uint8_t core;
    std::uint8_t syn_type;
    std::uint8_t configs;  
    std::uint16_t isi_10us;
};

struct AER_ENCODER_INPUT_command
{
    AER_ENCODER_INPUT_command() {};
    AER_ENCODER_INPUT_command (const P2TPkt& pkt) : address(pkt.body[0]), ts_1ms(pkt.body[1]) {};

    std::uint8_t address;
    std::uint8_t ts_1ms;
};

struct AER_C2F_INPUT_command
{
    AER_C2F_INPUT_command() {};
};

#endif
