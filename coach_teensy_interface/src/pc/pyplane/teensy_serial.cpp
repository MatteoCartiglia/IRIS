#include "teensy_serial.h"

#include <fcntl.h>
#include <system_error>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>

namespace plane {

void TeensySerial::closeAndThrowErrno(const std::string& what)
{
	int e = errno;
	close();
	throw std::system_error(e, std::system_category(), what);
}

void TeensySerial::open(const std::string& deviceName)
{
	struct stat statinfo;
	if (stat(deviceName.c_str(), &statinfo) == -1) {
		throw std::system_error(errno, std::system_category(), "attempting to stat " + deviceName);
	}
	if ((statinfo.st_mode & S_IFMT) != S_IFCHR) {
		throw std::system_error(ENODEV, std::generic_category(), deviceName + " is not a device");
	}

	fd = ::open(deviceName.c_str(), O_RDWR);
	if (fd == -1) {
		closeAndThrowErrno("opening " + deviceName);
	}

	if (lockf(fd, F_TLOCK, 0) == -1) {
		closeAndThrowErrno("attempting to lock " + deviceName);
	}

	struct termios settings;

	if (tcgetattr(fd, &settings) == -1) {
		closeAndThrowErrno("tcgetattr on " + deviceName);
	}

	cfmakeraw(&settings);

	if (tcsetattr(fd, TCSANOW, &settings) == -1) {
		closeAndThrowErrno("tcsetattr on " + deviceName);
	}
}

void TeensySerial::read(std::vector<std::uint8_t>& inputBuffer) const
{
	constexpr size_t MaxInputBufferSize = 1024;

	inputBuffer.resize(MaxInputBufferSize);
	auto retval = ::read(fd, inputBuffer.data(), MaxInputBufferSize);
	if (retval == -1) {
		throw std::system_error(errno, std::system_category(), "read");
	}
	inputBuffer.resize(retval);
}

size_t TeensySerial::write(const std::vector<std::uint8_t>& outputBuffer) const
{
	auto bytesToWrite = outputBuffer.size();
	if (bytesToWrite == 0) {
		return 0;
	}
	auto retval = ::write(fd, outputBuffer.data(), outputBuffer.size());
	if (retval == -1) {
		throw std::system_error(errno, std::system_category(), "write");
	}
	size_t bytesWritten = retval;
    return bytesWritten;
}

void TeensySerial::close()
{
	if (fd != -1) {
		::close(fd);
		fd = -1;
	}
}

}
