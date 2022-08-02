//---------------------------------------------------------------------------------------------------------------------------------------
// Header file defining enumerated and structured datatypes
//
// Author: Matteo Cartiglia <camatteo@ini.uzh.ch>
// Last updated: 15 JUL 2022 (Ciara Giles-Doran <gciara@student.ethz.ch>)
//---------------------------------------------------------------------------------------------------------------------------------------

#ifndef DATATYPES_H
#define DATATYPES_H

#include <cstdint>
#include <string>
#include "constants.h"

static constexpr std::size_t MAX_PKT_BODY_LEN = SERIAL_COMMS_MAX_PKT_LEN - sizeof(std::uint8_t);    // Clarify

// ---------------------------------------------------- Defining struct prototypes ------------------------------------------------------
struct DAC_command;
struct BIASGEN_command;
struct AER_DECODER_OUTPUT_command;
struct AER_ENCODER_INPUT_command;
struct ENCODER_INPUT_command;
struct C2F_INPUT_command;


//---------------------------------------------------------------------------------------------------------------------------------------
// ENUMERATED DATATYPES
//---------------------------------------------------------------------------------------------------------------------------------------


// ---------------------------------------------------- Teensy Status Return Types ------------------------------------------------------
enum class TeensyStatus 
{
    UnknownCommand                  = 0,
    Success                         = 1,
};


// --------------------------------------------------- PC -> Teensy Comm Packet Types --------------------------------------------------
enum class P2tPktType 
{
    P2t_setDACvoltage               = 1U,
    P2t_setBiasGen                  = 2U,
    P2t_reqOutputDecoder            = 3U,
    P2t_reqInputEncoder             = 4U,
    P2t_reqInputC2F                 = 5U,
    P2t_setSPI                      = 6U,
};


//---------------------------------------------------------------------------------------------------------------------------------------
// STRUCTURED DATATYPES
//---------------------------------------------------------------------------------------------------------------------------------------


// -------------------------------------------------- PC -> Teensy Comm Packet Struct ---------------------------------------------------
struct P2TPkt 
{
    P2TPkt(){};
    P2TPkt(const DAC_command& dac);    
    P2TPkt(const BIASGEN_command& biasGen);
    P2TPkt(const AER_DECODER_OUTPUT_command& outputDecoder);
    P2TPkt(const ENCODER_INPUT_command& inputEncoder);
    P2TPkt(const C2F_INPUT_command& inputC2F);

    std::uint8_t header;                          // Packet length encoded in header excludes size of header
    std::uint8_t body[MAX_PKT_BODY_LEN];

}__attribute__ ((packed));


// -------------------------------------------- Struct for PC -> Teensy -> DAC communication --------------------------------------------
struct DAC_command
{
    DAC_command() {};
    DAC_command (const P2TPkt& pkt) : dac_number(pkt.body[0]), command_address(pkt.body[1]), data( pkt.body[2] << SERIAL_COMMS_SHIFT | pkt.body[3] ) {};


    std::uint8_t dac_number;
    std::uint8_t command_address; 
    std::uint16_t data;
    std::string name;

};


// ------------------------------------------ Struct for PC -> Teensy -> BIASGEN communication ------------------------------------------
struct BIASGEN_command
{ 
    BIASGEN_command() {};
    BIASGEN_command ( const P2TPkt& pkt) : biasNo((pkt.body[0] << SERIAL_COMMS_SHIFT) | pkt.body[1]), currentValue_binary(( pkt.body[2] << SERIAL_COMMS_SHIFT) | pkt.body[3]) {};

    std::uint16_t biasNo;
    std::uint16_t currentValue_binary;
    std::string name;
    double currentValue_uA;
    bool transistorType;
};

// --------------------------- Struct for PC -> Teensy -> ALIVE communication [DECODER: Teensy output, ALIVE input] ---------------------
struct AER_DECODER_OUTPUT_command
{
    AER_DECODER_OUTPUT_command() {};
    AER_DECODER_OUTPUT_command (const P2TPkt& pkt) : data((pkt.body[0] << SERIAL_COMMS_SHIFT) | pkt.body[1]) {};

    std::uint16_t data;
};

// --------------------------- Struct for PC -> Teensy <- ALIVE communication [ENCODER: Teensy input, ALIVE output] ---------------------
struct ENCODER_INPUT_command
{
    ENCODER_INPUT_command() {};
};

// ----------------------------- Struct for PC -> Teensy <- ALIVE communication [C2F: Teensy input, ALIVE output] -----------------------
struct C2F_INPUT_command
{
    C2F_INPUT_command() {};
};

#endif
