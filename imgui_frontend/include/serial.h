//------------------------------------------------------------------------------------------------------------------------------------------
// Header file for serial communication class
//------------------------------------------------------------------------------------------------------------------------------------------

#ifndef SERIAL_H
#define SERIAL_H

#include <system_error>
#include <vector>
#include <cstdio>           // Standard input output
#include <cerrno>           // Error number definitions
#include <cstring>

#include <unistd.h>         // UNIX standard function definitions
#include <termios.h>        // POSIX terminal control definitions
#include <fcntl.h>          // File control definitions
#include <sys/types.h>
#include <sys/stat.h>

#include "../../teensy_backend/include/datatypes.h"
#include "../../teensy_backend/include/constants.h"

class Serial 
{
    public:

        //----------------------------------------------------------------------------------------------------------------------------------
        // Serial constructor: instantiates an object of the Serial class and opens the serial port
        //----------------------------------------------------------------------------------------------------------------------------------
        Serial();

        //----------------------------------------------------------------------------------------------------------------------------------
        // Delete the copy constructor
        //----------------------------------------------------------------------------------------------------------------------------------
        Serial(const Serial&) = delete;

        //----------------------------------------------------------------------------------------------------------------------------------
        // Serial destructor: destroys instance of class object
        //----------------------------------------------------------------------------------------------------------------------------------
        ~Serial() { closeSerialPort(); }

        //----------------------------------------------------------------------------------------------------------------------------------
        // openSerialPort: opens serial port using the device name provided
        //----------------------------------------------------------------------------------------------------------------------------------
        void openSerialPort();

        //----------------------------------------------------------------------------------------------------------------------------------
        // closeSerialPort: Closes the serial port
        //----------------------------------------------------------------------------------------------------------------------------------
        void closeSerialPort();

        //----------------------------------------------------------------------------------------------------------------------------------
        // readSerialPort: reads serial buffer until all expected responses have been processed
        //----------------------------------------------------------------------------------------------------------------------------------
        void readSerialPort(int expectedResponses, int bufferSize, char* logEntry) const;

        //----------------------------------------------------------------------------------------------------------------------------------
        // writeSerialPort: writes nBytes of data buffer to serial port
        //----------------------------------------------------------------------------------------------------------------------------------
        size_t writeSerialPort(const void *buffer, size_t nBytes) const;

        //----------------------------------------------------------------------------------------------------------------------------------
        // getFileDescriptor: retrieves the private member variable
        //----------------------------------------------------------------------------------------------------------------------------------
        int getFileDescriptor();

    private:
        
        //----------------------------------------------------------------------------------------------------------------------------------
        // closeAndThrowError: Closes serial port and throws the related error
        //----------------------------------------------------------------------------------------------------------------------------------
        void closeAndThrowError(const std::string& errorStr);

        // Terminal interface struct
        struct termios _serialPortSettings;

        // Stat variable for file status information
        struct stat _statinfo;

        // File descriptor: 
        int fd = -1;
};

#endif
