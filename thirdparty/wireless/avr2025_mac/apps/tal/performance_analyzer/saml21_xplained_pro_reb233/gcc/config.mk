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
PRJ_PATH = ../../../../../../../..

# Target CPU architecture: cortex-m3, cortex-m4
ARCH = cortex-m0plus

# Target part: none, sam3n4 or sam4l4aa
PART = saml21j18a

# Application target name. Given with suffix .a for library and .elf for a
# standalone application.
TARGET_FLASH = tal_performance_analyzer_flash.elf
TARGET_SRAM = tal_performance_analyzer_sram.elf

# List of C source files.
CSRCS = \
       common/utils/interrupt/interrupt_sam_nvic.c        \
       common2/services/delay/sam0/systick_counter.c      \
       sam0/boards/saml21_xplained_pro/board_init.c       \
       sam0/drivers/extint/extint_callback.c              \
       sam0/drivers/extint/extint_sam_l_c/extint.c        \
       sam0/drivers/port/port.c                           \
       sam0/drivers/sercom/sercom.c                       \
       sam0/drivers/sercom/sercom_interrupt.c             \
       sam0/drivers/sercom/spi/spi.c                      \
       sam0/drivers/sercom/usart/usart.c                  \
       sam0/drivers/sercom/usart/usart_interrupt.c        \
       sam0/drivers/system/clock/clock_saml21/clock.c     \
       sam0/drivers/system/clock/clock_saml21/gclk.c      \
       sam0/drivers/system/interrupt/system_interrupt.c   \
       sam0/drivers/system/pinmux/pinmux.c                \
       sam0/drivers/system/system.c                       \
       sam0/drivers/tc/tc_interrupt.c                     \
       sam0/drivers/tc/tc_sam_l_c/tc.c                    \
       sam0/utils/cmsis/saml21/source/gcc/startup_saml21.c \
       sam0/utils/cmsis/saml21/source/system_saml21.c     \
       sam0/utils/stdio/read.c                            \
       sam0/utils/stdio/write.c                           \
       sam0/utils/syscalls/gcc/syscalls.c                 \
       thirdparty/wireless/addons/sio2host/uart/sio2host.c \
       thirdparty/wireless/avr2025_mac/apps/tal/performance_analyzer/main.c \
       thirdparty/wireless/avr2025_mac/apps/tal/performance_analyzer/src/init_state.c \
       thirdparty/wireless/avr2025_mac/apps/tal/performance_analyzer/src/peer_search_initiator.c \
       thirdparty/wireless/avr2025_mac/apps/tal/performance_analyzer/src/peer_search_receptor.c \
       thirdparty/wireless/avr2025_mac/apps/tal/performance_analyzer/src/per_mode_common_utils.c \
       thirdparty/wireless/avr2025_mac/apps/tal/performance_analyzer/src/per_mode_initiator.c \
       thirdparty/wireless/avr2025_mac/apps/tal/performance_analyzer/src/per_mode_receptor.c \
       thirdparty/wireless/avr2025_mac/apps/tal/performance_analyzer/src/perf_api_serial_handler.c \
       thirdparty/wireless/avr2025_mac/apps/tal/performance_analyzer/src/performance_main.c \
       thirdparty/wireless/avr2025_mac/apps/tal/performance_analyzer/src/range_measure.c \
       thirdparty/wireless/avr2025_mac/apps/tal/performance_analyzer/src/user_interface.c \
       thirdparty/wireless/avr2025_mac/apps/tal/performance_analyzer/src/wait_for_event.c \
       thirdparty/wireless/avr2025_mac/source/pal/common_sw_timer/common_sw_timer.c \
       thirdparty/wireless/avr2025_mac/source/pal/pal.c   \
       thirdparty/wireless/avr2025_mac/source/resources/buffer/src/bmm.c \
       thirdparty/wireless/avr2025_mac/source/resources/queue/src/qmm.c \
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
       thirdparty/wireless/avr2025_mac/source/tfa/at86rf233/src/tfa.c \
       thirdparty/wireless/services/common_hw_timer/sam0/hw_timer.c \
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
       sam0/boards/saml21_xplained_pro                    \
       sam0/drivers/extint                                \
       sam0/drivers/extint/extint_sam_l_c                 \
       sam0/drivers/port                                  \
       sam0/drivers/sercom                                \
       sam0/drivers/sercom/spi                            \
       sam0/drivers/sercom/usart                          \
       sam0/drivers/system                                \
       sam0/drivers/system/clock                          \
       sam0/drivers/system/clock/clock_saml21             \
       sam0/drivers/system/interrupt                      \
       sam0/drivers/system/interrupt/system_interrupt_saml21 \
       sam0/drivers/system/pinmux                         \
       sam0/drivers/system/power                          \
       sam0/drivers/system/power/power_sam_l              \
       sam0/drivers/system/reset                          \
       sam0/drivers/system/reset/reset_sam_l              \
       sam0/drivers/tc                                    \
       sam0/drivers/tc/tc_sam_l_c                         \
       sam0/utils                                         \
       sam0/utils/cmsis/saml21/include                    \
       sam0/utils/cmsis/saml21/source                     \
       sam0/utils/header_files                            \
       sam0/utils/preprocessor                            \
       sam0/utils/stdio/stdio_serial                      \
       thirdparty/CMSIS/Include                           \
       thirdparty/CMSIS/Lib/GCC                           \
       thirdparty/wireless/addons/sio2host/uart           \
       thirdparty/wireless/avr2025_mac/apps/tal/performance_analyzer \
       thirdparty/wireless/avr2025_mac/apps/tal/performance_analyzer/inc \
       thirdparty/wireless/avr2025_mac/apps/tal/performance_analyzer/saml21_xplained_pro_reb233 \
       thirdparty/wireless/avr2025_mac/include            \
       thirdparty/wireless/avr2025_mac/source/mac/inc     \
       thirdparty/wireless/avr2025_mac/source/pal         \
       thirdparty/wireless/avr2025_mac/source/pal/common_sw_timer \
       thirdparty/wireless/avr2025_mac/source/resources/buffer/inc \
       thirdparty/wireless/avr2025_mac/source/resources/queue/inc \
       thirdparty/wireless/avr2025_mac/source/tal/at86rf233/inc \
       thirdparty/wireless/avr2025_mac/source/tal/inc     \
       thirdparty/wireless/avr2025_mac/source/tfa/inc     \
       thirdparty/wireless/services/common_hw_timer       \
       thirdparty/wireless/services/common_hw_timer/sam0  \
       thirdparty/wireless/services/trx_access \
       thirdparty/wireless/avr2025_mac/apps/tal/performance_analyzer/saml21_xplained_pro_reb233/gcc

