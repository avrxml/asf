/**
 * \file
 *
 * \brief ATPL230 Sniffer Interface for Physical layer
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
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

/* System includes */
#include <stdio.h>
#include "string.h"

/* ASF */
#include "asf.h"

/* Board config */
#include "conf_board.h"
#include "board.h"
#if BOARD == ATPL230AMB
#include "led.h"
#define BSP_LED_SUPPORT
#elif BOARD == SAM4CP16BMB
#include "led.h"
#define BSP_LED_SUPPORT
#endif

/* Phy includes */
#include "atpl230.h"
#include "conf_atpl230.h"
#include "conf_phy.h"

/* Sniffer interface */
#include "sniffer_if.h"

/* @cond 0 */
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/* @endcond */

/**
 * \weakgroup sniffer_plc_group
 * @{
 */

/* Default empty USI interface*/
usi_status_t Dummy_sniffer_send_cmd(void *msg);

#ifdef __GNUC__
usi_status_t usi_send_cmd(void *msg) __attribute__ ((weak, alias("Dummy_sniffer_send_cmd")));

#endif

#ifdef __ICCARM__
#pragma weak usi_send_cmd=Dummy_sniffer_send_cmd
#endif

/* \name Data structure to communicate with USI layer */
/* @{ */
static x_usi_sniffer_cmd_params x_phy_sniffer_msg;
/* @} */

#ifdef BSP_LED_SUPPORT
/* \name Led indicator */
/* @{ */
static uint8_t uc_sniffer_led_enable;
/* @} */
#endif

/*  \name Data buffers
 *  \note In noise mode: num symbols in rx noise mode is 8 and 97 bytes/symbol,
 * so 8 * 97 = 776 bytes. In addition to header size in worst case
 *   Headers us_len = 2 * PHY_DMA_OFFSET = 2 * 16 = 32.	Total size is 776 + 32 =
 * 808 bytes */
/* @{ */
static uint8_t uc_sniffer_data_buf[800];    /*   Receive working buffer */
static uint8_t uc_sniffer_rsp_buf[800];     /*   Response working buffer */
/* @} */

/**
 * \internal
 * \brief Task to manage the serialization of the reception message through USI.
 *
 */
