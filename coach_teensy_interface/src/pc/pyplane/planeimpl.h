#ifndef PLANE_PLANEIMPL_H_
#define PLANE_PLANEIMPL_H_

#include "plane.h"
#include "teensy_serial.h"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

#define throwTeensyStatusError() _throwTeensyStatusError(__func__)

namespace plane {

// Timeouts for those commands for which a reply is expected back from the Teensy.
// TimeoutFactor is the multiple by which the typical measured time is multiplied to produce a timeout duration.

using namespace std::chrono_literals;
static constexpr unsigned int TimeoutFactor = 5;
static constexpr std::chrono::duration ResetTimeout = TimeoutFactor * 1s;
static constexpr std::chrono::duration GetFirmwareVersionTimeout = TimeoutFactor * 1s;
static constexpr std::chrono::duration SetVoltageTimeout = TimeoutFactor * 1s;
static constexpr std::chrono::duration SetVoltageWaveformTimeout = TimeoutFactor * 1s;
static constexpr std::chrono::duration SetCurrentTimeout = TimeoutFactor * 5s;
static constexpr std::chrono::duration SetLedIntensityTimeout = TimeoutFactor * 1s;
static constexpr std::chrono::duration SetBitDepthTimeout = TimeoutFactor * 1s;
static constexpr std::chrono::duration ReadVoltageTimeout = TimeoutFactor * 1s;
static constexpr std::chrono::duration ReadCurrentTimeout = TimeoutFactor * 1s;
static constexpr std::chrono::duration AcqWaveformTimeoutBase = TimeoutFactor * 1s; // Actual timeout is variable
static constexpr std::chrono::duration AcqTransRespTimeoutBase = TimeoutFactor * 1s; // Actual timeout is variable
static constexpr std::chrono::duration ReqC2fOutputTimeoutBase = TimeoutFactor * 1s; // Actual timeout is variable
static constexpr std::chrono::duration SendCoachEventsTimeout = TimeoutFactor * 1s;

// Delay after receiving the HardResetImminent message after which it can be hoped that the Teensy has successfully reset.
static constexpr std::chrono::duration HardResetDelay = 10s;

class PlaneImpl {
public:
    PlaneImpl() {};
    ~PlaneImpl();

    void setDebug(bool on) { debug = on; }
    bool getDebug() const { return debug; }

    void open(const std::string& nameToOpen);
    const char* getDeviceName() const { return deviceName.empty() ? NULL : deviceName.c_str(); }

    void _throwTeensyStatusError [[ noreturn ]] (const std::string& function) const;

    TeensyStatus reset(ResetType type);
    std::tuple<int, int, int> getFirmwareVersion() const;

    float setVoltage(DacChannel channel, float v_V);
    float getSetVoltage(DacChannel channel) const { return dacChannels_V[ static_cast<int>(channel) ]; }

    void setVoltageWaveform(const std::vector<float>& values_V);
    std::vector<float> getVoltageWaveform() const { return waveform_V; }

    float getMaxCurrent(AdcChannel adc) const;
    float setCurrent(AdcChannel adc, DacChannel dac, float i_A);
    float getSetCurrent(AdcChannel adc) const { return adcChannels[ static_cast<int>(adc) ]; }

    void setLedIntensity(std::uint8_t intensity);
    std::uint8_t getLedIntensity() const { return ledIntensity; }

    void setBitDepth(BitDepth bits);
    BitDepth getBitDepth() const { return bitDepth; }

    float readVoltage(AdcChannel adc) const;
    float readCurrent(AdcChannel adc) const;

    std::vector<float> acquireWaveform(DacChannel dac, AdcChannel adc, float interval_s) const;
    std::vector<float> acquireTransientResponse(DacChannel dac, AdcChannel adc, float interval_s, float step_V) const;

    void requestEvents(float duration_s);

    void readC2fOutput(float duration_s, std::array<unsigned int, NUM_C2FS>& c2fs) const;

    std::vector<CoachOutputEvent> readEvents();
    void sendCoachEvents(const std::vector<coach::CoachInputEvent>& events) const;

private:
    void transmit(Pkt *pkt) const; // To Teensy.
    void receive();                // From Teensy.
    std::size_t handleTeensyToPcPacket(const std::vector<std::uint8_t>& receiveBuffer);
    void handleTeensyStatus(std::uint8_t u8_status);
    void handleTeensyWaveform(const PktT2pWaveform *pkt);
    void handleTeensyAerOutput(const PktT2pAerOutput *pkt);
    void handleTeensyC2fOutput(const PktT2pC2fOutput *pkt);

    template<typename T>
    T sendPacketAndGetReply(std::unique_ptr<std::promise<T>>& promisePtr, Pkt *pkt,
                                                                        const std::chrono::seconds& timeout_s) const;

    float reqOneReading(AdcChannel adc, const std::chrono::seconds& timeout_s) const;
    std::vector<float> sendPacketAndGetWform(Pkt* pkt, const std::chrono::seconds& timeout_s, AdcChannel adc) const;

    bool debug{false};
    TeensySerial teensy;
    std::string deviceName;
    mutable std::unique_ptr<std::promise<TeensyStatus>> statusPromise;
    mutable std::unique_ptr<std::promise<PktT2pFirmwareVersion>> firmwareVersionPromise;
    mutable std::unique_ptr<std::promise<std::uint16_t>> rangedValuePromise;
    mutable std::unique_ptr<std::promise<std::vector<std::uint16_t>>> waveformPromise;
    mutable std::unique_ptr<std::promise<std::array<unsigned int, NUM_C2FS>>> c2fsPromise;
    std::array<float, NUM_DACS> dacChannels_V{};
    std::array<float, NUM_ADCS> adcChannels{};
    std::vector<float> waveform_V{};
    std::uint8_t ledIntensity{0};
    BitDepth bitDepth{DEFAULT_BIT_DEPTH};
    std::atomic_uint8_t lastTeensyStatus;
    std::atomic_bool runReceiveThread{false};

    mutable std::mutex receiveDataMutex; // Covers receiveEvents and receiveQueue
    bool receiveEvents{false};
    std::vector<CoachOutputEvent> receiveQueue;

    std::thread receiveThread;
};

}

#endif
