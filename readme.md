# Playdate-C++

## Building for simulator
1. mkdir build
2. cd build
3. cmake ..
4. make

## Building for playdate
1. mkdir build
2. cd build
3. cmake -DCMAKE_TOOLCHAIN_FILE=/Users/sergio.prada/Developer/ ..PlaydateSDK/C_API/buildsupport/arm.cmake
4. make