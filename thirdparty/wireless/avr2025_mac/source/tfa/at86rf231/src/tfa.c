/**
 * \file tfa.c
 *
 * \brief Implementation of Transceiver Feature Access (TFA) functionality.
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

#if (defined ENABLE_TFA) || (defined TFA_BAT_MON)

/* === INCLUDES ============================================================ */

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "pal.h"
#include "return_val.h"
#include "tal.h"
#include "ieee_const.h"
#include "tal_constants.h"
#include "at86rf231.h"
#include "tal_internal.h"
#include "tfa.h"

/* === TYPES =============================================================== */

/* === MACROS ============================================================== */

/* Constant define for the ED scaling: register value at -35dBm */
#define CLIP_VALUE_REG                  (56)

/* === GLOBALS ============================================================= */

#ifdef ENABLE_TFA

/**
 * TFA PIB attribute to reduce the Rx sensitivity.
 * Represents the Rx sensitivity value in dBm; example: -52
 */
static int8_t tfa_pib_rx_sens;
#endif

/* === PROTOTYPES ========================================================== */

#ifdef ENABLE_TFA
static void init_tfa_pib(void);
static void write_all_tfa_pibs_to_trx(void);

#endif

/* === IMPLEMENTATION ====================================================== */

#ifdef ENABLE_TFA

/*
 * \brief Gets a TFA PIB attribute
 *
 * This function is called to retrieve the transceiver information base
 * attributes.
 *
 * \param[in] tfa_pib_attribute TAL infobase attribute ID
 * \param[out] value TFA infobase attribute value
 *
 * \return MAC_UNSUPPORTED_ATTRIBUTE if the TFA infobase attribute is not found
 *         MAC_SUCCESS otherwise
 */
retval_t tfa_pib_get(tfa_pib_t tfa_pib_attribute, void *value)
{
	switch (tfa_pib_attribute) {
	case TFA_PIB_RX_SENS:
		*(uint8_t *)value = tfa_pib_rx_sens;
		break;

	default:
		/* Invalid attribute id */
		return MAC_UNSUPPORTED_ATTRIBUTE;
	}

	return MAC_SUCCESS;
}

#endif

#ifdef ENABLE_TFA

/*
 * \brief Sets a TFA PIB attribute
 *
 * This function is called to set the transceiver information base
 * attributes.
 *
 * \param[in] tfa_pib_attribute TFA infobase attribute ID
 * \param[in] value TFA infobase attribute value to be set
 *
 * \return MAC_UNSUPPORTED_ATTRIBUTE if the TFA info base attribute is not found
 *         TAL_BUSY if the TAL is not in TAL_IDLE state.
 *         MAC_SUCCESS if the attempt to set the PIB attribute was successful
 */
retval_t tfa_pib_set(tfa_pib_t tfa_pib_attribute, void *value)
{
	switch (tfa_pib_attribute) {
	case TFA_PIB_RX_SENS:
	{
		uint8_t reg_val;

		tfa_pib_rx_sens = *((int8_t *)value);
		if (tfa_pib_rx_sens > -49) {
			reg_val = 0xF;
			tfa_pib_rx_sens = -49;
		} else if (tfa_pib_rx_sens <= RSSI_BASE_VAL_DBM) {
			reg_val = 0x0;
			tfa_pib_rx_sens = RSSI_BASE_VAL_DBM;
		} else {
			reg_val
				= ((tfa_pib_rx_sens -
					(RSSI_BASE_VAL_DBM)) / 3) + 1;
		}

		trx_bit_write(SR_RX_PDT_LEVEL, reg_val);
	}
	break;

	default:
		/* Invalid attribute id */
		return MAC_UNSUPPORTED_ATTRIBUTE;
	}

	return MAC_SUCCESS;
}

#endif

#ifdef ENABLE_TFA

/*
 * \brief Initializes the TFA
 *
 * This function is called to initialize the TFA.
 *
 * \return MAC_SUCCESS if everything went correct;
 *         FAILURE otherwise
 */
retval_t tfa_init(void)
{
	init_tfa_pib();
	write_all_tfa_pibs_to_trx();

	return MAC_SUCCESS;
}

#endif

#ifdef ENABLE_TFA

/*
 * \brief Reset the TFA
 *
 * This function is called to reset the TFA.
 *
 * \param set_default_pib Defines whether PIB values need to be set
 *                        to its default values
 */
void tfa_reset(bool set_default_pib)
{
	if (set_default_pib) {
		init_tfa_pib();
	}

	write_all_tfa_pibs_to_trx();
}

#endif

#ifdef ENABLE_TFA

/*
 * \brief Perform a CCA
 *
 * This function performs a CCA request.
 *
 * \return phy_enum_t PHY_IDLE or PHY_BUSY
 */
