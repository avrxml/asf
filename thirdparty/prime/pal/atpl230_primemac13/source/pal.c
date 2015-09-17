/**
 * \file
 *
 * \brief Physical Abstraction Layer
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
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

#include <string.h>

#include "asf.h"

#include "conf_pal.h"

/* @cond 0 */
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/* @endcond */

#ifdef PAL_DBG_PRIME_MESSAGES
static uint32_t ul_last_rx_bcn_tics = 0; /* only for debug */
static uint32_t ul_last_tx_bcn_tics = 0; /* only for debug */
#endif

/**
 * \weakgroup pal_prime_group
 * @{
 */

/* \name Internal PHY event indicators */
/* @{ */
static uint8_t uc_rx_msg_flag;
static uint8_t uc_tx_res_flag;
static uint8_t uc_tx_bcn_res_flag;
/* @} */

/* \name Transmission parameters */
/* @{ */
static xPhyMsgTxResult_t x_pal_tx_res;
static xPhyMsgTxResult_t x_pal_bcn_tx_res;
uint8_t uc_tx_head_type;
uint8_t uc_tx_level;
uint8_t uc_tx_scheme;
uint32_t l_tx_time;
uint8_t uc_tx_delay;
/* @} */

/* \name Reception paramters */
/* @{ */
static xPhyMsgRx_t x_pal_rx_msg;
static uint8_t uc_pal_data_buf_rx[PHY_MAX_PPDU_SIZE];
/* @} */

/* \name CRC capablity */
/* @{ */
static uint8_t uc_pal_crc_enable;
/* @} */

/* \name Error counters */
/* @{ */
static uint16_t ul_rx_false_positive;
static uint16_t ul_rx_bad_crc;
/* @} */

/* \name Quality constants */
/* @{ */
const uint32_t phyEvmThresholdEx[] = {
	PHY_EVM_THRESHOLD_EX_0,
	PHY_EVM_THRESHOLD_EX_1,
	PHY_EVM_THRESHOLD_EX_2,
	PHY_EVM_THRESHOLD_EX_3,
	PHY_EVM_THRESHOLD_EX_4,
	PHY_EVM_THRESHOLD_EX_5,
	PHY_EVM_THRESHOLD_EX_6,
	PHY_EVM_THRESHOLD_EX_7,
	PHY_EVM_THRESHOLD_EX_8,
	PHY_EVM_THRESHOLD_EX_9,
	PHY_EVM_THRESHOLD_EX_10,
	PHY_EVM_THRESHOLD_EX_11,
	PHY_EVM_THRESHOLD_EX_12,
	PHY_EVM_THRESHOLD_EX_13,
	PHY_EVM_THRESHOLD_EX_14,
	PHY_EVM_THRESHOLD_EX_15,
	PHY_EVM_THRESHOLD_EX_16,
	PHY_EVM_THRESHOLD_EX_17,
	PHY_EVM_THRESHOLD_EX_18,
	PHY_EVM_THRESHOLD_EX_19,
	PHY_EVM_THRESHOLD_EX_20,
	PHY_EVM_THRESHOLD_EX_21,
	PHY_EVM_THRESHOLD_EX_22,
	PHY_EVM_THRESHOLD_EX_23,
	PHY_EVM_THRESHOLD_EX_24,
	PHY_EVM_THRESHOLD_EX_25,
	PHY_EVM_THRESHOLD_EX_26,
	PHY_EVM_THRESHOLD_EX_27,
	PHY_EVM_THRESHOLD_EX_28,
	PHY_EVM_THRESHOLD_EX_29,
	PHY_EVM_THRESHOLD_EX_30,
	PHY_EVM_THRESHOLD_EX_31,
	PHY_EVM_THRESHOLD_EX_32,
	PHY_EVM_THRESHOLD_EX_33,
	PHY_EVM_THRESHOLD_EX_34,
	PHY_EVM_THRESHOLD_EX_35,
	PHY_EVM_THRESHOLD_EX_36,
	PHY_EVM_THRESHOLD_EX_37,
	PHY_EVM_THRESHOLD_EX_38,
	PHY_EVM_THRESHOLD_EX_39,
	PHY_EVM_THRESHOLD_EX_40,
	PHY_EVM_THRESHOLD_EX_41,
	PHY_EVM_THRESHOLD_EX_42,
	PHY_EVM_THRESHOLD_EX_43,
	PHY_EVM_THRESHOLD_EX_44,
	PHY_EVM_THRESHOLD_EX_45,
	PHY_EVM_THRESHOLD_EX_46,
	PHY_EVM_THRESHOLD_EX_47,
	PHY_EVM_THRESHOLD_EX_48,
	PHY_EVM_THRESHOLD_EX_49,
	PHY_EVM_THRESHOLD_EX_50,
	PHY_EVM_THRESHOLD_EX_51,
	PHY_EVM_THRESHOLD_EX_52,
	PHY_EVM_THRESHOLD_EX_53,
	PHY_EVM_THRESHOLD_EX_54,
	PHY_EVM_THRESHOLD_EX_55,
	PHY_EVM_THRESHOLD_EX_56,
	PHY_EVM_THRESHOLD_EX_57,
	PHY_EVM_THRESHOLD_EX_58,
	PHY_EVM_THRESHOLD_EX_59
};
/* @} */

