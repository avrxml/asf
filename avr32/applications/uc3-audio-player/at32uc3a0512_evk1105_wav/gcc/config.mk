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
TARGET = uc3-audio-player_common_base.elf

# List of C source files.
CSRCS = \
       avr32/applications/uc3-audio-player/clocks/clocks_fosc0_12000000_fosc1_11289600.c \
       avr32/applications/uc3-audio-player/codec_wav_task.c \
       avr32/applications/uc3-audio-player/com_task.c     \
       avr32/applications/uc3-audio-player/main.c         \
       avr32/applications/uc3-audio-player/nav_automatic.c \
       avr32/applications/uc3-audio-player/usb_host_enum.c \
       avr32/applications/uc3-audio-player/usb_host_task.c \
       avr32/applications/uc3-audio-player/usb_task.c     \
       avr32/applications/uc3-audio-player/user_interface/controller/joystick_controller.c \
       avr32/applications/uc3-audio-player/user_interface/gui/et024006dhu_gui.c \
       avr32/applications/uc3-audio-player/user_interface/gui/sdram_loader.c \
       avr32/boards/evk1105/led.c                         \
       avr32/components/audio/amp/tpa6130a2/tpa6130.c     \
       avr32/components/audio/codec/tlv320aic23b/tlv320aic23b.c \
       avr32/components/audio/dac/pwm_dac/pwm_dac.c       \
       avr32/components/display/et024006dhu/et024006dhu.c \
       avr32/components/memory/sd_mmc/sd_mmc_spi/sd_mmc_spi.c \
       avr32/components/memory/sd_mmc/sd_mmc_spi/sd_mmc_spi_mem.c \
       avr32/drivers/abdac/abdac.c                        \
       avr32/drivers/ebi/sdramc/sdramc.c                  \
       avr32/drivers/ebi/smc/smc.c                        \
       avr32/drivers/flashc/flashc.c                      \
       avr32/drivers/gpio/gpio.c                          \
       avr32/drivers/intc/intc.c                          \
       avr32/drivers/pdca/pdca.c                          \
       avr32/drivers/pm/pm.c                              \
       avr32/drivers/pm/pm_conf_clocks.c                  \
       avr32/drivers/pm/power_clocks_lib.c                \
       avr32/drivers/spi/spi.c                            \
       avr32/drivers/ssc/i2s/ssc_i2s.c                    \
       avr32/drivers/twi/twi.c                            \
       avr32/drivers/usbb/_asf_v1/usb_drv.c               \
       avr32/drivers/wdt/wdt.c                            \
       avr32/services/audio/audio_mixer/audio_mixer.c     \
       avr32/services/audio/audio_player/ai_sd_mmc/ai_sd_mmc.c \
       avr32/services/audio/audio_player/ai_usb_ms/ai_usb_ms.c \
       avr32/services/audio/audio_player/ai_usb_ms/ai_usb_ms_wav_support.c \
       avr32/services/audio/audio_player/ai_usb_ms/host_mass_storage_task.c \
       avr32/services/audio/audio_player/audio_interface.c \
       avr32/services/audio/audio_player/players/buff_player.c \
       avr32/services/audio/audio_player/players/wav/wav_player.c \
       avr32/services/audio/wav/wavdecode.c               \
       avr32/services/fs/fat/fat.c                        \
       avr32/services/fs/fat/fat_unusual.c                \
       avr32/services/fs/fat/file.c                       \
       avr32/services/fs/fat/nav_filterlist.c             \
       avr32/services/fs/fat/navigation.c                 \
       avr32/services/fs/fat/play_list.c                  \
       avr32/services/fs/fat/reader_txt.c                 \
       avr32/services/fs/fat/unicode.c                    \
       avr32/services/usb/_asf_v1/class/mass_storage/host_mem/host_mem.c \
       avr32/services/usb/_asf_v1/class/mass_storage/scsi_decoder/scsi_decoder.c \
       common/components/memory/data_flash/at45dbx/_asf_v1/at45dbx.c \
       common/components/memory/data_flash/at45dbx/_asf_v1/at45dbx_mem.c \
       common/services/storage/ctrl_access/ctrl_access.c  \
       thirdparty/dlmalloc/dlmalloc.c                     \
       thirdparty/dlmalloc/dlsbrk.c

