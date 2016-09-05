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
PART = sam4sd32c

# Application target name. Given with suffix .a for library and .elf for a
# standalone application.
TARGET_FLASH = htpt_temperature_sensor_sam4s_xplained_pro_flash.elf
TARGET_SRAM = htpt_temperature_sensor_sam4s_xplained_pro_sram.elf

# List of C source files.
CSRCS = \
       common/components/memory/eeprom/at30tse75x/at30tse75x.c \
       common/services/clock/sam4s/sysclk.c               \
       common/services/delay/sam/cycle_counter.c          \
       common/services/serial/usart_serial.c              \
       common/utils/interrupt/interrupt_sam_nvic.c        \
       common/utils/stdio/read.c                          \
       common/utils/stdio/write.c                         \
       sam/boards/sam4s_xplained_pro/init.c               \
       sam/drivers/pdc/pdc.c                              \
       sam/drivers/pio/pio.c                              \
       sam/drivers/pio/pio_handler.c                      \
       sam/drivers/pmc/pmc.c                              \
       sam/drivers/pmc/sleep.c                            \
       sam/drivers/tc/tc.c                                \
       sam/drivers/twi/twi.c                              \
       sam/drivers/uart/uart.c                            \
       sam/drivers/usart/usart.c                          \
       sam/drivers/wdt/wdt.c                              \
       sam/utils/cmsis/sam4s/source/templates/gcc/startup_sam4s.c \
       sam/utils/cmsis/sam4s/source/templates/system_sam4s.c \
       sam/utils/syscalls/gcc/syscalls.c                  \
       thirdparty/wireless/ble_sdk/apps/htpt_temperature_sensor/htpt_app.c \
       thirdparty/wireless/ble_sdk/ble_services/ble_mgr/ble_manager.c \
       thirdparty/wireless/ble_sdk/ble_services/device_infomation/device_info.c \
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
       common/components/memory/eeprom/at30tse75x         \
       common/services/clock                              \
       common/services/delay                              \
       common/services/gpio                               \
       common/services/ioport                             \
       common/services/serial                             \
       common/services/serial/sam_uart                    \
       common/services/twi                                \
       common/utils                                       \
       common/utils/stdio/stdio_serial                    \
       sam/boards                                         \
       sam/boards/sam4s_xplained_pro                      \
       sam/drivers/pdc                                    \
       sam/drivers/pdc/pdc_uart_example                   \
       sam/drivers/pio                                    \
       sam/drivers/pmc                                    \
       sam/drivers/tc                                     \
       sam/drivers/twi                                    \
       sam/drivers/uart                                   \
       sam/drivers/usart                                  \
       sam/drivers/wdt                                    \
       sam/utils                                          \
       sam/utils/cmsis/sam4s/include                      \
       sam/utils/cmsis/sam4s/source/templates             \
       sam/utils/header_files                             \
       sam/utils/preprocessor                             \
       thirdparty/CMSIS/Include                           \
       thirdparty/CMSIS/Lib/GCC                           \
       thirdparty/wireless/ble_sdk/apps/config/sam4s      \
       thirdparty/wireless/ble_sdk/apps/htpt_temperature_sensor \
       thirdparty/wireless/ble_sdk/ble_services/ble_mgr   \
       thirdparty/wireless/ble_sdk/ble_services/device_infomation \
       thirdparty/wireless/ble_sdk/inc                    \
       thirdparty/wireless/ble_sdk/services/console       \
       thirdparty/wireless/ble_sdk/services/serial/uart   \
       thirdparty/wireless/ble_sdk/services/serial_fifo   \
       thirdparty/wireless/ble_sdk/services/timer         \
       thirdparty/wireless/ble_sdk/utils \
       thirdparty/wireless/ble_sdk/apps/htpt_temperature_sensor/sam4s_xplained_pro/gcc

# Additional search paths for libraries.
LIB_PATH =  \
       thirdparty/CMSIS/Lib/GCC                           \
       thirdparty/wireless/ble_sdk/lib/cm4f/gcc          

# List of libraries to use during linking.
LIBS =  \
       arm_cortexM4l_math                                 \
       ble_sdk                                            \
       m                                                 

# Path relative to top level directory pointing to a linker script.
LINKER_SCRIPT_FLASH = sam/utils/linker_scripts/sam4s/sam4sd32/gcc/flash.ld
LINKER_SCRIPT_SRAM  = sam/utils/linker_scripts/sam4s/sam4sd32/gcc/sram.ld

# Path relative to top level directory pointing to a linker script.
DEBUG_SCRIPT_FLASH = sam/boards/sam4s_xplained_pro/debug_scripts/gcc/sam4s_xplained_pro_flash.gdb
DEBUG_SCRIPT_SRAM  = sam/boards/sam4s_xplained_pro/debug_scripts/gcc/sam4s_xplained_pro_sram.gdb

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
       -D ATT_DB_MEMORY                                   \
       -D BLE_DEVICE_ROLE=BLE_ROLE_PERIPHERAL             \
       -D BOARD=SAM4S_XPLAINED_PRO                        \
       -D BOARD_AT30TSE_DEVICE_ADDR=0x0                   \
       -D DEVICE_INFORMATION_SERVICE                      \
       -D ENABLE_POWER_SAVE                               \
       -D HTPT_SERVICE                                    \
       -D NENABLE_PTS=false                               \
       -D NEW_EVT_HANDLER                                 \
       -D UART_FLOWCONTROL_4WIRE_MODE=true                \
       -D UART_FLOWCONTROL_6WIRE_MODE=false               \
       -D __SAM4SD32C__                                   \
       -D printf=iprintf                                  \
       -D scanf=iscanf

# Extra flags to use when linking
LDFLAGS = \

# Pre- and post-build commands
PREBUILD_CMD = 
POSTBUILD_CMD = 