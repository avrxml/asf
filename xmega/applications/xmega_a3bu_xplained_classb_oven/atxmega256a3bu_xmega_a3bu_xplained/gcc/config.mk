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
MCU = atxmega256a3bu

# Application target name. Given with suffix .a for library and .elf for a
# standalone application.
TARGET = xmega_a3bu_xplained_classb.elf

# C source files located from the top-level source directory
CSRCS = \
       common/components/display/st7565r/st7565r.c        \
       common/services/clock/xmega/sysclk.c               \
       common/services/gfx_mono/gfx_mono_c12832_a1z.c     \
       common/services/gfx_mono/gfx_mono_framebuffer.c    \
       common/services/gfx_mono/gfx_mono_generic.c        \
       common/services/gfx_mono/gfx_mono_menu.c           \
       common/services/gfx_mono/gfx_mono_spinctrl.c       \
       common/services/gfx_mono/gfx_mono_text.c           \
       common/services/gfx_mono/sysfont.c                 \
       common/services/ioport/xmega/ioport_compat.c       \
       common/services/sleepmgr/xmega/sleepmgr.c          \
       common/services/spi/xmega_usart_spi/usart_spi.c    \
       xmega/applications/xmega_a3bu_xplained_classb_oven/classb_tests_cert/analog/classb_analog.c \
       xmega/applications/xmega_a3bu_xplained_classb_oven/classb_tests_cert/classb_rtc_common.c \
       xmega/applications/xmega_a3bu_xplained_classb_oven/classb_tests_cert/crc/classb_crc_hw.c \
       xmega/applications/xmega_a3bu_xplained_classb_oven/classb_tests_cert/crc/classb_crc_sw.c \
       xmega/applications/xmega_a3bu_xplained_classb_oven/classb_tests_cert/freq/classb_freq.c \
       xmega/applications/xmega_a3bu_xplained_classb_oven/classb_tests_cert/interrupts/classb_interrupt_monitor.c \
       xmega/applications/xmega_a3bu_xplained_classb_oven/classb_tests_cert/registers/classb_cpu_gcc.c \
       xmega/applications/xmega_a3bu_xplained_classb_oven/classb_tests_cert/sram/classb_sram.c \
       xmega/applications/xmega_a3bu_xplained_classb_oven/classb_tests_cert/wdt/classb_wdt_test.c \
       xmega/applications/xmega_a3bu_xplained_classb_oven/drv/keyboard.c \
       xmega/applications/xmega_a3bu_xplained_classb_oven/drv/touch.c \
       xmega/applications/xmega_a3bu_xplained_classb_oven/main.c \
       xmega/applications/xmega_a3bu_xplained_classb_oven/oven_bitmaps.c \
       xmega/applications/xmega_a3bu_xplained_classb_oven/oven_classb.c \
       xmega/applications/xmega_a3bu_xplained_classb_oven/oven_controller.c \
       xmega/applications/xmega_a3bu_xplained_classb_oven/oven_plant.c \
       xmega/applications/xmega_a3bu_xplained_classb_oven/oven_ui.c \
       xmega/boards/xmega_a3bu_xplained/init.c            \
       xmega/drivers/adc/adc.c                            \
       xmega/drivers/adc/xmega_aau/adc_aau.c              \
       xmega/drivers/dac/dac.c                            \
       xmega/drivers/nvm/nvm.c                            \
       xmega/drivers/rtc32/rtc32.c                        \
       xmega/drivers/tc/tc.c                              \
       xmega/drivers/usart/usart.c                        \
       xmega/drivers/wdt/wdt.c

# Assembler source files located from the top-level source directory
ASSRCS = \
       thirdparty/qtouch/generic/avr8/qtouch/common/qt_asm_xmega.S \
       xmega/drivers/cpu/ccp.s                            \
       xmega/drivers/nvm/nvm_asm.s

