/**
 * @file epd_hw_timer.c
 *
 * @brief
 *
 *
 *  Copyright (c) 2015 Atmel Corporation. All rights reserved.
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
 */

#include <compiler.h>
#include <parts.h>
#include "sysclk.h"
#include "tc_megarf.h"

#include "EPD_hw_timer.h"
#include "conf_EPD_hw_timer.h"

/**
 * @brief Disable Compare Interrupt in channel C
 * @param tc Timer Address
 */
void tc_disable_compare_int(volatile void *p_tc, enum tc_cc_channel_t channel_index)
{
	switch (channel_index){
	case TC_COMPA:
		tc_disable_compa_int(p_tc);		/*  Disable interrupts. */
		clear_compa_flag(p_tc);			/*  Clear status register. */
		break;
	case TC_COMPB:
		tc_disable_compb_int(p_tc);
		clear_compb_flag(p_tc);
		break;
	case TC_COMPC:
		tc_disable_compc_int(p_tc);
		clear_compc_flag(p_tc);
		break;
	default: 
		break;
	}
		
}

/**
 * @brief Disable Overflow Interrupt
 * @param tc Timer Address
 */
void tc_disable_overflow_int(volatile void *p_tc, enum tc_cc_channel_t channel_index)
{
	switch (channel_index){
	case TC_COMPA:
		tc_disable_ovf_int(p_tc);		/*  Disable interrupts. */
		clear_ovf_flag(p_tc);			/*  Clear status register. */
		break;
	case TC_COMPB:
		tc_disable_ovf_int(p_tc);
		clear_ovf_flag(p_tc);
		break;
	case TC_COMPC:
		tc_disable_ovf_int(p_tc);
		clear_ovf_flag(p_tc);
		break;
	default: 
		break;
	}
		
}

/**
* \brief Configure TC for timer, waveform generation or capture.
*
* \param p_tc Pointer to a TC instance.
* \param ul_channel Channel to configure.
* \param ul_mode Control mode register value to set.
* 
*/

void tc_initc(volatile void *p_tc, TC_CC_CHANNEL_t channel_index, struct tc_control_reg *p_control_reg)
{
//enum tc_cc_channel_t *tc_channel;

/*  Disable TC clock. */
tc_disable_overflow_int(p_tc,channel_index);

/*  Disable interrupts. */
tc_disable_compare_int(p_tc, channel_index);

/*  Set CO mode. */
tc_set_co_mode(p_tc,channel_index, p_control_reg->co_mode);

/*  Set WG mode. */
tc_set_wg_mode(p_tc, channel_index, p_control_reg->wg_mode);

}


/*! \brief  to start the running timer
 */
void tc_start(volatile void *p_tc, struct tc_control_reg *p_control_reg)
{
	/*  Set clock. */
	tc_set_clk_source(p_tc, 0, p_control_reg->cs_select);

}

/*! \brief  to stop the running timer
 */
void tc_stop(volatile void *p_tc)
{
	tc_set_clk_source(p_tc, 0, TC_CLKSEL_OFF_gc);
	
	/*  Disable interrupts. */
	tc_disable_compare_int(p_tc, TC_COMPA);
	tc_disable_compare_int(p_tc, TC_COMPB);
	tc_disable_compare_int(p_tc, TC_COMPC);	
	tc_disable_ovf_int(p_tc);	
	
	/*  Set clock. */
	tc_disable(p_tc);
		
}


