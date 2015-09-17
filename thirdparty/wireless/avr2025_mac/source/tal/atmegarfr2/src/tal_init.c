/**
 * \file tal_init.c
 *
 * \brief This file implements functions for initializing TAL.
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

/* === INCLUDES ============================================================ */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "return_val.h"
#include "tal.h"
#include "ieee_const.h"
#include "tal_pib.h"
#include "tal_irq_handler.h"
#include "stack_config.h"
#include "bmm.h"
#include "qmm.h"
#include "pal.h"
#include "tal.h"
#include "tal_internal.h"
#include "tal_constants.h"
#include "atmega256rfr2.h"
#include "tal_config.h"
#ifdef BEACON_SUPPORT
#include "tal_slotted_csma.h"
#endif  /* BEACON_SUPPORT */
#ifdef ENABLE_TFA
#include "tfa.h"
#endif
#include "mac_build_config.h"
#ifdef STB_ON_SAL
#include "stb.h"
#endif

/**
 * \addtogroup group_tal_init_rfr2
 * @{
 */

/* === MACROS ============================================================== */

/* Value in us used for delay between poll attempts for transceiver access. */
#define TRX_POLL_WAIT_TIME_US       (100)

/* Ratio between max time of TR1 / transceiver poll delay */
#define P_ON_TO_CLKM_ATTEMPTS       ((uint8_t) \
	(P_ON_TO_CLKM_AVAILABLE_MAX_US / TRX_POLL_WAIT_TIME_US))

/* Ratio between max time of TR2 / transceiver poll delay */
#define SLEEP_TO_TRX_OFF_ATTEMPTS   ((uint8_t) \
	(SLEEP_TO_TRX_OFF_MAX_US / TRX_POLL_WAIT_TIME_US))

/* Ratio between max time of TR13 / transceiver poll delay */
#define RESET_TO_TRX_OFF_ATTEMPTS   ((uint8_t) \
	(RESET_TO_TRX_OFF_MAX_US / TRX_POLL_WAIT_TIME_US))

/* === GLOBALS ============================================================= */
#ifdef BEACON_SUPPORT
/* Beacon Support */
#ifdef ENABLE_FTN_PLL_CALIBRATION
uint8_t TAL_CSMA_CCA;
uint8_t TAL_CSMA_BEACON_LOSS_TIMER;
uint8_t TAL_CALIBRATION;
#else  /* NO FTN PLL CALIBRATION */
uint8_t TAL_CSMA_CCA;
uint8_t TAL_CSMA_BEACON_LOSS_TIMER;
#endif  /* ENABLE_FTN_PLL_CALIBRATION */
#else /* No BEACON_SUPPORT */
#ifdef ENABLE_FTN_PLL_CALIBRATION
uint8_t TAL_CALIBRATION;
#endif  /* ENABLE_FTN_PLL_CALIBRATION */
#endif  /* BEACON_SUPPORT */

/* === PROTOTYPES ========================================================== */

static retval_t trx_init(void);
static void trx_config(void);
static retval_t trx_reset(void);
static retval_t internal_tal_reset(bool set_default_pib);

/**
 * \brief Initializes all timers used by the TAL module by assigning id's to
 * each of them
 */
static retval_t tal_timer_init(void);

/**
 * \brief Stops all initialized TAL timers
 */
static void tal_timers_stop(void);

/* ! @} */

/* === IMPLEMENTATION ====================================================== */

/*
 * \brief Initializes the TAL
 *
 * This function is called to initialize the TAL. The transceiver is
 * initialized, the TAL PIBs are set to their default values, and the TAL state
 * machine is set to TAL_IDLE state.
 *
 * \return MAC_SUCCESS  if the transceiver state is changed to TRX_OFF and the
 *                 current device part number and version number are correct;
 *         FAILURE otherwise
 */
