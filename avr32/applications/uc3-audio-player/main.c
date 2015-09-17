/**************************************************************************
 *
 * \file
 *
 * \brief Audio player main file
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 ***************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <stddef.h>
#include <stdio.h>

#include <avr32/io.h>
#include "compiler.h"
#include "board.h"
#include "intc.h"
#include "gpio.h"

#if (UC3L || UC3C || UC3A3)
#include "twim.h"
#else // use the standard TWI module on UC3A0x and UC3A1x
#include "twi.h"
#endif

#include "com_task.h"
#include "audio_interface.h"
#include "audio_mixer.h"
#include "clocks.h"

#include "conf_audio_mixer.h"
#include "conf_audio_interface.h"
#include "conf_audio_player.h"

#if defined(AUDIO_MIXER_DAC_ABDAC) && DEFAULT_DACS == AUDIO_MIXER_DAC_ABDAC
# include "tpa6130.h"
# include "conf_tpa6130.h"
#elif defined(AUDIO_MIXER_DAC_AIC23B) && DEFAULT_DACS == AUDIO_MIXER_DAC_AIC23B
# include "tlv320aic23b.h"
#endif
#if (defined USB_RESYNC_METHOD) && (USB_RESYNC_METHOD == USB_RESYNC_METHOD_EXT_CLOCK_SYNTHESIZER)
#include "cs2200.h"
#endif
#if defined(USE_AUDIO_PLAYER_BUFFERIZATION) && USE_AUDIO_PLAYER_BUFFERIZATION == true
#include "conf_buff_player.h"
#endif
#if defined(SUPPORT_SD_MMC) && SUPPORT_SD_MMC == true
  #if defined(SUPPORT_SD_MMC_MCI) && SUPPORT_SD_MMC_MCI == true
    #include "sd_mmc_mci.h"
    #include "conf_sd_mmc_mci.h"
  #else
    #include "spi.h"
    #include "sd_mmc_spi.h"
    #include "conf_sd_mmc_spi.h"
  #endif
#endif

#if (defined USB_RESYNC_METHOD) && (USB_RESYNC_METHOD == USB_RESYNC_METHOD_EXT_CLOCK_SYNTHESIZER)
//! CPU frequency
uint32_t g_fcpu_hz=0;

//! HSB frequency
uint32_t g_fhsb_hz=0;

//! PBA frequency
uint32_t g_fpba_hz=0;

//! PBB frequency
uint32_t g_fpbb_hz=0;
#endif

#if (defined USB_RESYNC_METHOD) && (USB_RESYNC_METHOD == USB_RESYNC_METHOD_EXT_CLOCK_SYNTHESIZER)
/*! \brief Initializes the two-wire interface using the internal RCOSC.
 */
static void init_twi_CS2200(uint32_t fpba_hz)
{
  static const gpio_map_t CS2200_TWI_GPIO_MAP =
  {
    {CS2200_TWI_SCL_PIN, CS2200_TWI_SCL_FUNCTION},
    {CS2200_TWI_SDA_PIN, CS2200_TWI_SDA_FUNCTION}
  };

  static twi_options_t CS2200_TWI_OPTIONS =
  {
    .speed  = CS2200_TWI_MASTER_SPEED,
    .chip   = CS2200_TWI_SLAVE_ADDRESS
  };
  CS2200_TWI_OPTIONS.pba_hz = fpba_hz;

  gpio_enable_module(CS2200_TWI_GPIO_MAP,
                     sizeof(CS2200_TWI_GPIO_MAP) / sizeof(CS2200_TWI_GPIO_MAP[0]));
  twi_master_init(CS2200_TWI, &CS2200_TWI_OPTIONS);
}
#endif

#if (defined __GNUC__) && (defined __AVR32__)

/*! \brief Replaces Newlib's implementation of the standard \c wcstombs function
 *         because of portability issues with \c wchar_t.
 *
 * \warning This function must be used with the `-fshort-wchar' code generation
 *          option.
 */
