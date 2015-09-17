/**
 * \file
 *
 * \brief Board specific code for low power example.
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

#include "board.h"
#include "pio.h"
#include "pio_handler.h"
#include "pmc.h"
#include <rstc.h>
#include "low_power_board.h"

/** IRQ priority for PIO (The lower the value, the greater the priority) */
#define IRQ_PRIOR_PIO    0

/** Clock list from fast RC */
uint32_t g_fastrc_clock_list[][3] = {
	/* MCK,    FastRC,                  Prescaler */
	{125000,   CKGR_MOR_MOSCRCF_4_MHz,  PMC_MCKR_PRES_CLK_32},
	{250000,   CKGR_MOR_MOSCRCF_4_MHz,  PMC_MCKR_PRES_CLK_16},
	{500000,   CKGR_MOR_MOSCRCF_4_MHz,  PMC_MCKR_PRES_CLK_8},
	{1000000,  CKGR_MOR_MOSCRCF_4_MHz,  PMC_MCKR_PRES_CLK_4},
	{2000000,  CKGR_MOR_MOSCRCF_4_MHz,  PMC_MCKR_PRES_CLK_2},
	{4000000,  CKGR_MOR_MOSCRCF_4_MHz,  PMC_MCKR_PRES_CLK_1},
	{8000000,  CKGR_MOR_MOSCRCF_8_MHz,  PMC_MCKR_PRES_CLK_1},
	{12000000, CKGR_MOR_MOSCRCF_12_MHz, PMC_MCKR_PRES_CLK_1}
};

/** Clock list from PLL */
uint32_t g_pll_clock_list[][4] = {
	/* MCK, MUL, DIV, PRES */
	/* MCK = 8000000 * (11+1) / 1 / 4 = 24 MHz */
	{24000000, 11, 1, PMC_MCKR_PRES_CLK_4},
	/* MCK = 8000000 * (11+1) / 1 / 3 = 32 MHz */
	{32000000, 11, 1, PMC_MCKR_PRES_CLK_3},
	/* MCK = 8000000 * (11+1) / 1 / 2 = 48 MHz */
	{48000000, 11, 1, PMC_MCKR_PRES_CLK_2},
	/* MCK = 8000000 * (15+1) / 1 / 2 = 64 MHz */
	{64000000, 15, 1, PMC_MCKR_PRES_CLK_2},
	/* MCK = 8000000 * (20+1) / 2 / 1 = 84 MHz */
	{84000000, 20, 2, PMC_MCKR_PRES_CLK_1},
	/* MCK = 8000000 * (24+1) / 2 / 1 = 100 MHz */
	{100000000, 24, 2, PMC_MCKR_PRES_CLK_1},
	/* MCK = 8000000 * (29+1) / 2 / 1 = 120 MHz */
	{120000000, 29, 2, PMC_MCKR_PRES_CLK_1}
};

/**
 * \brief Initialize SAM4C_EK board for low power test.
 */
void init_specific_board(void)
{
	/* Configure all PIOs as inputs to save power */
	pio_set_input(PIOA, 0xFFFFFFFF, PIO_PULLUP);
	pio_set_input(PIOB, 0xFFFFFFFF, PIO_PULLUP);

	/* Enable the PMC clocks of push button for wakeup */
	pmc_enable_periph_clk(ID_PIOA);
	pio_handler_set_priority(PIOA, PIOA_IRQn, IRQ_PRIOR_PIO);
}
