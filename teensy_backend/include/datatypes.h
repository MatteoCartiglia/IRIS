//---------------------------------------------------------------------------------------------------------------------------------------
// Header file defining enumerated and structured datatypes
//
// Author: Matteo Cartiglia <camatteo@ini.uzh.ch>
// Last updated: 16 AUG 2022 (Ciara Giles-Doran)
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
struct C2F_OUTPUT_command;
struct SPI_INPUT_command;
struct HANDSHAKE_C2F_command;
struct HANDSHAKE_ENCODER_command;

//---------------------------------------------------------------------------------------------------------------------------------------
// ENUMERATED DATATYPES
//---------------------------------------------------------------------------------------------------------------------------------------


// ---------------------------------------------------- Teensy Status Return Types ------------------------------------------------------
enum class TeensyStatus 
{
    UnknownCommand                  = 0U,
    Success                         = 1U,
};


// --------------------------------------------------- PC -> Teensy Comm Packet Types --------------------------------------------------
enum class PktType 
{
    Pkt_setDACvoltage               = 1U,
    Pkt_setBiasGen                  = 2U,
    Pkt_reqOutputDecoder            = 3U,
    Pkt_reqInputEncoder             = 4U,
    Pkt_reqInputC2F                 = 5U,
    Pkt_setSPI                      = 6U,
    Pkt_handshakeC2F                = 7U,
    Pkt_handshakeEncoder            = 8U
};


//---------------------------------------------------------------------------------------------------------------------------------------
// STRUCTURED DATATYPES
//---------------------------------------------------------------------------------------------------------------------------------------

// ------------------------------------------------- ALIVE -> Teensy -> PC data struct --------------------------------------------------
struct outputALIVE
{
    uint8_t data;
    unsigned long timestamp;
};

// -------------------------------------------------- PC -> Teensy Comm Packet Struct ---------------------------------------------------

struct Pkt 
{
    Pkt(){};
    Pkt(const DAC_command& dac);    
    Pkt(const BIASGEN_command& biasGen);
    Pkt(const AER_DECODER_OUTPUT_command& outputDecoder);
    Pkt(const ENCODER_INPUT_command& inputEncoder);
    Pkt(const C2F_INPUT_command& inputC2F);
    Pkt(const SPI_INPUT_command& spi);
    Pkt(const HANDSHAKE_C2F_command& handshakeC2F);
    Pkt(const HANDSHAKE_ENCODER_command& handshakeEncoder);

    std::uint8_t header;                                        // Packet length encoded in header excludes size of header
    std::uint8_t body[MAX_PKT_BODY_LEN];

}__attribute__ ((packed));


// -------------------------------------------- Struct for PC -> Teensy -> DAC communication --------------------------------------------

struct DAC_command
{
    DAC_command() {};
    DAC_command (const Pkt& pkt) : dac_number(pkt.body[0]), command_address(pkt.body[1]), data( pkt.body[2] << SERIAL_COMMS_SHIFT | pkt.body[3] ) {};

    std::uint8_t dac_number;
    std::uint8_t command_address; 
    std::uint16_t data;
    std::string name;

};


// ------------------------------------------ Struct for PC -> Teensy -> BIASGEN communication ------------------------------------------

struct BIASGEN_command
{ 
    BIASGEN_command() {};
    BIASGEN_command ( const Pkt& pkt) : biasNo((pkt.body[0] << SERIAL_COMMS_SHIFT) | pkt.body[1]), currentValue_binary(( pkt.body[2] << SERIAL_COMMS_SHIFT) | pkt.body[3]) {};

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
    AER_DECODER_OUTPUT_command (const Pkt& pkt) : data((pkt.body[0] << SERIAL_COMMS_SHIFT) | pkt.body[1]) {};

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

// --------------------------------------------- Struct for PC -> Teensy -> SPI communication -------------------------------------------

struct SPI_INPUT_command
{
    SPI_INPUT_command()  {};
    SPI_INPUT_command ( const Pkt& pkt) : spi_number(pkt.body[0]), address((pkt.body[1] << SERIAL_COMMS_SHIFT) | pkt.body[2]), value((pkt.body[3] << SERIAL_COMMS_SHIFT) | pkt.body[4]) {};

    uint8_t spi_number;
    uint16_t address;
    uint16_t value;
};

struct HANDSHAKE_C2F_command
{
    HANDSHAKE_C2F_command() {};
};

struct HANDSHAKE_ENCODER_command
{
    HANDSHAKE_ENCODER_command() {};
};


#endif
