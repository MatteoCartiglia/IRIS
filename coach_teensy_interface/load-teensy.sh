#!/usr/bin/env bash

# In order to use this, get the teensy_loader_cli source (>= version 2.2) from
# https://github.com/PaulStoffregen/teensy_loader_cli
# On Ubuntu, install the libusb-dev package, then make teensy_loader_cli and
# install it in e.g. /usr/bin.
# There is no install target in the teensy_loader_cli Makefile, so the install
# has to done manually, e.g. using 'install -p teensy_loader_cli /usr/bin/'.
# Also download 00-teensy.rules from https://www.pjrc.com/teensy/00-teensy.rules and install:
# 'install -p 00-teensy.rules /etc/udev/rules.d/'.
# Reload the udev rules with 'udevadm control --reload'.

if teensy_loader_cli --mcu=TEENSY40 -v -w ${1:-build/teensy/main.hex}; then
	echo Done!
fi
