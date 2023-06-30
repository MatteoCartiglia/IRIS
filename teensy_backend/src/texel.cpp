#include <Arduino.h>

#include "constants_texel.h"
#include "datatypes.h"
#include "texel.h"

void Texel::setup()
{
    pinMode(RESET_PIN, OUTPUT);
    pinMode(REGRST_PIN, OUTPUT);
    pinMode(SYNRST_PIN, OUTPUT);

    pinMode(ENCODER_REQ, INPUT);
    pinMode(ENCODER_ACK, OUTPUT);
    pinMode(ENCODER_SCL, OUTPUT);
    pinMode(ENCODER_SDA, INPUT);

    pinMode(DECODER_REQ, OUTPUT);
    pinMode(DECODER_ACK, INPUT);
    pinMode(DECODER_OUTPUT_BIT_0_PIN, OUTPUT);
    pinMode(DECODER_OUTPUT_BIT_1_PIN, OUTPUT);
    pinMode(DECODER_OUTPUT_BIT_2_PIN, OUTPUT);
    pinMode(DECODER_OUTPUT_BIT_3_PIN, OUTPUT);
    pinMode(DECODER_OUTPUT_BIT_4_PIN, OUTPUT);
    pinMode(DECODER_OUTPUT_BIT_5_PIN, OUTPUT);
    pinMode(DECODER_OUTPUT_BIT_6_PIN, OUTPUT);
    pinMode(DECODER_OUTPUT_BIT_7_PIN, OUTPUT);
    pinMode(DECODER_OUTPUT_BIT_8_PIN, OUTPUT);
    pinMode(DECODER_OUTPUT_BIT_9_PIN, OUTPUT);
    pinMode(DECODER_OUTPUT_BIT_10_PIN, OUTPUT);
    pinMode(DECODER_OUTPUT_BIT_11_PIN, OUTPUT);
    pinMode(DECODER_OUTPUT_BIT_12_PIN, OUTPUT);
    pinMode(DECODER_OUTPUT_BIT_13_PIN, OUTPUT);
    pinMode(DECODER_OUTPUT_BIT_14_PIN, OUTPUT);
    pinMode(DECODER_OUTPUT_BIT_15_PIN, OUTPUT);
    pinMode(DECODER_OUTPUT_BIT_16_PIN, OUTPUT);
    pinMode(DECODER_OUTPUT_BIT_17_PIN, OUTPUT);
    pinMode(DECODER_OUTPUT_BIT_18_PIN, OUTPUT);
    pinMode(DECODER_OUTPUT_BIT_19_PIN, OUTPUT);
    pinMode(DECODER_OUTPUT_BIT_20_PIN, OUTPUT);
    pinMode(DECODER_OUTPUT_BIT_21_PIN, OUTPUT);
    pinMode(DECODER_OUTPUT_BIT_22_PIN, OUTPUT);
    pinMode(DECODER_OUTPUT_BIT_23_PIN, OUTPUT);
    pinMode(DECODER_OUTPUT_BIT_24_PIN, OUTPUT);
    pinMode(DECODER_OUTPUT_BIT_25_PIN, OUTPUT);
    pinMode(DECODER_OUTPUT_BIT_26_PIN, OUTPUT);
    pinMode(DECODER_OUTPUT_BIT_27_PIN, OUTPUT);
    pinMode(DECODER_OUTPUT_BIT_28_PIN, OUTPUT);
    pinMode(DECODER_OUTPUT_BIT_29_PIN, OUTPUT);
    pinMode(DECODER_OUTPUT_BIT_30_PIN, OUTPUT);
    pinMode(DECODER_OUTPUT_BIT_31_PIN, OUTPUT);

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
