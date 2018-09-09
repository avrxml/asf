/**
 * @file
 *
 * @brief
 *
 * Copyright (c) 2013-2018 Microchip Technology Inc. and its subsidiaries.
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

/* Prevent double inclusion */
#ifndef TAL_CONFIG_H
#define TAL_CONFIG_H

/* === INCLUDES ============================================================ */

#include "at86rf232.h"

/* === EXTERNALS =========================================================== */

/* === MACROS ============================================================== */

#define TAL_RADIO_WAKEUP_TIME_SYM       (TAL_CONVERT_US_TO_SYMBOLS( \
	SLEEP_TO_TRX_OFF_TYP_US))
#define TAL_FIRST_TIMER_ID              (0)

#ifndef ANTENNA_DEFAULT
#define ANTENNA_DEFAULT                 (ANT_CTRL_1)
#endif

#ifdef ENABLE_FTN_PLL_CALIBRATION

/*
 * PLL calibration and filter tuning timer timeout in minutes
 */
#define TAL_CALIBRATION_TIMEOUT_MIN         (5UL)

/*
 * PLL calibration and filter tuning timer timeout in us,
 */
#define TAL_CALIBRATION_TIMEOUT_US          ((TAL_CALIBRATION_TIMEOUT_MIN) * \
	(60UL) * (1000UL) * (1000UL))
#endif  /* ENABLE_FTN_PLL_CALIBRATION */

/* === TYPES =============================================================== */

/* Timer ID's used by TAL */
#ifdef BEACON_SUPPORT
/* Beacon Support */
#ifdef ENABLE_FTN_PLL_CALIBRATION
typedef enum tal_timer_id_tag {
	TAL_CSMA_CCA                    = (TAL_FIRST_TIMER_ID),
	TAL_CSMA_BEACON_LOSS_TIMER      = (TAL_FIRST_TIMER_ID + 1),
	TAL_CALIBRATION                 = (TAL_FIRST_TIMER_ID + 2)
} SHORTENUM tal_timer_id_t;

#define NUMBER_OF_TAL_TIMERS        (3)
#else
typedef enum tal_timer_id_tag {
	TAL_CSMA_CCA                    = (TAL_FIRST_TIMER_ID),
	TAL_CSMA_BEACON_LOSS_TIMER      = (TAL_FIRST_TIMER_ID + 1)
} SHORTENUM tal_timer_id_t;

#define NUMBER_OF_TAL_TIMERS        (2)
#endif  /* ENABLE_FTN_PLL_CALIBRATION */
#else /* No BEACON_SUPPORT */
#ifdef ENABLE_FTN_PLL_CALIBRATION
typedef enum tal_timer_id_tag {
	TAL_CALIBRATION                 = (TAL_FIRST_TIMER_ID)
} SHORTENUM tal_timer_id_t;

#define NUMBER_OF_TAL_TIMERS        (1)
#else
#define NUMBER_OF_TAL_TIMERS        (0)
#endif  /* ENABLE_FTN_PLL_CALIBRATION */
#endif  /* BEACON_SUPPORT */

#if (NUMBER_OF_TAL_TIMERS > 0)
#define TAL_LAST_TIMER_ID    (TAL_FIRST_TIMER_ID + NUMBER_OF_TAL_TIMERS - 1) /*
	                                                                      *
	                                                                      *-1:
	                                                                      *
	                                                                      *
	                                                                      *timer
	                                                                      *
	                                                                      *
	                                                                      *id
	                                                                      *
	                                                                      *
	                                                                      *starts
	                                                                      *
	                                                                      *
	                                                                      *with
	                                                                      *
	                                                                      *
	                                                                      *0
	                                                                      **/
#else
#define TAL_LAST_TIMER_ID    (TAL_FIRST_TIMER_ID)
#endif

#ifdef ENABLE_QUEUE_CAPACITY
#define TAL_INCOMING_FRAME_QUEUE_CAPACITY   (255)
#endif  /* ENABLE_QUEUE_CAPACITY */

/* === PROTOTYPES ========================================================== */

#endif /* TAL_CONFIG_H */
