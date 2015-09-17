/**
 * \file
 *
 * \brief Resistive touch service settings for the ADS7843 controller.
 *
 * Copyright (c) 2011-2015 Atmel Corporation. All rights reserved.
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

#ifndef RTOUCH_ADS7843_H_INCLUDED
#define RTOUCH_ADS7843_H_INCLUDED

#include "ads7843.h"

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/// @endcond

/**
 * \ingroup sam_services_rtouch_group
 * \defgroup sam_services_rtouch_ads7843_group ADS7843 Touch Controller
 *
 * This is hardware specific configuration that configures the resistive touch 
 * service for use with the ADS7843 touch controller. All the functionalities 
 * in this configuration are provided by the \ref sam_component_ads7843_group 
 * driver.
 *
 * @{
 */

/**
 * ADS7843 touch driver specific function, see
 * \ref ads7843_is_pressed
 */
#define rtouch_is_pressed()                    ads7843_is_pressed()

/**
 * ADS7843 touch driver specific function, see
 * \ref ads7843_set_handler
 */
#define rtouch_set_handler(p_handler)          ads7843_set_handler(p_handler)

/**
 * ADS7843 touch driver specific function, see
 * \ref ads7843_enable_interrupt
 */
#define rtouch_enable_interrupt()              ads7843_enable_interrupt()

/**
 * ADS7843 touch driver specific function, see
 * \ref ads7843_disable_interrupt
 */
#define rtouch_disable_interrupt()             ads7843_disable_interrupt()

/**
 * ADS7843 touch driver specific function, see
 * \ref ads7843_get_raw_point
 */
#define rtouch_get_raw_point(p_x, p_y)         ads7843_get_raw_point(p_x, p_y)

/**
 * ADS7843 touch driver specific function, see
 * \ref ads7843_init
 */
#define rtouch_init_device()                   ads7843_init()

//@}

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/// @endcond

#endif /* RTOUCH_ADS7843_H_INCLUDED */