/**
 * \brief Calculates the QT value with the last message rx parameters
 *
 * \retval QT value
 */
static uint8_t _pal_get_qt(void)
{
	uint8_t uc_cinr_min;
	uint8_t uc_qt;

	/* Get the proper values from the latest incomming message received to
	 * avoid noise that update registers */
	phy_get_cfg_param(REG_ATPL230_TXRXBUF_CINRMIN_RX0 + x_pal_rx_msg.uc_buff_id,
			&uc_cinr_min, sizeof(uc_cinr_min));

	/* convert CINR into Qt (Qt[dB] = CINR[dB] + 3, and the measurements */
	/* from the chip are in 1/4dB) */
	uc_qt = (uc_cinr_min + 12) / 4;

	/* Truncate Qt to 21 dB (corresponds to an SNR = 7) */
	if (uc_qt > 21) {
		uc_qt = 21;
	}

	return(uc_qt);
}

/**
 * \brief Get the extended signal noise ratio value
 *
 * \retval SNR in a 60-value table
 */

static uint8_t _pal_get_ex_snr(void)
{
	uint32_t ul_evm_payload;
	uint8_t uc_snr;

	ul_evm_payload = x_pal_rx_msg.evm_payload_acum >> 8;

	for (uc_snr = 0; uc_snr < PHY_EVM_TRESHOLD_EX_LEVELS; uc_snr++) {
		if (ul_evm_payload > phyEvmThresholdEx[uc_snr]) {
			return uc_snr;
		}
	}
	return 0;
}

/**
 * \brief Get result from transmission
 *
 * \param pvParameters    Parameters
 */
static void pal_get_tx_result(void *pvParameters)
{
	xPhyMsgTxResult_t x_write_result;
#ifdef PAL_DBG_PRIME_MESSAGES
	uint32_t ul_tx_time;
#endif

	UNUSED(pvParameters);

	/* initialize tx res structure */
	memset(&x_write_result, 0, sizeof(xPhyMsgTxResult_t));

	/* get phy queue uc_result */
	phy_tx_frame_result_cb(&x_write_result);

	if (x_write_result.uc_id_buffer != 0xFF) {
		if (x_write_result.uc_id_buffer == 0) {
			/* fill BCN info to pass to mac layer */
			memcpy(&x_pal_bcn_tx_res, &x_write_result, sizeof(xPhyMsgTxResult_t));
			/* enable BCN flag */
			uc_tx_bcn_res_flag = true;
		} else {
			/* fill info to pass to mac layer */
			memcpy(&x_pal_tx_res, &x_write_result, sizeof(xPhyMsgTxResult_t));
			/* enable flag */
			uc_tx_res_flag = true;
		}

#ifdef PAL_DBG_PRIME_MESSAGES
		/* get tx time */
		phy_get_cfg_param(REG_ATPL230_TXRXBUF_EMITIME1_TX0 + ((x_write_result.uc_id_buffer) << 2),
				&ul_tx_time, 4);
		printf("\ttxRes GEN %u,%u [%u]\r\n", (unsigned int)x_write_result.uc_id_buffer,
				(unsigned int)x_write_result.uc_result, ul_tx_time);
#endif

#ifdef PAL_TX_LED0_SWITCH_OFF
		LED_Off(LED0);
#endif

#ifdef PAL_TX_LED1_SWITCH_OFF
		LED_Off(LED1);
#endif
	}
}

/**
 * \brief Get result from reception
 *
 * \param pvParameters    Parameters
 */
