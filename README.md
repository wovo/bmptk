# bmptk

file: bmptk/README.md

Copyright (c) 2012 .. 2107 Wouter van Ooijen (wouter@voti.nl)

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at 
http://www.boost.org/LICENSE_1_0.txt)

This is the bmptk (Bare Metal Programming Tool Kit) main directory.
Bmptk is a minimalist make-based development environment for 
small micro-controllers using GCC C, C++, or assembler on 
Windows (tested on W10) and Linux (tested on Lubuntu).

This directory contains :

   files
      - bmptk.h            : top-level header file for bmptk
      - license.txt        : license (refers to the boost license)
      - license_1_0.txt    : boost license
	   - Makefile           : makes ao. doxygen documentation 
      - Makefile.local     : locations of the external tools (colne and edit)
      - Makefile.inc       : included by the project makefile
      - Makefile.subdirs   : included by a makfile to recurse subdirectories
      - README.md          : this file

   subdirectories
      - targets            : chip and board specific stuff
      - tools              : tools (executables, scripts)
      
=============================================================================

Linux (lubuntu) notes:

setup on Windows:
* get vmware player - free for non-commercial use
* get lubuntu from http://www.momotrade.com/tool/vm/lubuntu1604t.html
* run and login as root:password
* right-click desktop and open terminal
* might need vm->removable devices->connect to connect an USB device
* uno sometimes needs re-plugging

get packages:
* sudo apt-get update
* sudo apt-get install git
* sudo apt-get install arm-none-eabi
* sudo apt-get install bossa-cli
* sudo apt-get install doxygen
* sudo apt-get install gcc-avr
* sudo apt-get install avr-libc
* sudo apt-get install avrdude
* sudo apt-get install build-essential

get bmptk etc:
* git clone http://www.github.com/wovo/bmptk
* git clone http://www.github.com/wovo/hwlib
* git clone http://www.github.com/philsquared/Catch

build tool:
* go to bmptk/tools/lpc21isp_197 && make build (again after clean...)

to run: 
* plug the due in => vm->removable devices->connect
* you must run as root (sudo make run) or have serial port permission

todo:
* ST
* MCP430
* avrdude-usbasp
* ESP82366
* use -Wextra?
* Arduino Uno doesn't barf on ctors!
* compile bossac on linux
