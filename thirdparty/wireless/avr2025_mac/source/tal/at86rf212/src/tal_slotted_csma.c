/**
 * @file tal_slotted_csma.c
 *
 * @brief This file implements slotted CSMA-CA functions.
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

#ifdef BEACON_SUPPORT

/* === INCLUDES ============================================================ */

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "pal.h"
#include "return_val.h"
#include "tal.h"
#include "ieee_const.h"
#include "tal_constants.h"
#include "tal_slotted_csma.h"
#include "tal_pib.h"
#include "at86rf212.h"
#include "tal_internal.h"
#include "tal_tx.h"
#include "tal_irq_handler.h"
#include "mac_build_config.h"

/* === TYPES =============================================================== */

/* === MACROS ============================================================== */

/**
 * *\addtogroup group_tal_slotted_csma_212
 * @{
 */

/*
 * Conversion of octets to microseconds
 */
#define CCA_GUARD_DURATION_US           (1000)  /* used for calculation overhead
	                                        **/
#define CSMA_BEACON_LOSS_GUARD_TIME_US  (2000)
#define PRE_BEACON_GUARD_TIME_US        (1000)

/*
 * Mask used to check if the frame is an Acknowledgement frame
 */
#define ACK_FRAME                       (0x02)

/*
 * Defines the length of an acknowledgement frame
 */
#define ACK_FRAME_LEN                   (0x05)

/* === GLOBALS ============================================================= */

static uint8_t NB;
static uint8_t BE;
#if (MAC_START_REQUEST_CONFIRM == 1)
uint8_t transaction_duration_periods;
#else
static uint8_t transaction_duration_periods;
#endif
static uint8_t remaining_backoff_periods;
static uint8_t number_of_tx_retries;
static uint32_t cca_starttime_us;

/* === PROTOTYPES ========================================================== */

static bool check_beacon_reception(void);
static inline void csma_param_init(void);

#if (MAC_START_REQUEST_CONFIRM == 0)
static void calculate_transaction_duration(void);

#endif
static void csma_backoff_calculation(void);
static void cca_timer_handler_cb(void *parameter);
static uint8_t perform_cca_twice(void);
static void send_frame_at_next_backoff_boundary(void);
static void start_beacon_loss_timer(void);
static void beacon_loss_timer_cb(void *parameter);
static void tx_done(retval_t status);

/* ! @} */
/* === IMPLEMENTATION ====================================================== */

/*
 * \brief Starts slotted CSMA
 */
bool slotted_csma_start(bool perform_frame_retry)
{
	PIN_CSMA_START(); /* debug pin to switch on: define ENABLE_DEBUG_PINS,
	                   * pal_config.h */

	bool return_val = false; /* Assume error case */

	if (check_beacon_reception()) {
		tal_state = TAL_SLOTTED_CSMA;

		csma_param_init();
		if (perform_frame_retry) {
			number_of_tx_retries = 0;
		} else {
			/* Use the max value to indicate that no retries are
			 * required. */
			number_of_tx_retries = tal_pib.MaxFrameRetries;
		}

		calculate_transaction_duration();

		/* Get a random backoff period duration. */
		remaining_backoff_periods = (uint8_t)(rand() & ((1 << BE) - 1));

		csma_backoff_calculation();

		return_val = true;
	}

	return return_val;
}

/**
 * \brief Checks if node is receiving beacons
 *
 * \return  true: beacons are received otherwise false
 */
static bool check_beacon_reception(void)
{
	uint32_t time_between_beacons_sym;
	uint32_t next_beacon_time_sym;
	uint8_t number_of_lost_beacon = 0;
	uint32_t now_time;

	time_between_beacons_sym = TAL_GET_BEACON_INTERVAL_TIME(
			tal_pib.BeaconOrder);
	next_beacon_time_sym = tal_add_time_symbols(tal_pib.BeaconTxTime,
			time_between_beacons_sym);

	pal_get_current_time(&now_time);
	now_time = TAL_CONVERT_US_TO_SYMBOLS(now_time);

	/* If the last beacon was not received, calculate/estimate the next
	 * beacon time */
	while (next_beacon_time_sym < now_time) {
		tal_pib.BeaconTxTime = next_beacon_time_sym;
		next_beacon_time_sym = tal_add_time_symbols(
				tal_pib.BeaconTxTime,
				time_between_beacons_sym);
		number_of_lost_beacon++;
		if (number_of_lost_beacon == aMaxLostBeacons) { /* sync loss */
			return false;
		}
	}

	return true;
}

