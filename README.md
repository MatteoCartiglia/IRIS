# Overview

This project contains the resources necessary to interface a the custom VLSI chip "ALIVE" with a Teensy 4.1 board (https://www.pjrc.com/store/teensy40.html).

The `imgui_frontend` folder contains the source code to run an intuitive GUI to:
- control the Bias Generator and DAC
- display and record live events.

The `teesy_backend` folder contains the source code to be uploaded to the Teensy 4.1 board to interface between the ALIVE chip and the GUI displayed on the PC. It also contains a shared "constants" file. 
- 

### Frontend

The frontend runs using the 'dear imgui' (https://github.com/ocornut/imgui) C++ interface with opengl3. The relevant files of imGui are included in this repository so there is no requirement of specific installations of this package. However, you will need to installGLFW (http://www.glfw.org) which can be done as follows:

#### Linux:
    - apt-get install libglfw-dev
#### Mac OS X:
    - brew install glfw
#### MSYS2:
    - pacman -S --noconfirm --needed mingw-w64-x86_64-toolchain mingw-w64-x86_64-glfw


### Backend

This code is compiled using "platformIO" plugin on visual studio code. However direct upload is also possible.


## Getting Started

1. IN

### Frontend

Compile the make file in `imgui_frontend/src/src` and then run the compiled executable file.
```
cd imgui_frontend/src/src
make
./main
```


### To-Do:

- AER interface
- Input interfaces
