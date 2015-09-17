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
ARCH = cortex-m0plus

# Target part: none, sam3n4 or sam4l4aa
PART = samr21g18a

# Application target name. Given with suffix .a for library and .elf for a
# standalone application.
TARGET_FLASH = examples_mqtt-example_flash.elf
TARGET_SRAM = examples_mqtt-example_sram.elf

# List of C source files.
CSRCS = \
       common/utils/interrupt/interrupt_sam_nvic.c        \
       common2/services/delay/sam0/systick_counter.c      \
       sam0/boards/samr21_xplained_pro/board_init.c       \
       sam0/drivers/adc/adc_sam_d_r/adc.c                 \
       sam0/drivers/extint/extint_callback.c              \
       sam0/drivers/extint/extint_sam_d_r/extint.c        \
       sam0/drivers/nvm/nvm.c                             \
       sam0/drivers/port/port.c                           \
       sam0/drivers/rtc/rtc_sam_d_r/rtc_count.c           \
       sam0/drivers/rtc/rtc_sam_d_r/rtc_count_interrupt.c \
       sam0/drivers/sercom/i2c/i2c_sam0/i2c_master.c      \
       sam0/drivers/sercom/sercom.c                       \
       sam0/drivers/sercom/sercom_interrupt.c             \
       sam0/drivers/sercom/spi/spi.c                      \
       sam0/drivers/sercom/usart/usart.c                  \
       sam0/drivers/sercom/usart/usart_interrupt.c        \
       sam0/drivers/system/clock/clock_samd21_r21_da/clock.c \
       sam0/drivers/system/clock/clock_samd21_r21_da/gclk.c \
       sam0/drivers/system/interrupt/system_interrupt.c   \
       sam0/drivers/system/pinmux/pinmux.c                \
       sam0/drivers/system/system.c                       \
       sam0/drivers/tc/tc_interrupt.c                     \
       sam0/drivers/tc/tc_sam_d_r/tc.c                    \
       sam0/drivers/wdt/wdt.c                             \
       sam0/drivers/wdt/wdt_callback.c                    \
       sam0/utils/cmsis/samr21/source/gcc/startup_samr21.c \
       sam0/utils/cmsis/samr21/source/system_samr21.c     \
       sam0/utils/stdio/read.c                            \
       sam0/utils/stdio/write.c                           \
       sam0/utils/syscalls/gcc/syscalls.c                 \
       thirdparty/wireless/SmartConnect_6LoWPAN/apps/aes/aes-ccm.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/apps/aes/aes.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/apps/aes/netstack-aes.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/apps/ip64-addr/ip64-addr.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/apps/led_control/ledctrl.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/apps/mdns/mdns.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/apps/simple-rpl/simple-rpl.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/dev/leds.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/dev/nullradio.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/dev/serial-line.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/dev/slip.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/lib/assert.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/lib/crc16.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/lib/gcr.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/lib/ifft.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/lib/list.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/lib/me.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/lib/me_tabs.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/lib/memb.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/lib/mmem.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/lib/petsciiconv.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/lib/print-stats.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/lib/random.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/lib/ringbuf.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/lib/sensors.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/lib/trickle-timer.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/net/ip/dhcpc.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/net/ip/psock.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/net/ip/resolv.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/net/ip/simple-udp.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/net/ip/slipdev.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/net/ip/tcp-socket.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/net/ip/tcpip.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/net/ip/udp-socket.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/net/ip/uip-debug.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/net/ip/uip-packetqueue.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/net/ip/uip-split.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/net/ip/uip-udp-packet.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/net/ip/uiplib.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/net/ipv4/uaodv-rt.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/net/ipv4/uaodv.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/net/ipv4/uip-fw-drv.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/net/ipv4/uip-fw.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/net/ipv4/uip-neighbor.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/net/ipv4/uip-over-mesh.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/net/ipv4/uip.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/net/ipv4/uip_arp.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/net/ipv6/sicslowpan.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/net/ipv6/uip-ds6-nbr.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/net/ipv6/uip-ds6-route.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/net/ipv6/uip-ds6.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/net/ipv6/uip-icmp6.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/net/ipv6/uip-nd6.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/net/ipv6/uip6.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/net/linkaddr.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/net/mac/contikimac/contikimac.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/net/mac/csma.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/net/mac/frame802154.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/net/mac/framer-802154.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/net/mac/framer-nullmac.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/net/mac/mac-sequence.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/net/mac/mac.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/net/mac/nullmac.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/net/mac/nullrdc-noframer.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/net/mac/nullrdc.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/net/mac/phase.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/net/nbr-table.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/net/netstack.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/net/packetbuf.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/net/packetqueue.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/net/queuebuf.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/net/rime/rimestats.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/net/rpl/rpl-dag.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/net/rpl/rpl-ext-header.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/net/rpl/rpl-icmp6.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/net/rpl/rpl-mrhof.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/net/rpl/rpl-of0.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/net/rpl/rpl-timers.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/net/rpl/rpl.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/sys/arg.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/sys/autostart.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/sys/compower.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/sys/ctimer.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/sys/energest.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/sys/etimer.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/sys/mt.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/sys/process.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/sys/procinit.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/sys/rtimer.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/sys/stimer.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/sys/timer.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/dev/at86rf233/rf233.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/examples/mqtt-example/mqtt-example-main.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/examples/mqtt-example/mqtt-example.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/examples/mqtt-example/symbols.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/services/sam0/battery-sensor.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/services/sam0/button-sensor.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/services/sam0/edbg-eui.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/services/sam0/flash.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/services/sam0/leds-arch.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/services/sam0/node-id.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/services/sam0/sam0-sensors.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/services/sam0/sam0_sensors.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/services/sam0/temp-sensor.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/services/sam0/voltage-sensor.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/services/sam0/watchdog.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/services/timer/sam0/tc-clock.c \
       thirdparty/wireless/SmartConnect_6LoWPAN/services/timer/sam0/tc-rtimer-arch.c \
       thirdparty/wireless/addons/sio2host/uart/sio2host.c \
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
       sam0/boards/samr21_xplained_pro                    \
       sam0/drivers/adc                                   \
       sam0/drivers/adc/adc_sam_d_r                       \
       sam0/drivers/extint                                \
       sam0/drivers/extint/extint_sam_d_r                 \
       sam0/drivers/nvm                                   \
       sam0/drivers/port                                  \
       sam0/drivers/rtc                                   \
       sam0/drivers/rtc/rtc_sam_d_r                       \
       sam0/drivers/sercom                                \
       sam0/drivers/sercom/i2c                            \
       sam0/drivers/sercom/i2c/i2c_sam0                   \
       sam0/drivers/sercom/spi                            \
       sam0/drivers/sercom/usart                          \
       sam0/drivers/system                                \
       sam0/drivers/system/clock                          \
       sam0/drivers/system/clock/clock_samd21_r21_da      \
       sam0/drivers/system/interrupt                      \
       sam0/drivers/system/interrupt/system_interrupt_samr21 \
       sam0/drivers/system/pinmux                         \
       sam0/drivers/system/power                          \
       sam0/drivers/system/power/power_sam_d_r            \
       sam0/drivers/system/reset                          \
       sam0/drivers/system/reset/reset_sam_d_r            \
       sam0/drivers/tc                                    \
       sam0/drivers/tc/tc_sam_d_r                         \
       sam0/drivers/wdt                                   \
       sam0/utils                                         \
       sam0/utils/cmsis/samr21/include                    \
       sam0/utils/cmsis/samr21/source                     \
       sam0/utils/header_files                            \
       sam0/utils/preprocessor                            \
       sam0/utils/stdio/stdio_serial                      \
       thirdparty/CMSIS/Include                           \
       thirdparty/CMSIS/Lib/GCC                           \
       thirdparty/wireless/SmartConnect_6LoWPAN           \
       thirdparty/wireless/SmartConnect_6LoWPAN/apps/aes  \
       thirdparty/wireless/SmartConnect_6LoWPAN/apps/ip64-addr \
       thirdparty/wireless/SmartConnect_6LoWPAN/apps/led_control \
       thirdparty/wireless/SmartConnect_6LoWPAN/apps/mdns \
       thirdparty/wireless/SmartConnect_6LoWPAN/apps/mqtt \
       thirdparty/wireless/SmartConnect_6LoWPAN/apps/simple-rpl \
       thirdparty/wireless/SmartConnect_6LoWPAN/apps/thsq-conf \
       thirdparty/wireless/SmartConnect_6LoWPAN/core      \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/dev  \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/lib  \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/loader \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/net  \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/net/ip \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/net/ipv4 \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/net/ipv6 \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/net/ipv6/multicast \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/net/mac \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/net/mac/contikimac \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/net/rime \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/net/rpl \
       thirdparty/wireless/SmartConnect_6LoWPAN/core/sys  \
       thirdparty/wireless/SmartConnect_6LoWPAN/dev/at86rf233 \
       thirdparty/wireless/SmartConnect_6LoWPAN/examples/mqtt-example \
       thirdparty/wireless/SmartConnect_6LoWPAN/examples/mqtt-example/inc \
       thirdparty/wireless/SmartConnect_6LoWPAN/examples/mqtt-example/samr21_xpro \
       thirdparty/wireless/SmartConnect_6LoWPAN/services/sam0 \
       thirdparty/wireless/SmartConnect_6LoWPAN/services/timer/sam0 \
       thirdparty/wireless/addons/sio2host/uart           \
       thirdparty/wireless/services/trx_access \
       thirdparty/wireless/SmartConnect_6LoWPAN/examples/mqtt-example/samr21_xpro/gcc

