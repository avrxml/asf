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
PRJ_PATH = ../../../../..

# Microcontroller: atxmega128a1, atmega128, attiny261, etc.
MCU = atxmega384c3

# Application target name. Given with suffix .a for library and .elf for a
# standalone application.
TARGET = xmega_c3_xplained_demo.elf

# C source files located from the top-level source directory
CSRCS = \
       common/components/display/ssd1306/font.c           \
       common/components/display/ssd1306/ssd1306.c        \
       common/components/memory/sd_mmc/sd_mmc.c           \
       common/components/memory/sd_mmc/sd_mmc_mem.c       \
       common/components/memory/sd_mmc/sd_mmc_spi.c       \
       common/services/calendar/calendar.c                \
       common/services/clock/xmega/sysclk.c               \
       common/services/fifo/fifo.c                        \
       common/services/gfx_mono/gfx_mono_framebuffer.c    \
       common/services/gfx_mono/gfx_mono_generic.c        \
       common/services/gfx_mono/gfx_mono_text.c           \
       common/services/gfx_mono/gfx_mono_ug_2832hsweg04.c \
       common/services/gfx_mono/sysfont.c                 \
       common/services/ioport/xmega/ioport_compat.c       \
       common/services/sleepmgr/xmega/sleepmgr.c          \
       common/services/spi/xmega_usart_spi/usart_spi.c    \
       common/services/storage/ctrl_access/ctrl_access.c  \
       common/services/usb/class/cdc/device/udi_cdc.c     \
       common/services/usb/class/composite/device/udi_composite_desc.c \
       common/services/usb/class/msc/device/udi_msc.c     \
       common/services/usb/udc/udc.c                      \
       common/utils/stdio/read.c                          \
       common/utils/stdio/stdio_usb/stdio_usb.c           \
       common/utils/stdio/write.c                         \
       thirdparty/fatfs/fatfs-port-r0.09/diskio.c         \
       thirdparty/fatfs/fatfs-port-r0.09/xmega/fattime.c  \
       thirdparty/fatfs/fatfs-r0.09/src/ff.c              \
       thirdparty/fatfs/fatfs-r0.09/src/option/ccsbcs.c   \
       xmega/applications/xmega_c3_xplained_demo/app_bitmaps.c \
       xmega/applications/xmega_c3_xplained_demo/app_cpu_load.c \
       xmega/applications/xmega_c3_xplained_demo/app_microsd.c \
       xmega/applications/xmega_c3_xplained_demo/app_sampling.c \
       xmega/applications/xmega_c3_xplained_demo/app_touch.c \
       xmega/applications/xmega_c3_xplained_demo/app_usb.c \
       xmega/applications/xmega_c3_xplained_demo/main.c   \
       xmega/boards/xmega_c3_xplained/init.c              \
       xmega/drivers/adc/adc.c                            \
       xmega/drivers/adc/xmega_bcd/adc_bcd.c              \
       xmega/drivers/nvm/nvm.c                            \
       xmega/drivers/rtc/rtc.c                            \
       xmega/drivers/tc/tc.c                              \
       xmega/drivers/usart/usart.c                        \
       xmega/drivers/usb/usb_device.c

# Assembler source files located from the top-level source directory
ASSRCS = \
       thirdparty/qtouch/generic/avr8/qtouch/common/qt_asm_xmega.S \
       xmega/drivers/cpu/ccp.s                            \
       xmega/drivers/nvm/nvm_asm.s

# Include path located from the top-level source directory
INC_PATH = \
       common/boards                                      \
       common/components/display/ssd1306                  \
       common/components/memory/sd_mmc                    \
       common/services/calendar                           \
       common/services/clock                              \
       common/services/delay                              \
       common/services/fifo                               \
       common/services/gfx_mono                           \
       common/services/gpio                               \
       common/services/ioport                             \
       common/services/sleepmgr                           \
       common/services/spi                                \
       common/services/storage/ctrl_access                \
       common/services/usb                                \
       common/services/usb/class/cdc                      \
       common/services/usb/class/cdc/device               \
       common/services/usb/class/composite/device         \
       common/services/usb/class/msc                      \
       common/services/usb/class/msc/device               \
       common/services/usb/udc                            \
       common/utils                                       \
       common/utils/stdio/stdio_usb                       \
       thirdparty/fatfs/fatfs-port-r0.09/xmega            \
       thirdparty/fatfs/fatfs-r0.09/src                   \
       thirdparty/qtouch/generic/avr8/qtouch/common       \
       thirdparty/qtouch/generic/include                  \
       xmega/applications/xmega_c3_xplained_demo          \
       xmega/applications/xmega_c3_xplained_demo/atxmega384c3_xmega_c3_xplained \
       xmega/boards                                       \
       xmega/boards/xmega_c3_xplained                     \
       xmega/drivers/adc                                  \
       xmega/drivers/cpu                                  \
       xmega/drivers/nvm                                  \
       xmega/drivers/pmic                                 \
       xmega/drivers/rtc                                  \
       xmega/drivers/sleep                                \
       xmega/drivers/tc                                   \
       xmega/drivers/usart                                \
       xmega/drivers/usb                                  \
       xmega/utils                                        \
       xmega/utils/preprocessor \
       xmega/applications/xmega_c3_xplained_demo/atxmega384c3_xmega_c3_xplained/gcc

# Library paths from the top-level source directory
LIB_PATH =  \
       thirdparty/qtouch/generic/avr8/qtouch/lib/gcc     

# Libraries to link with the project
LIBS =  \
       avrxmega6g1-4qt-k-0rs                             

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
       -D ACCESS_USB_ENABLED                              \
       -D BOARD=XMEGA_C3_XPLAINED                         \
       -D GFX_MONO_UG_2832HSWEG04=1                       \
       -D IOPORT_XMEGA_COMPAT                             \
       -D ISR_CUSTOM_H=\"app_cpu_load.h\"                 \
       -D QTOUCH_STUDIO_MASKS=1                           \
       -D QT_DELAY_CYCLES=1                               \
       -D QT_NUM_CHANNELS=4                               \
       -D SD_MMC_ENABLE                                   \
       -D SNS1=F                                          \
       -D SNSK1=F                                         \
       -D _QTOUCH_                                        \
       -D _SNS1_SNSK1_SAME_PORT_

# Extra flags to use when linking
LDFLAGS =  \
       -Wl,--section-start=.BOOT=0x60000                 

# Pre- and post-build commands
PREBUILD_CMD = 
POSTBUILD_CMD = 