#
# Copyright (c) 2011 Atmel Corporation. All rights reserved.
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

# Target CPU architecture: cortex-m3, cortex-m4
ARCH = cortex-m4

# Target part: none, sam3n4 or sam4l4aa
PART = sam4e16e

# Application target name. Given with suffix .a for library and .elf for a
# standalone application.
TARGET_FLASH = example_hids_cdc_msc_flash.elf
TARGET_SRAM = example_hids_cdc_msc_sram.elf

# List of C source files.
CSRCS = \
       common/components/memory/virtual_mem/virtual_mem.c \
       common/services/clock/sam4e/sysclk.c               \
       common/services/sleepmgr/sam/sleepmgr.c            \
       common/services/storage/ctrl_access/ctrl_access.c  \
       common/services/usb/class/cdc/device/udi_cdc.c     \
       common/services/usb/class/composite/device/example2/main.c \
       common/services/usb/class/composite/device/example2/memories_initialization_sam.c \
       common/services/usb/class/composite/device/example2/sam4e16e_sam4e_xplained_pro/ui.c \
       common/services/usb/class/composite/device/example2/uart_sam.c \
       common/services/usb/class/composite/device/udi_composite_desc.c \
       common/services/usb/class/hid/device/kbd/udi_hid_kbd.c \
       common/services/usb/class/hid/device/mouse/udi_hid_mouse.c \
       common/services/usb/class/hid/device/udi_hid.c     \
       common/services/usb/class/msc/device/udi_msc.c     \
       common/services/usb/udc/udc.c                      \
       common/utils/interrupt/interrupt_sam_nvic.c        \
       sam/boards/sam4e_xplained_pro/init.c               \
       sam/drivers/matrix/matrix.c                        \
       sam/drivers/pio/pio.c                              \
       sam/drivers/pio/pio_handler.c                      \
       sam/drivers/pmc/pmc.c                              \
       sam/drivers/pmc/sleep.c                            \
       sam/drivers/udp/udp_device.c                       \
       sam/drivers/usart/usart.c                          \
       sam/utils/cmsis/sam4e/source/templates/exceptions.c \
       sam/utils/cmsis/sam4e/source/templates/gcc/startup_sam4e.c \
       sam/utils/cmsis/sam4e/source/templates/system_sam4e.c \
       sam/utils/syscalls/gcc/syscalls.c

# List of assembler source files.
ASSRCS = 

# List of include paths.
INC_PATH = \
       common/boards                                      \
       common/components/memory/virtual_mem               \
       common/services/clock                              \
       common/services/ioport                             \
       common/services/sleepmgr                           \
       common/services/storage/ctrl_access                \
       common/services/usb                                \
       common/services/usb/class/cdc                      \
       common/services/usb/class/cdc/device               \
       common/services/usb/class/composite/device         \
       common/services/usb/class/composite/device/example2 \
       common/services/usb/class/composite/device/example2/sam4e16e_sam4e_xplained_pro \
       common/services/usb/class/hid                      \
       common/services/usb/class/hid/device               \
       common/services/usb/class/hid/device/kbd           \
       common/services/usb/class/hid/device/mouse         \
       common/services/usb/class/msc                      \
       common/services/usb/class/msc/device               \
       common/services/usb/udc                            \
       common/utils                                       \
       sam/boards                                         \
       sam/boards/sam4e_xplained_pro                      \
       sam/drivers/matrix                                 \
       sam/drivers/pio                                    \
       sam/drivers/pmc                                    \
       sam/drivers/udp                                    \
       sam/drivers/usart                                  \
       sam/utils                                          \
       sam/utils/cmsis/sam4e/include                      \
       sam/utils/cmsis/sam4e/source/templates             \
       sam/utils/fpu                                      \
       sam/utils/header_files                             \
       sam/utils/preprocessor                             \
       thirdparty/CMSIS/Include                           \
       thirdparty/CMSIS/Lib/GCC \
       common/services/usb/class/composite/device/example2/sam4e16e_sam4e_xplained_pro/gcc

# Additional search paths for libraries.
LIB_PATH =  \
       thirdparty/CMSIS/Lib/GCC                          

# List of libraries to use during linking.
LIBS =  \
       arm_cortexM4lf_math_softfp                         \
       m                                                 

# Path relative to top level directory pointing to a linker script.
LINKER_SCRIPT_FLASH = sam/utils/linker_scripts/sam4e/sam4e16/gcc/flash.ld
LINKER_SCRIPT_SRAM  = sam/utils/linker_scripts/sam4e/sam4e16/gcc/sram.ld

# Path relative to top level directory pointing to a linker script.
DEBUG_SCRIPT_FLASH = sam/boards/sam4e_xplained_pro/debug_scripts/gcc/sam4e_xplained_pro_flash.gdb
DEBUG_SCRIPT_SRAM  = sam/boards/sam4e_xplained_pro/debug_scripts/gcc/sam4e_xplained_pro_sram.gdb

# Project type parameter: all, sram or flash
PROJECT_TYPE        = all

# Additional options for debugging. By default the common Makefile.in will
# add -g3.
DBGFLAGS = 

# Application optimization used during compilation and linking:
# -O0, -O1, -O2, -O3 or -Os
OPTIMIZATION = -O1

# Extra flags to use when archiving.
ARFLAGS = 

# Extra flags to use when assembling.
ASFLAGS =  \
       -mfloat-abi=softfp                                 \
       -mfpu=fpv4-sp-d16                                 

# Extra flags to use when compiling.
CFLAGS =  \
       -mfloat-abi=softfp                                 \
       -mfpu=fpv4-sp-d16                                 

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
       -D ARM_MATH_CM4=true                               \
       -D BOARD=SAM4E_XPLAINED_PRO                        \
       -D UDD_ENABLE                                      \
       -D VIRTUAL_MEMORY_ENABLE                           \
       -D __SAM4E16E__                                    \
       -D printf=iprintf                                  \
       -D scanf=iscanf

# Extra flags to use when linking
LDFLAGS = \

# Pre- and post-build commands
PREBUILD_CMD = 
POSTBUILD_CMD = 