static void pal_get_rx(void *pvParameters)
{
	xPhyMsgRx_t x_read_msg;
	uint32_t ul_crc_calc;
	uint8_t uc_crc_type;
	uint8_t uc_crc_valid;
	uint8_t *puc_crc_buf;
#ifdef PAL_DBG_PRIME_MESSAGES
	uint16_t idx;
#endif

	UNUSED(pvParameters);

	/* initialize read msg structure */
	memset(&x_read_msg, 0, sizeof(xPhyMsgRx_t));

	/* set pointer to reception data buffer */
	x_read_msg.data_buf = uc_pal_data_buf_rx;

	/* block until get phy queue uc_result */
	phy_rx_frame_cb(&x_read_msg);

	/* build response */
	if (x_read_msg.data_len) {
		if (uc_pal_crc_enable) {
			/* crc invalid by default */
			uc_crc_valid = 0;
			/* check CRC */
			if (x_read_msg.header_type == PCRC_HT_PROMOTION) {
				uc_crc_type = CRC_TYPE_8;
			} else {
				uc_crc_type = CRC_TYPE_32;
			}

			ul_crc_calc = pcrc_calculate_prime_crc(x_read_msg.data_buf, x_read_msg.data_len - (uc_crc_type + 1), x_read_msg.header_type, uc_crc_type);
			puc_crc_buf = &x_read_msg.data_buf[x_read_msg.data_len - (uc_crc_type + 1)];
			if (uc_crc_type == CRC_TYPE_8) {
				if (*puc_crc_buf == (uint8_t)ul_crc_calc) {
					uc_crc_valid = 1;
				}
			} else if (uc_crc_type == CRC_TYPE_32) {
				if ((*puc_crc_buf++ == (uint8_t)(ul_crc_calc >> 24)) &&
						(*puc_crc_buf++ == (uint8_t)(ul_crc_calc >> 16)) &&
						(*puc_crc_buf++ == (uint8_t)(ul_crc_calc >> 8)) &&
						(*puc_crc_buf++ == (uint8_t)ul_crc_calc)) {
					uc_crc_valid = 1;
				}
			}

			if (uc_crc_valid) {
				x_read_msg.data_len -= (uc_crc_type + 1);
				/* fill info to pass to mac layer */
				memcpy(&x_pal_rx_msg, &x_read_msg, sizeof(xPhyMsgRx_t));
				/* enable flag */
				uc_rx_msg_flag = true;
				/* reset x_read_msg */
				x_read_msg.data_len = 0;
			} else {
				ul_rx_bad_crc++;
#ifdef PAL_DBG_PRIME_MESSAGES
				printf(" PAL CRC error...\n");
#endif
				memset(&x_pal_rx_msg, 0, sizeof(x_pal_rx_msg));
			}
		} else {
			/* fill info to pass to mac layer */
			memcpy(&x_pal_rx_msg, &x_read_msg, sizeof(xPhyMsgRx_t));
			/* enable flag */
			uc_rx_msg_flag = true;
			/* reset x_read_msg */
			x_read_msg.data_len = 0;
		}

#ifdef PAL_RX_LED0_SWITCH_OFF
		LED_Off(LED0);
#endif

#ifdef PAL_RX_LED1_SWITCH_OFF
		LED_Off(LED1);
#endif
	}
}

/**
 * \brief This function initiates PRIME PHY layer configuration and
 * it must be implemented externally by the user.
 *
 * \note Init PHY layer configuration with the rest of PHY layer functions.
 * This function will be invoked by function phy_init() and it allows the user
 * to configure PHY layer initialization. Users can adapt the PHY layer to every
 * hardware platform.
 * This is an example of usage, configuring analog front end and different
 * thresholds for load detection.
 * ATMEL strongly recommends to consult with technical support to change any
 * parameter in this file.
 *
 */
void pal_init(void)
{
	uint8_t uc_cfg_value;

	/* Init flags of Tx/Rx messages */
	uc_rx_msg_flag = false;
	uc_tx_res_flag = false;
	uc_tx_bcn_res_flag = false;

	/* Init error counters */
	ul_rx_false_positive = 0;
	ul_rx_bad_crc = 0;

	/* Initialize rx buffer */
	memset(uc_pal_data_buf_rx, 0, sizeof(uc_pal_data_buf_rx));

	/* Enable QR modes in RX and TX */
	uc_cfg_value = true;
	phy_set_cfg_param(PHY_ID_RX_QR_MODE_ID, &uc_cfg_value, sizeof(uc_cfg_value));
	phy_set_cfg_param(PHY_ID_TX_QR_MODE_ID, &uc_cfg_value, sizeof(uc_cfg_value));

	/* disable MAC coproc by default */
	phy_mac_crc_disable();

	/* enable CRC in PAL */
	uc_pal_crc_enable = true;
}

/**
 * \brief This function runs the PAL layer process.
 * It must be called periodically.
 *
 */
void pal_process(void)
{
	/* Tx result */
	pal_get_tx_result(NULL);

	/* Rx */
	pal_get_rx(NULL);
}

