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
ARCH = cortex-m0plus

# Target part: none, sam3n4 or sam4l4aa
PART = samd20j18

# Application target name. Given with suffix .a for library and .elf for a
# standalone application.
TARGET_FLASH = nobcn_rfd_ncp_flash.elf
TARGET_SRAM = nobcn_rfd_ncp_sram.elf

# List of C source files.
CSRCS = \
       common/utils/interrupt/interrupt_sam_nvic.c        \
       common2/services/delay/sam0/systick_counter.c      \
       sam0/boards/samd20_xplained_pro/board_init.c       \
       sam0/drivers/extint/extint_callback.c              \
       sam0/drivers/extint/extint_sam_d_r/extint.c        \
       sam0/drivers/port/port.c                           \
       sam0/drivers/sercom/sercom.c                       \
       sam0/drivers/sercom/sercom_interrupt.c             \
       sam0/drivers/sercom/spi/spi.c                      \
       sam0/drivers/sercom/usart/usart.c                  \
       sam0/drivers/sercom/usart/usart_interrupt.c        \
       sam0/drivers/system/clock/clock_samd20/clock.c     \
       sam0/drivers/system/clock/clock_samd20/gclk.c      \
       sam0/drivers/system/interrupt/system_interrupt.c   \
       sam0/drivers/system/pinmux/pinmux.c                \
       sam0/drivers/system/system.c                       \
       sam0/drivers/tc/tc_interrupt.c                     \
       sam0/drivers/tc/tc_sam_d_r/tc.c                    \
       sam0/utils/cmsis/samd20/source/gcc/startup_samd20.c \
       sam0/utils/cmsis/samd20/source/system_samd20.c     \
       sam0/utils/stdio/read.c                            \
       sam0/utils/stdio/write.c                           \
       sam0/utils/syscalls/gcc/syscalls.c                 \
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
       thirdparty/wireless/services/common_hw_timer/sam0/hw_timer.c \
       thirdparty/wireless/services/sal/at86rf2xx/src/sal.c \
       thirdparty/wireless/services/trx_access/trx_access.c

# List of assembler source files.
ASSRCS = 

# List of include paths.
INC_PATH = \
       common/boards                                      \
       common/services/serial                             \
       common/utils                                       \
       common2/services/delay                             \
       common2/services/delay/sam0                        \
       sam0/boards                                        \
       sam0/boards/samd20_xplained_pro                    \
       sam0/drivers/extint                                \
       sam0/drivers/extint/extint_sam_d_r                 \
       sam0/drivers/port                                  \
       sam0/drivers/sercom                                \
       sam0/drivers/sercom/spi                            \
       sam0/drivers/sercom/usart                          \
       sam0/drivers/system                                \
       sam0/drivers/system/clock                          \
       sam0/drivers/system/clock/clock_samd20             \
       sam0/drivers/system/interrupt                      \
       sam0/drivers/system/interrupt/system_interrupt_samd20 \
       sam0/drivers/system/pinmux                         \
       sam0/drivers/system/power                          \
       sam0/drivers/system/power/power_sam_d_r            \
       sam0/drivers/system/reset                          \
       sam0/drivers/system/reset/reset_sam_d_r            \
       sam0/drivers/tc                                    \
       sam0/drivers/tc/tc_sam_d_r                         \
       sam0/utils                                         \
       sam0/utils/cmsis/samd20/include                    \
       sam0/utils/cmsis/samd20/source                     \
       sam0/utils/header_files                            \
       sam0/utils/preprocessor                            \
       sam0/utils/stdio/stdio_serial                      \
       thirdparty/CMSIS/Include                           \
       thirdparty/CMSIS/Lib/GCC                           \
       thirdparty/wireless/addons/sio2host/uart           \
       thirdparty/wireless/avr2025_mac/addons/serial_if/inc \
       thirdparty/wireless/avr2025_mac/apps/mac/serial_if \
       thirdparty/wireless/avr2025_mac/apps/mac/serial_if/no_bcn_rfd/ncp \
       thirdparty/wireless/avr2025_mac/apps/mac/serial_if/no_bcn_rfd/ncp/samd20_xplained_pro_rf233 \
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
       thirdparty/wireless/services/common_hw_timer/sam0  \
       thirdparty/wireless/services/sal/inc               \
       thirdparty/wireless/services/trx_access \
       thirdparty/wireless/avr2025_mac/apps/mac/serial_if/no_bcn_rfd/ncp/samd20_xplained_pro_rf233/gcc

# Additional search paths for libraries.
LIB_PATH =  \
       thirdparty/CMSIS/Lib/GCC                           \
       thirdparty/wireless/services/common_hw_timer/sam0/lib

# List of libraries to use during linking.
LIBS =  \
       arm_cortexM0l_math                                 \
       sam0_lib_hw_timer                                 

# Path relative to top level directory pointing to a linker script.
LINKER_SCRIPT_FLASH = sam0/utils/linker_scripts/samd20/gcc/samd20j18_flash.ld
LINKER_SCRIPT_SRAM  = sam0/utils/linker_scripts/samd20/gcc/samd20j18_sram.ld

# Path relative to top level directory pointing to a linker script.
DEBUG_SCRIPT_FLASH = sam0/boards/samd20_xplained_pro/debug_scripts/gcc/samd20_xplained_pro_flash.gdb
DEBUG_SCRIPT_SRAM  = sam0/boards/samd20_xplained_pro/debug_scripts/gcc/samd20_xplained_pro_sram.gdb

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
       -D ANTENNA_DIVERSITY=0                             \
       -D ARM_MATH_CM0PLUS=true                           \
       -D BOARD=SAMD20_XPLAINED_PRO                       \
       -D CUSTOM_DEFAULT_TX_PWR                           \
       -D CUSTOM_PWR_TABLE                                \
       -D DISABLE_TSTAMP_IRQ=1                            \
       -D ENABLE_TSTAMP                                   \
       -D EXTINT_CALLBACK_MODE=true                       \
       -D EXT_RF_FRONT_END_CTRL                           \
       -D HIGHEST_STACK_LAYER=MAC                         \
       -D PAL_USE_SPI_TRX=1                               \
       -D SAL_TYPE=AT86RF2xx                              \
       -D SPI_CALLBACK_MODE=false                         \
       -D STB_ON_SAL                                      \
       -D SYSTICK_MODE                                    \
       -D TAL_TYPE=AT86RF233                              \
       -D TC_ASYNC=true                                   \
       -D TEST_HARNESS                                    \
       -D USART_CALLBACK_MODE=true                        \
       -D _DEBUG_=0                                       \
       -D __SAMD20J18__

# Extra flags to use when linking
LDFLAGS = \

# Pre- and post-build commands
PREBUILD_CMD = 
POSTBUILD_CMD = 