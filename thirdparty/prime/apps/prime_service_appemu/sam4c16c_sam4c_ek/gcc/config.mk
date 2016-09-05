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
PRJ_PATH = ../../../../../..

# Target CPU architecture: cortex-m3, cortex-m4
ARCH = cortex-m4

# Target part: none, sam3n4 or sam4l4aa
PART = sam4c16c:0

# Application target name. Given with suffix .a for library and .elf for a
# standalone application.
TARGET_FLASH = apps_prime_service_appemu_flash.elf
TARGET_SRAM = apps_prime_service_appemu_sram.elf

# List of C source files.
CSRCS = \
       common/services/clock/sam4c/sysclk.c               \
       common/services/serial/usart_serial.c              \
       common/utils/interrupt/interrupt_sam_nvic.c        \
       common/utils/stdio/read.c                          \
       common/utils/stdio/write.c                         \
       sam/boards/sam4c_ek/init.c                         \
       sam/drivers/efc/efc.c                              \
       sam/drivers/gpbr/gpbr.c                            \
       sam/drivers/pdc/pdc.c                              \
       sam/drivers/pio/pio.c                              \
       sam/drivers/pio/pio_handler.c                      \
       sam/drivers/pmc/pmc.c                              \
       sam/drivers/pmc/sleep.c                            \
       sam/drivers/spi/spi.c                              \
       sam/drivers/tc/tc.c                                \
       sam/drivers/uart/uart.c                            \
       sam/drivers/usart/usart.c                          \
       sam/drivers/wdt/wdt.c                              \
       sam/services/flash_efc/flash_efc.c                 \
       sam/services/plc/buart_if/buart_if.c               \
       sam/services/plc/busart_if/busart_if.c             \
       sam/services/plc/hal/prime/hal.c                   \
       sam/services/plc/pcrc/pcrc.c                       \
       sam/services/plc/pplc_if/atpl230/pplc_if.c         \
       sam/services/plc/usi/usi.c                         \
       sam/utils/cmsis/sam4c/source/templates/gcc/startup_sam4c.c \
       sam/utils/cmsis/sam4c/source/templates/system_sam4c.c \
       sam/utils/syscalls/gcc/syscalls.c                  \
       thirdparty/freertos/freertos-7.3.0/source/FreeRTOS_CLI.c \
       thirdparty/freertos/freertos-7.3.0/source/list.c   \
       thirdparty/freertos/freertos-7.3.0/source/portable/gcc/sam/port.c \
       thirdparty/freertos/freertos-7.3.0/source/portable/memmang/heap_4.c \
       thirdparty/freertos/freertos-7.3.0/source/queue.c  \
       thirdparty/freertos/freertos-7.3.0/source/tasks.c  \
       thirdparty/freertos/freertos-7.3.0/source/timers.c \
       thirdparty/prime/apps/prime_service_appemu/app_emu.c \
       thirdparty/prime/apps/prime_service_appemu/app_emu_common.c \
       thirdparty/prime/apps/prime_service_appemu/app_emu_service.c \
       thirdparty/prime/apps/prime_service_appemu/main.c  \
       thirdparty/prime/mac/mac_1_3/module_config/conf_mac.c \
       thirdparty/prime/mac/mac_1_3/source/PrimeVersion.c \
       thirdparty/prime/oss/oss_if.c                      \
       thirdparty/prime/pal/atpl230_primemac13/source/pal.c \
       thirdparty/prime/phy/atpl230/coupling_config/conf_atpl230.c \
       thirdparty/prime/phy/atpl230/source/atpl230.c      \
       thirdparty/prime/phy/atpl230/source/atpl230_iir_filter.c \
       thirdparty/prime/sscs/sscs-4-32/addons/serial_if/serial_432_if.c

# List of assembler source files.
ASSRCS = 

