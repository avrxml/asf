/**
 * @file tal_init.c
 *
 * @brief This file implements functions for initializing TAL and reset
 *
 * Copyright (c) 2015 Atmel Corporation. All rights reserved.
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
 * Copyright (c) 2015, Atmel Corporation All rights reserved.
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
#include "stack_config.h"
#include "bmm.h"
#include "qmm.h"
#include "pal.h"
#include "tal.h"
#include "tal_internal.h"
#include "tal_config.h"
#ifdef ENABLE_TFA
#include "tfa.h"
#endif
#include "mac_build_config.h"
#ifdef IQ_RADIO
#include "pal_internal.h"
#endif

/* === MACROS ============================================================== */

/* === GLOBALS ============================================================= */

/* === PROTOTYPES ========================================================== */

/**
 * \brief Initializes all timers used by the TAL module by assigning id's to
 * each of them
 */
static retval_t tal_timer_init(void);

/**
 * \brief Stops all initialized TAL timers
 */

static void cleanup_tal(trx_id_t trx_id);
static void trx_init(void);

uint8_t TAL_T_0;
uint8_t TAL_T_1;

#ifdef ENABLE_FTN_PLL_CALIBRATION

uint8_t TAL_T_CALIBRATION_0;
uint8_t TAL_T_CALIBRATION_1;

#endif

#if ((defined RF215v1) && (defined SUPPORT_LEGACY_OQPSK))

uint8_t TAL_T_AGC_0;
uint8_t TAL_T_AGC_1;

#endif

/* === IMPLEMENTATION ====================================================== */

/**
 * @brief Initializes the TAL
 *
 * This function is called to initialize the TAL. The transceiver is
 * initialized, the TAL PIBs are set to their default values, and the TAL state
 * machine is set to TAL_IDLE state.
 *
 * @return MAC_SUCCESS  if the transceiver state is changed to TRX_OFF and the
 *                 current device part number and version number are correct;
 *         FAILURE otherwise
 */
retval_t tal_init(void)
{
	/* Init the PAL and by this means also the transceiver interface */
	if (pal_init() != MAC_SUCCESS) {
		return FAILURE;
	}

	/* Reset trx */
	if (trx_reset(RFBOTH) != MAC_SUCCESS) {
		return FAILURE;
	}

	/* Check if RF215 is connected */
#if (defined RF215v1) || (defined RF215v3)
	if ((trx_reg_read( RG_RF_PN) != 0x34) ||
#else
#   error "No part number defined"
#endif
#if (defined RF215v1)
			(trx_reg_read( RG_RF_VN) != 0x01))
#elif (defined RF215v3)
			(trx_reg_read( RG_RF_VN) != 0x03))
