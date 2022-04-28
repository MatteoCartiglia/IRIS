#include <cstdint>

static constexpr std::size_t MAX_PKT_LEN = 5;
static constexpr unsigned int PKT_HDR_PKT_TYPE_SHIFT = 0;

static constexpr std::size_t MAX_PKT_BODY_LEN = MAX_PKT_LEN - sizeof(std::uint8_t);


struct P2TPkt {
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
};

struct SPI_command{
    SPI_command();
    SPI_command ( const P2TPkt& pkt) : spi_number(pkt.body[0]), address(pkt.body[1]), value(pkt.body[2]) {};

    uint8_t spi_number;
    uint8_t address;
    uint8_t value;
};

struct AER_in_command{
    AER_in_command();
    AER_in_command (const P2TPkt& pkt) : address(pkt.body[0] << 8 | pkt.body[1] ), isi_10us(pkt.body[2] << 8 | pkt.body[3]) {};

    uint16_t address;
    uint16_t isi_10us; // in units of 10us
};

struct AER_out{
    AER_out();
    AER_out (const P2TPkt& pkt) : address(pkt.body[0]), ts_1ms(pkt.body[1] <<pkt.body[2]) {};

    uint8_t address;
    uint16_t ts_1ms;
};



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
enum class TeensyMessage { 
    Success              = 0,
    Reset_fail           = 2,
    SetSPI_fail          = 3,
    SetDC_fail           = 4,
    Aerout_Failed        = 5,

};


