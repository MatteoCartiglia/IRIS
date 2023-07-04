#include <Arduino.h>
#include <Wire.h>

#include "constants_texel.h"
#include "datatypes.h"
#include "texel.h"

static void pinModeAll23017Pins(Adafruit_MCP23017& mcp23017, uint8_t mode)
{
    for (uint8_t pinID = 0; pinID < 32; pinID++)
        mcp23017.pinMode(pinID, mode);
}

void Texel::setup()
{
    pinMode(RESET_PIN, OUTPUT);
    pinMode(REGRST_PIN, OUTPUT);
    pinMode(SYNRST_PIN, OUTPUT);

    pinMode(ENCODER_REQ, INPUT);
    pinMode(ENCODER_ACK, OUTPUT);
    Wire.setSCL(ENCODER_SCL);
    Wire.setSDA(ENCODER_SDA);
    U8_AO00_15.begin(U8_I2C_ADDRESS, &Wire);
    U9_AO16_31.begin(U9_I2C_ADDRESS, &Wire);
    pinModeAll23017Pins(U8_AO00_15, INPUT);
    pinModeAll23017Pins(U9_AO16_31, INPUT);
    
    // Setup of 'DECODER' pins, i.e. the pins for the Teensy->Texel AE bus is done in the TeensyOut constructor
    // which makes me [amw] nervous, since the time at which file-scope objects are constructed is ill-defined and maybe
    // the setup should only be done during the framework's call to setup(). - Issue #38
    
    pinMode(NC_PIN_29, OUTPUT);
}

void Texel::reset(uint8_t parameter)
{
    if (parameter & ResetTypeTexelReset)
        digitalWrite(RESET_PIN, LOW);
    if (parameter & ResetTypeTexelRegRst)
        digitalWrite(REGRST_PIN, LOW);
    if (parameter & ResetTypeTexelSynRst)
        digitalWrite(SYNRST_PIN, LOW);
    delayMicroseconds(RESET_DELAY_US);
    if (parameter & ResetTypeTexelSynRst)
        digitalWrite(SYNRST_PIN, HIGH);
    if (parameter & ResetTypeTexelRegRst)
        digitalWrite(REGRST_PIN, HIGH);
    if (parameter & ResetTypeTexelReset)
        digitalWrite(RESET_PIN, HIGH);
}