#else
#   error "No IC version defined"
#endif
	{
		/*Wrong PN or VN*/
		return FAILURE;
	}

	/* Initialize trx */
	trx_init();

	if (tal_timer_init() != MAC_SUCCESS) {
		return FAILURE;
	}

	/* Initialize the buffer management */
	bmm_buffer_init();

	/* Configure both trx and set default PIB values */
	for (trx_id_t trx_id = (trx_id_t)0; trx_id < NUM_TRX; trx_id++) {
		/* Configure transceiver */
		trx_config(trx_id);
#ifdef RF215v1
		/* Calibrate LO */
		calibrate_LO(trx_id);
#endif

		/* Set the default PIB values */
		init_tal_pib(trx_id); /* see 'tal_pib.c' */
		calculate_pib_values(trx_id);

		/*
		 * Write all PIB values to the transceiver
		 * that are needed by the transceiver itself.
		 */
		write_all_tal_pib_to_trx(trx_id); /* see 'tal_pib.c' */
		config_phy(trx_id);

		tal_rx_buffer[trx_id] = bmm_buffer_alloc(LARGE_BUFFER_SIZE);
		if (tal_rx_buffer[trx_id] == NULL) {
			return FAILURE;
		} else {
			/* Fill trx_id in new buffer */
			frame_info_t *frm_info
				= (frame_info_t *)BMM_BUFFER_POINTER(
					tal_rx_buffer[trx_id]);
			frm_info->trx_id = trx_id;
		}

		/* Init incoming frame queue */
		qmm_queue_init(&tal_incoming_frame_queue[trx_id]);

		tal_state[trx_id] = TAL_IDLE;
		tx_state[trx_id] = TX_IDLE;
	}

	/* Init seed of rand() */
	tal_generate_rand_seed();

	#ifndef DISABLE_IEEE_ADDR_CHECK
	for (uint8_t trx_id = 0; trx_id < 2; trx_id++) {
		/* Check if a valid IEEE address is available. */

		/*
		 * This while loop is on purpose, since just in the
		 * rare case that such an address is randomly
		 * generated again, we must repeat this.
		 */
		while ((tal_pib[trx_id].IeeeAddress == 0x0000000000000000) ||
				(tal_pib[trx_id].IeeeAddress ==
				((uint64_t)-1))) {
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
			uint8_t *ptr_pib
				= (uint8_t *)&tal_pib[trx_id].IeeeAddress;

			for (uint8_t i = 0; i < 8; i++) {
				*ptr_pib++ = (uint8_t)rand();

				/*
				 * Note:
				 * Even if casting the 16 bit rand value back to
				 * 8 bit,
				 * and running the loop 8 timers (instead of
				 * only 4 times)
				 * may look cumbersome, it turns out that the
				 * code gets
				 * smaller using 8-bit here.
				 * And timing is not an issue at this place...
				 */
			}
		}
	}
#endif

#ifdef IQ_RADIO
	/* Init BB IRQ handler */
	pal_dev_irq_flag_clr(RF215_BB);
	pal_dev_irq_init(RF215_BB, bb_irq_handler_cb);
	pal_dev_irq_en(RF215_BB);

	/* Init RF IRQ handler */
	pal_dev_irq_flag_clr(RF215_RF);
	pal_dev_irq_init(RF215_RF, rf_irq_handler_cb);
	pal_dev_irq_en(RF215_RF);
#else

	/*
	 * Configure interrupt handling.
	 * Install a handler for the radio and the baseband interrupt.
	 */
	pal_trx_irq_flag_clr();
	trx_irq_init((FUNC_PTR)trx_irq_handler_cb);
	pal_trx_irq_en(); /* Enable transceiver main interrupt. */
#endif

#if ((defined SUPPORT_FSK) && (defined SUPPORT_MODE_SWITCH))
	init_mode_switch();
#endif

	return MAC_SUCCESS;
} /* tal_init() */

/**
 * @brief Configures the transceiver
 *
 * This function is called to configure a certain transceiver (RF09 or RF24)
 * after trx sleep or reset or power on.
 *
 * @param trx_id Transceiver identifier
 */