/**
 * \brief Initializes CSMA variables
 */
static inline void csma_param_init(void)
{
	NB = 0;

	if (tal_pib.BattLifeExt) {
		if (tal_pib.MinBE > 2) {
			BE = 2;
		} else {
			BE = tal_pib.MinBE;
		}
	} else {
		BE = tal_pib.MinBE;
	}
}

/**
 * \brief Calculates the entire transaction duration
 */
#if (MAC_START_REQUEST_CONFIRM == 1)
void calculate_transaction_duration(void)
#else
static void calculate_transaction_duration(void)
#endif
{
	uint8_t transaction_duration_octets;
	uint16_t transaction_duration_sym;

	/* number of octets */
	transaction_duration_octets = (*tal_frame_to_tx) + PHY_OVERHEAD;

	/* Add interframe spacing - independend on ACK transmission. */
	if (*tal_frame_to_tx > aMaxSIFSFrameSize) {
		transaction_duration_sym = macMinLIFSPeriod_def; /* symbols */
	} else {
		transaction_duration_sym = macMinSIFSPeriod_def; /* symbols */
	}

	/* If the frame requested an ACK, add ACK handling time. */
	if (mac_frame_ptr->mpdu[PL_POS_FCF_1] & FCF_ACK_REQUEST) {
		/* Ensure there is room for the ACK. */
		transaction_duration_octets += ACK_FRAME_LEN + PHY_OVERHEAD; /*
		                                                              *
		                                                              *
		                                                              *
		                                                              *octets
		                                                              **/

		/* Space is needed until the ACK is sent. */
		transaction_duration_sym += aTurnaroundTime +
				aUnitBackoffPeriod;                       /*
		                                                           *
		                                                           *
		                                                           *
		                                                           *symbols
		                                                           **/
	}

	transaction_duration_sym += transaction_duration_octets *
			SYMBOLS_PER_OCTET;

	transaction_duration_periods = transaction_duration_sym /
			aUnitBackoffPeriod;

	/* Round up to the next integer number. */
	if ((transaction_duration_sym % aUnitBackoffPeriod) > 0) {
		transaction_duration_periods++;
	}

	/* Add 2 backoff periods that are used for CCA. */
	transaction_duration_periods += 2;
}

/**
 * \brief Calculates the entire transaction duration
 * \param phy_frame Address to the frame
 */
#ifdef BEACON_SUPPORT
uint16_t calc_frame_transmit_duration(uint8_t *phy_frame)
{
	uint8_t transaction_duration_octets;
	uint16_t transaction_duration_sym;

	/* number of octets */
	transaction_duration_octets = (*phy_frame) + PHY_OVERHEAD;

	/* Add interframe spacing - independent on ACK transmission. */
	if (*phy_frame > aMaxSIFSFrameSize) {
		transaction_duration_sym = macMinLIFSPeriod_def; /* symbols */
	} else {
		transaction_duration_sym = macMinSIFSPeriod_def; /* symbols */
	}

	/* If the frame requested an ACK, add ACK handling time. */
	if (phy_frame[PL_POS_FCF_1] & FCF_ACK_REQUEST) {
		/* Ensure there is room for the ACK. */
		transaction_duration_octets += ACK_FRAME_LEN + PHY_OVERHEAD; /**
		                                                              *
		                                                              *
		                                                              *octets
		                                                              **/

		/* Space is needed until the ACK is sent. */
		transaction_duration_sym += aTurnaroundTime +
				aUnitBackoffPeriod;                       /**
		                                                           *
		                                                           *
		                                                           *symbols
		                                                           **/
	}

	transaction_duration_sym += transaction_duration_octets *
			SYMBOLS_PER_OCTET;

	transaction_duration_sym = TAL_PSDU_US_PER_OCTET(
			transaction_duration_sym);

	return transaction_duration_sym;
}

#endif /* BEACON_SUPPORT */

/**
 * \brief Calculates backoff duration and handles the start of the CCA
 */