void sniffer_if_process(void)
{
	xPhyMsgRx_t x_read_msg;
	uint16_t us_sniffer_response_len;
	uint8_t uc_channel, uc_symbols, uc_mac_en, uc_snr, uc_quality;
	uint32_t ul_timeIni, ul_timeEnd, ul_len;
	uint8_t uc_sna[6];

	/* initialize read msg structure */
	memset(&x_read_msg, 0, sizeof(xPhyMsgRx_t));
	/* set pointer to reception data buffer */
	x_read_msg.data_buf = uc_sniffer_data_buf;

	/* block until get phy queue result */
	phy_rx_frame_cb(&x_read_msg);

	/* build response */
	if (x_read_msg.data_len) {
		/* Update SNA for CRC calculations (only in BCN msg) */
		/* check if header type is beacon */
		if (x_read_msg.header_type == 0x2) {
			/* Update SNA for CRC calculations */
			if (x_read_msg.mode == MODE_TYPE_A) {
				memcpy(uc_sna, x_read_msg.data_buf + 6, 6);
			} else if (x_read_msg.mode == MODE_TYPE_B) {
				memcpy(uc_sna, x_read_msg.data_buf + 7, 6);
			} else if (x_read_msg.mode == MODE_TYPE_BC) {
				memcpy(uc_sna, x_read_msg.data_buf + 7, 6);
			}

			/* update SNA configuration */
			phy_set_cfg_param(REG_ATPL230_SNA0, uc_sna, sizeof(uc_sna));
		}

#ifdef BSP_LED_SUPPORT
		/* Toogle rx led */
		if (uc_sniffer_led_enable) {
			LED_Toggle(LED0);
		}
#endif
		us_sniffer_response_len = 0;

		if (x_read_msg.mode == MODE_TYPE_A) {
			uc_sniffer_rsp_buf[us_sniffer_response_len++] = SNIFFER_IF_PHY_MESSAGE_PRIME_1_3;
		} else if (x_read_msg.mode == MODE_TYPE_B) {
			uc_sniffer_rsp_buf[us_sniffer_response_len++] = SNIFFER_IF_PHY_MESSAGE_PRIME_PLUS;
		} else if (x_read_msg.mode == MODE_TYPE_BC) {
			uc_sniffer_rsp_buf[us_sniffer_response_len++] = SNIFFER_IF_PHY_MESSAGE_PRIME_PLUS;
		} else {
			x_read_msg.data_len = 0;
			return;
		}

		uc_sniffer_rsp_buf[us_sniffer_response_len++] = SNIFFER_VERSION;
		uc_sniffer_rsp_buf[us_sniffer_response_len++] = SNIFFER_ATPL230;
		uc_sniffer_rsp_buf[us_sniffer_response_len++] = x_read_msg.scheme;

		phy_get_cfg_param(PHY_ID_RX_PAYLOAD_LEN_SYM, &uc_symbols, 1);
		uc_sniffer_rsp_buf[us_sniffer_response_len++] = uc_symbols;

		/* get PHY params needed by the sniffer */
		phy_get_cfg_param(PHY_ID_CFG_TXRX_CHANNEL, &uc_channel, 1);

		uc_snr = (x_read_msg.cinr_avg >> 2) + 3;
		uc_quality = ((x_read_msg.cinr_avg >> 2) / 3) + 1;
		if (uc_quality > 7) {
			uc_quality = 7;
		}

		uc_sniffer_rsp_buf[us_sniffer_response_len++] = uc_quality;
		uc_sniffer_rsp_buf[us_sniffer_response_len++] = uc_snr;
		uc_sniffer_rsp_buf[us_sniffer_response_len++] = uc_channel;
		/* padding (11 bytes) */
		uc_sniffer_rsp_buf[us_sniffer_response_len++] = 0;
		uc_sniffer_rsp_buf[us_sniffer_response_len++] = 0;
		uc_sniffer_rsp_buf[us_sniffer_response_len++] = 0;
		uc_sniffer_rsp_buf[us_sniffer_response_len++] = 0;
		uc_sniffer_rsp_buf[us_sniffer_response_len++] = 0;
		uc_sniffer_rsp_buf[us_sniffer_response_len++] = 0;
		uc_sniffer_rsp_buf[us_sniffer_response_len++] = 0;
		uc_sniffer_rsp_buf[us_sniffer_response_len++] = 0;
		uc_sniffer_rsp_buf[us_sniffer_response_len++] = 0;
		uc_sniffer_rsp_buf[us_sniffer_response_len++] = 0;
		uc_sniffer_rsp_buf[us_sniffer_response_len++] = 0;

		phy_get_cfg_param(REG_ATPL230_TXRXBUF_RECTIME1_RX0 + (x_read_msg.uc_buff_id * 4), &ul_timeEnd, 4);

		/* Get length in microsec. */
		if (x_read_msg.mode == MODE_TYPE_A) {
			ul_len = TIME_PRIME_1_3_PREAMBLE_US + TIME_PRIME_1_3_HEADER_US + (uc_symbols *  TIME_OFDM_SYMBOL_US);
		} else if (x_read_msg.mode == MODE_TYPE_B) {
			ul_len = TIME_PRIME_PLUS_PREAMBLE_US + TIME_PRIME_PLUS_HEADER_US + ((x_read_msg.scheme > 6 ? uc_symbols * 4 : uc_symbols) * TIME_OFDM_SYMBOL_US);
		} else if (x_read_msg.mode == MODE_TYPE_BC) {
			ul_len = TIME_PRIME_1_3_PREAMBLE_US + TIME_PRIME_1_3_HEADER_US + TIME_PRIME_PLUS_PREAMBLE_US + TIME_PRIME_PLUS_HEADER_US +
					((x_read_msg.scheme > 6 ? uc_symbols * 4 : uc_symbols) * TIME_OFDM_SYMBOL_US);
		} else {
			ul_len = 0;
		}

		ul_timeIni = (ul_timeEnd - TIME_IN_TICS(ul_len));

		uc_sniffer_rsp_buf[us_sniffer_response_len++] = (ul_timeIni >> 24);
		uc_sniffer_rsp_buf[us_sniffer_response_len++] = (ul_timeIni >> 16) & 0xFF;
		uc_sniffer_rsp_buf[us_sniffer_response_len++] = (ul_timeIni >> 8) & 0xFF;
		uc_sniffer_rsp_buf[us_sniffer_response_len++] = ul_timeIni & 0xFF;

		uc_sniffer_rsp_buf[us_sniffer_response_len++] = (ul_timeEnd >> 24);
		uc_sniffer_rsp_buf[us_sniffer_response_len++] = (ul_timeEnd >> 16) & 0xFF;
		uc_sniffer_rsp_buf[us_sniffer_response_len++] = (ul_timeEnd >> 8) & 0xFF;
		uc_sniffer_rsp_buf[us_sniffer_response_len++] = ul_timeEnd & 0xFF;

		uc_sniffer_rsp_buf[us_sniffer_response_len++] = 0;
		uc_sniffer_rsp_buf[us_sniffer_response_len++] = x_read_msg.rssi_avg;

		uc_mac_en = phy_get_mac_en();
		uc_sniffer_rsp_buf[us_sniffer_response_len++] = uc_mac_en;

		/* Compute length with CRC, */
		ul_len = x_read_msg.data_len;

		uc_sniffer_rsp_buf[us_sniffer_response_len++] = (uint8_t)(ul_len >> 8);
		uc_sniffer_rsp_buf[us_sniffer_response_len++] = (uint8_t)(ul_len);

		memcpy(&uc_sniffer_rsp_buf[us_sniffer_response_len], x_read_msg.data_buf, ul_len);
		us_sniffer_response_len += ul_len;

		x_phy_sniffer_msg.ptr_buf = uc_sniffer_rsp_buf;
		x_phy_sniffer_msg.us_len = us_sniffer_response_len;
		usi_send_cmd(&x_phy_sniffer_msg);
	}
}

