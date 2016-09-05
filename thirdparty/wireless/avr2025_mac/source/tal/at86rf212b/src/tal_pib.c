/**
 * @file tal_pib.c
 *
 * @brief This file handles the TAL PIB attributes, set/get and initialization
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
#include <stdlib.h>
#include "pal.h"
#include "return_val.h"
#include "tal.h"
#include "ieee_const.h"
#include "tal_constants.h"
#include "tal_pib.h"
#include "at86rf212b.h"
#include "tal_internal.h"
#ifdef ENABLE_TFA
#include "tfa.h"
#endif  /* #ifdef ENABLE_TFA */

/* === TYPES =============================================================== */

/* === MACROS ============================================================== */

/**
 * \addtogroup group_tal_pib
 * @{
 */

#define GET_CHINA_FREQ(x)           (11 + (2 * x))

/* === GLOBALS ============================================================= */

/**
 * Tx power table in flash.
 */
static FLASH_DECLARE( uint8_t tx_pwr_table_NA[TX_PWR_TABLE_SIZE])
	= { TX_PWR_TABLE_NA };
static FLASH_DECLARE( uint8_t tx_pwr_table_EU[TX_PWR_TABLE_SIZE])
	= { TX_PWR_TABLE_EU };
static FLASH_DECLARE( uint8_t tx_pwr_table_China[TX_PWR_TABLE_SIZE])
	= { TX_PWR_TABLE_CHINA };

/* === PROTOTYPES ========================================================== */

static void limit_tx_pwr(void);
static void set_default_tx_pwr(void);
static uint8_t convert_phyTransmitPower_to_reg_value(
		uint8_t phyTransmitPower_value);
static bool apply_channel_page_configuration(uint8_t ch_page);

/* ! @} */
/* === IMPLEMENTATION ====================================================== */

/*
 * \brief Initialize the TAL PIB
 *
 * This function initializes the TAL information base attributes
 * to their default values.
 */
void init_tal_pib(void)
{
	tal_pib.MaxCSMABackoffs = TAL_MAX_CSMA_BACKOFFS_DEFAULT;
	tal_pib.MinBE = TAL_MINBE_DEFAULT;
	tal_pib.PANId = TAL_PANID_BC_DEFAULT;
	tal_pib.ShortAddress = TAL_SHORT_ADDRESS_DEFAULT;
	tal_pib.CurrentChannel = TAL_CURRENT_CHANNEL_DEFAULT;
	tal_pib.CurrentPage = TAL_CURRENT_PAGE_DEFAULT;
	tal_pib.MaxFrameDuration = TAL_MAX_FRAME_DURATION_DEFAULT;
	tal_pib.SHRDuration = TAL_SHR_DURATION_DEFAULT;
	tal_pib.SymbolsPerOctet = TAL_SYMBOLS_PER_OCTET_DEFAULT;
	tal_pib.MaxBE = TAL_MAXBE_DEFAULT;
	tal_pib.MaxFrameRetries = TAL_MAXFRAMERETRIES_DEFAULT;
	tal_pib.TransmitPower = TAL_TRANSMIT_POWER_DEFAULT;
	limit_tx_pwr();
	tal_pib.CCAMode = TAL_CCA_MODE_DEFAULT;
	tal_pib.PrivatePanCoordinator = TAL_PAN_COORDINATOR_DEFAULT;
#ifdef BEACON_SUPPORT
	tal_pib.BattLifeExt = TAL_BATTERY_LIFE_EXTENSION_DEFAULT;
	tal_pib.BeaconOrder = TAL_BEACON_ORDER_DEFAULT;
	tal_pib.SuperFrameOrder = TAL_SUPERFRAME_ORDER_DEFAULT;
#endif  /* BEACON_SUPPORT */

#ifdef PROMISCUOUS_MODE
	tal_pib.PromiscuousMode = TAL_PIB_PROMISCUOUS_MODE_DEFAULT;
#endif

#if (defined SW_CONTROLLED_CSMA) && (defined TX_OCTET_COUNTER)
	tal_tx_octet_cnt = 0;
#endif
}

/*
 * \brief Write all shadow PIB variables to the transceiver
 *
 * This function writes all shadow PIB variables to the transceiver.
 * It is assumed that the radio does not sleep.
 */
