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
PRJ_PATH = ../../../../../../../..

# Microcontroller: atxmega128a1, atmega128, attiny261, etc.
MCU = atxmega128a1

# Application target name. Given with suffix .a for library and .elf for a
# standalone application.
TARGET = sha204_example_client_host.elf

# C source files located from the top-level source directory
CSRCS = \
       common/boards/security_xplained/security_xplained.c \
       common/components/crypto/sha204/example/example_client_host/sha204_example_client_host.c \
       common/components/crypto/sha204/example/example_utilities_led/sha204_example_led.c \
       common/components/crypto/sha204/example/example_utilities_log/sha204_example_log.c \
       common/components/crypto/sha204/sha204_comm.c      \
       common/components/crypto/sha204/sha204_command_marshaling.c \
       common/components/crypto/sha204/sha204_i2c.c       \
       common/components/crypto/sha204/xmega_sha204/sha204_timer.c \
       common/services/clock/xmega/sysclk.c               \
       common/services/ioport/xmega/ioport_compat.c       \
       common/services/serial/usart_serial.c              \
       common/services/sleepmgr/xmega/sleepmgr.c          \
       xmega/boards/xmega_a1_xplained/init.c              \
       xmega/drivers/tc/tc.c                              \
       xmega/drivers/twi/twim.c                           \
       xmega/drivers/twi/twis.c                           \
       xmega/drivers/usart/usart.c

# Assembler source files located from the top-level source directory
ASSRCS = \
       xmega/drivers/cpu/ccp.s

# Include path located from the top-level source directory
INC_PATH = \
       common/boards                                      \
       common/boards/security_xplained                    \
       common/components/crypto/sha204                    \
       common/components/crypto/sha204/example/example_client_host \
       common/components/crypto/sha204/example/example_client_host/atxmega128a1_xmega_a1_xplained_security \
       common/components/crypto/sha204/example/example_utilities_led \
       common/components/crypto/sha204/example/example_utilities_log \
       common/components/crypto/sha204/xmega_sha204       \
       common/services/clock                              \
       common/services/gpio                               \
       common/services/ioport                             \
       common/services/serial                             \
       common/services/serial/xmega_usart                 \
       common/services/sleepmgr                           \
       common/services/twi                                \
       common/utils                                       \
       xmega/boards                                       \
       xmega/boards/xmega_a1_xplained                     \
       xmega/drivers/cpu                                  \
       xmega/drivers/pmic                                 \
       xmega/drivers/sleep                                \
       xmega/drivers/tc                                   \
       xmega/drivers/twi                                  \
       xmega/drivers/usart                                \
       xmega/utils                                        \
       xmega/utils/preprocessor \
       common/components/crypto/sha204/example/example_client_host/atxmega128a1_xmega_a1_xplained_security/gcc

# Library paths from the top-level source directory
LIB_PATH = 

# Libraries to link with the project
LIBS = 

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
       -D BOARD=XMEGA_A1_XPLAINED                         \
       -D EXT_BOARD=SECURITY_XPLAINED                     \
       -D IOPORT_XMEGA_COMPAT

# Extra flags to use when linking
LDFLAGS =  \
       -Wl,--section-start=.BOOT=0x20000                 

# Pre- and post-build commands
PREBUILD_CMD = 
POSTBUILD_CMD = 