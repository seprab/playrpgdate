#!/bin/bash

# Navigate to the project root directory
cd "$(dirname "$0")"

# Create and navigate to the build directory
mkdir -p build
cd build

# Run CMake to configure the project for the simulator
cmake ..

# Build the project
make