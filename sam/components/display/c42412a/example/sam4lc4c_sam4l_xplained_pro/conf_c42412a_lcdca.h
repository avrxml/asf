/**
 * \file
 *
 * \brief C42412A configuration.
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

/** Configuration of the C42412A LCD glass driver */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef CONF_C42412A_LCDCA_H_INCLUDED
#define CONF_C42412A_LCDCA_H_INCLUDED

/** Bias configuration. */
#define CONF_C42412A_X_BIAS    false

/** Init contrast configuration. */
#define CONF_C42412A_CONTRAST  30

/**
 * \name Init timing definition
 * @{
 */
/** Clock prescaler configuration. */
#define CONF_C42412A_PRES    false
/** Clock divider configuration. */
#define CONF_C42412A_CLKDIV  7
/** Frame count 0 configuration. */
#define CONF_C42412A_FC0     2
/** Frame count 1 configuration. */
#define CONF_C42412A_FC1     2
/** Frame count 1 configuration. */
#define CONF_C42412A_FC2     1
/** @} */

/** Blink timer configuration. */
#define CONF_C42412A_BLINK_TIMER                 LCDCA_TIMER_FC0

/** Circular animation configuration. */
#define CONF_C42412A_CIRCULAR_ANIMATION_TIMER    LCDCA_TIMER_FC0

/** Text scrolling configuration. */
#define CONF_C42412A_TEXT_SCROLLING_TIMER        LCDCA_TIMER_FC0

/**
 * \name C42412A LCD Glass Common and Segment number
 * @{
 */
#define C42412A_NB_OF_COM 4
#define C42412A_NB_OF_SEG 24
/** @} */

/**
 * \name C42412A LCD Glass Connection Initialization
 * @{
 */
#define C42412A_PORT_MASK C42412A_NB_OF_SEG
#define C42412A_LCD_DUTY  (C42412A_NB_OF_COM % 4)
/** @} */

/**
 * \name Digit Decoder
 * @{
 */
/** 14-segment with 4 commons terminals */
#define C42412A_FIRST_14SEG_4C  4
#define C42412A_WIDTH_14SEG_4C  5
/** @} */

#endif /* CONF_C42412A_LCDCA_H_INCLUDED */
