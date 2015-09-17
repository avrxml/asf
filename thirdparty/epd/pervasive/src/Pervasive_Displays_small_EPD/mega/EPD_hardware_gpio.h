/**
* \file
*
* \brief The definition of EPD GPIO pins
*
* Copyright (c) 2012-2014 Pervasive Displays Inc. All rights reserved.
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

#include "Pervasive_Displays_small_EPD.h"

#ifndef DISPLAY_HARDWARE_GPIO_H_INCLUDED
#define DISPLAY_HARDWARE_GPIO_H_INCLUDED

#define	_BV(bit)   (1 << (bit)) /**< left shift 1 bit */
#define	_HIGH      1            /**< signal high */
#define	_LOW       !_HIGH       /**< signal low */

#define	config_gpio_dir_o(Pin)  ioport_set_pin_dir(Pin, IOPORT_DIR_OUTPUT) /**< set output direction for an IOPORT pin */
#define	config_gpio_dir_i(Pin)  ioport_set_pin_dir(Pin, IOPORT_DIR_INPUT)  /**< set input direction for an IOPORT pin */
#define	set_gpio_high(Pin)      ioport_set_pin_level(Pin,_HIGH) /**< set HIGH for an IOPORT pin */
#define	set_gpio_low(Pin)       ioport_set_pin_level(Pin,_LOW)  /**< set LOW for an IOPORT pin */
#define	set_gpio_invert(Pin)    ioport_toggle_pin_level(Pin) /**< toggle the value of an IOPORT pin */
#define	input_get(Pin)          ioport_get_pin_level(Pin)    /**< get current value of an IOPORT pin */

bool EPD_IsBusy(void);
void EPD_cs_high (void);
void EPD_cs_low (void);
void EPD_flash_cs_high(void);
void EPD_flash_cs_low (void);
void EPD_rst_high (void);
void EPD_rst_low (void);
void EPD_discharge_high (void);
void EPD_discharge_low (void);
void EPD_Vcc_turn_off (void);
void EPD_Vcc_turn_on (void);
void EPD_border_high(void);
void EPD_border_low (void);
void EPD_pwm_low (void);
void EPD_pwm_high(void);
void SPIMISO_low(void);
void SPIMOSI_low(void);
void SPICLK_low(void);
void EPD_initialize_gpio(void);

#endif	//DISPLAY_HARDWARE_GPIO_H_INCLUDED


