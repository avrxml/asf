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
#ifndef TAL_TX_H
#define TAL_TX_H

/* === INCLUDES ============================================================ */

#include "tal.h"

/* === EXTERNALS =========================================================== */

/* === TYPES =============================================================== */

/* === MACROS ============================================================== */

/* === PROTOTYPES ========================================================== */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup group_tal_tx
 * @{
 */

/**
 * \brief Handles interrupts issued due to end of transmission
 *
 * \param underrun_occured  true if under-run has occurred
 */

void handle_tx_end_irq(bool underrun_occured);

/**
 * \brief Sends frame using trx features to handle CSMA and re-transmissions
 *
 * \param csma_mode Indicates the CSMA mode used
 * \param tx_retries Flag indicating if transmission retries are requested
 *                   by the MAC layer
 */

void send_frame(csma_mode_t csma_mode, bool tx_retries);

/**
 * \brief Implements the handling of the transmission end.
 *
 * This function handles the callback for the transmission end.
 */
void tx_done_handling(void);

/* ! @} */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* TAL_TX_H */

/* EOF */
