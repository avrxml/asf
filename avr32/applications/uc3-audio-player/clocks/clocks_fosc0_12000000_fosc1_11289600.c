/*****************************************************************************
 *
 * \file
 *
 * \brief Clock configuration file with a 12MHz crystal on OSC0 and 11.2896MHz
 * crystal on OSC1.
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
 ******************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <avr32/io.h>
#include "board.h"
#include "clocks.h"
#include "conf_audio_player.h"
#include "conf_audio_mixer.h"

#include "pm.h"
#include "flashc.h"
#include "gpio.h"

#if defined(AUDIO_MIXER_DAC_ABDAC) && DEFAULT_DACS == AUDIO_MIXER_DAC_ABDAC
  #include "abdac.h"
  #include "tpa6130.h"
  #include "conf_tpa6130.h"
#elif defined(AUDIO_MIXER_DAC_AIC23B) && DEFAULT_DACS == AUDIO_MIXER_DAC_AIC23B
  #include "tlv320aic23b.h"
  #include "conf_tlv320aic23b.h"
#endif

// Check clock configuration

#if FOSC0 != 12000000 || FOSC1 != 11289600
  #error FOSC0 must be equal to 12MHz and FOSC1 to 11.2896MHz
#endif

#if defined(AUDIO_MIXER_DAC_AIC23B) && DEFAULT_DACS == AUDIO_MIXER_DAC_AIC23B
  #if AIC23B_MCLK_HZ != 11289600 && AIC23B_MCLK_HZ != 12000000
    #error AIC23B_MCLK_HZ must be equal to 11.2896MHz or to 12MHz
  #endif
#endif

#if FCPU_HZ != 62092800 || \
    FHSB_HZ != 62092800 || \
    FPBB_HZ != 62092800 || \
    FPBA_HZ != 62092800
  #error FCPU_HZ, FHSB_HZ, FPBB_HZ and FPBA_HZ must be equal to 62.0928MHz
#endif

static void init_usb_clock(void);
static void init_codec_gclk(void);
       void set_gclk2_freq(int freq_hz);

/*! \brief Initializes the MCU system clocks.
 */
void init_sys_clocks(void)
{
  // Switch to OSC0 to speed up the booting
  pm_switch_to_osc0(&AVR32_PM, FOSC0, OSC0_STARTUP);

  // Start oscillator1
  pm_enable_osc1_crystal(&AVR32_PM, FOSC1);
  //
  pm_enable_clk1(&AVR32_PM, OSC1_STARTUP);

  // Set PLL0 (fed from OSC1 = 11.2896 MHz) to 124.1856 MHz
  // We use OSC1 since we need a correct master clock for the SSC module to generate
  // the correct sample rate
  pm_pll_setup(&AVR32_PM, 0,  // pll.
    10,  // mul.
    1,   // div.
    1,   // osc.
    16); // lockcount.

  // Set PLL operating range and divider (fpll = fvco/2)
  // -> PLL0 output = 62.0928 MHz
  pm_pll_set_option(&AVR32_PM, 0, // pll.
    1,  // pll_freq.
    1,  // pll_div2.
    0); // pll_wbwdisable.

  // start PLL0 and wait for the lock
  pm_pll_enable(&AVR32_PM, 0);
  pm_wait_for_pll0_locked(&AVR32_PM);
  // Set all peripheral clocks to run at master clock rate
  pm_cksel(&AVR32_PM,
    0,   // pbadiv.
    0,   // pbasel.
    0,   // pbbdiv.
    0,   // pbbsel.
    0,   // hsbdiv.
    0);  // hsbsel.

  // Set one waitstate for the flash
  flashc_set_wait_state(1);

  // Switch to PLL0 as the master clock
  pm_switch_to_clock(&AVR32_PM, AVR32_PM_MCCTRL_MCSEL_PLL0);

  init_usb_clock();
  init_codec_gclk();
}

/*! \brief Initializes the USB clock.
 */
static void init_usb_clock(void)
{
  pm_configure_usb_clock();
}



#if defined(AUDIO_MIXER_DAC_ABDAC) && DEFAULT_DACS == AUDIO_MIXER_DAC_ABDAC

static void init_codec_gclk(void)
{
  // Configure the ABDAC generic clock
  // We do not activate it here since it is done in the low level DRIVERS/ABDAC/abdac.c driver.
  pm_gc_setup(&AVR32_PM, AVR32_PM_GCLK_ABDAC,
    AVR32_GC_USES_OSC, AVR32_GC_USES_OSC1, 0, 0);
}

void set_gclk2_freq(int freq_hz)
{
  if (!abdac_set_dac_sample_rate(freq_hz))
    abdac_set_dac_hz(TPA6130_ABDAC, FOSC1, freq_hz);
}

