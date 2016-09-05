/**
 * @file tal_timer_config.h
 *
 * @brief File contains TAL timer configuration parameters.
 *
 * Copyright (c) 2013-2015 Atmel Corporation. All rights reserved.
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
 */

/*
 * Copyright (c) 2013-2015 Atmel Corporation. All rights reserved.
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
