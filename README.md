# Dactyl Pteron Keyboard

This keyboard is a fork of the [dactyl-cc](https://github.com/mjohns/dactyl-cc) that is attempting to imitate the
  [pteron](https://github.com/FSund/pteron-keyboard) keyboard.
 Major thanks to Matthew Adereth, Tom Short, Michael Johns, and Filip Sund for their work.

This is still a work in progress, so stay tuned for updates.

All the work has been done for me (and you). Quick table of contents:
src/key_data.h/cc: The key layout
src/dactyl.cc: The shape of the keyboard, using the key layout as a starting point
src/util: How keys are defined, as well as a gorgeous wrapper for creating complex OpenSCAD files

CMake is the preferred way to build and leads to the fastest recompilation times. You only need to run the cmake command once.
```
cd build
cmake ../src
make && ./dactyl
```

If you do not have cmake installed you can run the simple build script which just uses g++.
```
cd build
./build_simple.sh
```

You can generate an stl from the command line with the following command:
```
cd build
openscad -o ../things/v1_left.stl v1_left.scad
```
