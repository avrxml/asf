/**
 * \file
 *
 * \brief Example configuration
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
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
