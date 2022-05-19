
#ifndef TEENSY_INTERFACE_H
#define TEENSY_INTERFACE_H

#include <cstdint>
#include <cstddef>

static constexpr std::size_t MAX_PKT_LEN = 5;
static constexpr unsigned int PKT_HDR_PKT_TYPE_SHIFT = 0;

static constexpr std::size_t MAX_PKT_BODY_LEN = MAX_PKT_LEN - sizeof(std::uint8_t);




enum class P2tPktType { // keep Compatible with PLANE+COACH
    P2tReset              = 1U << PKT_HDR_PKT_TYPE_SHIFT,
    P2tGetFirmwareVersion = 2U << PKT_HDR_PKT_TYPE_SHIFT,
    P2tSetDcVoltage       = 3U << PKT_HDR_PKT_TYPE_SHIFT,
    P2tRequestAerOutput   = 11U << PKT_HDR_PKT_TYPE_SHIFT,
    P2tSendEvents         = 14U << PKT_HDR_PKT_TYPE_SHIFT,
    P2tGetTeensySN        = 15U << PKT_HDR_PKT_TYPE_SHIFT,
    P2tSetBiasGen         = 16U << PKT_HDR_PKT_TYPE_SHIFT,
    P2tSetSPI             = 4U << PKT_HDR_PKT_TYPE_SHIFT,

};

//readback message
enum class TeensyStatus {
    Success = 10,
    HardResetFailed = 1,
    IncorrectCurrentSwitchRange = 2,
    CurrentCannotBeSet = 3,
    CurrentOutsideSearchRange = 4,
    UnknownCommand = 5,
    AerHandshakeFailed = 6,
    HardResetImminent = 7,  // Sent before hard reset sequence is started to give host a chance to disconnect
    HardResetNotSupported = 8,
};

struct DAC_command;
struct SPI_command;
struct BIASGEN_command;
struct AER_out;

struct P2TPkt {
    P2TPkt(const DAC_command& dc) ;
    P2TPkt(const SPI_command& sp);
    P2TPkt(const BIASGEN_command& bg);
    P2TPkt(const AER_out& aero);
    P2TPkt() {}

    std::uint8_t header; // Packet length encoded in header excludes size of header
    std::uint8_t body[MAX_PKT_BODY_LEN];
}__attribute__ ((packed));

struct DAC_command
{
    DAC_command() {};
    DAC_command (const P2TPkt& pkt) : dac_number(pkt.body[0]), command_address(pkt.body[1]), data( pkt.body[2] << 8 | pkt.body[3] ) {};

    uint8_t dac_number;
    uint8_t command_address; 
    std::uint16_t data;
};

struct SPI_command{
    SPI_command()  {};
    SPI_command ( const P2TPkt& pkt) : spi_number(pkt.body[0]), address(pkt.body[1]),  value( pkt.body[2] << 8 | pkt.body[3] ) {};

    uint8_t spi_number;
    uint8_t address;
    uint16_t value;
};

struct BIASGEN_command{ 
    BIASGEN_command() {};
    BIASGEN_command ( const P2TPkt& pkt) : address(pkt.body[0]), course_val(pkt.body[1]),  fine_val( pkt.body[2]),transistor_type(pkt.body[3]) {};

    uint8_t address;

    uint8_t course_val;
    uint8_t fine_val;
    uint8_t transistor_type;

};

struct AER_in_command{
    AER_in_command() ;
    AER_in_command (const P2TPkt& pkt) : address(pkt.body[0] << 8 | pkt.body[1] ), isi_10us(pkt.body[2] << 8 | pkt.body[3]) {};

    uint16_t address;
    uint16_t isi_10us; // in units of 10us
};

struct AER_out{
    AER_out() {};
    AER_out (const P2TPkt& pkt) : address(pkt.body[0]), ts_1ms(pkt.body[1] <<pkt.body[2]) {};

    uint8_t address;
    uint16_t ts_1ms;
};


#endif 
