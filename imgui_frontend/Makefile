# ------------------------------------------------------------------------------------------------
# Cross Platform Makefile. Compatible with MSYS2/MINGW, Ubuntu 14.04.1 and Mac OS X
# You will need to install GLFW (http://www.glfw.org)
#
# @author Matteo Cartiglia <camatteo@ini.uzh.ch>
# ------------------------------------------------------------------------------------------------

#---------------------------------------------------------------------
# MACRO DEFINITIONS
#---------------------------------------------------------------------
IMGUI_DIR = imgui/imgui_src
BACKENDS_DIR = imgui/imgui_backend
SOURCE_DIR = src
OBJ_DIR = src/.obj
ODIR_PATH = src/.obj/
TEENSY_BACKEND_DIR = ../teensy_backend/src
EXE = alive_interface

UNAME_S := $(shell uname -s)
LINUX_GL_LIBS = -lGL

#---------------------------------------------------------------------
# INPUT SOURCES
#---------------------------------------------------------------------

SOURCES +=  $(SOURCE_DIR)/main.cpp \
			$(SOURCE_DIR)/guiFunctions.cpp \
			$(SOURCE_DIR)/dataFunctions.cpp \
			$(SOURCE_DIR)/serial.cpp \
			$(IMGUI_DIR)/imgui.cpp \
			$(IMGUI_DIR)/imgui_demo.cpp \
			$(IMGUI_DIR)/imgui_draw.cpp \
			$(IMGUI_DIR)/imgui_tables.cpp \
			$(IMGUI_DIR)/imgui_widgets.cpp \
			$(IMGUI_DIR)/implot.cpp \
			$(IMGUI_DIR)/implot_items.cpp \
			$(IMGUI_DIR)/implot_demo.cpp \
			$(TEENSY_BACKEND_DIR)/datatypes.cpp \
			$(BACKENDS_DIR)/imgui_impl_glfw.cpp \
			$(BACKENDS_DIR)/imgui_impl_opengl3.cpp \
			
# OBJS = $(addsuffix .o, $(basename $(notdir $(SOURCES))))
OBJS = $(addprefix $(ODIR_PATH), $(notdir $(SOURCES:.cpp=.o)))

#---------------------------------------------------------------------
# COMMON BUILD FLAGS
#---------------------------------------------------------------------

CXXFLAGS = -std=c++17 -I/imgui -I/backends -g -Wall -Wformat 

#---------------------------------------------------------------------
# BUILD FLAGS PER PLATFORM
#---------------------------------------------------------------------

# LINUX
ifeq ($(UNAME_S), Linux)
	ECHO_MESSAGE = "Linux"
	LIBS += $(LINUX_GL_LIBS) `pkg-config --static --libs glfw3`
	LIBS += -pthread

	CXXFLAGS += `pkg-config --cflags glfw3` -lstdc++fs
	CFLAGS = $(CXXFLAGS)
endif

# APPLE
ifeq ($(UNAME_S), Darwin)
	ECHO_MESSAGE = "Mac OS X"
	LIBS += -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
	LIBS += -L/usr/local/lib -L/opt/local/lib -L/opt/homebrew/lib 
	LIBS += -lglfw
	CXXFLAGS += -I/usr/local/include -I/opt/local/include -I/opt/homebrew/include
	CFLAGS = $(CXXFLAGS)
endif

#---------------------------------------------------------------------
# RELEASE BUILD RULES
#---------------------------------------------------------------------

$(OBJ_DIR)/%.o: $(SOURCE_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OBJ_DIR)/%.o: $(IMGUI_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OBJ_DIR)/%.o: $(BACKENDS_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OBJ_DIR)/%.o: $(TEENSY_BACKEND_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

all: $(EXE)
	@echo Build complete for $(ECHO_MESSAGE)

$(EXE): $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)


#---------------------------------------------------------------------
# CLEAN RULES
#---------------------------------------------------------------------

clean:
	rm -f $(EXE) $(OBJS)
