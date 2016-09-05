/**
 * \file
 *
 * \brief Reinforced Safety Watchdog Timer (RSWDT) driver for SAM.
 *
 * Copyright (c) 2015 Atmel Corporation. All rights reserved.
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
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include "rswdt.h"

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/// @endcond

/**
 * \defgroup sam_drivers_rswdt_group Reinforced Safety Watchdog Timer (RSWDT)
 *
 * Driver for the RSWDT (Reinforced Safety Watchdog Timer). This driver provides access to the main
 * features of the RSWDT controller.
 * The Reinforced Safety Watchdog Timer can be used to prevent system lock-up if the software
 * becomes trapped in a deadlock. It features a 12-bit down counter that allows
 * a watchdog period of up to 16 seconds (slow clock at 32.768 kHz). It can
 * generate a general reset or a processor reset only. In addition, it can be
 * stopped while the processor is in debug mode or idle mode.
 *
 * @{
 */

#define RSWDT_KEY_PASSWORD  0xC4000000
#define RSWDT_SLCK_DIV      128
#define RSWDT_MAX_VALUE     4095

/**
 * \brief Get counter value or permitted range value of reinforced safety watchdog timer from the
 * desired timeout period (in us).
 *
 * \note The value returned by this function can be used by rswdt_init() if it is
 * not RSWDT_INVALID_ARGUMENT.
 *
 * \param ul_us The desired timeout period (in us).
 * \param ul_sclk The slow clock on board (in Hz).
 *
 * \return If the desired period is beyond the reinforced safety watchdog period, this function
 * returns RSWDT_INVALID_ARGUMENT. Otherwise it returns valid value.
 */
uint32_t rswdt_get_timeout_value(uint32_t ul_us, uint32_t ul_sclk)
{
	uint32_t max, min;

	min = RSWDT_SLCK_DIV * 1000000 / ul_sclk;
	max = min * RSWDT_MAX_VALUE;

	if ((ul_us < min) || (ul_us > max)) {
		return RSWDT_INVALID_ARGUMENT;
	}

	return RSWDT_MR_WDV(ul_us / min);
}

/**
 * \brief Initialize reinforced safety watchdog timer with the given mode.
 *
 * \param p_rswdt Pointer to a RSWDT instance.
 * \param ul_mode Bitmask of reinforced safety watchdog timer mode.
 * \param us_counter The value loaded in the 12-bit Reinforced Safety Watchdog Counter.
 * \param us_delta The permitted range for reloading the Reinforced Safety Watchdog Timer.
 */
void rswdt_init(Rswdt *p_rswdt, uint32_t ul_mode, uint16_t us_counter,
		uint16_t us_delta)
{
	p_rswdt->RSWDT_MR = ul_mode |
			RSWDT_MR_WDV(us_counter) | RSWDT_MR_WDD(us_delta);
}

/**
 * \brief Disable the reinforced safety watchdog timer.
 */
void rswdt_disable(Rswdt *p_rswdt)
{
	p_rswdt->RSWDT_MR = RSWDT_MR_WDDIS;
}

/**
 * \brief Restart the reinforced safety watchdog timer.
 */
void rswdt_restart(Rswdt *p_rswdt)
{
	if (p_rswdt == RSWDT) {
		p_rswdt->RSWDT_CR = RSWDT_KEY_PASSWORD | RSWDT_CR_WDRSTT;
	}
}

/**
 * \brief Check the reinforced safety watchdog timer status.
 *
 * \return Bitmask of reinforced safety watchdog timer status.
 */
uint32_t rswdt_get_status(Rswdt *p_rswdt)
{
	return p_rswdt->RSWDT_SR;
}

/**
 * \brief Get the timeout period of the Reinforced Safety WatchDog Timer in microseconds.
 *
 * \param p_rswdt Pointer to a RSWDT instance.
 * \param ul_sclk The slow clock frequency (in Hz).
 *
 * \return The timeout period in microseconds.
 */
uint32_t rswdt_get_us_timeout_period(Rswdt *p_rswdt, uint32_t ul_sclk)
{
	return RSWDT_MR_WDV(p_rswdt->RSWDT_MR) * RSWDT_SLCK_DIV / ul_sclk * 1000000;
}

//@}

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/// @endcond
