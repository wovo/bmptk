# bmptk

file: bmptk/README.md

Copyright (c) 2012 .. 2021 
- Wouter van Ooijen (wouter@voti.nl)
- Carlos van Rooijen
- Oscar Kromhout
- (and probably many other contributors over the years)

Bmptk itself is distributed under the Boost Software License, 
Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at 
http://www.boost.org/LICENSE_1_0.txt)

This is the bmptk (Bare Metal Programming Tool Kit) main directory.
Bmptk is a minimalist make-based development environment for 
small micro-controllers using GCC C, C++, or assembler on 
Windows and Linux.

## files and directories

This directory contains :
- files
   - bmptk.h            : top-level header file for bmptk
   - license.txt        : license (refers to the boost license)
   
   - license_1_0.txt    : boost license
   - Makefile           : makes ao. doxygen documentation 
   - Makefile.local     : locations of the external tools (colne and edit)
   - Makefile.inc       : included by the project makefile
   - Makefile.subdirs   : included by a makfile to recurse subdirectories
   - README.md          : this file
- subdirectories
   - targets            : chip and board specific stuff
   - tools              : tools (executables, scripts)
   
## features

For each target, bmptk provides
- startup code, including stack setup
- commands to compiler, link, download and run, and open a serial terminal
- device header files (in most cases from the manufacturer) for the memory 
mapped special registers

The heap is by default disabled: using heap functions will generate an 
exaplanatory linker error.

For C++, global constructor calls (ctors) are also disabled, and
will generate an exaplanatory linker error.

Most tools (toolchain, downloaders) are not part of bmptk
and must be installed separately. 
Installation instructions for Ubuntu can be found at
https://www.github.com/wovo/ubuntu-toolchains

## target support

Actively supported targets chips and boards are:
- native (windows or linux)
- atmega328: Arduino Uno, Arduino Nano, my-first-devboard
- lpc1114fn28: DB103
- atsam3x8e: Arduino Due
- stm32f103c8: Blue Pill, termite
- mimxrt1062dvl6a: Teensy 4.0

## akefile 

The development environment uses make and makescipt(s) to do the work.
The main makescipt is Makescript.inc. 
If it exists it includes Makescript.custom, or else Makescript.local.
The makefile for a project must set a few variables, and then include
(directly, or via a chain of includes) the bmptk/Makescript.inc.

As a minimum, a project makefile must set the target the project has
to be built for. 
```make
TARGET := arduino_uno
```
    
Valid targets are either bare chips: 
- atmega328
- lpc1114fn28
- sam3x8e
- stm32f103c8
- mimxrt1062dvl6a

or boards: 
- arduino_uno
- arduino_nano
- my_first_dev_board
- db103
- arduino_due
- blue_pill, termite
- teensy_40
Additionally, the target native can be used to produce a native executable.

The name of the project can be set.
If not set, the default is main.
The project name determines the base name of the various 
files tat are build like .exe, .bin or .hex.
```make
PROJECT := main
```
    
A project contains at least on source file.
The base name of this file must match the PROJECT name, 
and its extension must be .asm, .c, or .cpp.    
Additional source files can be specified by setting SOURCES:
```make
SOURCES := interface.cpp
```
    
The header files can be specified by setting HEADERS:
Additional source files can be specified by setting SOURCES:
```make
HEADERS := interface.hpp
```
    
The bmptk makefile uses a very simple rebuild principle: 
it assumes all SOURCES depend on all HEADERS.
This is conservative, but it ensures that the sources that
must be compiled are (provided that the HEADERS are set correctly).

By default, bmptk assumes that all sources and headers are in
the project directory (where the make command is run).   
Additional locations where the sources and headers can reside
can be specified by setting SEARCH:
```make
SOURCES := local_library ../shared_library
```
    
By default, the files that are needed will be produced.    
You can specify extra results to be produced,
like .lst files for a source file, or the .lss file
for the project. 
```make
RESULTS := main.lst main.lss
```
    
Some programmers use a serial-over-USB port for downloading.
On linux, the serial port to use can generally be deduced
automatically. 
On windows, or when you must specify the serial port, you
can set the SERIAL_PORT:
```make
SERIAL_PORT := COM4
```
    
For most targets, after the the project has been build,
downloaded and started, a terminal window
is started with a serial connection to the target. 
This connection uses the SERIAL_PORT at the SERIAL_BAUDRATE,
which (if not set) defaults to 38400 baud.
You can prevent the terminal window from being started by 
setting RUN_TERMINAL to empty:
```make
RUN_TERMINAL :=
```    
