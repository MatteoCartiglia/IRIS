#!/bin/bash

for PY in /opt/python/*; do
    rm -R CMakeCache.txt CMakeFiles
    cmake -DCMAKE_PREFIX_PATH="${PY}" ..
    make CMAKE_PREFIX_PATH="${PY}" -j$(nproc --all) buildwheel
done