# List of assembler source files.
ASSRCS = \
       avr32/drivers/intc/exception.S                     \
       avr32/utils/startup/trampoline_uc3.S

# List of include paths.
INC_PATH = \
       avr32/applications/uc3-audio-player                \
       avr32/applications/uc3-audio-player/at32uc3a0512_evk1105_wav \
       avr32/applications/uc3-audio-player/clocks         \
       avr32/applications/uc3-audio-player/conf           \
       avr32/applications/uc3-audio-player/user_interface/controller \
       avr32/applications/uc3-audio-player/user_interface/gui \
       avr32/boards                                       \
       avr32/boards/evk1105                               \
       avr32/components/audio                             \
       avr32/components/audio/amp/tpa6130a2               \
       avr32/components/audio/codec/tlv320aic23b          \
       avr32/components/audio/dac/pwm_dac                 \
       avr32/components/display/et024006dhu               \
       avr32/components/memory/sd_mmc/sd_mmc_spi          \
       avr32/components/memory/sdram                      \
       avr32/components/touch/qt1081                      \
       avr32/drivers/abdac                                \
       avr32/drivers/cpu/cycle_counter                    \
       avr32/drivers/ebi/sdramc                           \
       avr32/drivers/ebi/smc                              \
       avr32/drivers/flashc                               \
       avr32/drivers/gpio                                 \
       avr32/drivers/intc                                 \
       avr32/drivers/pdca                                 \
       avr32/drivers/pm                                   \
       avr32/drivers/spi                                  \
       avr32/drivers/ssc/i2s                              \
       avr32/drivers/twi                                  \
       avr32/drivers/usbb/_asf_v1                         \
       avr32/drivers/usbb/_asf_v1/enum                    \
       avr32/drivers/wdt                                  \
       avr32/services/audio/audio_mixer                   \
       avr32/services/audio/audio_player                  \
       avr32/services/audio/audio_player/ai_sd_mmc        \
       avr32/services/audio/audio_player/ai_usb_ms        \
       avr32/services/audio/audio_player/players          \
       avr32/services/audio/audio_player/players/wav      \
       avr32/services/audio/wav                           \
       avr32/services/fs/fat                              \
       avr32/services/usb/_asf_v1                         \
       avr32/services/usb/_asf_v1/class/mass_storage/host_mem \
       avr32/services/usb/_asf_v1/class/mass_storage/scsi_decoder \
       avr32/utils                                        \
       avr32/utils/preprocessor                           \
       common/boards                                      \
       common/components/memory/data_flash/at45dbx/_asf_v1 \
       common/services/storage/ctrl_access                \
       common/utils                                       \
       thirdparty/dlmalloc                                \
       thirdparty/newlib_addons/libs/include \
       avr32/applications/uc3-audio-player/at32uc3a0512_evk1105_wav/gcc

# Additional search paths for libraries.
LIB_PATH =  \
       thirdparty/newlib_addons/libs/at32ucr2            

# List of libraries to use during linking.
LIBS =  \
       newlib_addons-at32ucr2-speed_opt                   \
       m                                                 

# Path relative to top level directory pointing to a linker script.
LINKER_SCRIPT = avr32/applications/uc3-audio-player/linker_scripts/at32uc3a/0512/gcc/link_uc3a0512_sdram.lds

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
CFLAGS =  \
       -fshort-wchar                                     

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
       -D BOARD=EVK1105                                   \
       -D CONFIG_INTERRUPT_FORCE_INTC                     \
       -D SD_MMC_SPI_ENABLE

# Extra flags to use when linking
LDFLAGS = \
        -Wl,-e,_trampoline                                \
       -Wl,--defsym,__stack_size__=5K

# Pre- and post-build commands
PREBUILD_CMD = 
POSTBUILD_CMD = 