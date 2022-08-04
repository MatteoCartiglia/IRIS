# Overview

This project contains the resources necessary to interface a the custom VLSI chip "ALIVE" with a Teensy 4.1 board (https://www.pjrc.com/store/teensy40.html).

The `imgui_frontend` folder contains the source code to run an intuitive GUI to:
- control the Bias Generator and DAC voltages
- display and record live events (i.e. neuron firing & C2F current)

The `teesy_backend` folder contains a shared constants file and the source code to be uploaded to the Teensy 4.1 board to:
- interface between the ALIVE chip and the GUI displayed on the PC.


## Frontend

The frontend runs using the 'Dear ImGui' (https://github.com/ocornut/imgui) C++ interface with GLFW (http://www.glfw.org) and OpenGL (https://www.opengl.org/) backends. The relevant files of ImGui and OpenGL are included in this repository however you will need to install GLFW which can be done as follows:

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
2. Connect the Teensy to the PC and check how it appears in the /dev/ directory. You can do this by running `ls /dev/`. It will typically appear as "ttyACM0", however it may also appear under another name, depending on your machine. If it does not appear as "ttyACM0", update the SERIAl_PORT_NAME variable in the `teensy_backend/include/constants.h` file.
3. Upload the backend code to Teensy using the PlatformIO plugin.
4. In the `imgui_frontend` directory, run the shell script `clean_build_ruh.sh`. This will create an `alive_interface` executable.


## To-Do:

- Option to Save & load new bias values. Need to upload to the chip all the biases if they are not the POR value. (Same as DAC init)
- Biasgen configs: add an "other" tab, to catch the ones that dont have that prefix
- Add the option to upload a file to input interface (Decoder). The file should
be a .csv with 2 columns: ISI(inter-spike-interval) (in ms), input bit patten
  - Send all the data to the teensy
  - Have a "timer" subthread that will stimulate the chip when the ISI is over
- Add a first req/ack to get the handshaking started. Sometimes it starts already
in a hung state --> if chip response is wrong

- Remove ENCODER/C2F/DECODER_Bit_X structure: does not allow for generability. What if in the future we have a different number of bits?

- Fix make file: launch only one thread and make the arduino thread a slave
- Include a heartbeat signal


## Done:

- Tested DAC
- Fixed and tested biasgen
- Added and tested SPI interfaces
- Tested input interface (decoder)
