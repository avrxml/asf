/**
 * @file tal_init.c
 *
 * @brief This file implements functions for initializing TAL.
 *
 * Copyright (c) 2013 Atmel Corporation. All rights reserved.
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
 * Copyright (c) 2013, Atmel Corporation All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

/* === INCLUDES ============================================================ */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "pal.h"
#include "return_val.h"
#include "tal.h"
#include "ieee_const.h"
#include "tal_pib.h"
#include "pal.h"
#include "tal_irq_handler.h"
#include "stack_config.h"
#include "bmm.h"
#include "qmm.h"
#include "tal.h"
#include "tal_internal.h"
#include "tal_constants.h"
#include "at86rf230b.h"
#include "tal_config.h"
#ifdef BEACON_SUPPORT
#include "tal_slotted_csma.h"
#endif  /* BEACON_SUPPORT */
#ifdef NON_BLOCKING_SPI
#include "tal_rx.h"
#endif

/**
 * \addtogroup group_tal_init_230b
 * @{
 */

/* === MACROS ============================================================== */

/* Value in us used for delay between poll attempts for transceiver access. */
#define TRX_POLL_WAIT_TIME_US       (100)

/* === GLOBALS ============================================================= */

#if ((MAC_SCAN_ED_REQUEST_CONFIRM == 1) && (defined BEACON_SUPPORT))
#ifdef ENABLE_FTN_PLL_CALIBRATION
uint8_t TAL_ACK_WAIT_TIMER;
uint8_t TAL_ED_SCAN_DURATION_TIMER;
uint8_t TAL_ED_SAMPLE_TIMER;
uint8_t TAL_CSMA_CCA;
uint8_t TAL_CSMA_BEACON_LOSS_TIMER;
uint8_t TAL_CALIBRATION;
#else
uint8_t TAL_ACK_WAIT_TIMER;
uint8_t TAL_ED_SCAN_DURATION_TIMER;
uint8_t TAL_ED_SAMPLE_TIMER;
uint8_t TAL_CSMA_CCA;
uint8_t TAL_CSMA_BEACON_LOSS_TIMER;
#endif  /* ENABLE_FTN_PLL_CALIBRATION */
#endif

/* ED Scan is switched on */
/* No beacon support */
#if ((MAC_SCAN_ED_REQUEST_CONFIRM == 1) && (!defined BEACON_SUPPORT))
#ifdef ENABLE_FTN_PLL_CALIBRATION
uint8_t TAL_ED_SCAN_DURATION_TIMER;
uint8_t TAL_ED_SAMPLE_TIMER;
uint8_t TAL_CALIBRATION;
#else
uint8_t TAL_ED_SCAN_DURATION_TIMER;
uint8_t TAL_ED_SAMPLE_TIMER;
#endif  /* ENABLE_FTN_PLL_CALIBRATION */
#endif

/* ED Scan is switched off */
/* Network with beacon support */
#if ((MAC_SCAN_ED_REQUEST_CONFIRM == 0) && (defined BEACON_SUPPORT))
#ifdef ENABLE_FTN_PLL_CALIBRATION
uint8_t TAL_ACK_WAIT_TIMER;
uint8_t TAL_CSMA_CCA;
uint8_t TAL_CSMA_BEACON_LOSS_TIMER;
uint8_t TAL_CALIBRATION;
#else
uint8_t TAL_ACK_WAIT_TIMER;
uint8_t TAL_CSMA_CCA;
uint8_t TAL_CSMA_BEACON_LOSS_TIMER;
#endif  /* ENABLE_FTN_PLL_CALIBRATION */
#endif

/* ED Scan is switched off */
/* No beacon support */
#if ((MAC_SCAN_ED_REQUEST_CONFIRM == 0) && (!defined BEACON_SUPPORT))
#ifdef ENABLE_FTN_PLL_CALIBRATION
uint8_t TAL_CALIBRATION;
#endif  /* ENABLE_FTN_PLL_CALIBRATION */
#endif

/* === PROTOTYPES ========================================================== */

static retval_t trx_init(void);
static void trx_config(void);
static retval_t trx_reset(void);
static retval_t internal_tal_reset(bool set_default_pib);

