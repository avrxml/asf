/**
 * \file
 *
 * \brief API driver for component ADS7843.
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

#ifndef ADS7843_H_INCLUDED
#define ADS7843_H_INCLUDED

#include "compiler.h"

/** @cond 0*/
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/** @endcond*/

/**
 * \defgroup sam_component_ads7843_group Resistive Touch - ADS7843 Controller
 *
 * Low-level driver for the ADS7843 touch controller. This driver provides
 * access to the main features of the ADS7843 controller.
 *
 * \{
 */

/**
 * \brief Return the touch screen status, pressed or not.
 *
 * \return 1 if the touchscreen is pressed, 0 otherwise.
 */
uint32_t ads7843_is_pressed(void);

/**
 * \brief Set the touch interrupt handler.
 *
 * \note This handler will be called whenever a touch event is detected by the
 * ADS7843 controller.
 *
 * \param p_handler Interrupt handler function pointer.
 */
void ads7843_set_handler(void (*p_handler) (uint32_t, uint32_t));

/**
 * \brief Enable interrupts on touch event.
 */
void ads7843_enable_interrupt(void);

/**
 * \brief Disable interrupts on touch event.
 */
void ads7843_disable_interrupt(void);

/**
 * \brief Get the touch raw coordinates.
 *
 * \param p_x Pointer to an integer representing the X value.
 * \param p_y Pointer to an integer representing the Y value.
 */
void ads7843_get_raw_point(uint32_t *p_x, uint32_t *p_y);

/**
 * \brief Initialize the SPI communication with the ADS7843 controller.
 */
uint32_t ads7843_init(void);

/**@}*/

/** @cond 0*/
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/** @endcond*/

#endif /* ADS7843_H_INCLUDED */
