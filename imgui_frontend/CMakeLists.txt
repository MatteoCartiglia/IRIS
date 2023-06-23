find_package(Threads REQUIRED)
find_package(glfw3 REQUIRED)

# Set policy CMP0072 to old to make sure that we link to libGL rather than
# libOpenGL / libGLX (and without getting a dev warning) as libGL is what the
# manually written Makefile was calling for.
# This can be re-examined in the future, as the OLD behaviour for this policy is
# deprecated, but building with libOpenGL needs testing first.

cmake_policy(SET CMP0072 OLD)

find_package(OpenGL REQUIRED)

set (IMGUI_DIR imgui/imgui_src)
set (BACKENDS_DIR imgui/imgui_backend)
set (SOURCE_DIR src)
set (TEENSY_BACKEND_DIR ../teensy_backend/src)

set (IMGUI_SOURCES
	${IMGUI_DIR}/imgui.cpp
	${IMGUI_DIR}/imgui_demo.cpp
	${IMGUI_DIR}/imgui_draw.cpp
	${IMGUI_DIR}/imgui_tables.cpp
	${IMGUI_DIR}/imgui_widgets.cpp
	${IMGUI_DIR}/implot.cpp
	${IMGUI_DIR}/implot_items.cpp
	${IMGUI_DIR}/implot_demo.cpp
)

set (IMGUI_BACKENDS
	${BACKENDS_DIR}/imgui_impl_glfw.cpp
	${BACKENDS_DIR}/imgui_impl_opengl3.cpp
)

set (ALIVE_SOURCES
	src/main.cpp
	src/guiFunctions.cpp
	src/dataFunctions.cpp
	src/serial.cpp
	${IMGUI_SOURCES}
	${TEENSY_BACKEND_DIR}/datatypes.cpp
	${IMGUI_BACKENDS}
)

add_executable(alive_interface ${ALIVE_SOURCES})
set_property(TARGET alive_interface PROPERTY CXX_STANDARD 17)
target_compile_options(alive_interface PRIVATE -Wall -Wformat)
target_link_libraries(alive_interface
	Threads::Threads
	glfw
	OpenGL::GL
	${CMAKE_DL_LIBS}
	stdc++fs
)

# ${CMAKE_DL_LIBS} is needed for dlopen() and dlclose()

# stdc++fs is needed for things in the std::experimental::filesystem namespace,
# at least for GCC < 9.1 and LLVM < 9.0, but maybe not on Mac, so maybe linking
# with that actually needs to be conditional.
# See the 'Notes' at https://en.cppreference.com/w/cpp/filesystem and also 
# https://discourse.cmake.org/t/correct-way-to-link-std-filesystem-with-gcc-8/4121
