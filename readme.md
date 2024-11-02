# CardoBlast

Attempting to create an text-based RPG for the playdate using c++.

## How to build
I work with this either with Terminal or CLion

### Terminal
Add execution permission to bootstrap with chmod +x bootstrap.sh, and launch it. Then choose the option that works for you.

### CLion
1. Open the project in CLion
2. Open Run/Debug Configurations
3. Add a new configuration
- Name: CardoBlast
- Target: CardoBlast
- Executable: (~/Developer/PlaydateSDK/bin/Playdate Simulator.app/Contents/MacOS/Playdate Simulator)
- Program arguments: <ProjectDir>/CardoBlast.pdx
4. Go to Clion Preferences -> Build, Execution, Deployment -> CMake
5. Add two profiles
6. "Simulator"
- Name: Simulator
- Build type: Debug
- CMake options: -DPROJECT_NAME=CardoBlast
- Build directory: <ProjectDir>/build/simulator
7. "Device"
- Name: Device
- Build type: Debug
- CMake options: -DPROJECT_NAME="CardoBlast" -DCMAKE_TOOLCHAIN_FILE=/Users/sergio.prada/Developer/PlaydateSDK/C_API/buildsupport/arm.cmake
- Build directory: <ProjectDir>/build/device


## Credits
- [UtilForever RPG](https://github.com/utilForever/SimpleRPG-Text/tree/master).
- [Playdate-cpp](https://github.com/nstbayless/playdate-cpp)
- [Playdate SDK](https://play.date/)
- [JSMN - json parser](https://github.com/zserge/jsmn)

## Disclaimer

This repository incorporates the submodule [playdate-cpp](https://github.com/nstbayless/playdate-cpp). Please refer to its README for guidance on configuration and setup. This project has been developed and tested on a Mac M1, running Sonoma 14.15.

## Additional requirements to the official guide
- PLaydate SDK path Environment variable is required!

## Troubleshooting

### Linker Issue on Mac

**Problem Description:**

When compiling, you might encounter a linker error similar to the following:
```
ld: error: .data has both ordered ['.ARM.exidx.text.__aeabi_atexit' ...] and unordered ['.data.__atexit_recursive_mutex' ...] sections.
```

This issue is discussed in detail in this [Playdate Developer Forum thread](https://devforum.play.date/t/cpp-guide-c-on-playdate/5085/39).

**Solution:**

The resolution involves updating to the latest ARM toolchain and creating symbolic links in `/usr/local/bin`. This solution is based on the advice provided in [this forum post](https://devforum.play.date/t/cpp-guide-c-on-playdate/5085/40).

To apply the fix, execute the following command in the Terminal:

```
sudo ln -sf /Applications/ARM/bin/* /usr/local/bin
```
This command creates symbolic links for the ARM toolchain binaries in /usr/local/bin, ensuring that the build process can correctly locate and use these tools.