size_t wcstombs(char *s, const wchar_t *pwcs, size_t n)
{
  size_t count = 0;

  while (n--)
  {
    if ((*s++ = (char)*pwcs++) == '\0')
      break;
    count++;
  }

  return count;
}

#endif


/*! \brief Initializes MCU exceptions.
 */
static void init_exceptions(void)
{
#if (defined __GNUC__) && (defined __AVR32__)
  // Import the Exception Vector Base Address.
  extern void _evba;

  // Load the Exception Vector Base Address in the corresponding system
  // register.
  Set_system_register(AVR32_EVBA, (int)&_evba);
#endif

  // Enable exceptions globally.
  Enable_global_exception();
}


/*! \brief Initializes the HSB bus matrix
 */
static void init_hmatrix(void)
{
  // For the internal-flash HMATRIX slave, use last master as default.
  union
  {
    unsigned long                 scfg;
    avr32_hmatrix_scfg_t          SCFG;
  } u_avr32_hmatrix_scfg = {AVR32_HMATRIX.scfg[AVR32_HMATRIX_SLAVE_FLASH]};
  u_avr32_hmatrix_scfg.SCFG.defmstr_type = AVR32_HMATRIX_DEFMSTR_TYPE_LAST_DEFAULT;
  AVR32_HMATRIX.scfg[AVR32_HMATRIX_SLAVE_FLASH] = u_avr32_hmatrix_scfg.scfg;
}

/*! \brief Initializes the heap.
 */
static void init_heap(void)
{
#ifdef _DEBUG
  #if __GNUC__ && __AVR32__

  // Import the location of the heap.
  extern void __heap_start__;
  extern void __heap_end__;

  uint32_t *p_heap_word;

  // Initialize the heap.
  for (p_heap_word = &__heap_start__; (void *)p_heap_word < &__heap_end__; p_heap_word++)
    *p_heap_word = DEFAULT_HEAP_INIT_WORD;

  #elif __ICCAVR32__

  // Import the location of the heap.
  #pragma segment = "HEAP"

  uint32_t *p_heap_word;

  // Initialize the heap.
  for (p_heap_word = __segment_begin("HEAP"); (void *)p_heap_word < __segment_end("HEAP"); p_heap_word++)
    *p_heap_word = DEFAULT_HEAP_INIT_WORD;

  #endif
#endif
}

/*! \brief Initializes MCU interrupts.
 */
static void init_interrupts(void)
{
  // Initialize interrupt handling.
  irq_initialize_vectors();
  // Enable interrupts globally.
  cpu_irq_enable();
}


/*! \brief Low-level initialization routine called during startup, before the
 *         \ref main function.
 */
#if (defined __GNUC__) && (defined __AVR32__)
int _init_startup(void)
#elif (defined __ICCAVR32__)
int __low_level_init(void)
#endif
{
  init_exceptions();
  init_hmatrix();

  // EWAVR32: Request initialization of data segments.
  // GCC: Don't-care value.
  return 1;
}

/*! \brief Initializes the two-wire interface.
 */
