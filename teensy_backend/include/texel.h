#ifndef TEXEL_H
#define TEXEL_H

#include "teensyOut.h"
#include "constants_texel.h"

class Texel {
public:
    Texel() : aerToTexel(DECODER_REQ, DECODER_ACK, AI_bus_pins, sizeof(AI_bus_pins) / sizeof(*AI_bus_pins)) {}
        // Assuming zero delay needed
        // Data, req and ack are active high.
    
    void setup();
    void reset(uint8_t parameter);
private:
    TeensyOut aerToTexel;
};

#endif
