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
PRJ_PATH = ../../../../..

# Target CPU architecture: cortex-m3, cortex-m4
ARCH = cortex-m0plus

# Target part: none, sam3n4 or sam4l4aa
PART = samd20j18

# Application target name. Given with suffix .a for library and .elf for a
# standalone application.
TARGET_FLASH = thirdparty_lwip_raw_basic_http_example_flash.elf
TARGET_SRAM = thirdparty_lwip_raw_basic_http_example_sram.elf

# List of C source files.
CSRCS = \
       common/utils/interrupt/interrupt_sam_nvic.c        \
       common2/services/delay/sam0/systick_counter.c      \
       sam0/boards/samd20_xplained_pro/board_init.c       \
       sam0/components/ethernet_phy/ksz8851snl/ksz8851snl.c \
       sam0/drivers/extint/extint_callback.c              \
       sam0/drivers/extint/extint_sam_d_r/extint.c        \
       sam0/drivers/port/port.c                           \
       sam0/drivers/sercom/sercom.c                       \
       sam0/drivers/sercom/sercom_interrupt.c             \
       sam0/drivers/sercom/spi/spi.c                      \
       sam0/drivers/sercom/spi/spi_interrupt.c            \
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
       thirdparty/lwip/lwip-1.4.1/src/api/api_lib.c       \
       thirdparty/lwip/lwip-1.4.1/src/api/api_msg.c       \
       thirdparty/lwip/lwip-1.4.1/src/api/err.c           \
       thirdparty/lwip/lwip-1.4.1/src/api/netbuf.c        \
       thirdparty/lwip/lwip-1.4.1/src/api/netdb.c         \
       thirdparty/lwip/lwip-1.4.1/src/api/netifapi.c      \
       thirdparty/lwip/lwip-1.4.1/src/api/sockets.c       \
       thirdparty/lwip/lwip-1.4.1/src/api/tcpip.c         \
       thirdparty/lwip/lwip-1.4.1/src/core/def.c          \
       thirdparty/lwip/lwip-1.4.1/src/core/dhcp.c         \
       thirdparty/lwip/lwip-1.4.1/src/core/dns.c          \
       thirdparty/lwip/lwip-1.4.1/src/core/ipv4/autoip.c  \
       thirdparty/lwip/lwip-1.4.1/src/core/ipv4/icmp.c    \
       thirdparty/lwip/lwip-1.4.1/src/core/ipv4/igmp.c    \
       thirdparty/lwip/lwip-1.4.1/src/core/ipv4/inet.c    \
       thirdparty/lwip/lwip-1.4.1/src/core/ipv4/inet_chksum.c \
       thirdparty/lwip/lwip-1.4.1/src/core/ipv4/ip.c      \
       thirdparty/lwip/lwip-1.4.1/src/core/ipv4/ip_addr.c \
       thirdparty/lwip/lwip-1.4.1/src/core/ipv4/ip_frag.c \
       thirdparty/lwip/lwip-1.4.1/src/core/lwip_init.c    \
       thirdparty/lwip/lwip-1.4.1/src/core/lwip_timers_141.c \
       thirdparty/lwip/lwip-1.4.1/src/core/mem.c          \
       thirdparty/lwip/lwip-1.4.1/src/core/memp.c         \
       thirdparty/lwip/lwip-1.4.1/src/core/netif.c        \
       thirdparty/lwip/lwip-1.4.1/src/core/pbuf.c         \
       thirdparty/lwip/lwip-1.4.1/src/core/raw.c          \
       thirdparty/lwip/lwip-1.4.1/src/core/stats.c        \
       thirdparty/lwip/lwip-1.4.1/src/core/sys.c          \
       thirdparty/lwip/lwip-1.4.1/src/core/tcp.c          \
       thirdparty/lwip/lwip-1.4.1/src/core/tcp_in.c       \
       thirdparty/lwip/lwip-1.4.1/src/core/tcp_out.c      \
       thirdparty/lwip/lwip-1.4.1/src/core/udp.c          \
       thirdparty/lwip/lwip-1.4.1/src/netif/etharp.c      \
       thirdparty/lwip/lwip-port-1.4.1/sam/netif/sam0_spi_ksz8851snl.c \
       thirdparty/lwip/raw_http_basic_example/network/ethernet.c \
       thirdparty/lwip/raw_http_basic_example/network/httpserver/fs.c \
       thirdparty/lwip/raw_http_basic_example/network/httpserver/fsdata.c \
       thirdparty/lwip/raw_http_basic_example/network/httpserver/httpd.c \
       thirdparty/lwip/raw_http_basic_example/network/timer_mgt.c \
       thirdparty/lwip/raw_http_basic_example/raw_http_basic_example_samd20.c

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
       sam0/components/ethernet_phy/ksz8851snl            \
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
       thirdparty/lwip/lwip-1.4.1/src/include             \
       thirdparty/lwip/lwip-1.4.1/src/include/ipv4        \
       thirdparty/lwip/lwip-1.4.1/src/include/lwip        \
       thirdparty/lwip/lwip-port-1.4.1/sam/include        \
       thirdparty/lwip/lwip-port-1.4.1/sam/include/netif  \
       thirdparty/lwip/raw_http_basic_example             \
       thirdparty/lwip/raw_http_basic_example/network     \
       thirdparty/lwip/raw_http_basic_example/network/httpserver \
       thirdparty/lwip/raw_http_basic_example/samd20_xplained_pro \
       thirdparty/lwip/raw_http_basic_example/samd20_xplained_pro/gcc

# Additional search paths for libraries.
LIB_PATH =  \
       thirdparty/CMSIS/Lib/GCC                          

# List of libraries to use during linking.
LIBS =  \
       arm_cortexM0l_math                                

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
       -D ARM_MATH_CM0PLUS=true                           \
       -D BOARD=SAMD20_XPLAINED_PRO                       \
       -D EXTINT_CALLBACK_MODE=true                       \
       -D SPI_CALLBACK_MODE=true                          \
       -D SYSTICK_MODE                                    \
       -D TC_ASYNC=true                                   \
       -D USART_CALLBACK_MODE=true                        \
       -D __SAMD20J18__

# Extra flags to use when linking
LDFLAGS = \
                                                          \
       -Wl,--defsym,STACK_SIZE=0xa00                      \
       -Wl,--defsym,__stack_size__=0xa00

# Pre- and post-build commands
PREBUILD_CMD = 
POSTBUILD_CMD = 