/**
 * \brief Check if a message has been received from PHY layer
 *
 * \param puc_psdu      Pointer to data buffer with the received message
 * \param pus_len       Length of data buffer
 * \param puc_level     Level of received message
 * \param puc_scheme    Modulation scheme of received message
 * \param pul_time      Reception instant
 *
 */
void pal_data_indication(uint8_t *puc_psdu, uint16_t *pus_len,
		uint8_t *puc_level, uint8_t *puc_scheme, uint32_t *pul_time)
{
	*pus_len = 0;
	if (uc_rx_msg_flag) {
		uc_rx_msg_flag = false;
		if (x_pal_rx_msg.data_len) {
			/* Copy message and return parameters */
			memcpy(puc_psdu, x_pal_rx_msg.data_buf, x_pal_rx_msg.data_len);
			/* provide rx info */
			*pus_len = x_pal_rx_msg.data_len;
			*puc_level = x_pal_rx_msg.rssi_avg;
			*puc_scheme = x_pal_rx_msg.scheme;
			*pul_time = x_pal_rx_msg.rx_time;

			/* reset global data rx len */
			x_pal_rx_msg.data_len = 0;
		}
	}
}

/**
 * \brief Check if a message has been received from PHY layer
 *
 * \param px_msg      Pointer to reception data
 *
 */
void pal_data_indication_ex(xPalMsgRx_t *px_msg)
{
#ifdef PAL_DBG_PRIME_MESSAGES
	unsigned int us_lnid;
	unsigned int uc_level;
	unsigned int uc_down;
	unsigned int uc_rx_count;
	unsigned int uc_tx_count;
#endif

	if (uc_rx_msg_flag) {
		uc_rx_msg_flag = false;
		if (x_pal_rx_msg.data_len) {
			/* Copy message */
			memcpy(px_msg, &x_pal_rx_msg, sizeof(xPalMsgRx_t));
			/* reset global data rx len */
			x_pal_rx_msg.data_len = 0;
#ifdef PAL_DBG_PRIME_MESSAGES
			if (px_msg->header_type == PHY_HT_BEACON) {
				printf("\t\t<- BCN %u, [%u]\r\n", (unsigned int)((px_msg->data_buf[5]) >> 3),
						(px_msg->rx_time - ul_last_rx_bcn_tics));
				ul_last_rx_bcn_tics = px_msg->rx_time;
			} else if (px_msg->header_type == PHY_HT_GENERIC) {
				uc_level = (unsigned int)(px_msg->data_buf[1] & 0x3F);
				uc_down = (unsigned int)(px_msg->data_buf[1] & 0x40);
				us_lnid = ((unsigned int)(px_msg->data_buf[6]) << 6) + ((unsigned int)(px_msg->data_buf[7]) >> 2);
				if (px_msg->data_buf[3] & 0x02) {
					switch (px_msg->data_buf[4]) {
					case 1:
						us_lnid = ((unsigned int)(px_msg->data_buf[6]) << 6) + ((unsigned int)(px_msg->data_buf[7]) >> 2);
						printf("\t\t<- REG (%u) (%u)\r\n", us_lnid, uc_level);
						break;

					case 2:
						printf("\t\t<- CON (%u)\r\n", uc_level);
						break;

					case 3:
						printf("\t\t<- PRO (%u)\r\n", uc_level);
						break;

					case 4:
						printf("\t\t<- BSI (%u)\r\n", uc_level);
						break;

					case 5:
						printf("\t\t<- FRA (%u)\r\n", uc_level);
						break;

					case 6:
						printf("\t\t<- CFP (%u)\r\n", uc_level);
						break;

					case 7:
						uc_rx_count = (px_msg->data_buf[9]) >> 5;
						uc_tx_count = ((px_msg->data_buf[9]) >> 2) & 0x07;
						if (uc_down) {
							printf("\t\t<- ALV D (%u) (%u) [%u]\r\n", us_lnid, uc_level, px_msg->uc_buff_id);
						} else {
							printf("\t\t<- ALV U (%u) (%u) [%u]\r\n", us_lnid, uc_level, px_msg->uc_buff_id);
						}

						break;

					case 8:
						printf("\t\t<- MUL (%u)\r\n", uc_level);
						break;

					case 9:
						printf("\t\t<- PRM (%u)\r\n", uc_level);
						break;

					case 10:
						printf("\t\t<- SEC (%u)\r\n", uc_level);
						break;

					default:
						printf("\t\t<- ERROR (%u)\r\n", uc_level);
					}
				} else {
					if (uc_down) {
						printf("\t\t<- DATA D (%u) (%u) d[%02x][%u]\r\n", us_lnid, uc_level, px_msg->data_buf[9], px_msg->uc_buff_id);
					} else {
						printf("\t\t<- DATA U (%u) (%u) d[%02x][%u]\r\n", us_lnid, uc_level, px_msg->data_buf[9], px_msg->uc_buff_id);
					}
				}
			} else {
				printf("\t\t<- PNPDU \t\t buf(%u)\r\n", px_msg->uc_buff_id);
			}
#endif
		} else {
			px_msg->data_len = 0;
		}
	} else {
		px_msg->data_len = 0;
	}
}

