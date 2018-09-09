/*****************************************************************************
*
* \file
*
* \brief Resistive Touch driver
*
 * Copyright (c) 2009-2018 Microchip Technology Inc. and its subsidiaries.
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
*****************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */
#ifndef _RTOUCH_H_
#define _RTOUCH_H_

/**
 * \defgroup group_avr32_components_touch_resistive_touch TOUCH - Resistive Touch
 *
 * This driver offers resistive touch measurements by using the on-chip ADC.
 * In addition it offers calibration and a callback function for the
 * application.
 *
 * To improve the overall results four ADC channels are used to measure the
 * touch position. That means the measurements are done from each direction of
 * the touch surface.
 *
 * @{
 */

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Info for one calibration point, raw samples and actual panel position. */
typedef struct rtouch_calibration_point_struct {
	uint16_t panelX; /**< Actual panel X coordinate. */
	uint16_t panelY; /**< Actual panel Y coordinate. */
	uint16_t rawX; /**< Raw X sample value. */
	uint16_t rawY; /**< Raw Y sample value. */
} rtouch_calibration_point_t;

/** Collection of the three calibration points required to compute
 *  calibration. */
typedef struct rtouch_calibration_points_struct {
	rtouch_calibration_point_t point1;
	rtouch_calibration_point_t point2;
	rtouch_calibration_point_t point3;
} rtouch_calibration_points_t;

/** Calibration matrix coefficients, computed from three calibration points. */
typedef struct rtouch_calibration_matrix_struct {
	int32_t A;
	int32_t B;
	int32_t C;
	int32_t D;
	int32_t E;
	int32_t F;
	int32_t K;
} rtouch_calibration_matrix_t;

/** Event type used by event struct \ref rtouch_event_t. */
typedef enum rtouch_event_type_enum {
	RTOUCH_NO_EVENT,
	RTOUCH_PRESS,
	RTOUCH_MOVE,
	RTOUCH_RELEASE
} rtouch_event_type_t;

/** User-friendly touch event structure. */
typedef struct rtouch_event_struct {
	rtouch_event_type_t type;
	int16_t rawX; /**< Raw X sample. */
	int16_t rawY; /**< Raw Y sample. */
	int16_t panelX; /**< Calibrated panel X position. */
	int16_t panelY; /**< Calibrated panel Y position. */
} rtouch_event_t;

/** Type definition for a user touch event callback function. */
typedef void (*rtouch_event_handler_t)( rtouch_event_t const *event );

void rtouch_init(void);
void rtouch_disable(void);
void rtouch_enable(void);

void rtouch_get_event(rtouch_event_t *event);
bool rtouch_is_touched(void);

void rtouch_compute_calibration_matrix(
		rtouch_calibration_points_t const *points,
		rtouch_calibration_matrix_t *matrix );

void rtouch_set_calibration_matrix(rtouch_calibration_matrix_t const *source);
void rtouch_get_calibration_matrix(rtouch_calibration_matrix_t *destination);

void rtouch_set_event_handler(rtouch_event_handler_t handler);
rtouch_event_handler_t rtouch_get_event_handler(void);

#ifdef __cplusplus
}
#endif

/** @} */

#endif
