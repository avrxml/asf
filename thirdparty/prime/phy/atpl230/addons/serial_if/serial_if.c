/**
 * \file
 *
 * \brief ATPL230 Serial Interface for Physical layer
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

/* Board config */
#include "conf_board.h"

/* Phy includes */
#include "atpl230.h"
#include "conf_atpl230.h"
#include "conf_phy.h"

/* Serial interface */
#include "serial_if.h"

/* @cond 0 */
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/* @endcond */

/**
 * \weakgroup serial_plc_group
 * @{
 */

/* Default empty USI interface*/
extern int8_t usi_send_cmd(void *msg);
int8_t Dummy_serial_send_cmd(void *msg);

#ifdef __GNUC__
int8_t usi_send_cmd( void *msg ) __attribute__ ((weak, alias("Dummy_serial_send_cmd")));

#endif

#ifdef __ICCARM__
#pragma weak usi_send_cmd=Dummy_serial_send_cmd
#endif

#ifdef CONF_BOARD_LCD_EN
#include "c42364a.h"
#include <stdio.h>
/* \name Tx/Rx messages counters */
/* @{ */
static uint16_t us_rx_counter;
static uint16_t us_tx_counter;
/* @} */
#endif

/*  \name Data buffers
 * \note In noise mode: num symbols in rx noise mode is 8 and 97 bytes/symbol,
 * so 8 * 97 = 776 bytes. In addition to header size in worst case
 * Headers us_len = 2 * PHY_DMA_OFFSET = 2 * 16 = 32.	Total size is
 * 776 + 32 = 808 bytes */
/* @{ */
static uint8_t uc_serial_rsp_buf[808];   /*   Response working buffer */
static uint8_t uc_serial_data_buf[808];  /*   Receive working buffer */
/* @} */

/* \name Data structure to communicate with USI layer */
/* @{ */
static x_usi_serial_cmd_params x_phy_serial_msg;
/* @} */

/**
 * \internal
 * \brief Memcopy with byte order reversal.
 *
 * Copies puc_buf[] into puc_dst[], re-ordering the bytes to adapt to the serial
 * communication.
 *
 * \param puc_dst    Pointer to buffer where the data will be copied
 * \param puc_buf    Pointer to buffer data
 * \param us_len     Length of data to copy
 */
static void _memcpy_rev(uint8_t *puc_dst, uint8_t *puc_buf, uint16_t us_len)
{
	uint8_t *ptr_uc_mem_dst, *ptr_uc_mem_src;
	uint16_t us_idx;

	if (us_len <= 4) {
		ptr_uc_mem_dst = puc_dst + us_len - 1;
		ptr_uc_mem_src = puc_buf;
		for (us_idx = 0; us_idx < us_len; us_idx++) {
			*ptr_uc_mem_dst-- = (uint8_t)*ptr_uc_mem_src++;
		}
	} else {
		memcpy(puc_dst, puc_buf, us_len);
	}
}

/**
 * \internal
 * \brief Task to manage the serialization of the result of transmission through
 * USI.
 *
 * \param pvParameters Pointer that will be used as the parameter for the task
 * being created.
 */
