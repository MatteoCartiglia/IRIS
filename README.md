# Overview

This project contains the resources necessary to interface a custom VLSI chip (ALIVE/SPAIC/AHCP.. etc) with a  [Teensy 4.1] board (https://www.pjrc.com/store/teensy40.html).

The `imgui_frontend` folder contains the codes to run an intuitive GUI to control SPI's and record events.

The `teesy_backend` folder contains the code that needs to be uploaded to the Teensy.

## Packages

### Frontend

The frontend runs using the 'dear imgui' (https://github.com/ocornut/imgui) c++ interface with opengl3.

Files of imGui are included in this repository so there is no requirement of specific installations of this package.

However if you need to debug I recommend to install and run the gitlab repo. Many tutorials exist and I point you to those.

Compiler: c++11

### Backned

This code is compiled using platformIO plugin on visual studio code. However direct upload is also possible.

## Running the code
### Frontend

Compile the make file in `imgui_frontend/src/src` and then run the compiled executable file.
```
cd imgui_frontend/src/src
make
./main
```

### Frontend

This code is compiled using platformio plugin on visual studio code. However direct upload is also possible.

### To-Do:

- AER interface
- Input interfaces
