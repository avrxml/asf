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
PRJ_PATH = ../../../../../..

# Target CPU architecture: ap, ucr1, ucr2 or ucr3
ARCH = ucr2

# Target part: none, ap7xxx or uc3xxxxx
PART = uc3a0512

# Target device flash memory details (used by the avr32program programming
# tool: [cfi|internal]@address
FLASH = internal@0x80000000

# Clock source to use when programming; xtal, extclk or int
PROG_CLOCK = int

# Application target name. Given with suffix .a for library and .elf for a
# standalone application.
TARGET = wifi_example.elf

# List of C source files.
CSRCS = \
       avr32/boards/evk1105/led.c                         \
       avr32/components/display/et024006dhu/et024006dhu.c \
       avr32/drivers/adc/adc.c                            \
       avr32/drivers/ebi/smc/smc.c                        \
       avr32/drivers/flashc/flashc.c                      \
       avr32/drivers/gpio/gpio.c                          \
       avr32/drivers/intc/intc.c                          \
       avr32/drivers/pm/pm.c                              \
       avr32/drivers/pm/pm_conf_clocks.c                  \
       avr32/drivers/pm/power_clocks_lib.c                \
       avr32/drivers/rtc/rtc.c                            \
       avr32/drivers/spi/spi.c                            \
       avr32/drivers/usart/usart.c                        \
       common/components/memory/data_flash/at45dbx/_asf_v1/at45dbx.c \
       common/components/memory/data_flash/at45dbx/_asf_v1/at45dbx_mem.c \
       common/services/clock/uc3a0_a1/sysclk.c            \
       common/services/storage/ctrl_access/ctrl_access.c  \
       thirdparty/hd/example/http_server/fsdata.c         \
       thirdparty/hd/example/http_server/http_server_gui.c \
       thirdparty/hd/example/http_server/main.c           \
       thirdparty/hd/example/net/fs.c                     \
       thirdparty/hd/example/net/httpd.c                  \
       thirdparty/hd/example/net/lwip_setup.c             \
       thirdparty/hd/example/net/ping.c                   \
       thirdparty/hd/example/net/ttcp.c                   \
       thirdparty/hd/example/ports/avr32/avr32_spi.c      \
       thirdparty/hd/example/ports/avr32/board_init.c     \
       thirdparty/hd/example/ports/avr32/gui.c            \
       thirdparty/hd/example/ports/avr32/gui_getstring.c  \
       thirdparty/hd/example/ports/avr32/timer.c          \
       thirdparty/hd/example/util/console.c               \
       thirdparty/hd/example/util/getopt.c                \
       thirdparty/hd/example/util/printf-stdarg.c         \
       thirdparty/hd/example/util/util.c                  \
       thirdparty/hd/example/wl/cmd_wl.c                  \
       thirdparty/hd/example/wl/fw_download.c             \
       thirdparty/hd/example/wl/wl_cm.c                   \
       thirdparty/lwip/lwip-1.3.2/src/api/api_lib.c       \
       thirdparty/lwip/lwip-1.3.2/src/api/api_msg.c       \
       thirdparty/lwip/lwip-1.3.2/src/api/err.c           \
       thirdparty/lwip/lwip-1.3.2/src/api/netbuf.c        \
       thirdparty/lwip/lwip-1.3.2/src/api/netdb.c         \
       thirdparty/lwip/lwip-1.3.2/src/api/netifapi.c      \
       thirdparty/lwip/lwip-1.3.2/src/api/sockets.c       \
       thirdparty/lwip/lwip-1.3.2/src/api/tcpip.c         \
       thirdparty/lwip/lwip-1.3.2/src/core/dhcp.c         \
       thirdparty/lwip/lwip-1.3.2/src/core/dns.c          \
       thirdparty/lwip/lwip-1.3.2/src/core/init.c         \
       thirdparty/lwip/lwip-1.3.2/src/core/ipv4/autoip.c  \
       thirdparty/lwip/lwip-1.3.2/src/core/ipv4/icmp.c    \
       thirdparty/lwip/lwip-1.3.2/src/core/ipv4/igmp.c    \
       thirdparty/lwip/lwip-1.3.2/src/core/ipv4/inet.c    \
       thirdparty/lwip/lwip-1.3.2/src/core/ipv4/inet_chksum.c \
       thirdparty/lwip/lwip-1.3.2/src/core/ipv4/ip.c      \
       thirdparty/lwip/lwip-1.3.2/src/core/ipv4/ip_addr.c \
       thirdparty/lwip/lwip-1.3.2/src/core/ipv4/ip_frag.c \
       thirdparty/lwip/lwip-1.3.2/src/core/mem.c          \
       thirdparty/lwip/lwip-1.3.2/src/core/memp.c         \
       thirdparty/lwip/lwip-1.3.2/src/core/netif.c        \
       thirdparty/lwip/lwip-1.3.2/src/core/pbuf.c         \
       thirdparty/lwip/lwip-1.3.2/src/core/raw.c          \
       thirdparty/lwip/lwip-1.3.2/src/core/stats.c        \
       thirdparty/lwip/lwip-1.3.2/src/core/sys.c          \
       thirdparty/lwip/lwip-1.3.2/src/core/tcp.c          \
       thirdparty/lwip/lwip-1.3.2/src/core/tcp_in.c       \
       thirdparty/lwip/lwip-1.3.2/src/core/tcp_out.c      \
       thirdparty/lwip/lwip-1.3.2/src/core/udp.c          \
       thirdparty/lwip/lwip-1.3.2/src/netif/etharp.c      \
       thirdparty/lwip/lwip-port-1.3.2/hd/if/netif/wlif.c

