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
PRJ_PATH = ../../../../../../..

# Target CPU architecture: cortex-m3, cortex-m4
ARCH = cortex-m4

# Target part: none, sam3n4 or sam4l4aa
PART = sam4sd32c

# Application target name. Given with suffix .a for library and .elf for a
# standalone application.
TARGET_FLASH = wilc3000_weather_concurrent_bt_demo_flash.elf
TARGET_SRAM = wilc3000_weather_concurrent_bt_demo_sram.elf

# List of C source files.
CSRCS = \
       common/components/wifi/wilc/bsp/source/nm_bsp_sam4s.c \
       common/components/wifi/wilc/bus_wrapper/source/nm_bus_wrapper_sam4s.c \
       common/components/wifi/wilc/common/source/nm_common.c \
       common/components/wifi/wilc/driver/source/m2m_coex.c \
       common/components/wifi/wilc/driver/source/m2m_hif.c \
       common/components/wifi/wilc/driver/source/m2m_periph.c \
       common/components/wifi/wilc/driver/source/m2m_wifi.c \
       common/components/wifi/wilc/driver/source/nmasic.c \
       common/components/wifi/wilc/driver/source/nmbus.c  \
       common/components/wifi/wilc/driver/source/nmdrv.c  \
       common/components/wifi/wilc/driver/source/nmsdio.c \
       common/components/wifi/wilc/driver/source/nmspi.c  \
       common/components/wifi/wilc/driver/source/spi_flash.c \
       common/components/wifi/wilc/drv_hash/PMK.c         \
       common/components/wifi/wilc/netif/source/wilc_netif.c \
       common/components/wifi/wilc/os/source/m2m_wifi_ex.c \
       common/components/wifi/wilc/os/source/net_init.c   \
       common/components/wifi/wilc/os/source/os_hook.c    \
       common/components/wifi/wilc/weather_concurrent_bt_demo/ap.c \
       common/components/wifi/wilc/weather_concurrent_bt_demo/main.c \
       common/components/wifi/wilc/weather_concurrent_bt_demo/sta.c \
       common/services/clock/sam4s/sysclk.c               \
       common/services/delay/sam/cycle_counter.c          \
       common/services/serial/usart_serial.c              \
       common/utils/interrupt/interrupt_sam_nvic.c        \
       common/utils/osprintf/osprintf.c                   \
       common/utils/stdio/read.c                          \
       common/utils/stdio/write.c                         \
       sam/boards/sam4s_xplained_pro/init.c               \
       sam/drivers/pdc/pdc.c                              \
       sam/drivers/pio/pio.c                              \
       sam/drivers/pio/pio_handler.c                      \
       sam/drivers/pmc/pmc.c                              \
       sam/drivers/pmc/sleep.c                            \
       sam/drivers/spi/spi.c                              \
       sam/drivers/uart/uart.c                            \
       sam/drivers/usart/usart.c                          \
       sam/drivers/wdt/wdt.c                              \
       sam/utils/cmsis/sam4s/source/templates/gcc/startup_sam4s.c \
       sam/utils/cmsis/sam4s/source/templates/system_sam4s.c \
       sam/utils/syscalls/gcc/syscalls.c                  \
       thirdparty/freertos/freertos-7.3.0/source/FreeRTOS_CLI.c \
       thirdparty/freertos/freertos-7.3.0/source/list.c   \
       thirdparty/freertos/freertos-7.3.0/source/portable/gcc/sam/port.c \
       thirdparty/freertos/freertos-7.3.0/source/portable/memmang/heap_4.c \
       thirdparty/freertos/freertos-7.3.0/source/queue.c  \
       thirdparty/freertos/freertos-7.3.0/source/tasks.c  \
       thirdparty/freertos/freertos-7.3.0/source/timers.c \
       thirdparty/lwip/lwip-1.4.1-dev/src/api/api_lib.c   \
       thirdparty/lwip/lwip-1.4.1-dev/src/api/api_msg.c   \
       thirdparty/lwip/lwip-1.4.1-dev/src/api/err.c       \
       thirdparty/lwip/lwip-1.4.1-dev/src/api/netbuf.c    \
       thirdparty/lwip/lwip-1.4.1-dev/src/api/netdb.c     \
       thirdparty/lwip/lwip-1.4.1-dev/src/api/netifapi.c  \
       thirdparty/lwip/lwip-1.4.1-dev/src/api/pppapi.c    \
       thirdparty/lwip/lwip-1.4.1-dev/src/api/sockets.c   \
       thirdparty/lwip/lwip-1.4.1-dev/src/api/tcpip.c     \
       thirdparty/lwip/lwip-1.4.1-dev/src/core/def.c      \
       thirdparty/lwip/lwip-1.4.1-dev/src/core/dhcp.c     \
       thirdparty/lwip/lwip-1.4.1-dev/src/core/dns.c      \
       thirdparty/lwip/lwip-1.4.1-dev/src/core/inet_chksum.c \
       thirdparty/lwip/lwip-1.4.1-dev/src/core/init.c     \
       thirdparty/lwip/lwip-1.4.1-dev/src/core/ipv4/autoip.c \
       thirdparty/lwip/lwip-1.4.1-dev/src/core/ipv4/icmp.c \
       thirdparty/lwip/lwip-1.4.1-dev/src/core/ipv4/igmp.c \
       thirdparty/lwip/lwip-1.4.1-dev/src/core/ipv4/ip4.c \
       thirdparty/lwip/lwip-1.4.1-dev/src/core/ipv4/ip4_addr.c \
       thirdparty/lwip/lwip-1.4.1-dev/src/core/ipv4/ip_frag.c \
       thirdparty/lwip/lwip-1.4.1-dev/src/core/ipv6/dhcp6.c \
       thirdparty/lwip/lwip-1.4.1-dev/src/core/ipv6/ethip6.c \
       thirdparty/lwip/lwip-1.4.1-dev/src/core/ipv6/icmp6.c \
       thirdparty/lwip/lwip-1.4.1-dev/src/core/ipv6/inet6.c \
       thirdparty/lwip/lwip-1.4.1-dev/src/core/ipv6/ip6.c \
       thirdparty/lwip/lwip-1.4.1-dev/src/core/ipv6/ip6_addr.c \
       thirdparty/lwip/lwip-1.4.1-dev/src/core/ipv6/ip6_frag.c \
       thirdparty/lwip/lwip-1.4.1-dev/src/core/ipv6/mld6.c \
       thirdparty/lwip/lwip-1.4.1-dev/src/core/ipv6/nd6.c \
       thirdparty/lwip/lwip-1.4.1-dev/src/core/mem.c      \
       thirdparty/lwip/lwip-1.4.1-dev/src/core/memp.c     \
       thirdparty/lwip/lwip-1.4.1-dev/src/core/netif.c    \
       thirdparty/lwip/lwip-1.4.1-dev/src/core/pbuf.c     \
       thirdparty/lwip/lwip-1.4.1-dev/src/core/raw.c      \
       thirdparty/lwip/lwip-1.4.1-dev/src/core/stats.c    \
       thirdparty/lwip/lwip-1.4.1-dev/src/core/sys.c      \
       thirdparty/lwip/lwip-1.4.1-dev/src/core/tcp.c      \
       thirdparty/lwip/lwip-1.4.1-dev/src/core/tcp_in.c   \
       thirdparty/lwip/lwip-1.4.1-dev/src/core/tcp_out.c  \
       thirdparty/lwip/lwip-1.4.1-dev/src/core/timers.c   \
       thirdparty/lwip/lwip-1.4.1-dev/src/core/udp.c      \
       thirdparty/lwip/lwip-1.4.1-dev/src/netif/etharp.c  \
       thirdparty/lwip/lwip-1.4.1-dev/src/netif/ethernetif.c \
       thirdparty/lwip/lwip-1.4.1-dev/src/netif/slipif.c  \
       thirdparty/lwip/lwip-port-1.4.1-dev/sam/sys_arch.c \
       thirdparty/lwip/lwip-tinyservices/tinydhcpserver.c \
       thirdparty/lwip/lwip-tinyservices/tinyservices.c

