/*****************************************************************************
 *
 * \file
 *
 * \brief SSC I2S example application.
 *
 * This file gives an example of using the SSC in I2S mode.
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
 *****************************************************************************/

/*! \mainpage
 * \section intro Introduction
 * This documents data structures, functions, variables, defines, enums, and
 * typedefs in the software for application note AVR32x. It also gives an
 * example of the usage of the SSC driver generating I2S output.
 *
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC for AVR32 and IAR Systems compiler
 * for AVR32. Other compilers may or may not work.
 *
 * \section deviceinfo Device Info
 * All AVR32 devices with an SSC module can be used. This example has been tested
 * with the following setup:
 *
 * \section setupinfo Setup Information
 * CPU speed: <i>12 MHz</i>
 *
 * The SSC bus located on the EVK1100/EVK1101 board must be connected to an Audio Codec.
 *
 * \section applicationUsage Application Usage
 *
 * \subsection applicationUsageMaster Master mode
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/products/AVR32/">Atmel AVR32</A>.\n
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


#include <avr32/io.h>
#include "board.h"
#include "pm.h"
#include "gpio.h"
#include "ssc_i2s.h"


/*! \brief Main function, application starts executing here after
 *         initializing the stack pointer.
 */
int main(void)
{
  static const gpio_map_t SSC_GPIO_MAP =
  {
    {AVR32_SSC_TX_CLOCK_0_PIN,      AVR32_SSC_TX_CLOCK_0_FUNCTION     },
    {AVR32_SSC_TX_DATA_0_PIN,       AVR32_SSC_TX_DATA_0_FUNCTION      },
    {AVR32_SSC_TX_FRAME_SYNC_0_PIN, AVR32_SSC_TX_FRAME_SYNC_0_FUNCTION}
  };

	volatile avr32_ssc_t *ssc = &AVR32_SSC;

  // Switch main clock to external oscillator 0 (crystal).
  pm_switch_to_osc0(&AVR32_PM, FOSC0, OSC0_STARTUP);

  // Assign GPIO to SSC.
  gpio_enable_module(SSC_GPIO_MAP, sizeof(SSC_GPIO_MAP) / sizeof(SSC_GPIO_MAP[0]));

  // SSC init in I2S mode.
	ssc_i2s_init(ssc, 11025, 8, 8, SSC_I2S_MODE_STEREO_OUT, FOSC0);

	unsigned char leds = 0x00;

	while (1)
  {
		leds <<= 1;
		leds ^= (leds ^ 0x08) >> 3;
    LED_Display_Field(LED_MONO0_GREEN |
                      LED_MONO1_GREEN |
                      LED_MONO2_GREEN |
                      LED_MONO3_GREEN,
                      leds);

    while (!(ssc_i2s_get_status(ssc) & AVR32_SSC_SR_TXRDY_MASK));
		ssc_i2s_transfer(ssc, 0xA3);
	}
}
