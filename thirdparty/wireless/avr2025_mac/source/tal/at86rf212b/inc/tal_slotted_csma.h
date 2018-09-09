/**
 * @file tal_slotted_csma.h
 *
 * @brief File provides CSMA-CA states.
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

#ifdef BEACON_SUPPORT

/* Prevent double inclusion */
#ifndef TAL_SLOTTED_CSMA_H
#define TAL_SLOTTED_CSMA_H

/**
 * \ingroup group_tal_tx_csma_212b
 * \defgroup group_tal_slotted_csma_212b  Slotted CSMA Module
 * Transmits data using Slotted CSMA/CA mechanism.
 *  @{
 */

/* === INCLUDES ============================================================ */

/* === EXTERNALS =========================================================== */

/* === TYPES =============================================================== */

/*
 * CSMA-CA states
 */
typedef enum csma_state_tag {
	CSMA_IDLE = 0,
	BACKOFF_WAITING_FOR_CCA_TIMER,
	BACKOFF_WAITING_FOR_BEACON,
	CSMA_ACCESS_FAILURE,
	FRAME_SENDING,
	TX_DONE_SUCCESS,
	TX_DONE_FRAME_PENDING,
	TX_DONE_NO_ACK,
	NO_BEACON_TRACKING,
	CSMA_HANDLE_BEACON
} csma_state_t;

/* === MACROS ============================================================== */

/* === PROTOTYPES ========================================================== */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Starts slotted CSMA
 */
bool slotted_csma_start(bool perform_frame_retry);

/**
 * \brief State machine handling slotted CSMA
 */
void slotted_csma_state_handling(void);

#if (MAC_START_REQUEST_CONFIRM == 1)

/**
 * \brief Calculates the entire transaction duration
 */
void calculate_transaction_duration(void);

#endif

#ifdef BEACON_SUPPORT
uint16_t calc_frame_transmit_duration(uint8_t *phy_frame);

#endif /* BEACON_SUPPORT */
/* ! @} */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* TAL_SLOTTED_CSMA_H */

#endif /* BEACON_SUPPORT */

/* EOF */