# Include path located from the top-level source directory
INC_PATH = \
       common/boards                                      \
       common/components/display/st7565r                  \
       common/services/clock                              \
       common/services/delay                              \
       common/services/gfx_mono                           \
       common/services/gpio                               \
       common/services/ioport                             \
       common/services/sleepmgr                           \
       common/services/spi                                \
       common/utils                                       \
       thirdparty/qtouch/generic/avr8/qtouch/common       \
       thirdparty/qtouch/generic/include                  \
       xmega/applications/xmega_a3bu_xplained_classb_oven \
       xmega/applications/xmega_a3bu_xplained_classb_oven/atxmega256a3bu_xmega_a3bu_xplained \
       xmega/applications/xmega_a3bu_xplained_classb_oven/classb_tests_cert \
       xmega/applications/xmega_a3bu_xplained_classb_oven/classb_tests_cert/analog \
       xmega/applications/xmega_a3bu_xplained_classb_oven/classb_tests_cert/crc \
       xmega/applications/xmega_a3bu_xplained_classb_oven/classb_tests_cert/freq \
       xmega/applications/xmega_a3bu_xplained_classb_oven/classb_tests_cert/interrupts \
       xmega/applications/xmega_a3bu_xplained_classb_oven/classb_tests_cert/registers \
       xmega/applications/xmega_a3bu_xplained_classb_oven/classb_tests_cert/sram \
       xmega/applications/xmega_a3bu_xplained_classb_oven/classb_tests_cert/wdt \
       xmega/applications/xmega_a3bu_xplained_classb_oven/drv \
       xmega/boards                                       \
       xmega/boards/xmega_a3bu_xplained                   \
       xmega/drivers/adc                                  \
       xmega/drivers/cpu                                  \
       xmega/drivers/dac                                  \
       xmega/drivers/nvm                                  \
       xmega/drivers/pmic                                 \
       xmega/drivers/rtc32                                \
       xmega/drivers/sleep                                \
       xmega/drivers/tc                                   \
       xmega/drivers/usart                                \
       xmega/drivers/wdt                                  \
       xmega/utils                                        \
       xmega/utils/preprocessor \
       xmega/applications/xmega_a3bu_xplained_classb_oven/atxmega256a3bu_xmega_a3bu_xplained/gcc

# Library paths from the top-level source directory
LIB_PATH =  \
       thirdparty/qtouch/generic/avr8/qtouch/lib/gcc     

# Libraries to link with the project
LIBS =  \
       avrxmega6g1-4qt-k-0rs                              \
       m                                                 

# Additional options for debugging. By default the common Makefile.in will
# add -gdwarf-2.
DBGFLAGS = 

# Optimization settings
OPTIMIZATION = -O2

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
CFLAGS =  \
       -fshort-enums                                     

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
       -D CLASSB_FREQ_TEST                                \
       -D CLASSB_INT_MON                                  \
       -D CONFIG_NVM_IGNORE_XMEGA_A3_D3_REVB_ERRATA       \
       -D F_CPU=24000000UL                                \
       -D GFX_MONO_C12832_A1Z=1                           \
       -D IOPORT_XMEGA_COMPAT                             \
       -D NUMBER_OF_PORTS=1                               \
       -D QTOUCH_STUDIO_MASKS=1                           \
       -D QT_DELAY_CYCLES=1                               \
       -D QT_NUM_CHANNELS=4                               \
       -D SNS1=F                                          \
       -D SNSK1=F                                         \
       -D _QTOUCH_                                        \
       -D _SNS1_SNSK1_SAME_PORT_

# Extra flags to use when linking
LDFLAGS =  \
       -Wl,--section-start=.BOOT=0x40000                  \
       -Wl,--section-start=.classb_sram_buffer=0x802000   \
       -Wl,--section-start=.data=0x802500                

# Pre- and post-build commands
PREBUILD_CMD = 
POSTBUILD_CMD = 