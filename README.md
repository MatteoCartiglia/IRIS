# Overview

This project contains the resources necessary to interface the custom VLSI chip "ALIVE" with a Teensy 4.1 board (https://www.pjrc.com/store/teensy40.html).

The `imgui_frontend` directory contains the source code to run an intuitive GUI to:
- control the Bias Generator and DAC voltages;
- display and record live events (i.e. neuron firing & C2F current).

The `teensy_backend` directory contains a shared constants file and the source code to be uploaded to the Teensy 4.1 board to:
- interface between the ALIVE chip and the GUI displayed on the PC.


## Frontend

The frontend runs using the 'Dear ImGui' (https://github.com/ocornut/imgui) C++ interface with GLFW (http://www.glfw.org) and OpenGL (https://www.opengl.org/) backends. The relevant files of ImGui and OpenGL are included in this repository, however, you will need to install GLFW which can be done as follows:

#### Linux:
    # apt-get install libglfw3-dev
#### Mac OS X:
    # brew install glfw
#### MSYS2:
    # pacman -S --noconfirm --needed mingw-w64-x86_64-toolchain mingw-w64-x86_64-glfw


## Backend

This code is compiled and uploaded using "PlatformIO" plugin on Visual Studio Code.


## Getting Started

1. Install the required dependencies (GLFW, platformIO plugin)
2. Connect the Teensy to the PC and check how it appears in the /dev directory. You can do this by running `ls /dev`. It will typically appear as `ttyACM0`, however it may also appear under another name, depending on your machine. If it does not appear as `ttyACM0`, update the `SERIAl_PORT_NAME` variable in the `teensy_backend/include/constants.h` file.
3. Upload the backend code to Teensy using the PlatformIO plugin.
4. In the `imgui_frontend` directory, run the shell script `clean_build_ruh.sh`. This will build and run an `alive_interface` executable.


## To-Do:

- Fix bug with BiasGen initialisation.
- Fix bug with editing filename when saving.
- Fix bug with BiasGen loading bias values from file.
