/**
 * \file
 *
 * \brief AVR XMEGA XCL (XMEGA Custom Logic) driver.
 *
 * Copyright (C) 2014-2015 Atmel Corporation. All rights reserved.
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

#include <stdint.h>
#include "interrupt.h"
#include "compiler.h"
#include "parts.h"

#include "xcl.h"
#include "sysclk.h"
#include "sleepmgr.h"

#if defined(CONFIG_XCL_CALLBACK_ENABLE) || defined(__DOXYGEN__)

/* ! \internal Local storage of XCL underflow callback function */
static xcl_callback_t xcl_unf_callback;

/**
 * \internal
 * \brief Interrupt handler for XCL underflow
 *
 * This function will handle interrupt on XCL underflow and
 * call xcl underflow callback function.
 *
 * \note
 * This ISR is declared only when CONFIG_XCL_CALLBACK_ENABLE is active in
 * conf_xcl.h.
 */
ISR(XCL_UNF_vect)
{
	Assert(xcl_unf_callback != NULL);
	xcl_unf_callback();
}

/**
 * \brief Set interrupt XCL underflow callback function
 *
 * This function allows the caller to set and change the interrupt callback
 * function. Without setting a callback function the interrupt handler in the
 * driver will only clear the interrupt flags.
 *
 * \note
 * This function is available only when CONFIG_XCL_CALLBACK_ENABLE is
 * active in conf_xcl.h.
 *
 * \param callback Reference to a callback function
 */
void xcl_set_unf_interrupt_callback(xcl_callback_t callback)
{
	xcl_unf_callback = callback;
}

/* ! \internal Local storage of XCL capture/compare callback function */
static xcl_callback_t xcl_cc_callback;

/**
 * \internal
 * \brief Interrupt handler for XCL compare or capture
 *
 * This function will handle interrupt on XCL compare or capture and
 * call the related callback function.
 *
 * \note
 * This ISR is declared only when CONFIG_XCL_CALLBACK_ENABLE is active in
 * conf_xcl.h.
 */
ISR(XCL_CC_vect)
{
	Assert(xcl_cc_callback != NULL);
	xcl_cc_callback();
}

/**
 * \brief Set XCL interrupt capture compare callback function
 *
 * This function allows the caller to set and change the interrupt callback
 * function. Without setting a callback function the interrupt handler in the
 * driver will only clear the interrupt flags.
 *
 * \note
 * This function is available only when CONFIG_XCL_CALLBACK_ENABLE is
 * active in conf_xcl.h.
 *
 * \param callback Reference to a callback function
 */
void xcl_set_ucc_interrupt_callback(xcl_callback_t callback)
{
	xcl_cc_callback = callback;
}

#endif

/* ! XCL requires clock or not */
bool xcl_is_synchronous = false;

/**
 * \brief Configure the XCL TC source clock to achieve a requested resolution
 *
 * This function will configure the XCL TC clock selection register in order to
 * achieve a resolution equal or higher than the one passed as an argument. This
 * function returns the real resulting resolution that has been configured.
 *
 * \param resolution the requested resolution
 * \return The resulting real configured resolution
 */
uint32_t xcl_tc_set_resolution(uint32_t resolution)
{
	uint32_t xcl_clk_rate = sysclk_get_peripheral_bus_hz(&XCL);

	if (resolution <= (xcl_clk_rate / 1024)) {
		xcl_tc_source_clock(DIV1024);
		return xcl_clk_rate / 1024;
	} else if (resolution <= (xcl_clk_rate / 256)) {
		xcl_tc_source_clock(DIV256);
		return xcl_clk_rate / 256;
	} else if (resolution <= (xcl_clk_rate / 64)) {
		xcl_tc_source_clock(DIV64);
		return xcl_clk_rate / 64;
	} else if (resolution <= (xcl_clk_rate / 8)) {
		xcl_tc_source_clock(DIV8);
		return xcl_clk_rate / 8;
	} else if (resolution <= (xcl_clk_rate / 4)) {
		xcl_tc_source_clock(DIV4);
		return xcl_clk_rate / 4;
	} else if (resolution <= (xcl_clk_rate / 2)) {
		xcl_tc_source_clock(DIV2);
		return xcl_clk_rate / 2;
	} else {
		xcl_tc_source_clock(DIV1);
		return xcl_clk_rate;
	}
}
