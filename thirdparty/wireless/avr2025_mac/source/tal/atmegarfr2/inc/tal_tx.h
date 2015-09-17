/**
 * \file tal_tx.h
 *
 * \brief File contains the TAL sub-state macros and functions that
 * perform frame transmission.
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
 *
 */

/*
 * Copyright (c) 2013-2015 Atmel Corporation. All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
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
 * \addtogroup group_tal_tx_rfr2
 * @{
 */

/**
 * \brief Handles interrupts issued due to end of transmission
 *
 * \param underrun_occured  true if under-run has occurred
 */

void handle_tx_end_irq(void);

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
