
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

- Finish testing the Save & load new bias values on DAC/BIASGEN:
  - Need to check with multimeter all is correct when uploading the DAC values
  - Fix the uploading of the Biasgen values. Visualization is correct but values don't get updated
  - when saving DAC values, the values 10/11 get saved wrongly (getDACvalues) (ASCII encoding problem)

- Biasgen configs: add an "other" tab, to catch the ones that dont have predifined prefix

- Add the option to upload a file to input interface (Decoder). The file should
be a .csv with 2 columns: ISI(inter-spike-interval) (in ms), input bit patten
  - AER_DECODER_OUTPUT_command will need a isi variable
  - Have a "timer" subthread that will stimulate the chip when the ISI is over
  [i think best to have it on the cpu side, since we have <thread> and more computing power]

- Fix communication of events between backend and frontend:
  - Make a togglable button for encoder and c2f events logging
  - When enabled save the events in an array in the teensy
  - Array of events with data + timestamp of when the event was acknoledged
  - When array is not empty, send them to the cpu in the default case
  - send them to the cpu
  - unpackage and visualize them in the cpu
  - add saving of the events: save to .csv (or .dat) similar to decoder. 2 columns:
  neuron-id, timestamp of the events

- Handshaking protocol:
  - Some weird things are happening on the encoder/c2f. Not working correctly.
    Sometimes it starts already in a hung state. Interrupt on "CHANGE" so if no change
    there is no ack.
  - Need to check that the active-high/active-low paradigm is working. For that
  put 2 random pins as req/ack on the board without the chip. Define as active high the
  handshaking and short the req to vdd observe on the scope both Req/Ack should
  should be easy to debug. Viceversa for active-low.
  - Add a button to manually force a handshake. This helps get it started if it is hung

- Include a heartbeat signal
- Make reset button to reset the whole chip
- Remove redundancy in SPI windows
- Fix make file: launch only one thread and make the backend thread a slave
- what are ENCODER_INPUT_command, C2F_INPUT_command?

## Done:

- Tested DAC
- Fixed and tested biasgen
- Added and tested SPI interfaces
- Tested AER of input interface (decoder) and addressing
