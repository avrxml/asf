/*****************************************************************************
 *
 * \file
 *
 * \brief TP6130 Audio amplifier Example.
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

/*! \mainpage
 * \section intro Introduction
 * This documents data structures, functions, variables, defines, enums, and
 * typedefs in the software for the TPA6130 driver.
 *
 * The given example transfers an input buffer of audio samples to an output
 * using the Audio Codec support.
 *
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC for AVR32 and IAR Systems compiler
 * for AVR32. Other compilers may or may not work.
 *
 * \section files Main Files
 * - tpa6130.c: TPA6130 driver.
 * - tpa6130.h: TPA6130 header file.
 * - conf_tpa6130.h: Configuration file for the TPA6130.
 * - tpa6130_example.c: application example.
 *
 * \section configinfo Configuration Information
 * This example has been tested with the following configuration:
 * - EVK1104 With OSC0=12Mhz and OSC1=11,2896Mhz
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/products/AVR32/">Atmel AVR32</A>.\n
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <avr32/io.h>
#include <stdio.h>
#include <string.h>
#include "compiler.h"
#include "preprocessor.h"

#include "print_funcs.h"

#include "flashc.h"
#include "pm.h"
#include "gpio.h"

#include "tpa6130.h"
#include "abdac.h"

#include "conf_tpa6130.h"
#include "board.h"
#include "audio.h"

#include "sound.h"

#if (UC3A3 || UC3C || UC3L)
#include "twim.h"
#else
#include "twi.h"
#endif

//! Sample Count Value
#define SOUND_SAMPLES                256
#define FPBA_HZ                 12000000
#define TPA6130_TWI_MASTER_SPEED  100000

void dac_reload_callback(void);
void dac_overrun_callback(void);
void adc_underrun_callback(void);
void adc_reload_callback(void);

int16_t samples[SOUND_SAMPLES];
uint32_t samples_count;

#define SAMPLE_OFFSET   0x80
#define SAMPLE_RATE     46875
#define SAMPLE_COUNT    (sizeof(sound_data))

static const int8_t sound_data[] =
{
  0x00, 0x04, 0x08, 0x0C, 0x10, 0x14, 0x17, 0x1B, 0x1F, 0x23,
  0x27, 0x2B, 0x2F, 0x32, 0x36, 0x3A, 0x3D, 0x41, 0x44, 0x47,
  0x4B, 0x4E, 0x51, 0x54, 0x57, 0x5A, 0x5D, 0x60, 0x62, 0x65,
  0x67, 0x69, 0x6C, 0x6E, 0x70, 0x72, 0x73, 0x75, 0x77, 0x78,
  0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7E, 0x7F, 0x7F, 0x7F,
  0x7F, 0x7F, 0x7F, 0x7F, 0x7E, 0x7E, 0x7D, 0x7C, 0x7B, 0x7A,
  0x79, 0x78, 0x77, 0x75, 0x73, 0x72, 0x70, 0x6E, 0x6C, 0x69,
  0x67, 0x65, 0x62, 0x60, 0x5D, 0x5A, 0x57, 0x54, 0x51, 0x4E,
  0x4B, 0x47, 0x44, 0x41, 0x3D, 0x3A, 0x36, 0x32, 0x2F, 0x2B,
  0x27, 0x23, 0x1F, 0x1B, 0x17, 0x14, 0x10, 0x0C, 0x08, 0x04,
  0x00, 0xFC, 0xF8, 0xF4, 0xF0, 0xEC, 0xE9, 0xE5, 0xE1, 0xDD,
  0xD9, 0xD5, 0xD1, 0xCE, 0xCA, 0xC6, 0xC3, 0xBF, 0xBC, 0xB9,
  0xB5, 0xB2, 0xAF, 0xAC, 0xA9, 0xA6, 0xA3, 0xA0, 0x9E, 0x9B,
  0x99, 0x97, 0x94, 0x92, 0x90, 0x8E, 0x8D, 0x8B, 0x89, 0x88,
  0x87, 0x86, 0x85, 0x84, 0x83, 0x82, 0x82, 0x81, 0x81, 0x81,
  0x80, 0x81, 0x81, 0x81, 0x82, 0x82, 0x83, 0x84, 0x85, 0x86,
  0x87, 0x88, 0x89, 0x8B, 0x8D, 0x8E, 0x90, 0x92, 0x94, 0x97,
  0x99, 0x9B, 0x9E, 0xA0, 0xA3, 0xA6, 0xA9, 0xAC, 0xAF, 0xB2,
  0xB5, 0xB9, 0xBC, 0xBF, 0xC3, 0xC6, 0xCA, 0xCE, 0xD1, 0xD5,
  0xD9, 0xDD, 0xE1, 0xE5, 0xE9, 0xEC, 0xF0, 0xF4, 0xF8, 0xFC
};

//! Welcome message to display.
#define MSG_WELCOME "\x1B[2J\x1B[H---------- Welcome to TPA6130 example ---------- \r\n"

void master_callback(uint32_t arg)
{
  if( arg == AUDIO_DAC_OUT_OF_SAMPLE_CB )
  {
    dac_overrun_callback();
  }

  else if( arg == AUDIO_DAC_RELOAD_CB )
  {
    dac_reload_callback();
  }

  else if( arg == AUDIO_ADC_OUT_OF_SAMPLE_CB )
  {
    adc_underrun_callback();;
  }

  else if( arg == AUDIO_ADC_RELOAD_CB )
  {
    adc_reload_callback();;
  }
}

void dac_reload_callback(void)
{
  // Nothing todo
}

void dac_overrun_callback(void)
{
  // Nothing todo
}


void adc_underrun_callback(void)
{
  // Nothing todo
}


void adc_reload_callback(void)
{
  // Nothing todo
}

static void twi_init(void)
{
  const gpio_map_t TPA6130_TWI_GPIO_MAP =
  {
  {TPA6130_TWI_SCL_PIN, TPA6130_TWI_SCL_FUNCTION},
  {TPA6130_TWI_SDA_PIN, TPA6130_TWI_SDA_FUNCTION}
  };

  const twi_options_t TPA6130_TWI_OPTIONS =
  {
    .pba_hz = FPBA_HZ,
    .speed  = TPA6130_TWI_MASTER_SPEED,
    .chip   = TPA6130_TWI_ADDRESS
  };

  // Assign I/Os to SPI.
  gpio_enable_module(TPA6130_TWI_GPIO_MAP,
    sizeof(TPA6130_TWI_GPIO_MAP) / sizeof(TPA6130_TWI_GPIO_MAP[0]));

  // Initialize as master.
  twi_master_init(TPA6130_TWI, &TPA6130_TWI_OPTIONS);
}

/*! \brief Initializes the MCU system clocks.
 */