void trx_config(trx_id_t trx_id)
{
	uint16_t reg_offset = RF_BASE_ADDR_OFFSET * trx_id;

#ifdef IQ_RADIO
	/* LVDS interface */
	/* RF part: RF enable, BB disabled, IQIF enabled */
	trx_bit_write(RF215_RF, SR_RF_IQIFC1_CHPM, 0x01);
	/* BB part: RF disable, BB enabled, IQIF enabled */
	trx_bit_write(RF215_BB, SR_RF_IQIFC1_CHPM, 0x03);
	/* Clock Phase I/Q IF Driver at BB */
	trx_bit_write(RF215_BB, SR_RF_IQIFC2_CPHADRV, 0);
	/* Clock Phase I/Q IF Receiver at BB */
	trx_bit_write(RF215_BB, SR_RF_IQIFC2_CPHAREC, 1);
	/* Enable embedded control at RF */
	trx_bit_write(RF215_RF, SR_RF_IQIFC0_EEC, 1);
	/* Configure BB */
	/* Setup IRQ mask: in chip mode, the baseband controls the RF's AGC */
	trx_reg_write(RF215_BB, reg_offset + RG_BBC0_IRQM,
			BB_IRQ_RXFE | BB_IRQ_TXFE | BB_IRQ_RXFS | BB_IRQ_AGCR |
			BB_IRQ_AGCH);
	trx_reg_write(RF215_BB, reg_offset + RG_RF09_IRQM, RF_IRQ_NO_IRQ);
	/* Configure RF */
	trx_reg_write(RF215_RF, reg_offset + RG_BBC0_IRQM, BB_IRQ_NO_IRQ);
	trx_reg_write(RF215_RF, reg_offset + RG_RF09_IRQM,
			RF_IRQ_IQIFSF | RF_IRQ_TRXERR | RF_IRQ_EDC | RF_IRQ_BATLOW |
			RF_IRQ_WAKEUP);
#else /* transceiver mode */
	/* Configure BB */
	/* Setup IRQ mask */
#ifdef ENABLE_TSTAMP
	trx_reg_write( reg_offset + RG_BBC0_IRQM,
			BB_IRQ_RXFE | BB_IRQ_TXFE | BB_IRQ_RXFS);
#else
	trx_reg_write( reg_offset + RG_BBC0_IRQM,
			BB_IRQ_RXFE | BB_IRQ_TXFE);
#endif
	/* Configure RF */
	trx_reg_write( reg_offset + RG_RF09_IRQM, RF_IRQ_BATLOW |
			RF_IRQ_WAKEUP);
#endif /* #ifdef IQ_RADIO */

#if (defined IQ_RADIO)
	/* Set clip detector OFF */
	uint8_t agcc = trx_reg_read(RF215_RF, reg_offset + RG_RF09_AGCC);
	agcc |= 0x80;
	trx_reg_write(RF215_RF, reg_offset + RG_RF09_AGCC, agcc);
#endif

	/* Enable frame filter */
	trx_bit_write( reg_offset + SR_BBC0_AFC0_AFEN0, 1);

#ifndef BASIC_MODE
#if (defined MEASURE_TIME_OF_FLIGHT) && (!defined IQ_RADIO)
	/* Enable automatic time of flight measurement */
	/* bit 3 CAPRXS, bit 2 RSTTXS, bit 0 EN */
	uint8_t cnt_cfg = CNTC_EN_MASK | CNTC_RSTTXS_MASK | CNTC_CAPRXS_MASK;
	trx_reg_write(reg_offset + RG_BBC0_CNTC, cnt_cfg);
#endif /* #if (defined MEASURE_TIME_OF_FLIGHT) && (!defined IQ_RADIO) */
#else /* BASIC_MODE */
	/* Enable counter for ACK timing: EN | RSTRXS */
	uint8_t cntc = CNTC_EN_MASK | CNTC_RSTRXS_MASK;
	trx_reg_write(reg_offset + RG_BBC0_CNTC, cntc);
#endif

#ifndef USE_TXPREP_DURING_BACKOFF
	/* Keep analog voltage regulator on during TRXOFF */
#ifdef IQ_RADIO
	trx_bit_write(RF215_RF, reg_offset + SR_RF09_AUXS_AVEN, 1);
#else
	trx_bit_write(reg_offset + SR_RF09_AUXS_AVEN, 1);
#endif /* #ifdef IQ_RADIO */
#endif

#ifndef BASIC_MODE
	/* Enable AACK */
	trx_reg_write(reg_offset + RG_BBC0_AMCS, AMCS_AACK_MASK);
	/* Set data pending for ACK frames to 1 for all address units */
	trx_reg_write(reg_offset + RG_BBC0_AMAACKPD, 0x0F);
#endif

#ifdef SUPPORT_MODE_SWITCH
	/* Use raw mode for mode switch PPDU in the not-inverted manner */
	trx_bit_write(reg_offset + SR_BBC0_FSKC4_RAWRBIT, 0);
#endif
}

/**
 * @brief Initializes the transceiver
 *
 * This function is called to initialize the general transceiver functionality
 * after power or IC reset.
 */
