#include "teensy_interface.h"

P2TPkt::P2TPkt(const DAC_command& dc) : header((std::uint8_t)P2tPktType::P2tSetDcVoltage) {
    body[0] = dc.dac_number; 
    body[1] = dc.command_address; 
    body[2] = dc.data >> 8;
    body[3] = dc.data & 0xFF;
    };

P2TPkt::P2TPkt(const SPI_command& sp) : header((std::uint8_t)P2tPktType::P2tSetSPI) {
    body[0] = sp.spi_number; 
    body[1] = sp.address; 
    body[2] = sp.value ; 
    };
P2TPkt::P2TPkt(const BIASGEN_command& bg) : header((std::uint8_t)P2tPktType::P2tSetBiasGen) {
    body[0] = bg.address; 
    body[1] = bg.course_val; 
    body[2] = bg.fine_val; 
    body[3] = bg.transistor_type; 
    };

P2TPkt::P2TPkt(const AER_out& aero) : header((std::uint8_t)P2tPktType::P2tRequestAerOutput) {
    };

