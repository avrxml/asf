/**
 * \file
 *
 * \brief Board configuration.
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

#ifndef CONF_BOARD_H_INCLUDED
#define CONF_BOARD_H_INCLUDED

/* Enable Com Port. */
#define CONF_BOARD_COM_PORT

/* Enable audio DAC Port. */
#define CONF_BOARD_ABDACB_PORT
//! \name audio DAC pins
// @{
#define ABDACB_AUDIO0_PIN             PIN_PC09C_ABDACB_DAC0
#define ABDACB_AUDIO0_GPIO            GPIO_PC09C_ABDACB_DAC0
#define ABDACB_AUDIO0_MUX             MUX_PC09C_ABDACB_DAC0
#define ABDACB_AUDIO1_PIN             PIN_PC13C_ABDACB_DAC1
#define ABDACB_AUDIO1_GPIO            GPIO_PC13C_ABDACB_DAC1
#define ABDACB_AUDIO1_MUX             MUX_PC13C_ABDACB_DAC1
#define ABDACB_CLK_PIN                PIN_PC12C_ABDACB_CLK
#define ABDACB_CLK_GPIO               GPIO_PC12C_ABDACB_CLK
#define ABDACB_CLK_MUX                MUX_PC12C_ABDACB_CLK
// @}

/* Configure push button pin as external interrupt */
#define CONF_BOARD_EIC

/* Enable board monitor com port */
//#define CONF_BOARD_BM_USART

/* Backlight: configured to initialize & off to save power */
#define CONF_BOARD_BL

/* USB port: configured to initialize to draw less current on 4L USB port */
#define CONF_BOARD_USB_PORT

#endif /* CONF_BOARD_H_INCLUDED */
