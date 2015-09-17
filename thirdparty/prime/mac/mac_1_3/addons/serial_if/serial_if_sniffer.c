/**
 * \file
 *
 * \brief PRIME Embedded Sniffer Serial Interface
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

#include <string.h>
#include "compiler.h"

#include "serial_if_sniffer.h"

/* From module: PRIME MAC Layer */
#include <mngl.h>
#include <prime.h>

/* From module: PLC Universal Serial Interface */
#include <usi.h>

/* From module: Physical Abstraction Layer (PAL) interface - ATPL230 and PRIME
 * MAC 1.3 */
#include <pal.h>

/* @cond 0 */
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/* @endcond */

/**
 * \weakgroup serial_sniffer_prime_group
 * @{
 */

/*  \name Commands received */
/* @{ */
#define SNIFFER_ACTIVATE_CRC                 0x01
#define SNIFFER_SELECT_CHANNEL               0x02
#define SNIFFER_ROBO_MODULATIONS             0x03
/* @} */

/*  \name Response */
/* @{ */
#define SNIFFER_PDU_CMD                      0x20
/* @} */

/*  \name Version format of header */
/* @{ */
#define SNIFFER_PDU_VERSION_FORMAT           0x01
/* @} */

/*  \name Length of the header */
/* @{ */
#define SNIFFER_PDU_HEADER_LENGTH            32
/* @} */

/*  \name Timer values */
/* @{ */
/*  Timer is a 20-bit counter, so to complete 32 bits is shifted 12 bits to */
/*  the left */
#define TIME_SHIFT_BITS                      12
/*  Time unit for phy timer is 10us */
#define TIME_PHY_UNIT_US                     10
/*  PREAMBLE + HEADER + PAYLOAD = 2048us  + 4480us + M*2240us */
#define TIME_PREAMBLE_US                     2048L
#define TIME_HEADER_US                       4480L
#define TIME_OFDM_SYMBOL_US                  2240L
#define TIME_PHY_PREAMBLE ((TIME_PREAMBLE_US / TIME_PHY_UNIT_US) << TIME_SHIFT_BITS)
#define TIME_PHY_HEADER   ((TIME_HEADER_US / TIME_PHY_UNIT_US) << TIME_SHIFT_BITS)
#define TIME_PHY_OFDM_SYMBOL ((TIME_OFDM_SYMBOL_US / TIME_PHY_UNIT_US) << TIME_SHIFT_BITS)
/* @} */

/*  \name Mac header size */
/* @{ */
#define MAC_HEADER_SIZE              7
/* @} */

/* External variables */
extern uint8_t cfgSnifferType;

/* Serial Buffer */
static uint8_t frameMacToPack[512];

/*  \name Include a long of 32 bits in a vector */
/* @{ */
/*  Includes highest significative byte at first position in vector */
#define _pack_ul(ul_data) \
	{ \
		*puc_tx++ = (uint8_t)(ul_data >> 24); \
		*puc_tx++ = (uint8_t)(ul_data >> 16); \
		*puc_tx++ = (uint8_t)(ul_data >> 8); \
		*puc_tx++ = (uint8_t)ul_data; \
	}
/* @} */

/*  \name Include a long of 32 bits in a vector */
/* @{ */
/*  Includes highest significative byte at first position in vector */
#define _pack_us(us_data) \
	{ \
		*puc_tx++ = (uint8_t)(us_data >> 8); \
		*puc_tx++ = (uint8_t)us_data; \
	}
/* @} */

/**
 * \internal
 * \brief Get the number of symbols in PDU
 *
 * \param us_pdu_len           PDU length
 * \param uc_scheme            Scheme
 * \param uc_phy_header_type   PHY header type
 *
 * \retval number of symbols in PDU
 */