/**
 * \brief Send message using standard PHY layer
 *
 * \param puc_mpdu      Pointer to transmission data
 * \param us_len        Length of transmission data
 * \param uc_level      Transmission level
 * \param uc_scheme     Modulation scheme
 * \param ul_time       Absolut Time to transmit in 10us.
 *
 * \retval 0 if ok
 * \retval otherwise transmission error
 */
uint8_t pal_data_request(uint8_t *puc_mpdu, uint16_t us_len, uint8_t uc_level,
		uint8_t uc_scheme, uint32_t ul_time)
{
	xPalMsgTx_t px_data_msg;

	px_data_msg.data_buf = puc_mpdu;
	px_data_msg.att_level = uc_level;
	px_data_msg.data_len = us_len;
	px_data_msg.disable_rx = 0;
	px_data_msg.mode = MODE_PRIME_V1_3;
	px_data_msg.scheme = uc_scheme;
	px_data_msg.tdelay = ul_time;
	px_data_msg.tmode = PHY_TX_SCHEDULING_MODE_ABSOLUTE;
	/* Buffer 1 by default */
	px_data_msg.uc_buff_id = 1;

	return (pal_data_request_ex(&px_data_msg));
}

/**
 * \brief Send message using PHY layer
 *
 * \param px_msg      Pointer to transmission data
 *
 * \retval 0 if ok
 * \retval otherwise transmission error
 */