retval_t tal_init(void)
{
	MCUSR = 0;

	/* Enable SRAM Data Retention */
	DRTRAM0 = _BV(ENDRT);
	DRTRAM1 = _BV(ENDRT);
	DRTRAM2 = _BV(ENDRT);
	DRTRAM3 = _BV(ENDRT);

	/* Init the PAL and by this means also the transceiver interface */
	if (pal_init() != MAC_SUCCESS) {
		return FAILURE;
	}

	if (trx_init() != MAC_SUCCESS) {
		return FAILURE;
	}

	if (tal_timer_init() != MAC_SUCCESS) {
		return FAILURE;
	}

#ifdef ENABLE_STACK_NVM
	pal_ps_get(INTERN_EEPROM, EE_IEEE_ADDR, 8, &tal_pib.IeeeAddress);
#endif

	/*
	 * Do the reset stuff.
	 * Set the default PIBs.
	 * Generate random seed.
	 */
	if (internal_tal_reset(true) != MAC_SUCCESS) {
		return FAILURE;
	}

#ifndef DISABLE_IEEE_ADDR_CHECK
	/* Check if a valid IEEE address is available. */

	/*
	 * This while loop is on purpose, since just in the
	 * rare case that such an address is randomly
	 * generated again, we must repeat this.
	 */
	while ((tal_pib.IeeeAddress == 0x0000000000000000) ||
			(tal_pib.IeeeAddress == 0xFFFFFFFFFFFFFFFF)
			) {
		/*
		 * In case no valid IEEE address is available, a random
		 * IEEE address will be generated to be able to run the
		 * applications for demonstration purposes.
		 * In production code this can be omitted.
		 */

		/*
		 * The proper seed for function rand() has already been
		 * generated
		 * in function tal_generate_rand_seed().
		 */
		uint8_t *ptr_pib = (uint8_t *)&tal_pib.IeeeAddress;

		for (uint8_t i = 0; i < 8; i++) {
			*ptr_pib++ = (uint8_t)rand();

			/*
			 * Note:
			 * Even if casting the 16 bit rand value back to 8 bit,
			 * and running the loop 8 timers (instead of only 4
			 * times)
			 * may look cumbersome, it turns out that the code gets
			 * smaller using 8-bit here.
			 * And timing is not an issue at this place...
			 */
		}
	}
#endif  /* #ifndef DISABLE_IEEE_ADDR_CHECK */
#ifdef ENABLE_STACK_NVM
	pal_ps_set(EE_IEEE_ADDR, 8, &tal_pib.IeeeAddress);
#endif

	/*
	 * Configure interrupt handling.
	 * Install handlers for the transceiver interrupts.
	 */
	pal_trx_irq_init_rx_end((FUNC_PTR)trx_rx_end_handler_cb);
	pal_trx_irq_init_tx_end((FUNC_PTR)trx_tx_end_handler_cb);
	pal_trx_irq_init_awake((FUNC_PTR)trx_awake_handler_cb);

#if (defined BEACON_SUPPORT) || (defined ENABLE_TSTAMP)
	/* Configure time stamp interrupt. */
	pal_trx_irq_init_tstamp((FUNC_PTR)trx_irq_timestamp_handler_cb);
#endif /* (defined BEACON_SUPPORT) || (defined ENABLE_TSTAMP) */

	/* Initialize the buffer management module and get a buffer to store
	 * reveived frames. */
	bmm_buffer_init();
	tal_rx_buffer = bmm_buffer_alloc(LARGE_BUFFER_SIZE);

	/* Init incoming frame queue */
#ifdef ENABLE_QUEUE_CAPACITY
	qmm_queue_init(&tal_incoming_frame_queue,
			TAL_INCOMING_FRAME_QUEUE_CAPACITY);
#else
	qmm_queue_init(&tal_incoming_frame_queue);
#endif  /* ENABLE_QUEUE_CAPACITY */

#ifdef ENABLE_TFA
	tfa_init();
#endif

	return MAC_SUCCESS;
} /* tal_init() */