static void csma_backoff_calculation(void)
{
	uint32_t current_CAP_duration_sym;
	uint32_t current_CAP_end_sym;
	uint32_t next_backoff_boundary_us;
	uint32_t now_time_sym;
	uint32_t guard_time_before_next_beacon;

	/* \TODO consider CFP and BLE mode */
	current_CAP_duration_sym = TAL_GET_SUPERFRAME_DURATION_TIME(
			tal_pib.SuperFrameOrder);
	current_CAP_end_sym = tal_add_time_symbols(tal_pib.BeaconTxTime,
			current_CAP_duration_sym);

	/*
	 * Add some guard time to ensure that the transaction is completed
	 * before
	 * the timer fires that is going to track the next beacon.
	 */
	guard_time_before_next_beacon = TAL_RADIO_WAKEUP_TIME_SYM <<
			(tal_pib.BeaconOrder + 2);
	guard_time_before_next_beacon += TAL_CONVERT_US_TO_SYMBOLS(
			PRE_BEACON_GUARD_TIME_US);
	current_CAP_end_sym = tal_sub_time_symbols(current_CAP_end_sym,
			guard_time_before_next_beacon);

	/* Calculate next backoff period boundary. */
	{
		uint32_t time_since_last_beacon_sym;
		uint32_t next_backoff_boundary_period;

		pal_get_current_time(&now_time_sym);
		now_time_sym = TAL_CONVERT_US_TO_SYMBOLS(now_time_sym);

		time_since_last_beacon_sym = tal_sub_time_symbols(now_time_sym,
				tal_pib.BeaconTxTime);
		next_backoff_boundary_period = time_since_last_beacon_sym /
				aUnitBackoffPeriod;

		if ((time_since_last_beacon_sym % aUnitBackoffPeriod) > 0) {
			next_backoff_boundary_period++;
		}

		next_backoff_boundary_us
			= TAL_CONVERT_SYMBOLS_TO_US(
				pal_add_time_us(tal_pib.BeaconTxTime,
				(next_backoff_boundary_period *
				aUnitBackoffPeriod)));
	}

	/* Check if we are still within the CAP. */
	if (next_backoff_boundary_us >=
			TAL_CONVERT_SYMBOLS_TO_US(current_CAP_end_sym)) {
		/* current CAP is over, wait for next CAP */
		tal_csma_state = BACKOFF_WAITING_FOR_BEACON;
		start_beacon_loss_timer();
	} else { /* next backoff boundary is within current CAP */
		uint32_t remaining_periods_in_CAP; /* \TODO check if variable
		                                    * size can be reduced */

		/* Check if the remaining backoff time will expire in current
		 * CAP. */
		remaining_periods_in_CAP = tal_sub_time_symbols(
				current_CAP_end_sym, now_time_sym) /
				aUnitBackoffPeriod;

		if (remaining_backoff_periods > remaining_periods_in_CAP) {
			/*
			 * Reduce the backoff peridos by the remaining duration
			 * in
			 * the current CAP and continue in next CAP.
			 */
			remaining_backoff_periods -= remaining_periods_in_CAP;
			tal_csma_state = BACKOFF_WAITING_FOR_BEACON;

			start_beacon_loss_timer();
		} else { /* there are enough backoff periods in current CAP */
			uint32_t time_after_transaction_sym; /* \TODO check if
			                                      * variable size
			                                      * can be reduced
			                                      **/
			uint32_t transaction_duration_sym; /* \TODO check if
			                                    * variable size can
			                                    * be reduced */

			/* Add some guard time to wakeup the transceiver. */
			transaction_duration_sym
				= (transaction_duration_periods *
					aUnitBackoffPeriod) +
					TAL_CONVERT_US_TO_SYMBOLS(
					SLEEP_TO_TRX_OFF_TYP_US +
					CCA_GUARD_DURATION_US);

			time_after_transaction_sym
				= tal_add_time_symbols(TAL_CONVERT_US_TO_SYMBOLS(
					next_backoff_boundary_us),
					transaction_duration_sym);

			/* Check if the entire transaction fits into the current
			 * CAP. */
			if (time_after_transaction_sym < current_CAP_end_sym) {
				retval_t timer_status;
				uint32_t callback_start_time;

				/* Calculate the time needed to backoff. */
				cca_starttime_us
					= pal_add_time_us(
						next_backoff_boundary_us,
						TAL_CONVERT_SYMBOLS_TO_US(
						remaining_backoff_periods *
						aUnitBackoffPeriod));

				/*
				 * Ensure that wakeup time is available before
				 * CCA.
				 * The required duration depends on the current
				 * trx status.
				 * Assume here the worst case: trx is in SLEEP.
				 */

				/*
				 * \TODO depending on the duration that we need
				 * to backoff,
				 * set trx to SLEEP, TRX_OFF or PLL_ON
				 * meanwhile.
				 */
				while (pal_sub_time_us(cca_starttime_us,
						TAL_CONVERT_SYMBOLS_TO_US(
						now_time_sym)) <
						(SLEEP_TO_TRX_OFF_TYP_US +
						CCA_GUARD_DURATION_US)) {
					cca_starttime_us
						= pal_add_time_us(
							cca_starttime_us,
							TAL_CONVERT_SYMBOLS_TO_US(
							aUnitBackoffPeriod));
				}

				/*
				 * Start the CCA timer.
				 * Add some time to locate the next backoff
				 * boundary
				 * once CCA timer fires.
				 */
				callback_start_time
					= pal_sub_time_us(cca_starttime_us,
						(SLEEP_TO_TRX_OFF_TYP_US +
						CCA_PREPARATION_DURATION_US));

				timer_status = pal_timer_start(TAL_CSMA_CCA,
						callback_start_time,
						TIMEOUT_ABSOLUTE,
						(FUNC_PTR)cca_timer_handler_cb,
						NULL);

				if (timer_status == MAC_SUCCESS) {
					tal_csma_state
						= BACKOFF_WAITING_FOR_CCA_TIMER;
				} else if (timer_status ==
						PAL_TMR_INVALID_TIMEOUT) {
					/* Start the CCA immediately. */
					cca_timer_handler_cb(NULL);
				} else {
					tal_csma_state = CSMA_ACCESS_FAILURE;
					Assert("CCA timer start problem" == 0);
				}

				/* debug pin to switch on: define
				 * ENABLE_DEBUG_PINS, pal_config.h */
				PIN_BACKOFF_START();
			} else {
				/* Restart again after next beacon. */
				NB = 0;
				remaining_backoff_periods
					= (uint8_t)(rand() &
						((1 << BE) - 1));
				tal_csma_state = BACKOFF_WAITING_FOR_BEACON;

				start_beacon_loss_timer();
			}
		}
	}
}

