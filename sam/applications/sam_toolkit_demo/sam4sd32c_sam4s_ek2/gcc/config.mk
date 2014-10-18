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
TARGET_FLASH = sam_toolkit_demo_flash.elf
TARGET_SRAM = sam_toolkit_demo_sram.elf

# List of C source files.
CSRCS = \
       common/components/memory/nand_flash/nand_flash_ebi/ftl_lib/nand_flash_mem.c \
       common/components/memory/nand_flash/nand_flash_ebi/nand_flash_ecc_sw.c \
       common/components/memory/nand_flash/nand_flash_ebi/nand_flash_model.c \
       common/components/memory/nand_flash/nand_flash_ebi/nand_flash_raw_smc.c \
       common/components/memory/nand_flash/nand_flash_ebi/nand_flash_spare_scheme.c \
       common/services/clock/sam4s/sysclk.c               \
       common/services/delay/sam/cycle_counter.c          \
       common/services/gfx/gfx_generic.c                  \
       common/services/gfx/gfx_ili93xx.c                  \
       common/services/gfx/gfx_text.c                     \
       common/services/gfx/sysfont.c                      \
       common/services/serial/usart_serial.c              \
       common/services/sleepmgr/sam/sleepmgr.c            \
       common/services/spi/sam_spi/spi_master.c           \
       common/services/storage/ctrl_access/ctrl_access.c  \
       common/services/storage/ecc_hamming/ecc-sw.c       \
       common/services/usb/class/msc/device/udi_msc.c     \
       common/services/usb/class/msc/device/udi_msc_desc.c \
       common/services/usb/udc/udc.c                      \
       common/services/wtk/win.c                          \
       common/services/wtk/wtk.c                          \
       common/services/wtk/wtk_basic_frame.c              \
       common/services/wtk/wtk_button.c                   \
       common/services/wtk/wtk_check_box.c                \
       common/services/wtk/wtk_frame.c                    \
       common/services/wtk/wtk_icon.c                     \
       common/services/wtk/wtk_label.c                    \
       common/services/wtk/wtk_plot.c                     \
       common/services/wtk/wtk_progress_bar.c             \
       common/services/wtk/wtk_radio_button.c             \
       common/services/wtk/wtk_slider.c                   \
       common/utils/interrupt/interrupt_sam_nvic.c        \
       common/utils/membag/membag.c                       \
       common/utils/stdio/read.c                          \
       common/utils/stdio/write.c                         \
       sam/applications/sam_toolkit_demo/demo.c           \
       sam/applications/sam_toolkit_demo/demo_draw_bmpfile.c \
       sam/applications/sam_toolkit_demo/demo_parameters.c \
       sam/applications/sam_toolkit_demo/demo_qtouch.c    \
       sam/applications/sam_toolkit_demo/rtouch_calibrate.c \
       sam/applications/sam_toolkit_demo/sam4sd32c_sam4s_ek2/demo_board.c \
       sam/applications/sam_toolkit_demo/widget_scr_audio.c \
       sam/applications/sam_toolkit_demo/widget_scr_fft.c \
       sam/applications/sam_toolkit_demo/widget_scr_info.c \
       sam/applications/sam_toolkit_demo/widget_scr_lpm.c \
       sam/applications/sam_toolkit_demo/widget_scr_lpm_backup.c \
       sam/applications/sam_toolkit_demo/widget_scr_lpm_sleep.c \
       sam/applications/sam_toolkit_demo/widget_scr_lpm_wait.c \
       sam/applications/sam_toolkit_demo/widget_scr_main.c \
       sam/applications/sam_toolkit_demo/widget_scr_qtouch.c \
       sam/applications/sam_toolkit_demo/widget_scr_settings.c \
       sam/applications/sam_toolkit_demo/widget_scr_settings_backlight.c \
       sam/applications/sam_toolkit_demo/widget_scr_settings_date.c \
       sam/applications/sam_toolkit_demo/widget_scr_settings_time.c \
       sam/boards/sam4s_ek2/led.c                         \
       sam/components/display/aat31xx/aat31xx.c           \
       sam/components/display/ili93xx/ili93xx.c           \
       sam/components/resistive_touch/ads7843/ads7843.c   \
       sam/drivers/adc/adc.c                              \
       sam/drivers/adc/adc_sam3u.c                        \
       sam/drivers/ebi/smc/smc.c                          \
       sam/drivers/efc/efc.c                              \
       sam/drivers/matrix/matrix.c                        \
       sam/drivers/pdc/pdc.c                              \
       sam/drivers/pio/pio.c                              \
       sam/drivers/pio/pio_handler.c                      \
       sam/drivers/pmc/pmc.c                              \
       sam/drivers/pmc/sleep.c                            \
       sam/drivers/rtc/rtc.c                              \
       sam/drivers/spi/spi.c                              \
       sam/drivers/supc/supc.c                            \
       sam/drivers/tc/tc.c                                \
       sam/drivers/uart/uart.c                            \
       sam/drivers/udp/udp_device.c                       \
       sam/drivers/usart/usart.c                          \
       sam/drivers/wdt/wdt.c                              \
       sam/services/flash_efc/flash_efc.c                 \
       sam/services/resistive_touch/rtouch.c              \
       sam/utils/cmsis/sam4s/source/templates/gcc/startup_sam4s.c \
       sam/utils/cmsis/sam4s/source/templates/system_sam4s.c \
       sam/utils/syscalls/gcc/syscalls.c                  \
       thirdparty/fatfs/fatfs-port-r0.09/diskio.c         \
       thirdparty/fatfs/fatfs-port-r0.09/sam/fattime_rtc.c \
       thirdparty/fatfs/fatfs-r0.09/src/ff.c              \
       thirdparty/fatfs/fatfs-r0.09/src/option/ccsbcs.c   \
       thirdparty/freertos/freertos-7.0.0/source/croutine.c \
       thirdparty/freertos/freertos-7.0.0/source/list.c   \
       thirdparty/freertos/freertos-7.0.0/source/portable/gcc/sam/port.c \
       thirdparty/freertos/freertos-7.0.0/source/portable/memmang/heap_3.c \
       thirdparty/freertos/freertos-7.0.0/source/queue.c  \
       thirdparty/freertos/freertos-7.0.0/source/tasks.c  \
       thirdparty/freertos/freertos-7.0.0/source/timers.c \
       thirdparty/qtouch/generic/sam/qtouch/common/BitBangSPI_Master.c \
       thirdparty/qtouch/qdebug/QDebug.c                  \
       thirdparty/qtouch/qdebug/QDebugTransport.c