/**
 * \brief Initializes the transceiver
 *
 * This function is called to initialize the transceiver.
 *
 * \return MAC_SUCCESS  if the transceiver state is changed to TRX_OFF and the
 *                 current device part number and version number are correct;
 *         FAILURE otherwise
 */
static retval_t trx_init(void)
{
	tal_trx_status_t trx_status;
	uint8_t poll_counter = 0;

	sysclk_enable_peripheral_clock(&TRX_CTRL_0);
	TRX_RST_HIGH();
	TRX_SLP_TR_LOW();

	pal_timer_delay(P_ON_TO_CLKM_AVAILABLE_TYP_US);

	/* Apply reset pulse */
	TRX_RST_LOW();
	pal_timer_delay(RST_PULSE_WIDTH_US);
	TRX_RST_HIGH();

	/* Verify that the trx has reached TRX_OFF. */
	poll_counter = 0;
	do {
		/* Wait a short time interval. */
		pal_timer_delay(TRX_POLL_WAIT_TIME_US);

		trx_status = (tal_trx_status_t)trx_bit_read(SR_TRX_STATUS);

		/* Wait not more than max. value of TR2. */
		if (poll_counter == RESET_TO_TRX_OFF_ATTEMPTS) {
#if (_DEBUG_ > 0)
			Assert(
					"MAX Attempts to switch to TRX_OFF state reached" ==
					0);
#endif
			return FAILURE;
		}

		poll_counter++;
	} while (trx_status != TRX_OFF);

	tal_trx_status = TRX_OFF;

#if !defined(FPGA_EMULATION)
	/* Check if actually running on an ATmegaRFR2 device. */
	if (ATMEGARFR2_PART_NUM != trx_reg_read(RG_PART_NUM)) {
		return FAILURE;
	}
#endif
	return MAC_SUCCESS;
}

/**
 * \brief Internal TAL reset function
 *
 * \param set_default_pib Defines whether PIB values need to be set
 *                        to its default values
 *
 * \return MAC_SUCCESS  if the transceiver state is changed to TRX_OFF and the
 *                 current device part number and version number are correct;
 *         FAILURE otherwise
 */
static retval_t internal_tal_reset(bool set_default_pib)
{
	if (trx_reset() != MAC_SUCCESS) {
		return FAILURE;
	}

	/*
	 * Generate a seed for the random number generator in function rand().
	 * This is required (for example) as seed for the CSMA-CA algorithm.
	 */
	tal_generate_rand_seed();

	/* Configure the transceiver register values. */
	trx_config();

	if (set_default_pib) {
		/* Set the default PIB values */
		init_tal_pib(); /* implementation can be found in 'tal_pib.c' */
	} else {
		/* nothing to do - the current TAL PIB attribute values are used
		**/
	}

	/*
	 * Write all PIB values to the transceiver
	 * that are needed by the transceiver itself.
	 */
	write_all_tal_pib_to_trx(); /* implementation can be found in
	                             *'tal_pib.c' */

	/* Reset TAL variables. */
	tal_state = TAL_IDLE;

#ifdef BEACON_SUPPORT
	tal_csma_state = CSMA_IDLE;
#endif  /* BEACON_SUPPORT */

#if ((MAC_START_REQUEST_CONFIRM == 1) && (defined BEACON_SUPPORT))
	tal_beacon_transmission = false;
#endif /* ((MAC_START_REQUEST_CONFIRM == 1) && (defined BEACON_SUPPORT)) */

	tal_rx_on_required = false;

	return MAC_SUCCESS;
}

/**
 * \brief Configures the transceiver
 *
 * This function is called to configure the transceiver after reset.
 */
