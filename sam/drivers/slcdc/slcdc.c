/**
 * \file
 *
 * \brief SAM Segment LCD Controller (SLCDC).
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

#include "slcdc.h"
#include "sysclk.h"
#include "sleepmgr.h"

/**
 * \ingroup group_sam_drivers_slcdc
 *
 * @{
 */

/**
 * \internal
 * \brief SLCDC callback function pointer.
 */
slcdc_callback_t slcdc_callback_pointer = NULL;

/** The actual frame rate value */
static uint32_t g_frame_rate = 0;

status_code_t slcdc_init(Slcdc *p_slcdc, struct slcdc_config *slcdc_cfg)
{
	if(slcdc_get_status(p_slcdc) == SLCDC_SR_ENA) {
		return ERR_BUSY;
	}

	/* Clock initialization */
	sysclk_enable_peripheral_clock(ID_SLCDC);

	/* SLCDC basic configuration */
	p_slcdc->SLCDC_MR = (CONF_SLCDC_BIAS << SLCDC_MR_BIAS_Pos) |
			((CONF_SLCDC_COM_NUM - 1) << SLCDC_MR_COMSEL_Pos) |
			SLCDC_MR_SEGSEL(CONF_SLCDC_SEG_NUM - 1) |
			slcdc_cfg->buf_time;
	if(slcdc_set_frame_rate(p_slcdc, slcdc_cfg->frame_rate)
				!= STATUS_OK) {
		return ERR_INVALID_ARG;
	}
	slcdc_set_display_mode(p_slcdc, slcdc_cfg->disp_mode);

	return STATUS_OK;
}

status_code_t slcdc_set_frame_rate(Slcdc *p_slcdc, uint32_t frame_rate)
{
	uint32_t ul_divisors[SLCDC_CLOCK_PRE_MAX] =
		{8, 16, 32, 64, 128, 256, 512, 1024};
	uint32_t ul_pre = 0;
	uint32_t ul_div;

	/* Find prescaler and divisor values */
	ul_div = (BOARD_FREQ_SLCK_XTAL / ul_divisors[ul_pre])
			/ frame_rate
			/ SLCDC_NCOM_VALUE;

	while ((ul_div > SLCDC_CLOCK_DIV_MAX) && (ul_pre < SLCDC_CLOCK_PRE_MAX)) {
		ul_pre++;
		ul_div = (BOARD_FREQ_SLCK_XTAL / ul_divisors[ul_pre])
				/ frame_rate
				/ SLCDC_NCOM_VALUE;
	}

	/* Return result */
	if (ul_pre < SLCDC_CLOCK_PRE_MAX) {
		p_slcdc->SLCDC_FRR = 0;
		p_slcdc->SLCDC_FRR = ((ul_div - 1) << SLCDC_FRR_DIV_Pos) |
			(ul_pre << SLCDC_FRR_PRESC_Pos);
		g_frame_rate = (BOARD_FREQ_SLCK_XTAL / ul_divisors[ul_pre])
			/ ul_div / SLCDC_NCOM_VALUE;
		return STATUS_OK;
	} else {
		return ERR_INVALID_ARG;
	}
}

status_code_t slcdc_set_blink_freq(Slcdc *p_slcdc, uint32_t value,
		bool is_period_freq)
{
	uint32_t ul_div;

	if(value == 0) {
		ul_div = 0;
	} else {
		if(is_period_freq) {
			ul_div = (g_frame_rate / value);
		} else {
			ul_div = (g_frame_rate * value);
		}
	}
	if(ul_div < 256) {
		p_slcdc->SLCDC_DR &= ~SLCDC_DR_LCDBLKFREQ_Msk;
		p_slcdc->SLCDC_DR |= SLCDC_DR_LCDBLKFREQ(ul_div);
		return STATUS_OK;
	} else {
		return ERR_INVALID_ARG;
	}
}

void slcdc_enable(Slcdc *p_slcdc)
{
	sleepmgr_lock_mode(SLEEPMGR_BACKUP);
	p_slcdc->SLCDC_CR = SLCDC_CR_LCDEN;
}

void slcdc_disable(Slcdc *p_slcdc)
{
	sleepmgr_unlock_mode(SLEEPMGR_BACKUP);
	p_slcdc->SLCDC_CR = SLCDC_CR_LCDDIS;
}

void slcdc_set_display_memory(Slcdc *p_slcdc)
{
	uint8_t tmp =  CONF_SLCDC_COM_NUM;

	while(tmp--) {
		slcdc_set_lsb_memory(p_slcdc, tmp, ~0UL);
		slcdc_set_msb_memory(p_slcdc, tmp, ~0UL);
	}
}

void slcdc_clear_display_memory(Slcdc *p_slcdc)
{
	uint8_t tmp =  CONF_SLCDC_COM_NUM;

	while(tmp--) {
		slcdc_clear_lsb_memory(p_slcdc, tmp, ~0UL);
		slcdc_clear_msb_memory(p_slcdc, tmp, ~0UL);
	}
}

void slcdc_set_callback(Slcdc *p_slcdc, slcdc_callback_t callback,
	uint32_t ul_sources, uint8_t irq_line, uint8_t irq_level)
{
	slcdc_callback_pointer = callback;
	NVIC_ClearPendingIRQ((IRQn_Type)irq_line);
	NVIC_SetPriority((IRQn_Type)irq_line, irq_level);
	NVIC_EnableIRQ((IRQn_Type)irq_line);
	slcdc_enable_interrupt(p_slcdc, ul_sources);
}

void SLCDC_Handler(void)
{
	/* Interrupt handler */
	if (slcdc_callback_pointer != NULL) {
		slcdc_callback_pointer();
	}
}

/**
 * @}
 */