void write_all_tal_pib_to_trx(void)
{
	uint8_t *ptr_to_reg;

	ptr_to_reg = (uint8_t *)&tal_pib.PANId;
	for (uint8_t i = 0; i < 2; i++) {
		trx_reg_write((RG_PAN_ID_0 + i), *ptr_to_reg);
		ptr_to_reg++;
	}

	ptr_to_reg = (uint8_t *)&tal_pib.IeeeAddress;
	for (uint8_t i = 0; i < 8; i++) {
		trx_reg_write((RG_IEEE_ADDR_0 + i), *ptr_to_reg);
		ptr_to_reg++;
	}

	ptr_to_reg = (uint8_t *)&tal_pib.ShortAddress;
	for (uint8_t i = 0; i < 2; i++) {
		trx_reg_write((RG_SHORT_ADDR_0 + i), *ptr_to_reg);
		ptr_to_reg++;
	}

	/* Configure TX_ARET; CSMA and CCA */
	trx_bit_write(SR_CCA_MODE, tal_pib.CCAMode);

#ifdef SW_CONTROLLED_CSMA

	/*
	 * If receiver is enabled during backoff periods,
	 * CSMA and frame re-transmissions are handled by software.
	 * Setup trx for immediate transmission.
	 */
	trx_bit_write(SR_MAX_FRAME_RETRIES, 0);
	trx_bit_write(SR_MAX_CSMA_RETRIES, 7);
#else
	trx_bit_write(SR_MIN_BE, tal_pib.MinBE);
	trx_bit_write(SR_MAX_BE, tal_pib.MaxBE);
#endif

	trx_bit_write(SR_AACK_I_AM_COORD, tal_pib.PrivatePanCoordinator);

	/* set phy parameter */

	apply_channel_page_configuration(tal_pib.CurrentPage);

	{
		uint8_t reg_value;

		reg_value = convert_phyTransmitPower_to_reg_value(
				tal_pib.TransmitPower);
		trx_reg_write(RG_PHY_TX_PWR, reg_value);
	}

#ifdef PROMISCUOUS_MODE
	if (tal_pib.PromiscuousMode) {
		set_trx_state(CMD_RX_ON);
	}

#endif
}

/*
 * \brief Gets a TAL PIB attribute
 *
 * This function is called to retrieve the transceiver information base
 * attributes.
 *
 * \param[in] attribute TAL infobase attribute ID
 * \param[out] value TAL infobase attribute value
 *
 * \return MAC_UNSUPPORTED_ATTRIBUTE if the TAL infobase attribute is not found
 *         MAC_SUCCESS otherwise
 */
#if (HIGHEST_STACK_LAYER == TAL)
retval_t tal_pib_get(uint8_t attribute, uint8_t *value)
{
	switch (attribute) {
	case macMaxCSMABackoffs:
		*value = tal_pib.MaxCSMABackoffs;
		break;

	case macMinBE:
		*value = tal_pib.MinBE;
		break;

	case macPANId:
		*(uint16_t *)value = tal_pib.PANId;
		break;

#ifdef PROMISCUOUS_MODE
	case macPromiscuousMode:
		*(uint16_t *)value = tal_pib.PromiscuousMode;
		break;

#endif
	case macShortAddress:
		*(uint16_t *)value = tal_pib.ShortAddress;
		break;

	case phyCurrentChannel:
		*value = tal_pib.CurrentChannel;
		break;

	case phyChannelsSupported:
		*(uint32_t *)value = tal_pib.SupportedChannels;
		break;

	case phyTransmitPower:
		*value = tal_pib.TransmitPower;
		break;

	case phyCCAMode:
		*value = tal_pib.CCAMode;
		break;

	case phyCurrentPage:
		*value = tal_pib.CurrentPage;
		break;

	case phyMaxFrameDuration:
		*(uint16_t *)value = tal_pib.MaxFrameDuration;
		break;

	case phySymbolsPerOctet:
		*value = tal_pib.SymbolsPerOctet;
		break;

	case phySHRDuration:
		*value = tal_pib.SHRDuration;
		break;

	case macMaxBE:
		*value = tal_pib.MaxBE;
		break;

	case macMaxFrameRetries:
		*value = tal_pib.MaxFrameRetries;
		break;

	case macIeeeAddress:
		*(uint64_t *)value = tal_pib.IeeeAddress;
		break;

#ifdef BEACON_SUPPORT
	case macBattLifeExt:
		*(bool *)value = tal_pib.BattLifeExt;
		break;

	case macBeaconOrder:
		*value = tal_pib.BeaconOrder;
		break;

	case macSuperframeOrder:
		*value = tal_pib.SuperFrameOrder;
		break;

	case macBeaconTxTime:
		*(uint32_t *)value = tal_pib.BeaconTxTime;
		break;

#endif  /* BEACON_SUPPORT */
	case mac_i_pan_coordinator:
		*(bool *)value = tal_pib.PrivatePanCoordinator;
		break;

	case macAckWaitDuration:

		/*
		 * AT86RF212B does not support changing this value w.r.t.
		 * compliance operation.
		 */
		return MAC_UNSUPPORTED_ATTRIBUTE;

	default:
		/* Invalid attribute id */
		return MAC_UNSUPPORTED_ATTRIBUTE;
	}

	return MAC_SUCCESS;
} /* tal_pib_get() */

