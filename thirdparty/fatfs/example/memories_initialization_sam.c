/**
 * \file
 *
 * \brief Initialization of memories (SAM)
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
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
