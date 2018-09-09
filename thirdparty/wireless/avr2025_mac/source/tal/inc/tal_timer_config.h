/**
 * @file tal_timer_config.h
 *
 * @brief File contains TAL timer configuration parameters.
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
#ifndef TAL_TIMER_CONFIG_H
#define TAL_TIMER_CONFIG_H

/* === INCLUDES ============================================================ */
#include "tal_config.h"
#include "return_val.h"
/* === EXTERNALS =========================================================== */

/* === MACROS ============================================================== */

/* === TYPES =============================================================== */
#if ((TAL_TYPE == AT86RF231) || (TAL_TYPE == AT86RF212) || (TAL_TYPE ==	\
	ATMEGARFA1) || \
	(TAL_TYPE == ATMEGARFR2) || (TAL_TYPE == AT86RF233) || (TAL_TYPE == \
	AT86RF212B))

/* Timer ID's used by TAL */
#ifdef BEACON_SUPPORT
/* Beacon Support */
#ifdef ENABLE_FTN_PLL_CALIBRATION
extern uint8_t TAL_CSMA_CCA;
extern uint8_t TAL_CSMA_BEACON_LOSS_TIMER;
extern uint8_t TAL_CALIBRATION;
#ifdef SW_CONTROLLED_CSMA
extern uint8_t TAL_T_BOFF;
#endif

#ifdef SW_CONTROLLED_CSMA
#define NUMBER_OF_TAL_TIMERS        (4)
#else
#define NUMBER_OF_TAL_TIMERS        (3)
#endif

#else /* FTN_PLL_CALIBRATION DISABLED */
extern uint8_t TAL_CSMA_CCA;
extern uint8_t TAL_CSMA_BEACON_LOSS_TIMER;
#ifdef SW_CONTROLLED_CSMA
extern uint8_t TAL_T_BOFF;
#endif

#ifdef SW_CONTROLLED_CSMA
#define NUMBER_OF_TAL_TIMERS        (3)
#else
#define NUMBER_OF_TAL_TIMERS        (2)
#endif
#endif  /* ENABLE_FTN_PLL_CALIBRATION */
#else /* No BEACON_SUPPORT */
#ifdef ENABLE_FTN_PLL_CALIBRATION
extern uint8_t TAL_CALIBRATION;
#ifdef SW_CONTROLLED_CSMA
extern uint8_t TAL_T_BOFF;
#endif

#ifdef SW_CONTROLLED_CSMA
#define NUMBER_OF_TAL_TIMERS        (2)
#else
#define NUMBER_OF_TAL_TIMERS        (1)
#endif
#else
#ifdef SW_CONTROLLED_CSMA
extern uint8_t TAL_T_BOFF;
#endif
#ifdef SW_CONTROLLED_CSMA
#define NUMBER_OF_TAL_TIMERS        (1)
#else
#define NUMBER_OF_TAL_TIMERS        (0)
#endif
#endif  /* ENABLE_FTN_PLL_CALIBRATION */
#endif  /* BEACON_SUPPORT */

#elif  (TAL_TYPE == AT86RF230B)
#include "mac_build_config.h"

#if ((MAC_SCAN_ED_REQUEST_CONFIRM == 1) && (defined BEACON_SUPPORT))
#ifdef ENABLE_FTN_PLL_CALIBRATION
extern uint8_t TAL_ACK_WAIT_TIMER;
extern uint8_t TAL_ED_SCAN_DURATION_TIMER;
extern uint8_t TAL_ED_SAMPLE_TIMER;
extern uint8_t TAL_CSMA_CCA;
extern uint8_t TAL_CSMA_BEACON_LOSS_TIMER;
extern uint8_t TAL_CALIBRATION;

#define NUMBER_OF_TAL_TIMERS        (6)
#else
extern uint8_t TAL_ACK_WAIT_TIMER;
extern uint8_t TAL_ED_SCAN_DURATION_TIMER;
extern uint8_t TAL_ED_SAMPLE_TIMER;
extern uint8_t TAL_CSMA_CCA;
extern uint8_t TAL_CSMA_BEACON_LOSS_TIMER;