/*
 * \brief State machine handling slotted CSMA
 */
void slotted_csma_state_handling(void)
{
	switch (tal_csma_state) {
	case BACKOFF_WAITING_FOR_CCA_TIMER:
		break;

	case BACKOFF_WAITING_FOR_BEACON:

		/*
		 * Do not perform any operation and wait until the next beacon
		 * reception. If several beacons are not received, the beacon
		 * loss
		 * timer expires and stops the entire transaction.
		 */
		break;

	case CSMA_HANDLE_BEACON:

		/* debug pin to switch on: define ENABLE_DEBUG_PINS,
		 * pal_config.h */
		PIN_WAITING_FOR_BEACON_END();
		PIN_BEACON_LOSS_TIMER_END();

		pal_timer_stop(TAL_CSMA_BEACON_LOSS_TIMER);
		csma_backoff_calculation();
		break;

	case CSMA_ACCESS_FAILURE:
		NB++;
		BE++;
		/* ensure that BE is no more than macMaxBE */
		if (BE > tal_pib.MaxBE) {  /* macMaxBE */
			BE = tal_pib.MaxBE; /* macMaxBE */
		}

		if (NB > macMaxCSMABackoffs) {
			/* terminate with channel access failure */
			tx_done(MAC_CHANNEL_ACCESS_FAILURE);
		} else {
			/* restart backoff */
			csma_backoff_calculation();
		}

		break;

	case NO_BEACON_TRACKING:
		/* terminate with channel access failure */
		tx_done(MAC_CHANNEL_ACCESS_FAILURE);
		break;

	case FRAME_SENDING:
		/* waiting for end of frame transmission */
		break;

	case TX_DONE_SUCCESS:
		tx_done(MAC_SUCCESS);
		break;

	case TX_DONE_FRAME_PENDING:
		tx_done(TAL_FRAME_PENDING);
		break;

	case TX_DONE_NO_ACK:
		if (number_of_tx_retries < tal_pib.MaxFrameRetries) {
			number_of_tx_retries++;
			set_trx_state(CMD_RX_AACK_ON);

			/*
			 * Start the entire CSMA procedure again,
			 * but do not reset the number of transmission attempts.
			 */
			BE++;

			/* ensure that BE is no more than macMaxBE */
			if (BE > tal_pib.MaxBE) { /* macMaxBE */
				BE = tal_pib.MaxBE; /* macMaxBE */
			}

			NB = 0;
			remaining_backoff_periods
				= (uint8_t)(rand() & ((1 << BE) - 1));
			csma_backoff_calculation();
		} else {
			tx_done(MAC_NO_ACK);
		}

		PIN_NO_ACK_END();
		break;

	default:
		Assert("INVALID CSMA status" == 0);
		break;
	}
} /* csma_ca_state_handling() */