#endif  /* (HIGHEST_STACK_LAYER != MAC) */

/*
 * \brief Sets a TAL PIB attribute
 *
 * This function is called to set the transceiver information base
 * attributes.
 *
 * \param attribute TAL infobase attribute ID
 * \param value TAL infobase attribute value to be set
 *
 * \return MAC_UNSUPPORTED_ATTRIBUTE if the TAL info base attribute is not found
 *         TAL_BUSY if the TAL is not in TAL_IDLE state. An exception is
 *         macBeaconTxTime which can be accepted by TAL even if TAL is not
 *         in TAL_IDLE state.
 *         MAC_SUCCESS if the attempt to set the PIB attribute was successful
 *         TAL_TRX_ASLEEP if trx is in SLEEP mode and access to trx is required
 */
retval_t tal_pib_set(uint8_t attribute, pib_value_t *value)
{
	/*
	 * Do not allow any changes while ED or TX is done.
	 * We allow changes during RX, but it's on the user's own risk.
	 */
#if (MAC_SCAN_ED_REQUEST_CONFIRM == 1)
	if (tal_state == TAL_ED_RUNNING) {
		Assert("TAL is busy" == 0);
		return TAL_BUSY;
	}

#endif /* (MAC_SCAN_ED_REQUEST_CONFIRM == 1) */

	/*
	 * Distinguish between PIBs that need to be changed in trx directly
	 * and those that are simple variable udpates.
	 * Ensure that the transceiver is not in SLEEP.
	 * If it is in SLEEP, change it to TRX_OFF.
	 */

	switch (attribute) {
	case macMaxFrameRetries:

		/*
		 * The new PIB value is not immediately written to the
		 * transceiver. This is done on a frame-by-frame base.
		 */
		tal_pib.MaxFrameRetries = value->pib_value_8bit;
		break;

	case macMaxCSMABackoffs:

		/*
		 * The new PIB value is not immediately written to the
		 * transceiver. This is done on a frame-by-frame base.
		 */
		tal_pib.MaxCSMABackoffs = value->pib_value_8bit;
		break;

#ifdef BEACON_SUPPORT
	case macBattLifeExt:
		tal_pib.BattLifeExt = value->pib_value_bool;
		break;

	case macBeaconOrder:
		tal_pib.BeaconOrder = value->pib_value_8bit;
		break;

	case macSuperframeOrder:
		tal_pib.SuperFrameOrder = value->pib_value_8bit;
		break;

	case macBeaconTxTime:
		tal_pib.BeaconTxTime = value->pib_value_32bit;
		break;

#endif  /* BEACON_SUPPORT */
#ifdef PROMISCUOUS_MODE
	case macPromiscuousMode:
		tal_pib.PromiscuousMode = value->pib_value_8bit;
		if (tal_pib.PromiscuousMode) {
			tal_trx_wakeup();

			/* Check if receive buffer is available or queue is not
			 * full. */
			if (NULL == tal_rx_buffer) {
				set_trx_state(CMD_PLL_ON);
				tal_rx_on_required = true;
			} else {
				set_trx_state(CMD_RX_ON);
			}
		} else {
			set_trx_state(CMD_TRX_OFF);
			tal_rx_on_required = false;
		}
		break;
#endif

	default:

		/*
		 * Following PIBs require access to trx.
		 * Therefore trx must be at least in TRX_OFF.
		 */

		if (tal_trx_status == TRX_SLEEP) {
			/* While trx is in SLEEP, register cannot be accessed.
			**/
			return TAL_TRX_ASLEEP;
		}

		switch (attribute) {
		case macMinBE:
			tal_pib.MinBE = value->pib_value_8bit;
#ifndef REDUCED_PARAM_CHECK

			/*
			 * macMinBE must not be larger than macMaxBE or
			 * calculation
			 * of macMaxFrameWaitTotalTime will fail.
			 */
			if (tal_pib.MinBE > tal_pib.MaxBE) {
				tal_pib.MinBE = tal_pib.MaxBE;
			}

#endif  /* REDUCED_PARAM_CHECK */
#ifndef SW_CONTROLLED_CSMA
			trx_bit_write(SR_MIN_BE, tal_pib.MinBE);
#endif
			break;

		case macPANId:
			tal_pib.PANId = value->pib_value_16bit;
			uint8_t *ptr_pan;
			ptr_pan = (uint8_t *)&tal_pib.PANId;
			for (uint8_t i = 0; i < 2; i++) {
				trx_reg_write((RG_PAN_ID_0 + i), *ptr_pan);
				ptr_pan++;
			}
			break;

		case macShortAddress:
			tal_pib.ShortAddress = value->pib_value_16bit;
			uint8_t *ptr_shrt;
			ptr_shrt = (uint8_t *)&tal_pib.ShortAddress;
			for (uint8_t i = 0; i < 2; i++) {
				trx_reg_write((RG_SHORT_ADDR_0 + i),
						*ptr_shrt);
				ptr_shrt++;
			}
			break;

		case phyCurrentChannel:
			if (tal_state != TAL_IDLE) {
				return TAL_BUSY;
			}

#ifdef HIGH_DATA_RATE_SUPPORT
			if ((tal_pib.CurrentPage == 5) ||
					(tal_pib.CurrentPage == 18) ||
					(tal_pib.CurrentPage == 19))
#else
			if (tal_pib.CurrentPage == 5)
#endif
			{
				if (((uint32_t)TRX_SUPPORTED_CHANNELS_CHINA &
						((uint32_t)0x01 <<
						value->pib_value_8bit)) ==
						false) {
					return MAC_INVALID_PARAMETER;
				}
			} else {
				if (((uint32_t)TRX_SUPPORTED_CHANNELS &
						((uint32_t)0x01 <<
						value->pib_value_8bit)) ==
						false) {
					return MAC_INVALID_PARAMETER;
				}
			}

			{
				uint8_t previous_channel;
				tal_trx_status_t previous_trx_status = TRX_OFF;

				previous_channel = tal_pib.CurrentChannel;
				tal_pib.CurrentChannel = value->pib_value_8bit;

				/*Set the default Power Values For the Changed
				 * Channel according to values in datasheet*/
				set_default_tx_pwr();

				/*
				 * Set trx to "soft" off avoiding that ongoing
				 * transaction (e.g. ACK) are interrupted.
				 */
				if (tal_trx_status != TRX_OFF) {
					previous_trx_status = RX_AACK_ON; /* any
					                                   *
					                                   *
					                                   *
					                                   *other
					                                   *
					                                   *
					                                   *
					                                   *than
					                                   *
					                                   *
					                                   *
					                                   *TRX_OFF
					                                   *
					                                   *
					                                   *
					                                   *state
					                                   **/
					do {
						/* set TRX_OFF until it could be
						 * set;
						 * trx might be busy */
					} while (set_trx_state(CMD_TRX_OFF) !=
							TRX_OFF);
				}

				/* Check if frequency band/modulation is
				 * changed. */
#ifdef HIGH_DATA_RATE_SUPPORT
				if ((tal_pib.CurrentPage == 5) ||
						(tal_pib.CurrentPage == 18) ||
						(tal_pib.CurrentPage == 19))
#else
				if (tal_pib.CurrentPage == 5)
#endif
				{
					trx_bit_write(SR_CC_NUMBER, GET_CHINA_FREQ(
							tal_pib.CurrentChannel));
				} else if ((tal_pib.CurrentChannel > 0) &&
						(previous_channel > 0)) {
					trx_bit_write(SR_CHANNEL,
							tal_pib.CurrentChannel);
				} else {
					uint8_t reg_value;

					/* Set modulation and channel */
					apply_channel_page_configuration(
							tal_pib.CurrentPage);
					limit_tx_pwr();
					reg_value
						=
							convert_phyTransmitPower_to_reg_value(
							tal_pib.TransmitPower);
					trx_reg_write(RG_PHY_TX_PWR,
							reg_value);
				}

				/* Re-store previous trx state */
				if (previous_trx_status != TRX_OFF) {
					/* Set to default state */
					set_trx_state(CMD_RX_AACK_ON);
				}
			}
			break;

		case phyCurrentPage:
			if (tal_state != TAL_IDLE) {
				return TAL_BUSY;
			} else {
				uint8_t page;
				tal_trx_status_t previous_trx_status = TRX_OFF;
				bool ret_val;

				/*
				 * Changing the channel, channel page or
				 * modulation
				 * requires that TRX is in TRX_OFF.
				 * Store current trx state and return to default
				 * state
				 * after channel page has been set.
				 */
				if (tal_trx_status != TRX_OFF) {
					previous_trx_status = RX_AACK_ON; /* any
					                                   *
					                                   *
					                                   *
					                                   *other
					                                   *
					                                   *
					                                   *
					                                   *than
					                                   *
					                                   *
					                                   *
					                                   *TRX_OFF
					                                   *
					                                   *
					                                   *
					                                   *state
					                                   **/
					do {
						/* set TRX_OFF until it could be
						 * set;
						 * trx might be busy */
					} while (set_trx_state(CMD_TRX_OFF) !=
							TRX_OFF);
				}

				page = value->pib_value_8bit;

				ret_val
					= apply_channel_page_configuration(page);

				if (previous_trx_status != TRX_OFF) {
					/* Set to default state */
					set_trx_state(CMD_RX_AACK_ON);
				}

				if (ret_val) {
					tal_pib.CurrentPage = page;
				} else {
					return MAC_INVALID_PARAMETER;
				}

				/*Set the default Power Values For the Changed
				 * Channel Page according to values in
				 * datasheet*/
				set_default_tx_pwr();
			}

			break;

		case macMaxBE:
			tal_pib.MaxBE = value->pib_value_8bit;
#ifndef REDUCED_PARAM_CHECK

			/*
			 * macMinBE must not be larger than macMaxBE or
			 * calculation
			 * of macMaxFrameWaitTotalTime will fail.
			 */
			if (tal_pib.MaxBE < tal_pib.MinBE) {
				tal_pib.MinBE = tal_pib.MaxBE;
			}

#endif  /* REDUCED_PARAM_CHECK */
#ifndef SW_CONTROLLED_CSMA
			trx_bit_write(SR_MAX_BE, tal_pib.MaxBE);
#endif
			break;

		case phyTransmitPower:
		{
			uint8_t reg_value;

			tal_pib.TransmitPower = value->pib_value_8bit;

			/* Limit tal_pib.TransmitPower to max/min trx values */
			limit_tx_pwr();
			reg_value = convert_phyTransmitPower_to_reg_value(
					tal_pib.TransmitPower);
			trx_reg_write(RG_PHY_TX_PWR, reg_value);
		}
		break;

		case phyCCAMode:
			tal_pib.CCAMode = value->pib_value_8bit;
			trx_bit_write(SR_CCA_MODE, tal_pib.CCAMode);
			break;

		case macIeeeAddress:
		{
			uint8_t *ptr;
			tal_pib.IeeeAddress = value->pib_value_64bit;
			ptr = (uint8_t *)&tal_pib.IeeeAddress;
			for (uint8_t i = 0; i < 8; i++) {
				trx_reg_write((RG_IEEE_ADDR_0 + i), *ptr);
				ptr++;
			}
		}
		break;

		case mac_i_pan_coordinator:
			tal_pib.PrivatePanCoordinator = value->pib_value_bool;
			trx_bit_write(SR_AACK_I_AM_COORD,
					tal_pib.PrivatePanCoordinator);
			break;

		case macAckWaitDuration:

			/*
			 * AT86RF212B does not support changing this value
			 * w.r.t.
			 * compliance operation.
			 * The ACK timing can be reduced to 2 symbols using TFA
			 * function.
			 */
			return MAC_UNSUPPORTED_ATTRIBUTE;

		default:
			return MAC_UNSUPPORTED_ATTRIBUTE;
		}

		break; /* end of 'default' from 'switch (attribute)' */
	}
	return MAC_SUCCESS;
} /* tal_pib_set() */

