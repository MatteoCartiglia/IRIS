# 0.4.0

* Fix #33 - Implement get_firmware_version()

# 0.3.5

* Fix #58 & #61 in the Teensy firmware - no user-visible change.

* Fix #55 - read voltage and request packet commands now always do 12-bit voltage sampling.

* Fix #62 - Enforce and document a 6kHz limit on the slow sampling rate.

# 0.3.4

* Fix #50 - acquire_transient_response() now waits properly for the results to be received from the Teensy before returning so that the results will always be available by the time the user's code can call read_adc_samples(). Or it will time out and throw an exception!

* Fix #51 - Fill in and update docstrings for fast sampling mode

# 0.3.3

* Fix #53 - Start in SamplingMode.Slow rather than SamplingMode.Off for backward compatilbility with scripts that ran with pyplane versions < 0.3.0 in which there were no modes and all that was required to start sampling was set_sample_rate(), with no set_sampling_mode().
Returning to a 0Hz sampling rate will still cause the sampling mode to revert to Off though; this fix only affects the initial mode.

* Fix #56 - Sort the fast mode ADC numbers into increasing ADC number order in pyplane, the same as the firmware, to ensure that the results that come back are matched to the correct ADC.

* Add handling of new Teensy to PC error codes.
    This is done principally by avoiding causing them in the first place.
    In order to do this reliably, sampling (fast or slow) is now explicitly stopped when Plane.open() is called.

# 0.3.2

* Fix #54 - Don't allow users to specify a sampling interval deltaT_us < 5 times the number of ADCs to be sampled from.

# 0.3.1

* Fix issue with ADC voltages reported in fast mode by read_adc_samples() being too small by a factor of 4.

# 0.3.0

* Implement 'fast sampling mode' with the following new functions:
    set_sampling_mode()
    get_sampling_mode()
    set_fast_sampling_adcs()
    get_fast_sampling_adcs()
    acquire_transient_response()
    read_adc_samples()

# 0.2.1

* Fix #3  - Include building the Teensy code into the build process.
* Fix #42 - Separate GUI & pyplane sources into pc/gui & pc/pyplane directories.

# 0.2.0

* Fix #44 - Handles coalesced packets.
    Beware: this relies on having matched firmware! Only use firmware from the same build.
    Sampling packets can no longer be reported as being malformed, but it is possible that they get truncated if the entire receive buffer (or the
    Teensy transmit buffer) is filled - this will lead to an exception.
    Receiving an unknown error no. in an Error packet also now causes an exception to be thrown.
* Add Plane.debug property to allow switching on and off at runtime the printing on stderr of packets transmitted to and received from the Teensy.
* Fix #41 - Rename the 'main' GUI program to planegui.

# 0.1.0

* Fix #43 - Increase timeout factor from 2x to 5x the typical execution time of read_voltage(), read_current() and set_current().
* Fix #38 - Attempting to open a file which is not a character device now reports "foo is not a device: No such device" rather than "tcgetattr on foo: Inappropriate ioctl for device"
* Plane.get_device_name() now returns None if no device has been opened, rather than a zero-length string.
* Fix #40 - Change back to using an awk script to generate pybind enums from C++ enums.

# 0.0.7

* Fix #40 (interim fix) such that all relevant enum values are available in Python.

# 0.0.6

* Fix #39 - Add timeouts on read_adc_instantaneous(), read_current() and set_current() of approx. double the typical completion times and throw an exception if a timeout occurs.
* Add a load-teensy.sh script to load the firmware from the command line.

# 0.0.5

* Fix #34 - Treat a second attempt to open the same device from pyplane.Plane as a no-op, but throw a RuntimeError if a user tries to open a different device after already successfully opening a first device.
* Add Plane.get_device_name().
* Fix #35 - Prevent the same device being opened twice in different objects (e.g. by different users) by obtaining a lock.
* Fix python hanging on exit if sampling wasn't running (#30).
* Improve docstring for Plane.open() by explicitly mentioning that it can throw RuntimeError.
* Add a pyplane.get_version() function.
* Add a changelog!
