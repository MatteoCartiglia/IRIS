## Overview

This project contains the resources necessary to interface a custom VLSI chip (ALIVE/SPAIC/AHCP.. etc) with a  [Teensy 4.1] board (https://www.pjrc.com/store/teensy40.html).

The `imgui_frontend` folder contains the codes to run an intuitive GUI to control SPI's and record events.

The `teesy_backend` folder contains the code that needs to be uploaded to the Teensy.

## To Build the Software

# Frontend

The frontend runs using the 'dear imgui' (https://github.com/ocornut/imgui) c++ interface with opengl3.

Installations of this package is required. Many tutorials exist and I point you to those.

# Backned

This code is compiled using platformio plugin on visual studio code. However direct upload is also possible.


### To-Do:
- Input interfaces