static void _serial_if_get_tx_result_task(void *pvParameters)
{
	xPhyMsgTxResult_t x_write_result;
	uint8_t us_serial_response_len;
#ifdef CONF_BOARD_LCD_EN
	uint8_t uc_tx_channel;
	uint8_t uc_display_buf[10];
#endif

	UNUSED(pvParameters);

	/* get phy queue result */
	phy_tx_frame_result_cb(&x_write_result);

	if (x_write_result.uc_id_buffer != 0xFF) {
#ifdef CONF_BOARD_LCD_EN
		us_tx_counter++;
		c42364a_show_numeric_dec(us_tx_counter);
		phy_get_cfg_param(PHY_ID_CFG_TXRX_CHANNEL, &uc_tx_channel, 1);
		sprintf((char *)uc_display_buf, "TX CH%d", uc_tx_channel);
		c42364a_show_text((const uint8_t *)uc_display_buf);
#endif
		/* build response */
		us_serial_response_len = 0;
		uc_serial_rsp_buf[us_serial_response_len++] = SERIAL_IF_PHY_COMMAND_SEND_MSG_RSP;
		uc_serial_rsp_buf[us_serial_response_len++] = x_write_result.uc_id_buffer;
		uc_serial_rsp_buf[us_serial_response_len++] = x_write_result.uc_result;
		uc_serial_rsp_buf[us_serial_response_len++] = (uint8_t)(x_write_result.rms_calc >> 8);
		uc_serial_rsp_buf[us_serial_response_len++] = (uint8_t)(x_write_result.rms_calc);
		/* set usi parameters */
		x_phy_serial_msg.ptr_buf = &uc_serial_rsp_buf[0];
		x_phy_serial_msg.us_len = us_serial_response_len;
		usi_send_cmd(&x_phy_serial_msg);
	}
}

/**
 * \internal
 * \brief Task to manage the serialization of the reception message through USI.
 *
 * \param pvParameters Pointer that will be used as the parameter for the task
 * being created.
 */
static void _serial_if_get_rx_task(void *pvParameters)
{
#ifdef CONF_BOARD_LCD_EN
	uint8_t uc_tx_channel;
	uint8_t uc_display_buf[10];
#endif
	uint16_t us_serial_response_len;
	xPhyMsgRx_t x_read_msg;

	UNUSED(pvParameters);

	/* initialize read msg structure */
	memset(&x_read_msg, 0, sizeof(xPhyMsgRx_t));
	/* set pointer to reception data buffer */
	x_read_msg.data_buf = uc_serial_data_buf;

	/* block until get phy queue result */
	phy_rx_frame_cb(&x_read_msg);

	/* build response */
	if (x_read_msg.data_len) {
		us_serial_response_len = 0;
		if (x_read_msg.mode == MODE_NOISE) {
			uc_serial_rsp_buf[us_serial_response_len++] = SERIAL_IF_PHY_COMMAND_NOISE_RSP;
			uc_serial_rsp_buf[us_serial_response_len++] = x_read_msg.noise_result;
			uc_serial_rsp_buf[us_serial_response_len++] = (uint8_t)(x_read_msg.data_len >> 8);
			uc_serial_rsp_buf[us_serial_response_len++] = (uint8_t)x_read_msg.data_len;
			memcpy(&uc_serial_rsp_buf[us_serial_response_len], x_read_msg.data_buf, x_read_msg.data_len);
			us_serial_response_len += x_read_msg.data_len;
		} else {
			uc_serial_rsp_buf[us_serial_response_len++] = SERIAL_IF_PHY_COMMAND_RECEIVE_MSG;
			uc_serial_rsp_buf[us_serial_response_len++] = x_read_msg.uc_buff_id;
			uc_serial_rsp_buf[us_serial_response_len++] = x_read_msg.mode;
			uc_serial_rsp_buf[us_serial_response_len++] = x_read_msg.scheme;
			uc_serial_rsp_buf[us_serial_response_len++] = x_read_msg.header_type;
			uc_serial_rsp_buf[us_serial_response_len++] = x_read_msg.rssi_avg;
			uc_serial_rsp_buf[us_serial_response_len++] = x_read_msg.cinr_avg;
			uc_serial_rsp_buf[us_serial_response_len++] = (uint8_t)(x_read_msg.evm_header >> 8);
			uc_serial_rsp_buf[us_serial_response_len++] = (uint8_t)(x_read_msg.evm_header);
			uc_serial_rsp_buf[us_serial_response_len++] = (uint8_t)(x_read_msg.evm_payload >> 8);
			uc_serial_rsp_buf[us_serial_response_len++] = (uint8_t)(x_read_msg.evm_payload);
			uc_serial_rsp_buf[us_serial_response_len++] = (uint8_t)(x_read_msg.evm_header_acum >> 24);
			uc_serial_rsp_buf[us_serial_response_len++] = (uint8_t)(x_read_msg.evm_header_acum >> 16);
			uc_serial_rsp_buf[us_serial_response_len++] = (uint8_t)(x_read_msg.evm_header_acum >> 8);
			uc_serial_rsp_buf[us_serial_response_len++] = (uint8_t)(x_read_msg.evm_header_acum);
			uc_serial_rsp_buf[us_serial_response_len++] = (uint8_t)(x_read_msg.evm_payload_acum >> 24);
			uc_serial_rsp_buf[us_serial_response_len++] = (uint8_t)(x_read_msg.evm_payload_acum >> 16);
			uc_serial_rsp_buf[us_serial_response_len++] = (uint8_t)(x_read_msg.evm_payload_acum >> 8);
			uc_serial_rsp_buf[us_serial_response_len++] = (uint8_t)(x_read_msg.evm_payload_acum);

			if (x_read_msg.mode == MODE_TYPE_A) {
				uc_serial_rsp_buf[us_serial_response_len++] = (uint8_t)((x_read_msg.data_len - 3) >> 8);
				uc_serial_rsp_buf[us_serial_response_len++] = (uint8_t)(x_read_msg.data_len - 3);

				/* remove Generic Data Frame Header */
				memcpy(&uc_serial_rsp_buf[us_serial_response_len], x_read_msg.data_buf + 3, x_read_msg.data_len - 3);
				us_serial_response_len += x_read_msg.data_len - 3;
			} else {
				uc_serial_rsp_buf[us_serial_response_len++] = (uint8_t)(x_read_msg.data_len >> 8);
				uc_serial_rsp_buf[us_serial_response_len++] = (uint8_t)x_read_msg.data_len;

				memcpy(&uc_serial_rsp_buf[us_serial_response_len], x_read_msg.data_buf, x_read_msg.data_len);
				us_serial_response_len += x_read_msg.data_len;
			}
		}

	#ifdef CONF_BOARD_LCD_EN
		us_rx_counter++;
		c42364a_show_numeric_dec(us_rx_counter);
		phy_get_cfg_param(PHY_ID_CFG_TXRX_CHANNEL, &uc_tx_channel, 1);
		sprintf((char *)uc_display_buf, "RX CH%d", uc_tx_channel);
		c42364a_show_text((const uint8_t *)uc_display_buf);
	#endif
		/* set usi parameters */
		x_phy_serial_msg.ptr_buf = &uc_serial_rsp_buf[0];
		x_phy_serial_msg.us_len = us_serial_response_len;
		usi_send_cmd(&x_phy_serial_msg);
	}
}