/**
 * \brief CCA timer callback
 *
 * \param parameter Unused callback parameter
 */
static void cca_timer_handler_cb(void *parameter)
{
	/* debug pin to switch on: define ENABLE_DEBUG_PINS, pal_config.h */
	PIN_BACKOFF_END();

	if (perform_cca_twice() == PHY_IDLE) {
		send_frame_at_next_backoff_boundary();
	} else {
		tal_csma_state = CSMA_ACCESS_FAILURE;
	}

	parameter = parameter; /* Keep compiler happy. */
}

/**
 * \brief Performs CCA twice
 */
static uint8_t perform_cca_twice(void)
{
	uint8_t cca_status;
	uint8_t cca_done;
	uint8_t CW = 2;
	uint32_t now_time_us;

	do {
		pal_get_current_time(&now_time_us);
	} while (pal_add_time_us(now_time_us,
			(SLEEP_TO_TRX_OFF_TYP_US +
			CCA_PREPARATION_DURATION_US)) <
			cca_starttime_us);

#if ((MAC_START_REQUEST_CONFIRM == 1) && (defined BEACON_SUPPORT))
	if (tal_beacon_transmission) {
#if (_DEBUG_ > 0)
		Assert("Ongoing beacon transmission, slotted CSMA busy" == 0);
#endif
		return PHY_BUSY;
	}
#endif /* ((MAC_START_REQUEST_CONFIRM == 1) && (defined BEACON_SUPPORT)) */

	/* Ensure that trx is at least in TRX_OFF mode at this time. */
	if (tal_trx_status == TRX_SLEEP) {
		set_trx_state(CMD_TRX_OFF);
	}

	do {
		pal_get_current_time(&now_time_us);
	} while (pal_add_time_us(now_time_us,
			(TRX_OFF_TO_PLL_ON_TIME_US +
			CCA_PREPARATION_DURATION_US)) <
			cca_starttime_us);

	/*
	 * Set trx to PLL_ON.
	 * If trx is busy and trx cannot be set to PLL_ON, assess channel as
	 * busy.
	 */
	if (set_trx_state(CMD_PLL_ON) != PLL_ON) {
		return PHY_BUSY;
	}

	/* no interest in receiving frames while doing CCA */
	trx_bit_write(SR_RX_PDT_DIS, RX_DISABLE); /* disable frame reception
	                                           * indication */

	/* do CCA twice */
	do {
		/* wait here until 16us before backoff boundary */
		/* assume TRX is in PLL_ON */
		do {
			pal_get_current_time(&now_time_us);
		} while (pal_add_time_us(now_time_us,
				CCA_PRE_START_DURATION_US) <
				cca_starttime_us);

		set_trx_state(CMD_RX_ON);

		/* debug pin to switch on: define ENABLE_DEBUG_PINS,
		 * pal_config.h */
		PIN_CCA_START();

		/* Start CCA */
		trx_bit_write(SR_CCA_REQUEST, CCA_START);

		/* wait until CCA is done and get status */
		pal_timer_delay(TAL_CONVERT_SYMBOLS_TO_US(CCA_DURATION_SYM));

		do {
			/* poll until CCA is really done; */
			cca_done = trx_bit_read(SR_CCA_DONE);
		} while (cca_done != CCA_COMPLETED);

		/* between both CCA switch trx to PLL_ON to reduce power
		 * consumption */
		set_trx_state(CMD_PLL_ON);

		/* debug pin to switch on: define ENABLE_DEBUG_PINS,
		 * pal_config.h */
		PIN_CCA_END();

		/* check if channel was idle or busy */
		if (trx_bit_read(SR_CCA_STATUS) == CCA_CH_IDLE) {
			/* do next CCA at next backoff boundary */
			cca_starttime_us = pal_add_time_us(cca_starttime_us,
					TAL_CONVERT_SYMBOLS_TO_US(
					aUnitBackoffPeriod));
			CW--;
			cca_status = PHY_IDLE;
		} else { /* PHY busy */
			cca_status = PHY_BUSY;
			set_trx_state(CMD_RX_AACK_ON);
			break; /* if channel is busy do no do CCA for the second
			        * time */
		}
	} while (CW > 0);

	/*
	 * Keep trx ready for transmission if channel is idle.
	 * The transceiver is still in PLL_ON.
	 * If the channel is not idle, the trx handling is done in
	 * csma_backoff().
	 */

	/*
	 * Clear CCA interrupt flag.
	 * This is only necessary for debugging, because only in debug mode
	 * interrupt that are not handled cause an assert in the ISR.
	 */
#if (_DEBUG_ > 0)
	trx_reg_read(RG_IRQ_STATUS);
#endif

	/*
	 * Since we are not interested in any frames that might be received
	 * during CCA, reject any information that indicates a previous frame
	 * reception.
	 */
	trx_bit_write(SR_RX_PDT_DIS, RX_ENABLE); /* enable frame reception
	                                          * indication */

	return cca_status;
}