/**
 * \brief Initializes all timers used by the TAL module by assigning id's to
 *each of them
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
	 * For systems including the AT86RF230B the random seed generation
	 * cannot be done using a dedicated hardware feature.
	 * Therefore all random seed generation needs to be done by special
	 * means (e.g. utilization of ADC) that generate a random value only
	 * within a certain range.
	 *
	 * In case the node already has a valid IEEE address (i.e. an IEEE
	 * address which is different from 0x0000000000000000 or
	 * 0xFFFFFFFFFFFFFFFF), this IEEE address (the lower 16 bit)
	 * shall be used as seed for srand(), since each node should have a
	 *unique
	 * IEEE address.
	 * In this case srand() is called directly and function
	 *tal_generate_rand_seed()
	 * is not called.
	 *
	 * Note: This behaviour is different in all other TALs, since in these
	 * cases the seed for srand() is always generated based on transceiver
	 * hardware support.
	 */

#ifndef DISABLE_IEEE_ADDR_CHECK
	if ((tal_pib.IeeeAddress == 0x0000000000000000) ||
			(tal_pib.IeeeAddress == 0xFFFFFFFFFFFFFFFF)
			) {
		/*
		 * Generate a seed for the random number generator in function
		 *rand().
		 * This is required (for example) as seed for the CSMA-CA
		 *algorithm.
		 */
		tal_generate_rand_seed();

		/*
		 * Now that we have generated a random seed, we can generate a
		 *random
		 * IEEE address for this node.
		 */
		do {
			/*
			 * In case no valid IEEE address is available, a random
			 * IEEE address will be generated to be able to run the
			 * applications for demonstration purposes.
			 * In production code this can be omitted.
			 */

			/*
			 * The proper seed for function rand() has already been
			 *generated
			 * in function tal_generate_rand_seed().
			 */
			uint8_t *ptr_pib = (uint8_t *)&tal_pib.IeeeAddress;

			for (uint8_t i = 0; i < 8; i++) {
				*ptr_pib++ = (uint8_t)rand();

				/*
				 * Note:
				 * Even if casting the 16 bit rand value back to
				 *8 bit,
				 * and running the loop 8 timers (instead of
				 *only 4 times)
				 * may look cumbersome, it turns out that the
				 *code gets
				 * smaller using 8-bit here.
				 * And timing is not an issue at this place...
				 */
			}
		}
		/* Check if a valid IEEE address is available. */
		while ((tal_pib.IeeeAddress == 0x0000000000000000) ||
				(tal_pib.IeeeAddress == 0xFFFFFFFFFFFFFFFF)
				);
	} else {
		/* Valid IEEE address, so no need to generate a new random seed.
		 **/
		uint16_t cur_rand_seed = (uint16_t)tal_pib.IeeeAddress;
		srand(cur_rand_seed);
	}

#else

	/*
	 * No check for a valid IEEE address done, so directly create a seed
	 * for srand().
	 */
	tal_generate_rand_seed();
#endif

	/*
	 * Do the reset stuff.
	 * Set the default PIBs.
	 */
	if (internal_tal_reset(true) != MAC_SUCCESS) {
		return FAILURE;
	}

	pal_trx_reg_read(RG_IRQ_STATUS); /* clear pending irqs, dummy read */

	/*
	 * Configure interrupt handling.
	 * Install a handler for the transceiver interrupt.
	 */
	pal_trx_irq_init((FUNC_PTR)trx_irq_handler_cb);
	pal_trx_irq_en(); /* Enable transceiver main interrupt. */

	/* Initialize the buffer management module and get a buffer to store
	 *reveived frames. */
	bmm_buffer_init();
	tal_rx_buffer = bmm_buffer_alloc(LARGE_BUFFER_SIZE);

	/* Init incoming frame queue */
#ifdef ENABLE_QUEUE_CAPACITY
	qmm_queue_init(&tal_incoming_frame_queue,
			TAL_INCOMING_FRAME_QUEUE_CAPACITY);
#else
	qmm_queue_init(&tal_incoming_frame_queue);