phy_enum_t tfa_cca_perform(void)
{
	tal_trx_status_t trx_status;
	uint8_t cca_status;
	uint8_t cca_done;

	/* Ensure that trx is not in SLEEP for register access */
	do {
		trx_status = set_trx_state(CMD_TRX_OFF);
	} while (trx_status != TRX_OFF);

	/* no interest in receiving frames while doing CCA */
	trx_bit_write(SR_RX_PDT_DIS, RX_DISABLE); /* disable frame reception
	                                           * indication */

	/* Set trx to rx mode. */
	do {
		trx_status = set_trx_state(CMD_RX_ON);
	} while (trx_status != RX_ON);

	/* Start CCA */
	trx_bit_write(SR_CCA_REQUEST, CCA_START);

	/* wait until CCA is done */
	pal_timer_delay(TAL_CONVERT_SYMBOLS_TO_US(CCA_DURATION_SYM));
	do {
		/* poll until CCA is really done */
		cca_done = trx_bit_read(SR_CCA_DONE);
	} while (cca_done != CCA_DETECTION_DONE);

	set_trx_state(CMD_TRX_OFF);

	/* Check if channel was idle or busy. */
	if (trx_bit_read(SR_CCA_STATUS) == CCA_STATUS_CHANNEL_IS_IDLE) {
		cca_status = PHY_IDLE;
	} else {
		cca_status = PHY_BUSY;
	}

	/* Enable frame reception again. */
	trx_bit_write(SR_RX_PDT_DIS, RX_ENABLE);

	return (phy_enum_t)cca_status;
}

#endif

#ifdef ENABLE_TFA

/*
 * \brief Perform a single ED measurement
 *
 * \return ed_value Result of the measurement
 *         If the build switch TRX_REG_RAW_VALUE is defined, the transceiver's
 *         register value is returned.
 */
uint8_t tfa_ed_sample(void)
{
	trx_irq_reason_t trx_irq_cause;
	uint8_t ed_value;
	tal_trx_status_t trx_status;

	/* Make sure that receiver is switched on. */
	do {
		trx_status = set_trx_state(CMD_RX_ON);
	} while (trx_status != RX_ON);

	/*
	 * Disable the transceiver interrupts to prevent frame reception
	 * while performing ED scan.
	 */
	trx_bit_write(SR_RX_PDT_DIS, RX_DISABLE);

	/* Write dummy value to start measurement. */
	trx_reg_write(RG_PHY_ED_LEVEL, 0xFF);

	/* Wait for ED measurement completion. */
	pal_timer_delay(TAL_CONVERT_SYMBOLS_TO_US(ED_SAMPLE_DURATION_SYM));
	do {
		trx_irq_cause
			= (trx_irq_reason_t)trx_reg_read(RG_IRQ_STATUS);
	} while ((trx_irq_cause & TRX_IRQ_CCA_ED_READY) !=
			TRX_IRQ_CCA_ED_READY);

	/* Read the ED Value. */
	ed_value = trx_reg_read(RG_PHY_ED_LEVEL);

	/* Clear IRQ register */
	trx_reg_read(RG_IRQ_STATUS);
	/* Enable reception agian */
	trx_bit_write(SR_RX_PDT_DIS, RX_ENABLE);
	/* Switch receiver off again */
	set_trx_state(CMD_TRX_OFF);

#ifndef TRX_REG_RAW_VALUE

	/*
	 * Scale ED result.
	 * Clip values to 0xFF if > -35dBm
	 */
	if (ed_value > CLIP_VALUE_REG) {
		ed_value = 0xFF;
	} else {
		ed_value
			= (uint8_t)(((uint16_t)ed_value *
				0xFF) / CLIP_VALUE_REG);
	}
#endif

	return ed_value;
}

#endif

#if (defined ENABLE_TFA) || (defined TFA_BAT_MON)

/*
 * \brief Get the transceiver's supply voltage
 *
 * \return mv Milli Volt; 0 if below threshold, 0xFFFF if above threshold
 */
