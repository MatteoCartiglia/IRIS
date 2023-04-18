//------------------------------------------------------------------------------------------------------------------------------------------
// Source file for serial communication class
//------------------------------------------------------------------------------------------------------------------------------------------

#include "../include/serial.h"

//------------------------------------------------------------------------------------------------------------------------------------------
// Serial constructor: instantiates an object of the Serial class and opens the serial port
//------------------------------------------------------------------------------------------------------------------------------------------
Serial::Serial() {

    try {
        openSerialPort();
    }

    catch(std::exception &exception) {
        printf("Error opening serial port. \t Error %i; '%s'\n", errno, strerror(errno));
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
// openSerialPort: opens serial port using the device name provided
//------------------------------------------------------------------------------------------------------------------------------------------
void Serial::openSerialPort() {

    const std::string& deviceName = DEVICE_NAME;

    // Get file attributes for the device filepath
    if (stat(deviceName.c_str(), &_statinfo) == -1) {
		throw std::system_error(errno, std::system_category(), "Unable to stat " + deviceName);
	}

    // Throw an error if the filepath provided does not point to a connected device
	if ((_statinfo.st_mode & S_IFMT) != S_IFCHR) {
		throw std::system_error(ENODEV, std::generic_category(), deviceName + " is not a device.");
	}

    // Open the serial port in read/write mode
	fd = open(deviceName.c_str(), O_RDWR);

    // Throw error if error occurs opening serial port
	if (fd == -1) {
		closeAndThrowError("Invalid file descriptor. Cannot open " + deviceName);
	}

    // Locking serial port file descriptor to restrict access to this process only
	if (lockf(fd, F_TLOCK, 0) == -1) {
		closeAndThrowError("Unable to lock " + deviceName);
	}

    // Get the current attributes of the Serial port 
	if (tcgetattr(fd, &_serialPortSettings) == -1) {
		closeAndThrowError("tcgetattr on " + deviceName);
	}

    cfmakeraw(&_serialPortSettings);
    cfsetispeed(&_serialPortSettings, B19200);            // Set Read Speed as 19200                     
    cfsetospeed(&_serialPortSettings, B19200);            // Set Write Speed as 19200
    
    // Set the Serial Port attributes
	if (tcsetattr(fd, TCSANOW, &_serialPortSettings) == -1) {   
		closeAndThrowError("tcsetattr on " + deviceName);
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------
// closeSerialPort: Closes the serial port
//------------------------------------------------------------------------------------------------------------------------------------------
void Serial::closeSerialPort() {

	if (fd != -1) {
		close(fd);
		fd = -1;
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------
// readSerialPort: reads serial buffer until all expected responses have been processed
//------------------------------------------------------------------------------------------------------------------------------------------
void Serial::readSerialPort(int expectedResponses, int bufferSize, char* logEntry) const
{
    int serialReadBytes = 0;

    if(fd != -1) 
    {
        char serialReadBuffer[bufferSize];
        std::fill(serialReadBuffer, serialReadBuffer + bufferSize, ' ');

        try {
            serialReadBytes = read(fd, &serialReadBuffer, bufferSize);
            
            if(serialReadBytes <=0) 
            {
                throw std::runtime_error("Serial port read error.");
            }

            for(int i = 0; i < bufferSize; i++) {
                logEntry[i] = serialReadBuffer[i];
            }
        }
        
        catch(std::exception &exception) {
            printf("Error reading from serial port. \t Error %i; '%s' \t Serial read byte: %d\n", errno, strerror(errno), serialReadBytes);
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
// writeSerialPort: writes nBytes of data buffer to serial port 
//------------------------------------------------------------------------------------------------------------------------------------------
size_t Serial::writeSerialPort(const void *buffer, size_t nBytes) const
{
    int serialWriteBytes = 0;
    
    if(fd != -1) {

        try {
            serialWriteBytes = write(fd, buffer, nBytes);
            
            if(serialWriteBytes <= 0)
            {
                throw std::runtime_error("Serial port write error.");
            }

            return serialWriteBytes;
        }

        catch(std::exception &exception) {
            printf("Error writing to serial port. \t\t Error %i; '%s' \t Serial write byte: %d\n", errno, strerror(errno), serialWriteBytes);
            return serialWriteBytes;
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------------------
// getFileDescriptor: retrieves the private member variable
//----------------------------------------------------------------------------------------------------------------------------------
int Serial::getFileDescriptor()
{
    return fd;
}

//------------------------------------------------------------------------------------------------------------------------------------------
// closeAndThrowError: Closes serial port and throws the related error
//------------------------------------------------------------------------------------------------------------------------------------------
void Serial::closeAndThrowError(const std::string& errorStr) {

	int errorNo = errno;
	closeSerialPort();
	throw std::system_error(errorNo, std::system_category(), errorStr);
}