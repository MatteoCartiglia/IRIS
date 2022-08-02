#ifndef COACH_COACH_H
#define COACH_COACH_H

// The above include guard has COACH in it twice because the first part is for the namespace.

// Copyright (C) University of Zurich, 2020

// This file relates to Greg Burman's classchip design of 2018 which is called CoACH and is documented in the git
// repository at https://code.ini.uzh.ch/CoACH/CoACH-docs

#include <cstdint>

namespace coach {

class CoachInputEvent {
public:
    CoachInputEvent(std::uint32_t e) : event(e) {}
    explicit operator uint32_t() const { return event; }
private:
    std::uint32_t event;
};

struct Coach {

// Note that the formatting of the enum classes below should be kept exactly as it is for the enum names to be
// correctly extracted for pybind by the pybind_enums.awk script.

static constexpr float Vdd = 1.8;

// The list of biases in enum class BiasAddress reflects the list extracted from the chip schematics and given in
// biases_CoACH.ods as posted to the Mattermost Classchip channel on 12 Feb 2020 at
// https://mm.ini.uzh.ch/ncs/pl/ztmqo8dnu7n1jxf4qyc4ipipdc
// This is, at the time of writing (2 April 2020), not the same as that given in chip_architecture.tex
// Additionally, I [amw] was told there is a bias at address 0 which is just called 'BUFFER'.

enum class BiasAddress {
    BUFFER = 0,
    AHN_VPW_N = 1,
    ACN_VADPPTAU_N = 2,
    ACN_VADPWEIGHT_N = 3,
    ACN_VADPGAIN_N = 4,
    ACN_VADPTAU_P = 5,
    ACN_VADPCASC_N = 6 ,
    ACN_VREFR_N = 7,
    ACN_VLEAK_N = 8,
    ACN_VGAIN_N = 9,
    ACN_VDC_P = 10,
    LDS_VTAU_P = 11,
    LDS_VWEIGHT_P = 12,
    ATN_VADPPTAU_N = 13,
    ATN_VADPWEIGHT_N = 14,
    ATN_VADPGAIN_N = 15,
    ATN_VADPTAU_P = 16,
    ATN_VADPCASC_N = 17,
    ATN_VREFR_N = 18,
    ATN_VCC_N = 19,
    ATN_VSPKTHR_P = 20,
    ATN_VLEAK_N = 21,
    ATN_VGAIN_N = 22,
    ATN_VDC_P = 23,
    DPI_VTAU_P = 24,
    DPI_VWEIGHT_N = 25,
    DPI_VTHR_N = 26,
    PEX_VTAU_N = 27,
    ASN_VADPPTAU_N = 28,
    ASN_VADPWEIGHT_N = 29,
    ASN_VADPGAIN_N = 30,
    ASN_VADPTAU_P = 31,
    ASN_VADPCASC_N = 32,
    ASN_VCC_N = 33,
    ASN_VSPKTHR_P = 34,
    ASN_VLEAK_N = 35,
    ASN_VGAIN_N = 36,
    ASN_VDC_P = 37,
    DDI_VWEIGHT_N = 38,
    DDI_VTHR_N = 39,
    DDI_VTAU_P = 40,
    HHN_VBUF_N = 41,
    HHN_VAHPSAT_N = 42,
    HHN_VCAREST2_N = 43,
    HHN_VCABUF_N = 44,
    HHN_VCAREST_N = 45,
    HHN_VCAIN_P = 46,
    HHN_VKDSAT_N = 47,
    HHN_VPUWIDTH_N = 48,
    HHN_VKDTAU_N = 49,
    HHN_VTHRES_N = 50,
    HHN_VNASAT_N = 51,
    HHN_VDC_P = 52,
    HHN_VELEAK_N = 53,
    HHN_VNATAU_N = 54,
    HHN_VGLEAK_N = 55,
    HHN_VPADBIAS_N = 56,
    HHN_VPUTHRES_N = 57,
    SFP_VB_N = 95,
    DVS_REFR_P = 96,
    DVS_OFF_N = 97,
    DVS_ON_N = 98,
    DVS_DIFF_N = 99,
    DVS_SF_P = 100,
    DVS_CAS_N = 101,
    DVS_PR_P = 102,
    RR_BIAS_P = 103,
    C2F_HYS_P = 104,
    C2F_REF_L = 105,
    C2F_REF_H = 106,
    C2F_BIAS_P = 107,
    C2F_PWLK_P = 108,
    NTA_VB_N = 109,
    CSR_VT_N = 110,
    BAB_VB_N = 111,
    FOD_VB_N = 112,
    FOI_VB_N = 113,
    NDP_VB_N = 114,
    NSF_VB_N = 115,
    SOS_VB2_N = 116,
    PDP_VB_P = 117,
    PSF_VB_P = 118,
    PTA_VB_P = 119,
    SOS_VB1_N = 120,
    SRE_VB1_N = 121,
    SRE_VB2_N = 122,
    WRT_VB_N = 123,
    WTA_VB_N = 124,
    WTA_VEX_N = 125,
    WTA_VINH_N = 126,
    WTA_VGAIN_P = 127
};

// Different input event types (IETs) are distiguished by their upper bits.

// clang-format off
static constexpr unsigned int IetBiasGen = 0x00000;
static constexpr unsigned int IetAerc    = 0x80000;
static constexpr unsigned int IetPulse   = 0xC0000;

static constexpr unsigned int BiasGenEventAddressMask   = 0x7F000;
static constexpr unsigned int BiasGenEventMasterMask    = 0x00E00;
static constexpr unsigned int BiasGenEventFineMask      = 0x001FE;
static constexpr unsigned int BiasGenEventNPMask        = 0x00001;

static constexpr unsigned int BiasGenEventAddressShift = 12;
static constexpr unsigned int BiasGenEventFineShift    = 1;

enum class BiasGenMasterCurrent {
    I60pA =  0x00000,
    I460pA = 0x00200,
    I3_8nA = 0x00400,
    I30nA =  0x00600,
    I240nA = 0x00800
};

enum class BiasType {
    P = 0,
    N = 1
};

static constexpr unsigned int AercCurrentOpMuxSelMask   = 0x0E000;
static constexpr unsigned int AercVoltageOpMuxSelMask   = 0x01800;
static constexpr unsigned int AercVoltageIpDemuxSelMask = 0x00600;
static constexpr unsigned int AercMiscControlMask       = 0x001FF;

static constexpr unsigned int Aerc_SRE_VEN_VSI   = 1 << 0;
static constexpr unsigned int Aerc_WTA_VHEN_SI   = 1 << 1;
static constexpr unsigned int Aerc_DSY_S0_ASI    = 1 << 2;
static constexpr unsigned int Aerc_DSY_S1_ASI    = 1 << 3;
static constexpr unsigned int Aerc_ACN_ADPEN_ASI = 1 << 4;
static constexpr unsigned int Aerc_ACN_DCEN_ASBI = 1 << 5;
static constexpr unsigned int Aerc_ATN_DCEN_ASBI = 1 << 6;
static constexpr unsigned int Aerc_ATN_ADPEN_ASI = 1 << 7;
static constexpr unsigned int Aerc_ASN_DCEN_ASBI = 1 << 8;

enum class SynapseSelect {
    NoneSelected = 0,
    LDS  = Aerc_DSY_S1_ASI,
    DPI  = Aerc_DSY_S0_ASI,
    DDI  = Aerc_DSY_S0_ASI | Aerc_DSY_S1_ASI
};

// enum class CurrentOutputSelect assumes the select bits 0, 1 & 2 for the Current Output Mux Select Lines are
// connected to bits 13, 14 & 15 respectively in the AERC register, and not in the reverse order,
// and that 000b means Select Line 0 is active, 001b means Select Line 1 is active etc.
// Note that the bits are shifted as required for the AERC event.

enum class CurrentOutputSelect {
    SelectLine0 = 0x00000,
    SelectLine1 = 0x02000,
    SelectLine2 = 0x04000,
    SelectLine3 = 0x06000,
    SelectLine4 = 0x08000,
    SelectLine5 = 0x0A000,
    SelectLine6 = 0x0C000
};

static constexpr unsigned int AercCurrentOpMuxSelShift = 13;

// enum class VoltageOutputSelect assumes the Select 0 & Select 1 bits for the Voltage Output Mux Select Lines are
// connected to bits 11 & 12 respectively in the AERC register, and not in the reverse order.
// Note that the bits are shifted as required for the AERC event.

enum class VoltageOutputSelect {
    NoneSelected = 0x00000,
    SelectLine0 = 0x01000,
    SelectLine1 = 0x00800,
    SelectLine2 = 0x01800
};

// enum class VoltageInputSelect assumes the Select 0 & Select 1 bits for the Voltage Input Demux Select Lines are
// connected to bits 10 & 9 respectively in the AERC register, i.e in reverse order!
// Note that the bits are shifted as required for the AERC event.

enum class VoltageInputSelect {
    NoneSelected = 0x00000,
    SelectLine0 = 0x00200,
    SelectLine1 = 0x00400,
    SelectLine2 = 0x00600
};
// clang-format on

static CoachInputEvent generateBiasGenEvent(BiasAddress a, BiasType t, BiasGenMasterCurrent m, std::uint8_t fine)
{
    return IetBiasGen | static_cast<unsigned int>(a) << BiasGenEventAddressShift | static_cast<unsigned int>(m) |
           fine << BiasGenEventFineShift | static_cast<unsigned int>(t);
}

static CoachInputEvent generateAercEvent(CurrentOutputSelect cos, VoltageOutputSelect vos, VoltageInputSelect vis,
                                      SynapseSelect ss, unsigned int misc)
{
    return IetAerc | static_cast<unsigned int>(cos) | static_cast<unsigned int>(vos) | static_cast<unsigned int>(vis) |
           static_cast<unsigned int>(ss) | (misc & AercMiscControlMask & ~(Aerc_DSY_S0_ASI | Aerc_DSY_S1_ASI));
}

static CoachInputEvent generatePulseEvent()
{
    return IetPulse;
}

};

}

#endif
