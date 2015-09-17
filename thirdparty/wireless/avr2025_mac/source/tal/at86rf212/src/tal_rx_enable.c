/**
 * @file tal_rx_enable.c
 *
 * @brief File provides functionality supporting RX-Enable feature.
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

/* === INCLUDES ============================================================ */

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "pal.h"
#include "return_val.h"
#include "tal_tx.h"
#include "tal.h"
#include "ieee_const.h"
#include "tal_constants.h"
#include "at86rf212.h"
#include "bmm.h"
#include "qmm.h"
#include "tal_rx.h"
#include "tal_internal.h"

/* === TYPES =============================================================== */

/* === MACROS ============================================================== */

/* === GLOBALS ============================================================= */

/* === PROTOTYPES ========================================================== */

/* === IMPLEMENTATION ====================================================== */

/*
 * \brief Switches receiver on or off
 *
 * This function switches the receiver on (PHY_RX_ON) or off (PHY_TRX_OFF).
 *
 * \param state New state of receiver
 *
 * \return TAL_BUSY if the TAL state machine cannot switch receiver on or off,
 *         TRX_OFF if receiver has been switched off, or
 *         RX_ON otherwise.
 *
 */
uint8_t tal_rx_enable(uint8_t state)
{
	/*
	 * Trx can only be enabled if TAL is not busy;
	 * i.e. if TAL is IDLE.
	 */
	if (TAL_IDLE != tal_state) {
#ifdef BEACON_SUPPORT
		if (tal_state != TAL_SLOTTED_CSMA) {
			return TAL_BUSY;
		}

#else
		return TAL_BUSY;
#endif
	}

	if (state == PHY_TRX_OFF) {
		/*
		 * If the rx needs to be switched off, we are not interested in
		 * a frame
		 * that is currently being received.
		 * This must not be a Forced TRX_OFF (CMD_FORCED_TRX_OFF) since
		 * this could
		 * corrupt an already outoing ACK frame.
		 */
		set_trx_state(CMD_TRX_OFF);
		tal_rx_on_required = false;
		return TRX_OFF;
	} else {
#ifdef SNIFFER
		set_trx_state(CMD_RX_ON);
#else   /* #ifndef SNIFFER */
		if (NULL != tal_rx_buffer) {
#ifdef PROMISCUOUS_MODE
			if (tal_pib.PromiscuousMode) {
				set_trx_state(CMD_RX_ON);
			} else {
				set_trx_state(CMD_RX_AACK_ON);
			}

#else   /* Normal operation */
			set_trx_state(CMD_RX_AACK_ON);
#endif  /* PROMISCUOUS_MODE */
		} else {
			/*
			 * If no rx buffer is available, the corresponding
			 * information is stored and will be used by tal_task()
			 * to
			 * switch on the receiver later.
			 *
			 * Even if a receive buffer is not available,
			 * the TAL returns MAC_SUCCESS. The TAL will try to
			 * allocate a receive
			 * buffer as soon as possible and will switch on the
			 * receiver.
			 */
			tal_rx_on_required = true;
		}
#endif  /* #ifndef SNIFFER */
		return RX_ON; /* MAC layer assumes RX_ON as return value */
	}
}

/* EOF */
