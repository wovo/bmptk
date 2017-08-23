#############################################################################
# 
# Makefile for bmptk zip file, etc.
#
# Copyright (c) 2012 .. 2017 Wouter van Ooijen (www.voti.nl)
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at 
# http://www.boost.org/LICENSE_1_0.txt)
#
# This file makes some meta-stuff that is probably 
# interesting only for me as bmptk maintainer.
#
# targets:
#    clean                  : total cleanup 
#    build, examples        : build all examples
#    zip                    : create bmptk.zip, includes documentation
#    www                    : create webpages (www subdir, includes zip)
#    push                   : clean, git commit -a, git push 
#    ftp                    : create webpages and ftp to website
#                             (won't work from your PC ;) )
#
# The commit and ftp targets won't work on your PC because they require
# tools and/or passwords that are outside the bmptk tree.
# 
#############################################################################

ifeq ($(OS),Windows_NT)
   # WPF is the windows prefix for 32-bit applications
   ifeq ($(wildcard C:/Program\ Files\ (x86)/* ),)
      WPF := C:/Program Files
   else
      WPF := C:/Program Files (x86)
   endif
   REMOVE = bmptk-rm
else
   # linux
   REMOVE = rm -rf
endif

ifneq ($(wildcard Makefile.custom),)
   include makefile.custom
else   
   include makefile.local
endif

.PHONY: clean doc docs documentation build all examples 
.PHONY: zip commit tests webpage

all: examples 
build: examples 

examples:
	cd examples && make build
	@echo "**** examples build completed succesfully"

tests:
	cd tests && make build
	@echo "**** tests build completed succesfully"

zip: clean	 
	$(7za) a -tzip bmptk.zip .
	@echo "**** zip build completed succesfully"
   
webpage:
	$(REMOVE) -rf transfer
	mkdir transfer
	-cp ../_ftp_crc_s transfer
	cp webpage/* transfer
	python \ftpbmptk.py
	cp transfer/_ftp_crc_s ..
	@echo "**** ftp transfer completed succesfully"
   
	# cd examples && make clean   
clean:
	$(REMOVE) -rf index.html docs bmptk.zip transfer
		
	@echo "**** cleanup completed succesfully"
		
push: 
	make clean
	git add -A
	git commit -a -m "update"
	git push

      
		