static void trx_init(void)
{
	/*
	 * Configure generic trx functionality
	 * Configure Trx registers that are reset during DEEP_SLEEP and IC reset
	 * I.e.: RF_CFG, RF_BMDVC, RF_XOC, RF_IQIFC0, RF_IQIFC1, RF_IQIFC2
	 */
#ifdef TRX_IRQ_POLARITY
#if (TRX_IRQ_POLARITY == 0)
	trx_bit_write( SR_RF_CFG_IRQP, 0); /* 1 = active low */
#endif
#endif
#if (TRX_CLOCK_OUTPUT_SELECTION != 1)
#ifdef IQ_RADIO
	trx_bit_write(RF215_RF, SR_RF_CLKO_OS, TRX_CLOCK_OUTPUT_SELECTION);
#else
	trx_bit_write( SR_RF_CLKO_OS, TRX_CLOCK_OUTPUT_SELECTION);
#endif
#endif
}

/**
 * @brief Resets TAL state machine and sets the default PIB values if requested
 *
 * @param trx_id Transceiver identifier
 * @param set_default_pib Defines whether PIB values need to be set
 *                        to its default values
 *
 * @return
 *      - @ref MAC_SUCCESS if the transceiver state is changed to TRX_OFF
 *      - @ref FAILURE otherwise
 * @ingroup apiTalApi
 */
retval_t tal_reset(trx_id_t trx_id, bool set_default_pib)
{
	rf_cmd_state_t previous_trx_state[NUM_TRX];

	/* Clean TAL and removed any pending tasks */
	if (trx_id == RFBOTH) {
		for (trx_id_t i = (trx_id_t)0; i < NUM_TRX; i++) {
			/* Clean TAL and removed any pending tasks */
			cleanup_tal(i);
		}
	} else {
		cleanup_tal(trx_id);
	}

	previous_trx_state[RF09] = trx_state[RF09];
	previous_trx_state[RF24] = trx_state[RF24];

	/* Reset the actual device or part of the device */
	if (trx_reset(trx_id) != MAC_SUCCESS) {
		return FAILURE;
	}

	/* Init Trx if necessary, e.g. trx was in deep sleep */
	if (((previous_trx_state[RF09] == RF_SLEEP) &&
			(previous_trx_state[RF24] == RF_SLEEP)) ||
			(trx_id == RFBOTH)) {
		trx_init(); /* Initialize generic trx functionality */
	}

	if (trx_id == RFBOTH) {
		for (trx_id_t i = (trx_id_t)0; i < NUM_TRX; i++) {
			/* Configure the transceiver register values. */
			trx_config(i);

			if (set_default_pib) {
				/* Set the default PIB values */
				init_tal_pib(i); /* see 'tal_pib.c' */
				calculate_pib_values(i);
			} else {
				/* nothing to do - the current TAL PIB attribute
				 *values are used */
			}

			write_all_tal_pib_to_trx(i); /* see 'tal_pib.c' */
			config_phy(i);

			/* Reset TAL variables. */
			tal_state[i] = TAL_IDLE;
			tx_state[i] = TX_IDLE;
		}
	} else {
		/* Configure the transceiver register values. */
		trx_config(trx_id);

		if (set_default_pib) {
			/* Set the default PIB values */
			init_tal_pib(trx_id); /* see 'tal_pib.c' */
			calculate_pib_values(trx_id);
		} else {
			/* nothing to do - the current TAL PIB attribute values
			 *are used */
		}

		write_all_tal_pib_to_trx(trx_id); /* see 'tal_pib.c' */
		config_phy(trx_id);

		/* Reset TAL variables. */
		tal_state[trx_id] = TAL_IDLE;
		tx_state[trx_id] = TX_IDLE;
	}

	/*
	 * Configure interrupt handling.
	 * Install a handler for the transceiver interrupt.
	 */
#ifdef IQ_RADIO
	pal_dev_irq_init(RF215_BB, bb_irq_handler_cb);
	pal_dev_irq_init(RF215_RF, rf_irq_handler_cb);
	pal_dev_irq_en(RF215_BB); /* Enable transceiver main interrupt. */
	pal_dev_irq_en(RF215_RF); /* Enable transceiver main interrupt. */
#else
	trx_irq_init((FUNC_PTR)trx_irq_handler_cb);
	pal_trx_irq_en(); /* Enable transceiver main interrupt. */
#endif

	return MAC_SUCCESS;
}

