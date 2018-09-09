/**
 * \file
 *
 * \brief SAM L21 External Interrupt Driver Configuration Header
 *
 * Copyright (c) 2017-2018 Microchip Technology Inc. and its subsidiaries.
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
#ifndef CONF_EXTINT_H_INCLUDED
#define CONF_EXTINT_H_INCLUDED


#include <asf.h>

#  define EXTINT_CLOCK_SELECTION   EXTINT_CLK_GCLK
#  define EXTINT_CLOCK_SOURCE      GCLK_GENERATOR_0

void button_cb(void);

/* Button Initialize */
static inline void button_init(void)
{
	struct extint_chan_conf eint_chan_conf;
	extint_chan_get_config_defaults(&eint_chan_conf);

	eint_chan_conf.gpio_pin           = SW0_EIC_PIN;
	eint_chan_conf.gpio_pin_pull      = EXTINT_PULL_UP;
	eint_chan_conf.gpio_pin_mux       = SW0_EIC_PINMUX;
	eint_chan_conf.detection_criteria = EXTINT_DETECT_FALLING;
	eint_chan_conf.filter_input_signal = true;
	extint_chan_set_config(SW0_EIC_LINE, &eint_chan_conf);
	
	extint_register_callback(button_cb,
							SW0_EIC_LINE,
							EXTINT_CALLBACK_TYPE_DETECT);
	
	extint_chan_enable_callback(SW0_EIC_LINE,
							EXTINT_CALLBACK_TYPE_DETECT);
}

/**
 * \brief Read the current state of the button pin
 *
 */
static inline uint8_t button_0_input_level(void) 
{
	return port_pin_get_input_level(BUTTON_0_PIN);
}

#endif
