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
PRJ_PATH = ../../../../../../..

# Microcontroller: atxmega128a1, atmega128, attiny261, etc.
MCU = atxmega256a3bu

# Application target name. Given with suffix .a for library and .elf for a
# standalone application.
TARGET = apps_peer2peer.elf

# C source files located from the top-level source directory
CSRCS = \
       common/services/clock/xmega/sysclk.c               \
       common/services/ioport/xmega/ioport_compat.c       \
       common/services/sleepmgr/xmega/sleepmgr.c          \
       common/services/spi/xmega_spi/spi_master.c         \
       common/services/usb/class/cdc/device/udi_cdc.c     \
       common/services/usb/class/cdc/device/udi_cdc_desc.c \
       common/services/usb/udc/udc.c                      \
       common/utils/stdio/read.c                          \
       common/utils/stdio/stdio_usb/stdio_usb.c           \
       common/utils/stdio/write.c                         \
       thirdparty/wireless/addons/sio2host/usb/sio2host.c \
       thirdparty/wireless/avr2130_lwmesh/apps/PeertoPeer/Peer2Peer.c \
       thirdparty/wireless/avr2130_lwmesh/source/nwk/src/nwk.c \
       thirdparty/wireless/avr2130_lwmesh/source/nwk/src/nwkDataReq.c \
       thirdparty/wireless/avr2130_lwmesh/source/nwk/src/nwkFrame.c \
       thirdparty/wireless/avr2130_lwmesh/source/nwk/src/nwkGroup.c \
       thirdparty/wireless/avr2130_lwmesh/source/nwk/src/nwkRoute.c \
       thirdparty/wireless/avr2130_lwmesh/source/nwk/src/nwkRouteDiscovery.c \
       thirdparty/wireless/avr2130_lwmesh/source/nwk/src/nwkRx.c \
       thirdparty/wireless/avr2130_lwmesh/source/nwk/src/nwkSecurity.c \
       thirdparty/wireless/avr2130_lwmesh/source/nwk/src/nwkTx.c \
       thirdparty/wireless/avr2130_lwmesh/source/phy/at86rf231/src/phy.c \
       thirdparty/wireless/avr2130_lwmesh/source/sys/src/sys.c \
       thirdparty/wireless/avr2130_lwmesh/source/sys/src/sysEncrypt.c \
       thirdparty/wireless/avr2130_lwmesh/source/sys/src/sysTimer.c \
       thirdparty/wireless/services/common_hw_timer/xmega/hw_timer.c \
       thirdparty/wireless/services/sal/atxmega_sal/src/sal.c \
       thirdparty/wireless/services/trx_access/trx_access.c \
       xmega/boards/xmega_a3bu_xplained/init.c            \
       xmega/drivers/nvm/nvm.c                            \
       xmega/drivers/spi/spi.c                            \
       xmega/drivers/tc/tc.c                              \
       xmega/drivers/usb/usb_device.c

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
       common/services/usb                                \
       common/services/usb/class/cdc                      \
       common/services/usb/class/cdc/device               \
       common/services/usb/udc                            \
       common/utils                                       \
       common/utils/stdio/stdio_usb                       \
       thirdparty/wireless/addons/sio2host/usb            \
       thirdparty/wireless/avr2130_lwmesh/apps/PeertoPeer \
       thirdparty/wireless/avr2130_lwmesh/apps/PeertoPeer/xmega_a3bu_xplained_rz600rf231 \
       thirdparty/wireless/avr2130_lwmesh/source/nwk/inc  \
       thirdparty/wireless/avr2130_lwmesh/source/phy/at86rf231/inc \
       thirdparty/wireless/avr2130_lwmesh/source/sys/inc  \
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
       xmega/drivers/usb                                  \
       xmega/utils                                        \
       xmega/utils/preprocessor \
       thirdparty/wireless/avr2130_lwmesh/apps/PeertoPeer/xmega_a3bu_xplained_rz600rf231/gcc

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
       -D IOPORT_XMEGA_COMPAT                             \
       -D PHY_AT86RF231                                   \
       -D SAL_TYPE=ATXMEGA_SAL

# Extra flags to use when linking
LDFLAGS =  \
       -Wl,--section-start=.BOOT=0x40000                 

# Pre- and post-build commands
PREBUILD_CMD = 
POSTBUILD_CMD = 