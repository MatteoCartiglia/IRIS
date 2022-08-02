cmake_minimum_required(VERSION 3.10)

project(PLANE VERSION 1.8.7)

configure_file(config/version.h.in ${PROJECT_BINARY_DIR}/config/version.h @ONLY)

add_subdirectory(pc)
add_subdirectory(teensy)
