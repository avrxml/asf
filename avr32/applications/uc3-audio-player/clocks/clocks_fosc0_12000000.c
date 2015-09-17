/*****************************************************************************
 *
 * \file
 *
 * \brief Clock configuration file with a 12MHz crystal on OSC0. (NC on OSC1s)
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

#if DEFAULT_DACS == AUDIO_MIXER_DAC_ABDAC
  #include "abdac.h"
  #include "tpa6130.h"
  #include "conf_tpa6130.h"
#elif DEFAULT_DACS == AUDIO_MIXER_DAC_AIC23B
  #include "tlv320aic23b.h"
  #include "conf_tlv320aic23b.h"
#endif

// Check clock configuration

#if FOSC0 != 12000000
  #error FOSC0 must be equal to 12MHz
#endif

#if DEFAULT_DACS == AUDIO_MIXER_DAC_AIC23B
  #if AIC23B_MCLK_HZ != 12000000
    #error AIC23B_MCLK_HZ must be equal to 12MHz
  #endif
  #if defined(AIC23B_DAC_USE_RX_CLOCK) && AIC23B_DAC_USE_RX_CLOCK == ENABLED
    #error AIC23B_DAC_USE_RX_CLOCK must be disabled.
  #endif
#endif

#if FCPU_HZ != 66000000 || \
    FHSB_HZ != 66000000 || \
    FPBB_HZ != 66000000 || \
    FPBA_HZ != 66000000
  #error FCPU_HZ, FHSB_HZ, FPBB_HZ and FPBA_HZ must be equal to 66MHz
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

  // Set PLL0 (fed from OSC0 = 12 MHz) to 132 MHz
  // We use OSC0 since we need a correct master clock for the SSC module to generate
  // the correct sample rate
  pm_pll_setup(&AVR32_PM, 0,  // pll.
    10,  // mul.
    1,   // div.
    0,   // osc.
    16); // lockcount.

  // Set PLL operating range and divider (fpll = fvco/2)
  // -> PLL0 output = 66 MHz
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

#if DEFAULT_DACS == AUDIO_MIXER_DAC_ABDAC

static void init_codec_gclk(void)
{
  // Configure the ABDAC generic clock
  // We do not activate it here since it is done in the low level DRIVERS/ABDAC/abdac.c driver.
  pm_gc_setup(&AVR32_PM, AVR32_PM_GCLK_ABDAC,
    AVR32_GC_USES_OSC, AVR32_GC_USES_OSC0, 0, 0);
}

void set_gclk2_freq(int freq_hz)
{
  if (!abdac_set_dac_sample_rate(freq_hz))
    abdac_set_dac_hz(TPA6130_ABDAC, FOSC1, freq_hz);
}

#elif DEFAULT_DACS == AUDIO_MIXER_DAC_AIC23B

/*! \brief Sets up generic clock for the audio codec.
 */
static void init_codec_gclk(void)
{
 // Use PBA for the I2S clock
  const int gc = 0;
  gpio_enable_module_pin(TLV320_PM_GCLK_PIN, TLV320_PM_GCLK_FUNCTION);
  pm_gc_setup(&AVR32_PM, gc, AVR32_GC_USES_OSC, AVR32_GC_USES_OSC0, 0, 0);
  pm_gc_enable(&AVR32_PM, gc);
}

void set_gclk2_freq(int freq_hz)
{
}

#else

static void init_codec_gclk(void)
{
}

void set_gclk2_freq(int freq_hz)
{
}

#endif