# Additional search paths for libraries.
LIB_PATH =  \
       thirdparty/CMSIS/Lib/GCC                           \
       thirdparty/wireless/SmartConnect_6LoWPAN/apps/mqtt/mqtt_samr21

# List of libraries to use during linking.
LIBS =  \
       arm_cortexM0l_math                                 \
       MQTT_SAMR21                                       

# Path relative to top level directory pointing to a linker script.
LINKER_SCRIPT_FLASH = sam0/utils/linker_scripts/samr21/gcc/samr21g18a_flash.ld
LINKER_SCRIPT_SRAM  = sam0/utils/linker_scripts/samr21/gcc/samr21g18a_sram.ld

# Path relative to top level directory pointing to a linker script.
DEBUG_SCRIPT_FLASH = sam0/boards/samr21_xplained_pro/debug_scripts/gcc/samr21_xplained_pro_flash.gdb
DEBUG_SCRIPT_SRAM  = sam0/boards/samr21_xplained_pro/debug_scripts/gcc/samr21_xplained_pro_sram.gdb

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
       -D ADC_CALLBACK_MODE=false                         \
       -D ARM_MATH_CM0PLUS=true                           \
       -D AUTOSTART_ENABLE=1                              \
       -D BOARD=SAMR21_XPLAINED_PRO                       \
       -D ENABLE_LEDCTRL=1                                \
       -D EXTINT_CALLBACK_MODE=true                       \
       -D HW_ACK=1                                        \
       -D I2C_MASTER_CALLBACK_MODE=false                  \
       -D MAC=NULLMAC                                     \
       -D MESH_NODE=1                                     \
       -D RDC=NULLRDC                                     \
       -D RTC_COUNT_ASYNC=true                            \
       -D SPI_CALLBACK_MODE=false                         \
       -D SYSTICK_MODE                                    \
       -D TC_ASYNC=true                                   \
       -D UIP_CONF_IPV6=1                                 \
       -D USART_CALLBACK_MODE=true                        \
       -D WDT_CALLBACK_MODE=true                          \
       -D __SAMR21G18A__

# Extra flags to use when linking
LDFLAGS = \

# Pre- and post-build commands
PREBUILD_CMD = 
POSTBUILD_CMD = 