#elif defined(AUDIO_MIXER_DAC_AIC23B) && DEFAULT_DACS == AUDIO_MIXER_DAC_AIC23B

  #if defined(AIC23B_DAC_USE_RX_CLOCK) && AIC23B_DAC_USE_RX_CLOCK == true
void set_gclk2_freq(int freq_hz)
{
  const int gc_master_clock = 0;
  const int gc_tx_clock = 2;
  int temp;

  pm_gc_disable(&AVR32_PM, gc_master_clock);
  pm_gc_disable(&AVR32_PM, gc_tx_clock);
  switch (freq_hz)
  {
  // 32000 - stereo - 16 bits -> PLL0 (62.092MHz)
  case (32000*2*16):
    // Codec master clock - 12MHz
    pm_gc_setup(&AVR32_PM, gc_master_clock, AVR32_GC_USES_OSC, AVR32_GC_USES_OSC0, 0, 0);
    // I2S TX clock - 48MHz/46 ~ 1.024MHz = freq_hz
    pm_gc_setup(&AVR32_PM, gc_tx_clock, AVR32_GC_USES_PLL, AVR32_GC_USES_PLL1, 1, 22);
    // Configure the DAC
    aic23b_configure_freq(FOSC0, 32000);
    break;
  // 44100 - stereo - 16 bits -> OSC1 (11.289600MHz)
  case (44100*2*16):
    // Codec master clock - 11.289600MHz
    pm_gc_setup(&AVR32_PM, gc_master_clock, AVR32_GC_USES_OSC, AVR32_GC_USES_OSC1, 0, 0);
    // I2S TX clock - 11.289600MHz/8 = 1.4112MHz = freq_hz
    pm_gc_setup(&AVR32_PM, gc_tx_clock, AVR32_GC_USES_OSC, AVR32_GC_USES_OSC1, 1, 3);
    // Configure the DAC
    aic23b_configure_freq(FOSC1, 44100);
    break;
  // 48000 - stereo - 16 bits -> PLL1 (48MHz)
  case (48000*2*16):
    // Codec master clock - 12MHz
    pm_gc_setup(&AVR32_PM, gc_master_clock, AVR32_GC_USES_OSC, AVR32_GC_USES_OSC0, 0, 0);
    // I2S TX clock - 48MHz/32 ~ 1.536MHz = freq_hz
    pm_gc_setup(&AVR32_PM, gc_tx_clock, AVR32_GC_USES_PLL, AVR32_GC_USES_PLL1, 1, 15);
    // Configure the DAC
    aic23b_configure_freq(FOSC0, 48000);
    break;
  default:
    // not supported! (but we try anyway! to avoid locked-up conditions)
    // Codec master clock - 12MHz
    pm_gc_setup(&AVR32_PM, gc_master_clock, AVR32_GC_USES_OSC, AVR32_GC_USES_OSC0, 0, 0);
    // I2S TX clock
    temp = (FOSC0 + freq_hz) / (2*freq_hz) - 1;
    pm_gc_setup(&AVR32_PM, gc_tx_clock, AVR32_GC_USES_OSC, AVR32_GC_USES_OSC0, 1, temp);
    // Configure the DAC
    aic23b_configure_freq(FOSC0, freq_hz / (2*16));
  }
  pm_gc_enable(&AVR32_PM, gc_tx_clock);
  pm_gc_enable(&AVR32_PM, gc_master_clock);
}
  #else

void set_gclk2_freq(int freq_hz)
{
}

  #endif

/*! \brief Sets up generic clock for the audio codec.
 */
static void init_codec_gclk(void)
{
  #if defined(AIC23B_DAC_USE_RX_CLOCK) && AIC23B_DAC_USE_RX_CLOCK == true
  // GCLK to supply the I2S TX clock
  gpio_enable_module_pin(TLV320_PM_GCLK_RX_PIN, TLV320_PM_GCLK_RX_FUNCTION);
  gpio_enable_module_pin(TLV320_PM_GCLK_PIN, TLV320_PM_GCLK_FUNCTION);
  #else
  // Use PBA for the I2S clock
  const int gc = 0;
  gpio_enable_module_pin(TLV320_PM_GCLK_PIN, TLV320_PM_GCLK_FUNCTION);
    #if AIC23B_MCLK_HZ == 11289600
  pm_gc_setup(&AVR32_PM, gc, AVR32_GC_USES_OSC, AVR32_GC_USES_OSC1, 0, 0);
    #elif AIC23B_MCLK_HZ == 12000000
  pm_gc_setup(&AVR32_PM, gc, AVR32_GC_USES_OSC, AVR32_GC_USES_OSC0, 0, 0);
    #endif
  pm_gc_enable(&AVR32_PM, gc);
  #endif
}

#else

static void init_codec_gclk(void)
{
}

void set_gclk2_freq(int freq_hz)
{
}

#endif

