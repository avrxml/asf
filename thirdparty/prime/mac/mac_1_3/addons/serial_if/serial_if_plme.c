/**
 * \file
 *
 * \brief PRIME Serial Interface for PLME
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

#include "serial_if_plme.h"

/* From module: PRIME MAC Layer */
#include <mngl.h>
#include <prime.h>

/* From module: PLC Universal Serial Interface */
#include <usi.h>

/* @cond 0 */
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/* @endcond */

/**
 * \weakgroup serial_plme_prime_group
 * @{
 */

/*  \name Reception variables */
/* @{ */
static uint8_t *puc_rx;             /*  Pointer to received message */
/* @} */

static uint8_t NewPlmeCallBackCmd[PRIME_PLME_DATA_SIZE];
static uint8_t commandPlmeToPack[PRIME_PLME_DATA_SIZE];

#pragma message ("PRIME_MAC_CFG: PLME_SERIALIZATION")

/**
 * \internal
 * \brief This function extracts the PLME SAP Testmode.Request.
 *
 * \note This function extracts the PLME SAP Testmode.Request from the
 * received packet and stores the data in the circular buffer if possible.
 *
 * \retval 0 primitive stored
 * \retval -1 primitive could not be stored
 */
static int _serial_if_plme_unpack_testmode_req(void)
{
	uint8_t *puc_rx_buff;
	uint8_t uc_enable;
	uint8_t uc_mode;
	uint8_t uc_modulation;
	uint8_t uc_pwr_level;

	/* Extract PLME primitive */
	puc_rx_buff = puc_rx;
	uc_enable = (uint8_t)(*puc_rx_buff++);
	uc_mode = (uint8_t)(*puc_rx_buff++);
	uc_modulation = (uint8_t)(*puc_rx_buff++);
	uc_pwr_level = (uint8_t)(*puc_rx_buff++);

	/* Store the primitive in the circular buffer */
	return(prime_PLME_TESTMODE_request(uc_enable, uc_mode, uc_modulation,
	       uc_pwr_level));
}

/**
 * \internal
 * \brief This function extracts the PLME SAP Get.Request.
 *
 * \note This function extracts the PLME SAP Get.Request from the
 * received packet and stores the data in the circular buffer if possible.
 *
 * \retval 0 primitive stored
 * \retval -1 primitive could not be stored
 */
static int _serial_if_plme_unpack_get_req(void)
{
	uint8_t *puc_rx_buff;
	uint16_t us_pib_attr;

	/* Extract PLME primitive */
	puc_rx_buff = puc_rx;
	us_pib_attr = (((uint16_t)puc_rx_buff[0]) << 8) + (uint16_t)puc_rx_buff[1];

	/* Store the primitive in the circular buffer */
	return(prime_PLME_GET_request(us_pib_attr));
}

/**
 * \internal
 * \brief This function extracts the PLME SAP Set.Request.
 *
 * \note This function extracts the PLME SAP Set.Request from the
 * received packet and stores the data in the circular buffer if possible.
 *
 * \retval 0 primitive stored
 * \retval -1 primitive could not be stored
 */
static int _serial_if_plme_unpack_set_req(void)
{
	uint8_t *puc_rx_buff, *puc_rx_buff2;
	uint8_t uc_use_random;
	uint16_t us_pib_attr, us_len;
	uint32_t ul_pib_attr_val;

	/* Extract PLME primitive */
	puc_rx_buff = puc_rx;
	us_pib_attr = (((uint16_t)puc_rx_buff[0]) << 8) + (uint16_t)puc_rx_buff[1];
	puc_rx_buff += 2;

	if (us_pib_attr == 0x8083) {   /* PHY_AGC */
		/* Store the primitive in the circular buffer */
		/* 2: mode, gain */
		return(prime_PLME_SET_BUF_request(us_pib_attr, (uint8_t *)puc_rx_buff, 2));
	} else if (us_pib_attr == 0x8089) {  /* PHY_TX_PARAMS */
		puc_rx_buff2 = puc_rx_buff;
		puc_rx_buff2++;
		puc_rx_buff2++;
		us_len = (((uint16_t)puc_rx_buff2[0]) << 8) + (uint16_t)puc_rx_buff2[1];
		puc_rx_buff2 += 2;
		uc_use_random = (uint8_t)(*puc_rx_buff2++);
		if (uc_use_random) {
			us_len = puc_rx_buff2 - puc_rx_buff;
		} else {
			us_len += puc_rx_buff2 - puc_rx_buff;
		}

		/* Store the primitive in the circular buffer */
		return(prime_PLME_SET_BUF_request(us_pib_attr, (uint8_t *)puc_rx_buff, us_len));
	} else {
		/* ul_pib_attr_val = getUint32((uint8_t **)&puc_rx_buff); */
		ul_pib_attr_val = ((uint32_t)puc_rx_buff[0] << 24) |
				((uint32_t)puc_rx_buff[1] << 16) |
				((uint32_t)puc_rx_buff[2] << 8) |
				(uint32_t)puc_rx_buff[3];
		/* Store the primitive in the circular buffer */
		return(prime_PLME_SET_request(us_pib_attr, ul_pib_attr_val));
	}
}