# List of include paths.
INC_PATH = \
       common/boards                                      \
       common/services/clock                              \
       common/services/ioport                             \
       common/services/serial                             \
       common/services/serial/sam_uart                    \
       common/utils                                       \
       common/utils/stdio/stdio_serial                    \
       sam/boards                                         \
       sam/boards/sam4c_ek                                \
       sam/drivers/efc                                    \
       sam/drivers/gpbr                                   \
       sam/drivers/pdc                                    \
       sam/drivers/pdc/pdc_uart_example                   \
       sam/drivers/pio                                    \
       sam/drivers/pmc                                    \
       sam/drivers/spi                                    \
       sam/drivers/tc                                     \
       sam/drivers/uart                                   \
       sam/drivers/usart                                  \
       sam/drivers/wdt                                    \
       sam/services/flash_efc                             \
       sam/services/plc/buart_if                          \
       sam/services/plc/busart_if                         \
       sam/services/plc/hal/prime                         \
       sam/services/plc/pcrc                              \
       sam/services/plc/pplc_if/atpl230                   \
       sam/services/plc/usi                               \
       sam/utils                                          \
       sam/utils/cmsis/sam4c/include                      \
       sam/utils/cmsis/sam4c/source/templates             \
       sam/utils/header_files                             \
       sam/utils/preprocessor                             \
       thirdparty/CMSIS/Include                           \
       thirdparty/CMSIS/Lib/GCC                           \
       thirdparty/freertos/freertos-7.3.0/source/include  \
       thirdparty/freertos/freertos-7.3.0/source/portable/gcc/sam \
       thirdparty/prime/apps/prime_service_appemu         \
       thirdparty/prime/apps/prime_service_appemu/sam4c16c_sam4c_ek \
       thirdparty/prime/mac/mac_1_3/addons/bootloader     \
       thirdparty/prime/mac/mac_1_3/include               \
       thirdparty/prime/mac/mac_1_3/module_config         \
       thirdparty/prime/oss                               \
       thirdparty/prime/pal/atpl230_primemac13/include    \
       thirdparty/prime/pal/atpl230_primemac13/module_config \
       thirdparty/prime/phy/atpl230/coupling_config       \
       thirdparty/prime/phy/atpl230/include               \
       thirdparty/prime/sscs/sscs-4-32/addons/serial_if   \
       thirdparty/prime/sscs/sscs-4-32/include \
       thirdparty/prime/apps/prime_service_appemu/sam4c16c_sam4c_ek/gcc

# Additional search paths for libraries.
LIB_PATH =  \
       thirdparty/CMSIS/Lib/GCC                           \
       thirdparty/prime/mac/mac_1_3/libs/gcc              \
       thirdparty/prime/sscs/sscs-4-32/libs/gcc          

# List of libraries to use during linking.
LIBS =  \
       arm_cortexM4l_math                                 \
       sam4c-mac13-sn-gcc                                 \
       sam4c-sscs432-sn-gcc                               \
       m                                                 

# Path relative to top level directory pointing to a linker script.
LINKER_SCRIPT_FLASH = thirdparty/prime/apps/prime_service_appemu/linker_scripts/gcc/sam4cp16b_0_flash.ld
LINKER_SCRIPT_SRAM  = sam/utils/linker_scripts/sam4c/gcc/sam4c16c_0_sram.ld

# Path relative to top level directory pointing to a linker script.
DEBUG_SCRIPT_FLASH = sam/boards/sam4c_ek/debug_scripts/gcc/sam4c_ek_flash_0.gdb
DEBUG_SCRIPT_SRAM  = sam/boards/sam4c_ek/debug_scripts/gcc/sam4c_ek_sram_0.gdb

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
       -D BOARD=SAM4C_EK                                  \
       -D __FREERTOS__                                    \
       -D __SAM4C16C_0__                                  \
       -D printf=iprintf                                  \
       -D scanf=iscanf

# Extra flags to use when linking
LDFLAGS = \

# Pre- and post-build commands
PREBUILD_CMD = 
POSTBUILD_CMD = 