# List of assembler source files.
ASSRCS = 

# List of include paths.
INC_PATH = \
       common/boards                                      \
       common/components/wifi/wilc                        \
       common/components/wifi/wilc/weather_concurrent_bt_demo \
       common/components/wifi/wilc/weather_concurrent_bt_demo/sam4sd32c_sam4s_xplained_pro \
       common/services/clock                              \
       common/services/delay                              \
       common/services/gpio                               \
       common/services/ioport                             \
       common/services/serial                             \
       common/services/serial/sam_uart                    \
       common/utils                                       \
       common/utils/osprintf                              \
       common/utils/stdio/stdio_serial                    \
       sam/boards                                         \
       sam/boards/sam4s_xplained_pro                      \
       sam/drivers/pdc                                    \
       sam/drivers/pdc/pdc_uart_example                   \
       sam/drivers/pio                                    \
       sam/drivers/pmc                                    \
       sam/drivers/spi                                    \
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
       thirdparty/freertos/freertos-7.3.0/source/include  \
       thirdparty/freertos/freertos-7.3.0/source/portable/gcc/sam \
       thirdparty/lwip/lwip-1.4.1-dev/src/include         \
       thirdparty/lwip/lwip-1.4.1-dev/src/include/lwip    \
       thirdparty/lwip/lwip-port-1.4.1-dev/sam/include    \
       thirdparty/lwip/lwip-tinyservices \
       common/components/wifi/wilc/weather_concurrent_bt_demo/sam4sd32c_sam4s_xplained_pro/gcc

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
       -D __FREERTOS__                                    \
       -D __SAM4SD32C__                                   \
       -D printf=iprintf                                  \
       -D scanf=iscanf

# Extra flags to use when linking
LDFLAGS = \

# Pre- and post-build commands
PREBUILD_CMD = 
POSTBUILD_CMD = 