/**
 * @brief Resets transceiver(s)
 *
 * @param trx_id Transceiver identifier
 *
 * @return MAC_SUCCESS  if the transceiver returns TRX_OFF
 *         FAILURE otherwise
 */
retval_t trx_reset(trx_id_t trx_id)
{
	retval_t status = MAC_SUCCESS;

	ENTER_TRX_REGION();

	uint32_t start_time;
	uint32_t current_time;
	pal_get_current_time(&start_time);

	/* Trigger reset */
	if (trx_id == RFBOTH) {
		TAL_RF_IRQ_CLR_ALL(RF09);
		TAL_RF_IRQ_CLR_ALL(RF24);

		tal_state[RF09] = TAL_RESET;
		tal_state[RF24] = TAL_RESET;

		/* Apply reset pulse; low active */
		/* Apply reset pulse; low active */
#ifdef IQ_RADIO
		RST_LOW();
		PAL_WAIT_1_US();
		PAL_WAIT_1_US();
		RST_HIGH();
		RST_LOW();
		PAL_WAIT_1_US();
		RST_HIGH();
#else
		RST_LOW();
		PAL_WAIT_1_US();
		RST_HIGH();
#endif
	} else { /* only a single trx_id; i.e. RF09 or RF24 */
		TAL_RF_IRQ_CLR_ALL(trx_id);

		/* Trigger reset of device */
		uint16_t reg_offset = RF_BASE_ADDR_OFFSET * trx_id;
#ifdef IQ_RADIO
		trx_reg_write(RF215_RF, reg_offset + RG_RF09_CMD, RF_RESET);
		trx_reg_write(RF215_BB, reg_offset + RG_RF09_CMD, RF_RESET);
#else
		trx_reg_write( reg_offset + RG_RF09_CMD, RF_RESET);
#endif
	}

	/* Wait for IRQ line */
	while (1) {
		/*
		 * @ToDo: Use a different macro for IRQ line; the polarity might
		 *be
		 * different after reset
		 */
#ifdef IQ_RADIO
		if ((PAL_DEV_IRQ_GET(RF215_BB) == HIGH) &&
				(PAL_DEV_IRQ_GET(RF215_RF) == HIGH))
#else
		if (TRX_IRQ_GET() == HIGH)
#endif
		{
			break;
		}

		/* Handle timeout */
		pal_get_current_time(&current_time);
		/* @ToDo: Remove magic number */
		if (pal_sub_time_us(current_time, start_time) > 1000) {
			status = FAILURE;
			break;
		}
	}

	/* Check for trx status */
	if (status == MAC_SUCCESS) { /* IRQ has been fired and no timeout has
	                              * occurred */
		if (trx_id == RFBOTH) {
			for (uint8_t i = (trx_id_t)0; i < NUM_TRX; i++) {
				uint16_t reg_offset = RF_BASE_ADDR_OFFSET * i;
				trx_state[i] = (rf_cmd_state_t)trx_reg_read(
						reg_offset + RG_RF09_STATE);
#ifdef IQ_RADIO
				rf_cmd_state_t bb_state;
				bb_state = (rf_cmd_state_t)trx_reg_read(
						RF215_BB,
						reg_offset + RG_RF09_STATE);
				if ((trx_state[i] != RF_TRXOFF) ||
						(bb_state != RF_TRXOFF))
#else
				if (trx_state[i] != RF_TRXOFF)
#endif
				{
					status = FAILURE;
				}
			}
		} else { /* single trx */
			uint16_t reg_offset = RF_BASE_ADDR_OFFSET * trx_id;
			trx_state[trx_id] = (rf_cmd_state_t)trx_reg_read(
					reg_offset + RG_RF09_STATE);
#ifdef IQ_RADIO
			rf_cmd_state_t bb_state;
			bb_state = (rf_cmd_state_t)trx_reg_read(RF215_BB,
					reg_offset +
					RG_RF09_STATE);
			if ((trx_state[trx_id] != RF_TRXOFF) ||
					(bb_state != RF_TRXOFF))
#else
			if (trx_state[trx_id] != RF_TRXOFF)
#endif
			{
				status = FAILURE;
			}
		}
	}

	/* Get all IRQ status information */
#ifdef IQ_RADIO
	bb_irq_handler_cb();
	rf_irq_handler_cb();
#else
	trx_irq_handler_cb();
#endif
	if (trx_id == RFBOTH) {
		for (uint8_t i = (trx_id_t)0; i < NUM_TRX; i++) {
			TAL_RF_IRQ_CLR(i, RF_IRQ_WAKEUP);
		}
	} else { /* single trx */
		TAL_RF_IRQ_CLR(trx_id, RF_IRQ_WAKEUP);
	}

#ifdef IQ_RADIO
	pal_trx_irq_flag_clr(RF215_BB);
	pal_trx_irq_flag_clr(RF215_RF);
#else
	pal_trx_irq_flag_clr();
#endif

	LEAVE_TRX_REGION();

	return status;
}