# Additional search paths for libraries.
LIB_PATH =  \
       thirdparty/CMSIS/Lib/GCC                           \
       thirdparty/wireless/services/common_hw_timer/sam0/lib

# List of libraries to use during linking.
LIBS =  \
       arm_cortexM0l_math                                 \
       sam0_lib_hw_timer                                 

# Path relative to top level directory pointing to a linker script.
LINKER_SCRIPT_FLASH = sam0/utils/linker_scripts/saml21/gcc/saml21j18a_flash.ld
LINKER_SCRIPT_SRAM  = sam0/utils/linker_scripts/saml21/gcc/saml21j18a_sram.ld

# Path relative to top level directory pointing to a linker script.
DEBUG_SCRIPT_FLASH = sam0/boards/saml21_xplained_pro/debug_scripts/gcc/saml21_xplained_pro_flash.gdb
DEBUG_SCRIPT_SRAM  = sam0/boards/saml21_xplained_pro/debug_scripts/gcc/saml21_xplained_pro_sram.gdb

# Project type parameter: all, sram or flash
PROJECT_TYPE        = flash

# Additional options for debugging. By default the common Makefile.in will
# add -g3.
DBGFLAGS = 

# Application optimization used during compilation and linking:
# -O0, -O1, -O2, -O3 or -Os
OPTIMIZATION = -O3

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
       -D BOARD=SAML21_XPLAINED_PRO                       \
       -D DISABLE_TSTAMP_IRQ=1                            \
       -D ENABLE_DEEP_SLEEP                               \
       -D ENABLE_TFA                                      \
       -D EXTINT_CALLBACK_MODE=true                       \
       -D FFD                                             \
       -D HIGHEST_STACK_LAYER=TAL                         \
       -D HIGH_DATA_RATE_SUPPORT                          \
       -D PAL_USE_SPI_TRX=1                               \
       -D SPI_CALLBACK_MODE=false                         \
       -D SYSTICK_MODE                                    \
       -D TAL_TYPE=AT86RF233                              \
       -D TC_ASYNC=true                                   \
       -D TRX_REG_RAW_VALUE                               \
       -D USART_CALLBACK_MODE=true                        \
       -D _DEBUG_=0                                       \
       -D __SAML21J18A__

# Extra flags to use when linking
LDFLAGS = \

# Pre- and post-build commands
PREBUILD_CMD = 
POSTBUILD_CMD = 