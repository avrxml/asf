/**
 * \file
 *
 * \brief Application-specific system clock management to support automatic
 * detection of external oscillator
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

#include "preprocessor.h"
#include "compiler.h"
#include "board.h"
#include "pm_uc3l.h"
#include "scif_uc3l.h"
#include "flashcdw.h"
#include "ast.h"
#include "cycle_counter.h"

#include "sysclk_auto.h"
#include "freq_detect.h"
#include "gpio.h"

// Declare local "u_avr32_scif_oscctrl0_t" definition to give access
// to "oscctrl0" register with a 32-bits access or bitfield access
typedef union
{
  unsigned long                 oscctrl0;
  avr32_scif_oscctrl0_t         OSCCTRL0;
} u_avr32_scif_oscctrl0_t;


/*! \name External routines required by main.c file and usbc driver
 */
//! \{

/**
 * \brief  Detects extern OSC frequency and initialize system clocks on it
 */
void sysclk_auto_init(void)
{
	int mul;

	// Switch to OSC ISP
	// Set max startup time to make sure any crystal will be supported
	// We cannot use a TC to measure this OSC frequency
	// because the master clock must be faster than the clock selected by the TC

	// Configure OSC0 in crystal mode, external crystal with a fcrystal Hz frequency.

	// Replace "scif_configure_osc_crystalmode(SCIF_OSC0, 16000000)" by
	// inline routine to safe code (160B)
	{
		u_avr32_scif_oscctrl0_t   u_avr32_scif_oscctrl0 = {AVR32_SCIF.oscctrl0};

		// Modify : Configure the oscillator mode to crystal and set the gain according to the
		// crystal frequency.
		u_avr32_scif_oscctrl0.OSCCTRL0.mode = SCIF_OSC_MODE_2PIN_CRYSTAL;
		u_avr32_scif_oscctrl0.OSCCTRL0.gain =
			(16000000 <  900000) ? AVR32_SCIF_OSCCTRL0_GAIN_G0 :
			(16000000 < 3000000) ? AVR32_SCIF_OSCCTRL0_GAIN_G1 :
			(16000000 < 8000000) ? AVR32_SCIF_OSCCTRL0_GAIN_G2 :
										  AVR32_SCIF_OSCCTRL0_GAIN_G3;
		AVR32_ENTER_CRITICAL_REGION( );
		// Unlock the write-protected OSCCTRL0 register
		SCIF_UNLOCK(AVR32_SCIF_OSCCTRL0);
		// Write Back
		AVR32_SCIF.oscctrl0 = u_avr32_scif_oscctrl0.oscctrl0;
		AVR32_LEAVE_CRITICAL_REGION( );
	}

	// Enable the OSC0
	scif_enable_osc(SCIF_OSC0, AVR32_SCIF_OSCCTRL0_STARTUP_16384_RCOSC, true);
	pm_set_mclk_source(PM_CLK_SRC_OSC0);

	// Initialize the AST with the internal RC oscillator
	// AST will count at the frequency of 115KHz/2
	if (!ast_init_counter(&AVR32_AST, AST_OSC_RC, 0, 0)) {
		while (1);
	}
	// Enable the AST
	ast_enable(&AVR32_AST);

	// Detect the frequency
	switch (freq_detect_start()) {
	case 8000000:
		mul = 11;
		break;
	case 16000000:
		mul = 5;
		break;
	case 12000000:
	default:
		mul = 7;
		break;
	}

	scif_pll_opt_t opt;

	// Set PLL0 VCO @ 96 MHz
	// Set PLL0 @ 48 MHz
	opt.osc = SCIF_OSC0;
	opt.lockcount = 63;
	opt.div = 1;
	opt.mul = mul;
	opt.pll_div2 = 1;
	opt.pll_wbwdisable = 0;
	opt.pll_freq = 1;

	// lockcount in main clock for the PLL wait lock
	scif_pll0_setup((const scif_pll_opt_t *)&opt);

	/* Enable PLL0 */
	scif_pll0_enable();

	/* Wait for PLL0 locked */
	scif_wait_for_pll0_locked();

	// Configure and start the RC120Mhz internal oscillator.
	scif_start_rc120M();

	// Since our target is to set the CPU&HSB frequency domains to 30MHz, we must
	// set one wait-state and enable the High-speed read mode on the flash controller.
	flashcdw_set_flash_waitstate_and_readmode(30000000UL);

	// Set the CPU clock domain to 30MHz (by applying a division ratio = 4).
	pm_set_clk_domain_div((pm_clk_domain_t)AVR32_PM_CLK_GRP_CPU, PM_CKSEL_DIVRATIO_4);

	// Set the PBA clock domain to 30MHz (by applying a division ratio = 4).
	pm_set_clk_domain_div((pm_clk_domain_t)AVR32_PM_CLK_GRP_PBA, PM_CKSEL_DIVRATIO_4);

	// Set the PBB clock domain to 30MHz (by applying a division ratio = 4).
	pm_set_clk_domain_div((pm_clk_domain_t)AVR32_PM_CLK_GRP_PBB, PM_CKSEL_DIVRATIO_4);

	// Set the main clock source to be DFLL0.
	pm_set_mclk_source(PM_CLK_SRC_RC120M);
}


/**
 * \brief Enable the USB generic clock
 *
 * \pre The USB generic clock must be configured to 48MHz.
 */
void sysclk_auto_enable_usb(void)
{
	// Setup USB GCLK
	scif_gc_setup(AVR32_USBC_GCLK_NUM,
			SCIF_GCCTRL_PLL0, AVR32_SCIF_DIVEN_NO_DIV_CLOCK, 0);
	// Enable USB GCLK
	scif_gc_enable(AVR32_USBC_GCLK_NUM);

}


/**
 * \brief Disable the USB generic clock
 */
void sysclk_auto_disable_usb(void)
{
	AVR32_SCIF.gcctrl[AVR32_USBC_GCLK_NUM] = 0;
	scif_gc_setup(AVR32_USBC_GCLK_NUM,
			SCIF_GCCTRL_PLL0, AVR32_SCIF_DIVEN_NO_DIV_CLOCK, 0);
}
//! \}

/*! \name External routines required by frequency detection service
 */
//! \{

//! Callback used to reset the counter of the reference's oscillator.
void freq_detect_iface_ref_cnt_reset(void)
{
	ast_set_counter_value(&AVR32_AST, 0);
}

//! Callback used to read the counter value of the reference's oscillator.
int freq_detect_iface_ref_cnt_value(void)
{
	return ast_get_counter_value(&AVR32_AST) * 2;
}

//! Callback used to reset the counter of the target oscillator.
void freq_detect_iface_target_cnt_reset(void)
{
	Set_sys_count(0);
}

//! Callback used to read the counter value of the target oscillator.
int freq_detetc_iface_target_cnt_value(void)
{
	return Get_sys_count();
}
//! \}