/**
 * \brief Received message
 *
 * \param puc_rx_msg  Pointer to the received data
 * \param us_len      Data length of the received data
 *
 * \retval true if there is no error
 * \retval false if length is invalid or sniffer command is wrong
 */
uint8_t serial_if_sniffer_api_parser(uint8_t *puc_rx_msg, uint16_t us_len)
{
	uint8_t uc_sniffer_if_cmd;
	uint8_t uc_data;

	/* Protection for invalid length */
	if (!us_len) {
		return true;
	}

	/* Process received message */
	uc_sniffer_if_cmd  = puc_rx_msg[0];
	uc_data = puc_rx_msg[1];

	switch (uc_sniffer_if_cmd) {
	/* GET command */
	case SNIFFER_IF_PHY_COMMAND_MAC_CRC:
		if (uc_data == 1) {
			phy_mac_crc_enable();
		} else {
			phy_mac_crc_disable();
		}

		break;

	case SNIFFER_IF_PHY_COMMAND_SET_CHANNEL:
		phy_set_cfg_param(PHY_ID_CFG_TXRX_CHANNEL, (void *)&uc_data, 1);
		break;

	case SNIFFER_IF_PHY_COMMAND_ENABLE_PRIME_PLUS_ROBUST:
	case SNIFFER_IF_PHY_COMMAND_MESSAGE:
		/* NOT IMPLEMENTED */
		break;

	default:
		return false;
	}
	return true;
}

/**
 * \brief Initialize sniffer interface.
 *
 * \param  uc_enable_led Enable led
 */
void sniffer_if_init(uint8_t uc_enable_led)
{
	/* default SNA */
	uint8_t uc_sna[6] = {0x00, 0x80, 0xE1, 0x00, 0x00, 0x6F};
	uint8_t uc_rx_qr_enabled = 0x01;

	x_phy_sniffer_msg.uc_protocol_type = 0x13;

#ifdef BSP_LED_SUPPORT
	uc_sniffer_led_enable = uc_enable_led;
#else
	UNUSED(uc_enable_led);
#endif

	phy_mac_crc_disable();
	phy_set_cfg_param(REG_ATPL230_SNA0, uc_sna, sizeof(uc_sna));
	phy_set_cfg_param(PHY_ID_RX_QR_MODE_ID, &uc_rx_qr_enabled, 1);
}

/**
 * \brief Default serial interface for unused USI.
 */
usi_status_t Dummy_sniffer_send_cmd(void *msg)
{
	UNUSED(msg);

	return USI_STATUS_PROTOCOL_NOT_FOUND;
}

/* @} */

/* @cond 0 */
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/* @endcond */