static void trx_config(void)
{
	/* After we have initialized a proper seed for rand(),
	 * the transceiver's CSMA seed can be initialized.
	 * It needs to be assured that a seed for function rand()
	 * had been generated before.
	 */

	/*
	 * Init the SEED value of the CSMA backoff algorithm.
	 */
	uint16_t rand_value = (uint16_t)rand();
	trx_reg_write(RG_CSMA_SEED_0, (uint8_t)rand_value);
	trx_bit_write(SR_CSMA_SEED_1, (uint8_t)(rand_value >> 8));

	trx_bit_write(SR_AACK_SET_PD, PD_ACK_BIT_SET_ENABLE); /* ACKs for
	                                                       * data
	                                                       * requests,
	                                                       * indicate
	                                                       * pending data
	                                                       **/
	trx_bit_write(SR_RX_SAFE_MODE, RX_SAFE_MODE_ENABLE); /* Enable
	                                                     * buffer
	                                                     * protection
	                                                     * mode */
	trx_reg_write(RG_IRQ_MASK, TRX_IRQ_DEFAULT);
#if (defined BEACON_SUPPORT) || (defined ENABLE_TSTAMP)
	/* The TX start timestamp is taken by the respective PAL layer */
	trx_reg_write(RG_IRQ_MASK1, (1 << TX_START_EN));
#endif
	trx_reg_write(0x156, 0xFF); /* RPC feature configuration. */

#if (ANTENNA_DIVERSITY == 1)
	/* Use antenna diversity */
	trx_bit_write(SR_ANT_CTRL, ANTENNA_DEFAULT);
	trx_bit_write(SR_PDT_THRES, THRES_ANT_DIV_ENABLE);
	trx_bit_write(SR_ANT_DIV_EN, ANT_DIV_ENABLE);
	trx_bit_write(SR_ANT_EXT_SW_EN, ANT_EXT_SW_ENABLE);
#endif

#ifdef CCA_ED_THRESHOLD

	/*
	 * Set CCA ED Threshold to other value than standard register due to
	 * board specific loss (see pal_config.h). */
	trx_bit_write(SR_CCA_ED_THRES, CCA_ED_THRESHOLD);
#endif

#ifdef EXT_RF_FRONT_END_CTRL
	/* Enable RF front end control, and TX filtering */
	trx_bit_write(SR_PA_EXT_EN, 1);
	trx_bit_write(SR_PLL_TX_FLT, 1);
#endif

	/* if not explicitly set, RX_OVERRIDE should remain its reset value */
#ifdef ENABLE_RX_OVERRIDE

	/*
	 * Allow overriding of a 'weak' frame when another frame with stronger
	 * signal power arrives during the reception of this 'weak' frame.
	 */
	trx_bit_write(SR_RX_OVERRIDE, ENABLE_RX_OVERRIDE);
#endif
}

/**
 * \brief Reset transceiver
 *
 * \return MAC_SUCCESS  if the transceiver state is changed to TRX_OFF
 *         FAILURE otherwise
 */
static retval_t trx_reset(void)
{
	tal_trx_status_t trx_status;
	uint8_t poll_counter = 0;

	/* trx might sleep, so wake it up */
	TRX_SLP_TR_LOW();
	pal_timer_delay(SLEEP_TO_TRX_OFF_TYP_US);

	/* Apply reset pulse */
	TRX_RST_LOW();
	pal_timer_delay(RST_PULSE_WIDTH_US);
	TRX_RST_HIGH();

	/* verify that trx has reached TRX_OFF */
	do {
		/* Wait a short time interval. */
		pal_timer_delay(TRX_POLL_WAIT_TIME_US);

		trx_status = (tal_trx_status_t)trx_bit_read(SR_TRX_STATUS);

		/* Wait not more than max. value of TR2. */
		if (poll_counter == SLEEP_TO_TRX_OFF_ATTEMPTS) {
#if (_DEBUG_ > 0)
			Assert(
					"MAX Attempts to switch to TRX_OFF state reached" ==
					0);
#endif
			return FAILURE;
		}

		poll_counter++;
	} while (trx_status != TRX_OFF);

	tal_trx_status = TRX_OFF;

	return MAC_SUCCESS;
}

