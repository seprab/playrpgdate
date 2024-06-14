#!/bin/bash

# Navigate to the project root directory
cd "$(dirname "$0")"

# Create and navigate to the build directory
mkdir -p build
cd build

# Run CMake to configure the project for the Playdate device
# Make sure to replace the path to the arm.cmake file with the correct one if necessary
cmake -DCMAKE_TOOLCHAIN_FILE=/Users/sergio.prada/Developer/PlaydateSDK/C_API/buildsupport/arm.cmake ..

# Build the project
make