static uint16_t _serial_if_sniffer_get_num_symbols(uint16_t us_pdu_len,
		uint8_t uc_scheme, uint8_t uc_phy_header_type)
{
	uint16_t us_num_symbols;
	uint8_t uc_num_bytes_symbol;
	uint16_t us_long_total;

	us_long_total = us_pdu_len - MAC_HEADER_SIZE;
	if (uc_phy_header_type == PHY_HT_PROMOTION) {
		us_long_total++;
	} else {
		us_long_total += 4;
	}

	switch (uc_scheme & 0x03) {
	case 0x00:
		uc_num_bytes_symbol = 12;
		break;

	case 0x01:
		uc_num_bytes_symbol = 24;
		break;

	case 0x02:
		uc_num_bytes_symbol = 36;
		break;

	default:
		uc_num_bytes_symbol = 12;
		break;
	}

	if (uc_scheme & 0x04) { /* Viterbi */
		us_long_total++;
		uc_num_bytes_symbol >>= 1;
	}

	us_num_symbols = us_long_total / uc_num_bytes_symbol;
	if (us_long_total % uc_num_bytes_symbol) {
		us_num_symbols++;
	}

	return us_num_symbols;
}

/**
 * \brief Received message
 *
 * \note Process Dialogue with peer
 *      - Proces Profiles
 *      - Wait for received message
 *      - Process incoming message
 *      - Send response if necessary
 *
 * \param puc_rx_msg  Pointer to the data attached to the connection request
 * \param us_len      Data length of the data attached to the request
 *
 * \retval true primitive stored
 * \retval false primitive could not be stored
 */
uint8_t serial_if_sniffer_api_parser(uint8_t *puc_rx_msg, uint16_t us_len)
{
#ifdef SNIFFER_STAND_ALONE
	uint8_t uc_cmd;
	uint8_t *puc_rx;

	/* Pointer to message data */
	puc_rx = puc_rx_msg;

	/* Extract command */
	uc_cmd = *puc_rx++;

	switch (cfgSnifferType) {
	case SNIFFER_ATPL240:
		if (uc_cmd == SNIFFER_SELECT_CHANNEL) {
			phy1020Sniffer_setRxChannel(*puc_rx++);
		}

	case SNIFFER_ATPL210:
		if (uc_cmd == SNIFFER_ACTIVATE_CRC) {
			phy1020Sniffer_setMacCoprocEn(*puc_rx++);
		}

	default:
		break;
	}
#endif
	puc_rx_msg = puc_rx_msg;
	us_len = us_len;
	return true;
}

/**
 * \brief This function packs the received packet.
 *
 * \param px_snif     Pointer to sniffer data parameters
 * \param puc_buf     Pointer to the data
 * \param us_len      Data length of the data
 *
 */
