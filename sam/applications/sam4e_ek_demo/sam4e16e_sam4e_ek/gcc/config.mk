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
PART = sam4e16e

# Application target name. Given with suffix .a for library and .elf for a
# standalone application.
TARGET_FLASH = sam4e_ek_demo_flash.elf
TARGET_SRAM = sam4e_ek_demo_sram.elf

# List of C source files.
CSRCS = \
       common/components/touch/qt/qt_i2c/qt_i2c.c         \
       common/services/clock/sam4e/sysclk.c               \
       common/services/delay/sam/cycle_counter.c          \
       common/services/serial/usart_serial.c              \
       common/services/sleepmgr/sam/sleepmgr.c            \
       common/services/spi/sam_spi/spi_master.c           \
       common/utils/interrupt/interrupt_sam_nvic.c        \
       common/utils/stdio/read.c                          \
       common/utils/stdio/write.c                         \
       sam/applications/sam4e_ek_demo/main.c              \
       sam/applications/sam4e_ek_demo/network/ethernet_sam.c \
       sam/applications/sam4e_ek_demo/network/httpserver_raw/cgi.c \
       sam/applications/sam4e_ek_demo/network/httpserver_raw/fs.c \
       sam/applications/sam4e_ek_demo/network/httpserver_raw/fsdata.c \
       sam/applications/sam4e_ek_demo/network/httpserver_raw/httpd.c \
       sam/applications/sam4e_ek_demo/network/timer_mgt_sam.c \
       sam/applications/sam4e_ek_demo/task_dsp.c          \
       sam/applications/sam4e_ek_demo/task_gfx.c          \
       sam/applications/sam4e_ek_demo/task_qtouch_cpn.c   \
       sam/applications/sam4e_ek_demo/task_webserver.c    \
       sam/components/display/aat31xx/aat31xx.c           \
       sam/components/display/ili93xx/ili93xx.c           \
       sam/components/ethernet_phy/ksz8051mnl/ethernet_phy.c \
       sam/components/resistive_touch/ads7843/ads7843.c   \
       sam/drivers/afec/afec.c                            \
       sam/drivers/dacc/dacc.c                            \
       sam/drivers/ebi/smc/smc.c                          \
       sam/drivers/gmac/gmac.c                            \
       sam/drivers/pdc/pdc.c                              \
       sam/drivers/pio/pio.c                              \
       sam/drivers/pio/pio_handler.c                      \
       sam/drivers/pmc/pmc.c                              \
       sam/drivers/pmc/sleep.c                            \
       sam/drivers/rstc/rstc.c                            \
       sam/drivers/spi/spi.c                              \
       sam/drivers/tc/tc.c                                \
       sam/drivers/twi/twi.c                              \
       sam/drivers/uart/uart.c                            \
       sam/drivers/usart/usart.c                          \
       sam/services/resistive_touch/rtouch.c              \
       sam/utils/cmsis/sam4e/source/templates/exceptions.c \
       sam/utils/cmsis/sam4e/source/templates/gcc/startup_sam4e.c \
       sam/utils/cmsis/sam4e/source/templates/system_sam4e.c \
       sam/utils/syscalls/gcc/syscalls.c                  \
       thirdparty/freertos/freertos-7.0.0/source/croutine.c \
       thirdparty/freertos/freertos-7.0.0/source/list.c   \
       thirdparty/freertos/freertos-7.0.0/source/portable/gcc/sam_cm4f/port.c \
       thirdparty/freertos/freertos-7.0.0/source/portable/memmang/heap_3.c \
       thirdparty/freertos/freertos-7.0.0/source/queue.c  \
       thirdparty/freertos/freertos-7.0.0/source/tasks.c  \
       thirdparty/freertos/freertos-7.0.0/source/timers.c \
       thirdparty/lwip/lwip-1.4.0/src/api/api_lib.c       \
       thirdparty/lwip/lwip-1.4.0/src/api/api_msg.c       \
       thirdparty/lwip/lwip-1.4.0/src/api/err.c           \
       thirdparty/lwip/lwip-1.4.0/src/api/netbuf.c        \
       thirdparty/lwip/lwip-1.4.0/src/api/netdb.c         \
       thirdparty/lwip/lwip-1.4.0/src/api/netifapi.c      \
       thirdparty/lwip/lwip-1.4.0/src/api/sockets.c       \
       thirdparty/lwip/lwip-1.4.0/src/api/tcpip.c         \
       thirdparty/lwip/lwip-1.4.0/src/core/def.c          \
       thirdparty/lwip/lwip-1.4.0/src/core/dhcp.c         \
       thirdparty/lwip/lwip-1.4.0/src/core/dns.c          \
       thirdparty/lwip/lwip-1.4.0/src/core/init.c         \
       thirdparty/lwip/lwip-1.4.0/src/core/ipv4/autoip.c  \
       thirdparty/lwip/lwip-1.4.0/src/core/ipv4/icmp.c    \
       thirdparty/lwip/lwip-1.4.0/src/core/ipv4/igmp.c    \
       thirdparty/lwip/lwip-1.4.0/src/core/ipv4/inet.c    \
       thirdparty/lwip/lwip-1.4.0/src/core/ipv4/inet_chksum.c \
       thirdparty/lwip/lwip-1.4.0/src/core/ipv4/ip.c      \
       thirdparty/lwip/lwip-1.4.0/src/core/ipv4/ip_addr.c \
       thirdparty/lwip/lwip-1.4.0/src/core/ipv4/ip_frag.c \
       thirdparty/lwip/lwip-1.4.0/src/core/lwip_timers_140.c \
       thirdparty/lwip/lwip-1.4.0/src/core/mem.c          \
       thirdparty/lwip/lwip-1.4.0/src/core/memp.c         \
       thirdparty/lwip/lwip-1.4.0/src/core/netif.c        \
       thirdparty/lwip/lwip-1.4.0/src/core/pbuf.c         \
       thirdparty/lwip/lwip-1.4.0/src/core/raw.c          \
       thirdparty/lwip/lwip-1.4.0/src/core/stats.c        \
       thirdparty/lwip/lwip-1.4.0/src/core/sys.c          \
       thirdparty/lwip/lwip-1.4.0/src/core/tcp.c          \
       thirdparty/lwip/lwip-1.4.0/src/core/tcp_in.c       \
       thirdparty/lwip/lwip-1.4.0/src/core/tcp_out.c      \
       thirdparty/lwip/lwip-1.4.0/src/core/udp.c          \
       thirdparty/lwip/lwip-1.4.0/src/netif/etharp.c      \
       thirdparty/lwip/lwip-port-1.4.0/sam/netif/ethernetif_sam4.c \
       thirdparty/lwip/lwip-port-1.4.0/sam/sys_arch.c