/**
 * \brief Set the default Power Values For Respective Pages and Channels
 */
static void set_default_tx_pwr(void)
{
	int8_t dbm_value;

	dbm_value = CONV_phyTransmitPower_TO_DBM(tal_pib.TransmitPower);

	if ((tal_pib.CurrentPage == 5) ||
			(tal_pib.CurrentPage == 18) ||
			(tal_pib.CurrentPage == 19)) {
		if ((tal_pib.CurrentChannel == 0) ||
				(tal_pib.CurrentChannel == 3)) {
			if (dbm_value > DEFAULT_TX_PWR_CHINA_CH_0_3) {
				dbm_value = DEFAULT_TX_PWR_CHINA_CH_0_3;
			}
		} else {
			if ((tal_pib.CurrentPage == 5) ||
					(tal_pib.CurrentPage == 18)) {
				if (dbm_value >
						DEFAULT_TX_PWR_OQPSK_RC_250_500)
				{
					dbm_value
						=
							DEFAULT_TX_PWR_OQPSK_RC_250_500;
				}
			}
		}
	} else if (tal_pib.CurrentPage == 0) {
		if (tal_pib.CurrentChannel == 0) {
			if (dbm_value > DEFAULT_TX_PWR_BPSK_20) {
				dbm_value = DEFAULT_TX_PWR_BPSK_20;
			}
		}
	} else {
		if (tal_pib.CurrentChannel == 0) {
			if (dbm_value >
					DEFAULT_TX_PWR_OQPSK_SIN_RC_100_200_400)
			{
				dbm_value
					=
						DEFAULT_TX_PWR_OQPSK_SIN_RC_100_200_400;
			}
		}
	}

	tal_pib.TransmitPower = TX_PWR_TOLERANCE | CONV_DBM_TO_phyTransmitPower(
			dbm_value);

	/*Handle other Channel and page combinations here*/
	limit_tx_pwr();

	trx_reg_write(RG_PHY_TX_PWR,
			convert_phyTransmitPower_to_reg_value(tal_pib.
			TransmitPower));
}

