/**
 * \file
 *
 * \brief Provides the low-level initialization functions that called
 * on chip startup.
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
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include "sam4c.h"

/* @cond 0 */
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/* @endcond */


/**
 * System Clock Frequency (Core Clock)
 */
uint32_t SystemCoreClock = CHIP_FREQ_MAINCK_RC_4MHZ;

/**
 * \brief Setup the microcontroller system.
 *
 * Initialize the System and update the SystemFrequency variable.
 */
void SystemInit(void)
{
	/*
	 * TODO:
	 * Add code to initialize the system according to your application.
	 *
	 * For SAM4C, the internal 4MHz fast RC oscillator is the deault clock
	 * selected to start up the system.
	 *
	 * Note:
	 * After reset, the core 1 is hold in reset and with no clock.
	 */
	SystemCoreClock = CHIP_FREQ_MAINCK_RC_4MHZ;
}

/**
 * \brief Get Core Clock Frequency.
 */
void SystemCoreClockUpdate(void)
{
	/*
	 * TODO:
	 * Add code to calculate the system frequency based upon the current
	 * register settings.
	 *
	 * This function can be used to retrieve the system core clock frequeny
	 * after user changed register sittings.
	 */
}

/**
 * \brief Initialize flash wait state according to operating frequency.
 *
 * \param ul_clk System clock frequency.
 */
#ifdef _SAM4C_EFC_INSTANCE_
void system_init_flash(uint32_t ul_clk)
{
	/* Set FWS for embedded Flash access according to operating frequency */
	if (ul_clk < CHIP_FREQ_FWS_0) {
		EFC->EEFC_FMR = EEFC_FMR_FWS(0)|EEFC_FMR_CLOE;
	} else if (ul_clk < CHIP_FREQ_FWS_1) {
		EFC->EEFC_FMR = EEFC_FMR_FWS(1)|EEFC_FMR_CLOE;
	} else if (ul_clk < CHIP_FREQ_FWS_2) {
		EFC->EEFC_FMR = EEFC_FMR_FWS(2)|EEFC_FMR_CLOE;
	} else if (ul_clk < CHIP_FREQ_FWS_3) {
		EFC->EEFC_FMR = EEFC_FMR_FWS(3)|EEFC_FMR_CLOE;
	} else if (ul_clk < CHIP_FREQ_FWS_4) {
		EFC->EEFC_FMR = EEFC_FMR_FWS(4)|EEFC_FMR_CLOE;
	} else {
		EFC->EEFC_FMR = EEFC_FMR_FWS(5)|EEFC_FMR_CLOE;
	}
}
#else
void system_init_flash( uint32_t ul_clk )
{
  /* Set FWS for embedded Flash access according to operating frequency */
#if !defined(ID_EFC1)
    if (ul_clk < CHIP_FREQ_FWS_0) {
		EFC0->EEFC_FMR = EEFC_FMR_FWS(0)|EEFC_FMR_CLOE;
	} else if (ul_clk < CHIP_FREQ_FWS_1) {
		EFC0->EEFC_FMR = EEFC_FMR_FWS(1)|EEFC_FMR_CLOE;
	} else if (ul_clk < CHIP_FREQ_FWS_2) {
		EFC0->EEFC_FMR = EEFC_FMR_FWS(2)|EEFC_FMR_CLOE;
	} else if (ul_clk < CHIP_FREQ_FWS_3) {
		EFC0->EEFC_FMR = EEFC_FMR_FWS(3)|EEFC_FMR_CLOE;
	} else if (ul_clk < CHIP_FREQ_FWS_4) {
		EFC0->EEFC_FMR = EEFC_FMR_FWS(4)|EEFC_FMR_CLOE;
	} else {
		EFC0->EEFC_FMR = EEFC_FMR_FWS(5)|EEFC_FMR_CLOE;
	}
#else
	if (ul_clk < CHIP_FREQ_FWS_0) {
		EFC0->EEFC_FMR = EEFC_FMR_FWS(0)|EEFC_FMR_CLOE;
		EFC1->EEFC_FMR = EEFC_FMR_FWS(0)|EEFC_FMR_CLOE;
	} else if (ul_clk < CHIP_FREQ_FWS_1) {
		EFC0->EEFC_FMR = EEFC_FMR_FWS(1)|EEFC_FMR_CLOE;
		EFC1->EEFC_FMR = EEFC_FMR_FWS(1)|EEFC_FMR_CLOE;
	} else if (ul_clk < CHIP_FREQ_FWS_2) {
		EFC0->EEFC_FMR = EEFC_FMR_FWS(2)|EEFC_FMR_CLOE;
		EFC1->EEFC_FMR = EEFC_FMR_FWS(2)|EEFC_FMR_CLOE;
	} else if (ul_clk < CHIP_FREQ_FWS_3) {
		EFC0->EEFC_FMR = EEFC_FMR_FWS(3)|EEFC_FMR_CLOE;
		EFC1->EEFC_FMR = EEFC_FMR_FWS(3)|EEFC_FMR_CLOE;
	} else if (ul_clk < CHIP_FREQ_FWS_4) {
		EFC0->EEFC_FMR = EEFC_FMR_FWS(4)|EEFC_FMR_CLOE;
		EFC1->EEFC_FMR = EEFC_FMR_FWS(4)|EEFC_FMR_CLOE;
	} else {
		EFC0->EEFC_FMR = EEFC_FMR_FWS(5)|EEFC_FMR_CLOE;
		EFC1->EEFC_FMR = EEFC_FMR_FWS(5)|EEFC_FMR_CLOE;
	}
	
#endif
}

#endif
/* @cond 0 */
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/* @endcond */
