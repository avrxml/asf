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
TARGET_FLASH = nobcn_ffd_ncp_flash.elf
TARGET_SRAM = nobcn_ffd_ncp_sram.elf

# List of C source files.
CSRCS = \
       common/services/clock/sam4l/dfll.c                 \
       common/services/clock/sam4l/osc.c                  \
       common/services/clock/sam4l/pll.c                  \
       common/services/clock/sam4l/sysclk.c               \
       common/services/delay/sam/cycle_counter.c          \
       common/services/serial/usart_serial.c              \
       common/services/spi/sam_spi/spi_master.c           \
       common/utils/interrupt/interrupt_sam_nvic.c        \
       common/utils/stdio/read.c                          \
       common/utils/stdio/write.c                         \
       sam/boards/sam4l_xplained_pro/init.c               \
       sam/drivers/bpm/bpm.c                              \
       sam/drivers/flashcalw/flashcalw.c                  \
       sam/drivers/gpio/gpio.c                            \
       sam/drivers/spi/spi.c                              \
       sam/drivers/tc/tc.c                                \
       sam/drivers/usart/usart.c                          \
       sam/drivers/wdt/wdt_sam4l.c                        \
       sam/utils/cmsis/sam4l/source/templates/exceptions.c \
       sam/utils/cmsis/sam4l/source/templates/gcc/startup_sam4l.c \
       sam/utils/cmsis/sam4l/source/templates/system_sam4l.c \
       sam/utils/syscalls/gcc/syscalls.c                  \
       thirdparty/wireless/addons/sio2host/uart/sio2host.c \
       thirdparty/wireless/avr2025_mac/addons/serial_if/src/serial_interface.c \
       thirdparty/wireless/avr2025_mac/apps/mac/serial_if/main.c \
       thirdparty/wireless/avr2025_mac/source/mac/src/mac.c \
       thirdparty/wireless/avr2025_mac/source/mac/src/mac_api.c \
       thirdparty/wireless/avr2025_mac/source/mac/src/mac_associate.c \
       thirdparty/wireless/avr2025_mac/source/mac/src/mac_beacon.c \
       thirdparty/wireless/avr2025_mac/source/mac/src/mac_callback_wrapper.c \
       thirdparty/wireless/avr2025_mac/source/mac/src/mac_data_extract_mhr.c \
       thirdparty/wireless/avr2025_mac/source/mac/src/mac_data_ind.c \
       thirdparty/wireless/avr2025_mac/source/mac/src/mac_data_req.c \
       thirdparty/wireless/avr2025_mac/source/mac/src/mac_disassociate.c \
       thirdparty/wireless/avr2025_mac/source/mac/src/mac_dispatcher.c \
       thirdparty/wireless/avr2025_mac/source/mac/src/mac_gts.c \
       thirdparty/wireless/avr2025_mac/source/mac/src/mac_mcps_data.c \
       thirdparty/wireless/avr2025_mac/source/mac/src/mac_misc.c \
       thirdparty/wireless/avr2025_mac/source/mac/src/mac_orphan.c \
       thirdparty/wireless/avr2025_mac/source/mac/src/mac_pib.c \
       thirdparty/wireless/avr2025_mac/source/mac/src/mac_poll.c \
       thirdparty/wireless/avr2025_mac/source/mac/src/mac_process_beacon_frame.c \
       thirdparty/wireless/avr2025_mac/source/mac/src/mac_process_tal_tx_frame_status.c \
       thirdparty/wireless/avr2025_mac/source/mac/src/mac_rx_enable.c \
       thirdparty/wireless/avr2025_mac/source/mac/src/mac_scan.c \
       thirdparty/wireless/avr2025_mac/source/mac/src/mac_security.c \
       thirdparty/wireless/avr2025_mac/source/mac/src/mac_start.c \
       thirdparty/wireless/avr2025_mac/source/mac/src/mac_sync.c \
       thirdparty/wireless/avr2025_mac/source/mac/src/mac_tx_coord_realignment_command.c \
       thirdparty/wireless/avr2025_mac/source/pal/common_sw_timer/common_sw_timer.c \
       thirdparty/wireless/avr2025_mac/source/pal/pal.c   \
       thirdparty/wireless/avr2025_mac/source/resources/buffer/src/bmm.c \
       thirdparty/wireless/avr2025_mac/source/resources/queue/src/qmm.c \
       thirdparty/wireless/avr2025_mac/source/stb/src/stb.c \
       thirdparty/wireless/avr2025_mac/source/stb/src/stb_armcrypto.c \
       thirdparty/wireless/avr2025_mac/source/stb/src/stb_help.c \
       thirdparty/wireless/avr2025_mac/source/tal/at86rf233/src/tal.c \
       thirdparty/wireless/avr2025_mac/source/tal/at86rf233/src/tal_ed.c \
       thirdparty/wireless/avr2025_mac/source/tal/at86rf233/src/tal_init.c \
       thirdparty/wireless/avr2025_mac/source/tal/at86rf233/src/tal_irq_handler.c \
       thirdparty/wireless/avr2025_mac/source/tal/at86rf233/src/tal_pib.c \
       thirdparty/wireless/avr2025_mac/source/tal/at86rf233/src/tal_pwr_mgmt.c \
       thirdparty/wireless/avr2025_mac/source/tal/at86rf233/src/tal_rx.c \
       thirdparty/wireless/avr2025_mac/source/tal/at86rf233/src/tal_rx_enable.c \
       thirdparty/wireless/avr2025_mac/source/tal/at86rf233/src/tal_slotted_csma.c \
       thirdparty/wireless/avr2025_mac/source/tal/at86rf233/src/tal_tx.c \
       thirdparty/wireless/avr2025_mac/source/tal/src/tal_helper.c \
       thirdparty/wireless/services/common_hw_timer/sam/hw_timer.c \
       thirdparty/wireless/services/sal/at86rf2xx/src/sal.c \
       thirdparty/wireless/services/trx_access/trx_access.c

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
       common/services/spi                                \
       common/services/spi/sam_spi                        \
       common/utils                                       \
       common/utils/stdio/stdio_serial                    \
       sam/boards                                         \
       sam/boards/sam4l_xplained_pro                      \
       sam/drivers/bpm                                    \
       sam/drivers/flashcalw                              \
       sam/drivers/gpio                                   \
       sam/drivers/spi                                    \
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
       thirdparty/wireless/avr2025_mac/addons/serial_if/inc \
       thirdparty/wireless/avr2025_mac/apps/mac/serial_if \
       thirdparty/wireless/avr2025_mac/apps/mac/serial_if/no_bcn_ffd/ncp \
       thirdparty/wireless/avr2025_mac/apps/mac/serial_if/no_bcn_ffd/ncp/sam4lc4c_reb233_xpro \
       thirdparty/wireless/avr2025_mac/include            \
       thirdparty/wireless/avr2025_mac/source/mac/inc     \
       thirdparty/wireless/avr2025_mac/source/pal         \
       thirdparty/wireless/avr2025_mac/source/pal/common_sw_timer \
       thirdparty/wireless/avr2025_mac/source/resources/buffer/inc \
       thirdparty/wireless/avr2025_mac/source/resources/queue/inc \
       thirdparty/wireless/avr2025_mac/source/stb/inc     \
       thirdparty/wireless/avr2025_mac/source/tal/at86rf233/inc \
       thirdparty/wireless/avr2025_mac/source/tal/inc     \
       thirdparty/wireless/services/common_hw_timer       \
       thirdparty/wireless/services/common_hw_timer/sam   \
       thirdparty/wireless/services/sal/inc               \
       thirdparty/wireless/services/trx_access \
       thirdparty/wireless/avr2025_mac/apps/mac/serial_if/no_bcn_ffd/ncp/sam4lc4c_reb233_xpro/gcc

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
       -D DISABLE_TSTAMP_IRQ=1                            \
       -D ENABLE_TSTAMP                                   \
       -D FFD                                             \
       -D HIGHEST_STACK_LAYER=MAC                         \
       -D PAL_USE_SPI_TRX=1                               \
       -D SAL_TYPE=AT86RF2xx                              \
       -D STB_ON_SAL                                      \
       -D TAL_TYPE=AT86RF233                              \
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