/**
 * \brief Limit the phyTransmitPower to the trx limits
 *
 * \param phyTransmitPower phyTransmitPower value
 *
 */
static void limit_tx_pwr(void)
{
	int8_t dbm_value;

	dbm_value = CONV_phyTransmitPower_TO_DBM(tal_pib.TransmitPower);

	/* Limit to the transceiver's absolute maximum/minimum. */
	if (dbm_value <= MIN_TX_PWR) {
		dbm_value = MIN_TX_PWR;
	} else if (dbm_value > MAX_TX_PWR) {
		dbm_value = MAX_TX_PWR;
	}

	tal_pib.TransmitPower = TX_PWR_TOLERANCE | CONV_DBM_TO_phyTransmitPower(
			dbm_value);
}

/**
 * \brief Converts a phyTransmitPower value to a register value
 *
 * \param phyTransmitPower_value phyTransmitPower value
 *
 * \return register value
 */
static uint8_t convert_phyTransmitPower_to_reg_value(
		uint8_t phyTransmitPower_value)
{
	int8_t dbm_value;
	uint8_t reg_value = 0x00;

	dbm_value = CONV_phyTransmitPower_TO_DBM(phyTransmitPower_value);

	/* Select the corresponding tx_pwr_table, also valid for high data rates
	**/
#ifdef HIGH_DATA_RATE_SUPPORT
	if ((tal_pib.CurrentPage == 5) || (tal_pib.CurrentPage == 18) ||
			(tal_pib.CurrentPage == 19))
#else
	if (tal_pib.CurrentPage == 5)
#endif
	{
		reg_value
			= PGM_READ_BYTE(
				&tx_pwr_table_China[MAX_TX_PWR - dbm_value]);
	} else { /* concerns channel pages={0, 2, 16, 17}*/
		if (tal_pib.CurrentChannel == 0) {
			reg_value
				= PGM_READ_BYTE(
					&tx_pwr_table_EU[MAX_TX_PWR -
					dbm_value]);
		} else { /* channels 1-10 */
			reg_value
				= PGM_READ_BYTE(
					&tx_pwr_table_NA[MAX_TX_PWR -
					dbm_value]);
		}
	}

	return reg_value;
}

