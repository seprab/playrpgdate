#!/bin/bash
export PROJECT_NAME="CardoBlast"


echo "Select build option:"
echo "1) Build for Playdate device"
echo "2) Build for simulator"
echo "3) Clean build directory"
read -p "Option (1-3): " option

# Navigate to the project root directory
cd "$(dirname "$0")"

clean_build_cache() {
    # Check if the "build" directory exists before attempting to remove it
    if [ -d "build" ]; then
        rm -rf build
        echo "Build directory removed."
    fi

    # Check if the "source" directory exists before attempting to remove it
    if [ -d "source" ]; then
        rm -rf source
        echo "Source directory removed."
    fi

    # Check if the "HelloWorld.pdx" file exists before attempting to remove it
    if [ -d "${PROJECT_NAME}.pdx" ]; then
        rm -rf ${PROJECT_NAME}.pdx
        echo "HelloWorld.pdx file removed."
    else
        echo "File ${PROJECT_NAME}.pdx does not exist."
    fi
    }

case $option in
    1)
        # Clean previous build cache
        clean_build_cache
        # Build for Playdate device
        mkdir -p build
        echo "Configuring the project, specifying the source and build directories, and sets the toolchain file for cross-compilation."
        cmake -S . -B build -DPROJECT_NAME=$PROJECT_NAME -DCMAKE_TOOLCHAIN_FILE=/Users/sergio.prada/Developer/PlaydateSDK/C_API/buildsupport/arm.cmake
        echo "Compiling the project using the configuration specified in the build directory."
        # using this command instead of "make" to avoid moving to another directory
        cmake --build build
        # echo "Running the executable."
        # open -a "${PLAYDATE_SDK_PATH}/bin/Playdate Simulator.app" --args ${PROJECT_NAME}.pdx
        ;;
    2)
        # Clean previous build cache
        clean_build_cache
        # Build for simulator
        mkdir -p build
        echo "Configuring the project, specifying the source and build directories, and sets the toolchain file for cross-compilation."
        cmake -S . -B build -DPROJECT_NAME=$PROJECT_NAME
        echo "Compiling the project using the configuration specified in the build directory."
        # using this command instead of "make" to avoid moving to another directory
        cmake --build build
        # echo "Running the executable."
        # open -a "${PLAYDATE_SDK_PATH}/bin/Playdate Simulator.app" --args ${PROJECT_NAME}.pdx
        ;;
    3)
        # Clean previous build cache
        clean_build_cache
        ;;
    *)
        echo "Invalid option."
        ;;
esac