uint8_t pal_data_request_ex(xPalMsgTx_t *px_msg)
{
	uint8_t uc_res;
	uint8_t uc_crc_type;
	uint32_t ul_crc_calc;
	uint8_t *puc_crc_buf;
	xPalMsgTx_t x_pal_tx_data;
#ifdef PAL_DBG_PRIME_MESSAGES
	unsigned int us_lnid;
	unsigned int uc_down;
	unsigned int uc_rx_count;
	unsigned int uc_tx_count;
#endif

	/* copy message to local tx var */
	memcpy(&x_pal_tx_data, px_msg, sizeof(xPalMsgTx_t));

	if (uc_pal_crc_enable) {
		/* Save parameters for later requests */
		uc_tx_head_type = ATPL230_GET_HEADER_TYPE(x_pal_tx_data.data_buf[0]);
		/* add HCS in case of generic packet */
		if (uc_tx_head_type == PCRC_HT_GENERIC) {
			ul_crc_calc = pcrc_calculate_prime_crc(x_pal_tx_data.data_buf, 2, uc_tx_head_type, CRC_TYPE_8);
			x_pal_tx_data.data_buf[2] = (uint8_t)ul_crc_calc;
		}

		/* add MAC crc */
		if (uc_tx_head_type == PCRC_HT_PROMOTION) {
			uc_crc_type = CRC_TYPE_8;
		} else {
			uc_crc_type = CRC_TYPE_32;
		}

		ul_crc_calc = pcrc_calculate_prime_crc(x_pal_tx_data.data_buf, x_pal_tx_data.data_len, uc_tx_head_type, uc_crc_type);
		puc_crc_buf = &x_pal_tx_data.data_buf[x_pal_tx_data.data_len];
		if (uc_crc_type == CRC_TYPE_8) {
			*puc_crc_buf = (uint8_t)ul_crc_calc;
			x_pal_tx_data.data_len++;
		} else if (uc_crc_type == CRC_TYPE_32) {
			*puc_crc_buf = (uint8_t)(ul_crc_calc >> 24);
			*(puc_crc_buf + 1) = (uint8_t)(ul_crc_calc >> 16);
			*(puc_crc_buf + 2) = (uint8_t)(ul_crc_calc >> 8);
			*(puc_crc_buf + 3) = (uint8_t)ul_crc_calc;
			x_pal_tx_data.data_len += 4;
		}
	}

	/* update tx result data to "in process" */
	if (uc_tx_head_type == PCRC_HT_BEACON) {
		x_pal_bcn_tx_res.uc_result = PHY_TX_RESULT_PROCESS;
		uc_tx_bcn_res_flag = false;
	} else {
		x_pal_tx_res.uc_result = PHY_TX_RESULT_PROCESS;
		uc_tx_res_flag = false;
	}

	uc_res = phy_tx_frame((xPhyMsgTx_t *)&x_pal_tx_data);

#ifdef PAL_DBG_PRIME_MESSAGES
	if (uc_tx_head_type == PHY_HT_BEACON) {
		printf("-> BCN %u, [%u]\r\n", (unsigned int)((x_pal_tx_data.data_buf[5]) >> 3), (x_pal_tx_data.tdelay - ul_last_tx_bcn_tics));
		ul_last_tx_bcn_tics = x_pal_tx_data.tdelay;
	} else if (uc_tx_head_type == PHY_HT_GENERIC) {
		uc_down = (unsigned int)(x_pal_tx_data.data_buf[1] & 0x40);
		us_lnid = ((unsigned int)(x_pal_tx_data.data_buf[6]) << 6) + ((unsigned int)(x_pal_tx_data.data_buf[7]) >> 2);
		if (x_pal_tx_data.data_buf[3] & 0x02) {
			switch (x_pal_tx_data.data_buf[4]) {
			case 1:
				us_lnid = ((unsigned int)(x_pal_tx_data.data_buf[6]) << 6) + ((unsigned int)(x_pal_tx_data.data_buf[7]) >> 2);
				printf("-> REG (%u)\r\n", us_lnid);
				break;

			case 2:
				printf("-> CON\r\n");
				break;

			case 3:
				printf("-> PRO\r\n");
				break;

			case 4:
				printf("-> BSI\r\n");
				break;

			case 5:
				printf("-> FRA\r\n");
				break;

			case 6:
				printf("-> CFP\r\n");
				break;

			case 7:
				uc_rx_count = (x_pal_tx_data.data_buf[9]) >> 5;
				uc_tx_count = ((x_pal_tx_data.data_buf[9]) >> 2) & 0x07;
				if (uc_down) {
					printf("-> ALV D (%u) [%u]\r\n", us_lnid, x_pal_tx_data.uc_buff_id);
				} else {
					printf("-> ALV U (%u) [%u]\r\n", us_lnid, x_pal_tx_data.uc_buff_id);
				}

				break;

			case 8:
				printf("-> MUL\r\n");
				break;

			case 9:
				printf("-> PRM\r\n");
				break;

			case 10:
				printf("-> SEC\r\n");
				break;

			default:
				printf("-> ERROR\r\n");
			}
		} else {
			if (uc_down) {
				printf("\t\t-> DATA D (%u) d[%02x] [%u]\r\n", us_lnid, x_pal_tx_data.data_buf[9], x_pal_tx_data.uc_buff_id);
			} else {
				printf("\t\t-> DATA U (%u) d[%02x] [%u]\r\n", us_lnid, x_pal_tx_data.data_buf[9], x_pal_tx_data.uc_buff_id);
			}
		}
	} else {
		printf("-> PNPDU\r\n");
	}
#endif

	return(uc_res);
}

/**
 * \brief Get timer
 *
 * \param pul_timer      Pointer to timer
 *
 * \return PAL_CFG_SUCCESS if there is no error, otherwise returns
 * PAL_CFG_INVALID_INPUT.
 */
uint8_t pal_timer_get(uint32_t *pul_timer)
{
	uint32_t ul_time;

	phy_get_cfg_param(REG_ATPL230_VHIGH_TIMER_BEACON_REF, &ul_time, sizeof(uint32_t));
	if (ul_time) {
		*pul_timer = ul_time;
	} else {
		return PAL_CFG_INVALID_INPUT;
	}

	return PAL_CFG_SUCCESS;
}

/**
 * \brief Get carrier detect
 *
 * \param puc_cd        Pointer to carrier detect
 * \param puc_rssi      Pointer to RSSI
 * \param pul_time      Pointer to time
 * \param puc_header    Pointer to header type
 *
 * \return PAL_CFG_SUCCESS if there is no error, otherwise returns
 * PAL_CFG_INVALID_INPUT.
 */
uint8_t pal_cd_get(uint8_t *puc_cd, uint8_t *puc_rssi, uint32_t *pul_time,
		uint8_t *puc_header)
{
	uint8_t uc_temp;
	uint8_t uc_result;

	uc_result = phy_get_cfg_param(REG_ATPL230_PHY_SFR, &uc_temp, sizeof(uint8_t));
	*puc_cd = (uc_temp & 0x40) >> 6;

	if (*puc_cd) {
		/* Don't apply */
		*puc_rssi = 0; 
		*pul_time = 0;
		*puc_header = 1;
	} else {
		*puc_rssi = 0;
		*pul_time = 0;
		*puc_header = 0;
	}

	return(uc_result);
}

