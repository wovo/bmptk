#############################################################################
#
# file: bmptk/Makefile.inc
#
# makefile 'body' for bare metal C / C++ / assembler projects using GCC
# and some related things
#
# This file is meant to be included by a project-specific makefile.
#
# Copyright (c) 2012 .. 2017 Wouter van Ooijen (wouter@voti.nl)
# 
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at 
# http://www.boost.org/LICENSE_1_0.txt)
# 
#############################################################################


# This string must contain neither spaces (problems on windows8)
# nor periods (problems on xtensa)
VERSION := V04_00_work_in_progress_2017_01_23


#============================================================================
#
# Invoke the Makefile.custom (if it exists) or else the Makefile.local,
# to get the locations of the external toolsets
# 
#============================================================================

ifeq ($(OS),Windows_NT)
   # WPF32 is the windows prefix for 32-bit applications
   # Who at Microsoft got this 'bright' idea to make the NEW directory
   # the default??
   ifeq ($(wildcard C:/Program\ Files\ (x86)/* ),)
      WPF32 := C:/Program Files
   else
      WPF32 := C:/Program Files (x86)
   endif
   SUDO ?= 
else
   SUDO ?= sudo
endif

ifneq ($(wildcard $(BMPTK)/Makefile.custom),)
   include $(BMPTK)/Makefile.custom
else   
   include $(BMPTK)/Makefile.local
endif


#============================================================================
#
# For windows these executables are distributed with bmptk,
# or must be installed separately.
#
# For another platform you will have to compile and/or install them 
# or find a suitable replacement.
# 
#============================================================================

ifeq ($(OS),Windows_NT)

   # seems to be required in some circumstances
   SHELL = C:/Windows/System32/cmd.exe
   #SHELL = cmd.exe
   #export PATH=%PATH%;C:/Windows/System32;C:/Windows/SysWOW64
   
   # distributed with bmptk 
   MAKE          ?= "bmptk-make.exe"
   REMOVE        ?= "$(BMPTK)/tools/bmptk-rm.exe" -f
   #COPY          ?= "$(BMPTK)/tools/bmptk-cp.exe" -f
   COPY          ?= "$(BMPTK)/tools/bmptk-cp.exe" -f
   IMAGE_SIZES   ?= "$(BMPTK)/tools/image_sizes/image_sizes.exe"
   CHECK_PORT    ?= "$(BMPTK)/tools/check_serial_port/check_serial_port.exe"
   CHECK_DRIVE   ?= "$(BMPTK)/tools/check_drive_name/check_drive_name.exe"
   COPY_TO_DRIVE ?= "$(BMPTK)/tools/copy_to_drive/copy_to_drive.exe"
   LPC21ISP      ?= "$(BMPTK)/tools/lpc21isp_197/lpc21isp_hr.exe" 
   BMPTK-SSC     ?= $(PYTHON) "$(BMPTK)/tools/bmptk-ssc.py"
   AVRDUDE       ?= "$(BMPTK)/tools/avrdude/avrdude.exe"
   AVRDUDEC      ?= "$(BMPTK)/tools/avrdude/avrdude.conf"
   TERMINAL      ?= "$(BMPTK)/tools/Terminal.exe"
   BOSSAC        ?= "$(BMPTK)/tools/bmptk-bossac.exe"
   DUE_BOOTMODE  ?= "$(BMPTK)/tools/due_bootmode/due_bootmode.exe" $(SERIAL_PORT) 
   
   # installed separately
   ATPROGRAM     ?= "$(AtmelToolchain)/../Atmel Studio 6.2/atbackend/atprogram.exe"
   STM32_CLI     ?= "$(STM32LU)/ST-LINK Utility/ST-LINK_CLI.exe"     
   
   # windows specific
   PAUSE         ?= cmd /c pause     
   
   # needed to be able to update PATH
   .ONESHELL:
   export PATH=%PATH%;$(GCC-WIN)/bin
   
   # find python
   PYTHON_LOCATIONS += 'C:/Program Files\ (x86)/Pytho*'
   #PYTHON_LOCATIONS += C:/temp/readme.tx?
   #PYTHON_LOCATIONS += C:/Python C:/Python36 C:/Python35
   PYTHON_LOCATION  ?= $(foreach x, $(PYTHON_LOCATIONS), $(wildcard $(x)))
   # $(error [$(PYTHON_LOCATIONS)] $(PYTHON_LOCATION))  
   
   PYTHON_LOCATION  := C:\Python36
   #PYTHON_LOCATION  := C:\Program Files (x86)\Python
   INCLUDES         += -I$(PYTHON_LOCATION)\include
   PYTHON_DLLS      += "$(PYTHON_LOCATION)\Python3.dll" "$(PYTHON_LOCATION)\Python36.dll"
   
else

   # linux
   MAKE          ?= make
   REMOVE        ?= rm -f
   COPY          ?= cp
   
   # distributed with bmptk
   IMAGE_SIZES   ?= $(BMPTK)/tools/image_sizes/image_sizes
   CHECK_PORT    ?= $(BMPTK)/tools/check_serial_port/check_serial_port
   CHECK_DRIVE   ?= $(BMPTK)/tools/check_drive_name/check_drive_name
   # COPY_TO_DRIVE ?= $(BMPTK)/tools/copy_to_drive/copy_to_drive
   LPC21ISP_EXE  ?= $(BMPTK)/tools/lpc21isp_197/lpc21isp_hr_linux.exe
   LPC21ISP      ?= $(SUDO) $(LPC21ISP_EXE)
   
   # installed separately
   AVRDUDE       ?= $(SUDO) avrdude
   AVRDUDEC      ?= "$(BMPTK)/tools/avrdude/avrdude.conf"
   BOSSAC        ?= $(SUDO) bossac

   DUE_BOOTMODE  ?= $(SUDO) stty -F $(SERIAL_PORT) speed 1200 cs8 -cstopb -parenb; sleep 1.0
   
endif

$(BMPTK)/tools/lpc21isp_197/lpc21isp_hr_linux.exe:
	cd $(BMPTK)/tools/lpc21isp_197; make


#============================================================================
#
# The main project source file is sure part of the project.
# By default it is the name of the project, but we must find out what
# the extension is: .asm, .c or .cpp
# 
#============================================================================

# some targets can be build are not C/C++/asm projects
ifeq ($(MAKECMDGOALS),renumber)
   NOPROJECT = 1
endif
ifeq ($(MAKECMDGOALS),notab)
   NOPROJECT = 1
endif
ifeq ($(MAKECMDGOALS),clean)
   NOPROJECT = 1
endif
ifeq ($(MAKECMDGOALS),uno-bootloader)
   NOPROJECT = 1
endif
ifeq ($(MAKECMDGOALS),85-bootloader)
   NOPROJECT = 1
endif
ifeq ($(MAKECMDGOALS),mef)
   NOPROJECT = 1
endif
ifeq ($(MAKECMDGOALS),doxygen)
   NOPROJECT = 1
endif
ifeq ($(MAKECMDGOALS),codelite_workspace)
   NOPROJECT = 1
endif

# the default project name is 'main'
PROJECT ?= main

ifeq ($(wildcard $(PROJECT).cpp),)
   ifeq ($(wildcard $(PROJECT).c),)
      ifeq ($(wildcard $(PROJECT).asm),)
         ifneq ($(NOPROJECT),1)
            $(error PROJECT ($(PROJECT)) is not a .cpp, .c or .asm file)
         endif   
      else	  
         SOURCES += $(PROJECT).asm
      endif	  
   else	  
      SOURCES += $(PROJECT).c
   endif	  
else	     
   SOURCES += $(PROJECT).cpp
endif


#============================================================================
#
# some (possible) result files 
# 
#============================================================================


LINKERSCRIPT  := $(PROJECT).ld
BIN           := $(PROJECT).bin
ELFX          := $(PROJECT).elfx
ELF           := $(PROJECT).elf
MAP           := $(PROJECT).map
HEX           := $(PROJECT).hex
DMP           := $(PROJECT).dmp
LSSX          := $(PROJECT).lssx
LSS           := $(PROJECT).lss
EXE           := $(PROJECT).exe
DLL           := $(PROJECT).dll
PYD           := $(PROJECT).pyd
NMDUMP        := $(PROJECT).nmdump   
ANNOTATED     := $(PROJECT).annotated
   
# and remember to delete these result files
CLEAN         += $(LINKERSCRIPT)
CLEAN         += $(BIN)
CLEAN         += $(ELFX)
CLEAN         += $(ELF)
CLEAN         += $(MAP)
CLEAN         += $(HEX)
CLEAN         += $(DMP)
CLEAN         += $(LSSX)
CLEAN         += $(LSS)
CLEAN         += $(EXE)
CLEAN         += $(DLL)
CLEAN         += $(PYD)
CLEAN         += $(NMDUMP)
CLEAN         += $(ANNOTATED)


#============================================================================
#
# decide whether we have a C++ project or a C/Assembler project
#
# When a project uses at least one C++ file it is a C++ project, and
# the C++ tools are used for ALL files. If not, the C tools are used.
# 
#============================================================================

SOURCES := $(strip $(SOURCES))
ifeq ($(patsubst %.cpp,,$(SOURCES)),$(SOURCES))
   LANGUAGE ?= C
else
   LANGUAGE ?= C++
endif

   
#============================================================================
#
# boards
# 
#============================================================================

ifeq ($(TARGET),esp-01)
   CHIP        ?= esp8266
   # by default the chip runs at 80 MHz
   XTAL        ?= 80000   
endif

ifeq ($(TARGET),lpc800_mini_kit)
   CHIP        ?= lpc810m021fn8
   XTAL        ?= 12000   
endif

ifeq ($(TARGET),lpc800_max)
   CHIP        ?= lpc812m101jdh16
   XTAL        ?= 12000  
   
# does this work?   
#   PORT_CHECK  ?=
#   DOWNLOAD_DRIVE ?= MBED
#   RUN         ?= $(COPY_TO_DRIVE) $(BIN) $(DOWNLOAD_DRIVE) application.bin   

endif

ifeq ($(TARGET),tiny_11c14)
   CHIP        ?= lpc11c14fbd48
   XTAL        ?= 12000   
endif

ifeq ($(TARGET),arduino_uno)
   CHIP        ?= atmega328
   XTAL        ?= 12000   
endif

ifeq ($(TARGET),my_first_dev_board)
   CHIP        ?= atmega328
   XTAL        ?= 12000   
endif

ifeq ($(TARGET),arduino_nano)
   CHIP        ?= atmega328
   XTAL        ?= 12000   
endif

ifeq ($(TARGET),arduino_atmega328)
   CHIP        ?= atmega328p
endif

ifeq ($(TARGET),arduino_due)
   CHIP        ?= sam3x8e
   XTAL        ?= 12000   
endif

ifeq ($(TARGET),msp_exp430g2)
   CHIP        ?= msp430g2553
   XTAL        ?= 16000   
endif

ifeq ($(TARGET),msp_exp430fr4133)
   CHIP        ?= msp430fr4133
   XTAL        ?= 16000   
endif

ifeq ($(TARGET),db103)
   CHIP        ?= lpc1114fn28
endif

ifeq ($(TARGET),db104)
   CHIP        ?= lpc1114fn28
endif

ifeq ($(TARGET),db105)
   CHIP        ?= lpc810m021fn8
endif

ifeq ($(TARGET),scouting_lock)
   CHIP        ?= stm32f030k6
endif

ifeq ($(TARGET),blue_pill)
   CHIP        ?= stm32f103c8
endif

ifeq ($(TARGET),stm32_minisystem)
   CHIP        ?= stm32f103c8
endif

ifeq ($(TARGET),stm32f0discovery)
   CHIP        ?= stm32f051r8
endif

ifeq ($(TARGET),stm32l1discovery)
   CHIP        ?= stm32l152rc
endif

ifeq ($(TARGET),stm32f411nucleo)
   CHIP        ?= stm32f411re
endif

ifeq ($(TARGET),blue_pill)
   CHIP        ?= stm32f103c8
endif

ifeq ($(TARGET),frdm_kl25z)
   CHIP        ?= kl25z128vlk4
endif

ifeq ($(TARGET),frdm_kl27z)
   CHIP        ?= mkl27z64vlh4
endif

ifeq ($(TARGET),xmc_2go_1100)
   CHIP        ?= xmc1100
endif

# the target did not match any boards, and no explicit
# chip was specified, assume the TARGET is the chip
ifeq ($(CHIP),)
   CHIP := $(TARGET)
endif


#============================================================================
#
# target-specific parts
# 
#============================================================================

define All_Targets
   # make sure the bmptk and hwcpp files are found
   SEARCH            += $(BMPTK)
   SEARCH            += $(BMPTK)/targets 
   
   # the language dialects to use
   CPP_LANGUAGE      ?= -std=c++17 -fconcepts
   C_LANGUAGE        ?= -std=c11
   AS_LANGUAGE       ?= -x assembler-with-cpp   
   
   # common settings for all languages
   ALL_ERRORS        ?= -Wall -Werror
   ALL_IGNORE        += -Wno-maybe-uninitialized 
   ALL_IGNORE        += -Wno-unused-local-typedefs
   ALL_IGNORE        += -Wno-unused-but-set-variable
   ALL_IGNORE        += -Wno-unused-local-typedefs
   ALL_IGNORE        += -Wno-unused-function
   ALL_IGNORE        += -Wno-attributes
   ALL_OPTIMIZATION  ?= -Os
   
   # cpp: don't use features that are useless and/or too expensive
   CPP_RTTI          ?= -fno-rtti 
   CPP_EXCEPTIONS    ?= -fno-exceptions -fno-asynchronous-unwind-tables
   CPP_THREADSAFE    ?= -fno-threadsafe-statics 
   CPP_CXA           ?= -fno-use-cxa-get-exception-ptr   
   
endef

define Hosted
   $(eval $(All_Targets))
   EMBEDDED          ?= 0
   HOSTED            ?= 1
   HAS_FILESYSTEM    ?= 1
   HEAP              ?= NATIVE
   STACK_SIZE        ?= 1   
   CPP_LPTHREAD      ?= -lpthread
endef

define Embedded 
   $(eval $(All_Targets))
   EMBEDDED          ?= 1
   HOSTED            ?= 0
   HAS_FILESYSTEM    ?= 0
   HEAP              ?= NONE
   
   ALL_SECTIONS      ?= -fdata-sections -ffunction-sections
   #CORE_FLAGS        += -fno-asynchronous-unwind-tables
   
   LN_MAP            ?= -Wl,-Map,$(MAP)      
   LN_SECTIONS       ?= -Wl,--gc-sections
   LN_ERRORS         ?= -Wl,-fatal-warnings 
   
   # RESULTS += $(DMP) $(LSS) $(NMDUMP) 
   # RESULTS += $(MAP)
   # RESULTS += $(LST)
   
   # create a 'fake' iostream in hwcpp
   DEFINES         += -DHWCPP_FAKE_OSTREAM 
endef

   #=========================================================================
   #
   # running native
   # 
   #=========================================================================
   
# running native on Windows     
define Windows      
   CHIP              := Windows_Native
   PREFIX            := $(GCC-WIN)/bin/
   TARGET_CPP_FLAGS  += -mno-ms-bitfields   
   TARGET_C_FLAGS    += -mno-ms-bitfields   
   TARGET_LN_FLAGS   += -lgdi32
   RESULTS           += $(EXE)
   export PATH       := $PREFIX:$(PATH)
   RUN               ?= ./$(EXE)
   RUN_PAUSE         ?= $(PAUSE)
   DEFINES           += -DHWLIB_TARGET_Windows   
endef

# running native on some Linux
define Linux      
   CHIP              := Linux_Native
   PREFIX            := 
   RESULTS           += $(EXE)
   RUN               ?= sudo ./$(EXE)
   RUN_PAUSE         ?= read -p "Press return to continue" x
   DEFINES           += -DHWLIB_TARGET_Linux
endef   

ifeq ($(OS),Windows_NT)   
   define Native
      $(eval $(Hosted))
      $(eval $(Windows))
   endef
else 
   define Native
      $(eval $(Hosted))
      $(eval $(Linux))
   endef  
endif 

ifeq ($(TARGET),native)
   $(eval $(Native)) 
endif

ifeq ($(TARGET),pyd)
   $(eval $(Native))
   ifeq ($(OS),Windows_NT)   
      $(eval $(Windows))
	  
	  # enable exeptions (disabled by default by bmptk)
      PROJECT_CPP_FLAGS += -fexceptions -frtti
	  
	  # build a .pyd dll file instead of a .exe executable
	  RESULTS := $(filter-out $(EXE),$(RESULTS)) $(PYD)
	  
	  # needed for pybind11
	  CPP_RTTI          := -frtti 
      CPP_EXCEPTIONS    := -fexceptions 
      CPP_LANGUAGE      := -std=gnu++17   
   else
      # untested
   endif   
endif	  
      
   #=========================================================================
   #
   # 8-bit AVR chips
   # 
   #=========================================================================
   
define Atmega
   $(eval $(Embedded))
   ifeq ($(OS),Windows_NT)   
      PREFIX          := $(GCC-AVR)/bin/avr-
   else
      PREFIX          := $(GCC-AVR)
      # CPP                ?= "$(PREFIX)c++"
   endif
   SEARCH          += $(BMPTK)/targets/avr $(BMPTK)/targets/avr/include
   SEARCH          += $(BMPTK)/targets/avr $(BMPTK)/targets/avr/avr8-hacks
   DEFINES         += -DBMPTK_INCLUDE_CHIP=io.h   
   RESULTS         += $(ELF) $(BIN) $(HEX) 
   CPU             ?= avr
   ROOT            ?= __vectors
   #LN_TEMPLATE     ?= bmptk_linkerscript_avr5_xn.c
   # OBJ             += bmptk_startup_avr.o
   # LDFLAGS	       += -T "C:\Program Files (x86)\Arduino\hardware\tools\avr\avr\lib\ldscripts\avr5.xn"
   # LDFLAGS         += -T bmptk_linkerscript_avr5_xn.ld
   #LDFLAGS	       += -v
   CPP_LANGUAGE    := -std=c++17 -fconcepts  
   OBJECTS         += cxa_virtual.o
endef

ifeq ($(CHIP),atmega328)
   $(eval $(Atmega))
   ROM_START      ?= 0x0000
   RAM_START      ?= 0x0100
   ROM_SIZE       ?= 32k
   RAM_SIZE       ?= 2k
   STACK_SIZE     ?= 1536
   XTAL           ?= 20000   
   DEFINES        += -D__AVR_ATmega328P__ 
   ARCH_FLAGS     += -mmcu=atmega328p
   # PORT_CHECK     ?= $(CHECK_PORT) $(SERIAL_PORT)  
   RUN            ?= $(AVRDUDE) -C$(AVRDUDEC) -carduino -patmega328p  -P$(SERIAL_PORT) -b115200 -Uflash:w:$(HEX)
   # RUN              ?= $(AVRDUDE) -C$(AVRDUDEC) -c usbasp -p atmega328p  -P$(SERIAL_PORT) -b115200 -Uflash:w:$(HEX)
   # RUN            ?= $(ATPROGRAM) -t avrispmk2 -d atmega328p erase program -f $(HEX)
   LINKERSCRIPT := 
   RUN_TERMINAL   ?= $(LPC21ISP) -termonly -term $(HEX) $(SERIAL_PORT) $(CONSOLE_BAUDRATE) 12000 
endif

.PHONY: bootloader
bootloader:
	@echo 'download the arduino bootloader'
	$(AVRDUDE) -C$(AVRDUDEC) -c usbasp -p atmega328p -b 19200 -v -e -U efuse:w:0x05:m -U hfuse:w:0xD6:m -U lfuse:w:0xFF:m
	$(AVRDUDE) -C$(AVRDUDEC) -c usbasp -p atmega328p  -P$(SERIAL_PORT) -b115200 -Uflash:w:$(BMPTK)/tools/optiboot_atmega328.hex
	@echo ' '   


   #=========================================================================
   #
   # 16-bit MSP430 chips
   # 
   #=========================================================================
   
define MSP430
   $(eval $(Embedded))
   CPU             ?= msp430
   ROOT            ?= __start
   ROM_START      ?= 0x00000000   
   ifeq ($(OS),Windows_NT)
      PREFIX         := $(GCC-MSP430)/bin/msp430-elf-
   else
      PREFIX         := $(GCC-MSP430)/bin/msp430-
   endif   
   # SEARCH         += c:/ti/ccsv6/tools/compiler/gcc_msp430_4.9.14r1_98/msp430-elf/include
   # SEARCH         += c:/ti/ccsv6/ccs_base/msp430/include_gcc
   SEARCH         += $(BMPTK)/targets/msp430/include
   DEFINES        += -DBMPTK_INCLUDE_CHIP=msp430.h 
   #RUN_PAUSE       ?= $(PAUSE)
   RESULTS         += $(ELF) $(BIN) $(HEX) 
   # linkerscript.ld
endef

ifeq ($(CHIP),msp430g2553)
   $(eval $(MSP430))   
   ROM_SIZE       ?= 16k
   RAM_SIZE       ?= 512
   STACK_SIZE     ?= 384
   DEFINES        += -mmcu=msp430g2553 
   LDFLAGS	      += -T$(BMPTK)/targets/msp430/linkerscripts/$(CHIP).ld
   RUN            ?= $(MSP430FLASHER) -n MSP430G2553 -w $(HEX) -v -z [VCC] -m SBW2 -g
   RUN_PAUSE      := $(PAUSE)
endif

ifeq ($(CHIP),msp430fr4133)
   $(eval $(MSP430))   
   ROM_SIZE       ?= 15k
   RAM_SIZE       ?= 2k
   STACK_SIZE     ?= 1536  
   DEFINES        += -mmcu=msp430fr4133 
   LDFLAGS	      += -T$(BMPTK)/targets/msp430/linkerscripts/$(CHIP).ld
   RUN            ?= $(MSP430FLASHER) -n MSP430FR4133 -w $(HEX) -v -z [VCC] -m SBW2 -g
   RUN_PAUSE      := $(PAUSE)
endif   
   
   #=========================================================================
   #
   # ARM v7 chips
   # 
   #=========================================================================

define ARM_V7
   $(eval $(Embedded))
   PREFIX          := $(GCC-ARM)/bin/$(ARM-EABI)-
#   CORE_FLAGS      += -mcpu=cortex-m0 -fomit-frame-pointer -march=armv6-m
   SEARCH          += $(BMPTK)/targets/armv7 $(BMPTK)/targets/armv7/include
   SOURCES         += bmptk_arm_startup.asm
   COREFLAGS    += -nostdlib
   COREFLAGS    += -nodefaultlibs  
   LDFLAGS    += -nostdlib
   LDFLAGS    += -nodefaultlibs
   CORE_FLAGS      += -nostartfiles 
   LDFLAGS         += -nostartfiles   
   RESULTS         += $(HEX) 
endef

define LPC_ARM_V7
   $(eval $(ARM_V7))
   $(eval $(LPC))
   LDFLAGS        += -T linkerscript.ld 
   LN_TEMPLATE    ?= linkerscript.ld
   ROM_START      ?= 0x00000000
   RAM_START      ?= 0x40000000
   INCLUDES       += -I$(BMPTK)/targets/armv7/include
   XTAL           ?= 12000   
   PORT_CHECK     ?= $(CHECK_PORT) $(SERIAL_PORT)  
   RUN            ?= $(LPC21ISP) -control -verify -term $(HEX) $(SERIAL_PORT) $(SERIAL_BAUDRATE) 12000
endef

ifeq ($(CHIP),lpc2148fbd64)
   $(eval $(LPC_ARM_V7))
   ROM_SIZE       ?= 512k
   RAM_SIZE       ?= 32k
   STACK_SIZE     ?= 24576
   DEFINES        += -DBMPTK_INCLUDE_CHIP=lpc2148.h 
endif
  
   
   #=========================================================================
   #
   # Cortex chips common
   # 
   #=========================================================================

define Cortex
   $(eval $(Embedded))
   
   ifeq ($(GCC-ARM),)
      # use shell to find the tools   
      PREFIX := $(ARM-EABI)-
   else
      # use absolute paths to find the tools
      PREFIX := $(GCC-ARM)/bin/$(ARM-EABI)-
   endif

   SEARCH          += $(BMPTK)/targets/cortex $(BMPTK)/targets/cortex/cmsis
   SOURCES         += bmptk_cortex.c
   COREFLAGS    += -nostdlib
   COREFLAGS    += -nodefaultlibs  
   LDFLAGS    += -nostdlib
   LDFLAGS    += -nodefaultlibs  
   CORE_FLAGS      += -nostartfiles 
   LDFLAGS         += -nostartfiles -lm -lgcc -lc
   LN_TEMPLATE    ?= bmptk_linkerscript_cortex.c
   # should be $(LINKERSCRIPT)
   LDFLAGS	       += -T $(LINKERSCRIPT)
   OBJ         += bmptk_startup_cortex.o   
   CPU         ?= cortex
   ROOT        ?= __startup
endef   
   
define Cortex-M0
   $(eval $(Cortex)) 
   ARCH_FLAGS      := -mcpu=cortex-m0 -mthumb -fomit-frame-pointer -march=armv6-m
endef

define Cortex-M0+
   $(eval $(Cortex))
   ARCH_FLAGS     = -mcpu=cortex-m0 -mthumb -fomit-frame-pointer -march=armv6-m
endef

define Cortex-M3
   $(eval $(Cortex))
   ARCH_FLAGS     := -mcpu=cortex-m3 -mthumb -fomit-frame-pointer -march=armv7-m
endef

define Cortex-M4F
   $(eval $(Cortex))
   ARCH_FLAGS     := -mcpu=cortex-m4 -mthumb -fomit-frame-pointer 
   # -march=armv7-m
endef
   
      #======================================================================
      #
      # NXP : LPC Cortex chips
      # 
      #======================================================================

define LPC_Cortex
   $(eval $(LPC))
   ROM_START       ?= 0x00000000
   RAM_START       ?= 0x10000000
   INCLUDES        += -I$(BMPTK)/targets/cortex/lpc/cmsis   
   XTAL            ?= 12000   
   RESULTS         += $(HEX)
   PORT_CHECK      ?= $(CHECK_PORT) $(SERIAL_PORT)
   LPCISP_PARAMS   := $(HEX) $(SERIAL_PORT) $(SERIAL_BAUDRATE) 12000
   # RUN             ?= $(LPC21ISP) -control -verify -term $(HEX) $(SERIAL_PORT) $(SERIAL_BAUDRATE) 12000
   RUN             ?= $(LPC21ISP) -control -verify $(HEX) $(SERIAL_PORT) $(SERIAL_BAUDRATE) 12000
   RUN_TERMINAL   ?= $(LPC21ISP) -termonly -term $(HEX) $(SERIAL_PORT) $(CONSOLE_BAUDRATE) 12000      
endef

define LPC81X
   $(eval $(Cortex-M0+))
   $(eval $(LPC_Cortex))
   INCLUDES       += -I$(BMPTK)/targets/cortex/lpc/cmsis/lpc800/inc
   DEFINES        += -DBMPTK_INCLUDE_CHIP=LPC8xx.h 
endef

ifeq ($(CHIP),lpc810m021fn8)
   $(eval $(LPC81X))   
   ROM_SIZE       ?= 8k
   RAM_SIZE       ?= 1k
   STACK_SIZE     ?= 768
endif

ifeq ($(CHIP),lpc812m101jdh16)
   $(eval $(LPC81X))
   ROM_SIZE       ?= 16k
   RAM_SIZE       ?= 4k
   STACK_SIZE     ?= 3072
endif

define LPC1114
   $(eval $(Cortex-M0))
   $(eval $(LPC_Cortex))
   INCLUDES       += -I$(BMPTK)/targets/cortex/lpc/cmsis/11xx/inc 
   DEFINES        += -DBMPTK_INCLUDE_CHIP=LPC11xx.h 
endef

ifeq ($(CHIP),lpc1114fn28)
   $(eval $(LPC1114))
   ROM_SIZE       ?= 32k
   RAM_SIZE       ?= 4k
   STACK_SIZE     ?= 3072
endif

ifeq ($(CHIP),lpc11c14fbd48)
   $(eval $(LPC1114))
   ROM_SIZE       ?= 32k
   RAM_SIZE       ?= 8k
   STACK_SIZE     ?= 6144  
endif

      #======================================================================
      #
      # STM Cortex chips
      # 
      #======================================================================

define STM32
   ROM_START      ?= 0x08000000
   RAM_START      ?= 0x20000000     
   INCLUDES       += -I$(BMPTK)/targets/cortex/stm32
   RESULTS         += $(HEX)
   RUN            ?= $(STM32_CLI) -P $(HEX) -Rst
   RUN_PAUSE      ?= $(PAUSE)
endef

ifeq ($(CHIP),stm32f051r8)
   $(eval $(Cortex-M0))
   $(eval $(STM32))
   ROM_SIZE       ?= 64k
   RAM_SIZE       ?= 8k
   STACK_SIZE     ?= 6144  
   DEFINES        += -DBMPTK_INCLUDE_CHIP=stm32f051x8.h 
endif

ifeq ($(CHIP),stm32f030k6)
   $(eval $(Cortex-M0))
   $(eval $(STM32))
   ROM_SIZE       ?= 16k
   RAM_SIZE       ?= 4k
   STACK_SIZE     ?= 2048  
   DEFINES        += -DBMPTK_INCLUDE_CHIP=stm32f030x6.h 
endif

ifeq ($(CHIP),stm32f103c8)
   $(eval $(Cortex-M3))
   $(eval $(STM32))
   ROM_SIZE       ?= 64k
   RAM_SIZE       ?= 20k
   STACK_SIZE     ?= 6144  
   DEFINES        += -DBMPTK_INCLUDE_CHIP=stm32f103xb.h 
endif

ifeq ($(CHIP),stm32l152rc)
   $(eval $(Cortex-M3))
   $(eval $(STM32))
   ROM_SIZE       ?= 256k
   RAM_SIZE       ?= 32k  
   STACK_SIZE     ?= 24576
   DEFINES        += -DBMPTK_INCLUDE_CHIP=stm32l1xx.h 
endif

ifeq ($(CHIP),stm32f411re)
   $(eval $(Cortex-M4F))
   $(eval $(STM32))
   ROM_SIZE       ?= 512k
   RAM_SIZE       ?= 128k
   STACK_SIZE     ?= 98304
   DEFINES        += -DBMPTK_INCLUDE_CHIP=stm32f411xe.h 
endif

      #======================================================================
      #
      # NXP/Freescale Cortex chips
      # 
      #======================================================================

define Freescale
   ROM_START      ?= 0x00000000   
   RAM_START      ?= 0x20000000
   INCLUDES       += -I$(BMPTK)/targets/cortex/freescale/cmsis 
   RESULTS        += $(BIN)
   RUN            ?= $(STM32_CLI) -P $(HEX) -Rst
   RUN_PAUSE      ?= $(PAUSE)
   COPY_TO_DRIVE  ?= " "
endef

ifeq ($(CHIP),kl25z128vlk4)
   $(eval $(Cortex-M0))
   $(eval $(Freescale))   
   ROM_SIZE       ?= 128k
   RAM_SIZE       ?= 8k
   STACK_SIZE     ?= 6144  
   DOWNLOAD_DRIVE ?= FRDM-KL25Z
   DEFINES        += -DBMPTK_INCLUDE_CHIP=MKL25Z4.h 
   RUN            = $(COPY_TO_DRIVE) $(BIN) $(DOWNLOAD_DRIVE) application.bin
endif

ifeq ($(CHIP),mkl27z64vlh4)
   $(eval $(Cortex-M0))
   $(eval $(Freescale))   
   ROM_SIZE       ?= 64k
   RAM_START      ?= 0x1FFFF000
   RAM_SIZE       ?= 16k
   STACK_SIZE     ?= 8096
   DOWNLOAD_DRIVE ?= FRDM-KL27Z
   DEFINES        += -DBMPTK_INCLUDE_CHIP=MKL28Z644.h 
   RUN            = sudo mount -vo uid=$$(id -u),gid=$$(id -u) /dev/disk/by-label/FRDM-KL27* /mnt/tmp && cp -v $(BIN) /mnt/tmp && sync && sudo umount -v /mnt/tmp
   #RUN            = $(COPY_TO_DRIVE) $(BIN) $(DOWNLOAD_DRIVE) application.bin
endif

      #======================================================================
      #
      # Infineon Cortex chips
      # 
      #======================================================================

define Infineon
   ROM_START      ?= 0x00000000   
   INCLUDES       += -I$(BMPTK)/targets/cortex/infineon/cmsis 
   RUN            ?= $(STM32_CLI) -P $(HEX) -Rst
   RUN_PAUSE      ?= $(PAUSE)
endef

ifeq ($(CHIP),xmc1100)
   $(eval $(Cortex-M0))
   $(eval $(Infineon))   
   ROM_SIZE       ?= 64k
   RAM_START      ?= 0x20000000
   RAM_SIZE       ?= 16k
   STACK_SIZE     ?= 12288
   DOWNLOAD_DRIVE ?= FRDM-KL25Z
   DEFINES        += -DBMPTK_INCLUDE_CHIP=XMC1100.h 
   RUN            = $(COPY_TO_DRIVE) $(BIN) $(DOWNLOAD_DRIVE) application.bin
endif


      #======================================================================
      #
      # Atmel Cortex chips
      # 
      #======================================================================
      
define Atmel_SAM3
   $(eval $(Cortex-M3))
   ROM_START      ?= 0x00000000   
   RAM_START      ?= 0x20000000     
   INCLUDES       += -I$(BMPTK)/targets/cortex/atmel
   INCLUDES       += -I$(BMPTK)/targets/cortex/atmel/sam3xa/include
   PORT_CHECK     ?= $(CHECK_PORT) $(SERIAL_PORT)  
   RUN_PRE        := $(DUE_BOOTMODE)
   # was al gezet?
   RESULTS        += $(BIN)
   #RUN            := $(BOSSAC) -p $(subst /dev/,,$(SERIAL_PORT)) -U false -e -w -v -b $(BIN) -R
   RUN            := $(BOSSAC) --port=$(subst /dev/,,$(SERIAL_PORT)) --force_usb_port=false -e -w -v -b $(BIN) -R
   # RUN            := $(BOSSAC) -e -w -v -b $(BIN)   
   DEFINES        += -DDONT_USE_CMSIS_INIT 
   DEFINES        += -DBMPTK_INCLUDE_CHIP=sam.h 
   LDFLAGS        += -I$(BMPTK)/targets/cortex/atmel/sam3xa/source/as_gcc
   RUN_TERMINAL   ?= $(LPC21ISP) -termonly -term $(HEX) $(SERIAL_PORT) $(CONSOLE_BAUDRATE) 12000   
   
endef     

#   ifeq($(SERIAL_BAUDRATE),1200)
#      $(error do not use 1200 baud: it will reset the Due) 
#   endif
#   ifeq($(CONSOLE_BAUDRATE),1200)
#      $(error do not use 1200 baud: it will reset the Due) 
#   endif 

ifeq ($(CHIP),sam3x8e)
   $(eval $(Atmel_SAM3))
   ROM_SIZE       ?= 512k
   RAM_SIZE       ?= 96k
   STACK_SIZE     ?= 73728     
   DEFINES        += -D__SAM3X8E__ 
endif

      #======================================================================
      #
      # Tensilica xtensa chips:
      #
      # Tensilica Xtensa LX3
      # 
      #======================================================================

define Xtensa-106
   $(eval $(Embedded)) 
   PREFIX         := $(GCC-XTENSA)/bin/xtensa-lx106-elf-
endef

ifeq ($(CHIP),esp8266)
   HEAP           ?= NATIVE
   $(eval $(Xtensa-106))   
   ROM_START      ?= 0x00000000   
   ROM_SIZE       ?= 64k
   RAM_START      ?= 0x20000000
   RAM_SIZE       ?= 16k
   INCLUDES       += -I$(ESP-SDK)/include -I$(ESP-SDK)/include/json
   INCLUDES       += -I$(BMPTK)
   INCLUDES       += -I$(BMPTK)/targets/esp8266 
   DEFINES        += -DBMPTK_INCLUDE_CHIP=esp8266.h 
   
   CORE_FLAGS     += -Os -g -O2 -Wl,-EL 
   CORE_FLAGS     += -fno-inline-functions -nostdlib -mlongcalls 
   CORE_FLAGS     += -mtext-section-literals -D__ets__ -DICACHE_FLASH 
   CORE_FLAGS     += 
   
   LD_FLAGS       += -T$(ESP-SDK)\ld\eagle.app.v6.ld 
   LD_FLAGS       += -L$(ESP-SDK)\lib
   LD_FLAGS       += -nostdlib -Wl,--no-check-sections -u call_user_start -Wl,-static
   ESP_LIBS       := -lc -lgcc -lhal -lphy -lpp -lnet80211 -llwip -lwpa -lmain
   LD_FLAGS       += -Wl,--start-group $(ESP_LIBS) -Wl,--end-group -o $@
   
   OUT_1_ARGS	= -bo $@ -bs .text -bs .data -bs .rodata -bc -ec
   OUT_2_ARGS	= -es .irom0.text $@ -ec
   
   RESULTS        += $(ELF) $(BIN1) $(BIN2)
   
   PORT_CHECK     ?= $(CHECK_PORT) $(SERIAL_PORT)    
   RUN            = $(ESP-LOADER) -p $(SERIAL_PORT) -b $(SERIAL_BAUDRATE) write_flash 0x00000 $(BIN1) 0x40000 $(BIN2)
   RUN_PAUSE      ?= $(PAUSE)
      
   $(BIN1): $(ELF)
		$(ESP-TOOL) -eo $(ELF) $(OUT_1_ARGS)

   $(BIN2): $(ELF)
		$(ESP-TOOL) -eo $(ELF) $(OUT_2_ARGS)        
   
endif

   
#============================================================================
#
# If a valid target was recognised, hosted will be either 1 or 0, 
# so if not, raise an error.
# 
#============================================================================   

ifneq ($(NOPROJECT),1)
   ifeq ($(HOSTED),)
      $(error invalid or no target specified: "$(TARGET)")
   endif
endif   


#============================================================================
#
# where to find the files
# 
#============================================================================

INCLUDES  += $(foreach x, $(SEARCH), -I$(x))

   
#============================================================================
#
# GCC (and other) executables
# 
#============================================================================

CC          := "$(PREFIX)gcc"
CPP       := "$(PREFIX)g++"
AS          := "$(PREFIX)gcc"
OBJCOPY     := "$(PREFIX)objcopy"
OBJDUMP     := "$(PREFIX)objdump"
SIZES       := "$(PREFIX)size"
NM          := "$(PREFIX)nm"
FILT        := "$(PREFIX)c++filt"

ifeq ($(LANGUAGE),C)
   LINKER    := "$(PREFIX)gcc"
else
   LINKER    := "$(PREFIX)g++"
endif


#============================================================================
#
# Add the .o version of all source files to the objects list
# 
#============================================================================

SOURCES_X    := $(patsubst %.asm,%.o,$(patsubst %.S,%.o,$(SOURCES)))
SOURCES_Y    := $(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SOURCES_X)))
OBJECTS      += $(patsubst %.cc,%.o,$(SOURCES_Y))
CLEAN        += $(OBJECTS)


#============================================================================
#
# Pass information to the code 
# 
#============================================================================

DEFINES   += -DBMPTK_TARGET=$(TARGET) 
DEFINES   += -DBMPTK_TARGET_$(TARGET) 
DEFINES   += -DHWLIB_TARGET_$(TARGET) 
DEFINES   += -DHWCPP_TARGET_$(TARGET)
DEFINES   += -DGF_TARGET_$(TARGET)

DEFINES   += -DBMPTK_CHIP=$(CHIP) 
DEFINES   += -DBMPTK_CHIP_$(CHIP) 
DEFINES   += -DBMPTK_XTAL=$(XTAL) 
DEFINES   += -DBMPTK_BAUDRATE=$(CONSOLE_BAUDRATE) 
DEFINES   += -DHWLIB_BAUDRATE=$(CONSOLE_BAUDRATE)
DEFINES   += -DGODAFOSS_BAUDRATE=$(CONSOLE_BAUDRATE)
DEFINES   += -DGF_BAUDRATE=$(CONSOLE_BAUDRATE)
DEFINES   += -DBMPTK_VERSION=$(VERSION) 

ifeq ($(HOSTED),1)
   DEFINES   += -DBMPTK_HOSTED 
endif   
ifeq ($(EMBEDDED),1)
   DEFINES   += -DBMPTK_EMBEDDED 
endif   
ifeq ($(HAS_FILESYSTEM),1)
   DEFINES   += -DBMPTK_HAS_FILESYSTEM 
endif   

HEAP ?= NONE
ifeq ($(HEAP),NONE)
   OBJECTS         += bmptk_heap_none.o
endif   
ifeq ($(HEAP),BMPTK)
   OBJECTS         += bmptk_heap_bmptk.o
endif   


#============================================================================
#
# Dependencies
# 
#============================================================================

ifneq ("$(wildcard $(Makefile))","")
   MAKEFILE    := Makefile
endif
ifneq ("$(wildcard $(makefile))","")
   MAKFILE     := makefile
endif

# Assume compilation depends on 
# all headers, the LN_SOURCE, and the makefile
DEPENDENCIES    += $(HEADERS) $(LN_SOURCE) $(MAKEFILE)
$(OBJECTS): $(DEPENDENCIES)


#============================================================================
#
# where to find non-local files
# 
# VPATH is not used, beacause it would also 'find' .ld and .o files,
# which should be created locally
#
#============================================================================

vpath %.h    $(SEARCH)
vpath %.hpp  $(SEARCH)
vpath %.asm  $(SEARCH)
vpath %.c    $(SEARCH)
vpath %.cpp  $(SEARCH)
vpath %.cc   $(SEARCH)
vpath %.inc  $(SEARCH)


#============================================================================
#
# combine the tool options
# 
#============================================================================
 
COMMON        := 
COMMON        += $(ARCH_FLAGS)
COMMON        += $(ALL_ERRORS)
COMMON        += $(ALL_IGNORE)
COMMON        += $(ALL_OPTIMIZATION)
COMMON        += $(ALL_SECTIONS)
COMMON        += $(ALL_LIBRARIES)
COMMON        += $(INCLUDES) 
COMMON        += $(DEFINES)

AS_FLAGS      := $(COMMON) $(AS_LANGUAGE)
C_FLAGS       := $(COMMON) $(C_LANGUAGE)
CPP_FLAGS     := $(COMMON) $(CPP_LANGUAGE)
LN_FLAGS      := $(COMMON) -lgcc 

# add C++-specific flags
CPP_FLAGS     += $(CPP_LPTHREAD) 
CPP_FLAGS     += $(CPP_RTTI) 
CPP_FLAGS     += $(CPP_EXCEPTIONS)
CPP_FLAGS     += $(CPP_THREADSAFE)
CPP_FLAGS     += $(CPP_CXA)

# add linker-specific flags
LN_FLAGS      += $(LN_MAP)
LN_FLAGS      += $(LN_SECTIONS)
LN_FLAGS      += $(LN_ERRORS)

# target-specific and project-specific flags are last 
# so they can override all previous flags
AS_FLAGS      += $(TARGET_AS_FLAGS)  $(PROJECT_AS_FLAGS) 
C_FLAGS       += $(TARGET_C_FLAGS)   $(PROJECT_C_FLAGS)
CPP_FLAGS     += $(TARGET_CPP_FLAGS) $(PROJECT_CPP_FLAGS)
LN_FLAGS      += $(TARGET_LN_FLAGS)  $(PROJECT_LN_FLAGS)


#============================================================================
#
# choose which compiler and linker to use
# 
#============================================================================

ifeq ($(LANGUAGE),C)
   LINKER  := $(CC)   $(LN_FLAGS)
   AS      := $(AS)   $(AS_FLAGS)
   CC      := $(CC)   $(C_FLAGS)
   CPP     := 
else
   LINKER  := $(CPP)  $(LN_FLAGS)
   AS      := $(AS)   $(AS_FLAGS)
   CC      := $(CC)   $(C_FLAGS)
   CPP     := $(CPP)  $(CPP_FLAGS)
endif   


#============================================================================
#
# the file(s) that hold the stack allocated by/within bmptk
# 
#============================================================================

CALCULATED_SIZE_STACK_FILE  := bmptk_calculated_size_stack
FIXED_SIZE_STACK_FILE       := bmptk_fixed_size_stack

CLEAN         += $(CALCULATED_SIZE_STACK_FILE).c
CLEAN         += $(CALCULATED_SIZE_STACK_FILE).o
CLEAN         += $(FIXED_SIZE_STACK_FILE).c
CLEAN         += $(FIXED_SIZE_STACK_FILE).o 

# choose between a calculated size stack and a fixed size stack
ifeq ($(STACK_SIZE),AUTOMATIC)
   BMPTK_STACK_FILE  := $(CALCULATED_SIZE_STACK_FILE)
else 
   BMPTK_STACK_FILE  := $(FIXED_SIZE_STACK_FILE)
endif


#============================================================================
#
# how to make things
# 
#============================================================================

# use only the explcit rules below
.SUFFIXES:
.SUFFIXES: .annotated

# How to show the toolchain version
.PHONY: version
build: version
version:
	@echo 'Show toolchain version at $(CC)'
	@$(CC) --version   
   
   #=========================================================================
   #
   # Compile
   # 
   #=========================================================================
   
%.lst: $(DEPENDENCIES)
%.o:   $(DEPENDENCIES)
%.lss: $(DEPENDENCIES)

# How to make assembler listing from asm
%.lst: %.asm 
	@echo 'Compiling C file to asm listing: $<'
	$(AS) -c -S > $(patsubst %.o,%.lst,$@) $<
	@echo ' '   
   
# How to make assembler listing from C
%.lst: %.c 
	@echo 'Compiling C file (.c) to asm listing: $<'
	$(CC) -c -S -o $(patsubst %.o,%.lst,$@) $<
	@echo ' '   
   
# How to make assembler listing from C++ (.cpp) 
%.lst: %.cpp 
	@echo 'Compiling C++ file (.cpp) to asm listing: $<'
	$(CPP) -c -S -o $(patsubst %.o,%.lst,$@) $<
	@echo '  '

# How to make assembler listing from C++ (.cc)  
%.lst: %.cc 
	@echo 'Compiling C++ file (.cc) to asm listing: $<'
	$(CPP) -c -S -o $(patsubst %.o,%.lst,$@) $<
	@echo '  '

# How to make an object file from an assembler file
%.o: %.asm 
	@echo 'Assembling (.asm) file: $<'
	$(AS)  -c -o $@ $<
	@echo ' '

# How to make an object file from a C file
%.o: %.c 
	@echo 'Compiling C (.c) file: $<'
	$(CC) -c -o $@ $< 
	@echo ' '

# How to make an object file from a C++ file (.cpp)
%.o: %.cpp 
	@echo 'Compiling C++ (.cpp) file: $<'
	$(CPP) -c -o $@ $< 
	@echo ' '
    
# How to make an object file from a C++ file (.cc)
%.o: %.cc 
	@echo 'Compiling C++ (.cc) file: $<'
	$(CPP) -c -o $@ $< 
	@echo ' '
    
   #=========================================================================
   #
   # create the linkerscript from the template 
   # 
   #=========================================================================

LD_CONFIG    += -DROM_START=$(ROM_START) 
LD_CONFIG    += -DROM_SIZE=$(ROM_SIZE) 
LD_CONFIG    += -DRAM_START=$(RAM_START) 
LD_CONFIG    += -DRAM_SIZE=$(RAM_SIZE)

# How to make a linkerscipt by preprocessing a .c linkerfile template
$(LINKERSCRIPT): $(LN_TEMPLATE)
	@echo 'Creating the linkerscript'
	$(CC) $(LD_CONFIG) -E -P -C $< -o $@ 
	@echo ' '
   
   #=========================================================================
   #
   # fixed size stack, link
   # 
   #=========================================================================

# How to create a fixed size stack 
$(FIXED_SIZE_STACK_FILE).c: $(MAKEFILE)
	@echo 'create the (fixed size) stack file $(FIXED_SIZE_STACK_FILE).o'
	$(CC) -E -P -C -DSTACK_SIZE=$(STACK_SIZE) $(BMPTK)/targets/bmptk_stack.c -o $@
	@echo ' '
   
# make .elf by linking all objects
$(ELF): $(addprefix ./,$(OBJECTS)) $(BMPTK_STACK_FILE).o $(LIBS) $(LINKERSCRIPT)
	@echo 'Linking ELF target: $(ELF)'
	$(LINKER) -o $@ $(OBJECTS) $(BMPTK_STACK_FILE).o $(LDFLAGS) $(LIBS) $(LINKER_FLAGS)
	@echo ' '   
    
# How to make an .exe executable by linking all objects
$(EXE): $(OBJECTS) $(LIBS) $(ALL_LIBRARIES) 
	@echo 'Linking EXE target: $(EXE)'
	$(LINKER) -o $@ $(OBJECTS) $(LDFLAGS) $(LIBS) $(ALL_LIBRARIES) $(LINKER_FLAGS)
	@echo ' '  
      
# How to make an .dll dynamic link library by linking all objects
$(DLL): $(OBJECTS) $(LIBS) $(ALL_LIBRARIES)
	@echo 'Linking DLL target: $(DLL)'
	$(LINKER) -shared -o $@ $(OBJECTS) $(LDFLAGS) $(LIBS) $(ALL_LIBRARIES)
	@echo ' '  
      
# How to make a .pyd python extension by linking all objects
$(PYD): $(OBJECTS) $(LIBS) $(ALL_LIBRARIES) 
	@echo 'Linking PYD target: $(PYD)'
	$(LINKER) -shared -o $@ $(OBJECTS) $(LDFLAGS) $(LIBS) $(ALL_LIBRARIES) $(PYTHON_DLLS)
	@echo ' '  
      
   #=========================================================================
   #
   # calculated size stack
   # 
   #=========================================================================

# How to make an .elfx file by linking all objects with a fixed (dummy) stack
$(ELFX): $(addprefix ./,$(OBJECTS)) $(LIBS) $(FIXED_SIZE_STACK_FILE).o $(LINKERSCRIPT)
	@echo 'Linking ELFX target: $(ELFX)'
	$(LINKER) -o $@ $(OBJECTS) $(FIXED_SIZE_STACK_FILE).o $(LDFLAGS) $(LIBS)
	@echo ' '

# How to make a .lssx (listing) file from an .elfx file
$(LSSX): $(ELFX)
	$(OBJDUMP) -S $< > $@
   
# uncomment to generate an annotated listing
# (off by default because it slows things down)
#ifeq ($(ANNOTATE),YES)
   ANNOTATE := -annotate $(ANNOTATED) -filt $(FILT)  
#endif   
   
# How to create a calculated size stack file
$(CALCULATED_SIZE_STACK_FILE).c: bmptk_stack.c $(LSSX)
	@echo 'analysing the required stack size'
	$(BMPTK-SSC) $(ANNOTATE) $(CPU) $(ROOT) $(LSSX) $< $@
	@echo ' '


   #=========================================================================
   #
   # .elf postprocessing
   # 
   #=========================================================================
   
# How to make a .lss (listing) file from an .elf file
$(LSS): $(ELF)
	$(OBJDUMP) -S $< > $@

# How to make a sizes listing from the elf file
$(NMDUMP): $(ELF)
	$(NM) -S --size-sort --radix=d $(ELF) >$(NMDUMP)
	$(SIZES) -B -t $(OBJECTS)
	$(IMAGE_SIZES) $(MAP)

# How to make a .dmp file from an .elf file
$(DMP): $(ELF)
	$(OBJDUMP) -x --syms $< > $@

# How to make a .bin file from an .elf file
$(BIN): $(ELF)
	$(OBJCOPY) -O binary -R .bss $< $@

# How to make a .hex file from an .elf file
$(HEX): $(ELF)
	$(OBJCOPY) --output-target ihex -R .bss $< $@
	
# How to make an .srec file from an .elf file
$(SREC): $(ELF)
	$(OBJCOPY) --output-target srec -R .bss $< $@
	

#============================================================================
#
# targets
# 
#============================================================================

.PHONY: $(PROJECT) all build new fresh run clean doxygen mef renumber notab
.PHONY: uno-bootloader 85-bootloader

.DEFAULT_GOAL := build

# aliases
all:     $(PROJECT)
build:   $(PROJECT)
rebuild: clean build
new:     clean build
fresh:   clean build
rerun:   clean build run

uno-bootloader:
	$(COPY) $(BMPTK)/tools/bootloaders/optiboot_atmega328.hex optiboot_atmega328.hex
	$(AVRDUDE) -p m328p -C$(AVRDUDEC) -cusbasp -F -e -U flash:w:optiboot_atmega328.hex

85-bootloader:
	$(COPY) $(BMPTK)/tools/bootloaders/t85_default.hex t85_default.hex
	$(AVRDUDE) -p t85 -C$(AVRDUDEC) -cusbasp -F -e -U flash:w:t85_default.hex

# Build the project
$(PROJECT): $(RESULTS)

PORT_CHECK :=

# Run (= download and start) the project
run: $(PROJECT) $(LPC21ISP_EXE)
	$(PORT_CHECK)
	$(RUN_PRE)   
	-$(RUN)
	$(RUN_TERMINAL)
	$(RUN_PAUSE)
    
doxygen: doxyfile
	Doxygen
   
NOTABFILES = $(SOURCES) $(HEADERS)
NOTABFAKES = $(NOTABFILES:%=notab-%)   
.PHONY: $(NOTABFAKES)
notab: $(NOTABFAKES)
$(sort $(NOTABFAKES)):
	@$(PYTHON) $(BMPTK)/tools/bmptk-notab.py $(@:notab-%=%)
   
ifeq ($(OS),Windows_NT)
   OSNAME ?= windows
else
   OSNAME ?= linux
endif   

codelite_workspace:
	$(PYTHON) $(BMPTK)/tools/bmptk-mef.py -os $(OSNAME)
   
renumber:
	$(PYTHON) $(BMPTK)/tools/bmptk-renumber.py
   
# Cleanup
CLEAN += -rf doxygen_docs
CLEAN += -rf .settings temp .cproject .project
CLEAN += -rf .clang .codelite __codelite.workspace
clean:
	$(REMOVE) $(CLEAN)
