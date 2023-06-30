if (NOT DEFINED ARDUINOPATH)
    message(STATUS "No arduino path provided. Teensy firmware will not be built")
else()
    set(CMAKE_CXX_STANDARD 17)

    # This toolchain file is based on https://github.com/apmorton/teensy-template/blob/master/Makefile
    # and on the Teensy Makefile.

    include(CMakeForceCompiler)


    set(TEENSY_VERSION 41)
    set(CPU_CORE_SPEED 600000000)


    # Derived variables
    #
    set(CPU cortex-m7)

    set(TOOLSPATH "${ARDUINOPATH}/hardware/tools/")
    set(COMPILERPATH "${TOOLSPATH}/arm/bin/")
    set(COREPATH "${ARDUINOPATH}/hardware/teensy/avr/cores/teensy4/")


    # Normal toolchain configuration
    #

    # this one is important
    set(CMAKE_SYSTEM_NAME Generic)

    # specify the cross compiler
    set(CMAKE_C_COMPILER ${COMPILERPATH}arm-none-eabi-gcc)
    set(CMAKE_CXX_COMPILER ${COMPILERPATH}arm-none-eabi-g++)

    # where is the target environment
    set(CMAKE_FIND_ROOT_PATH ${COMPILERPATH})

    # search for programs in the build host directories
    set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

    # for libraries and headers in the target directories
    set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)


    # Additional C and CPP files for Arduino support
    #

    file(GLOB TEENSY_C_FILES ${COREPATH}*.c)
    file(GLOB TEENSY_CPP_FILES ${COREPATH}*.cpp)
    set(TEENSY_SOURCES ${TEENSY_C_FILES} ${TEENSY_CPP_FILES})
    #list(REMOVE_ITEM TEENSY_SOURCES ${COREPATH}main.cpp)


    set(CPU_DEFINE __IMXRT1062__)
    set(LINKER_FILE ${COREPATH}imxrt1062_t41.ld)

    set(TEENSY_DEFINITIONS
        USB_SERIAL
        LAYOUT_US_ENGLISH
        F_CPU=${CPU_CORE_SPEED}
        ${CPU_DEFINE}
        ARDUINO_TEENSY41
        )

    message("TEENSY_DEFINITIONS " ${TEENSY_DEFINITIONS} )
    # Macros to wrap add_[executable|library] for seamless Teensy integration
    #
    function(teensy_add_executable TARGET)
        message("Corepath " ${COREPATH})
        set(ELFTARGET ${TARGET}.elf)
        message("mm ${ELFTARGET} ${ARGN} ${TEENSY_SOURCES}")
        add_executable(${ELFTARGET} ${ARGN} ${TEENSY_SOURCES} ${TEENSY_LIB_SOURCES})
        string(TOUPPER ${TARGET} UCTARGET)

        set_target_properties(${ELFTARGET} PROPERTIES COMPILE_FLAGS "-Wall -g -Os -mcpu=${CPU} -mthumb -nostdlib -MMD -felide-constructors -fno-exceptions -fno-rtti -std=gnu++0x")
        set_target_properties(${ELFTARGET} PROPERTIES COMPILE_DEFINITIONS "${TEENSY_DEFINITIONS} ${INCLUDE_DIRECTORIES} -DTARGET_${UCTARGET} -D__arm__ -DTEENSYDUINO=151")
        set_target_properties(${ELFTARGET} PROPERTIES INCLUDE_DIRECTORIES "${COREPATH}")
        set_target_properties(${ELFTARGET} PROPERTIES LINK_FLAGS "-Os -Wl,--gc-sections,--defsym=__rtc_localtime=0 --specs=nano.specs -mcpu=${CPU} -mthumb -T${LINKER_FILE}")

        add_custom_command(OUTPUT ${TARGET}.hex
            COMMAND ${COMPILERPATH}arm-none-eabi-size ${ELFTARGET}
            COMMAND ${COMPILERPATH}arm-none-eabi-objcopy -O ihex -R .eeprom ${ELFTARGET} ${TARGET}.hex
            DEPENDS ${ELFTARGET}
            COMMENT "Translating ${ELFTARGET} to ${TARGET}.hex"
            )

        add_custom_target(${TARGET} ALL DEPENDS ${TARGET}.hex)
    endfunction()

    macro(import_arduino_library LIB_ROOT LIB_NAME)
        # Check if we can find the library.
        if(NOT EXISTS ${LIB_ROOT})
            message(FATAL_ERROR "Could not find the Arduino library directory")
        endif(NOT EXISTS ${LIB_ROOT})
        set(LIB_DIR "${LIB_ROOT}/${LIB_NAME}")
        #message(LIB_DIR ${LIB_DIR})
        if(NOT EXISTS "${LIB_DIR}")
            message(FATAL_ERROR "Could not find the directory for library '${LIB_NAME}'")
        endif(NOT EXISTS "${LIB_DIR}")

        # Add it to the include path.
        include_directories("${LIB_DIR}")
        set(INCLUDE_DIRECTORIES "${INCLUDE_DIRECTORIES}-I${LIB_DIR} ")
        message("include_directories " ${LIB_DIR} "  - " ${INCLUDE_DIRECTORIES})

        # Mark source files to be built along with the sketch code.
        file(GLOB SOURCES_CPP ABSOLUTE "${LIB_DIR}" "${LIB_DIR}/*.cpp")
        foreach(SOURCE_CPP ${SOURCES_CPP})
            set(TEENSY_LIB_SOURCES ${TEENSY_LIB_SOURCES} ${SOURCE_CPP})
        endforeach(SOURCE_CPP ${SOURCES_CPP})
        file(GLOB SOURCES_C ABSOLUTE "${LIB_DIR}" "${LIB_DIR}/*.c")
        foreach(SOURCE_C ${SOURCES_C})
            set(TEENSY_LIB_SOURCES ${TEENSY_LIB_SOURCES} ${SOURCE_C})
            message("lib src ${TEENSY_LIB_SOURCES}")
        endforeach(SOURCE_C ${SOURCES_C})
    endmacro(import_arduino_library)

    set(ARDUINO_LIB_ROOT "${ARDUINOPATH}/hardware/teensy/avr/libraries")
    import_arduino_library(${ARDUINO_LIB_ROOT} ADC)
    import_arduino_library(${ARDUINO_LIB_ROOT} Wire)
    import_arduino_library(${ARDUINO_LIB_ROOT} SPI)

    set(MAIN_SRCS
        src/main.cpp
        src/AER_in.cpp
        src/dac.cpp
        src/datatypes.cpp
        src/spiConfig.cpp
        src/teensyOut.cpp
#        lib/mcp/Adafruit_MCP23017.cpp
#        lib/mcp/Adafruit_MCP23017.h
        )

    teensy_add_executable(ahpc ${MAIN_SRCS})
    teensy_add_executable(alive ${MAIN_SRCS})
    teensy_add_executable(spaic ${MAIN_SRCS})
    teensy_add_executable(texel ${MAIN_SRCS} src/texel.cpp)
    include_directories(include)

endif()
