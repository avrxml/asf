#
# Copyright (c) 2010 Atmel Corporation. All rights reserved.
#
# \asf_license_start
#
# \page License
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
#
# 3. The name of Atmel may not be used to endorse or promote products derived
#    from this software without specific prior written permission.
#
# 4. This software may only be redistributed and used in connection with an
#    Atmel microcontroller product.
#
# THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
# WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
# EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
# STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#
# \asf_license_stop
#

# Path to top level ASF directory relative to this project directory.
PRJ_PATH = ../../../../../../../../..

# Microcontroller: atxmega128a1, atmega128, attiny261, etc.
MCU = atmega128rfa1

# Application target name. Given with suffix .a for library and .elf for a
# standalone application.
TARGET = examples_example_qm_mega_stk600.elf

# C source files located from the top-level source directory
CSRCS = \
       mega/boards/stk600/rc128x_rfx/init.c               \
       thirdparty/qtouch/generic/avr8/qmatrix/examples/example_qm_mega_stk600/BitBangSPI_Master.c \
       thirdparty/qtouch/generic/avr8/qmatrix/examples/example_qm_mega_stk600/atmega128rfa1_stk600/main_m128rfa1.c \
       thirdparty/qtouch/qdebug/QDebug.c                  \
       thirdparty/qtouch/qdebug/QDebugTransport.c         \
       thirdparty/qtouch/qdebug/eeprom_access_mega.c

# Assembler source files located from the top-level source directory
ASSRCS = \
       thirdparty/qtouch/generic/avr8/qmatrix/common/qm_asm_tiny_mega.S

# Include path located from the top-level source directory
INC_PATH = \
       common/boards                                      \
       common/services/gpio                               \
       common/services/ioport                             \
       common/utils                                       \
       mega/boards/stk600                                 \
       mega/boards/stk600/rc128x_rfx                      \
       mega/utils                                         \
       mega/utils/preprocessor                            \
       thirdparty/qtouch/generic/avr8                     \
       thirdparty/qtouch/generic/avr8/qmatrix/common      \
       thirdparty/qtouch/generic/avr8/qmatrix/examples/example_qm_mega_stk600 \
       thirdparty/qtouch/generic/avr8/qmatrix/examples/example_qm_mega_stk600/atmega128rfa1_stk600/include \
       thirdparty/qtouch/generic/include                  \
       thirdparty/qtouch/qdebug \
       thirdparty/qtouch/generic/avr8/qmatrix/examples/example_qm_mega_stk600/atmega128rfa1_stk600/gcc

# Library paths from the top-level source directory
LIB_PATH =  \
       thirdparty/qtouch/generic/avr8/qmatrix/lib/gcc    

# Libraries to link with the project
LIBS =  \
       m128rfa1_32qm_8x_4y_krs_4rs                       

# Additional options for debugging. By default the common Makefile.in will
# add -gdwarf-2.
DBGFLAGS = 

# Optimization settings
OPTIMIZATION = -Os

# Extra flags used when creating an EEPROM Intel HEX file. By default the
# common Makefile.in will add -j .eeprom
# --set-section-flags=.eeprom="alloc,load" --change-section-lma .eeprom=0.
EEPROMFLAGS = 

# Extra flags used when creating an Intel HEX file. By default the common
# Makefile.in will add -R .eeprom -R .usb_descriptor_table.
FLASHFLAGS = 

# Extra flags to use when archiving.
ARFLAGS = 

# Extra flags to use when assembling.
ASFLAGS = 

# Extra flags to use when compiling.
CFLAGS = 

# Extra flags to use when preprocessing.
#
# Preprocessor symbol definitions
#   To add a definition use the format "-D name[=definition]".
#   To cancel a definition use the format "-U name".
#
# The most relevant symbols to define for the preprocessor are:
#   BOARD      Target board in use, see boards/board.h for a list.
#   EXT_BOARD  Optional extension board in use, see boards/board.h for a list.
CPPFLAGS = \
       -D BOARD=STK600_MEGA_RF

# Extra flags to use when linking
LDFLAGS = 

# Pre- and post-build commands
PREBUILD_CMD = 
POSTBUILD_CMD = 