# List of assembler source files.
ASSRCS = 

# List of include paths.
INC_PATH = \
       common/boards                                      \
       common/components/touch/qt/qt_i2c                  \
       common/components/touch/qt/qt_i2c/device_specific  \
       common/services/clock                              \
       common/services/delay                              \
       common/services/ioport                             \
       common/services/serial                             \
       common/services/serial/sam_uart                    \
       common/services/sleepmgr                           \
       common/services/spi                                \
       common/services/spi/sam_spi                        \
       common/services/twi                                \
       common/utils                                       \
       common/utils/stdio/stdio_serial                    \
       sam/applications/sam4e_ek_demo                     \
       sam/applications/sam4e_ek_demo/network             \
       sam/applications/sam4e_ek_demo/network/httpserver_raw \
       sam/applications/sam4e_ek_demo/sam4e16e_sam4e_ek   \
       sam/boards                                         \
       sam/boards/sam4e_ek                                \
       sam/components/display/aat31xx                     \
       sam/components/display/ili93xx                     \
       sam/components/ethernet_phy/ksz8051mnl             \
       sam/components/resistive_touch/ads7843             \
       sam/drivers/afec                                   \
       sam/drivers/dacc                                   \
       sam/drivers/ebi/smc                                \
       sam/drivers/gmac                                   \
       sam/drivers/pdc                                    \
       sam/drivers/pio                                    \
       sam/drivers/pmc                                    \
       sam/drivers/rstc                                   \
       sam/drivers/spi                                    \
       sam/drivers/tc                                     \
       sam/drivers/twi                                    \
       sam/drivers/uart                                   \
       sam/drivers/usart                                  \
       sam/services/resistive_touch                       \
       sam/utils                                          \
       sam/utils/cmsis/sam4e/include                      \
       sam/utils/cmsis/sam4e/source/templates             \
       sam/utils/fpu                                      \
       sam/utils/header_files                             \
       sam/utils/preprocessor                             \
       thirdparty/CMSIS/Include                           \
       thirdparty/CMSIS/Lib/GCC                           \
       thirdparty/freertos/freertos-7.0.0/source/include  \
       thirdparty/freertos/freertos-7.0.0/source/portable/gcc/sam_cm4f \
       thirdparty/lwip/lwip-1.4.0/src/include             \
       thirdparty/lwip/lwip-1.4.0/src/include/ipv4        \
       thirdparty/lwip/lwip-1.4.0/src/include/lwip        \
       thirdparty/lwip/lwip-port-1.4.0/sam/include \
       sam/applications/sam4e_ek_demo/sam4e16e_sam4e_ek/gcc

# Additional search paths for libraries.
LIB_PATH =  \
       thirdparty/CMSIS/Lib/GCC                          

# List of libraries to use during linking.
LIBS =  \
       arm_cortexM4lf_math                                \
       m                                                 

# Path relative to top level directory pointing to a linker script.
LINKER_SCRIPT_FLASH = sam/utils/linker_scripts/sam4e/sam4e16e/gcc/flash.ld
LINKER_SCRIPT_SRAM  = sam/utils/linker_scripts/sam4e/sam4e16e/gcc/sram.ld

# Path relative to top level directory pointing to a linker script.
DEBUG_SCRIPT_FLASH = sam/boards/sam4e_ek/debug_scripts/gcc/sam4e_ek_flash.gdb
DEBUG_SCRIPT_SRAM  = sam/boards/sam4e_ek/debug_scripts/gcc/sam4e_ek_sram.gdb

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
ASFLAGS =  \
       -mfloat-abi=softfp                                 \
       -mfpu=vfpv4                                       

# Extra flags to use when compiling.
CFLAGS =  \
       -mfloat-abi=softfp                                 \
       -mfpu=vfpv4                                       

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
       -D BOARD=SAM4E_EK                                  \
       -D FREERTOS_USED                                   \
       -D HTTP_USED=1                                     \
       -D TFTP_USED=1                                     \
       -D __SAM4E16E__                                    \
       -D printf=iprintf

# Extra flags to use when linking
LDFLAGS = \
