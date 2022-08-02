#ifndef PLANE_PLANE_H_
#define PLANE_PLANE_H_

// PLANE = Platform for Learning About Neuromorphic Engineering

#include "coach.h"
#include "usb_packets.h"

#include <array>
#include <bitset>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace plane {

struct CoachOutputEvent {
    constexpr CoachOutputEvent(unsigned int t, unsigned int a) : timestamp(t), address(a) {}
    unsigned int timestamp;
    unsigned int address;
};

class PlaneImpl;

class Plane {
public:
    static constexpr float TeensyReferenceVoltage_V = 3.313; // See https://mm.ini.uzh.ch/ncs/pl/eqb98zxa7f8a3j7godfik9xjho
    static constexpr float MaxSettableVoltage_V = coach::Coach::Vdd;

    // Lower bounds on interval that can be specified for acquireWaveform() & acquireTransientResponse()
    // See Issue #95 - https://code.ini.uzh.ch/CoACH/CoACH_Teensy_interface/-/issues/95

    static constexpr float MinAcqWformInterval12Bit_s = 9.8e-5;
    static constexpr float MinAcqWformInterval10Bit_s = 8.2e-5;
    static constexpr float MinAcqTransRespInterval12Bit_s = 2.2e-5;
    static constexpr float MinAcqTransRespInterval10Bit_s = 6.0e-6;

    Plane();
    ~Plane();

    void setDebug(bool on);
    bool getDebug() const;

    void open(const std::string& deviceName) const;
    const char* getDeviceName() const;

    TeensyStatus reset(ResetType type) const;
    std::tuple<int, int, int> getFirmwareVersion() const;

    float setVoltage(DacChannel dac, float v_V) const;
    float getSetVoltage(DacChannel dac) const;

    void setVoltageWaveform(const std::vector<float>& values_V) const;
    std::vector<float> getVoltageWaveform() const;

    float getMaxCurrent(AdcChannel adc) const;
    float setCurrent(AdcChannel adc, DacChannel dac, float i_A) const;
    float getSetCurrent(AdcChannel adc) const;

    void setLedIntensity(std::uint8_t intensity) const;
    std::uint8_t getLedIntensity() const;

    void setBitDepth(BitDepth bits) const;
    BitDepth getBitDepth() const;

    float readVoltage(AdcChannel adc) const;
    float readCurrent(AdcChannel adc) const;

    std::vector<float> acquireWaveform(DacChannel dac, AdcChannel adc, float interval_s) const;
    std::vector<float> acquireTransientResponse(DacChannel dac, AdcChannel adc, float interval_s, float step_V) const;

    void sendCoachEvents(const std::vector<coach::CoachInputEvent>& events) const;

    void requestEvents(float duration_s) const;

    std::array<unsigned int, NUM_C2FS> readC2fOutput(float duration_s) const;

    std::vector<CoachOutputEvent> readEvents() const;

private:
    std::unique_ptr<PlaneImpl> pImpl;
};

}

#endif
