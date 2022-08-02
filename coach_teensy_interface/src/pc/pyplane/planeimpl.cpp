#include "planeimpl.h"

#include <algorithm>
#include <arpa/inet.h>
#include <climits>
#include <cmath>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <system_error>

namespace plane {

static void validateVoltage(float v_V)
{
    if (v_V < 0 || v_V > Plane::MaxSettableVoltage_V) {
        throw std::invalid_argument("attempting to set a voltage that is either negative or greater than the maximum settable voltage.");
    }
}

static float rangedValueToFloat(std::uint16_t rv, AdcChannel adc)
{
    float f = (rv & RANGED_VALUE_VALUE_MASK) * AdcLsb[int(adc)];
    if (IsSwitchableAdc(adc) && (rv & RANGED_VALUE_RANGE_MASK) == std::uint16_t(CurrentRange::Low)) {
        f *= LOW_RANGE_FACTOR;
    }
    return f;
}

static void dumpTransmitBuffer(const std::vector<std::uint8_t> buffer)
{
    std::cerr << std::hex << std::setfill('0') << "Tx ";
    for (auto byte : buffer) {
        std::cerr << std::setw(2) << int(byte);
    }
    std::cerr << std::endl << std::dec;
}

template<typename T>
static void setPromiseValue(std::unique_ptr<std::promise<T>>& promisePtr, T value)
{
    if (promisePtr) {
        promisePtr->set_value(value);
    }
}

PlaneImpl::~PlaneImpl()
{
    bool receiveThreadWasRunning = runReceiveThread;
    runReceiveThread = false;
    if (receiveThread.joinable()) {
        if (receiveThreadWasRunning) {
            // receiveThread.join() will block with receiveThread blocked in ::read().
            // To avoid this, ask the Teensy to send us a packet before calling join().
            getFirmwareVersion();
        }
        receiveThread.join();
    }
}

void PlaneImpl::open(const std::string& nameToOpen)
{
    // Fix #34. Treat an attempt to open the same device a second time as a no-op, but throw a runtime error if an
    // attempt is made to open a different device.
    if (!deviceName.empty()) {
        if (nameToOpen == deviceName) {
            return;
        }
        else {
            throw std::runtime_error("another device was already opened for this object.");
        }
    }

    teensy.open(nameToOpen); // This will throw a std::system_error if it can't open the named device
    deviceName = nameToOpen;
    runReceiveThread = true;
    receiveThread = std::thread(&PlaneImpl::receive, this);
}

void PlaneImpl::receive()
{
    std::vector<std::uint8_t> receiveBuffer;
    while (runReceiveThread) {
        teensy.read(receiveBuffer);
        while (!receiveBuffer.empty()) {
            auto packetSize = handleTeensyToPcPacket(receiveBuffer);
            receiveBuffer.erase(receiveBuffer.begin(), receiveBuffer.begin() + packetSize);
        }
    }
}

std::size_t PlaneImpl::handleTeensyToPcPacket(const std::vector<std::uint8_t>& receiveBuffer)
{
    const Pkt *pkt = reinterpret_cast<const Pkt*>(receiveBuffer.data());
    std::uint16_t header = ntohs(pkt->header);
    std::uint16_t packetType = getPktTypeFromHeader(header);
    std::size_t packetSize = getPktSizeFromHeader(header);
    if (debug) {
        std::cerr << "Rx " << std::hex << std::setfill('0') << std::setw(4) << header << std::dec << std::endl;
    }
    switch (packetType) {
    case std::uint16_t(T2pPktType::T2pStatus):
        handleTeensyStatus(reinterpret_cast<const PktT2pStatus*>(pkt)->status);
        break;
    case std::uint16_t(T2pPktType::T2pFirmwareVersion):
        setPromiseValue(firmwareVersionPromise, *reinterpret_cast<const PktT2pFirmwareVersion*>(pkt));
        break;
    case std::uint16_t(T2pPktType::T2pSingleReading):
        setPromiseValue(rangedValuePromise, ntohs(reinterpret_cast<const PktT2pReading*>(pkt)->ranged_value));
        break;
    case std::uint16_t(T2pPktType::T2pWaveform):
        handleTeensyWaveform(reinterpret_cast<const PktT2pWaveform*>(pkt));
        break;
    case std::uint16_t(T2pPktType::T2pAerOutput):
    case std::uint16_t(T2pPktType::T2pAerOutputLast):
        handleTeensyAerOutput(reinterpret_cast<const PktT2pAerOutput*>(pkt));
        break;
    case std::uint16_t(T2pPktType::T2pC2fOutput):
        handleTeensyC2fOutput(reinterpret_cast<const PktT2pC2fOutput*>(pkt));
        break;
    default:
        throw std::system_error(EIO, std::generic_category(), "received unknown packet type");
        break;
    }
    return packetSize;
}

void PlaneImpl::handleTeensyStatus(std::uint8_t u8_status)
{
    lastTeensyStatus = u8_status;
    TeensyStatus status = TeensyStatus(u8_status);

    // If we're currently expecting a T2pSingleReading, and we've got a T2pStatus status instead, signal an error
    // condition has occurred by putting RANGED_VALUE_ERROR in the promise.
    if (rangedValuePromise) {
        setPromiseValue(rangedValuePromise, RANGED_VALUE_ERROR);
        return;
    }

    // If we're currently expecting a T2pWaveform, and we've got a T2pStatus status instead, signal an error condition
    // has occurred by putting an empty vector in the promise.
    if (waveformPromise) {
        setPromiseValue(waveformPromise, std::vector<uint16_t>());
        return;
    }

    switch (status) {
    case TeensyStatus::Success:
    case TeensyStatus::HardResetFailed:
    case TeensyStatus::IncorrectCurrentSwitchRange:
    case TeensyStatus::CurrentCannotBeSet:
    case TeensyStatus::CurrentOutsideSearchRange:
    case TeensyStatus::AerHandshakeFailed:
    case TeensyStatus::HardResetNotSupported:
        setPromiseValue(statusPromise, status);
        break;
    case TeensyStatus::HardResetImminent:
        // This means that the receive thread should stop and the connection to the Teensy should be closed.
        // Setting runReceiveThread = false will cause the loop in PlaneImpl::receive() to exit, and thus the thread to
        // finish executing.
        runReceiveThread = false;
        setPromiseValue(statusPromise, status);
        break;
    case TeensyStatus::UnknownCommand:
        throw std::system_error(EIO, std::generic_category(), "Teensy received unknown command");
        break;
    default:
        throw std::system_error(EIO, std::generic_category(), "received unknown status " + std::to_string(u8_status));
    }
}

void PlaneImpl::handleTeensyWaveform(const PktT2pWaveform *pkt)
{
    unsigned int nWords = getPktSizeFromHeader(ntohs(pkt->header)) / sizeof(std::uint16_t) - 1; // - 1 to not include header
    std::vector<std::uint16_t> wform(nWords);
    for (unsigned int i = 0; i < nWords; i++) {
        wform[i] = ntohs(pkt->values[i]);
    }
    setPromiseValue(waveformPromise, wform);
}

void PlaneImpl::handleTeensyAerOutput(const PktT2pAerOutput *pkt)
{
    std::lock_guard<std::mutex> lock(receiveDataMutex);
    if (receiveEvents) {
        std::uint16_t header = ntohs(pkt->header);
        unsigned int nEvents = getPktSizeFromHeader(header) / sizeof(std::uint16_t) - 1; // - 1 to not include header
        for (unsigned int i = 0; i < nEvents; i++) {
            aero_t event = ntohs(pkt->events[i]);
            receiveQueue.push_back(CoachOutputEvent(aero_t_ts(event), aero_t_addr(event)));
        }
        if (getPktTypeFromHeader(header) == std::uint16_t(T2pPktType::T2pAerOutputLast)) {
            receiveEvents = false;
        }
    }
}

void PlaneImpl::handleTeensyC2fOutput(const PktT2pC2fOutput *pkt)
{
    if (getPktSizeFromHeader(ntohs(pkt->header)) / sizeof(std::uint16_t) != NUM_C2FS + 1) {
        throw std::system_error(EIO, std::generic_category(), "received incorrect number of C2F values");
    }
    std::array<unsigned int, NUM_C2FS> c2fs;
    for (unsigned int i = 0; i < NUM_C2FS; i++) {
        c2fs[i] = ntohs(pkt->values[i]);
    }
    setPromiseValue(c2fsPromise, c2fs);
}

// pkt.header must be still in host order when transmit is called.
// All other elements of the packet must already be in network order.

void PlaneImpl::transmit(Pkt *pkt) const
{
    std::size_t pktSize = getPktSize(*pkt);
    pkt->header = htons(pkt->header);
    std::vector<std::uint8_t> outputBuffer(pktSize);
    const std::uint8_t *p = reinterpret_cast<const std::uint8_t *>(pkt);
    for (unsigned int i = 0; i < pktSize; i++) {
        outputBuffer[i] = *p++;
    }
    if (debug) {
        dumpTransmitBuffer(outputBuffer);
    }
    teensy.write(outputBuffer);
    pkt->header = ntohs(pkt->header);
}

template<typename T>
T PlaneImpl::sendPacketAndGetReply(std::unique_ptr<std::promise<T>>& promisePtr, Pkt *pkt,
                                                                            const std::chrono::seconds& timeout_s) const
{
    promisePtr = std::make_unique<std::promise<T>>();
    std::future<T> f = promisePtr->get_future();
    transmit(pkt);

    // When the receive thread gets the reply from the Teensy, it sets the value of the promise.

    auto futureStatus = f.wait_for(timeout_s);
    if (futureStatus != std::future_status::ready) {
        promisePtr.reset(nullptr);
        throw(std::runtime_error((futureStatus == std::future_status::timeout) ? "timed out."
                                                                               : "unexpected future status."));
    }
    T reply = f.get();
    promisePtr.reset(nullptr);
    return reply;
}

void PlaneImpl::_throwTeensyStatusError [[ noreturn ]] (const std::string& function) const
{
    throw(std::runtime_error(function + "() received unexpected Teensy error status " + std::to_string(lastTeensyStatus)));
}

TeensyStatus PlaneImpl::reset(ResetType type)
{
    PktP2tReset pkt;
    pkt.header = makePktHeader(P2tPktType::P2tReset, sizeof(pkt));
    pkt.reset_type = std::uint8_t(type);

    TeensyStatus status = sendPacketAndGetReply(statusPromise, reinterpret_cast<Pkt*>(&pkt), ResetTimeout);
    if (type == ResetType::Hard && (status == TeensyStatus::HardResetFailed || status == TeensyStatus::HardResetNotSupported)) {
        return status;
    }
    if (type == ResetType::Hard && status == TeensyStatus::HardResetImminent) {
        // Hard resetting the Teensy means that the USB device will go away and come back again, but unless we close the
        // connection while this is happening, the device will come back after reset under a different name, e.g.
        // ttyACM1 instead of ttyACM0, but with multiple devices, there is no way of knowing the new device name.
        // Closing the device in good time before the Teensy resets itself increases the chance of being able to re-open
        // it after reset under the same name. This is obviously racy, and can also easily still re-open the wrong
        // device if multiple Teensys attached to the same host are reset more or less simultaneously, but for a single
        // Teensy per host situation it should (mostly) work.

        // The reception of TeensyStatus::HardResetImminent will already have caused the receiveThread to stop running,
        // so now the thread can be joined and the connection can be closed.
        if (receiveThread.joinable()) {
            receiveThread.join();
        }
        teensy.close();

        // Now we need to wait long enough for the Teensy to have actually reset.
        // This is obviously racy, but what else can we do?
        std::this_thread::sleep_for(HardResetDelay);

        // Now we can try re-opening the device (which will throw a std::system_error if it fails)
        // and re-starting the receive thread.
        teensy.open(deviceName);
        runReceiveThread = true;
        receiveThread = std::thread(&PlaneImpl::receive, this);
        status = TeensyStatus::Success;
    }
    if (status != TeensyStatus::Success) {
        throwTeensyStatusError();
    }

    dacChannels_V.fill(0.0);
    if (type == ResetType::Hard) {
        waveform_V.clear();
    }
    adcChannels.fill(0.0);
    ledIntensity = 0;
    bitDepth = DEFAULT_BIT_DEPTH;
    return status;
}

std::tuple<int, int, int> PlaneImpl::getFirmwareVersion() const
{
    firmwareVersionPromise = std::make_unique<std::promise<PktT2pFirmwareVersion>>();
    std::future<PktT2pFirmwareVersion> f = firmwareVersionPromise->get_future();

    PktP2tGetFirmwareVersion pkt;
    pkt.header = makePktHeader(P2tPktType::P2tGetFirmwareVersion, sizeof(pkt));
    transmit(reinterpret_cast<Pkt*>(&pkt));

    auto futureStatus = f.wait_for(GetFirmwareVersionTimeout);
    if (futureStatus != std::future_status::ready && futureStatus != std::future_status::timeout) {
        firmwareVersionPromise.reset(nullptr);
        throw(std::runtime_error("unexpected future status."));
    }
    PktT2pFirmwareVersion versionPkt;
    if (futureStatus != std::future_status::timeout) {
        versionPkt = f.get();
    }
    else {
        versionPkt.major = versionPkt.minor = versionPkt.patch = -1;
    }
    firmwareVersionPromise.reset(nullptr);
    return std::make_tuple(versionPkt.major, versionPkt.minor, versionPkt.patch);
}

float PlaneImpl::setVoltage(DacChannel dac, float v_V)
{
    validateVoltage(v_V);
    std::uint16_t valueBits = std::uint16_t(v_V / DAC_LSB_V);
    v_V = valueBits * DAC_LSB_V;

    PktP2tSetDcVoltage pkt;
    pkt.header = makePktHeader(P2tPktType::P2tSetDcVoltage, sizeof(pkt));
    pkt.value = htons(valueBits);
    pkt.dac_channel = std::uint8_t(dac);

    if (sendPacketAndGetReply(statusPromise, reinterpret_cast<Pkt*>(&pkt), SetVoltageTimeout) != TeensyStatus::Success) {
        throwTeensyStatusError();
    }

    dacChannels_V[int(dac)] = v_V;
    return v_V;
}

void PlaneImpl::setVoltageWaveform(const std::vector<float>& values_V)
{
    if (values_V.size() > MAX_WAVEFORM_VALUES) {
        throw std::length_error("attempting to set a waveform with more than the maximum possible number of points.");
    }
    for (float v : values_V) {
        validateVoltage(v);
    }

    unsigned int i;
    PktP2tSetVoltageWaveform pkt;
    pkt.header = makePktHeader(P2tPktType::P2tSetVoltageWaveform,
                               sizeof(pkt.header) + values_V.size() * sizeof(std::uint16_t));
    for (i = 0; i < values_V.size(); i++) {
        pkt.values[i] = htons(std::uint16_t(values_V[i] / DAC_LSB_V));
    }

    if (sendPacketAndGetReply(statusPromise, reinterpret_cast<Pkt*>(&pkt), SetVoltageWaveformTimeout) != TeensyStatus::Success) {
        throwTeensyStatusError();
    }

    waveform_V.resize(values_V.size());
    for (i = 0; i < values_V.size(); i++) {
        waveform_V[i] = ntohs(pkt.values[i]) * DAC_LSB_V;
    }
}

float PlaneImpl::getMaxCurrent(AdcChannel adc) const
{
    if (int(adc) < FIRST_CURRENT_ADC) {
        throw std::invalid_argument("attempting to get the maximum current for a voltage channel.");
    }

    return AdcLsb[int(adc)] * MAX_RANGED_VALUE_VALUE;
}

float PlaneImpl::setCurrent(AdcChannel adc, DacChannel dac, float i_A)
{
    if (int(adc) < FIRST_CURRENT_ADC) {
        throw std::invalid_argument("attempting to set a current on a voltage ADC channel.");
    }

    float lsb = AdcLsb[int(adc)];

    if (i_A > lsb * MAX_RANGED_VALUE_VALUE) {
        throw std::invalid_argument("attempting to set a current that is outside the range of settable currents for this ADC.");
    }

    CurrentRange range = CurrentRange::High;
    if (IsSwitchableAdc(adc) && i_A < lsb * MAX_RANGED_VALUE_VALUE * LOW_RANGE_FACTOR) {
        range = CurrentRange::Low;
        lsb *= LOW_RANGE_FACTOR;
    }
    std::uint16_t rangedValueBits = std::uint16_t(i_A / lsb) & RANGED_VALUE_VALUE_MASK;
    rangedValueBits |= std::uint16_t(range);

    PktP2tSetCurrent pkt;
    pkt.header = makePktHeader(P2tPktType::P2tSetCurrent, sizeof(pkt));
    pkt.ranged_value = htons(rangedValueBits);
    pkt.dac_channel = std::uint8_t(dac);
    pkt.adc_channel = std::uint8_t(adc);

    std::uint16_t rangedValue = sendPacketAndGetReply(rangedValuePromise, reinterpret_cast<Pkt*>(&pkt), SetCurrentTimeout);

    if (rangedValue == RANGED_VALUE_ERROR) {
        switch (lastTeensyStatus) {
        case uint8_t(TeensyStatus::IncorrectCurrentSwitchRange):
            throw std::runtime_error("error occurred trying to set current. Incorrect current range set on switch.");
            break;
        case uint8_t(TeensyStatus::CurrentCannotBeSet):
            throw std::runtime_error("error occurred trying to set current. Target current outside range settable with allowable DAC values.");
            break;
        case uint8_t(TeensyStatus::CurrentOutsideSearchRange):
            throw std::runtime_error("error occurred trying to set current. Target current outside of search range.");
            break;
        default:
            throwTeensyStatusError();
            break;
        }
    }

    adcChannels[int(adc)] = rangedValueToFloat(rangedValue, adc);
    return adcChannels[int(adc)];
}

void PlaneImpl::setLedIntensity(std::uint8_t intensity)
{
    PktP2tSetLedIntensity pkt;
    pkt.header = makePktHeader(P2tPktType::P2tSetLedIntensity, sizeof(pkt));
    pkt.value = intensity;

    if (sendPacketAndGetReply(statusPromise, reinterpret_cast<Pkt*>(&pkt), SetLedIntensityTimeout) != TeensyStatus::Success) {
        throwTeensyStatusError();
    }

    ledIntensity = intensity;
}

void PlaneImpl::setBitDepth(BitDepth bits)
{
    PktP2tSetBitDepth pkt;
    pkt.header = makePktHeader(P2tPktType::P2tSetBitDepth, sizeof(pkt));
    pkt.depth = std::uint8_t(bits);

    if (sendPacketAndGetReply(statusPromise, reinterpret_cast<Pkt*>(&pkt), SetBitDepthTimeout) != TeensyStatus::Success) {
        throwTeensyStatusError();
    }

    bitDepth = bits;
}

float PlaneImpl::reqOneReading(AdcChannel adc, const std::chrono::seconds& timeout_s) const
{
    PktP2tReqOneReading pkt;
    pkt.header = makePktHeader(P2tPktType::P2tRequestOneReading, sizeof(pkt));
    pkt.adc_channel = std::uint8_t(adc);

    std::uint16_t rangedValue = sendPacketAndGetReply(rangedValuePromise, reinterpret_cast<Pkt*>(&pkt), timeout_s);
    if (rangedValue == RANGED_VALUE_ERROR) {
        throwTeensyStatusError();
    }
    return rangedValueToFloat(rangedValue, adc);
}

float PlaneImpl::readVoltage(AdcChannel adc) const
{
    if (int(adc) >= FIRST_CURRENT_ADC) {
        throw std::invalid_argument("attempting to read a voltage on a current ADC channel.");
    }
    return reqOneReading(adc, ReadVoltageTimeout);
}

float PlaneImpl::readCurrent(AdcChannel adc) const
{
    if (int(adc) < FIRST_CURRENT_ADC) {
        throw std::invalid_argument("attempting to read a current on a voltage ADC channel.");
    }
    return reqOneReading(adc, ReadCurrentTimeout);
}

std::vector<float> PlaneImpl::sendPacketAndGetWform(Pkt* pkt, const std::chrono::seconds& timeout_s, AdcChannel adc) const
{
    std::vector<std::uint16_t> wform_u16 = sendPacketAndGetReply(waveformPromise, pkt, timeout_s);

    unsigned int nWords = wform_u16.size();
    if (nWords == 0) {
        throwTeensyStatusError();
    }

    std::vector<float> wform_f(nWords);
    for (unsigned int i = 0; i < nWords; i++) {
        wform_f[i] = rangedValueToFloat(wform_u16[i], adc);
    }
    return wform_f;
}

std::vector<float> PlaneImpl::acquireWaveform(DacChannel dac, AdcChannel adc, float interval_s) const
{
    if (interval_s > MAX_WAVEFORM_DELAY_S) {
        throw std::invalid_argument("interval greater than the maximum supported.");
    }

    if ((bitDepth == BitDepth::BitDepth10 && interval_s < Plane::MinAcqWformInterval10Bit_s)
        || (bitDepth == BitDepth::BitDepth12 && interval_s < Plane::MinAcqWformInterval12Bit_s)) {
            throw std::invalid_argument("interval less than the minimum possible in " + std::to_string(int(bitDepth)) + "-bit mode.");
    }

    PktP2tReqReadings pkt;
    pkt.header = makePktHeader(P2tPktType::P2tRequestReadings, sizeof(pkt));
    static_assert(sizeof(pkt.delay) == sizeof(std::uint16_t), "PktP2tReqReadings::delay is assumed to be 16 bits");
    pkt.delay = htons(std::uint16_t(interval_s / WAVEFORM_DELAY_RESOLUTION_S));
    pkt.dac_channel = std::uint8_t(dac);
    pkt.adc_channel = std::uint8_t(adc);

    std::chrono::seconds timeout_s = AcqWaveformTimeoutBase + std::chrono::seconds(int(std::ceil(interval_s * MAX_WAVEFORM_VALUES)));

    return sendPacketAndGetWform(reinterpret_cast<Pkt*>(&pkt), timeout_s, adc);
}

std::vector<float> PlaneImpl::acquireTransientResponse(DacChannel dac, AdcChannel adc, float interval_s, float step_V) const
{
    if (interval_s > MAX_TRANS_RESP_DELAY_S) {
        throw std::invalid_argument("interval greater than the maximum supported.");
    }

    if ((bitDepth == BitDepth::BitDepth10 && interval_s < Plane::MinAcqTransRespInterval10Bit_s)
        || (bitDepth == BitDepth::BitDepth12 && interval_s < Plane::MinAcqTransRespInterval12Bit_s)) {
            throw std::invalid_argument("interval less than the minimum possible in " + std::to_string(int(bitDepth)) + "-bit mode.");
    }

    validateVoltage(step_V);

    PktP2tReqTransResp pkt;
    pkt.header = makePktHeader(P2tPktType::P2tRequestTransResp, sizeof(pkt));
    pkt.step_value = htons(std::uint16_t(step_V / DAC_LSB_V));
    static_assert(sizeof(pkt.delay) == sizeof(std::uint16_t), "PktP2tReqTransResp::delay is assumed to be 16 bits");
    pkt.delay = htons(std::uint16_t(interval_s / TRANS_RESP_DELAY_RESOLUTION_S));
    pkt.dac_channel = std::uint8_t(dac);
    pkt.adc_channel = std::uint8_t(adc);

    std::chrono::seconds timeout_s = AcqTransRespTimeoutBase + std::chrono::seconds(int(std::ceil(interval_s * MAX_WAVEFORM_VALUES)));

    return sendPacketAndGetWform(reinterpret_cast<Pkt*>(&pkt), timeout_s, adc);
}

void PlaneImpl::requestEvents(float duration_s)
{
    if (duration_s > MAX_AER_OP_DURATION_S) {
        throw std::invalid_argument("duration greater than the maximum supported.");
    }

    std::lock_guard<std::mutex> lock(receiveDataMutex);
    receiveEvents = true;

    PktP2tReqAerOutput pkt;
    pkt.header = makePktHeader(P2tPktType::P2tRequestAerOutput, sizeof(pkt));
    static_assert(sizeof(pkt.duration) == sizeof(std::uint16_t), "PktP2tReqAerOutput::duration is assumed to be 16 bits");
    pkt.duration = htons(std::uint16_t(duration_s / AER_OP_DURATION_RESOLUTION_S));
    transmit(reinterpret_cast<Pkt*>(&pkt));
}

void PlaneImpl::readC2fOutput(float duration_s, std::array<unsigned int, NUM_C2FS>& c2fs) const
{
    if (duration_s > MAX_C2F_OP_DURATION_S) {
        throw std::invalid_argument("duration greater than the maximum supported.");
    }

    PktP2tReqC2fOutput pkt;
    pkt.header = makePktHeader(P2tPktType::P2tRequestC2fOutput, sizeof(pkt));
    static_assert(sizeof(pkt.duration) == sizeof(std::uint16_t), "PktP2tReqC2fOutput::duration is assumed to be 16 bits");
    pkt.duration = htons(std::uint16_t(duration_s / C2F_OP_DURATION_RESOLUTION_S));

    std::chrono::seconds timeout_s = ReqC2fOutputTimeoutBase + std::chrono::seconds(int(duration_s + 0.5));

    c2fs = sendPacketAndGetReply(c2fsPromise, reinterpret_cast<Pkt*>(&pkt), timeout_s);
}

std::vector<CoachOutputEvent> PlaneImpl::readEvents()
{
    std::lock_guard<std::mutex> lock(receiveDataMutex);
    return std::move(receiveQueue);
}

void PlaneImpl::sendCoachEvents(const std::vector<coach::CoachInputEvent>& events) const
{
    if (events.size() > MAX_SEND_COACH_EVENTS) {
        throw std::length_error("attempting to send more than the maximum number of events.");
    }

    PktP2tSendCoachEvents pkt;
    pkt.header = makePktHeader(P2tPktType::P2tSendCoachEvents,
                               sizeof(pkt.header) + events.size() * sizeof(std::uint32_t));
    for (unsigned int i = 0; i < events.size(); i++) {
        pkt.events[i] = htonl(std::uint32_t(events[i]));
    }

    if (sendPacketAndGetReply(statusPromise, reinterpret_cast<Pkt*>(&pkt), SendCoachEventsTimeout) != TeensyStatus::Success) {
        throwTeensyStatusError();
    }
}

}