/**
 * \brief Get zero-cross time
 *
 * \param pul_zct      Pointer to zero-cross time
 *
 * \return 1 if there is no error, otherwise returns PAL_CFG_INVALID_INPUT.
 */
uint8_t pal_zct_get(uint32_t *pul_zct)
{
	uint8_t uc_bufnum;

	phy_get_cfg_param(PHY_ID_RX_BUFFER_ID, &uc_bufnum, sizeof(uint8_t));

	return(phy_get_cfg_param(REG_ATPL230_TXRXBUF_ZCT1_RX0 + (uc_bufnum << 2),
	       pul_zct, sizeof(uint32_t)));
}

/**
 * \brief Get Automatic Gain Mode (AGC)
 *
 * \param puc_mode     Pointer to Auto/Manual mode
 * \param puc_gain     Pointer to initial receiving gain in auto mode
 *
 * \return PAL_CFG_SUCCESS if there is no error, otherwise returns
 * PAL_CFG_INVALID_INPUT.
 */
uint8_t pal_agc_get(uint8_t *puc_mode, uint8_t *puc_gain)
{
	*puc_mode = 0; /* manual mode is not supported */
	*puc_gain = 0;

	return(PAL_CFG_INVALID_INPUT);
}

/**
 * \brief Set Automatic Gain Mode (AGC)
 *
 * \param uc_mode     Auto/Manual mode
 * \param uc_gain     Initial receiving gain in auto mode
 *
 * \return PAL_CFG_SUCCESS if there is no error, otherwise returns
 * PAL_CFG_INVALID_INPUT.
 */
uint8_t pal_agc_set(uint8_t uc_mode, uint8_t uc_gain)
{
	UNUSED(uc_mode);
	UNUSED(uc_gain);

	return(PAL_CFG_INVALID_INPUT); /* Not supported */
}

/**
 * \brief Get Noise Level (NL)
 *
 * \param puc_noise    Pointer to Noise Level
 *
 * \return PAL_CFG_SUCCESS if there is no error, otherwise returns
 * PAL_CFG_INVALID_INPUT.
 */
uint8_t pal_nl_get(uint8_t *puc_noise)
{
	uint8_t uc_buffer;
	uint8_t uc_rssi;
	uint8_t uc_cinr;

	phy_get_cfg_param(PHY_ID_TX_BUFFER_ID, &uc_buffer, 1);
	phy_get_cfg_param(REG_ATPL230_TXRXBUF_RSSIAVG_RX0 + uc_buffer, &uc_rssi, 1);
	phy_get_cfg_param(REG_ATPL230_TXRXBUF_CINRMIN_RX0 + uc_buffer, &uc_cinr, 1);

	/* cinr is in 1/4 db. */
	*puc_noise = (uc_rssi - (uc_cinr >> 3)); 

	return(PAL_CFG_SUCCESS);
}

/**
 * \brief Get extended PHY layer parameter
 *
 * \param us_id       Parameter Identification (see atpl230.h)
 * \param *p_val      Pointer to parameter value
 * \param uc_len      Length of the parameter
 *
 * \return PAL_CFG_SUCCESS if there is no error, otherwise returns
 * PAL_CFG_INVALID_INPUT
 */
uint8_t pal_get_cfg_ex(uint16_t us_id, void *p_val, uint16_t uc_len)
{
	uint8_t uc_res = PAL_CFG_SUCCESS;

	if (us_id == PAL_ID_QT) {
		*((uint8_t *)p_val) = _pal_get_qt();
	} else if (us_id == PAL_ID_EX_SNR) {
		*((uint8_t *)p_val) = _pal_get_ex_snr();
	} else if (us_id == PAL_ID_MAC_EN) {
		*((uint8_t *)p_val) = phy_get_mac_en();
	} else if (us_id == PAL_ID_TX_TEST_SEND) {
		if (uc_tx_res_flag) {
			*((uint8_t *)p_val) = x_pal_tx_res.uc_result;
		} else {
			*((uint8_t *)p_val) = PHY_TX_RESULT_PROCESS;
		}
	} else if (us_id == PAL_ID_TX_TEST_SEND_BCN) {
		if (uc_tx_bcn_res_flag) {
			*((uint8_t *)p_val) = x_pal_bcn_tx_res.uc_result;
		} else {
			*((uint8_t *)p_val) = PHY_TX_RESULT_PROCESS;
		}
	} else if (us_id == PAL_ID_STATS_RX_FALSE_POSITIVE) {
		*((uint16_t *)p_val) = ul_rx_false_positive;
	} else if (us_id == PAL_ID_STATS_RX_BAD_CRC) {
		*((uint16_t *)p_val) = ul_rx_bad_crc;
	} else if (us_id == PAL_ID_RMSCALC1_TX0) {
		/* get rms calc */
		phy_get_cfg_param(REG_ATPL230_TXRXBUF_RMSCALC1_TX0 +
				((x_pal_tx_res.uc_id_buffer) << 1), p_val, 2);
	} else if (us_id == PAL_ID_CHECK_CFG) {
		phy_get_cfg_param(0xFFBF, p_val, 1);
	} else if (us_id == PAL_ID_ENABLE_CRC) {
		*((uint8_t *)p_val) = uc_pal_crc_enable;
	} else {
		return phy_get_cfg_param(us_id, p_val, uc_len);
	}

	return(uc_res);
}