/**
 * \internal
 * \brief Packets a PLME Callback to avoid misaligments
 *
 * \param puc_plme_command     Pointer to the PLME command
 * \param l_len                Command length
 *
 * \retval length of the buffer packet
 */
static int _serial_if_plme_pack_plme_callback(uint8_t *puc_plme_command,
		int l_len)
{
	uint8_t *puc_command;

	puc_command = puc_plme_command;
	memcpy(commandPlmeToPack, puc_plme_command, l_len);
	switch (((PlmeCommand *)puc_command)->cmd) {
	case PRIME_PLME_RESET_CONFIRM:
	case PRIME_PLME_SLEEP_CONFIRM:
	case PRIME_PLME_RESUME_CONFIRM:
	case PRIME_PLME_TESTMODE_CONFIRM:
		*puc_command++ = ((PlmeResult *)commandPlmeToPack)->cmd;
		*puc_command++ = ((PlmeResult *)commandPlmeToPack)->result;
		return(puc_command - puc_plme_command);

	case PRIME_PLME_GET_CONFIRM:
		if (l_len == sizeof(PlmeGetValue)) {
			puc_command += 2;        /* (cmd + result) */
			*puc_command++ = commandPlmeToPack[2];
			*puc_command++ = commandPlmeToPack[3];
			*puc_command++ = commandPlmeToPack[4];
			*puc_command++ = commandPlmeToPack[5];
			*puc_command++ = commandPlmeToPack[6];
			*puc_command++ = commandPlmeToPack[7];
			return(puc_command - puc_plme_command);
		} else {
			puc_command += 2;        /* (cmd + result) */
			*puc_command++ = commandPlmeToPack[2];
			*puc_command++ = commandPlmeToPack[3];
			memcpy(puc_command, &commandPlmeToPack[4], l_len - (sizeof(PlmeGetArray) - 1));
			puc_command += l_len - (sizeof(PlmeGetArray) - 1);
			return(puc_command - puc_plme_command);
		}

	case PRIME_PLME_SET_CONFIRM:
		puc_command += 2;        /* (cmd + result) */
		*puc_command++ = commandPlmeToPack[2];
		*puc_command++ = commandPlmeToPack[3];
		return(puc_command - puc_plme_command);
	}
	/* This point of code shouldn't be reached */
	return (-1);
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
 * \retval 1 primitive stored
 * \retval 0 primitive could not be stored
 */
uint8_t serial_if_plme_api_parser(uint8_t *puc_rx_msg, uint16_t us_len)
{
	uint8_t uc_cmd;
	int8_t c_result = -1;

	/* Extract command */
	uc_cmd = CMD_PRIME_PROTOCOL(puc_rx_msg[0]);
	UNUSED(us_len);

	/* Pointer to message data */
	puc_rx = (uint8_t *)&puc_rx_msg[1];

	/* Check received command */
	switch (uc_cmd) {
	case PRIME_PLME_RESET_REQUEST:
		c_result = prime_PLME_RESET_request();
		break;

	case PRIME_PLME_SLEEP_REQUEST:
		c_result = prime_PLME_SLEEP_request();
		break;

	case PRIME_PLME_RESUME_REQUEST:
		c_result = prime_PLME_RESUME_request();
		break;

	case PRIME_PLME_TESTMODE_REQUEST:
		c_result = _serial_if_plme_unpack_testmode_req();
		break;

	case PRIME_PLME_GET_REQUEST:
		c_result = _serial_if_plme_unpack_get_req();
		break;

	case PRIME_PLME_SET_REQUEST:
		c_result = _serial_if_plme_unpack_set_req();
		break;

	default:
		c_result = 0;           /* Primitive is ignored */
	}

	if (c_result == 0) {
		return 1;
	} else {
		return 0;
	}
}


/** @brief	Function to encapsulate PLME Tx Control Process
 *
 *       @param
 *      @return
 *
 * This function will be called using a pointer
 **/
int8_t serial_if_plme_process(void)
{
	int l_len, l_packet_len;
	int8_t c_result = -1;
	x_usi_serial_cmd_params_t x_pkt_usi;

	l_len = prime_PLME_callback(NewPlmeCallBackCmd);
	if (l_len > 0) {
		l_packet_len = _serial_if_plme_pack_plme_callback(NewPlmeCallBackCmd, l_len);
		/* Pack and send the primitive */
		x_pkt_usi.uc_protocol_type = PROTOCOL_PLME_PRIME;
		x_pkt_usi.ptr_buf = &NewPlmeCallBackCmd[0];
		x_pkt_usi.us_len = l_packet_len;
		c_result = usi_send_cmd(&x_pkt_usi);
	}

	return(c_result);
}

/*  @} */

/* / @cond 0 */
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/* / @endcond */