/**
 * @brief Cleanup TAL
 *
 * @param trx_id Transceiver identifier
 */
static void cleanup_tal(trx_id_t trx_id)
{
	/* Clear all running TAL timers (for this trx id). */
	ENTER_CRITICAL_REGION();

	stop_tal_timer(trx_id);

	LEAVE_CRITICAL_REGION();

	/* Clear TAL Incoming Frame queue and free used buffers. */
	while (tal_incoming_frame_queue[trx_id].size > 0) {
		buffer_t *frame
			= qmm_queue_remove(
				&tal_incoming_frame_queue[trx_id], NULL);
		if (NULL != frame) {
			bmm_buffer_free(frame);
		}
	}
	/* Get new TAL Rx buffer if necessary */
	if (tal_rx_buffer[trx_id] == NULL) {
		tal_rx_buffer[trx_id] = bmm_buffer_alloc(LARGE_BUFFER_SIZE);
	}

	/* Handle buffer shortage */
	if (tal_rx_buffer[trx_id] == NULL) {
		tal_buf_shortage[trx_id] = true;
	} else {
		tal_buf_shortage[trx_id] = false;
		/* Fill trx_id in new buffer */
		frame_info_t *frm_info = (frame_info_t *)BMM_BUFFER_POINTER(
				tal_rx_buffer[trx_id]);
		frm_info->trx_id = trx_id;
	}
}

static retval_t tal_timer_init(void)
{
	if (MAC_SUCCESS != pal_timer_get_id(&TAL_T_0)) {
		return FAILURE;
	}

	if (MAC_SUCCESS != pal_timer_get_id(&TAL_T_1)) {
		return FAILURE;
	}

	#ifdef ENABLE_FTN_PLL_CALIBRATION
	if (MAC_SUCCESS != pal_timer_get_id(&TAL_T_CALIBRATION_0)) {
		return FAILURE;
	}

	if (MAC_SUCCESS != pal_timer_get_id(&TAL_T_CALIBRATION_1)) {
		return FAILURE;
	}

	#endif  /* ENABLE_FTN_PLL_CALIBRATION */

	#if ((defined RF215v1) && (defined SUPPORT_LEGACY_OQPSK))
	if (MAC_SUCCESS != pal_timer_get_id(&TAL_T_AGC_0)) {
		return FAILURE;
	}

	if (MAC_SUCCESS != pal_timer_get_id(&TAL_T_AGC_1)) {
		return FAILURE;
	}

    #endif
	return MAC_SUCCESS;
}

/* EOF */