/**
 * \brief Received message
 *
 * \param puc_rx_msg  Pointer to the data attached to the connection request
 * \param us_len      Data length of the data attached to the request
 *
 * \retval true if there is no error
 * \retval false if length is invalid or serial command is wrong
 */
uint8_t serial_if_api_parser(uint8_t *puc_rx_msg, uint16_t us_len)
{
	uint8_t uc_phy_id_len;
	uint8_t uc_serial_if_cmd;
	uint8_t uc_cmd_op, uc_mask;
	uint8_t *puc_rx;
	uint8_t uc_tx_result;
	uint8_t uc_cfg_value[0x0A];
	uint16_t us_phy_id;
	uint16_t us_serial_response_len;
	xPhyMsgTx_t x_phy_tx_msg;

	/* Protection for invalid us_length */
	if (!us_len) {
		return false;
	}

	/* Process received message */
	uc_serial_if_cmd = puc_rx_msg[0];
	puc_rx = &puc_rx_msg[1];
	us_serial_response_len = 0;

	switch (uc_serial_if_cmd) {
	/* GET command */
	case SERIAL_IF_PHY_COMMAND_GET_CFG:
		us_phy_id = ((uint16_t)*puc_rx++) << 8;
		us_phy_id += (uint16_t)*puc_rx++;
		uc_phy_id_len = *puc_rx++;
		if (phy_get_cfg_param(us_phy_id, uc_cfg_value, uc_phy_id_len) == PHY_CFG_SUCCESS) {
			/* build response */
			uc_serial_rsp_buf[us_serial_response_len++] = SERIAL_IF_PHY_COMMAND_GET_CFG_RSP;
			uc_serial_rsp_buf[us_serial_response_len++] = (uint8_t)(us_phy_id >> 8);
			uc_serial_rsp_buf[us_serial_response_len++] = (uint8_t)(us_phy_id);
			uc_serial_rsp_buf[us_serial_response_len++] = uc_phy_id_len;
			_memcpy_rev(&uc_serial_rsp_buf[4], uc_cfg_value, uc_phy_id_len);
			us_serial_response_len = uc_phy_id_len + 4;
			/* set usi parameters */
			x_phy_serial_msg.ptr_buf = &uc_serial_rsp_buf[0];
			x_phy_serial_msg.us_len = us_serial_response_len;
			usi_send_cmd(&x_phy_serial_msg);
		}

		break;

	/* SET command */
	case SERIAL_IF_PHY_COMMAND_SET_CFG:
		us_phy_id = ((uint16_t)*puc_rx++) << 8;
		us_phy_id += (uint16_t)*puc_rx++;
		uc_phy_id_len = *puc_rx++;
		_memcpy_rev(uc_cfg_value, puc_rx, uc_phy_id_len);
		us_serial_response_len = 0;
		/* build response */
		uc_serial_rsp_buf[us_serial_response_len++] = SERIAL_IF_PHY_COMMAND_SET_CFG_RSP;
		uc_serial_rsp_buf[us_serial_response_len++] = (uint8_t)(us_phy_id >> 8);
		uc_serial_rsp_buf[us_serial_response_len++] = (uint8_t)(us_phy_id);
		uc_serial_rsp_buf[us_serial_response_len++] = phy_set_cfg_param(us_phy_id, uc_cfg_value, uc_phy_id_len);
		/* set usi parameters */
		x_phy_serial_msg.ptr_buf = &uc_serial_rsp_buf[0];
		x_phy_serial_msg.us_len = us_serial_response_len;
		usi_send_cmd(&x_phy_serial_msg);
		break;

	/* CMD command (operations over bitfields, only in 8 bits) */
	case SERIAL_IF_PHY_COMMAND_CMD_CFG:
		us_phy_id = ((uint16_t)*puc_rx++) << 8;
		us_phy_id += (uint16_t)*puc_rx++;
		uc_cmd_op = *puc_rx++;
		uc_mask = *puc_rx++;
		/* build response */
		uc_serial_rsp_buf[us_serial_response_len++] = SERIAL_IF_PHY_COMMAND_CMD_CFG_RSP;
		uc_serial_rsp_buf[us_serial_response_len++] = (uint8_t)(us_phy_id >> 8);
		uc_serial_rsp_buf[us_serial_response_len++] = (uint8_t)(us_phy_id);
		uc_serial_rsp_buf[us_serial_response_len++] = uc_cmd_op;
		uc_serial_rsp_buf[us_serial_response_len++] = phy_cmd_cfg_param(us_phy_id, uc_cmd_op, uc_mask);
		/* set usi parameters */
		x_phy_serial_msg.ptr_buf = &uc_serial_rsp_buf[0];
		x_phy_serial_msg.us_len = us_serial_response_len;
		usi_send_cmd(&x_phy_serial_msg);
		break;

	/* Write command (send data msg) */
	case SERIAL_IF_PHY_COMMAND_SEND_MSG:
		/* configure write parameters */
		x_phy_tx_msg.uc_buff_id = *puc_rx++;
		x_phy_tx_msg.att_level = *puc_rx++;
		x_phy_tx_msg.mode = *puc_rx++;
		x_phy_tx_msg.scheme = *puc_rx++;
		x_phy_tx_msg.tmode = *puc_rx++;
		x_phy_tx_msg.tdelay = ((uint32_t)*puc_rx++) << 24;
		x_phy_tx_msg.tdelay += ((uint32_t)*puc_rx++) << 16;
		x_phy_tx_msg.tdelay += ((uint32_t)*puc_rx++) << 8;
		x_phy_tx_msg.tdelay += (uint32_t)*puc_rx++;
		x_phy_tx_msg.disable_rx = *puc_rx++;
		x_phy_tx_msg.data_len = ((uint16_t)*puc_rx++) << 8;
		x_phy_tx_msg.data_len += (uint16_t)*puc_rx++;
		x_phy_tx_msg.data_buf = uc_serial_data_buf;
		if (x_phy_tx_msg.mode == MODE_TYPE_A) {
			/* Generic Data Frame */
			uc_serial_data_buf[0] = 0xAA;
			uc_serial_data_buf[1] = 0xAA;
			uc_serial_data_buf[2] = 0xAA;
			x_phy_tx_msg.data_len += 3;
			/* copy data */
			memcpy(uc_serial_data_buf + 3, puc_rx, x_phy_tx_msg.data_len);
		} else {
			/* copy data */
			memcpy(uc_serial_data_buf, puc_rx, x_phy_tx_msg.data_len);
		}

		/* send to phy layer */
		uc_tx_result = phy_tx_frame(&x_phy_tx_msg);
		if (uc_tx_result) {             /* error in tx */
			/* build response */
			us_serial_response_len = 0;
			uc_serial_rsp_buf[us_serial_response_len++] = SERIAL_IF_PHY_COMMAND_SEND_MSG_RSP;
			uc_serial_rsp_buf[us_serial_response_len++] = x_phy_tx_msg.uc_buff_id;
			uc_serial_rsp_buf[us_serial_response_len++] = uc_tx_result;
			/* rms calc */
			uc_serial_rsp_buf[us_serial_response_len++] = 0;
			uc_serial_rsp_buf[us_serial_response_len++] = 0;
			/* set usi parameters */
			x_phy_serial_msg.ptr_buf = &uc_serial_rsp_buf[0];
			x_phy_serial_msg.us_len = us_serial_response_len;
			usi_send_cmd(&x_phy_serial_msg);
		}

		break;

	/* Set noise capture mode */
	case SERIAL_IF_PHY_COMMAND_NOISE_REQ:
	{
		uint8_t uc_noise_num_sym;
		uint8_t uc_noise_conf = 0;
		uint32_t ul_noise_delay_rec, ul_ref_time;

		uc_noise_num_sym = *puc_rx++;
		/* adjust number of symbols 0-7. */
		/* number of symbols not permitted. Max 8 */
		if (uc_noise_num_sym-- > 8) {
			/* build response */
			us_serial_response_len = 0;
			uc_serial_rsp_buf[us_serial_response_len++] = SERIAL_IF_PHY_COMMAND_NOISE_RSP;
			/* noise result = 7 -> reserved: error in ns */
			uc_serial_rsp_buf[us_serial_response_len++] = 7;
			/* set usi parameters */
			x_phy_serial_msg.ptr_buf = &uc_serial_rsp_buf[0];
			x_phy_serial_msg.us_len = us_serial_response_len;
			usi_send_cmd(&x_phy_serial_msg);
		} else {
			ul_noise_delay_rec = ((uint32_t)*puc_rx++) << 24;
			ul_noise_delay_rec += ((uint32_t)*puc_rx++) << 16;
			ul_noise_delay_rec += ((uint32_t)*puc_rx++) << 8;
			ul_noise_delay_rec += (uint32_t)*puc_rx++;
			if (ul_noise_delay_rec) {
				phy_get_cfg_param(REG_ATPL230_VHIGH_TIMER_BEACON_REF, &ul_ref_time, sizeof(ul_ref_time));
				ul_ref_time += ul_noise_delay_rec;
				phy_set_cfg_param(REG_ATPL230_TXRXBUF_RECTIME_NOISE1, &ul_ref_time, sizeof(ul_ref_time));
			}

			/* enable noise capture */
			uc_noise_conf = uc_noise_num_sym & ATPL230_TXRXBUF_NOISECONF_NS_Msk;
			uc_noise_conf |= ATPL230_TXRXBUF_NOISECONF_EBN_Msk;
			phy_set_cfg_param(REG_ATPL230_TXRXBUF_NOISECONF, &uc_noise_conf, sizeof(uint8_t));
			/* set forced mode */
			if (!ul_noise_delay_rec) {
				phy_cmd_cfg_param(REG_ATPL230_TXRXBUF_NOISECONF, PHY_CMD_CFG_OR, ATPL230_TXRXBUF_NOISECONF_FTN_Msk);
			}
		}
	}
	break;

	/* Set noise capture mode */
	case SERIAL_IF_PHY_COMMAND_GET_CFG_LIST:
	{
		uint8_t uc_num_elemts;
		uint8_t uc_id_len;
		uint8_t *puc_serial_rsp_buf;
		uint16_t us_id;

		/* reserve dynamic memory */
		puc_serial_rsp_buf = &uc_serial_rsp_buf[0];
		*puc_serial_rsp_buf++ = SERIAL_IF_PHY_COMMAND_GET_CFG_LIST_RSP;
		/* get parameters of the list */
		uc_num_elemts = *puc_rx++;
		*puc_serial_rsp_buf++ = uc_num_elemts;
		while (uc_num_elemts--) {
			*puc_serial_rsp_buf++ = *puc_rx;
			us_id = ((uint16_t)*puc_rx++) << 8;
			*puc_serial_rsp_buf++ = *puc_rx;
			us_id += (uint16_t)*puc_rx++;
			*puc_serial_rsp_buf++ = *puc_rx;
			uc_id_len = *puc_rx++;
			phy_get_cfg_param(us_id, uc_cfg_value, uc_id_len);
			_memcpy_rev(puc_serial_rsp_buf, uc_cfg_value, uc_id_len);
			puc_serial_rsp_buf += uc_id_len;
		}
		/* set usi parameters */
		x_phy_serial_msg.ptr_buf = &uc_serial_rsp_buf[0];
		x_phy_serial_msg.us_len = puc_serial_rsp_buf - &uc_serial_rsp_buf[0];
		usi_send_cmd(&x_phy_serial_msg);
	}
	break;

	/* Reset phy layer */
	case SERIAL_IF_PHY_COMMAND_RESET_PHY_LAYER:
	{
		if (*puc_rx) {
			/* Reset PHY layer SOFT*/
			phy_reset(PHY_RESET_SOFT_TYPE);
		} else {
			/* Reset PHY layer HARD */
			phy_reset(PHY_RESET_HARD_TYPE);
		}
	}
	break;

	default:
		return false;
	}

	return true;
}

