/**
 * \file
 *
 * \brief Initialization of memories (SAM)
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

#include "compiler.h"
#include "sysclk.h"
#include "conf_board.h" /* To get on-board memories configurations */
#include "conf_access.h"
#include "memories_initialization.h"

#ifdef CONF_BOARD_AT45DBX
#include "at45dbx.h"
#endif

#ifdef CONF_BOARD_SDRAMC
#include "sdramc.h"
#include "mt48lc16m16a2.h"
#endif

#ifdef CONF_BOARD_SMC_PSRAM
#include "smc.h"
static void psram_init(void)
{
	pmc_enable_periph_clk(ID_SMC);
	/* Complete SMC configuration between PSRAM and SMC waveforms. */
	smc_set_setup_timing(SMC, 0, SMC_SETUP_NWE_SETUP(1)
			| SMC_SETUP_NCS_WR_SETUP(0)
			| SMC_SETUP_NRD_SETUP(2)
			| SMC_SETUP_NCS_RD_SETUP(0));
	smc_set_pulse_timing(SMC, 0, SMC_PULSE_NWE_PULSE(3)
			| SMC_PULSE_NCS_WR_PULSE(4)
			| SMC_PULSE_NRD_PULSE(3)
			| SMC_PULSE_NCS_RD_PULSE(5));
	smc_set_cycle_timing(SMC, 0, SMC_CYCLE_NWE_CYCLE(4)
			| SMC_CYCLE_NRD_CYCLE(5));
	smc_set_mode(SMC, 0, SMC_MODE_READ_MODE | SMC_MODE_WRITE_MODE

#if SAM3S || SAM4S
			| SMC_MODE_DBW_8_BIT
#elif SAM3U
			| SMC_MODE_DBW_BIT_16
#elif SAM3XA
			| SMC_MODE_DBW_BIT_8
#endif
			);
}

#endif

#ifdef CONF_BOARD_SRAM
#include "smc.h"
#include "pio.h"
static void sram_init(void)
{
	/* Enable PMC clock for SMC */
	pmc_enable_periph_clk(ID_SMC);

	/* Complete SMC configuration between SRAM and SMC waveforms. */
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
			SMC_MODE_READ_MODE | SMC_MODE_WRITE_MODE |
			SMC_MODE_DBW_8_BIT);

	/* Configure LB, enable SRAM access */
	pio_configure_pin(PIN_EBI_NLB, PIN_EBI_NLB_FLAGS);

	/* Pull down LB, enable SRAM access */
	pio_set_pin_low(PIN_EBI_NLB);
}

#endif

void memories_initialization(void)
{
#ifdef CONF_BOARD_SMC_PSRAM
	psram_init();
#endif

#ifdef CONF_BOARD_SRAM
	sram_init();
#endif

#ifdef CONF_BOARD_SDRAMC
	/* Enable SMC peripheral clock */
	pmc_enable_periph_clk(ID_SMC);

	/* Complete SDRAM configuration */
	sdramc_init((sdramc_memory_dev_t *)&SDRAM_MICRON_MT48LC16M16A2,
			sysclk_get_cpu_hz());
#endif

#ifdef CONF_BOARD_AT45DBX
	at45dbx_init();

	if (at45dbx_mem_check() != true) {
		while (1) {
		}
	}
#endif
}
