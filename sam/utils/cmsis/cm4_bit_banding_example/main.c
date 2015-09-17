/**
 * \file
 *
 * \brief Bit Banding Example.
 *
 * Copyright (c) 2011-2015 Atmel Corporation. All rights reserved.
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
 */

/**
 * \mainpage Bit Banding Example
 *
 * \section Purpose
 *
 * The cm4-bitbanding example will help new users get familiar with
 * bitbanding feature of SAM4 microcontrollers.
 *
 * Bit-banding is a useful feature of Cortex M4 processor. The processor memory map
 * includes two bit-band regions, which occupy the lowest 1MB of the SRAM and
 * Peripheral memory regions. These bit-band regions map each word in an alias
 * region of memory to a bit in a bit-band region of memory. 
 *
 * The example will execute the following tests:
 * - Test REGISTER bit-banding accessing with LED0
 *   (BitbandingPeripheralTest())
 *
 * \section Requirements
 *
 * This package can be used with SAM4 evaluation kits.
 *
 * \section Description
 * The cm4-bitbanding example will help new users get familiar with
 * bitbanding feature of SAM4 microcontrollers.
 *
 * \section Usage
 *
 * -# Build the program and download it into the evaluation board.
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <math.h>
#include "asf.h"
#include "conf_board.h"

/**
 *  \brief Calculate bit band alias address.
 * 
 *  Calculate the bit band alias address and return a pointer address to word.
 *
 *  \param addr The byte address of bitbanding bit.
 *  \param bit  The bit position of bitbanding bit.
 *  \callergraph
 */
#define BITBAND_ALIAS_ADDRESS(addr, bit) \
    ((volatile uint32_t*)((((uint32_t)(addr) & 0xF0000000) + 0x02000000) \
                          +((((uint32_t)(addr)&0xFFFFF)*32)\
                          +(  (uint32_t)(bit)*4))))

//! Counts for 1ms timeTicks.
volatile uint32_t g_ul_ms_ticks = 0;

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/// @endcond

/**
 * \brief Delay number of tick Systicks (happens every 1 ms).
 */
__INLINE static void delay_ms(uint32_t ul_dly_ticks)
{
	uint32_t ul_cur_ticks;

	ul_cur_ticks = g_ul_ms_ticks;
	while ((g_ul_ms_ticks - ul_cur_ticks) < ul_dly_ticks);
}

 /**
 * \brief Configure LED pins.
 */
__INLINE static void led_config(void)
{
	// Set up LED pins.
	LED0_PIO->PIO_PER = LED0_MASK;
	LED0_PIO->PIO_OER = LED0_MASK;
	LED0_PIO->PIO_PUDR = LED0_MASK;
}

/**
 * \brief SysTick_Handler.
 */
void SysTick_Handler(void)
{
	// Increment counter necessary in delay().
	g_ul_ms_ticks++;
}

/**
 * \brief Application entry point.
 *
 * \return Unused (ANSI-C compatibility).
 */
int main(void)
{
	uint8_t uc_led_bit;

	/* Initialize the SAM4 system */
	SystemInit();

	WDT->WDT_MR = WDT_MR_WDDIS;

	// Set up SysTick Timer for 1 msec interrupts.
	if (SysTick_Config(SystemCoreClock / 1000)) {
		// Capture error.
		while (1);
	}

	led_config();

	uc_led_bit = (uint8_t) (log10(LED0_MASK) / log10(2));

	// Flash the LED.
	while (1) {
		/* Toggle LED with bit banding. */
		if (*BITBAND_ALIAS_ADDRESS(&LED0_PIO->PIO_ODSR, uc_led_bit)) {
			/* Turn on the LED. */
			*BITBAND_ALIAS_ADDRESS(&LED0_PIO->PIO_CODR, uc_led_bit) = 1;
		} else {
			/* Turn off the LED. */
			*BITBAND_ALIAS_ADDRESS(&LED0_PIO->PIO_SODR, uc_led_bit) = 1;
		}

		delay_ms(1000);
	}
}

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/// @endcond
