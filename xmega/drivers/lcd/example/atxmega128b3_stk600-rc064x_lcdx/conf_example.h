/**
 * \file
 *
 * \brief Board and chip-specific example configuration
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

#include "compiler.h"
#include "gpio.h"

#define LCD_CLOCK_SOURCE CLK_LCD_SRC_ULP_gc
#define LCD_CONTRAST_LEVEL 30

/*! \name USER LEDs
 */
//! @{
#define USERLEDPORT PORTC
#define USERLEDMASK (0x0F)
#define USERLED0POS PIN0_bp

static inline void user_led_set(uint8_t b)
{
	USERLEDPORT.OUT = ~(b << USERLED0POS);
}

static inline void user_led_toggle(void)
{
	USERLEDPORT.OUTTGL = USERLEDMASK;
}
//! @}

/*! \name Touch
 */
//! @{
#define	TOUCHPORT PORTB
#define	TOUCHMASK (0x0F)
#define	TOUCH0POS PIN0_bp

static inline bool is_touch0(void)
{
	if (TOUCHPORT.IN & (1<<(TOUCH0POS  ))) return false;
	else return true;
}

static inline bool is_touch1(void)
{
	if (TOUCHPORT.IN & (1<<(TOUCH0POS+1))) return false;
	else return true;
}

static inline bool is_touch2(void)
{
	if (TOUCHPORT.IN & (1<<(TOUCH0POS+2))) return false;
	else return true;
}

static inline bool is_touch3(void)
{
	if (TOUCHPORT.IN & (1<<(TOUCH0POS+3))) return false;
	else return true;
}

static inline uint8_t touch_get(void)
{
	return (((~TOUCHPORT.IN) & TOUCHMASK) << TOUCH0POS);
}
//! @}


/*! \name LCD BACKLIGHT
 */
//! @{

// No BACKLIGHT when using STK600_RC064_LCDX

static inline void backlight_on(void)
{

}

static inline void backlight_off(void)
{

}

static inline void backlight_toggle(void)
{

}
//! @}


#endif /* CONF_EXAMPLE_H */
