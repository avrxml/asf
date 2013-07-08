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
PRJ_PATH = ../../../../../../../../../..

# Target CPU architecture: ap, ucr1, ucr2 or ucr3
ARCH = ucr3

# Target part: none, ap7xxx or uc3xxxxx
PART = uc64l4u

# Target device flash memory details (used by the avr32program programming
# tool: [cfi|internal]@address
FLASH = internal@0x80000000

# Clock source to use when programming; xtal, extclk or int
PROG_CLOCK = int

# Application target name. Given with suffix .a for library and .elf for a
# standalone application.
TARGET = bootloader_avr32.elf

# List of C source files.
CSRCS = \
       avr32/drivers/ast/ast.c                            \
       avr32/drivers/flashcdw/flashcdw.c                  \
       avr32/drivers/gpio/gpio.c                          \
       avr32/drivers/pm/pm_uc3l.c                         \
       avr32/drivers/pm/power_clocks_lib.c                \
       avr32/drivers/scif/scif_uc3l.c                     \
       avr32/drivers/usbc/usbc_device.c                   \
       avr32/services/freq_detect/freq_detect.c           \
       common/services/clock/uc3l/dfll.c                  \
       common/services/clock/uc3l/osc.c                   \
       common/services/clock/uc3l/pll.c                   \
       common/services/clock/uc3l/sysclk.c                \
       common/services/isp/flip/uc3/isp.c                 \
       common/services/usb/class/dfu_flip/device/bootloader/avr32/atucl3_l4/sysclk_auto.c \
       common/services/usb/class/dfu_flip/device/bootloader/avr32/intc/intc.c \
       common/services/usb/class/dfu_flip/device/bootloader/avr32/main.c \
       common/services/usb/class/dfu_flip/device/udi_dfu_atmel.c \
       common/services/usb/class/dfu_flip/device/udi_dfu_atmel_desc.c \
       common/services/usb/udc/udc.c

# List of assembler source files.
ASSRCS = \
       common/services/isp/flip/uc3/boot.S

# List of include paths.
INC_PATH = \
       avr32/boards                                       \
       avr32/boards/dummy                                 \
       avr32/drivers/ast                                  \
       avr32/drivers/cpu/cycle_counter                    \
       avr32/drivers/flashcdw                             \
       avr32/drivers/gpio                                 \
       avr32/drivers/pm                                   \
       avr32/drivers/scif                                 \
       avr32/drivers/usbc                                 \
       avr32/services/freq_detect                         \
       avr32/utils                                        \
       avr32/utils/preprocessor                           \
       common/boards                                      \
       common/services/clock                              \
       common/services/isp/flip                           \
       common/services/isp/flip/uc3                       \
       common/services/usb                                \
       common/services/usb/class/dfu_flip                 \
       common/services/usb/class/dfu_flip/device          \
       common/services/usb/class/dfu_flip/device/bootloader/avr32 \
       common/services/usb/class/dfu_flip/device/bootloader/avr32/atucl3_l4/iar \
       common/services/usb/class/dfu_flip/device/bootloader/avr32/conf \
       common/services/usb/class/dfu_flip/device/bootloader/avr32/intc \
       common/services/usb/udc                            \
       common/utils \
       common/services/usb/class/dfu_flip/device/bootloader/avr32/atucl3_l4/gcc

# Additional search paths for libraries.
LIB_PATH = 

# List of libraries to use during linking.
LIBS = 

# Path relative to top level directory pointing to a linker script.
LINKER_SCRIPT = common/services/usb/class/dfu_flip/device/bootloader/avr32/atucl3_l4/gcc/link_at32ucl3_l4-isp.lds

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
       -D BOARD=DUMMY_BOARD                               \
       -D UDD_ENABLE

# Extra flags to use when linking
LDFLAGS = \
       -nostartfiles
