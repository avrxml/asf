/**
 * \file
 *
 * \brief Board and chip-specific example configuration
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
