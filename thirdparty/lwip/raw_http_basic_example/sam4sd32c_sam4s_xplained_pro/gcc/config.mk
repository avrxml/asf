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
ARCH = cortex-m4

# Target part: none, sam3n4 or sam4l4aa
PART = sam4sd32c

# Application target name. Given with suffix .a for library and .elf for a
# standalone application.
TARGET_FLASH = thirdparty_lwip_raw_basic_http_example_flash.elf
TARGET_SRAM = thirdparty_lwip_raw_basic_http_example_sram.elf

# List of C source files.
CSRCS = \
       common/services/clock/sam4s/sysclk.c               \
       common/services/delay/sam/cycle_counter.c          \
       common/services/serial/usart_serial.c              \
       common/services/spi/sam_spi/spi_master.c           \
       common/utils/interrupt/interrupt_sam_nvic.c        \
       common/utils/stdio/read.c                          \
       common/utils/stdio/write.c                         \
       sam/boards/sam4s_xplained_pro/init.c               \
       sam/components/ethernet_phy/ksz8851snl/ksz8851snl.c \
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
       sam/utils/cmsis/sam4s/source/templates/gcc/startup_sam4s.c \
       sam/utils/cmsis/sam4s/source/templates/system_sam4s.c \
       sam/utils/syscalls/gcc/syscalls.c                  \
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
       thirdparty/lwip/lwip-port-1.4.1/sam/netif/sam_spi_ksz8851snl.c \
       thirdparty/lwip/raw_http_basic_example/network/ethernet.c \
       thirdparty/lwip/raw_http_basic_example/network/httpserver/fs.c \
       thirdparty/lwip/raw_http_basic_example/network/httpserver/fsdata.c \
       thirdparty/lwip/raw_http_basic_example/network/httpserver/httpd.c \
       thirdparty/lwip/raw_http_basic_example/network/timer_mgt.c \
       thirdparty/lwip/raw_http_basic_example/raw_http_basic_example.c

# List of assembler source files.
ASSRCS = 

# List of include paths.
INC_PATH = \
       common/boards                                      \
       common/services/clock                              \
       common/services/delay                              \
       common/services/gpio                               \
       common/services/ioport                             \
       common/services/serial                             \
       common/services/serial/sam_uart                    \
       common/services/spi                                \
       common/services/spi/sam_spi                        \
       common/utils                                       \
       common/utils/stdio/stdio_serial                    \
       sam/boards                                         \
       sam/boards/sam4s_xplained_pro                      \
       sam/components/ethernet_phy/ksz8851snl             \
       sam/drivers/pdc                                    \
       sam/drivers/pdc/pdc_uart_example                   \
       sam/drivers/pio                                    \
       sam/drivers/pmc                                    \
       sam/drivers/spi                                    \
       sam/drivers/tc                                     \
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
       thirdparty/lwip/lwip-1.4.1/src/include             \
       thirdparty/lwip/lwip-1.4.1/src/include/ipv4        \
       thirdparty/lwip/lwip-1.4.1/src/include/lwip        \
       thirdparty/lwip/lwip-port-1.4.1/sam/include        \
       thirdparty/lwip/lwip-port-1.4.1/sam/include/netif  \
       thirdparty/lwip/raw_http_basic_example             \
       thirdparty/lwip/raw_http_basic_example/network     \
       thirdparty/lwip/raw_http_basic_example/network/httpserver \
       thirdparty/lwip/raw_http_basic_example/sam4sd32c_sam4s_xplained_pro \
       thirdparty/lwip/raw_http_basic_example/sam4sd32c_sam4s_xplained_pro/gcc

# Additional search paths for libraries.
LIB_PATH =  \
       thirdparty/CMSIS/Lib/GCC                          

# List of libraries to use during linking.
LIBS =  \
       arm_cortexM4l_math                                 \
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
       -D BOARD=SAM4S_XPLAINED_PRO                        \
       -D __SAM4SD32C__                                   \
       -D printf=iprintf                                  \
       -D scanf=iscanf

# Extra flags to use when linking
LDFLAGS = \
                                                          \
       -Wl,--defsym,STACK_SIZE=0x400                      \
       -Wl,--defsym,__stack_size__=0x400

# Pre- and post-build commands
PREBUILD_CMD = 
POSTBUILD_CMD = 