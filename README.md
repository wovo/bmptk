# bmptk

file: bmptk/README.md

Copyright (c) 2012 .. 2107 Wouter van Ooijen (wouter@voti.nl)

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at 
http://www.boost.org/LICENSE_1_0.txt)

This is the bmptk (Bare Metal Programming Tool Kit) main directory.
Bmptk is a minimalist make-based development environment for 
small micro-controllers using GCC C, C++, or assembler on Windows
(with limited support for Linux).

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