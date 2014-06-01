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
PRJ_PATH = ../../../../../../../../..

# Target CPU architecture: ap, ucr1, ucr2 or ucr3
ARCH = ucr3fp

# Target part: none, ap7xxx or uc3xxxxx
PART = uc3c0512c

# Target device flash memory details (used by the avr32program programming
# tool: [cfi|internal]@address
FLASH = internal@0x80000000

# Clock source to use when programming; xtal, extclk or int
PROG_CLOCK = int

# Application target name. Given with suffix .a for library and .elf for a
# standalone application.
TARGET = device_example.elf

# List of C source files.
CSRCS = \
       avr32/boards/uc3c_ek/init.c                        \
       avr32/boards/uc3c_ek/led.c                         \
       avr32/drivers/flashc/flashc.c                      \
       avr32/drivers/gpio/gpio.c                          \
       avr32/drivers/intc/intc.c                          \
       avr32/drivers/pm/pm_uc3c.c                         \
       avr32/drivers/pm/power_clocks_lib.c                \
       avr32/drivers/scif/scif_uc3c.c                     \
       avr32/drivers/spi/spi.c                            \
       avr32/drivers/usbc/usbc_device.c                   \
       common/components/memory/data_flash/at45dbx/at45dbx.c \
       common/components/memory/data_flash/at45dbx/at45dbx_mem.c \
       common/components/memory/sd_mmc/sd_mmc.c           \
       common/components/memory/sd_mmc/sd_mmc_mem.c       \
       common/components/memory/sd_mmc/sd_mmc_spi.c       \
       common/components/memory/virtual_mem/virtual_mem.c \
       common/services/clock/uc3c/osc.c                   \
       common/services/clock/uc3c/pll.c                   \
       common/services/clock/uc3c/sysclk.c                \
       common/services/sleepmgr/uc3/sleepmgr.c            \
       common/services/spi/uc3_spi/spi_master.c           \
       common/services/storage/ctrl_access/ctrl_access.c  \
       common/services/usb/class/msc/device/example/at32uc3c0512c_uc3c_ek/ui.c \
       common/services/usb/class/msc/device/example/main.c \
       common/services/usb/class/msc/device/example/memories_initialization_uc3.c \
       common/services/usb/class/msc/device/udi_msc.c     \
       common/services/usb/class/msc/device/udi_msc_desc.c \
       common/services/usb/udc/udc.c

# List of assembler source files.
ASSRCS = \
       avr32/drivers/intc/exception.S                     \
       avr32/utils/startup/startup_uc3.S                  \
       avr32/utils/startup/trampoline_uc3.S

# List of include paths.
INC_PATH = \
       avr32/boards                                       \
       avr32/boards/uc3c_ek                               \
       avr32/drivers/cpu/cycle_counter                    \
       avr32/drivers/flashc                               \
       avr32/drivers/gpio                                 \
       avr32/drivers/intc                                 \
       avr32/drivers/pm                                   \
       avr32/drivers/scif                                 \
       avr32/drivers/spi                                  \
       avr32/drivers/usbc                                 \
       avr32/utils                                        \
       avr32/utils/preprocessor                           \
       common/boards                                      \
       common/components/memory/data_flash/at45dbx        \
       common/components/memory/sd_mmc                    \
       common/components/memory/virtual_mem               \
       common/services/clock                              \
       common/services/delay                              \
       common/services/ioport                             \
       common/services/sleepmgr                           \
       common/services/spi                                \
       common/services/spi/uc3_spi                        \
       common/services/storage/ctrl_access                \
       common/services/usb                                \
       common/services/usb/class/msc                      \
       common/services/usb/class/msc/device               \
       common/services/usb/class/msc/device/example       \
       common/services/usb/class/msc/device/example/at32uc3c0512c_uc3c_ek \
       common/services/usb/udc                            \
       common/utils \
       common/services/usb/class/msc/device/example/at32uc3c0512c_uc3c_ek/gcc

# Additional search paths for libraries.
LIB_PATH = 

# List of libraries to use during linking.
LIBS = 

# Path relative to top level directory pointing to a linker script.
LINKER_SCRIPT = avr32/utils/linker_scripts/at32uc3c/0512c/gcc/link_uc3c0512c.lds

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
       -D ACCESS_USB_ENABLED                              \
       -D AT45DBX_ENABLE                                  \
       -D BOARD=UC3C_EK                                   \
       -D SD_MMC_ENABLE                                   \
       -D UDD_ENABLE                                      \
       -D VIRTUAL_MEMORY_ENABLE

# Extra flags to use when linking
LDFLAGS = \
       -nostartfiles -Wl,-e,_trampoline

# Pre- and post-build commands
PREBUILD_CMD = 
POSTBUILD_CMD = 