find_package(Threads REQUIRED)
find_package(glfw3 REQUIRED)
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

