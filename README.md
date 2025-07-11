# rome
A 3D general-purpose game engine for Linux and Unix-like distributions.

## Motivation
This engine is a full rewrite of my [**Cobalt engine**](https://github.com/teoemeaesse/cobalt-engine), improving on the features that were lacking in it and doubling down on a plugin-based approach.

## System Requirements
To compile and run you need the following:
- **CMake 3.14**
- **Clang** or **GCC**

You will also need the following dependencies on Linux to build SDL:
```
libx11-dev libxrandr-dev
```


## Installation 
Before anything else, run the setup script. This will get and install all the necessary dependencies.
```
./configure.sh
```

## Build
```
./build.sh
```
To build the tests, run the script with the ```-t``` flag

## Engine structure
**rome** is divided into two main parts:
- [**core**](https://github.com/teoemeaesse/rome/tree/main/core) - Core engine static library, handles everything from memory metrics to the ECS.
- [**engine**](https://github.com/teoemeaesse/rome/tree/main/engine) - **rome** standard plugins that constitute the bulk of the engine - graphics, user input, audio etc.
