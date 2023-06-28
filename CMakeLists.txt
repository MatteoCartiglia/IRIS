cmake_minimum_required(VERSION 3.10)

project(IRIS VERSION 0.6.22)

# The following line can be used to generate a .h file that includes the project
# version number so that it can be read from the C/C++ code.
# See https://code.ini.uzh.ch/CoACH/CoACH_Teensy_interface for how to do this.

# configure_file(config/version.h.in ${PROJECT_BINARY_DIR}/config/version.h @ONLY)

add_subdirectory(imgui_frontend)
add_subdirectory(teensy_backend)
