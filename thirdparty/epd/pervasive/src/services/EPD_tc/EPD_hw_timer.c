/**
 * @file epd_hw_timer.c
 *
 * @brief
 *
 *
 * Copyright (c) 2015-2018 Microchip Technology Inc. and its subsidiaries.
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


