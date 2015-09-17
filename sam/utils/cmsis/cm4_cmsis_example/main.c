/**
 * \file
 *
 * \brief CMSIS Example.
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
 * \mainpage CMSIS Example
 *
 * \section Purpose
 *
 * The cmsis example will help new users get familiar with
 * basic cmsis utilities of SAM4 microcontrollers.
 *
 * The example will execute the following tests:
 * - The application will flash the LED per second. The second
 * is calculated by the standard system tick interface of cmsis.
 *
 * \section Requirements
 *
 * This package can be used with SAM4 evaluation kits.
 *
 * \section Description
 * The cmsis example will use the system tick of the Cortex-M4.
 *
 * \section Usage
 *
 * -# Build the program and download it into the evaluation board.
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include "asf.h"
#include "conf_board.h"

__INLINE static void delay_ms(uint32_t ul_dly_ticks);

/* Systick Counter */
static volatile uint32_t g_ul_ms_ticks = 0U;

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/// @endcond

/**
 * \brief SysTick_Handler.
 */
void SysTick_Handler(void)
{
	/* Increment counter necessary in delay(). */
	g_ul_ms_ticks++;
}

/**
 * \brief Delay number of tick Systicks (happens every 1 ms).
 */
__INLINE static void delay_ms(uint32_t ul_dly_ticks)
{
	uint32_t ul_cur_ticks;

	ul_cur_ticks = g_ul_ms_ticks;
	while ((g_ul_ms_ticks - ul_cur_ticks) < ul_dly_ticks) {
	}
}

/**
 * \brief Application entry point.
 *
 * \return Unused (ANSI-C compatibility).
 */
int main(void)
{
	/* Initialize the SAM4 system */
	sysclk_init();
	board_init();

	/* Set up SysTick Timer for 1 msec interrupts. */
	if (SysTick_Config(sysclk_get_cpu_hz() / (uint32_t) 1000)) {
		/* Capture error. */
		while (1) {
		}
	}

	/* Flash the LED. */
	while (1) {
		/* Turn on the LED. */
		LED_On(LED0);
		/* Delay  1000 Msec. */
		delay_ms((uint32_t) 1000);

		/* Turn off the LED. */
		LED_Off(LED0);
		/* Delay  1000 Msec. */
		delay_ms((uint32_t) 1000);
	}
}

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/// @endcond
