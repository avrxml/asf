#
# Copyright (c) 2010 Atmel Corporation. All rights reserved.
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

# Microcontroller: atxmega128a1, atmega128, attiny261, etc.
MCU = atxmega256a3bu

# Application target name. Given with suffix .a for library and .elf for a
# standalone application.
TARGET = mac_no_beacon_sleep.elf

# C source files located from the top-level source directory
CSRCS = \
       common/services/clock/xmega/sysclk.c               \
       common/services/ioport/xmega/ioport_compat.c       \
       common/services/sleepmgr/xmega/sleepmgr.c          \
       common/services/spi/xmega_spi/spi_master.c         \
       thirdparty/wireless/avr2025_mac/apps/mac/no_beacon_sleep/main.c \
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
       thirdparty/wireless/avr2025_mac/source/tal/at86rf231/src/tal.c \
       thirdparty/wireless/avr2025_mac/source/tal/at86rf231/src/tal_ed.c \
       thirdparty/wireless/avr2025_mac/source/tal/at86rf231/src/tal_init.c \
       thirdparty/wireless/avr2025_mac/source/tal/at86rf231/src/tal_irq_handler.c \
       thirdparty/wireless/avr2025_mac/source/tal/at86rf231/src/tal_pib.c \
       thirdparty/wireless/avr2025_mac/source/tal/at86rf231/src/tal_pwr_mgmt.c \
       thirdparty/wireless/avr2025_mac/source/tal/at86rf231/src/tal_rx.c \
       thirdparty/wireless/avr2025_mac/source/tal/at86rf231/src/tal_rx_enable.c \
       thirdparty/wireless/avr2025_mac/source/tal/at86rf231/src/tal_slotted_csma.c \
       thirdparty/wireless/avr2025_mac/source/tal/at86rf231/src/tal_tx.c \
       thirdparty/wireless/avr2025_mac/source/tal/src/tal_helper.c \
       thirdparty/wireless/services/common_hw_timer/xmega/hw_timer.c \
       thirdparty/wireless/services/sal/atxmega_sal/src/sal.c \
       thirdparty/wireless/services/trx_access/trx_access.c \
       xmega/boards/xmega_a3bu_xplained/init.c            \
       xmega/drivers/nvm/nvm.c                            \
       xmega/drivers/spi/spi.c                            \
       xmega/drivers/tc/tc.c

# Assembler source files located from the top-level source directory
ASSRCS = \
       xmega/drivers/cpu/ccp.s                            \
       xmega/drivers/nvm/nvm_asm.s

# Include path located from the top-level source directory
INC_PATH = \
       common/boards                                      \
       common/services/clock                              \
       common/services/delay                              \
       common/services/gpio                               \
       common/services/ioport                             \
       common/services/sleepmgr                           \
       common/services/spi                                \
       common/services/spi/xmega_spi                      \
       common/utils                                       \
       thirdparty/wireless/avr2025_mac/apps/mac/no_beacon_sleep \
       thirdparty/wireless/avr2025_mac/apps/mac/no_beacon_sleep/xmega_a3bu_xplained_rz600rf231 \
       thirdparty/wireless/avr2025_mac/include            \
       thirdparty/wireless/avr2025_mac/source/mac/inc     \
       thirdparty/wireless/avr2025_mac/source/pal         \
       thirdparty/wireless/avr2025_mac/source/pal/common_sw_timer \
       thirdparty/wireless/avr2025_mac/source/resources/buffer/inc \
       thirdparty/wireless/avr2025_mac/source/resources/queue/inc \
       thirdparty/wireless/avr2025_mac/source/stb/inc     \
       thirdparty/wireless/avr2025_mac/source/tal/at86rf231/inc \
       thirdparty/wireless/avr2025_mac/source/tal/inc     \
       thirdparty/wireless/services/common_hw_timer       \
       thirdparty/wireless/services/common_hw_timer/xmega \
       thirdparty/wireless/services/sal/inc               \
       thirdparty/wireless/services/trx_access            \
       xmega/boards                                       \
       xmega/boards/xmega_a3bu_xplained                   \
       xmega/drivers/cpu                                  \
       xmega/drivers/nvm                                  \
       xmega/drivers/pmic                                 \
       xmega/drivers/sleep                                \
       xmega/drivers/spi                                  \
       xmega/drivers/tc                                   \
       xmega/utils                                        \
       xmega/utils/preprocessor \
       thirdparty/wireless/avr2025_mac/apps/mac/no_beacon_sleep/xmega_a3bu_xplained_rz600rf231/gcc

# Library paths from the top-level source directory
LIB_PATH =  \
       thirdparty/wireless/services/common_hw_timer/xmega/lib

# Libraries to link with the project
LIBS =  \
       xmegaa3_hw_timer_lib                              

# Additional options for debugging. By default the common Makefile.in will
# add -gdwarf-2.
DBGFLAGS = 

# Optimization settings
OPTIMIZATION = -Os

# Extra flags used when creating an EEPROM Intel HEX file. By default the
# common Makefile.in will add -j .eeprom
# --set-section-flags=.eeprom="alloc,load" --change-section-lma .eeprom=0.
EEPROMFLAGS = 

# Extra flags used when creating an Intel HEX file. By default the common
# Makefile.in will add -R .eeprom -R .usb_descriptor_table.
FLASHFLAGS = 

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
       -D BOARD=XMEGA_A3BU_XPLAINED                       \
       -D CONFIG_NVM_IGNORE_XMEGA_A3_D3_REVB_ERRATA       \
       -D DISABLE_TSTAMP_IRQ=1                            \
       -D ENABLE_TSTAMP                                   \
       -D FFD                                             \
       -D HIGHEST_STACK_LAYER=MAC                         \
       -D IOPORT_XMEGA_COMPAT                             \
       -D PAL_USE_SPI_TRX=1                               \
       -D SAL_TYPE=ATXMEGA_SAL                            \
       -D STB_ON_SAL                                      \
       -D TAL_TYPE=AT86RF231                              \
       -D _DEBUG_=0

# Extra flags to use when linking
LDFLAGS =  \
       -Wl,--section-start=.BOOT=0x40000                 

# Pre- and post-build commands
PREBUILD_CMD = 
POSTBUILD_CMD = 