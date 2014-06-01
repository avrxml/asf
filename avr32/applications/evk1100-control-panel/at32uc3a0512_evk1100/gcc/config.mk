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
PRJ_PATH = ../../../../..

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
TARGET = demo_evk1100_example.elf

# List of C source files.
CSRCS = \
       avr32/applications/evk1100-control-panel/actuators/actuator.c \
       avr32/applications/evk1100-control-panel/actuators/cpled.c \
       avr32/applications/evk1100-control-panel/actuators/lcd.c \
       avr32/applications/evk1100-control-panel/config/config_file.c \
       avr32/applications/evk1100-control-panel/localctrl/cptime.c \
       avr32/applications/evk1100-control-panel/localctrl/datalog.c \
       avr32/applications/evk1100-control-panel/localctrl/supervisor.c \
       avr32/applications/evk1100-control-panel/localctrl/syscmds.c \
       avr32/applications/evk1100-control-panel/main.c    \
       avr32/applications/evk1100-control-panel/mmi/mmi.c \
       avr32/applications/evk1100-control-panel/network/ethernet.c \
       avr32/applications/evk1100-control-panel/network/smtpclient/BasicSMTP.c \
       avr32/applications/evk1100-control-panel/network/tftpserver/BasicTFTP.c \
       avr32/applications/evk1100-control-panel/network/webserver/BasicWEB.c \
       avr32/applications/evk1100-control-panel/sensors/joystick.c \
       avr32/applications/evk1100-control-panel/sensors/light.c \
       avr32/applications/evk1100-control-panel/sensors/potentiometer.c \
       avr32/applications/evk1100-control-panel/sensors/pushb.c \
       avr32/applications/evk1100-control-panel/sensors/sensor.c \
       avr32/applications/evk1100-control-panel/sensors/temperature.c \
       avr32/applications/evk1100-control-panel/serial/serial.c \
       avr32/applications/evk1100-control-panel/shell/com1shell.c \
       avr32/applications/evk1100-control-panel/shell/fscmds.c \
       avr32/applications/evk1100-control-panel/shell/shell.c \
       avr32/applications/evk1100-control-panel/usb/appli/enum/device/usb_descriptors.c \
       avr32/applications/evk1100-control-panel/usb/appli/enum/device/usb_specific_request.c \
       avr32/applications/evk1100-control-panel/usb/appli/mass_storage/device_mass_storage_task.c \
       avr32/applications/evk1100-control-panel/usb/appli/mass_storage/host_mass_storage_task.c \
       avr32/applications/evk1100-control-panel/usb/appli/usbsys.c \
       avr32/applications/evk1100-control-panel/utils/printf-stdarg.c \
       avr32/applications/evk1100-control-panel/utils/tracedump.c \
       avr32/boards/evk1100/led.c                         \
       avr32/components/display/dip204/dip204.c           \
       avr32/components/memory/sd_mmc/sd_mmc_spi/sd_mmc_spi.c \
       avr32/components/memory/sd_mmc/sd_mmc_spi/sd_mmc_spi_mem.c \
       avr32/drivers/adc/adc.c                            \
       avr32/drivers/ebi/sdramc/sdramc.c                  \
       avr32/drivers/flashc/flashc.c                      \
       avr32/drivers/gpio/gpio.c                          \
       avr32/drivers/intc/intc.c                          \
       avr32/drivers/macb/macb.c                          \
       avr32/drivers/pm/pm.c                              \
       avr32/drivers/pm/pm_conf_clocks.c                  \
       avr32/drivers/pm/power_clocks_lib.c                \
       avr32/drivers/pwm/pwm.c                            \
       avr32/drivers/spi/spi.c                            \
       avr32/drivers/tc/tc.c                              \
       avr32/drivers/usart/usart.c                        \
       avr32/drivers/usbb/_asf_v1/enum/device/usb_device_task.c \
       avr32/drivers/usbb/_asf_v1/enum/device/usb_standard_request.c \
       avr32/drivers/usbb/_asf_v1/enum/host/usb_host_enum.c \
       avr32/drivers/usbb/_asf_v1/enum/host/usb_host_task.c \
       avr32/drivers/usbb/_asf_v1/enum/usb_task.c         \
       avr32/drivers/usbb/_asf_v1/usb_drv.c               \
       avr32/services/fs/fat/fat.c                        \
       avr32/services/fs/fat/fat_unusual.c                \
       avr32/services/fs/fat/file.c                       \
       avr32/services/fs/fat/fsaccess.c                   \
       avr32/services/fs/fat/navigation.c                 \
       avr32/services/usb/_asf_v1/class/mass_storage/host_mem/host_mem.c \
       avr32/services/usb/_asf_v1/class/mass_storage/scsi_decoder/scsi_decoder.c \
       avr32/utils/debug/print_funcs.c                    \
       common/components/memory/data_flash/at45dbx/_asf_v1/at45dbx.c \
       common/components/memory/data_flash/at45dbx/_asf_v1/at45dbx_mem.c \
       common/services/clock/uc3a0_a1/sysclk.c            \
       common/services/storage/ctrl_access/ctrl_access.c  \
       thirdparty/freertos/freertos-7.0.0/source/croutine.c \
       thirdparty/freertos/freertos-7.0.0/source/list.c   \
       thirdparty/freertos/freertos-7.0.0/source/portable/gcc/avr32_uc3/port.c \
       thirdparty/freertos/freertos-7.0.0/source/portable/gcc/avr32_uc3/read.c \
       thirdparty/freertos/freertos-7.0.0/source/portable/gcc/avr32_uc3/write.c \
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
       thirdparty/lwip/lwip-port-1.4.0/at32uc3/netif/ethernetif.c \
       thirdparty/lwip/lwip-port-1.4.0/at32uc3/sys_arch.c

