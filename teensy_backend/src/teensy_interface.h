
#ifndef TEENSY_INTERFACE_H
#define TEENSY_INTERFACE_H

#include <cstdint>

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
    P2tSetSPI             = 16U << PKT_HDR_PKT_TYPE_SHIFT,

};


//readback message
enum class TeensyStatus {
    Success = 0,
    HardResetFailed = 1,
    IncorrectCurrentSwitchRange = 2,
    CurrentCannotBeSet = 3,
    CurrentOutsideSearchRange = 4,
    UnknownCommand = 5,
    AerHandshakeFailed = 6,
    HardResetImminent = 7,  // Sent before hard reset sequence is started to give host a chance to disconnect
    HardResetNotSupported = 8,
};




struct P2TPkt {
    P2TPkt(const DAC_command& dc) : header((std::uint8_t)P2tPktType::P2tSetDcVoltage) {
        body[0] = dc.dac_number; 
        body[1] = dc.command_address; 
        body[2] = dc.data >> 8;
        body[3] = dc.data & 0xFF;
        };

    P2TPkt(const SPI_command& sp) : header((std::uint8_t)P2tPktType::P2tSetSPI) {
        body[0] = sp.spi_number; 
        body[1] = sp.address; 
        body[2] = sp.value; 
        };
    P2TPkt(const AER_out& sp) : header((std::uint8_t)P2tPktType::P2tRequestAerOutput) {
        body[0] = sp.address; 
        body[1] = sp.ts_1ms >> 8 ; 
        body[2] = sp.ts_1ms && 0xFF; 
        };
    std::uint8_t header; // Packet length encoded in header excludes size of header
    std::uint8_t body[MAX_PKT_BODY_LEN];
}__attribute__ ((packed));


struct DAC_command
{
    DAC_command();
    DAC_command (const P2TPkt& pkt) : dac_number(pkt.body[0]), command_address(pkt.body[1]), data( pkt.body[2] << 8 | pkt.body[3] ) {};

    uint8_t dac_number;
    uint8_t command_address; 
    uint16_t data;
}__attribute__ ((packed));

struct SPI_command{
    SPI_command();
    SPI_command ( const P2TPkt& pkt) : spi_number(pkt.body[0]), address(pkt.body[1]), value(pkt.body[2]) {};

    uint8_t spi_number;
    uint8_t address;
    uint8_t value;
}__attribute__ ((packed));

struct AER_in_command{
    AER_in_command();
    AER_in_command (const P2TPkt& pkt) : address(pkt.body[0] << 8 | pkt.body[1] ), isi_10us(pkt.body[2] << 8 | pkt.body[3]) {};

    uint16_t address;
    uint16_t isi_10us; // in units of 10us
}__attribute__ ((packed));

struct AER_out{
    AER_out();
    AER_out (const P2TPkt& pkt) : address(pkt.body[0]), ts_1ms(pkt.body[1] <<pkt.body[2]) {};

    uint8_t address;
    uint16_t ts_1ms;
}__attribute__ ((packed));


#endif 