/**
 * \brief Interface to manage the serialization of the reception message through
 * USI.
 *
 */
void serial_if_check_tx_result(void)
{
	uint8_t uc_null_params;

	_serial_if_get_tx_result_task(&uc_null_params);
}

/**
 * \brief Interface to manage the serialization of the reception message through
 * USI.
 *
 */
void serial_if_check_rx_msg(void)
{
	uint8_t uc_null_params;

	_serial_if_get_rx_task(&uc_null_params);
}

/**
 * \brief Initialize serial interface.
 *
 */
void serial_if_init(void)
{
	uint8_t uc_sna[6] = {0x00, 0x80, 0xE1, 0x00, 0x00, 0x6F};

	/* PROTOCOL_ATPL230 (see usi_conf.h) */
	x_phy_serial_msg.uc_protocol_type = 0x22;

	phy_mac_crc_disable();
	phy_set_cfg_param(REG_ATPL230_SNA0, uc_sna, sizeof(uc_sna));

	#ifdef CONF_BOARD_LCD_EN
	/* Reset rx/tx msg counters */
	us_rx_counter = 0;
	us_tx_counter = 0;
#endif
}

/**
 * \brief Default serial interface for unused USI.
 */
int8_t Dummy_serial_send_cmd(void *msg)
{
	UNUSED(msg);

	return 0;
}

/* @} */

/* @cond 0 */
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/* @endcond */
