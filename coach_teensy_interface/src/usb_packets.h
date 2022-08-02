#ifndef COACH_USB_PACKETS_H_
#define COACH_USB_PACKETS_H_

#include <cstddef>
#include <cstdint>

namespace plane {

/*
 * All packets start with a 16-bit packet header word, the lower 10 bits of
 * which specify the length of the packet in bytes, not including the packet
 * header word, and the upper 6 bits specify the packet type.
 * As the maximum packet length is 1024 bytes, the value of the packet length
 * given in the header word can range from 0 to 1022.
 * Packing the packet length and type into one 16-bit word is done to avoid
 * having the remainder of the packet starting at an odd address.
 */

static constexpr std::size_t MAX_PKT_LEN = 1024;
static constexpr unsigned int PKT_HDR_PKT_TYPE_SHIFT = 10;

static constexpr std::uint16_t PKT_HDR_PKT_LENGTH_MASK = MAX_PKT_LEN - 1;
static constexpr std::uint16_t PKT_HDR_PKT_TYPE_MASK = UINT16_MAX ^ PKT_HDR_PKT_LENGTH_MASK;

static constexpr std::size_t MAX_PKT_BODY_LEN = MAX_PKT_LEN - sizeof(std::uint16_t);

struct Pkt {
    std::uint16_t header; // Packet length encoded in header excludes size of header
    std::uint8_t body[MAX_PKT_BODY_LEN];
} __attribute__ ((packed));

// Note that the formatting of the enum classes below should be kept exactly as it is for the enum names to be
// correctly extracted for pybind by the pybind_enums.awk script.

enum class P2tPktType {
    P2tReset              = 1U << PKT_HDR_PKT_TYPE_SHIFT,
    P2tGetFirmwareVersion = 2U << PKT_HDR_PKT_TYPE_SHIFT,
    P2tSetDcVoltage       = 3U << PKT_HDR_PKT_TYPE_SHIFT,
    P2tSetVoltageWaveform = 4U << PKT_HDR_PKT_TYPE_SHIFT,
    P2tSetCurrent         = 5U << PKT_HDR_PKT_TYPE_SHIFT,
    P2tSetLedIntensity    = 6U << PKT_HDR_PKT_TYPE_SHIFT,
    P2tSetBitDepth        = 7U << PKT_HDR_PKT_TYPE_SHIFT,
    P2tRequestOneReading  = 8U << PKT_HDR_PKT_TYPE_SHIFT,
    P2tRequestReadings    = 9U << PKT_HDR_PKT_TYPE_SHIFT,
    P2tRequestTransResp   = 10U << PKT_HDR_PKT_TYPE_SHIFT,
    P2tRequestAerOutput   = 11U << PKT_HDR_PKT_TYPE_SHIFT,
    P2tRequestC2fOutput   = 12U << PKT_HDR_PKT_TYPE_SHIFT,
    /* P2tSetDacHighZ     = 13U << PKT_HDR_PKT_TYPE_SHIFT, - Reserved, not implemented */
    P2tSendCoachEvents    = 14U << PKT_HDR_PKT_TYPE_SHIFT,
};

static inline std::uint16_t makePktHeader(P2tPktType type, std::size_t pktSize)
{
    return std::uint16_t(type) | ((pktSize - sizeof(Pkt::header)) & PKT_HDR_PKT_LENGTH_MASK);
}

static inline std::size_t getPktSizeFromHeader(std::uint16_t header)
{
    return (header & PKT_HDR_PKT_LENGTH_MASK) + sizeof(header);
}

static inline std::size_t getPktSize(const Pkt& pkt)
{
    return getPktSizeFromHeader(pkt.header);
}

static inline std::uint16_t getPktTypeFromHeader(std::uint16_t header)
{
    return header & PKT_HDR_PKT_TYPE_MASK;
}

static inline std::uint16_t getPktType(const Pkt& pkt)
{
    return getPktTypeFromHeader(pkt.header);
}

/*
 * Values denoted as ranged values hold value in low-order RANGED_VALUE_VALUE_BITS bits,
 * and the next higher bit represents the range.
 */

static constexpr unsigned int RANGED_VALUE_VALUE_BITS = 12;
static constexpr std::uint16_t RANGED_VALUE_VALUE_MASK = (1U << RANGED_VALUE_VALUE_BITS) - 1;
static constexpr std::uint16_t RANGED_VALUE_RANGE_MASK = 1U << RANGED_VALUE_VALUE_BITS;

enum class CurrentRange {
    High = 0,
    Low = 1U << RANGED_VALUE_VALUE_BITS
};

static constexpr std::uint16_t MAX_RANGED_VALUE_VALUE = (1U << RANGED_VALUE_VALUE_BITS) - 1;
static constexpr std::uint16_t RANGED_VALUE_ERROR = UINT16_MAX;

static constexpr float WAVEFORM_DELAY_RESOLUTION_S = 1e-6; // 1us
static constexpr float MAX_WAVEFORM_DELAY_S = WAVEFORM_DELAY_RESOLUTION_S * UINT16_MAX;
static constexpr float TRANS_RESP_DELAY_RESOLUTION_S = 1e-6; // 1us
static constexpr float MAX_TRANS_RESP_DELAY_S = TRANS_RESP_DELAY_RESOLUTION_S * UINT16_MAX;

static constexpr float AER_OP_DURATION_RESOLUTION_S = 1e-3; // 1ms
static constexpr float MAX_AER_OP_DURATION_S = AER_OP_DURATION_RESOLUTION_S * UINT16_MAX;

static constexpr float C2F_OP_DURATION_RESOLUTION_S = 1e-3; // 1ms
static constexpr float MAX_C2F_OP_DURATION_S = C2F_OP_DURATION_RESOLUTION_S * UINT16_MAX;

struct PktP2tReset {
    std::uint16_t header;
    std::uint8_t reset_type;
} __attribute__ ((packed));

enum class ResetType {
    Soft = 0,
    Hard = 1
};

struct PktP2tGetFirmwareVersion {
    std::uint16_t header;
} __attribute__ ((packed));

struct PktP2tSetDcVoltage {
    std::uint16_t header;
    std::uint16_t value;
    std::uint8_t dac_channel;
} __attribute__ ((packed));

static constexpr unsigned int MAX_WAVEFORM_VALUES = MAX_PKT_BODY_LEN / sizeof(std::uint16_t);

struct PktP2tSetVoltageWaveform {
    std::uint16_t header;
    std::uint16_t values[MAX_WAVEFORM_VALUES];
} __attribute__ ((packed));

struct PktP2tSetCurrent {
    std::uint16_t header;
    std::uint16_t ranged_value;
    std::uint8_t dac_channel;
    std::uint8_t adc_channel;
} __attribute__ ((packed));

struct PktP2tSetLedIntensity {
    std::uint16_t header;
    std::uint8_t value;
} __attribute__ ((packed));

struct PktP2tSetBitDepth {
    std::uint16_t header;
    std::uint8_t depth;
} __attribute__ ((packed));

enum class BitDepth {
    BitDepth10 = 10,
    BitDepth12 = 12,
};

static constexpr BitDepth DEFAULT_BIT_DEPTH = BitDepth::BitDepth12;

struct PktP2tReqOneReading {
    std::uint16_t header;
    std::uint8_t adc_channel;
} __attribute__ ((packed));

struct PktP2tReqReadings {
    std::uint16_t header;
    std::uint16_t delay;
    std::uint8_t dac_channel;
    std::uint8_t adc_channel;
} __attribute__ ((packed));

struct PktP2tReqTransResp {
    std::uint16_t header;
    std::uint16_t step_value;
    std::uint16_t delay;
    std::uint8_t dac_channel;
    std::uint8_t adc_channel;
} __attribute__ ((packed));

struct PktP2tReqAerOutput {
    std::uint16_t header;
    std::uint16_t duration;
} __attribute__ ((packed));

struct PktP2tReqC2fOutput {
    std::uint16_t header;
    std::uint16_t duration;
} __attribute__ ((packed));

static constexpr unsigned int MAX_SEND_COACH_EVENTS = MAX_PKT_BODY_LEN / sizeof(std::uint32_t);

struct PktP2tSendCoachEvents {
    std::uint16_t header;
    std::uint32_t events[MAX_SEND_COACH_EVENTS];
} __attribute__ ((packed));

enum class T2pPktType {
    T2pStatus          = 1U << PKT_HDR_PKT_TYPE_SHIFT,
    T2pFirmwareVersion = 2U << PKT_HDR_PKT_TYPE_SHIFT,
    T2pSingleReading   = 3U << PKT_HDR_PKT_TYPE_SHIFT,
    T2pWaveform        = 4U << PKT_HDR_PKT_TYPE_SHIFT,
    T2pAerOutput       = 5U << PKT_HDR_PKT_TYPE_SHIFT,
    T2pAerOutputLast   = 6U << PKT_HDR_PKT_TYPE_SHIFT,
    T2pC2fOutput       = 7U << PKT_HDR_PKT_TYPE_SHIFT,
};

struct PktT2pStatus {
    std::uint16_t header;
    std::uint8_t status; // 0 = success, !0 = error code
} __attribute__ ((packed));

struct PktT2pFirmwareVersion {
    std::uint16_t header;
    std::uint8_t major;
    std::uint8_t minor;
    std::uint8_t patch;
} __attribute__ ((packed));

struct PktT2pReading {
    std::uint16_t header;
    std::uint16_t ranged_value;
} __attribute__ ((packed));

struct PktT2pWaveform {
    std::uint16_t header;
    std::uint16_t values[]; // Max 511 values
} __attribute__ ((packed));

typedef std::uint16_t aero_t;

static inline unsigned int aero_t_ts(aero_t a)
{
    return a & 0x1FFF;
}

static inline unsigned int aero_t_addr(aero_t a)
{
    return a >> 13;
}

struct PktT2pAerOutput {
    std::uint16_t header;
    aero_t events[]; // Max 511 events
} __attribute__ ((packed));

static constexpr unsigned int NUM_C2FS = 16;

struct PktT2pC2fOutput {
    std::uint16_t header;
    std::uint16_t values[NUM_C2FS];
} __attribute__ ((packed));

static constexpr unsigned int NUM_DACS = 32;
enum class DacChannel {
    AIN0  = 0,
    AIN1  = 1,
    AIN2  = 2,
    AIN3  = 3,
    AIN4  = 4,
    AIN5  = 5,
    AIN6  = 6,
    AIN7  = 7,
    AIN8  = 8,
    AIN9  = 9,
    AIN10 = 10,
    AIN11 = 11,
    AIN12 = 12,
    AIN13 = 13,
    AIN14 = 14,
    AIN15 = 15,
    GO23  = 16,
    GO22  = 17,
    GO5   = 18,
    GO3   = 19,
    GO2   = 20,
    GO4   = 21,
    GO1   = 22,
    GO0   = 23,
    DAC1  = 24,
    DAC2  = 25,
    DAC3  = 26,
    DAC4  = 27,
    PLUG  = 28,
    GO21  = 29,
    GO20  = 30,
    TP    = 31
};

static constexpr double DAC_LSB_V = 1.759531e-3; // Volts

static constexpr unsigned int NUM_ADCS = 32;
enum class AdcChannel {
    AOUT0  = 0, // Voltage
    AOUT1  = 1, // Voltage
    AOUT2  = 2, // Voltage
    AOUT3  = 3, // Voltage
    AOUT4  = 4, // Voltage
    AOUT5  = 5, // Voltage
    AOUT6  = 6, // Voltage
    AOUT7  = 7, // Voltage
    AOUT8  = 8, // Voltage
    AOUT9  = 9, // Voltage
    AOUT10 = 10, // Voltage
    AOUT11 = 11, // Voltage
    AOUT12 = 12, // Voltage
    AOUT13 = 13, // Voltage
    AOUT14 = 14, // Voltage
    AOUT15 = 15, // Voltage
    GO0_P  = 16, // Current
    GO0_N  = 17, // Current
    GO2_P  = 18, // Current
    GO2_N  = 19, // Current
    GO23   = 20, // Current
    GO22   = 21, // Current
    GO5    = 22, // Current
    GO3    = 23, // Current
    NCVDD1 = 24, // Current
    PLUG   = 25, // Current
    NCVDD3 = 26, // Current
    NCVDD2 = 27, // Current
    NCVDD5 = 28, // Current
    NCVDD4 = 29, // Current
    GO21_N = 30, // Current
    GO20_N = 31 // Current
};
static constexpr int FIRST_CURRENT_ADC = 16;

static inline bool IsSwitchableAdc(AdcChannel c)
{
    return c == AdcChannel::GO23 || c == AdcChannel::GO22 || c == AdcChannel::GO21_N || c == AdcChannel::GO20_N;
}

static constexpr double AdcLsb[NUM_ADCS] = {
    0.8056640625e-3, // Volts
    0.8056640625e-3, // Volts
    0.8056640625e-3, // Volts
    0.8056640625e-3, // Volts
    0.8056640625e-3, // Volts
    0.8056640625e-3, // Volts
    0.8056640625e-3, // Volts
    0.8056640625e-3, // Volts
    0.8056640625e-3, // Volts
    0.8056640625e-3, // Volts
    0.8056640625e-3, // Volts
    0.8056640625e-3, // Volts
    0.8056640625e-3, // Volts
    0.8056640625e-3, // Volts
    0.8056640625e-3, // Volts
    0.8056640625e-3, // Volts
    8.056640625e-9, // Amps
    8.056640625e-9, // Amps
    8.056640625e-9, // Amps
    8.056640625e-9, // Amps
    2.44140625e-8, // Amps GO23 LSB is 2.44140625e-10 if switch2 in low range position
    2.44140625e-8, // Amps GO22 LSB is 2.44140625e-10 if switch2 in low range position
    8.05664063e-10, // Amps
    8.05664063e-10, // Amps
    2.44140625e-8, // Amps
    2.44140625e-8, // Amps
    2.44140625e-8, // Amps
    2.44140625e-8, // Amps
    2.44140625e-8, // Amps
    2.44140625e-8, // Amps
    2.44140625e-8, // Amps GO21- LSB is 2.44140625e-10 if switch1 in low range position
    2.44140625e-8  // Amps GO20- LSB is 2.44140625e-10 if switch1 in low range position
};

// LOW_RANGE_FACTOR is the factor the high-range lsb value has to be multiplied by to give the low-range lsb value for
// the switchable channels GO23, GO22, GO21- and GO20-.
// It is determined by the ratio of the resistors on the board.

static constexpr float R_HIGH_RANGE_Ohm = 330.0;
static constexpr float R_LOW_RANGE_Ohm = 33.0e3;
static constexpr float LOW_RANGE_FACTOR = R_HIGH_RANGE_Ohm / R_LOW_RANGE_Ohm;

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

}

#endif