# List of assembler source files.
ASSRCS = 

# List of include paths.
INC_PATH = \
       common/boards                                      \
       common/components/memory/nand_flash/nand_flash_ebi \
       common/components/memory/nand_flash/nand_flash_ebi/ftl_lib \
       common/services/clock                              \
       common/services/delay                              \
       common/services/gfx                                \
       common/services/gpio                               \
       common/services/ioport                             \
       common/services/serial                             \
       common/services/serial/sam_uart                    \
       common/services/sleepmgr                           \
       common/services/spi                                \
       common/services/spi/sam_spi                        \
       common/services/storage/ctrl_access                \
       common/services/storage/ecc_hamming                \
       common/services/usb                                \
       common/services/usb/class/msc                      \
       common/services/usb/class/msc/device               \
       common/services/usb/udc                            \
       common/services/wtk                                \
       common/utils                                       \
       common/utils/membag                                \
       common/utils/stdio/stdio_serial                    \
       sam/applications/sam_toolkit_demo                  \
       sam/applications/sam_toolkit_demo/sam4sd32c_sam4s_ek2 \
       sam/boards                                         \
       sam/boards/sam4s_ek2                               \
       sam/components/display/aat31xx                     \
       sam/components/display/ili93xx                     \
       sam/components/resistive_touch/ads7843             \
       sam/drivers/adc                                    \
       sam/drivers/ebi/smc                                \
       sam/drivers/efc                                    \
       sam/drivers/matrix                                 \
       sam/drivers/pdc                                    \
       sam/drivers/pdc/pdc_uart_example                   \
       sam/drivers/pio                                    \
       sam/drivers/pmc                                    \
       sam/drivers/rtc                                    \
       sam/drivers/spi                                    \
       sam/drivers/supc                                   \
       sam/drivers/tc                                     \
       sam/drivers/uart                                   \
       sam/drivers/udp                                    \
       sam/drivers/usart                                  \
       sam/drivers/wdt                                    \
       sam/services/flash_efc                             \
       sam/services/resistive_touch                       \
       sam/utils                                          \
       sam/utils/cmsis/sam4s/include                      \
       sam/utils/cmsis/sam4s/source/templates             \
       sam/utils/header_files                             \
       sam/utils/preprocessor                             \
       thirdparty/CMSIS/Include                           \
       thirdparty/CMSIS/Lib/GCC                           \
       thirdparty/fatfs/fatfs-port-r0.09/sam              \
       thirdparty/fatfs/fatfs-r0.09/src                   \
       thirdparty/freertos/freertos-7.0.0/source/include  \
       thirdparty/freertos/freertos-7.0.0/source/portable/gcc/sam \
       thirdparty/qtouch/generic/include                  \
       thirdparty/qtouch/generic/sam/qtouch/common        \
       thirdparty/qtouch/qdebug \
       sam/applications/sam_toolkit_demo/sam4sd32c_sam4s_ek2/gcc

