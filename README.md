# haluj
## Description
This project is an experimental bare-metal framework for microcontrollers. It provides some primitive constructs to help developing asynchronous applications. It is written in C++ and uses some benefits of the recent standard features. 

Beware, since the project is in early development stage, all the information may subject to change

## Usage
Requirements;
- gcc cross compiler for bare-metal ARM targets. It can be downloaded from the address "https://developer.arm.com/open-source/gnu-toolchain/gnu-rm/downloads"
- "make" tool

Prior to build, the parameters listed below in k60d10.defs (under build folder) file, is required to be edited,

- **TOOLCHAIN_DIR**: gcc cross compiler folder including the bin folder (E.g: TOOLCHAIN_DIR = /wherever_it_is/gcc-arm-none-eabi-5_4-2016q3/bin)
- **TOOLCHAIN_PREFIX**: target prefix for cross compiler (E.g: TOOLCHAIN_PREFIX = arm-none-eabi-)

Framework usage examples are provided and can be found under appropriate folder. 

To build the gpio example, change the current directory to build
```
..>cd build
```
and then execute
```
..>make DEFS=k60d10.defs NAME=gpio-example MAIN_DIR=examples/gpio
```
Other examples can be build similarly.

The outputs (.bin and .elf) are produced in a folder with "out" prefix. 

## Supported Devices

Currently only NXP's Kinetis MK60D10 family devices are supported. 

It is better consumed with yoghurt :)
