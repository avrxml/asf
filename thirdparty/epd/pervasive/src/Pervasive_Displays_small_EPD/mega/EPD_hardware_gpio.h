/**
* \file
*
* \brief The definition of EPD GPIO pins
*
* Copyright (c) 2012-2014 Pervasive Displays Inc. All rights reserved.
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


