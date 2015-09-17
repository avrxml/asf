/**
 * \file
 *
 * \brief Example configuration
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
#ifndef CONF_EXAMPLE_H
#define CONF_EXAMPLE_H

/**
 * \name Example Settings
 */
//! @{
#define EXAMPLE_USART                 (&AVR32_USART3)
#define EXAMPLE_USART_RX_PIN          AVR32_USART3_RXD_0_0_PIN
#define EXAMPLE_USART_RX_FUNCTION     AVR32_USART3_RXD_0_0_FUNCTION
#define EXAMPLE_USART_TX_PIN          AVR32_USART3_TXD_0_0_PIN
#define EXAMPLE_USART_TX_FUNCTION     AVR32_USART3_TXD_0_0_FUNCTION
#define EXAMPLE_USART_IRQ             AVR32_USART3_IRQ
#define EXAMPLE_USART_BAUDRATE        57600
// DFLL target frequency, in Hz
#define EXAMPLE_TARGET_DFLL_FREQ_HZ   96000000
// MCU clock target frequency, in Hz
#define EXAMPLE_TARGET_MCUCLK_FREQ_HZ 12000000
#undef  EXAMPLE_TARGET_PBACLK_FREQ_HZ
// PBA clock target frequency, in Hz
#define EXAMPLE_TARGET_PBACLK_FREQ_HZ 12000000
	/**
	 * \name Parameters to pcl_configure_clocks().
	*/
	//! @{
	static scif_gclk_opt_t gc_dfllif_ref_opt =
		{ SCIF_GCCTRL_SLOWCLOCK, 0, false };
	static pcl_freq_param_t pcl_dfll_freq_param = {
		.main_clk_src = PCL_MC_DFLL0,
		.cpu_f        = EXAMPLE_TARGET_MCUCLK_FREQ_HZ,
		.pba_f        = EXAMPLE_TARGET_PBACLK_FREQ_HZ,
		.pbb_f        = EXAMPLE_TARGET_PBACLK_FREQ_HZ,
		.dfll_f       = EXAMPLE_TARGET_DFLL_FREQ_HZ,
		.pextra_params = &gc_dfllif_ref_opt
	};
//! @}
#endif /* CONF_EXAMPLE_H */