# Additional search paths for libraries.
LIB_PATH =  \
       common/components/memory/nand_flash/nand_flash_ebi/ftl_lib/gcc \
       thirdparty/CMSIS/Lib/GCC                           \
       thirdparty/qtouch/generic/sam/qtouch/lib/gcc      

# List of libraries to use during linking.
LIBS =  \
       _nand_flash_cortexm4                               \
       arm_cortexM4l_math                                 \
       sam4s-32qt-k-8rs-gnu                               \
       m                                                 

# Path relative to top level directory pointing to a linker script.
LINKER_SCRIPT_FLASH = sam/utils/linker_scripts/sam4s/sam4sd32/gcc/flash.ld
LINKER_SCRIPT_SRAM  = sam/utils/linker_scripts/sam4s/sam4sd32/gcc/sram.ld

# Path relative to top level directory pointing to a linker script.
DEBUG_SCRIPT_FLASH = sam/boards/sam4s_ek2/debug_scripts/gcc/sam4s_ek2_flash.gdb
DEBUG_SCRIPT_SRAM  = sam/boards/sam4s_ek2/debug_scripts/gcc/sam4s_ek2_sram.gdb

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
       -D ACCESS_USB_ENABLED                              \
       -D ARM_MATH_CM4=true                               \
       -D BOARD=SAM4S_EK2                                 \
       -D CONF_GFX_ILI93XX=1                              \
       -D DEF_TOUCH_QDEBUG_ENABLE_AT=0                    \
       -D DEF_TOUCH_QDEBUG_ENABLE_QM=0                    \
       -D DEF_TOUCH_QDEBUG_ENABLE_QTA=0                   \
       -D DEF_TOUCH_QDEBUG_ENABLE_QTB=0                   \
       -D QTOUCH_SLIDER_REVERSED                          \
       -D QT_DELAY_CYCLES=5                               \
       -D QT_NUM_CHANNELS=32                              \
       -D SNS1=C                                          \
       -D SNS2=A                                          \
       -D SNSK1=C                                         \
       -D SNSK2=A                                         \
       -D UDD_ENABLE                                      \
       -D _DEBUG_INTERFACE_                               \
       -D _QTOUCH_                                        \
       -D _SNS1_SNSK1_SAME_PORT_                          \
       -D _SNS2_SNSK2_SAME_PORT_                          \
       -D __FREERTOS__                                    \
       -D __SAM4SD32C__                                   \
       -D printf=iprintf                                  \
       -D scanf=iscanf

# Extra flags to use when linking
LDFLAGS = \

# Pre- and post-build commands
PREBUILD_CMD = 
POSTBUILD_CMD = 