#endif  /* ENABLE_QUEUE_CAPACITY */

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

	PAL_RST_HIGH();
	PAL_SLP_TR_LOW();

	pal_timer_delay(P_ON_TO_CLKM_AVAILABLE);

	/* apply reset pulse */
	PAL_RST_LOW();
	pal_timer_delay(RST_PULSE_WIDTH_US);
	PAL_RST_HIGH();

	/* Verify that TRX_OFF can be written */
	do {
		if (poll_counter == 0xFF) {
			return FAILURE;
		}

		/* Wait a short time interval. */
		pal_timer_delay(TRX_POLL_WAIT_TIME_US);
		poll_counter++;
		/* Check if AT86RF230 is connected; omit manufacturer id check
		 **/
	} while ((pal_trx_reg_read(RG_VERSION_NUM) != AT86RF230_REV_B) ||
			(pal_trx_reg_read(RG_PART_NUM) != AT86RF230));

	pal_trx_reg_write(RG_TRX_STATE, CMD_TRX_OFF);

	/* verify that trx has reached TRX_OFF */
	poll_counter = 0;
	do {
		trx_status = (tal_trx_status_t)pal_trx_bit_read(SR_TRX_STATUS);
		if (poll_counter == 0xFF) {
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
	/* tal_beacon_transmission = false; // not used by 230B TAL */
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
	/* Set pin driver strength */
	pal_trx_reg_write(RG_TRX_CTRL_0, ((CLKM_2mA << 6) |
			(CLKM_2mA << 4) | CLKM_1MHz));               /* fast
	                                                              * change */

	/*
	 * Init the SEED value of the CSMA backoff algorithm.
	 */
	uint16_t rand_value = (uint16_t)rand();
	pal_trx_reg_write(RG_CSMA_SEED_0, (uint8_t)rand_value);
	pal_trx_bit_write(SR_CSMA_SEED_1, (uint8_t)(rand_value >> 8));

	/*
	 * To make sure that the CSMA seed is properly set within the
	 *transceiver,
	 * put the trx to sleep briefly and wake it up again.
	 */
	tal_trx_sleep(SLEEP_MODE_1);
	tal_trx_wakeup();

	pal_trx_bit_write(SR_AACK_SET_PD, PD_ACK_BIT_SET_ENABLE); /* 1 == frame
	                                                           *pending bit
	                                                           *is always
	                                                           *set to 1 */
	pal_trx_bit_write(SR_TX_AUTO_CRC_ON, TX_AUTO_CRC_ENABLE); /* enable auto
	                                                           *crc */
	pal_trx_reg_write(RG_IRQ_MASK, TRX_IRQ_TRX_END); /* enable TRX_END
	                                                  *interrupt */

#ifdef CCA_ED_THRESHOLD

	/*
	 * Set CCA ED Threshold to other value than standard register due to
	 * board specific loss (see pal_config.h). */
	pal_trx_bit_write(SR_CCA_ED_THRES, CCA_ED_THRESHOLD);
#endif
}

/**
 * \brief Reset transceiver
 */
static retval_t trx_reset(void)
{
	tal_trx_status_t trx_status;
	uint8_t poll_counter = 0;
#if (EXTERN_EEPROM_AVAILABLE == 1)
	uint8_t xtal_trim_value;
#endif

	/* Get trim value for 16 MHz xtal; needs to be done before reset */
#if (EXTERN_EEPROM_AVAILABLE == 1)
	pal_ps_get(EXTERN_EEPROM, EE_XTAL_TRIM_ADDR, 1, &xtal_trim_value);
#endif

	/* trx might sleep, so wake it up */
	PAL_SLP_TR_LOW();
	pal_timer_delay(SLEEP_TO_TRX_OFF_US);

	PAL_RST_LOW();
	pal_timer_delay(RST_PULSE_WIDTH_US);
	PAL_RST_HIGH();

	/* verify that trx has reached TRX_OFF */
	do {
		trx_status = (tal_trx_status_t)pal_trx_bit_read(SR_TRX_STATUS);
		poll_counter++;
		if (poll_counter > 250) {
#if (_DEBUG_ > 0)
			Assert(
					"MAX Attempts to switch to TRX_OFF state reached" ==
					0);
#endif
			return FAILURE;
		}
	} while (trx_status != TRX_OFF);

	tal_trx_status = TRX_OFF;

	/* Write 16MHz xtal trim value to trx. */
	/* It's only necessary if it differs from the reset value. */
#if (EXTERN_EEPROM_AVAILABLE == 1)
	if (xtal_trim_value != 0x00) {
		pal_trx_bit_write(SR_XTAL_TRIM, xtal_trim_value);
	}

#endif

	return MAC_SUCCESS;
}

/*
 * \brief Resets TAL state machine and sets the default PIB values if requested
 *
 * \param set_default_pib Defines whether PIB values need to be set
 *                        to its default values
 */
retval_t tal_reset(bool set_default_pib)
{
	/*
	 * Do the reset stuff.
	 * Set the default PIBs depending on the given parameter
	 *set_default_pib.
	 */
	if (internal_tal_reset(set_default_pib) != MAC_SUCCESS) {
		return FAILURE;
	}

#if (MAC_SCAN_ED_REQUEST_CONFIRM == 1)
	/* Stop the ED sample timer. */
#ifdef ENABLE_HIGH_PRIO_TMR
	pal_stop_high_priority_timer(TAL_ED_SAMPLE_TIMER);
#endif  /* ENABLE_HIGH_PRIO_TMR */
#endif  /* (MAC_SCAN_ED_REQUEST_CONFIRM == 1) */

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

	/*
	 * Configure interrupt handling.
	 * Install a handler for the transceiver interrupt.
	 */
	pal_trx_irq_init((FUNC_PTR)trx_irq_handler_cb);
	/* The pending transceiver interrupts on the microcontroller are
	 *cleared. */
	pal_trx_irq_flag_clr();
	pal_trx_irq_en(); /* Enable transceiver main interrupt. */

#ifdef ENABLE_FTN_PLL_CALIBRATION
	{
		/* Handle PLL calibration and filter tuning. */
		retval_t timer_status;

		/* Calibration timer has already been stopped within this
		 *function. */

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

	return MAC_SUCCESS;
}

/*
 * \brief Generates a 16-bit random number used as initial seed for srand()
 */
void tal_generate_rand_seed(void)
{
	/*
	 * For AT86RF230B  bysed systems the random seed is generated by reading
	 *the random contents of the frame buffer after initialization
	 *	since there is no tranceiver based random number generator
	 * available.
	 */
	static uint8_t data[127];
	static uint16_t seed;

	pal_trx_frame_read(data, 127);

	for (uint8_t i = 0; i < 127; i++) {
		seed = seed + data[i];
	}
	srand(seed);
}

static retval_t tal_timer_init(void)
{
#if ((MAC_SCAN_ED_REQUEST_CONFIRM == 1) && (defined BEACON_SUPPORT))
#ifdef ENABLE_FTN_PLL_CALIBRATION
	if (MAC_SUCCESS != pal_timer_get_id(&TAL_ACK_WAIT_TIMER)) {
		return FAILURE;
	}

	if (MAC_SUCCESS != pal_timer_get_id(&TAL_ED_SCAN_DURATION_TIMER)) {
		return FAILURE;
	}

	if (MAC_SUCCESS != pal_timer_get_id(&TAL_ED_SAMPLE_TIMER)) {
		return FAILURE;
	}

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
	if (MAC_SUCCESS != pal_timer_get_id(&TAL_ACK_WAIT_TIMER)) {
		return FAILURE;
	}

	if (MAC_SUCCESS != pal_timer_get_id(&TAL_ED_SCAN_DURATION_TIMER)) {
		return FAILURE;
	}

	if (MAC_SUCCESS != pal_timer_get_id(&TAL_ED_SAMPLE_TIMER)) {
		return FAILURE;
	}

	if (MAC_SUCCESS != pal_timer_get_id(&TAL_CSMA_CCA)) {
		return FAILURE;
	}

	if (MAC_SUCCESS != pal_timer_get_id(&TAL_CSMA_BEACON_LOSS_TIMER)) {
		return FAILURE;
	}
#endif  /* ENABLE_FTN_PLL_CALIBRATION */
#endif

	/* ED Scan is switched on */
	/* No beacon support */
#if ((MAC_SCAN_ED_REQUEST_CONFIRM == 1) && (!defined BEACON_SUPPORT))
#ifdef ENABLE_FTN_PLL_CALIBRATION
	if (MAC_SUCCESS != pal_timer_get_id(&TAL_ED_SCAN_DURATION_TIMER)) {
		return FAILURE;
	}

	if (MAC_SUCCESS != pal_timer_get_id(&TAL_ED_SAMPLE_TIMER)) {
		return FAILURE;
	}

	if (MAC_SUCCESS != pal_timer_get_id(&TAL_CALIBRATION)) {
		return FAILURE;
	}

#else
	if (MAC_SUCCESS != pal_timer_get_id(&TAL_ED_SCAN_DURATION_TIMER)) {
		return FAILURE;
	}

	if (MAC_SUCCESS != pal_timer_get_id(&TAL_ED_SAMPLE_TIMER)) {
		return FAILURE;
	}
#endif  /* ENABLE_FTN_PLL_CALIBRATION */
#endif

	/* ED Scan is switched off */
	/* Network with beacon support */
#if ((MAC_SCAN_ED_REQUEST_CONFIRM == 0) && (defined BEACON_SUPPORT))
#ifdef ENABLE_FTN_PLL_CALIBRATION
	if (MAC_SUCCESS != pal_timer_get_id(&TAL_ACK_WAIT_TIMER)) {
		return FAILURE;
	}

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
	if (MAC_SUCCESS != pal_timer_get_id(&TAL_ACK_WAIT_TIMER)) {
		return FAILURE;
	}

	if (MAC_SUCCESS != pal_timer_get_id(&TAL_CSMA_CCA)) {
		return FAILURE;
	}

	if (MAC_SUCCESS != pal_timer_get_id(&TAL_CSMA_BEACON_LOSS_TIMER)) {
		return FAILURE;
	}
#endif  /* ENABLE_FTN_PLL_CALIBRATION */
#endif

	/* ED Scan is switched off */
	/* No beacon support */
#if ((MAC_SCAN_ED_REQUEST_CONFIRM == 0) && (!defined BEACON_SUPPORT))
#ifdef ENABLE_FTN_PLL_CALIBRATION
	if (MAC_SUCCESS != pal_timer_get_id(&TAL_CALIBRATION)) {
		return FAILURE;
	}
#endif  /* ENABLE_FTN_PLL_CALIBRATION */
#endif
	return MAC_SUCCESS;
}

static void tal_timers_stop(void)
{
#if (NUMBER_OF_TAL_TIMERS > 0)
#if ((MAC_SCAN_ED_REQUEST_CONFIRM == 1) && (defined BEACON_SUPPORT))
#ifdef ENABLE_FTN_PLL_CALIBRATION
	pal_timer_stop(TAL_ACK_WAIT_TIMER);
	pal_timer_stop(TAL_ED_SCAN_DURATION_TIMER);
	pal_timer_stop(TAL_ED_SAMPLE_TIMER);
	pal_timer_stop(TAL_CSMA_CCA);
	pal_timer_stop(TAL_CSMA_BEACON_LOSS_TIMER);
	pal_timer_stop(TAL_CALIBRATION);
#else
	pal_timer_stop(TAL_ACK_WAIT_TIMER);
	pal_timer_stop(TAL_ED_SCAN_DURATION_TIMER);
	pal_timer_stop(TAL_ED_SAMPLE_TIMER);
	pal_timer_stop(TAL_CSMA_CCA);
	pal_timer_stop(TAL_CSMA_BEACON_LOSS_TIMER);
#endif  /* ENABLE_FTN_PLL_CALIBRATION */
#endif

	/* ED Scan is switched on */
	/* No beacon support */
#if ((MAC_SCAN_ED_REQUEST_CONFIRM == 1) && (!defined BEACON_SUPPORT))
#ifdef ENABLE_FTN_PLL_CALIBRATION
	pal_timer_stop(TAL_ED_SCAN_DURATION_TIMER);
	pal_timer_stop(TAL_ED_SAMPLE_TIMER);
	pal_timer_stop(TAL_CALIBRATION);
#else
	pal_timer_stop(TAL_ED_SCAN_DURATION_TIMER);
	pal_timer_stop(TAL_ED_SAMPLE_TIMER);
#endif  /* ENABLE_FTN_PLL_CALIBRATION */
#endif

	/* ED Scan is switched off */
	/* Network with beacon support */
#if ((MAC_SCAN_ED_REQUEST_CONFIRM == 0) && (defined BEACON_SUPPORT))
#ifdef ENABLE_FTN_PLL_CALIBRATION
	pal_timer_stop(TAL_ACK_WAIT_TIMER);
	pal_timer_stop(TAL_CSMA_CCA);
	pal_timer_stop(TAL_CSMA_BEACON_LOSS_TIMER);
	pal_timer_stop(TAL_CALIBRATION);
#else
	pal_timer_stop(TAL_ACK_WAIT_TIMER);
	pal_timer_stop(TAL_CSMA_CCA);
	pal_timer_stop(TAL_CSMA_BEACON_LOSS_TIMER);
#endif  /* ENABLE_FTN_PLL_CALIBRATION */
#endif

	/* ED Scan is switched off */
	/* No beacon support */
#if ((MAC_SCAN_ED_REQUEST_CONFIRM == 0) && (!defined BEACON_SUPPORT))
#ifdef ENABLE_FTN_PLL_CALIBRATION
	pal_timer_stop(TAL_CALIBRATION);
#endif  /* ENABLE_FTN_PLL_CALIBRATION */
#endif
#endif /* (NUMBER_OF_TAL_TIMERS > 0) */
}

/* EOF */