/**
 * \brief Apply channel page configuartion to transceiver
 *
 * \param ch_page Channel page
 *
 * \return true if changes could be applied else false
 */
static bool apply_channel_page_configuration(uint8_t ch_page)
{
	/*
	 * Before updating the transceiver a number of TAL PIB attributes need
	 * to be updated depending on the channel page.
	 */

	/* \todo these configurations are not rate dependent here! But shall be!
	**/
	tal_pib.MaxFrameDuration = MAX_FRAME_DURATION;
	tal_pib.SHRDuration = NO_OF_SYMBOLS_PREAMBLE_SFD;
	tal_pib.SymbolsPerOctet = SYMBOLS_PER_OCTET;

	switch (ch_page) {
	case 0: /* BPSK */
		trx_bit_write(SR_BPSK_OQPSK, BPSK_MODE);
		trx_bit_write(SR_GC_TX_OFFS, BPSK_TX_OFFSET);
		trx_bit_write(SR_ALT_SPECTRUM, ALT_SPECTRUM_DISABLE);
		if (tal_pib.CurrentChannel == 0) { /* BPSK20, EU */
			trx_bit_write(SR_SUB_MODE, LOW_DATA_RATE);
		} else { /* BPSK40, NA */
			trx_bit_write(SR_SUB_MODE, HIGH_DATA_RATE);
		}

		/* Compliant ACK timing */
		trx_bit_write(SR_AACK_ACK_TIME, ACK_TIME_12_SYMBOLS);
		tal_pib.SupportedChannels = TRX_SUPPORTED_CHANNELS;
		trx_bit_write(SR_CC_BAND, 0);
		trx_bit_write(SR_CHANNEL, tal_pib.CurrentChannel);
		break;

	case 2: /* O-QPSK */
		trx_bit_write(SR_BPSK_OQPSK, OQPSK_MODE);
		trx_bit_write(SR_GC_TX_OFFS, OQPSK_TX_OFFSET);
		trx_bit_write(SR_OQPSK_DATA_RATE,
				ALTRATE_100_KBPS_OR_250_KBPS);
		trx_bit_write(SR_ALT_SPECTRUM, ALT_SPECTRUM_DISABLE);
		if (tal_pib.CurrentChannel == 0) { /* OQPSK100, EU */
			trx_bit_write(SR_SUB_MODE, LOW_DATA_RATE);
		} else { /* OQPSK250, NA */
			trx_bit_write(SR_SUB_MODE, HIGH_DATA_RATE);
		}

		/* Compliant ACK timing */
		trx_bit_write(SR_AACK_ACK_TIME, ACK_TIME_12_SYMBOLS);
		tal_pib.SupportedChannels = TRX_SUPPORTED_CHANNELS;
		trx_bit_write(SR_CC_BAND, 0);
		trx_bit_write(SR_CHANNEL, tal_pib.CurrentChannel);
		break;

	case 5: /* CHINESE_BAND, O-QPSK */
		trx_bit_write(SR_BPSK_OQPSK, OQPSK_MODE);
		trx_bit_write(SR_GC_TX_OFFS, OQPSK_TX_OFFSET);
		trx_bit_write(SR_OQPSK_DATA_RATE,
				ALTRATE_100_KBPS_OR_250_KBPS);
		trx_bit_write(SR_ALT_SPECTRUM, ALT_SPECTRUM_ENABLE);
		trx_bit_write(SR_SUB_MODE, HIGH_DATA_RATE);
		/* Compliant ACK timing */
		trx_bit_write(SR_AACK_ACK_TIME, ACK_TIME_12_SYMBOLS);
		tal_pib.SupportedChannels = TRX_SUPPORTED_CHANNELS_CHINA;
		/* Channel Page 5 supports channels 0-3. */
		if (tal_pib.CurrentChannel > 3) {
			tal_pib.CurrentChannel = 0;
		}

		trx_bit_write(SR_CC_BAND, 4);
		trx_bit_write(SR_CC_NUMBER,
				GET_CHINA_FREQ(tal_pib.CurrentChannel));

		break;

#ifdef HIGH_DATA_RATE_SUPPORT
	case 16:    /* non-compliant OQPSK mode 1 */
		trx_bit_write(SR_BPSK_OQPSK, OQPSK_MODE);
		trx_bit_write(SR_GC_TX_OFFS, OQPSK_TX_OFFSET);
		trx_bit_write(SR_ALT_SPECTRUM, ALT_SPECTRUM_DISABLE);
		trx_bit_write(SR_OQPSK_DATA_RATE,
				ALTRATE_200_KBPS_OR_500_KBPS);
		if (tal_pib.CurrentChannel == 0) { /* 200kbps, EU */
			trx_bit_write(SR_SUB_MODE, LOW_DATA_RATE);
		} else { /* 500kbps, NA */
			trx_bit_write(SR_SUB_MODE, HIGH_DATA_RATE);
		}

		/* Reduced ACK timing */
		trx_bit_write(SR_AACK_ACK_TIME, ACK_TIME_2_SYMBOLS);
		tal_pib.SupportedChannels = TRX_SUPPORTED_CHANNELS;
		trx_bit_write(SR_CC_BAND, 0);
		trx_bit_write(SR_CHANNEL, tal_pib.CurrentChannel);
		break;

	case 17:    /* non-compliant OQPSK mode 2 */
		trx_bit_write(SR_BPSK_OQPSK, OQPSK_MODE);
		trx_bit_write(SR_GC_TX_OFFS, OQPSK_TX_OFFSET);
		trx_bit_write(SR_ALT_SPECTRUM, ALT_SPECTRUM_DISABLE);
		trx_bit_write(SR_OQPSK_DATA_RATE,
				ALTRATE_400_KBPS_OR_1_MBPS);
		if (tal_pib.CurrentChannel == 0) { /* 400kbps, EU */
			trx_bit_write(SR_SUB_MODE, LOW_DATA_RATE);
		} else { /* 1000kbps, NA */
			trx_bit_write(SR_SUB_MODE, HIGH_DATA_RATE);
		}

		/* Reduced ACK timing */
		trx_bit_write(SR_AACK_ACK_TIME, ACK_TIME_2_SYMBOLS);
		tal_pib.SupportedChannels = TRX_SUPPORTED_CHANNELS;
		trx_bit_write(SR_CC_BAND, 0);
		trx_bit_write(SR_CHANNEL, tal_pib.CurrentChannel);
		break;

	case 18:    /* Chinese band, non-compliant mode 1 using O-QPSK 500 */
		trx_bit_write(SR_BPSK_OQPSK, OQPSK_MODE);
		trx_bit_write(SR_GC_TX_OFFS, OQPSK_TX_OFFSET);
		trx_bit_write(SR_OQPSK_DATA_RATE,
				ALTRATE_200_KBPS_OR_500_KBPS);
		trx_bit_write(SR_ALT_SPECTRUM, ALT_SPECTRUM_ENABLE);
		trx_bit_write(SR_SUB_MODE, HIGH_DATA_RATE);
		/* Reduced ACK timing */
		trx_bit_write(SR_AACK_ACK_TIME, ACK_TIME_2_SYMBOLS);
		tal_pib.SupportedChannels = TRX_SUPPORTED_CHANNELS_CHINA;
		/* Channel Page 18 supports channels 0-3. */
		if (tal_pib.CurrentChannel > 3) {
			tal_pib.CurrentChannel = 0;
		}

		trx_bit_write(SR_CC_BAND, 4);
		trx_bit_write(SR_CC_NUMBER,
				GET_CHINA_FREQ(tal_pib.CurrentChannel));
		break;

	case 19:    /* Chinese band, non-compliant mode 2 using O-QPSK 1000 */
		trx_bit_write(SR_BPSK_OQPSK, OQPSK_MODE);
		trx_bit_write(SR_GC_TX_OFFS, OQPSK_TX_OFFSET);
		trx_bit_write(SR_OQPSK_DATA_RATE,
				ALTRATE_400_KBPS_OR_1_MBPS);
		trx_bit_write(SR_ALT_SPECTRUM, ALT_SPECTRUM_ENABLE);
		trx_bit_write(SR_SUB_MODE, HIGH_DATA_RATE);
		/* Reduced ACK timing */
		trx_bit_write(SR_AACK_ACK_TIME, ACK_TIME_2_SYMBOLS);
		tal_pib.SupportedChannels = TRX_SUPPORTED_CHANNELS_CHINA;
		/* Channel Page 18 supports channels 0-3. */
		if (tal_pib.CurrentChannel > 3) {
			tal_pib.CurrentChannel = 0;
		}

		trx_bit_write(SR_CC_BAND, 4);
		trx_bit_write(SR_CC_NUMBER,
				GET_CHINA_FREQ(tal_pib.CurrentChannel));
		break;

#endif  /* #ifdef HIGH_DATA_RATE_SUPPORT */
	default:
		return false;
	}

	return true;
}

/* EOF */
