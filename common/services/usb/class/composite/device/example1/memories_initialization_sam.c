/**
 * \file
 *
 * \brief Initialization of memories
 * Specific SAM functions to initialize the memories
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

#include <asf.h>
#include "conf_board.h" /* To get on-board memories configurations */
#include "conf_access.h"
#include "main.h"

#ifdef CONF_BOARD_SMC_PSRAM
static void psram_init(void)
{
	pmc_enable_periph_clk(ID_SMC);
	/* complete SMC configuration between PSRAM and SMC waveforms. */
	smc_set_setup_timing(SMC, 0, SMC_SETUP_NWE_SETUP(0)
			| SMC_SETUP_NCS_WR_SETUP(2)
			| SMC_SETUP_NRD_SETUP(2)
			| SMC_SETUP_NCS_RD_SETUP(2));
	smc_set_pulse_timing(SMC, 0, SMC_PULSE_NWE_PULSE(6)
			| SMC_PULSE_NCS_WR_PULSE(6)
			| SMC_PULSE_NRD_PULSE(6)
			| SMC_PULSE_NCS_RD_PULSE(6));
	smc_set_cycle_timing(SMC, 0, SMC_CYCLE_NWE_CYCLE(8)
			| SMC_CYCLE_NRD_CYCLE(8));
	smc_set_mode(SMC, 0, SMC_MODE_READ_MODE | SMC_MODE_WRITE_MODE
			| SMC_MODE_DBW_BIT_16);
}
#endif

#ifdef CONF_BOARD_SRAM
static void ext_sram_init(void)
{
	pmc_enable_periph_clk(ID_SMC);
	smc_set_setup_timing(SMC, 0, SMC_SETUP_NWE_SETUP(1)
			| SMC_SETUP_NCS_WR_SETUP(1)
			| SMC_SETUP_NRD_SETUP(1)
			| SMC_SETUP_NCS_RD_SETUP(1));
	smc_set_pulse_timing(SMC, 0, SMC_PULSE_NWE_PULSE(6)
			| SMC_PULSE_NCS_WR_PULSE(6)
			| SMC_PULSE_NRD_PULSE(6)
			| SMC_PULSE_NCS_RD_PULSE(6));
	smc_set_cycle_timing(SMC, 0, SMC_CYCLE_NWE_CYCLE(7)
			| SMC_CYCLE_NRD_CYCLE(7));
	smc_set_mode(SMC, 0,
			SMC_MODE_READ_MODE | SMC_MODE_WRITE_MODE
#if !defined(SAM4S)
			| SMC_MODE_DBW_8_BIT
#endif
			);
	/* Configure LB, enable SRAM access */
	pio_configure_pin(PIN_EBI_NLB, PIN_EBI_NLB_FLAGS);
	/* Pull down LB, enable sram access */
	pio_set_pin_low(PIN_EBI_NLB);
}
#endif

void memories_initialization(void)
{
#ifdef CONF_BOARD_SMC_PSRAM
	psram_init();
#endif
#ifdef CONF_BOARD_SRAM
	ext_sram_init();
#endif
}
