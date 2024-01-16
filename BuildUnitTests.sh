#!/usr/bin/bash

if [ ! -d "build" ]; then
    mkdir build
fi
cd build
cmake ..
make
cd ..
./build/remote_led_functions
