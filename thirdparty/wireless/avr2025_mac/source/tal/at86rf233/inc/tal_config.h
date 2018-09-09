/**
 * @file tal_config.h
 *
 * @brief File contains TAL configuration parameters.
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

/*
 * Copyright (c) 2013-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

/* Prevent double inclusion */
#ifndef TAL_CONFIG_H
#define TAL_CONFIG_H

/* === INCLUDES ============================================================ */

#include "at86rf233.h"

/* === EXTERNALS =========================================================== */

/* === MACROS ============================================================== */

#define TAL_RADIO_WAKEUP_TIME_SYM       (TAL_CONVERT_US_TO_SYMBOLS( \
	SLEEP_TO_TRX_OFF_TYP_US))

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

#ifdef ENABLE_QUEUE_CAPACITY
#define TAL_INCOMING_FRAME_QUEUE_CAPACITY   (255)
#endif  /* ENABLE_QUEUE_CAPACITY */

/* === PROTOTYPES ========================================================== */

#endif /* TAL_CONFIG_H */
