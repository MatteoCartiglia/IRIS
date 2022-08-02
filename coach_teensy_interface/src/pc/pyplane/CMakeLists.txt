# We are going to build a library named plane that
# will then be used to build a python extension module called pyplane.

# First we specify the packages that must be present on the system
find_package(pybind11 REQUIRED)

# The Threads package is a cross platform way to include a threading
# library. On Linux this will bring in pthreads
find_package(Threads REQUIRED)

# Now we specify the sources for the base library.
# It is not necessary to specify the headers, but it may be
# useful for adding a formatting tool in the future.
set(LIBPLANE_HDRS
    coach.h
    plane.h
    planeimpl.h
    teensy_serial.h
    )

set(LIBPLANE_SRCS
    plane.cpp
    planeimpl.cpp
    teensy_serial.cpp
    )

# Add library will cause CMake to build a static library by
# default. We want to compile using C++17 so we specify the
# standard we want to use to compile plane.
# We must enable position independent code if we wish to
# link against our static library.
add_library(plane ${LIBPLANE_SRCS})
set_property(TARGET plane PROPERTY CXX_STANDARD 17)
set_property(TARGET plane PROPERTY POSITION_INDEPENDENT_CODE ON)
target_compile_options(plane PRIVATE -Wall -Wextra)
target_include_directories(plane
    PRIVATE ${PROJECT_SOURCE_DIR}
    PRIVATE ${PROJECT_SOURCE_DIR}/pc
    PRIVATE ${PROJECT_BINARY_DIR}/config
    )

# Add custom targets to generate the pybind enum definitions automagically from
# the C++ enums using the pybind_enums awk script. This is good for consistency.
# These custom targets are then declared as dependencies of pyplane.
set(PB_ENUM_AWK_SCRIPT ${PROJECT_SOURCE_DIR}/pc/pybind_enums.awk)
set(COACH_H ${PROJECT_SOURCE_DIR}/pc/coach.h)
add_custom_target(coach_pybind_enums
    COMMAND /bin/sh -c "awk -v enum=BiasAddress          -v scope=Coach -f ${PB_ENUM_AWK_SCRIPT} <${COACH_H} >coach.BiasAddress.pybind_enum.h"
    COMMAND /bin/sh -c "awk -v enum=BiasGenMasterCurrent -v scope=Coach -f ${PB_ENUM_AWK_SCRIPT} <${COACH_H} >coach.BiasGenMasterCurrent.pybind_enum.h"
    COMMAND /bin/sh -c "awk -v enum=BiasType             -v scope=Coach -f ${PB_ENUM_AWK_SCRIPT} <${COACH_H} >coach.BiasType.pybind_enum.h"
    COMMAND /bin/sh -c "awk -v enum=SynapseSelect        -v scope=Coach -f ${PB_ENUM_AWK_SCRIPT} <${COACH_H} >coach.SynapseSelect.pybind_enum.h"
    COMMAND /bin/sh -c "awk -v enum=CurrentOutputSelect  -v scope=Coach -f ${PB_ENUM_AWK_SCRIPT} <${COACH_H} >coach.CurrentOutputSelect.pybind_enum.h"
    COMMAND /bin/sh -c "awk -v enum=VoltageOutputSelect  -v scope=Coach -f ${PB_ENUM_AWK_SCRIPT} <${COACH_H} >coach.VoltageOutputSelect.pybind_enum.h"
    COMMAND /bin/sh -c "awk -v enum=VoltageInputSelect   -v scope=Coach -f ${PB_ENUM_AWK_SCRIPT} <${COACH_H} >coach.VoltageInputSelect.pybind_enum.h"
    DEPENDS ${PB_ENUM_AWK_SCRIPT}
    DEPENDS ${COACH_H}
    COMMENT "Building Python enum bindings coach.*.pybind_enum.h"
    VERBATIM
    )
set(PLANE_H ${PROJECT_SOURCE_DIR}/usb_packets.h)
add_custom_target(plane_pybind_enums
    COMMAND /bin/sh -c "awk -v enum=CurrentRange -f ${PB_ENUM_AWK_SCRIPT} <${PLANE_H} >plane.CurrentRange.pybind_enum.h"
    COMMAND /bin/sh -c "awk -v enum=ResetType    -f ${PB_ENUM_AWK_SCRIPT} <${PLANE_H} >plane.ResetType.pybind_enum.h"
    COMMAND /bin/sh -c "awk -v enum=DacChannel   -f ${PB_ENUM_AWK_SCRIPT} <${PLANE_H} >plane.DacChannel.pybind_enum.h"
    COMMAND /bin/sh -c "awk -v enum=AdcChannel   -f ${PB_ENUM_AWK_SCRIPT} <${PLANE_H} >plane.AdcChannel.pybind_enum.h"
    COMMAND /bin/sh -c "awk -v enum=BitDepth     -f ${PB_ENUM_AWK_SCRIPT} <${PLANE_H} >plane.BitDepth.pybind_enum.h"
    COMMAND /bin/sh -c "awk -v enum=TeensyStatus -f ${PB_ENUM_AWK_SCRIPT} <${PLANE_H} >plane.TeensyStatus.pybind_enum.h"
    DEPENDS ${PB_ENUM_AWK_SCRIPT}
    DEPENDS ${PLANE_H}
    COMMENT "Building Python enum bindings plane.*.pybind_enum.h"
    VERBATIM
    )

# Now we can specify the sources for the python extension module.
# For now the only source is a file that specifies the pybind11
# bindings. We will link this against the base library.
set(PYPLANE_SRCS
    pyplane.cpp
    )

# We can use add_library to build the library again. This time we link against
# our dependencies using CMake targets. Linking against pybind11::module will
# add a bunch of compilation options that are required.
# Finally we add the python module prefix and suffix which is required for the
# python interpreter to be able to find the module when we perform an import.
add_library(pyplane MODULE ${PYPLANE_SRCS})
add_dependencies(pyplane coach_pybind_enums plane_pybind_enums)
target_link_libraries(pyplane PUBLIC pybind11::module plane)
set_property(TARGET pyplane PROPERTY CXX_STANDARD 17)
target_compile_options(pyplane PRIVATE -Wall -Wextra)
target_include_directories(pyplane
    PRIVATE ${PROJECT_SOURCE_DIR}
    PRIVATE ${PROJECT_SOURCE_DIR}/pc
    PRIVATE ${PROJECT_BINARY_DIR}/config
    PRIVATE ${PROJECT_BINARY_DIR}/pc/pyplane
    )
set_target_properties(pyplane PROPERTIES
    PREFIX "${PYTHON_MODULE_PREFIX}"
    SUFFIX "${PYTHON_MODULE_EXTENSION}"
    )
