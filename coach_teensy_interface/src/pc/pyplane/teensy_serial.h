#ifndef PLANE_TEENSY_SERIAL_H_
#define PLANE_TEENSY_SERIAL_H_

#include <array>
#include <cstdint>
#include <string>
#include <vector>

namespace plane {

class TeensySerial {
public:
    TeensySerial() {}
    TeensySerial(const TeensySerial&) = delete;
    ~TeensySerial() { close(); }
    void open(const std::string& deviceName);
    void read(std::vector<std::uint8_t>& inputBuffer) const;
    size_t write(const std::vector<std::uint8_t>& outputBuffer) const;
    void close();

private:
    void closeAndThrowErrno(const std::string& what);

    int fd = -1;
};

}

#endif