#define NUMBER_OF_TAL_TIMERS        (5)
#endif  /* ENABLE_FTN_PLL_CALIBRATION */
#endif

/* ED Scan is switched on */
/* No beacon support */
#if ((MAC_SCAN_ED_REQUEST_CONFIRM == 1) && (!defined BEACON_SUPPORT))
#ifdef ENABLE_FTN_PLL_CALIBRATION
extern uint8_t TAL_ED_SCAN_DURATION_TIMER;
extern uint8_t TAL_ED_SAMPLE_TIMER;
extern uint8_t TAL_CALIBRATION;

#define NUMBER_OF_TAL_TIMERS        (3)
#else
extern uint8_t TAL_ED_SCAN_DURATION_TIMER;
extern uint8_t TAL_ED_SAMPLE_TIMER;

#define NUMBER_OF_TAL_TIMERS        (2)
#endif  /* ENABLE_FTN_PLL_CALIBRATION */
#endif

/* ED Scan is switched off */
/* Network with beacon support */
#if ((MAC_SCAN_ED_REQUEST_CONFIRM == 0) && (defined BEACON_SUPPORT))
#ifdef ENABLE_FTN_PLL_CALIBRATION
extern uint8_t TAL_ACK_WAIT_TIMER;
extern uint8_t TAL_CSMA_CCA;
extern uint8_t TAL_CSMA_BEACON_LOSS_TIMER;
extern uint8_t TAL_CALIBRATION;

#define NUMBER_OF_TAL_TIMERS        (4)
#else
extern uint8_t TAL_ACK_WAIT_TIMER;
extern uint8_t TAL_CSMA_CCA;
extern uint8_t TAL_CSMA_BEACON_LOSS_TIMER;

#define NUMBER_OF_TAL_TIMERS        (3)
#endif  /* ENABLE_FTN_PLL_CALIBRATION */
#endif

/* ED Scan is switched off */
/* No beacon support */
#if ((MAC_SCAN_ED_REQUEST_CONFIRM == 0) && (!defined BEACON_SUPPORT))
#ifdef ENABLE_FTN_PLL_CALIBRATION
extern uint8_t TAL_CALIBRATION;

#define NUMBER_OF_TAL_TIMERS        (1)
#else
#define NUMBER_OF_TAL_TIMERS        (0)
#endif  /* ENABLE_FTN_PLL_CALIBRATION */
#endif
#endif

#if (TAL_TYPE == AT86RF215)
extern uint8_t TAL_T_0;
extern uint8_t TAL_T_1;

#ifdef ENABLE_FTN_PLL_CALIBRATION
extern uint8_t TAL_T_CALIBRATION_0;
extern uint8_t TAL_T_CALIBRATION_1;
#endif

#if ((defined RF215v1) && (defined SUPPORT_LEGACY_OQPSK))

extern uint8_t TAL_T_AGC_0;
extern uint8_t TAL_T_AGC_1;

#endif

#if ((defined RF215v1) && (defined SUPPORT_LEGACY_OQPSK))
#ifdef ENABLE_FTN_PLL_CALIBRATION
#define NUMBER_OF_TAL_TIMERS        (6)
#else
#define NUMBER_OF_TAL_TIMERS        (4)
#endif /* ENABLE_FTN_PLL_CALIBRATION */
#else
#ifdef ENABLE_FTN_PLL_CALIBRATION
#define NUMBER_OF_TAL_TIMERS        (4)
#else
#define NUMBER_OF_TAL_TIMERS        (2)
#endif /* ENABLE_FTN_PLL_CALIBRATION */
#endif
#endif
/* === PROTOTYPES ========================================================== */

#endif /* TAL_TIMER_CONFIG_H */
/* EOF */
