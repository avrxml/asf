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
PRJ_PATH = ../../../..

# Target CPU architecture: ap, ucr1, ucr2 or ucr3
ARCH = ucr2

# Target part: none, ap7xxx or uc3xxxxx
PART = uc3a3256

# Target device flash memory details (used by the avr32program programming
# tool: [cfi|internal]@address
FLASH = internal@0x80000000

# Clock source to use when programming; xtal, extclk or int
PROG_CLOCK = int

# Application target name. Given with suffix .a for library and .elf for a
# standalone application.
TARGET = sensors_lightprox_wake.elf

# List of C source files.
CSRCS = \
       avr32/boards/uc3_a3_xplained/init.c                \
       avr32/boards/uc3_a3_xplained/led.c                 \
       avr32/drivers/eic/eic.c                            \
       avr32/drivers/flashc/flashc.c                      \
       avr32/drivers/gpio/gpio.c                          \
       avr32/drivers/intc/intc.c                          \
       avr32/drivers/pm/pm.c                              \
       avr32/drivers/pm/pm_conf_clocks.c                  \
       avr32/drivers/pm/power_clocks_lib.c                \
       avr32/drivers/twim/twim.c                          \
       avr32/drivers/usbb/usbb_device.c                   \
       common/applications/sensors/lightprox_wake/main.c  \
       common/boards/sensors_xplained/sensors_xplained.c  \
       common/services/clock/uc3a3_a4/sysclk.c            \
       common/services/sensors/drivers/akm/ak8975.c       \
       common/services/sensors/drivers/bosch/bma020.c     \
       common/services/sensors/drivers/bosch/bma150.c     \
       common/services/sensors/drivers/bosch/bma180.c     \
       common/services/sensors/drivers/bosch/bma220.c     \
       common/services/sensors/drivers/bosch/bma222.c     \
       common/services/sensors/drivers/bosch/bma250.c     \
       common/services/sensors/drivers/bosch/bma_axis.inc.c \
       common/services/sensors/drivers/bosch/bmp.c        \
       common/services/sensors/drivers/honeywell/hmc5883l.c \
       common/services/sensors/drivers/invensense/imu3000.c \
       common/services/sensors/drivers/invensense/itg3200.c \
       common/services/sensors/drivers/kionix/kxtf9.c     \
       common/services/sensors/drivers/osram/sfh5712.c    \
       common/services/sensors/drivers/osram/sfh7770.c    \
       common/services/sensors/physics/physics.c          \
       common/services/sensors/sensor.c                   \
       common/services/sensors/sensor_bus.c               \
       common/services/sensors/sensor_nvram.c             \
       common/services/sensors/sensor_platform.c          \
       common/services/sleepmgr/uc3/sleepmgr.c            \
       common/services/usb/class/cdc/device/udi_cdc.c     \
       common/services/usb/class/cdc/device/udi_cdc_desc.c \
       common/services/usb/udc/udc.c                      \
       common/utils/stdio/read.c                          \
       common/utils/stdio/stdio_usb/stdio_usb.c           \
       common/utils/stdio/write.c

# List of assembler source files.
ASSRCS = \
       avr32/drivers/intc/exception.S                     \
       avr32/utils/startup/startup_uc3.S                  \
       avr32/utils/startup/trampoline_uc3.S

# List of include paths.
INC_PATH = \
       avr32/applications/uc3-lightprox-wake/conf         \
       avr32/boards                                       \
       avr32/boards/uc3_a3_xplained                       \
       avr32/drivers/cpu/cycle_counter                    \
       avr32/drivers/eic                                  \
       avr32/drivers/flashc                               \
       avr32/drivers/gpio                                 \
       avr32/drivers/intc                                 \
       avr32/drivers/pm                                   \
       avr32/drivers/twim                                 \
       avr32/drivers/usbb                                 \
       avr32/utils                                        \
       avr32/utils/preprocessor                           \
       common/boards                                      \
       common/services/clock                              \
       common/services/delay                              \
       common/services/sensors                            \
       common/services/sensors/module_config              \
       common/services/sleepmgr                           \
       common/services/twi                                \
       common/services/usb                                \
       common/services/usb/class/cdc                      \
       common/services/usb/class/cdc/device               \
       common/services/usb/udc                            \
       common/utils                                       \
       common/utils/stdio/stdio_usb \
       avr32/applications/uc3-lightprox-wake/gcc         

# Additional search paths for libraries.
LIB_PATH = 

# List of libraries to use during linking.
LIBS =  \
       m                                                 

# Path relative to top level directory pointing to a linker script.
LINKER_SCRIPT = avr32/utils/linker_scripts/at32uc3a3/256/gcc/link_uc3a3256.lds

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
       -D BOARD=UC3_A3_XPLAINED                           \
       -D CONF_STDIO_REDIRECT                             \
       -D EXT_BOARD=SENSORS_XPLAINED_LIGHTPROX_1          \
       -D UDD_ENABLE

# Extra flags to use when linking
LDFLAGS = \
       -nostartfiles -Wl,-e,_trampoline

# Pre- and post-build commands
PREBUILD_CMD = 
POSTBUILD_CMD = 