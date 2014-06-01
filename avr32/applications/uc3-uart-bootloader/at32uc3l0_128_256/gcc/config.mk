#
# Copyright (c) 2009-2010 Atmel Corporation. All rights reserved.
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
PRJ_PATH = ../../../../..

# Target CPU architecture: ap, ucr1, ucr2 or ucr3
ARCH = ucr3

# Target part: none, ap7xxx or uc3xxxxx
PART = uc3l0128

# Target device flash memory details (used by the avr32program programming
# tool: [cfi|internal]@address
FLASH = internal@0x80000000

# Clock source to use when programming; xtal, extclk or int
PROG_CLOCK = int

# Application target name. Given with suffix .a for library and .elf for a
# standalone application.
TARGET = uc3_uart_bootloader.elf

# List of C source files.
CSRCS = \
       avr32/applications/uc3-uart-bootloader/clocks/conf_clocks.c \
       avr32/applications/uc3-uart-bootloader/intc/intc.c \
       avr32/applications/uc3-uart-bootloader/isp.c       \
       avr32/applications/uc3-uart-bootloader/uart_dfu.c  \
       avr32/drivers/flashcdw/flashcdw.c                  \
       avr32/drivers/gpio/gpio.c                          \
       avr32/drivers/pm/pm_uc3l.c                         \
       avr32/drivers/pm/power_clocks_lib.c                \
       avr32/drivers/scif/scif_uc3l.c                     \
       avr32/drivers/usart/usart.c

# List of assembler source files.
ASSRCS = \
       avr32/applications/uc3-uart-bootloader/boot/boot.S

# List of include paths.
INC_PATH = \
       avr32/applications/uc3-uart-bootloader             \
       avr32/applications/uc3-uart-bootloader/boot        \
       avr32/applications/uc3-uart-bootloader/conf        \
       avr32/applications/uc3-uart-bootloader/intc        \
       avr32/boards                                       \
       avr32/boards/dummy                                 \
       avr32/drivers/cpu/cycle_counter                    \
       avr32/drivers/flashcdw                             \
       avr32/drivers/gpio                                 \
       avr32/drivers/pm                                   \
       avr32/drivers/scif                                 \
       avr32/drivers/usart                                \
       avr32/utils                                        \
       avr32/utils/preprocessor                           \
       common/boards                                      \
       common/utils \
       avr32/applications/uc3-uart-bootloader/at32uc3l0_128_256/gcc

# Additional search paths for libraries.
LIB_PATH = 

# List of libraries to use during linking.
LIBS = 

# Path relative to top level directory pointing to a linker script.
LINKER_SCRIPT = avr32/applications/uc3-uart-bootloader/at32uc3l0_128_256/link_at32uc3l0128-isp.lds

# Additional options for debugging. By default the common Makefile.in will
# add -g3.
DBGFLAGS = 

# Application optimization used during compilation and linking:
# -O0, -O1, -O2, -O3 or -Os
OPTIMIZATION = -Os

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
       -D BOARD=DUMMY_BOARD

# Extra flags to use when linking
LDFLAGS = \
       -nostartfiles

# Pre- and post-build commands
PREBUILD_CMD = 
POSTBUILD_CMD = 