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
#include "low_power_board.h"

/** IRQ priority for PIO (The lower the value, the greater the priority) */
#define IRQ_PRIORITY_PIO    0

/** Clock list from fast RC */
uint32_t g_fastrc_clock_list[][3] = {
	/* MCK,    FastRC,                  Prescaler */
	{250000, CKGR_MOR_MOSCRCF_8_MHz, PMC_MCKR_PRES_CLK_32},
	{500000, CKGR_MOR_MOSCRCF_8_MHz, PMC_MCKR_PRES_CLK_16},
	{1000000, CKGR_MOR_MOSCRCF_8_MHz, PMC_MCKR_PRES_CLK_8},
	{2000000, CKGR_MOR_MOSCRCF_8_MHz, PMC_MCKR_PRES_CLK_4},
	{4000000, CKGR_MOR_MOSCRCF_8_MHz, PMC_MCKR_PRES_CLK_2},
	{8000000, CKGR_MOR_MOSCRCF_8_MHz, PMC_MCKR_PRES_CLK_1},
	{16000000, CKGR_MOR_MOSCRCF_16_MHz, PMC_MCKR_PRES_CLK_1},
	{24000000, CKGR_MOR_MOSCRCF_24_MHz, PMC_MCKR_PRES_CLK_1}
};

/** Clock list from PLL */
uint32_t g_pll_clock_list[][4] = {
	/* MCK, MUL, DIV, PRES */
	/* MCK = 32768 * (1464+1) / 1 / 1 = 48 MHz */
	{48000000, 1465, 1, PMC_MCKR_PRES_CLK_1},
	/* MCK = 32000 * (2929+1) / 1 / 1 = 96 MHz */
	{96000000, 2930, 1, PMC_MCKR_PRES_CLK_1},
};

/**
 * \brief Configures SAMG55 for low power demo.
 */
void init_specific_board(void)
{
	/* For the lowest power consumption all pins should have defined state
	 * e.g. no floating pins.
	 * Set all pins as input with pull-up enabled with the exception:
	 * - CDC UART RX (PA27) should not be pulled up because this pin is
	 *   driven by the EDBG in this application
	 * - CDC UART TX (PA28) This is actively driven by the SAMG55 in this
	 *   application
	 * - PB04 should set as output
	 * - PB8 PB9 PB10 PB11 should set as output
	 */
	pio_set_input(PIOA, 0xE7FFFFFF, PIO_PULLUP);
	pio_set_input(PIOA, 0x18000000, PIO_DEFAULT);
	pio_set_input(PIOB, 0xFFFF0EF, PIO_PULLUP);
	pio_set_output(PIOB, 0xF10, HIGH, DISABLE, DISABLE);
	/* Enable the PMC clocks of push button for wakeup */
	pmc_enable_periph_clk(ID_PIOA);
	pio_handler_set_priority(PIOA, PIOA_IRQn, IRQ_PRIORITY_PIO);
}
