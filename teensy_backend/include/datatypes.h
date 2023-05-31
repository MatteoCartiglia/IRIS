//---------------------------------------------------------------------------------------------------------------------------------------
// Header file defining enumerated and structured datatypes
//---------------------------------------------------------------------------------------------------------------------------------------

#ifndef DATATYPES_H
#define DATATYPES_H

#include <cstdint>
#include <string>
#include "constants.h"
#include "constants_global.h"

static constexpr std::size_t MAX_PKT_BODY_LEN = SERIAL_COMMS_MAX_PKT_LEN - sizeof(std::uint8_t); // Clarify

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
struct AER_out;
struct GetAerEncoderOutput;
struct RESET_command;

//---------------------------------------------------------------------------------------------------------------------------------------
// ENUMERATED DATATYPES
//---------------------------------------------------------------------------------------------------------------------------------------

// ---------------------------------------------------- Teensy Status Return Types ------------------------------------------------------
enum class TeensyStatus
{
    UnknownCommand = 0U,
    Success = 1U,
};

// --------------------------------------------------- PC <-> Teensy Comm Packet Types --------------------------------------------------
enum class PktType
{
    Pkt_setDACvoltage = 1U,
    Pkt_setBiasGen = 2U,
    Pkt_reqOutputDecoder = 3U,
    P2tRequestAerEncoderOutput = 4U,
    P2tRequestAerC2FOutput = 5U,
    Pkt_setSPI = 6U,
    Pkt_handshakeC2F = 7U,
    Pkt_handshakeEncoder = 8U,
    Pkt_aeroevent = 9U,
    PktGetAerEncoderOutput = 10U,
    PktResetChip = 11U

};

//---------------------------------------------------------------------------------------------------------------------------------------
// STRUCTURED DATATYPES
//---------------------------------------------------------------------------------------------------------------------------------------

// -------------------------------------------------- PC <-> Teensy Comm Packet Struct ---------------------------------------------------

struct Pkt
{
    Pkt(){};
    Pkt(const DAC_command &dac);
    Pkt(const BIASGEN_command &biasGen);
    Pkt(const AER_DECODER_OUTPUT_command &outputDecoder);
    Pkt(const ENCODER_INPUT_command &inputEncoder);
    Pkt(const C2F_INPUT_command &inputC2F);
    Pkt(const SPI_INPUT_command &spi);
    Pkt(const HANDSHAKE_C2F_command &handshakeC2F);
    Pkt(const HANDSHAKE_ENCODER_command &handshakeEncoder);
    Pkt(const GetAerEncoderOutput &AerEncoderOutput);
    Pkt(const RESET_command &reset);

    std::uint8_t header; // Packet length encoded in header excludes size of header
    std::uint8_t body[MAX_PKT_BODY_LEN];

} __attribute__((packed));

// -------------------------------------------- Struct for PC -> Teensy -> DAC communication --------------------------------------------

struct DAC_command
{
    DAC_command(){};
    DAC_command(const Pkt &pkt) : dac_number(pkt.body[0]), command_address(pkt.body[1]), data(pkt.body[2] << SERIAL_COMMS_SHIFT | pkt.body[3]){};

    std::uint8_t dac_number;
    std::uint8_t command_address;
    std::uint16_t data;
    std::string name;
};

// ------------------------------------------ Struct for PC -> Teensy -> BIASGEN communication ------------------------------------------

struct BIASGEN_command
{
    BIASGEN_command(){};
    BIASGEN_command(const Pkt &pkt) : biasNo((pkt.body[0] << SERIAL_COMMS_SHIFT) | pkt.body[1]), currentValue_binary((pkt.body[2] << SERIAL_COMMS_SHIFT) | pkt.body[3]), transistorType(pkt.body[4]){};

    std::uint16_t biasNo;
    std::uint16_t currentValue_binary;
    bool transistorType;

    std::string name;
    double currentValue_uA;
};

// --------------------------- Struct for Input interface PC -> Teensy -> communication [Input interface: Teensy output] ---------------------

struct AER_DECODER_OUTPUT_command
{
    AER_DECODER_OUTPUT_command(){};
    AER_DECODER_OUTPUT_command(const Pkt &pkt) : data((pkt.body[0] << SERIAL_COMMS_SHIFT) | pkt.body[1]), isi((pkt.body[2] << SERIAL_COMMS_SHIFT) | pkt.body[3]){};

    std::uint16_t data;
    std::uint16_t isi;
};

// --------------------------- Struct to enable logging of Encoder events PC -> Teensy   ---------------------

struct ENCODER_INPUT_command
{
    ENCODER_INPUT_command(){};
};

// --------------------------- Struct to enable logging of sending events PC -> Teensy   ---------------------

struct GetAerEncoderOutput
{
    GetAerEncoderOutput(){};
};

// ----------------------------- Struct to enable logging of c2f  PC-> Teensy  -----------------------

struct C2F_INPUT_command
{
    C2F_INPUT_command(){};
};

// -------------------------------------------------  Teensy -> PC data struct --------------------------------------------------

struct AER_out
{
    AER_out(){};
    AER_out(const Pkt &pkt) : data((pkt.body[0] << SERIAL_COMMS_SHIFT) | pkt.body[1]), timestamp((pkt.body[2] << SERIAL_COMMS_SHIFT * 3) | pkt.body[3] << SERIAL_COMMS_SHIFT * 2 | pkt.body[4] << SERIAL_COMMS_SHIFT * 3 | pkt.body[5]){};

    uint16_t data = 0;
    uint32_t timestamp = 0;
} __attribute__((packed));

// --------------------------------------------- Struct for PC -> Teensy -> SPI communication -------------------------------------------

struct SPI_INPUT_command
{
    SPI_INPUT_command(){};
    SPI_INPUT_command(const Pkt &pkt) : spi_number(pkt.body[0]), address((pkt.body[1] << SERIAL_COMMS_SHIFT) | pkt.body[2]), value((pkt.body[3] << SERIAL_COMMS_SHIFT) | pkt.body[4]){};

    uint8_t spi_number;
    uint16_t address;
    uint16_t value;
};

struct HANDSHAKE_C2F_command
{
    HANDSHAKE_C2F_command(){};
};

struct HANDSHAKE_ENCODER_command
{
    HANDSHAKE_ENCODER_command(){};
};

struct RESET_command
{
    RESET_command(){};
};

struct Aer_Data_Pkt
{
    Aer_Data_Pkt(){};
    Aer_Data_Pkt(AER_out event_buffer[], int8_t number_events);

    int8_t number_events = 0;
    AER_out body[MAX_EVENTS_PER_PACKET] = {};

} __attribute__((packed));

#endif
