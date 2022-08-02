#include "plane.h"
#include "planeimpl.h"

namespace plane {

// Define static constexpr member variables here without initializers for compatibility with C++ < 17
// and also for g++ 5.4.0 which claims to support C++17 but then still has these constants undefined unless they are
// defined thus. See Issue #26.
constexpr float Plane::TeensyReferenceVoltage_V;
constexpr float Plane::MaxSettableVoltage_V;
constexpr float Plane::MinAcqWformInterval12Bit_s;
constexpr float Plane::MinAcqWformInterval10Bit_s;
constexpr float Plane::MinAcqTransRespInterval12Bit_s;
constexpr float Plane::MinAcqTransRespInterval10Bit_s;

Plane::Plane() : pImpl(std::make_unique<PlaneImpl>())
{
}

Plane::~Plane() = default;

void Plane::setDebug(bool on)
{
    pImpl->setDebug(on);
}

bool Plane::getDebug() const
{
    return pImpl->getDebug();
}

void Plane::open(const std::string &deviceName) const
{
    pImpl->open(deviceName);
}

const char* Plane::getDeviceName() const
{
    return pImpl->getDeviceName();
}

TeensyStatus Plane::reset(ResetType type) const
{
    return pImpl->reset(type);
}

std::tuple<int, int, int> Plane::getFirmwareVersion() const
{
    return pImpl->getFirmwareVersion();
}

float Plane::setVoltage(DacChannel dac, float v_V) const
{
    return pImpl->setVoltage(dac, v_V);
}

float Plane::getSetVoltage(DacChannel dac) const
{
    return pImpl->getSetVoltage(dac);
}

void Plane::setVoltageWaveform(const std::vector<float>& values_V) const
{
    pImpl->setVoltageWaveform(values_V);
}

std::vector<float> Plane::getVoltageWaveform() const
{
    return pImpl->getVoltageWaveform();
}

float Plane::getMaxCurrent(AdcChannel adc) const
{
    return pImpl->getMaxCurrent(adc);
}

float Plane::setCurrent(AdcChannel adc, DacChannel dac, float i_A) const
{
    return pImpl->setCurrent(adc, dac, i_A);
}

float Plane::getSetCurrent(AdcChannel adc) const
{
    return pImpl->getSetCurrent(adc);
}

void Plane::setLedIntensity(std::uint8_t intensity) const
{
    pImpl->setLedIntensity(intensity);
}

std::uint8_t Plane::getLedIntensity() const
{
    return pImpl->getLedIntensity();
}

void Plane::setBitDepth(BitDepth bits) const
{
    pImpl->setBitDepth(bits);
}

BitDepth Plane::getBitDepth() const
{
    return pImpl->getBitDepth();
}

float Plane::readVoltage(AdcChannel adc) const
{
    return pImpl->readVoltage(adc);
}

float Plane::readCurrent(AdcChannel adc) const
{
    return pImpl->readCurrent(adc);
}

std::vector<float> Plane::acquireWaveform(DacChannel dac, AdcChannel adc, float interval_s) const
{
    return pImpl->acquireWaveform(dac, adc, interval_s);
}

std::vector<float> Plane::acquireTransientResponse(DacChannel dac, AdcChannel adc, float interval_s, float step_V) const
{
    return pImpl->acquireTransientResponse(dac, adc, interval_s, step_V);
}

void Plane::sendCoachEvents(const std::vector<coach::CoachInputEvent>& events) const
{
    pImpl->sendCoachEvents(events);
}

void Plane::requestEvents(float duration_s) const
{
    pImpl->requestEvents(duration_s);
}

std::array<unsigned int, NUM_C2FS> Plane::readC2fOutput(float duration_s) const
{
    std::array<unsigned int, NUM_C2FS> returnArray;
    pImpl->readC2fOutput(duration_s, returnArray);
    return returnArray;
}

std::vector<CoachOutputEvent> Plane::readEvents() const
{
    return pImpl->readEvents();
}

}
