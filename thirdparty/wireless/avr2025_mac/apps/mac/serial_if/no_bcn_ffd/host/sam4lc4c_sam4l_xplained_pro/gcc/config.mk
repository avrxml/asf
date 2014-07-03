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
PRJ_PATH = ../../../../../../../../../..

# Target CPU architecture: cortex-m3, cortex-m4
ARCH = cortex-m4

# Target part: none, sam3n4 or sam4l4aa
PART = sam4lc4c

# Application target name. Given with suffix .a for library and .elf for a
# standalone application.
TARGET_FLASH = nobcn_ffd_host_flash.elf
TARGET_SRAM = nobcn_ffd_host_sram.elf

# List of C source files.
CSRCS = \
       common/services/clock/sam4l/dfll.c                 \
       common/services/clock/sam4l/osc.c                  \
       common/services/clock/sam4l/pll.c                  \
       common/services/clock/sam4l/sysclk.c               \
       common/services/delay/sam/cycle_counter.c          \
       common/services/serial/usart_serial.c              \
       common/utils/interrupt/interrupt_sam_nvic.c        \
       common/utils/stdio/read.c                          \
       common/utils/stdio/write.c                         \
       sam/boards/sam4l_xplained_pro/init.c               \
       sam/drivers/bpm/bpm.c                              \
       sam/drivers/flashcalw/flashcalw.c                  \
       sam/drivers/tc/tc.c                                \
       sam/drivers/usart/usart.c                          \
       sam/drivers/wdt/wdt_sam4l.c                        \
       sam/utils/cmsis/sam4l/source/templates/exceptions.c \
       sam/utils/cmsis/sam4l/source/templates/gcc/startup_sam4l.c \
       sam/utils/cmsis/sam4l/source/templates/system_sam4l.c \
       sam/utils/syscalls/gcc/syscalls.c                  \
       thirdparty/wireless/addons/sio2host/uart/sio2host.c \
       thirdparty/wireless/addons/sio2ncp/uart/sio2ncp.c  \
       thirdparty/wireless/avr2025_mac/addons/api_parser/src/api_decoder.c \
       thirdparty/wireless/avr2025_mac/addons/api_parser/src/api_encoder.c \
       thirdparty/wireless/avr2025_mac/addons/serial_if/src/serial_interface.c \
       thirdparty/wireless/avr2025_mac/apps/mac/serial_if/main.c \
       thirdparty/wireless/avr2025_mac/source/pal/common_sw_timer/common_sw_timer.c \
       thirdparty/wireless/services/common_hw_timer/sam/hw_timer.c

# List of assembler source files.
ASSRCS = 

# List of include paths.
INC_PATH = \
       common/boards                                      \
       common/services/clock                              \
       common/services/delay                              \
       common/services/ioport                             \
       common/services/serial                             \
       common/services/serial/sam_uart                    \
       common/utils                                       \
       common/utils/stdio/stdio_serial                    \
       sam/boards                                         \
       sam/boards/sam4l_xplained_pro                      \
       sam/drivers/bpm                                    \
       sam/drivers/flashcalw                              \
       sam/drivers/tc                                     \
       sam/drivers/usart                                  \
       sam/drivers/wdt                                    \
       sam/utils                                          \
       sam/utils/cmsis/sam4l/include                      \
       sam/utils/cmsis/sam4l/source/templates             \
       sam/utils/header_files                             \
       sam/utils/preprocessor                             \
       thirdparty/CMSIS/Include                           \
       thirdparty/CMSIS/Lib/GCC                           \
       thirdparty/wireless/addons/sio2host/uart           \
       thirdparty/wireless/addons/sio2ncp/uart            \
       thirdparty/wireless/avr2025_mac/addons/api_parser/inc \
       thirdparty/wireless/avr2025_mac/addons/serial_if/inc \
       thirdparty/wireless/avr2025_mac/apps/mac/serial_if \
       thirdparty/wireless/avr2025_mac/apps/mac/serial_if/no_bcn_ffd/host \
       thirdparty/wireless/avr2025_mac/apps/mac/serial_if/no_bcn_ffd/host/sam4lc4c_sam4l_xplained_pro \
       thirdparty/wireless/avr2025_mac/include            \
       thirdparty/wireless/avr2025_mac/source/mac/inc     \
       thirdparty/wireless/avr2025_mac/source/pal/common_sw_timer \
       thirdparty/wireless/services/common_hw_timer       \
       thirdparty/wireless/services/common_hw_timer/sam \
       thirdparty/wireless/avr2025_mac/apps/mac/serial_if/no_bcn_ffd/host/sam4lc4c_sam4l_xplained_pro/gcc

# Additional search paths for libraries.
LIB_PATH =  \
       thirdparty/CMSIS/Lib/GCC                           \
       thirdparty/wireless/services/common_hw_timer/sam/lib

# List of libraries to use during linking.
LIBS =  \
       arm_cortexM4l_math                                 \
       sam4l_lib_hw_timer                                 \
       m                                                 

# Path relative to top level directory pointing to a linker script.
LINKER_SCRIPT_FLASH = sam/utils/linker_scripts/sam4l/sam4l4/gcc/flash.ld
LINKER_SCRIPT_SRAM  = sam/utils/linker_scripts/sam4l/sam4l4/gcc/sram.ld

# Path relative to top level directory pointing to a linker script.
DEBUG_SCRIPT_FLASH = sam/boards/sam4l_xplained_pro/debug_scripts/gcc/sam4l_xplained_pro_flash.gdb
DEBUG_SCRIPT_SRAM  = sam/boards/sam4l_xplained_pro/debug_scripts/gcc/sam4l_xplained_pro_sram.gdb

# Project type parameter: all, sram or flash
PROJECT_TYPE        = flash

# Additional options for debugging. By default the common Makefile.in will
# add -g3.
DBGFLAGS = 

# Application optimization used during compilation and linking:
# -O0, -O1, -O2, -O3 or -Os
OPTIMIZATION = -O1

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
       -D ARM_MATH_CM4=true                               \
       -D BOARD=SAM4L_XPLAINED_PRO                        \
       -D ENABLE_TSTAMP                                   \
       -D FFD                                             \
       -D HIGHEST_STACK_LAYER=MAC                         \
       -D TEST_HARNESS                                    \
       -D _DEBUG_=0                                       \
       -D __SAM4LC4C__                                    \
       -D printf=iprintf                                  \
       -D scanf=iscanf

# Extra flags to use when linking
LDFLAGS = \

# Pre- and post-build commands
PREBUILD_CMD = 
POSTBUILD_CMD = 