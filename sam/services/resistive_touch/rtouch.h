/**
 * \file
 *
 * \brief Resistive Touch Service.
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

#ifndef RTOUCH_H_INCLUDED
#define RTOUCH_H_INCLUDED

#include "compiler.h"
#include "conf_board.h"

#if defined(CONF_BOARD_ADS7843)
#include "rtouch_ads7843.h"
#else
# warning "No supported Touch component found!"
#include "rtouch_ads7843.h"
#endif

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/// @endcond

/**
 * \defgroup sam_services_rtouch_group Resistive Touch Service
 * This service provides an interface for handling resistive touch component.
 *
 * The resistive touch service consists of the following:
 * - Resistive touch service (rtouch.c/h)
 * - Resistive touch component specific implementation (ex. rtouch_ads7843.h)
 * - Resistive touch component controller driver (ex. ads7843.c/h)
 *
 * The resistive touch service provides an general interface to be used by
 * the application or by libraries. It can support various touch component and
 * also provide convenient routines to handle touch screen calibration.
 *
 * @{
 */

/**
 * \brief Point definition.
 */
typedef struct rtouch_point_struct
{
	/*! X coordinate */
	uint16_t x;
	/*! Y coordinate */
	uint16_t y;
} rtouch_point_t;

/**
 * \brief Storage structure for calibration point (actual and raw positions).
 */
typedef struct rtouch_calibration_point_struct
{
	/*! Actual panel coordinates */
	rtouch_point_t panel;
	/*! Raw sample values */
	rtouch_point_t raw;
} rtouch_calibration_point_t;

/**
 * \brief Collection of the five calibration points required to compute
 * calibration.
 */
typedef struct rtouch_calibration_points_struct
{
	rtouch_calibration_point_t point1;
	rtouch_calibration_point_t point2;
	rtouch_calibration_point_t point3;
	rtouch_calibration_point_t point4;
	rtouch_calibration_point_t point5;
} rtouch_calibration_points_t;

/**
 * \brief Touch events enumeration.
 */
typedef enum rtouch_event_type_enum
{
	RTOUCH_PRESS,
	RTOUCH_MOVE,
	RTOUCH_RELEASE
} rtouch_event_type_t;

/**
 * \brief User touch event structure.
 */
typedef struct rtouch_event_struct
{
	/*! Type of event */
	rtouch_event_type_t type;
	/*! Actual panel coordinates */
	rtouch_point_t panel;
	/*! Raw sample values */
	rtouch_point_t raw;
} rtouch_event_t;

typedef void (* rtouch_event_handler_t)(rtouch_event_t const * event);

/**
 * \brief Initialize the resistive touch service.
 *
 * \param ul_width The panel width in pixels.
 * \param ul_height The panel height in pixels.
 *
 * \return 0 on success.
 */
uint32_t rtouch_init(const uint32_t ul_width, const uint32_t ul_height);

/**
 * \brief Wait touch pressed.
 */
void rtouch_wait_pressed(void);

/**
 * \brief Wait touch released.
 */
void rtouch_wait_released(void);

/**
 * \brief Enable the resistive touch service for touch detection and sampling.
 */
void rtouch_enable(void);

/**
 * \brief Disable the resistive touch service. Touch interrupt will be
 * disabled as well.
 */
void rtouch_disable(void);

/**
 * \brief Set calibration parameters when manual calibration procedure
 * is not performed.
 */
void rtouch_set_calibration_parameter(int32_t xslope, int32_t yslope,
								int32_t rawx, int32_t rawy);

/**
 * \brief Perform the calibration process using the provided points.
 *
 * \param points Calibration and raw points.
 *
 * \return 1 if calibration was successful; 0 otherwise.
 */
uint32_t rtouch_compute_calibration(rtouch_calibration_point_t *points);

/**
 * \brief Set the touch event handler.
 * \note This handler is called whenever the position or state of the touch
 * screen changes. The handler is called from the rtouch_process() function.
 *
 * \param handler Pointer to the touch event handler, or NULL to disable
 * the handler.
 */
void rtouch_set_event_handler(rtouch_event_handler_t handler);

/**
 * \brief Get the touch event handler.

 * \note Use this to store the old handler while replacing it for a short
 * while, e.g. for calibration.
 *
 * \return Current touch event handler pointer.
 */
rtouch_event_handler_t rtouch_get_event_handler(void);

/**
 * \brief Core function for resistive touch service.
 *
 * \note An external timer interrupt should call rtouch_process() per 10ms
 * in order to compute the touch state and eventually generate one of the
 * following events: RTOUCH_PRESS or RTOUCH_RELEASE or RTOUCH_MOVE.
 */
void rtouch_process(void);

//@}

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/// @endcond

#endif /* RTOUCH_H_INCLUDED */