void init_sys_clocks(void)
{
  // Switch to OSC0 to speed up the booting
  pm_switch_to_osc0(&AVR32_PM, FOSC0, OSC0_STARTUP);

  // Start oscillator1
  pm_enable_osc1_crystal(&AVR32_PM, FOSC1);
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
  // Set all peripheral clocks torun at master clock rate
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

  // Use 12MHz from OSC0 and generate 96 MHz
  pm_pll_setup(&AVR32_PM, 1,  // pll.
    7,   // mul.
    1,   // div.
    0,   // osc.
    16); // lockcount.

  pm_pll_set_option(&AVR32_PM, 1, // pll.
    1,  // pll_freq: choose the range 80-180MHz.
    1,  // pll_div2.
    0); // pll_wbwdisable.

  // start PLL1 and wait forl lock
  pm_pll_enable(&AVR32_PM, 1);

  // Wait for PLL1 locked.
  pm_wait_for_pll1_locked(&AVR32_PM);

}

/*! main function */
int main(void)
{
  init_sys_clocks();

  // Initialize RS232 debug text output.
  init_dbg_rs232(FOSC0);

  print_dbg(MSG_WELCOME);

  // Enable LED0 and LED1
  gpio_enable_gpio_pin(LED0_GPIO);
  gpio_enable_gpio_pin(LED1_GPIO);

  // Configure TWI as master
  twi_init();

  // Initialize TPA6130
  tpa6130_init();

  // Initialize DAC that send audio to TPA6130
  tpa6130_dac_start(DEFAULT_DAC_SAMPLE_RATE_HZ,
                    DEFAULT_DAC_NUM_CHANNELS,
                    DEFAULT_DAC_BITS_PER_SAMPLE,
                    DEFAULT_DAC_SWAP_CHANNELS,
                    master_callback,
                      AUDIO_DAC_OUT_OF_SAMPLE_CB
                    | AUDIO_DAC_RELOAD_CB,
                    FOSC0);

  tpa6130_set_volume(0x2F);
  tpa6130_get_volume();

  int count = 0;
  int i=0;

  while(true)
  {
    count = 0;

    // Store sample from the sound_table array
    while(count < (SOUND_SAMPLES)){
      samples[count++] = ((uint8_t)sound_table[i]+0x80) << 8;
      samples[count++] = ((uint8_t)sound_table[i]+0x80) << 8;
      i++;
      if (i >= sizeof(sound_table)) i = 0;
    }

    gpio_set_gpio_pin(LED0_GPIO);
    gpio_clr_gpio_pin(LED1_GPIO);

    // Play buffer
    tpa6130_dac_output((void *) samples,SOUND_SAMPLES/2);

    gpio_clr_gpio_pin(LED0_GPIO);
    gpio_set_gpio_pin(LED1_GPIO);

    /* Wait until the reload register is empty.
     * This means that one transmission is still ongoing
     * but we are already able to set up the next transmission
     */
     while(!tpa6130_dac_output(NULL, 0));
  }
}