void serial_if_sniffer_pack_pdu(SnifferParam *px_snif, uint8_t *puc_buf,
		uint16_t us_len)
{
	uint8_t *puc_tx;
	uint16_t us_idx;
	uint32_t ul_rx_time_end;
	uint8_t uc_rx_scheme;
	uint8_t uc_rx_payload_len;
	uint32_t l_rx_time;
	uint8_t uc_rx_rssi;
	uint8_t uc_snr, uc_snr_ex, uc_chn;
	x_usi_serial_cmd_params_t x_pkt_usi;
	uint8_t uc_buff_id;

	/* Fill in received data, leaving space for required overhead */
	puc_tx = &frameMacToPack[0];
	*puc_tx++ = SNIFFER_PDU_CMD;
	*puc_tx++ = SNIFFER_PDU_VERSION_FORMAT;
	*puc_tx++ = cfgSnifferType;

	if ((cfgSnifferType & MASK_SNIFFER_EMBEDDED) == 0) {
		pal_get_cfg_ex(PAL_ID_RX_SCHEME, &uc_rx_scheme, 1);
		pal_get_cfg_ex(PAL_ID_RX_BUFFER_ID, &uc_buff_id, sizeof(uc_buff_id));
		pal_get_cfg_ex(PAL_ID_RX_RSSI_AVG_RX0 + uc_buff_id, &uc_rx_rssi, sizeof(uc_rx_rssi));
		pal_get_cfg_ex(PAL_ID_TXRX_CHANNEL, &uc_chn, 1);
		pal_get_cfg_ex(PAL_ID_RX_PAYLOAD_LEN, &uc_rx_payload_len, 1);
		/* Reception quality */
		pal_snr_get(&uc_snr, PAL_QT_UNKNOW);
		pal_get_cfg_ex(PAL_ID_EX_SNR, &uc_snr_ex, 1);
	} else if ((px_snif->type == SNIFFER_RX) || (px_snif->type == SNIFFER_TX)) {
		uc_rx_scheme = px_snif->scheme;
		uc_rx_payload_len = _serial_if_sniffer_get_num_symbols(us_len, uc_rx_scheme,
				((puc_buf[2] >> 4) & 0x03));
		l_rx_time = px_snif->time;
		pal_get_cfg_ex(PAL_ID_TXRX_CHANNEL, &uc_chn, 1);
		if (px_snif->type == SNIFFER_RX) {
			pal_get_cfg_ex(PAL_ID_RX_SCHEME, &uc_rx_scheme, 1);
			pal_get_cfg_ex(PAL_ID_RX_BUFFER_ID, &uc_buff_id, sizeof(uc_buff_id));
			pal_get_cfg_ex(PAL_ID_RX_RSSI_AVG_RX0 + uc_buff_id, &uc_rx_rssi, sizeof(uc_rx_rssi));
			/* Reception quality */
			pal_snr_get(&uc_snr, PAL_QT_UNKNOW);
			pal_get_cfg_ex(PAL_ID_EX_SNR, &uc_snr_ex, 1);
		} else {
			uc_rx_rssi = 136;
			uc_snr_ex = 59;
			uc_snr = 7;
		}
	} else { /* Sniffer Serial */
		pal_timer_get(&l_rx_time);
		uc_rx_scheme = 0x02;
		uc_rx_payload_len = _serial_if_sniffer_get_num_symbols(us_len, uc_rx_scheme,
				((puc_buf[2] >> 4) & 0x03));
		uc_rx_rssi = 136;
		uc_snr_ex = 59;
		uc_snr = 7;
		uc_chn = 0xff;  /* channel reserved for serial line */
	}

	/* Modulation scheme */
	*puc_tx++ = uc_rx_scheme;
	/* Payload length */
	*puc_tx++ = uc_rx_payload_len;

	/* SNR */
	*puc_tx++ = uc_snr;
	*puc_tx++ = uc_snr_ex;

	/* Reception Channel */
	*puc_tx++ = uc_chn;

	/* Set to  '0' unused, reserved 11 bytes */
	for (us_idx = 0; us_idx < 11; us_idx++) {
		*puc_tx++ = 0;
	}

	/* Add Time start */
	l_rx_time <<= TIME_SHIFT_BITS;  /* PC Sniffer expects a shifted value */
	_pack_ul(l_rx_time);

	/* Add Time End */
	ul_rx_time_end = l_rx_time + TIME_PHY_PREAMBLE + TIME_PHY_HEADER +
			(uc_rx_payload_len * TIME_PHY_OFDM_SYMBOL);
	_pack_ul(ul_rx_time_end);

	/* Add Power */
	_pack_us(uc_rx_rssi);

	/* Add Crc Coprocessor status */
	#ifdef SNIFFER_STAND_ALONE
	*puc_tx++ = phy1020Sniffer_getMacCoprocEn();
	#else
	*puc_tx++ = true;
	#endif
	/* Add pdu length */
	_pack_us(us_len);

	/* Copy pdu to Buffer */
	memcpy(puc_tx, puc_buf, us_len);

	/* Pack and send the primitive */
	x_pkt_usi.uc_protocol_type = PROTOCOL_SNIF_PRIME;
	x_pkt_usi.ptr_buf = (uint8_t *)&frameMacToPack[0];
	x_pkt_usi.us_len = SNIFFER_PDU_HEADER_LENGTH + us_len;
	usi_send_cmd(&x_pkt_usi);

	return;
}

/*  @} */

/* @cond 0 */
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/* @endcond */
