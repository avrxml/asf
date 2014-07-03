#
# Copyright (c) 2009-2010 Atmel Corporation. All rights reserved.
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

# Target CPU architecture: ap, ucr1, ucr2 or ucr3
ARCH = ucr2

# Target part: none, ap7xxx or uc3xxxxx
PART = uc3a3256s

# Target device flash memory details (used by the avr32program programming
# tool: [cfi|internal]@address
FLASH = internal@0x80000000

# Clock source to use when programming; xtal, extclk or int
PROG_CLOCK = int

# Application target name. Given with suffix .a for library and .elf for a
# standalone application.
TARGET = nobcn_ffd_ncp.elf

# List of C source files.
CSRCS = \
       avr32/boards/rz600/init.c                          \
       avr32/boards/rz600/led.c                           \
       avr32/drivers/flashc/flashc.c                      \
       avr32/drivers/gpio/gpio.c                          \
       avr32/drivers/intc/intc.c                          \
       avr32/drivers/pm/pm.c                              \
       avr32/drivers/pm/pm_conf_clocks.c                  \
       avr32/drivers/pm/power_clocks_lib.c                \
       avr32/drivers/spi/spi.c                            \
       avr32/drivers/tc/tc.c                              \
       avr32/drivers/usbb/usbb_device.c                   \
       common/services/clock/uc3a3_a4/sysclk.c            \
       common/services/sleepmgr/uc3/sleepmgr.c            \
       common/services/spi/uc3_spi/spi_master.c           \
       common/services/usb/class/cdc/device/udi_cdc.c     \
       common/services/usb/class/cdc/device/udi_cdc_desc.c \
       common/services/usb/udc/udc.c                      \
       common/utils/stdio/read.c                          \
       common/utils/stdio/stdio_usb/stdio_usb.c           \
       common/utils/stdio/write.c                         \
       thirdparty/wireless/addons/sio2host/usb/sio2host.c \
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
       thirdparty/wireless/avr2025_mac/source/tal/at86rf212/src/tal.c \
       thirdparty/wireless/avr2025_mac/source/tal/at86rf212/src/tal_ed.c \
       thirdparty/wireless/avr2025_mac/source/tal/at86rf212/src/tal_init.c \
       thirdparty/wireless/avr2025_mac/source/tal/at86rf212/src/tal_irq_handler.c \
       thirdparty/wireless/avr2025_mac/source/tal/at86rf212/src/tal_pib.c \
       thirdparty/wireless/avr2025_mac/source/tal/at86rf212/src/tal_pwr_mgmt.c \
       thirdparty/wireless/avr2025_mac/source/tal/at86rf212/src/tal_rx.c \
       thirdparty/wireless/avr2025_mac/source/tal/at86rf212/src/tal_rx_enable.c \
       thirdparty/wireless/avr2025_mac/source/tal/at86rf212/src/tal_slotted_csma.c \
       thirdparty/wireless/avr2025_mac/source/tal/at86rf212/src/tal_tx.c \
       thirdparty/wireless/avr2025_mac/source/tal/src/tal_helper.c \
       thirdparty/wireless/services/common_hw_timer/uc3/hw_timer.c \
       thirdparty/wireless/services/sal/at86rf2xx/src/sal.c \
       thirdparty/wireless/services/trx_access/trx_access.c

# List of assembler source files.
ASSRCS = \
       avr32/drivers/intc/exception.S                     \
       avr32/utils/startup/startup_uc3.S                  \
       avr32/utils/startup/trampoline_uc3.S

# List of include paths.
INC_PATH = \
       avr32/boards                                       \
       avr32/boards/rz600                                 \
       avr32/drivers/cpu/cycle_counter                    \
       avr32/drivers/flashc                               \
       avr32/drivers/gpio                                 \
       avr32/drivers/intc                                 \
       avr32/drivers/pm                                   \
       avr32/drivers/spi                                  \
       avr32/drivers/tc                                   \
       avr32/drivers/usbb                                 \
       avr32/utils                                        \
       avr32/utils/preprocessor                           \
       common/boards                                      \
       common/services/clock                              \
       common/services/delay                              \
       common/services/ioport                             \
       common/services/sleepmgr                           \
       common/services/spi                                \
       common/services/spi/uc3_spi                        \
       common/services/usb                                \
       common/services/usb/class/cdc                      \
       common/services/usb/class/cdc/device               \
       common/services/usb/udc                            \
       common/utils                                       \
       common/utils/stdio/stdio_usb                       \
       thirdparty/wireless/addons/sio2host/usb            \
       thirdparty/wireless/avr2025_mac/addons/serial_if/inc \
       thirdparty/wireless/avr2025_mac/apps/mac/serial_if \
       thirdparty/wireless/avr2025_mac/apps/mac/serial_if/no_bcn_ffd/ncp \
       thirdparty/wireless/avr2025_mac/apps/mac/serial_if/no_bcn_ffd/ncp/at32uc3a3256s_rz600_at86rf212 \
       thirdparty/wireless/avr2025_mac/include            \
       thirdparty/wireless/avr2025_mac/source/mac/inc     \
       thirdparty/wireless/avr2025_mac/source/pal         \
       thirdparty/wireless/avr2025_mac/source/pal/common_sw_timer \
       thirdparty/wireless/avr2025_mac/source/resources/buffer/inc \
       thirdparty/wireless/avr2025_mac/source/resources/queue/inc \
       thirdparty/wireless/avr2025_mac/source/stb/inc     \
       thirdparty/wireless/avr2025_mac/source/tal/at86rf212/inc \
       thirdparty/wireless/avr2025_mac/source/tal/inc     \
       thirdparty/wireless/services/common_hw_timer       \
       thirdparty/wireless/services/common_hw_timer/uc3   \
       thirdparty/wireless/services/sal/inc               \
       thirdparty/wireless/services/trx_access \
       thirdparty/wireless/avr2025_mac/apps/mac/serial_if/no_bcn_ffd/ncp/at32uc3a3256s_rz600_at86rf212/gcc

# Additional search paths for libraries.
LIB_PATH =  \
       thirdparty/wireless/services/common_hw_timer/uc3/lib

# List of libraries to use during linking.
LIBS =  \
       uc3_lib_hw_timer                                  

# Path relative to top level directory pointing to a linker script.
LINKER_SCRIPT = avr32/utils/linker_scripts/at32uc3a3/256s/gcc/link_uc3a3256s.lds

# Additional options for debugging. By default the common Makefile.in will
# add -g3.
DBGFLAGS = 

# Application optimization used during compilation and linking:
# -O0, -O1, -O2, -O3 or -Os
OPTIMIZATION = -Os

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
       -D BOARD=RZ600                                     \
       -D DISABLE_TSTAMP_IRQ=1                            \
       -D ENABLE_TSTAMP                                   \
       -D FFD                                             \
       -D HIGHEST_STACK_LAYER=MAC                         \
       -D PAL_USE_SPI_TRX=1                               \
       -D SAL_TYPE=AT86RF2xx                              \
       -D STB_ON_SAL                                      \
       -D TAL_TYPE=AT86RF212                              \
       -D TEST_HARNESS                                    \
       -D TEST_HARNESS_BIG_ENDIAN                         \
       -D UDD_ENABLE                                      \
       -D _DEBUG_=0

# Extra flags to use when linking
LDFLAGS = \
       -nostartfiles -Wl,-e,_trampoline

# Pre- and post-build commands
PREBUILD_CMD = 
POSTBUILD_CMD = 