uint16_t tfa_get_batmon_voltage(void)
{
	tal_trx_status_t previous_trx_status;
	uint8_t vth_val;
	uint8_t i;
	uint16_t mv = 1; /* 1 used as indicator flag */
	bool range;

	previous_trx_status = tal_trx_status;
	if (tal_trx_status == TRX_SLEEP) {
		set_trx_state(CMD_TRX_OFF);
	}

	/*
	 * Disable all trx interrupts.
	 * This needs to be done AFTER the transceiver has been woken up.
	 */
	pal_trx_irq_dis();

	/* Check if supply voltage is within lower range */
	trx_bit_write(SR_BATMON_HR, BATMON_LOW_RANGE);
	trx_bit_write(SR_BATMON_VTH, 0x0F);
	pal_timer_delay(5); /* Wait until Batmon has been settled. */
	if (trx_bit_read(SR_BATMON_OK) == BATMON_BELOW_THRES) {
		/* Lower range */
		/* Check if supply voltage is below lower limit */
		trx_bit_write(SR_BATMON_VTH, 0);
		pal_timer_delay(2); /* Wait until Batmon has been settled. */
		if (trx_bit_read(SR_BATMON_OK) == BATMON_BELOW_THRES) {
			/* below lower limit */
			mv = SUPPLY_VOLTAGE_BELOW_LOWER_LIMIT;
		}

		range = LOW;
	} else {
		/* Higher range */
		trx_bit_write(SR_BATMON_HR, BATMON_HIGH_RANGE);
		/* Check if supply voltage is above upper limit */
		trx_bit_write(SR_BATMON_VTH, 0x0F);
		pal_timer_delay(5); /* Wait until Batmon has been settled. */
		if (trx_bit_read(SR_BATMON_OK) == BATMON_ABOVE_THRES) {
			/* above upper limit */
			mv = SUPPLY_VOLTAGE_ABOVE_UPPER_LIMIT;
		}

		range = HIGH;
	}

	/* Scan through the current range for the matching threshold. */
	if (mv == 1) {
		vth_val = 0x0F;
		for (i = 0; i < 16; i++) {
			trx_bit_write(SR_BATMON_VTH, i);
			pal_timer_delay(2); /* Wait until Batmon has been
			                     * settled. */
			if (trx_bit_read(SR_BATMON_OK) ==
					BATMON_BELOW_THRES) {
				if (i > 0) {
					vth_val = i - 1;
				} else {
					vth_val = i;
				}

				break;
			}
		}

		if (range == HIGH) {
			mv = 2550 + (75 * vth_val);
		} else {
			mv = 1700 + (50 * vth_val);
		}
	}

	trx_reg_read(RG_IRQ_STATUS);

	/*
	 * Enable all trx interrupts.
	 * This needs to be done BEFORE putting the transceiver back to slee.
	 */
	pal_trx_irq_en();

	if (previous_trx_status == TRX_SLEEP) {
		set_trx_state(CMD_SLEEP);
	}

	return mv;
}

#endif  /* #if (defined ENABLE_TFA) || (defined TFA_BAT_MON) */

#ifdef ENABLE_TFA

/**
 * \brief Initialize the TFA PIB
 *
 * This function initializes the TFA information base attributes
 * to their default values.
 * \ingroup group_tfa
 */
static void init_tfa_pib(void)
{
	tfa_pib_rx_sens = TFA_PIB_RX_SENS_DEF;
}

#endif

#ifdef ENABLE_TFA

/**
 * \brief Write all shadow PIB variables to the transceiver
 *
 * This function writes all shadow PIB variables to the transceiver.
 * It is assumed that the radio does not sleep.
 * \ingroup group_tfa
 */
static void write_all_tfa_pibs_to_trx(void)
{
	tfa_pib_set(TFA_PIB_RX_SENS, (void *)&tfa_pib_rx_sens);
}

#endif

#ifdef ENABLE_TFA

/*
 * \brief Starts continuous transmission on current channel
 *
 * \param tx_mode Mode of continuous transmission (CW or PRBS)
 * \param random_content Use random content if true
 */
void tfa_continuous_tx_start(continuous_tx_mode_t tx_mode, bool random_content)
{
	uint8_t txcwdata[128];

	trx_bit_write(SR_TX_AUTO_CRC_ON, TX_AUTO_CRC_DISABLE);
	trx_reg_write(RG_TRX_STATE, CMD_TRX_OFF);
	trx_reg_write(0x36, 0x0F); /*TST_CTRL_DIGI*/

	/* Here: use 2MBPS mode for PSD measurements.
	 * Omit the two following lines, if 250k mode is desired for PRBS mode.
	 **/
	trx_reg_write(RG_TRX_CTRL_2, 0x03);
	trx_reg_write(RG_RX_CTRL, 0xA7);
	if (tx_mode == CW_MODE) {
		txcwdata[0] = 1; /* length */
		/* Step 12 - frame buffer write access */
		txcwdata[1] = 0x00; /* f=fch-0.5 MHz; set value to 0xFF for
		                     * f=fch+0.5MHz */
		trx_frame_write(txcwdata, 2);
	} else { /* PRBS mode */
		txcwdata[0] = 127; /* = max length */
		for (uint8_t i = 1; i < 128; i++) {
			if (random_content) {
				txcwdata[i] = (uint8_t)rand();
			} else {
				txcwdata[i] = 0;
			}
		}
		trx_frame_write(txcwdata, 128);
	}

	trx_reg_write(RG_PART_NUM, 0x54);
	trx_reg_write(RG_PART_NUM, 0x46);
	set_trx_state(CMD_PLL_ON);
	TRX_SLP_TR_HIGH();
	TRX_SLP_TR_LOW();
}

#endif

#ifdef ENABLE_TFA

/*
 * \brief Stops continuous transmission
 */
void tfa_continuous_tx_stop(void)
{
	tal_reset(false);
}

#endif

#endif /* #if (defined ENABLE_TFA) || (defined TFA_BAT_MON) */

/* EOF */