/**
 * \brief Set extended PHY layer parameter
 *
 * \param us_id       Parameter Identification (see atpl230.h)
 * \param *p_val      Pointer to parameter value
 * \param uc_len      Length of the parameter
 *
 * \return PAL_CFG_SUCCESS if there is no error, otherwise returns
 * PAL_CFG_INVALID_INPUT.
 */
uint8_t pal_set_cfg_ex(uint16_t us_id, void *p_val, uint16_t uc_len)
{
	uint8_t uc_res = PAL_CFG_SUCCESS;

	if (us_id == PAL_ID_STATS_RX_FALSE_POSITIVE) {
		ul_rx_false_positive = *((uint16_t *)p_val);
	} else if (us_id == PAL_ID_MAC_EN) {
		/* enable mac coproc */
		if (*((uint8_t *)p_val)) {
			phy_mac_crc_enable();
		} else {
			phy_mac_crc_disable();
		}
	} else if (us_id == PAL_ID_SNA) {
		/* init SNA crc values */
		pcrc_configure_sna(p_val);
		return phy_set_cfg_param(us_id, p_val, uc_len);
	} else if (us_id == PAL_ID_RESET_CFG) {
		/* Reset and configure the PHY layer */
		phy_reset(PHY_RESET_SOFT_TYPE);
		/* Disable MAC coproc by default */
		phy_mac_crc_disable();
	} else if (us_id == PAL_ID_DISABLE_BCN_TX) {
		/* disable tx of buffer id */
		phy_transmission_buff_disable(0);
		return phy_set_cfg_param(us_id, p_val, uc_len);
	} else if (us_id == PAL_ID_ENABLE_CRC) {
		uc_pal_crc_enable = *((uint8_t *)p_val);
	} else {
		return phy_set_cfg_param(us_id, p_val, uc_len);
	}

	return(uc_res);
}

/**
 * \brief Binary Command extended PHY layer parameter
 *
 * \param us_id       Parameter Identification (see atpl230.h)
 * \param uc_cmd      Command [PAL_CMD_REG_AND, PAL_CMD_REG_OR, PAL_CMD_REG_XOR]
 * \param uc_mask     Bit mask
 *
 * \return PAL_CFG_SUCCESS if there is no error, otherwise returns
 * PAL_CFG_INVALID_INPUT.
 */
uint8_t pal_cmd_cfg_ex(uint16_t us_id, uint8_t uc_cmd, uint8_t uc_mask)
{
	if (ATPL230_REG_PARAM(us_id)) {
		return phy_cmd_cfg_param(us_id, uc_cmd, uc_mask);
	} else {
		return PAL_CFG_INVALID_INPUT;
	}
}

/**
 * \brief Get Signal Noise Ratio (SNR)
 *
 * \param puc_snr      SNR parameter
 *
 * \param uc_qt        QT parameter to get SNR level. If uc_qt = 0xFF, the qt is
 *                     calculated from last rx message
 *
 * \return PAL_CFG_SUCCESS if there is no error, otherwise returns
 * PAL_CFG_INVALID_INPUT.
 */
uint8_t pal_snr_get(uint8_t *puc_snr, uint8_t uc_qt)
{
	uint8_t uc_qt_value;

	if (uc_qt == PAL_QT_UNKNOW) {
		uc_qt_value = _pal_get_qt();
	} else {
		uc_qt_value = uc_qt;
	}

	if (uc_qt_value == 0) {
		*puc_snr = 0;
	} else {
		*puc_snr = uc_qt_value / 3 + 1;
		if (*puc_snr > 7) {
			*puc_snr  = 7;
		}
	}

	return(PAL_CFG_SUCCESS);
}

/* @} */

/* @cond 0 */
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/* @endcond */