# List of assembler source files.
ASSRCS = \
       avr32/utils/startup/trampoline_uc3.S               \
       thirdparty/freertos/freertos-7.0.0/source/portable/gcc/avr32_uc3/exception.S

# List of include paths.
INC_PATH = \
       avr32/applications/evk1100-control-panel           \
       avr32/applications/evk1100-control-panel/actuators \
       avr32/applications/evk1100-control-panel/config    \
       avr32/applications/evk1100-control-panel/localctrl \
       avr32/applications/evk1100-control-panel/mmi       \
       avr32/applications/evk1100-control-panel/network   \
       avr32/applications/evk1100-control-panel/network/smtpclient \
       avr32/applications/evk1100-control-panel/network/tftpserver \
       avr32/applications/evk1100-control-panel/network/webserver \
       avr32/applications/evk1100-control-panel/sensors   \
       avr32/applications/evk1100-control-panel/serial    \
       avr32/applications/evk1100-control-panel/shell     \
       avr32/applications/evk1100-control-panel/usb/appli \
       avr32/applications/evk1100-control-panel/usb/appli/enum \
       avr32/applications/evk1100-control-panel/usb/appli/enum/device \
       avr32/applications/evk1100-control-panel/usb/appli/mass_storage \
       avr32/applications/evk1100-control-panel/utils     \
       avr32/boards                                       \
       avr32/boards/evk1100                               \
       avr32/components/display/dip204                    \
       avr32/components/ethernet_phy/dp83848              \
       avr32/components/joystick/skrhabe010               \
       avr32/components/memory/sd_mmc/sd_mmc_spi          \
       avr32/components/memory/sdram                      \
       avr32/drivers/adc                                  \
       avr32/drivers/cpu/cycle_counter                    \
       avr32/drivers/ebi/sdramc                           \
       avr32/drivers/flashc                               \
       avr32/drivers/gpio                                 \
       avr32/drivers/intc                                 \
       avr32/drivers/macb                                 \
       avr32/drivers/pm                                   \
       avr32/drivers/pwm                                  \
       avr32/drivers/spi                                  \
       avr32/drivers/tc                                   \
       avr32/drivers/usart                                \
       avr32/drivers/usbb/_asf_v1                         \
       avr32/drivers/usbb/_asf_v1/enum                    \
       avr32/drivers/usbb/_asf_v1/enum/device             \
       avr32/drivers/usbb/_asf_v1/enum/host               \
       avr32/services/fs/fat                              \
       avr32/services/usb/_asf_v1                         \
       avr32/services/usb/_asf_v1/class/mass_storage/host_mem \
       avr32/services/usb/_asf_v1/class/mass_storage/scsi_decoder \
       avr32/utils                                        \
       avr32/utils/debug                                  \
       avr32/utils/preprocessor                           \
       common/boards                                      \
       common/components/memory/data_flash/at45dbx/_asf_v1 \
       common/services/clock                              \
       common/services/delay                              \
       common/services/storage/ctrl_access                \
       common/utils                                       \
       thirdparty/freertos/freertos-7.0.0/source/include  \
       thirdparty/freertos/freertos-7.0.0/source/portable/gcc/avr32_uc3 \
       thirdparty/lwip/lwip-1.4.0/src/include             \
       thirdparty/lwip/lwip-1.4.0/src/include/ipv4        \
       thirdparty/lwip/lwip-1.4.0/src/include/lwip        \
       thirdparty/lwip/lwip-port-1.4.0/at32uc3/include \
       avr32/applications/evk1100-control-panel/at32uc3a0512_evk1100/gcc

# Additional search paths for libraries.
LIB_PATH = 

# List of libraries to use during linking.
LIBS = 

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
       -D ACCESS_MEM_TO_RAM_ENABLED                       \
       -D AT45DBX_ENABLE                                  \
       -D BOARD=EVK1100                                   \
       -D DISPLAY_MMI_SECOND=1                            \
       -D FREERTOS_USED                                   \
       -D HTTP_USED=1                                     \
       -D MMILCD_ENABLE=1                                 \
       -D NW_INTEGRATED_IN_CONTROL_PANEL=1                \
       -D SD_MMC_SPI_ENABLE                               \
       -D SMTP_USED=0                                     \
       -D TFTP_USED=0                                     \
       -D USB_ENABLE=1                                    \
       -D __FREERTOS__

# Extra flags to use when linking
LDFLAGS = \
        -Wl,-e,_trampoline

# Pre- and post-build commands
PREBUILD_CMD = 
POSTBUILD_CMD = 