static void init_twi(void)
{
#if defined(AUDIO_MIXER_DAC_ABDAC) && DEFAULT_DACS == AUDIO_MIXER_DAC_ABDAC
  const gpio_map_t TPA6130_TWI_GPIO_MAP =
  {
    {TPA6130_TWI_SCL_PIN, TPA6130_TWI_SCL_FUNCTION},
    {TPA6130_TWI_SDA_PIN, TPA6130_TWI_SDA_FUNCTION}
  };

  const twi_options_t TPA6130_TWI_OPTIONS =
  {
    .pba_hz = FPBA_HZ,
    .speed = TPA6130_TWI_MASTER_SPEED,
    .chip = TPA6130_TWI_ADDRESS
  };

  // Assign I/Os to TWI.
  gpio_enable_module(TPA6130_TWI_GPIO_MAP,
    sizeof(TPA6130_TWI_GPIO_MAP) / sizeof(TPA6130_TWI_GPIO_MAP[0]));

  // Initialize as master.
  twi_master_init(TPA6130_TWI, &TPA6130_TWI_OPTIONS);

#elif defined(AUDIO_MIXER_DAC_AIC23B) && DEFAULT_DACS == AUDIO_MIXER_DAC_AIC23B
  static const gpio_map_t AIC23B_TWI_GPIO_MAP =
  {
    {AIC23B_TWI_SCL_PIN, AIC23B_TWI_SCL_FUNCTION},
    {AIC23B_TWI_SDA_PIN, AIC23B_TWI_SDA_FUNCTION}
  };

  static twi_options_t AIC23B_TWI_OPTIONS =
  {
    .speed  = AIC23B_TWI_MASTER_SPEED,
    .chip   = AIC23B_TWI_ADDRESS
  };
  AIC23B_TWI_OPTIONS.pba_hz = FPBA_HZ;

  gpio_enable_module(AIC23B_TWI_GPIO_MAP,
                     sizeof(AIC23B_TWI_GPIO_MAP) / sizeof(AIC23B_TWI_GPIO_MAP[0]));
  twi_master_init(AIC23B_TWI, &AIC23B_TWI_OPTIONS);

#elif defined(AUDIO_MIXER_DAC_MS3) && DEFAULT_DACS == AUDIO_MIXER_DAC_MS3
    static const gpio_map_t TWI_GPIO_MAP =
    {
        {AVR32_TWI_SCL_0_0_PIN, AVR32_TWI_SCL_0_0_FUNCTION},
        {AVR32_TWI_SDA_0_0_PIN, AVR32_TWI_SDA_0_0_FUNCTION}
    };

    static const twi_options_t TWI_OPTIONS =
    {
        .pba_hz = FPBA_HZ,
        .speed  = 400000,
        .chip   = 0
    };

    gpio_enable_module(TWI_GPIO_MAP,
                     sizeof(TWI_GPIO_MAP) / sizeof(TWI_GPIO_MAP[0]));
    twi_master_init(&AVR32_TWI, &TWI_OPTIONS);
#endif // end defined(AUDIO_MIXER_DAC_AIC23B) && DEFAULT_DACS == AUDIO_MIXER_DAC_AIC23B
}

#if defined(SUPPORT_SD_MMC) && SUPPORT_SD_MMC == true

#if defined(SUPPORT_SD_MMC_MCI) && SUPPORT_SD_MMC_MCI == true

/*! \brief Initializes SD/MMC resources: GPIO, MCI and SD/MMC.
 */
static void sd_mmc_resources_init(void)
{
  static const gpio_map_t SD_MMC_MCI_GPIO_MAP =
  {
    {SD_SLOT_4BITS_CMD_PIN,   SD_SLOT_4BITS_CMD_FUNCTION },    // CMD Pin.
    {SD_SLOT_4BITS_CLK_PIN,   SD_SLOT_4BITS_CLK_FUNCTION},     // CLOCK Pin.
    {SD_SLOT_4BITS_DATA0_PIN, SD_SLOT_4BITS_DATA0_FUNCTION},   // DATA Pin.
    {SD_SLOT_4BITS_DATA1_PIN, SD_SLOT_4BITS_DATA1_FUNCTION},   // DATA Pin.
    {SD_SLOT_4BITS_DATA2_PIN, SD_SLOT_4BITS_DATA2_FUNCTION},   // DATA Pin.
    {SD_SLOT_4BITS_DATA3_PIN, SD_SLOT_4BITS_DATA3_FUNCTION}    // DATA Pin.
  };

  // Assign I/Os to SPI.
  gpio_enable_module(SD_MMC_MCI_GPIO_MAP,
                     sizeof(SD_MMC_MCI_GPIO_MAP) / sizeof(SD_MMC_MCI_GPIO_MAP[0]));

  // Initialize SD/MMC driver with MCI clock (PBB).
  sd_mmc_mci_init(SD_SLOT, FPBB_HZ, FCPU_HZ);
}

