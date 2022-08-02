## Overview

This project contains the resources necessary to use a [Teensy 4.0](https://www.pjrc.com/store/teensy40.html) to interface with the CoACH neuromorphic classchip via the PLANE board.

The `datasheets` folder contains datasheets for the chips used on the PCB.

The `kicad` folder contains the schematic and layout of the PCB.

The `src/config` folder contains a file used by CMake to provide version number macros.

The `src/pc` folder contains code to build a Python extension module (`pyplane`) through which the PLANE board and CoACH chip can be controlled from Python.

The `src/teensy` folder contains the code that needs to be uploaded to the Teensy.

The CoACH chip itself is documented in the [CoACH-docs project](https://code.ini.uzh.ch/CoACH/CoACH-docs).

## To Build the Software

### Prerequisites
* Linux, preferably Ubuntu
* Arduino software
* Teensyduino
* `teensy_loader_cli` from https://github.com/PaulStoffregen/teensy_loader_cli - see comments in [load-teensy.sh](load-teensy.sh) for detailed build and install instructions
* pybind11: https://pybind11.readthedocs.io - see https://pybind11.readthedocs.io/en/stable/installing.html#include-with-pypi for installation instructions (Hint: you probably want to use the `pip install "pybind11[global]"` method in order to get the pybind11 CMake files, but this will depend on your environment)

The Arduino software and Teensyarduino can be installed as follows:

```
cd /parent/dir/for/arduino/software
wget https://downloads.arduino.cc/arduino-1.8.13-linux64.tar.xz
wget https://www.pjrc.com/teensy/td_153/TeensyduinoInstall.linux64
tar -xf arduino-1.8.13-linux64.tar.xz
chmod +x TeensyduinoInstall.linux64
./TeensyduinoInstall.linux64 --dir=arduino-1.8.13
cd arduino-1.8.13/hardware/teensy/avr/cores/teensy4
make main.elf
```

Once you have all the prerequisites installed, you can clone this project and proceed to build it as follows.

### Command Line Build

In the `CoACH_teensy_interface` directory, do the following (replacing `/parent/dir/for/arduino/software/arduino-1.8.13` with the actual directory you installed the Arduino software in, obviously).

```
mkdir build && cd build
cmake -DARDUINOPATH=/parent/dir/for/arduino/software/arduino-1.8.13 ../src
make -j
```

The build produces
* `teensy/main.hex` which can be uploaded to the Teensy using the `load-teensy.sh` script, which in turn uses `teensy_loader_cli`.
* The Python extension module e.g. `pc/pyplane/pyplane.cpython-36m-x86_64-linux-gnu.so` which needs to be on the `PYTHONPATH` in order to be importable from Python.

### Building the Teensy Code using an IDE

An IDE can be used to take care of compiling and uploading the code to the Teensy. For Visual Studio Code, the [PlatformIO](https://platformio.org/) extension can be used for this. To do this with the Arduino IDE, follow [these](https://www.pjrc.com/teensy/td_download.html) instructions.

If Visual Studio Code is to be used, create a new project from the PlatformIO home screen. Choose Teensy 4.0 as the board and Arduino as the framework. Then copy everything in `teensy/*` to the `src/*` folder of the newly created project.

If the Arduino IDE is to be used, rename `teensy/main.cpp` to `teensy/teensy.ino`.
