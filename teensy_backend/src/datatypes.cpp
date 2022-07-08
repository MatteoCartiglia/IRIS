//---------------------------------------------------------------------------------------------------------------------------------------
//
//
// Author: Ciara Giles-Doran <gciara@student.ethz.ch>
// Last updated: 
//---------------------------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include "../include/datatypes.h"

P2TPkt::P2TPkt(const DAC_command& dc) : header((std::uint8_t)P2tPktType::P2tSetDcVoltage) 
{
    body[0] = dc.command_address; 
    body[1] = dc.data >> 8;
    body[2] = dc.data & 0xFF;
};

P2TPkt::P2TPkt(const BIASGEN_command& bg) : header((std::uint8_t)P2tPktType::P2tSetBiasGen) 
{
    body[0] = bg.address >> 8 ; 
    body[1] = bg.address; 
    body[2] = bg.course_val; 
    body[3] = bg.fine_val; 
    body[4] = bg.transistorType; 
};