#else

static void sd_mmc_resources_init(void)
{
  static const gpio_map_t SD_MMC_SPI_GPIO_MAP =
  {
    {SD_MMC_SPI_SCK_PIN,  SD_MMC_SPI_SCK_FUNCTION },  // SPI Clock.
    {SD_MMC_SPI_MISO_PIN, SD_MMC_SPI_MISO_FUNCTION},  // MISO.
    {SD_MMC_SPI_MOSI_PIN, SD_MMC_SPI_MOSI_FUNCTION},  // MOSI.
    {SD_MMC_SPI_NPCS_PIN, SD_MMC_SPI_NPCS_FUNCTION}   // Chip Select NPCS.
  };

  // SPI options.
  spi_options_t spiOptions =
  {
    .reg          = SD_MMC_SPI_NPCS,
    .baudrate     = SD_MMC_SPI_MASTER_SPEED,  // Defined in conf_sd_mmc_spi.h.
    .bits         = SD_MMC_SPI_BITS,          // Defined in conf_sd_mmc_spi.h.
    .spck_delay   = 0,
    .trans_delay  = 0,
    .stay_act     = 1,
    .spi_mode     = 0,
    .modfdis      = 1
  };

  // Assign I/Os to SPI.
  gpio_enable_module(SD_MMC_SPI_GPIO_MAP,
                     sizeof(SD_MMC_SPI_GPIO_MAP) / sizeof(SD_MMC_SPI_GPIO_MAP[0]));

  // If the SPI used by the SD/MMC is not enabled.
  if (!spi_is_enabled(SD_MMC_SPI))
  {
    // Initialize as master.
    spi_initMaster(SD_MMC_SPI, &spiOptions);

    // Set selection mode: variable_ps, pcs_decode, delay.
    spi_selectionMode(SD_MMC_SPI, 0, 0, 0);

    // Enable SPI.
    spi_enable(SD_MMC_SPI);
  }

  // Initialize SD/MMC with SPI PB clock.
  sd_mmc_spi_init(spiOptions, FPBA_HZ);
}

#endif

#endif

/*! \brief Main function of the audio player.
 * Refer to AVR32709: AVR32 UC3 Audio Decoder Over USB application note.
 */
int main(void)
{
#if (defined USB_RESYNC_METHOD) && (USB_RESYNC_METHOD == USB_RESYNC_METHOD_EXT_CLOCK_SYNTHESIZER)
  // Initialize the TWI using the internal RCOSC
  init_twi_CS2200(AVR32_PM_RCOSC_FREQUENCY);

  // Initialize the CS2200 and produce a default 11.2896 MHz frequency
  cs2200_setup(11289600, FOSC0);
#endif

  init_sys_clocks();
  init_heap();
  init_interrupts();

  // Configure standard I/O streams as unbuffered.
#if (defined __GNUC__) && (defined __AVR32__)
  setbuf(stdin, NULL);
#endif
  setbuf(stdout, NULL);

  // Init the I2C interface
  init_twi();

  // Init the buffering if needed
#if defined(USE_AUDIO_PLAYER_BUFFERIZATION) && USE_AUDIO_PLAYER_BUFFERIZATION == true
  BUFF_INIT();
#endif

  audio_mixer_enable_dacs(DEFAULT_DACS);
  audio_mixer_dacs_start(DEFAULT_DAC_SAMPLE_RATE_HZ,
                         DEFAULT_DAC_NUM_CHANNELS,
                         DEFAULT_DAC_BITS_PER_SAMPLE,
                         DEFAULT_DAC_SWAP_CHANNELS);

  task_init();
  ai_init();
  com_task_init();

  // SPI configuration for the SD/MMC support
#if defined(SUPPORT_SD_MMC) && SUPPORT_SD_MMC == true
  sd_mmc_resources_init();
#endif

  while (true)
  {
    ai_task();
    com_task();
    task();
  }
}
