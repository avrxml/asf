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
MCU = atxmega128a1

# Application target name. Given with suffix .a for library and .elf for a
# standalone application.
TARGET = examples_example_qt_qt600.elf

# C source files located from the top-level source directory
CSRCS = \
       common/services/clock/xmega/sysclk.c               \
       common/services/sleepmgr/xmega/sleepmgr.c          \
       thirdparty/qtouch/generic/avr8/qtouch/examples/example_qt_qt600/BitBangSPI_Master.c \
       thirdparty/qtouch/generic/avr8/qtouch/examples/example_qt_qt600/atxmega128a1_qt600/main_xm128a1.c \
       thirdparty/qtouch/qdebug/QDebug.c                  \
       thirdparty/qtouch/qdebug/QDebugTransport.c         \
       thirdparty/qtouch/qdebug/eeprom_access.c           \
       xmega/drivers/nvm/nvm.c                            \
       xmega/drivers/tc/tc.c

# Assembler source files located from the top-level source directory
ASSRCS = \
       thirdparty/qtouch/generic/avr8/qtouch/common/qt_asm_xmega.S \
       xmega/drivers/cpu/ccp.s                            \
       xmega/drivers/nvm/nvm_asm.s

# Include path located from the top-level source directory
INC_PATH = \
       common/boards                                      \
       common/services/clock                              \
       common/services/sleepmgr                           \
       common/utils                                       \
       thirdparty/qtouch/generic/avr8                     \
       thirdparty/qtouch/generic/avr8/qtouch/common       \
       thirdparty/qtouch/generic/avr8/qtouch/examples/example_qt_qt600 \
       thirdparty/qtouch/generic/include                  \
       thirdparty/qtouch/qdebug                           \
       xmega/boards                                       \
       xmega/boards/atxmega128a1_qt600                    \
       xmega/drivers/cpu                                  \
       xmega/drivers/nvm                                  \
       xmega/drivers/pmic                                 \
       xmega/drivers/sleep                                \
       xmega/drivers/tc                                   \
       xmega/utils                                        \
       xmega/utils/preprocessor \
       thirdparty/qtouch/generic/avr8/qtouch/examples/example_qt_qt600/atxmega128a1_qt600/gcc

# Library paths from the top-level source directory
LIB_PATH =  \
       thirdparty/qtouch/generic/avr8/qtouch/lib/gcc     

# Libraries to link with the project
LIBS =  \
       avrxmega7g1-16qt-k-4rs                            

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
       -D BOARD=ATXMEGA128A1_QT600                        \
       -D QT_DELAY_CYCLES=1                               \
       -D QT_NUM_CHANNELS=16                              \
       -D SNS1=H                                          \
       -D SNS2=K                                          \
       -D SNSK1=F                                         \
       -D SNSK2=J                                         \
       -D _DEBUG_INTERFACE_                               \
       -D _QTOUCH_                                        \
       -D _ROTOR_SLIDER_

# Extra flags to use when linking
LDFLAGS = 

# Pre- and post-build commands
PREBUILD_CMD = 
POSTBUILD_CMD = 