/**
 * \brief Sends the frame at the next backoff boundary
 */
static void send_frame_at_next_backoff_boundary(void)
{
	uint32_t now_time_us;

	/*
	 * Locate the next backoff boundary for the frame transmissiom;
	 * this backoff boundary is the starttime for the frame fransmission.
	 * Use a blocking approach, since next backoff boundary should be close.
	 */
	do {
		pal_get_current_time(&now_time_us);
	} while (pal_add_time_us(now_time_us, PRE_TX_DURATION_US) <
			cca_starttime_us);

	/* debug pin to switch on: define ENABLE_DEBUG_PINS, pal_config.h */
	PIN_TX_START();

	tal_csma_state = FRAME_SENDING;

	/* download and send frame, no CSMA and no frame_retry */
	send_frame(NO_CSMA_NO_IFS, false);
}

/**
 * \brief Starts the beacon loss timer
 */
static void start_beacon_loss_timer(void)
{
	uint32_t timer_duration_us;
#if (_DEBUG_ > 0)
	retval_t timer_status;
#endif

	/* debug pin to switch on: define ENABLE_DEBUG_PINS, pal_config.h */
	PIN_BEACON_LOSS_TIMER_START();

	timer_duration_us
		= TAL_CONVERT_SYMBOLS_TO_US(TAL_GET_BEACON_INTERVAL_TIME(tal_pib
			.BeaconOrder));
	timer_duration_us *= aMaxLostBeacons;
	timer_duration_us += CSMA_BEACON_LOSS_GUARD_TIME_US;

#if (_DEBUG_ > 0)
	timer_status =
#endif
	pal_timer_start(TAL_CSMA_BEACON_LOSS_TIMER,
			timer_duration_us,
			TIMEOUT_RELATIVE,
			(FUNC_PTR)beacon_loss_timer_cb,
			NULL);

#if (_DEBUG_ > 0)
	if (timer_status != MAC_SUCCESS) {
		if (timer_status == PAL_TMR_INVALID_TIMEOUT) {
			Assert(
					"beacon loss timer start failed: PAL_TMR_INVALID_TIMEOUT" ==
					0);
		} else if (timer_status == PAL_TMR_ALREADY_RUNNING) {
			Assert(
					"beacon loss timer start failed: PAL_TMR_ALREADY_RUNNING" ==
					0);
		} else {
			Assert("beacon loss timer start failed: ?" == 0);
		}
	}
#endif
}

/**
 * \brief Handler for beacon loss timer
 *
 * \param parameter Unused callback parameter
 */
static void beacon_loss_timer_cb(void *parameter)
{
	/* debug pin to switch on: define ENABLE_DEBUG_PINS, pal_config.h */
	PIN_BEACON_LOSS_TIMER_END();

#if (_DEBUG_ > 0)
	if (tal_csma_state != BACKOFF_WAITING_FOR_BEACON) {
		Assert("beacon loss timer issue" == 0);
	}
#endif

	tal_csma_state = NO_BEACON_TRACKING;

	parameter = parameter; /* Keep compiler happy. */
}

/**
 * \brief Finalizes the CSMA procedure
 *
 * \param status Result of the slotted transmission
 */
static void tx_done(retval_t status)
{
#if (_DEBUG_ > 0)
	switch (tal_state) {
	case TAL_SLOTTED_CSMA:
		/* That's the state we are expecting */
		break;

	default:
		Assert("unexpected tal_state" == 0);
		break;
	}
#endif
#if (_DEBUG_ > 0)
	if (pal_is_timer_running(TAL_CSMA_BEACON_LOSS_TIMER)) {
		Assert("beacon lost timer is still running" == 0);
	}
#endif

	tal_state = TAL_IDLE;
	tal_csma_state = CSMA_IDLE;

	/* debug pin to switch on: define ENABLE_DEBUG_PINS, pal_config.h */
	PIN_CSMA_END();

	tal_tx_frame_done_cb(status, mac_frame_ptr);
}

#endif /* BEACON_SUPPORT */

/* EOF */
