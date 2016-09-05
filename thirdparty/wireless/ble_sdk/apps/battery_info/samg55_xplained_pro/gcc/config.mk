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
PRJ_PATH = ../../../../../../..

# Target CPU architecture: cortex-m3, cortex-m4
ARCH = cortex-m4

# Target part: none, sam3n4 or sam4l4aa
PART = samg55j19

# Application target name. Given with suffix .a for library and .elf for a
# standalone application.
TARGET_FLASH = battery_info_samg55_xplained_pro_flash.elf
TARGET_SRAM = battery_info_samg55_xplained_pro_sram.elf

# List of C source files.
CSRCS = \
       common/services/clock/samg/sysclk.c                \
       common/services/delay/sam/cycle_counter.c          \
       common/services/serial/usart_serial.c              \
       common/services/sleepmgr/sam/sleepmgr.c            \
       common/utils/interrupt/interrupt_sam_nvic.c        \
       common/utils/stdio/read.c                          \
       common/utils/stdio/write.c                         \
       sam/boards/samg55_xplained_pro/board_init.c        \
       sam/drivers/efc/efc.c                              \
       sam/drivers/flexcom/flexcom.c                      \
       sam/drivers/pdc/pdc.c                              \
       sam/drivers/pio/pio.c                              \
       sam/drivers/pio/pio_handler.c                      \
       sam/drivers/pmc/pmc.c                              \
       sam/drivers/pmc/sleep.c                            \
       sam/drivers/supc/supc.c                            \
       sam/drivers/tc/tc.c                                \
       sam/drivers/usart/usart.c                          \
       sam/utils/cmsis/samg/samg55/source/templates/gcc/startup_samg55.c \
       sam/utils/cmsis/samg/samg55/source/templates/system_samg55.c \
       sam/utils/syscalls/gcc/syscalls.c                  \
       thirdparty/wireless/ble_sdk/apps/battery_info/battery_info.c \
       thirdparty/wireless/ble_sdk/ble_services/battery/battery.c \
       thirdparty/wireless/ble_sdk/ble_services/ble_mgr/ble_manager.c \
       thirdparty/wireless/ble_sdk/services/console/sam/console_serial.c \
       thirdparty/wireless/ble_sdk/services/serial/uart/sam/serial_drv.c \
       thirdparty/wireless/ble_sdk/services/serial_fifo/serial_fifo.c \
       thirdparty/wireless/ble_sdk/services/timer/sam/timer_hw.c \
       thirdparty/wireless/ble_sdk/src/platform.c

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
       common/services/sleepmgr                           \
       common/utils                                       \
       common/utils/stdio/stdio_serial                    \
       sam/boards                                         \
       sam/boards/samg55_xplained_pro                     \
       sam/drivers/efc                                    \
       sam/drivers/flexcom                                \
       sam/drivers/pdc                                    \
       sam/drivers/pdc/pdc_uart_example                   \
       sam/drivers/pio                                    \
       sam/drivers/pmc                                    \
       sam/drivers/supc                                   \
       sam/drivers/tc                                     \
       sam/drivers/usart                                  \
       sam/utils                                          \
       sam/utils/cmsis/samg/samg55/include                \
       sam/utils/cmsis/samg/samg55/source/templates       \
       sam/utils/fpu                                      \
       sam/utils/header_files                             \
       sam/utils/preprocessor                             \
       thirdparty/CMSIS/Include                           \
       thirdparty/CMSIS/Lib/GCC                           \
       thirdparty/wireless/ble_sdk/apps/battery_info      \
       thirdparty/wireless/ble_sdk/apps/config/samg55     \
       thirdparty/wireless/ble_sdk/ble_services/battery   \
       thirdparty/wireless/ble_sdk/ble_services/ble_mgr   \
       thirdparty/wireless/ble_sdk/inc                    \
       thirdparty/wireless/ble_sdk/services/console       \
       thirdparty/wireless/ble_sdk/services/serial/uart   \
       thirdparty/wireless/ble_sdk/services/serial_fifo   \
       thirdparty/wireless/ble_sdk/services/timer         \
       thirdparty/wireless/ble_sdk/utils \
       thirdparty/wireless/ble_sdk/apps/battery_info/samg55_xplained_pro/gcc

# Additional search paths for libraries.
LIB_PATH =  \
       thirdparty/CMSIS/Lib/GCC                           \
       thirdparty/wireless/ble_sdk/lib/cm4f/gcc          

# List of libraries to use during linking.
LIBS =  \
       arm_cortexM4lf_math_softfp                         \
       ble_sdk                                            \
       m                                                 

# Path relative to top level directory pointing to a linker script.
LINKER_SCRIPT_FLASH = sam/utils/linker_scripts/samg/samg55j19/gcc/flash.ld
LINKER_SCRIPT_SRAM  = sam/utils/linker_scripts/samg/samg55j19/gcc/sram.ld

# Path relative to top level directory pointing to a linker script.
DEBUG_SCRIPT_FLASH = sam/boards/samg55_xplained_pro/debug_scripts/gcc/samg55_xplained_pro_flash.gdb
DEBUG_SCRIPT_SRAM  = sam/boards/samg55_xplained_pro/debug_scripts/gcc/samg55_xplained_pro_sram.gdb

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
       -D ARM_MATH_CM4=true                               \
       -D ATT_DB_MEMORY                                   \
       -D BATTERY_SERVICE                                 \
       -D BLE_DEVICE_ROLE=BLE_ROLE_PERIPHERAL             \
       -D BOARD=SAMG55_XPLAINED_PRO                       \
       -D ENABLE_POWER_SAVE                               \
       -D NENABLE_PTS                                     \
       -D NEW_EVT_HANDLER                                 \
       -D UART_FLOWCONTROL_4WIRE_MODE=true                \
       -D UART_FLOWCONTROL_6WIRE_MODE=false               \
       -D __SAMG55J19__                                   \
       -D printf=iprintf                                  \
       -D scanf=iscanf

# Extra flags to use when linking
LDFLAGS = \

# Pre- and post-build commands
PREBUILD_CMD = 
POSTBUILD_CMD = 