# List of assembler source files.
ASSRCS = \
       avr32/drivers/intc/exception.S                     \
       avr32/utils/startup/startup_uc3.S                  \
       avr32/utils/startup/trampoline_uc3.S

# List of include paths.
INC_PATH = \
       avr32/boards                                       \
       avr32/boards/evk1105                               \
       avr32/components/display/et024006dhu               \
       avr32/components/touch/qt1081                      \
       avr32/drivers/adc                                  \
       avr32/drivers/cpu/cycle_counter                    \
       avr32/drivers/ebi/smc                              \
       avr32/drivers/flashc                               \
       avr32/drivers/gpio                                 \
       avr32/drivers/intc                                 \
       avr32/drivers/pm                                   \
       avr32/drivers/rtc                                  \
       avr32/drivers/spi                                  \
       avr32/drivers/usart                                \
       avr32/utils                                        \
       avr32/utils/preprocessor                           \
       common/boards                                      \
       common/components/memory/data_flash/at45dbx/_asf_v1 \
       common/services/clock                              \
       common/services/delay                              \
       common/services/storage/ctrl_access                \
       common/utils                                       \
       thirdparty/hd                                      \
       thirdparty/hd/example                              \
       thirdparty/hd/example/ports/avr32                  \
       thirdparty/hd/example/ports/avr32/conf             \
       thirdparty/hd/v2.6/include                         \
       thirdparty/lwip/lwip-1.3.2/src/include             \
       thirdparty/lwip/lwip-1.3.2/src/include/ipv4        \
       thirdparty/lwip/lwip-port-1.3.2/hd/if/include \
       thirdparty/hd/example/hd_spb104/at32uc3a0512_evk1105/gcc

# Additional search paths for libraries.
LIB_PATH =  \
       thirdparty/hd/v2.6/ucr2/gcc                        \
       thirdparty/hd/v2.6/ucr2/gcc                       

# List of libraries to use during linking.
LIBS =  \
       _ucr2_hd_spi_standalone_v2.6                       \
       _ucr2_hd_wl_standalone_v2.6                       

# Path relative to top level directory pointing to a linker script.
LINKER_SCRIPT = avr32/utils/linker_scripts/at32uc3a/0512/gcc/link_uc3a0512.lds

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
       -D AT45DBX_ENABLE                                  \
       -D BOARD=EVK1105                                   \
       -D EXT_BOARD=SPB104                                \
       -D WITH_GUI                                        \
       -D WITH_KEY                                        \
       -D WITH_WPA                                        \
       -D _ASSERT_ENABLE_

# Extra flags to use when linking
LDFLAGS = \
       -nostartfiles -Wl,-e,_trampoline

# Pre- and post-build commands
PREBUILD_CMD = 
POSTBUILD_CMD = 