/*
 * \brief Resets TAL state machine and sets the default PIB values if requested
 *
 * \param set_default_pib Defines whether PIB values need to be set
 *                        to its default values
 *
 * \return MAC_SUCCESS  if the transceiver state is changed to TRX_OFF
 *         FAILURE otherwise
 */
retval_t tal_reset(bool set_default_pib)
{
	/*
	 * Do the reset stuff.
	 * Set the default PIBs depending on the given parameter
	 * set_default_pib.
	 * Do NOT generate random seed again.
	 */
	if (internal_tal_reset(set_default_pib) != MAC_SUCCESS) {
		return FAILURE;
	}

	ENTER_CRITICAL_REGION();
	tal_timers_stop();
	LEAVE_CRITICAL_REGION();

	/* Clear TAL Incoming Frame queue and free used buffers. */
	while (tal_incoming_frame_queue.size > 0) {
		buffer_t *frame = qmm_queue_remove(&tal_incoming_frame_queue,
				NULL);
		if (NULL != frame) {
			bmm_buffer_free(frame);
		}
	}

#ifdef ENABLE_TFA
	tfa_reset(set_default_pib);
#endif

	/*
	 * Configure interrupt handling.  Clear all pending interrupts.
	 * Handlers have been installed in tal_init(), and are never
	 * uninstalled.
	 */
	pal_trx_irq_flag_clr_rx_end();
	pal_trx_irq_flag_clr_tx_end();
#if (defined BEACON_SUPPORT) || (defined ENABLE_TSTAMP)
	pal_trx_irq_flag_clr_tstamp();
#endif /* (defined BEACON_SUPPORT) || (defined ENABLE_TSTAMP) */
	pal_trx_irq_flag_clr_awake();

	/*
	 * To make sure that the CSMA seed is properly set within the
	 * transceiver,
	 * put the trx to sleep briefly and wake it up again.
	 */
	tal_trx_sleep(SLEEP_MODE_1);

	tal_trx_wakeup();

#ifdef ENABLE_FTN_PLL_CALIBRATION
	{
		/* Handle PLL calibration and filter tuning. */
		retval_t timer_status;

		/* Calibration timer has already been stopped within this
		 * function. */

		/* Start periodic calibration timer.*/
		timer_status = pal_timer_start(TAL_CALIBRATION,
				TAL_CALIBRATION_TIMEOUT_US,
				TIMEOUT_RELATIVE,
				(FUNC_PTR)calibration_timer_handler_cb,
				NULL);

		if (timer_status != MAC_SUCCESS) {
			Assert("PLL calibration timer start problem" == 0);
		}
	}
#endif  /* ENABLE_FTN_PLL_CALIBRATION */

#ifdef STB_ON_SAL
	stb_restart();
#endif

	return MAC_SUCCESS;
}

/*
 * \brief Generates a 16-bit random number used as initial seed for srand()
 *
 * This function generates a 16-bit random number by means of using the
 * Random Number Generator from the transceiver.
 * The Random Number Generator generates 2-bit random values. These 2-bit
 * random values are concatenated to the required 16-bit random seed.
 *
 * The generated random 16-bit number is feed into function srand()
 * as initial seed.
 *
 * The transceiver state is initally set to RX_ON.
 * After the completion of the random seed generation, the
 * trancseiver is set to TRX_OFF.
 *
 * As a prerequisite the preamble detector must not be disabled.
 *
 * Also in case the function is called from a different state than TRX_OFF,
 * additional trx state handling is required, such as reading the original
 * value and restoring this state after finishing the sequence.
 * Since in our case the function is called from TRX_OFF, this is not required
 * here.
 */
