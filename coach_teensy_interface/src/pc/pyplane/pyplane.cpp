#include "plane.h"
#include "version.h"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;
using namespace coach;
using namespace plane;

PYBIND11_MODULE(pyplane, m)
{
    m.doc() = "Interface to CoACH class-chip via Teensy based PLANE (Platform for Learning About Neuromorphic Engineering).";

    m.def("get_version", [](){ return std::make_tuple(PLANE_VERSION_MAJOR, PLANE_VERSION_MINOR, PLANE_VERSION_PATCH); });

    py::class_<Coach> coach(m, "Coach");

    coach.def_property_readonly_static("SRE_VEN_VSI", [](py::object /* self */) { return Coach::Aerc_SRE_VEN_VSI; });
    coach.def_property_readonly_static("WTA_VHEN_SI", [](py::object /* self */) { return Coach::Aerc_WTA_VHEN_SI; });
    coach.def_property_readonly_static("ACN_ADPEN_ASI", [](py::object /* self */) { return Coach::Aerc_ACN_ADPEN_ASI; });
    coach.def_property_readonly_static("ACN_DCEN_ASBI", [](py::object /* self */) { return Coach::Aerc_ACN_DCEN_ASBI; });
    coach.def_property_readonly_static("ATN_DCEN_ASBI", [](py::object /* self */) { return Coach::Aerc_ATN_DCEN_ASBI; });
    coach.def_property_readonly_static("ATN_ADPEN_ASI", [](py::object /* self */) { return Coach::Aerc_ATN_ADPEN_ASI; });
    coach.def_property_readonly_static("ASN_DCEN_ASBI", [](py::object /* self */) { return Coach::Aerc_ASN_DCEN_ASBI; });

// The following includes are automatically generated from the enums in coach.h and usb_packets.h by applying the
// awk script pybind_enums.awk
#include "coach.BiasAddress.pybind_enum.h"
#include "coach.BiasGenMasterCurrent.pybind_enum.h"
#include "coach.BiasType.pybind_enum.h"
#include "coach.SynapseSelect.pybind_enum.h"
#include "coach.CurrentOutputSelect.pybind_enum.h"
#include "coach.VoltageOutputSelect.pybind_enum.h"
#include "coach.VoltageInputSelect.pybind_enum.h"
#include "plane.CurrentRange.pybind_enum.h"
#include "plane.ResetType.pybind_enum.h"
#include "plane.DacChannel.pybind_enum.h"
#include "plane.AdcChannel.pybind_enum.h"
#include "plane.BitDepth.pybind_enum.h"
#include "plane.TeensyStatus.pybind_enum.h"

    py::class_<CoachInputEvent>(m, "CoachInputEvent")
        .def(py::init<std::uint32_t>())
        .def_property_readonly("event", [](const CoachInputEvent& cie){ return static_cast<std::uint32_t>(cie); })
        ;

    coach.def("generate_biasgen_event", &Coach::generateBiasGenEvent, "Generate a biasgen event suitable for passing to send_coach_event().");
    coach.def("generate_aerc_event", &Coach::generateAercEvent, "Generate an AER control event suitable for passing to send_coach_event().");
    coach.def("generate_pulse_event", &Coach::generatePulseEvent, "Generate a pulse event suitable for passing to send_coach_event().");

    py::class_<CoachOutputEvent>(m, "CoachOutputEvent")
        .def(py::init<unsigned int, unsigned int>())
        .def_readwrite("timestamp", &CoachOutputEvent::timestamp)
        .def_readwrite("address", &CoachOutputEvent::address)
        .def("as_tuple", [](const CoachOutputEvent& coe){ return std::make_pair(coe.timestamp, coe.address); })
        ;

    py::class_<Plane>(m, "Plane")
        .def_property_readonly_static("teensy_ref_voltage_V", [](py::object /* self */) { return Plane::TeensyReferenceVoltage_V; })
        .def_property_readonly_static("max_settable_voltage_V", [](py::object /* self */) { return &Plane::MaxSettableVoltage_V; })
        .def_property_readonly_static("min_acquire_waveform_interval_12bit_s", [](py::object /* self */)
            { return Plane::MinAcqWformInterval12Bit_s; })
        .def_property_readonly_static("min_acquire_waveform_interval_10bit_s", [](py::object /* self */)
            { return Plane::MinAcqWformInterval10Bit_s; })
        .def_property_readonly_static("min_acquire_transient_response_interval_12bit_s", [](py::object /* self */)
            { return Plane::MinAcqTransRespInterval12Bit_s; })
        .def_property_readonly_static("min_acquire_transient_response_interval_10bit_s", [](py::object /* self */)
            { return Plane::MinAcqTransRespInterval10Bit_s; })

        .def(py::init<>())

        .def_property("debug", &Plane::getDebug, &Plane::setDebug)

        .def("open", &Plane::open,
             "Open communication with the Teensy on the PLANE board via the named device, on Linux typically /dev/ttyACM0.\n"
             "Calling open twice is a no-op if the same device is requested, and an error if a different device is requested.\n"
             "Errors in open() are reported by throwing a RuntimeError.")

        .def("get_device_name", &Plane::getDeviceName,
             "Return the name of the device that was opened.")

        .def("reset", &Plane::reset,
             "Resets the (Teensy,) CoACH chip, DACs and current sensors (and blinks the LEDs).\n"
             "If ResetType.Soft is specified, the Teensy is not reset."
             " If ResetType.Hard is specified, the Teensy is reset. This takes several seconds to execute.\n"
             "Returns a TeensyStatus which can be Success, HardResetFailed, or, on v0.3 boards, HardResetNotSupported.")

        .def("get_firmware_version", &Plane::getFirmwareVersion,
             "Return a three-element tuple comprising major, minor, and patch version of the Teensy firmware,"
             " or (-1, -1, -1) if the version cannot be ascertained.\n")

        .def("set_voltage", &Plane::setVoltage,
             "Set the voltage, given in Volts, on the given DacChannel."
             " The maximum is given by max_settable_voltage_V.\n"
             "Returns the voltage, in Volts, that was set.")

        .def("get_set_voltage", &Plane::getSetVoltage,
             "Returns what was written using set_voltage(). Does not read from the board.")

        .def("set_voltage_waveform", &Plane::setVoltageWaveform,
             "Sets the waveform to apply in request_readings() and acquire_transient_response()"
             " by specifying a list of voltages, given in Volts.")

        .def("get_voltage_waveform", &Plane::getVoltageWaveform,
             "Returns the waveform written using set_voltage_waveform(). Does not read from the board.")

        .def("get_max_current", &Plane::getMaxCurrent,
             "Returns the maximum current, in Amps, that can be set for the given AdcChannel.")

        .def("set_current", &Plane::setCurrent,
             "Set the current, given in Amps, on the given AdcChannel.\n"
             "The valid range depends on the channel, and for GO23, GO22, GO21_N and GO20_N on the state of the switches."
             " Even within this range, it may not be possible to set the requested current, in which case a RuntimeError"
             " exception will be thrown. Callers should be prepared to catch such exceptions.\n"
             "Returns the current, in Amps, that was set.")

        .def("get_set_current", &Plane::getSetCurrent,
             "Returns what was written using set_current(). Does not read from the board.")

        .def("set_led_intensity", &Plane::setLedIntensity,
             "Set the intensity of the stimulus LED to a value in the range 0 to 255.")

        .def("get_led_intensity", &Plane::getLedIntensity,
             "Returns what was set using set_led_intensity(). Does not read from the board.")

        .def("set_bit_depth", &Plane::setBitDepth,
             "Set number of bits used for ADC conversions.\n"
             "Can be BitDepth.BitDepth10 or BitDepth.BitDepth12 (default).")

        .def("get_bit_depth", &Plane::getBitDepth,
             "Returns the value set by set_bit_depth(). Does not read from the board.")

        .def("read_voltage", &Plane::readVoltage,
             "Read one particular voltage AdcChannel. The return value is in Volts.")

        .def("read_current", &Plane::readCurrent,
             "Read one particular current AdcChannel. The return value is in Amps.")

        .def("acquire_waveform", &Plane::acquireWaveform,
             "Acquires a number of samples from the specified AdcChannel.\n"
             "Applies the values set using set_voltage_waveform() to the DacChannel given by the first argument and"
             " records samples from the AdcChannel given by the second argument at intervals given in seconds by the"
             " third argument.\n"
             "The results returned are in Volts or Amps, depending on the AdcChannel chosen.")

        .def("acquire_transient_response", &Plane::acquireTransientResponse,
             "Acquires a number of samples from the specified AdcChannel after applying a voltage step.\n"
             "Applies the values set using set_voltage_waveform() to the DacChannel given by the first argument and"
             " records samples from the AdcChannel given by the second argument at intervals given in seconds by the"
             " third argument.\n"
             "The first sample comes from before the voltage given by the fourth argument is applied; subsequent"
             " samples from subsequent time intervals.\n"
             "The results returned are in Volts or Amps, depending on the AdcChannel chosen.")

        .def("send_coach_events", &Plane::sendCoachEvents,
             "Send a list of CoachInputEvent prepared by one of the Coach.generate_*_event() functions to the CoACH chip.")

        .def("request_events", &Plane::requestEvents,
             "Enable events to be read using read_events().\n"
             "Events are received for the duration specified in seconds by the argument.")

        .def("read_c2f_output", &Plane::readC2fOutput,
             "Read all of the C2F values over a duration specified in seconds by the argument.")

        .def("read_events", &Plane::readEvents,
             "Return CoachOutputEvents read from a queue which is filled in the background once request_events() has"
             " been called.\n"
             "The timestamps in CoachOutputEvents are in units of 1.024 milliseconds.")

        ;
}
