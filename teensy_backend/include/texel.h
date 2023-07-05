#ifndef TEXEL_H
#define TEXEL_H

#include "Adafruit_MCP23017.h"
#include "teensyOut.h"
#include "constants_texel.h"

class Texel {
public:
    Texel() : aerToTexel(DECODER_REQ, DECODER_ACK, AI_bus_pins, sizeof(AI_bus_pins) / sizeof(*AI_bus_pins)) {}
        // Assuming zero delay needed
        // Data, req and ack are active high.
    
    void setup();
    void reset(uint8_t parameter);
    void write(uint32_t data) { aerToTexel.dataWrite(data); }
    uint32_t readAObus() { return U8_AO00_15.readGPIOAB() | (U9_AO16_31.readGPIOAB() << 16); }
    void setBias(uint8_t biasNum, uint32_t value)
    {
        write(TEXEL_AE_REGWRITE | ((value << TEXEL_AE_REGDATA_SHIFT) & TEXEL_AE_REGDATA_MASK) | (biasNum & TEXEL_AE_REGADDR_MASK) );
    }
    
private:
    TeensyOut aerToTexel;
    Adafruit_MCP23017 U8_AO00_15;
    Adafruit_MCP23017 U9_AO16_31;
};

#endif