void tal_generate_rand_seed(void)
{
	uint16_t seed = 0;
	uint8_t cur_random_val = 0;

	/* Ensure that PLL has locked and receive mode is reached. */
	tal_trx_status_t trx_state;
	do {
		trx_state = set_trx_state(CMD_RX_ON);
	} while (trx_state != RX_ON);

	/* Ensure that register bit RX_PDT_DIS is set to 0. */
	trx_bit_write(SR_RX_PDT_DIS, RX_ENABLE);

	/*
	 * We need to disable TRX IRQs while generating random values in RX_ON,
	 * we do not want to receive frames at this point of time at all.
	 */
	trx_reg_write(RG_IRQ_MASK, TRX_IRQ_NONE);

	/*
	 * The 16-bit random value is generated from various 2-bit random
	 * values.
	 */
	for (uint8_t i = 0; i < 8; i++) {
		/* Now we can safely read the 2-bit random number. */
		cur_random_val = trx_bit_read(SR_RND_VALUE);
		seed = seed << 2;
		seed |= cur_random_val;
		PAL_WAIT_1_US(); /* wait that the random value gets updated */
	}

	set_trx_state(CMD_FORCE_TRX_OFF);

	/*
	 * Now we need to clear potential pending TRX IRQs and
	 * enable the TRX IRQs again.
	 */
	trx_reg_write(RG_IRQ_STATUS, 0xFF);
	trx_reg_write(RG_IRQ_MASK, TRX_IRQ_DEFAULT);

	/* Set the seed for the random number generator. */
	srand(seed);
}

static retval_t tal_timer_init(void)
{
#ifdef BEACON_SUPPORT
	/* Beacon Support */
#ifdef ENABLE_FTN_PLL_CALIBRATION
	if (MAC_SUCCESS != pal_timer_get_id(&TAL_CSMA_CCA)) {
		return FAILURE;
	}

	if (MAC_SUCCESS != pal_timer_get_id(&TAL_CSMA_BEACON_LOSS_TIMER)) {
		return FAILURE;
	}

	if (MAC_SUCCESS != pal_timer_get_id(&TAL_CALIBRATION)) {
		return FAILURE;
	}

#else
	if (MAC_SUCCESS != pal_timer_get_id(&TAL_CSMA_CCA)) {
		return FAILURE;
	}

	if (MAC_SUCCESS != pal_timer_get_id(&TAL_CSMA_BEACON_LOSS_TIMER)) {
		return FAILURE;
	}
#endif  /* ENABLE_FTN_PLL_CALIBRATION */
#else /* No BEACON_SUPPORT */
#ifdef ENABLE_FTN_PLL_CALIBRATION
	if (MAC_SUCCESS != pal_timer_get_id(&TAL_CALIBRATION)) {
		return FAILURE;
	}
#endif  /* ENABLE_FTN_PLL_CALIBRATION */
#endif  /* BEACON_SUPPORT */
	return MAC_SUCCESS;
}

static void tal_timers_stop(void)
{
#if (NUMBER_OF_TAL_TIMERS > 0)
#ifdef BEACON_SUPPORT
	/* Beacon Support */
#ifdef ENABLE_FTN_PLL_CALIBRATION
	pal_timer_stop(TAL_CSMA_CCA);
	pal_timer_stop(TAL_CSMA_BEACON_LOSS_TIMER);
	pal_timer_stop(TAL_CALIBRATION);
#else
	pal_timer_stop(TAL_CSMA_CCA);
	pal_timer_stop(TAL_CSMA_BEACON_LOSS_TIMER);
#endif  /* ENABLE_FTN_PLL_CALIBRATION */
#else /* No BEACON_SUPPORT */
#ifdef ENABLE_FTN_PLL_CALIBRATION
	pal_timer_stop(TAL_CALIBRATION);
#endif  /* ENABLE_FTN_PLL_CALIBRATION */
#endif  /* BEACON_SUPPORT */
#endif /*(NUMBER_OF_TAL_TIMERS > 0)*/
}

/* EOF */
