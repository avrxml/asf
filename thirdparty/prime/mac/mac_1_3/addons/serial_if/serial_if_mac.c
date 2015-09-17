/**
 * \file
 *
 * \brief PRIME Serial Interface for MAC
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

#include "serial_if_mac.h"

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
 * \weakgroup serial_mac_prime_group
 * @{
 */

/*  \name Validity indications */
/* @{ */
#define VALID        1
#define NOTVALID     0
/* @} */

/*  \name Reception variables */
/* @{ */
static uint8_t *puc_rx;             /*  Pointer to received message */
static uint16_t us_rx_len;          /*  Data length of received message */
/* @} */

/*  \name Transmission variables */
/* @{ */
static uint16_t us_tx_len;          /*  Data length in transmitted message */
/* @} */

static MacSapCallBack cmd;
static MacSapCallBack *newMacCallBackCmd = &cmd;
static uint8_t commandMacToPack[PRIME_MACSAP_DATA_SIZE];

#pragma message ("PRIME_MAC_CFG: MAC_SERIALIZATION")

/**
 * \internal
 * \brief This function packs the MAC callback primitive Establish.Indication.
 *
 * \note Copies puc_buf[] into puc_dst[], re-ordering the bytes to adapt to the
 * serial communication.
 * After packing, it requests the sending to the CS layer.
 *
 * \param us_con_handle    Connection handler
 * \param puc_eui48_addr   Pointer to node's MAC address to set the connection
 * \param uc_type          Type of convergence sublayer for the connection
 * \param puc_data         Pointer to the data attached to the connection
 *						   request
 * \param us_data_len      Data length of the data attached to the request
 * \param us_cf_bytes      Number of CFP Bytes to be allocated for the
 * conecction
 * \param uc_error_type    Error type
 *
 * \retval 0 if the request has been made to the CS layer
 * \retval -1 if it was unable to pass the request to the CS layer
 */
static int8_t _serial_if_mac_pack_establish_ind(uint16_t us_con_handle,
		const uint8_t *puc_eui48_addr, uint8_t uc_type,
		const uint8_t *puc_data,
		uint16_t us_data_len, uint16_t us_cf_bytes,
		uint8_t uc_error_type)
{
	uint8_t *puc_tx_buff;
	uint16_t us_idx;

	/* Fill in received data, leaving space for required overhead */
	puc_tx_buff = &commandMacToPack[0];
	*puc_tx_buff++ = PRIME_MACSAP_ESTABLISH_INDICATION;
	*puc_tx_buff++ = (uint8_t)(us_con_handle >> 8);
	*puc_tx_buff++ = (uint8_t)us_con_handle;
	if (puc_eui48_addr == NULL) {
		*puc_tx_buff++ = NOTVALID; /* No EUI48 address */
	} else {
		*puc_tx_buff++ = VALID;
		for (us_idx = 0; us_idx < PRIME_MAC_LENGTH; us_idx++) {
			*puc_tx_buff++ = (char)puc_eui48_addr[us_idx];
		}
	}

	*puc_tx_buff++ = (char)uc_type;
	*puc_tx_buff++ = (uint8_t)(us_data_len >> 8);
	*puc_tx_buff++ = (uint8_t)us_data_len;
	for (us_idx = 0; us_idx < us_data_len; us_idx++) {
		*puc_tx_buff++ = (char)puc_data[us_idx];
	}
	*puc_tx_buff++ = (uint8_t)(us_cf_bytes >> 8);
	*puc_tx_buff++ = (uint8_t)us_cf_bytes;

	*puc_tx_buff++ = uc_error_type;

	/* Calculate MAC message length */
	us_tx_len = (uint16_t)(puc_tx_buff - &commandMacToPack[0]);

	return(0);
}

/**
 * \internal
 * \brief This function packs the MAC callback primitive Establish.Confirm.
 *
 * \note After packing, it requests the sending to the CS layer.
 *
 * \param us_con_handle    Connection handler
 * \param uc_reason        Reason for the connection indication
 * \param puc_eui48_addr   Pointer to node's MAC address to set the connection
 * \param uc_type          Type of convergence sublayer for the connection
 * \param puc_data         Pointer to the data attached to the connection
 *						   request
 * \param us_data_len      Data length of the data attached to the request
 * \param uc_error_type    Error type
 *
 * \retval 0 if the request has been made to the CS layer
 * \retval -1 if it was unable to pass the request to the CS layer
 */
static int8_t _serial_if_mac_pack_establish_conf(uint16_t us_con_handle,
		uint8_t uc_reason, const uint8_t *puc_eui48_addr,
		uint8_t uc_type,
		const uint8_t *puc_data, uint16_t us_data_len,
		uint8_t uc_error_type)
{
	uint8_t *puc_tx_buff;
	uint16_t us_idx;

	/* Fill in received data, leaving space for required overhead */
	puc_tx_buff = &commandMacToPack[0];
	*puc_tx_buff++ = PRIME_MACSAP_ESTABLISH_CONFIRM;
	*puc_tx_buff++ = (char)(us_con_handle >> 8);
	*puc_tx_buff++ = (char)us_con_handle;
	*puc_tx_buff++ = (char)uc_reason;
	if (puc_eui48_addr == NULL) {
		*puc_tx_buff++ = NOTVALID; /* No EUI48 address */
	} else {
		*puc_tx_buff++ = VALID;
		for (us_idx = 0; us_idx < PRIME_MAC_LENGTH; us_idx++) {
			*puc_tx_buff++ = (char)puc_eui48_addr[us_idx];
		}
	}

	*puc_tx_buff++ = (char)uc_type;
	*puc_tx_buff++ = (char)(us_data_len >> 8);
	*puc_tx_buff++ = (char)us_data_len;
	for (us_idx = 0; us_idx < us_data_len; us_idx++) {
		*puc_tx_buff++ = (char)puc_data[us_idx];
	}

	*puc_tx_buff++ = (char)uc_error_type;

	/* Calculate MAC message length */
	us_tx_len = (uint16_t)(puc_tx_buff - &commandMacToPack[0]);

	return(0);
}

/**
 * \internal
 * \brief This function packs the MAC callback primitive Release.Indication.
 *
 * \note After packing, it requests the sending to the CS layer.
 *
 * \param us_con_handle    Connection handler
 * \param uc_reason        Reason for the release
 * \param uc_type          Type of convergence sublayer for the connection
 * \param uc_error_type    Error type
 *
 * \retval 0 if the request has been made to the CS layer
 * \retval -1 if it was unable to pass the request to the CS layer
 */
static int8_t _serial_if_mac_pack_release_ind(uint16_t us_con_handle,
		uint8_t uc_reason, uint8_t uc_type, uint8_t uc_error_type)
{
	uint8_t *puc_tx_buff;

	/* Fill in received data, leaving space for required overhead */
	puc_tx_buff = &commandMacToPack[0];
	*puc_tx_buff++ = PRIME_MACSAP_RELEASE_INDICATION;
	*puc_tx_buff++ = (char)(us_con_handle >> 8);
	*puc_tx_buff++ = (char)us_con_handle;
	*puc_tx_buff++ = (char)uc_reason;
	*puc_tx_buff++ = (char)uc_type;
	*puc_tx_buff++ = (char)uc_error_type;

	/* Calculate MAC message length */
	us_tx_len = (uint16_t)(puc_tx_buff - &commandMacToPack[0]);

	return(0);
}

/**
 * \internal
 * \brief This function packs the MAC callback primitive Release.Confirm.
 *
 * \note After packing, it requests the sending to the CS layer.
 *
 * \param us_con_handle    Connection handler
 * \param uc_result        Result of the release
 * \param uc_type          Type of convergence sublayer for the connection
 * \param uc_error_type    Error type
 *
 * \retval 0 if the request has been made to the CS layer
 * \retval -1 if it was unable to pass the request to the CS layer
 */
static int8_t _serial_if_mac_pack_release_conf(uint16_t us_con_handle,
		uint8_t uc_result, uint8_t uc_type, uint8_t uc_error_type)
{
	uint8_t *puc_tx_buff;

	/* Fill in received data, leaving space for required overhead */
	puc_tx_buff = &commandMacToPack[0];
	*puc_tx_buff++ = PRIME_MACSAP_RELEASE_CONFIRM;
	*puc_tx_buff++ = (char)(us_con_handle >> 8);
	*puc_tx_buff++ = (char)us_con_handle;
	*puc_tx_buff++ = (char)uc_result;
	*puc_tx_buff++ = (char)uc_type;
	*puc_tx_buff++ = (char)uc_error_type;

	/* Calculate MAC message length */
	us_tx_len = (uint16_t)(puc_tx_buff - &commandMacToPack[0]);

	return(0);
}

/**
 * \internal
 * \brief This function packs the MAC callback primitive Join.Confirm.
 *
 * \note After packing, it requests the sending to the CS layer.
 *
 * \param us_con_handle    Connection handler
 * \param uc_result        Result to the join indication
 * \param uc_type          Type of convergence sublayer for the connection
 * \param uc_error_type    Error type
 *
 * \retval 0 if the request has been made to the CS layer
 * \retval -1 if it was unable to pass the request to the CS layer
 */
static int8_t _serial_if_mac_pack_join_conf(uint16_t us_con_handle,
		uint8_t uc_result, uint8_t uc_type, uint8_t uc_error_type)
{
	uint8_t *puc_tx_buff;

	/* Fill in received data, leaving space for required overhead */
	puc_tx_buff = &commandMacToPack[0];
	*puc_tx_buff++ = PRIME_MACSAP_JOIN_CONFIRM;
	*puc_tx_buff++ = (char)(us_con_handle >> 8);
	*puc_tx_buff++ = (char)us_con_handle;
	*puc_tx_buff++ = (char)uc_result;
	*puc_tx_buff++ = (char)uc_type;
	*puc_tx_buff++ = (char)uc_error_type;

	us_tx_len = (uint16_t)(puc_tx_buff - &commandMacToPack[0]);

	return(0);
}

/**
 * \internal
 * \brief This function packs the MAC callback primitive Join.Indication.Base.
 *
 * \note After packing, it requests the sending to the CS layer.
 *
 * \param us_con_handle    Connection handler
 * \param puc_eui48_addr   Pointer to node's MAC address to set the connection
 * \param uc_type          Type of convergence sublayer for the connection
 * \param puc_data         Pointer to the data attached to the connection
 *						   request
 * \param us_data_len      Data length of the data attached to the request
 * \param uc_error_type    Error type
 *
 * \retval 0 if the request has been made to the CS layer
 * \retval -1 if it was unable to pass the request to the CS layer
 */
static int8_t _serial_if_mac_pack_join_ind_base(uint16_t us_con_handle,
		const uint8_t *puc_eui48_addr, uint8_t uc_type,
		const uint8_t *puc_data,
		uint16_t us_data_len, uint8_t uc_error_type)
{
	uint8_t *puc_tx_buff;
	uint16_t us_idx;

	/* Fill in received data, leaving space for required overhead */
	puc_tx_buff = &commandMacToPack[0];
	*puc_tx_buff++ = PRIME_MACSAP_JOIN_INDICATION_BASE;
	*puc_tx_buff++ = (char)(us_con_handle >> 8);
	*puc_tx_buff++ = (char)us_con_handle;
	if (puc_eui48_addr == NULL) {
		*puc_tx_buff++ = NOTVALID; /* No EUI48 address */
	} else {
		*puc_tx_buff++ = VALID;
		for (us_idx = 0; us_idx < PRIME_MAC_LENGTH; us_idx++) {
			*puc_tx_buff++ = (char)puc_eui48_addr[us_idx];
		}
	}

	*puc_tx_buff++ = (char)uc_type;
	*puc_tx_buff++ = (char)(us_data_len >> 8);
	*puc_tx_buff++ = (char)us_data_len;
	for (us_idx = 0; us_idx < us_data_len; us_idx++) {
		*puc_tx_buff++ = (char)puc_data[us_idx];
	}

	*puc_tx_buff++ = (char)uc_error_type;

	/* Calculate MAC message length */
	us_tx_len = (uint16_t)(puc_tx_buff - &commandMacToPack[0]);

	return(0);
}

/**
 * \internal
 * \brief This function packs the MAC callback primitive
 *        Join.Indication.Service.
 *
 * \note After packing, it requests the sending to the CS layer.
 *
 * \param us_con_handle    Connection handler
 * \param puc_data         Pointer to the data attached to the connection
 *						   request
 * \param us_data_len      Data length of the data attached to the request
 * \param uc_type          Type of convergence sublayer for the connection
 * \param uc_error_type    Error type
 *
 * \retval 0 if the request has been made to the CS layer
 * \retval -1 if it was unable to pass the request to the CS layer
 */
static int8_t _serial_if_mac_pack_join_ind_service(uint16_t us_con_handle,
		const uint8_t *puc_data, uint16_t us_data_len, uint8_t uc_type,
		uint8_t uc_error_type)
{
	uint8_t *puc_tx_buff;
	uint16_t us_idx;

	/* Fill in received data, leaving space for required overhead */
	puc_tx_buff = &commandMacToPack[0];
	*puc_tx_buff++ = PRIME_MACSAP_JOIN_INDICATION_SERVICE;
	*puc_tx_buff++ = (char)(us_con_handle >> 8);
	*puc_tx_buff++ = (char)us_con_handle;
	*puc_tx_buff++ = (char)uc_type;
	*puc_tx_buff++ = (char)(us_data_len >> 8);
	*puc_tx_buff++ = (char)us_data_len;
	for (us_idx = 0; us_idx < us_data_len; us_idx++) {
		*puc_tx_buff++ = (char)puc_data[us_idx];
	}
	*puc_tx_buff++ = (char)uc_error_type;

	/* Calculate MAC message length */
	us_tx_len = (uint16_t)(puc_tx_buff - &commandMacToPack[0]);

	return(0);
}

/**
 * \internal
 * \brief This function packs the MAC callback primitive Leave.Confirm.
 *
 * \note After packing, it requests the sending to the CS layer.
 *
 * \param us_con_handle    Connection handler
 * \param uc_result        Result for the leave indication
 * \param uc_type          Type of convergence sublayer for the connection
 * \param uc_error_type    Error type
 *
 * \retval 0 if the request has been made to the CS layer
 * \retval -1 if it was unable to pass the request to the CS layer
 */
static int8_t _serial_if_mac_pack_leave_conf(uint16_t us_con_handle,
		uint8_t uc_result, uint8_t uc_type, uint8_t uc_error_type)
{
	uint8_t *puc_tx_buff;

	/* Fill in received data, leaving space for required overhead */
	puc_tx_buff = &commandMacToPack[0];
	*puc_tx_buff++ = PRIME_MACSAP_LEAVE_CONFIRM;
	*puc_tx_buff++ = (char)(us_con_handle >> 8);
	*puc_tx_buff++ = (char)us_con_handle;
	*puc_tx_buff++ = (char)uc_result;
	*puc_tx_buff++ = (char)uc_type;
	*puc_tx_buff++ = (char)uc_error_type;

	/* Calculate MAC message length */
	us_tx_len = (uint16_t)(puc_tx_buff - &commandMacToPack[0]);

	return(0);
}

/**
 * \internal
 * \brief This function packs the MAC callback primitive Leave.Indication.Base.
 *
 * \note After packing, it requests the sending to the CS layer.
 *
 * \param us_con_handle    Connection handler
 * \param puc_eui48_addr   Pointer to node's MAC address to set the connection
 * \param uc_type          Type of convergence sublayer for the connection
 * \param uc_error_type    Error type
 *
 * \retval 0 if the request has been made to the CS layer
 * \retval -1 if it was unable to pass the request to the CS layer
 */
static int8_t _serial_if_mac_pack_leave_ind_base(uint16_t us_con_handle,
		const uint8_t *puc_eui48_addr, uint8_t uc_type,
		uint8_t uc_error_type)
{
	uint8_t *puc_tx_buff;
	uint16_t us_idx;

	/* Fill in received data, leaving space for required overhead */
	puc_tx_buff = &commandMacToPack[0];
	*puc_tx_buff++ = PRIME_MACSAP_LEAVE_INDICATION_BASE;
	*puc_tx_buff++ = (char)(us_con_handle >> 8);
	*puc_tx_buff++ = (char)us_con_handle;
	if (puc_eui48_addr == NULL) {
		*puc_tx_buff++ = NOTVALID;      /* No EUI48 address */
	} else {
		*puc_tx_buff++ = VALID;
		for (us_idx = 0; us_idx < PRIME_MAC_LENGTH; us_idx++) {
			*puc_tx_buff++ = (char)puc_eui48_addr[us_idx];
		}
	}

	*puc_tx_buff++ = (char)uc_type;
	*puc_tx_buff++ = (char)uc_error_type;

	/* Calculate MAC message length */
	us_tx_len = (uint16_t)(puc_tx_buff - &commandMacToPack[0]);

	return(0);
}

/**
 * \internal
 * \brief This function packs the MAC callback primitive
 * Leave.Indication.Service.
 *
 * \note After packing, it requests the sending to the CS layer.
 *
 * \param us_con_handle    Connection handler
 * \param uc_type          Type of convergence sublayer for the connection
 * \param uc_error_type    Error type
 *
 * \retval 0 if the request has been made to the CS layer
 * \retval -1 if it was unable to pass the request to the CS layer
 */
static int8_t _serial_if_mac_pack_leave_ind_service(uint16_t us_con_handle,
		uint8_t uc_type, uint8_t uc_error_type)
{
	uint8_t *puc_tx_buff;

	/* Fill in received data, leaving space for required overhead */
	puc_tx_buff = &commandMacToPack[0];
	*puc_tx_buff++ = PRIME_MACSAP_LEAVE_INDICATION_SERVICE;
	*puc_tx_buff++ = (char)(us_con_handle >> 8);
	*puc_tx_buff++ = (char)us_con_handle;
	*puc_tx_buff++ = (char)uc_type;
	*puc_tx_buff++ = (char)uc_error_type;

	/* Calculate MAC message length */
	us_tx_len = (uint16_t)(puc_tx_buff - &commandMacToPack[0]);

	return(0);
}

/**
 * \internal
 * \brief This function packs the MAC callback primitive Data.Confirm.
 *
 * \note After packing, it requests the sending to the CS layer.
 *
 * \param us_con_handle    Connection handler
 * \param puc_data         Pointer to the data attached to the connection
 *						   request
 * \param uc_result        Result of the transmission
 * \param uc_type          Type of convergence sublayer for the connection
 * \param uc_error_type    Error type
 *
 * \retval 0 if the request has been made to the CS layer
 * \retval -1 if it was unable to pass the request to the CS layer
 */
static int8_t _serial_if_mac_pack_data_conf(uint16_t us_con_handle,
		const uint8_t *puc_data, uint8_t uc_result, uint8_t uc_type,
		uint8_t uc_error_type)
{
	uint8_t *puc_tx_buff;
	int16_t s_idx;
	uint16_t us_data_len;

	/* Since no data length is provided, send only valid buffer data to */
	/* save space and time */
	us_data_len = MAX_PDU_SIZE;
	for (s_idx = (MAX_PDU_SIZE - 1); s_idx >= 0; s_idx--) {
		if (puc_data[s_idx] != 0) {
			us_data_len = (uint16_t)(s_idx + 1);
			break;
		}
	}

	/* Fill in received data, leaving space for required overhead */
	puc_tx_buff = &commandMacToPack[0];
	*puc_tx_buff++ = PRIME_MACSAP_DATA_CONFIRM;
	*puc_tx_buff++ = (char)(us_con_handle >> 8);
	*puc_tx_buff++ = (char)us_con_handle;
	*puc_tx_buff++ = (char)uc_result;
	*puc_tx_buff++ = (char)uc_type;
	*puc_tx_buff++ = (char)(us_data_len >> 8);
	*puc_tx_buff++ = (char)us_data_len;
	for (s_idx = 0; s_idx < us_data_len; s_idx++) {
		*puc_tx_buff++ = (char)puc_data[s_idx];
	}
	*puc_tx_buff++ = (char)uc_error_type;

	/* Calculate MAC message length */
	us_tx_len = (uint16_t)(puc_tx_buff - &commandMacToPack[0]);

	return(0);
}

/**
 * \internal
 * \brief This function packs the MAC callback primitive Data.Indication.
 *
 * \note After packing, it requests the sending to the CS layer.
 *
 * \param us_con_handle    Connection handler
 * \param puc_data         Pointer to the data attached to the connection
 *						   request
 * \param us_data_len      Data length of the data attached to the request
 * \param uc_type          Type of convergence sublayer for the connection
 * \param uc_error_type    Error type
 *
 * \retval 0 if the request has been made to the CS layer
 * \retval -1 if it was unable to pass the request to the CS layer
 */
static int8_t _serial_if_mac_pack_data_ind(uint16_t us_con_handle,
		const uint8_t *puc_data, uint16_t us_data_len, uint8_t uc_type,
		uint8_t uc_error_type)
{
	uint8_t *puc_tx_buff;
	uint16_t us_idx;

	/* Fill in received data, leaving space for required overhead */
	puc_tx_buff = &commandMacToPack[0];
	*puc_tx_buff++ = PRIME_MACSAP_DATA_INDICATION;
	*puc_tx_buff++ = (char)(us_con_handle >> 8);
	*puc_tx_buff++ = (char)us_con_handle;
	*puc_tx_buff++ = (char)uc_type;
	*puc_tx_buff++ = (char)(us_data_len >> 8);
	*puc_tx_buff++ = (char)us_data_len;
	for (us_idx = 0; us_idx < us_data_len; us_idx++) {
		*puc_tx_buff++ = (char)puc_data[us_idx];
	}
	*puc_tx_buff++ = (char)uc_error_type;

	/* Calculate MAC message length */
	us_tx_len = (uint16_t)(puc_tx_buff - &commandMacToPack[0]);

	return(0);
}

/**
 * \internal
 * \brief This function extracts the MAC SAP Establish.Request.
 *
 * \note This function extracts the MAC SAP Establish.Request from the
 * received packet and stores the data in the circular buffer if possible.
 *
 * \retval 0 primitive stored
 * \retval -1 primitive could not be stored
 */
static int _serial_if_mac_unpack_establish_req(void)
{
	uint8_t *puc_rx_buff;
	uint8_t uc_eui48_addr_valid;
	uint16_t us_idx;

	/* Extract MAC primitive */
	puc_rx_buff = puc_rx;
	uc_eui48_addr_valid = *puc_rx_buff++;
	if (uc_eui48_addr_valid == VALID) {
		for (us_idx = 0; us_idx < PRIME_MAC_LENGTH; us_idx++) {
			newMacCallBackCmd->macAddr[us_idx] = (uint8_t)(*puc_rx_buff++);
		}
	} else {
		for (us_idx = 0; us_idx < PRIME_MAC_LENGTH; us_idx++) {
			newMacCallBackCmd->macAddr[us_idx] = 0;
		}
	}

	newMacCallBackCmd->type = (uint8_t)(*puc_rx_buff++);
	newMacCallBackCmd->bufLength = (((uint16_t)puc_rx_buff[0]) << 8) +
			(uint16_t)puc_rx_buff[1];
	puc_rx_buff += 2;

	if (newMacCallBackCmd->bufLength != 0) {
		for (us_idx = 0; us_idx < newMacCallBackCmd->bufLength; us_idx++) {
			newMacCallBackCmd->buf[us_idx] = (uint8_t)(*puc_rx_buff++);
		}
	}

	newMacCallBackCmd->arq = (uint8_t)(*puc_rx_buff++);
	newMacCallBackCmd->cfpBytes = (((uint16_t)puc_rx_buff[0]) << 8) +
			(uint16_t)puc_rx_buff[1];

	return(prime_MAC_ESTABLISH_request(newMacCallBackCmd->macAddr,
	       newMacCallBackCmd->type, newMacCallBackCmd->buf,
	       newMacCallBackCmd->bufLength, newMacCallBackCmd->arq,
	       newMacCallBackCmd->cfpBytes));
}

/**
 * \internal
 * \brief This function extracts the MAC SAP Establish.Response.
 *
 * \note This function extracts the MAC SAP Establish.Response from the
 * received packet and stores the data in the circular buffer if possible.
 *
 * \retval 0 primitive stored
 * \retval -1 primitive could not be stored
 */
static int _serial_if_mac_unpack_establish_resp(void)
{
	uint8_t *puc_rx_buff;
	uint16_t us_idx;

	/* Extract MAC primitive */
	puc_rx_buff = puc_rx;
	newMacCallBackCmd->handler = (((uint16_t)puc_rx_buff[0]) << 8) +
			(uint16_t)puc_rx_buff[1];
	puc_rx_buff += 2;
	newMacCallBackCmd->answer = (uint8_t)(*puc_rx_buff++);
	newMacCallBackCmd->bufLength = (((uint16_t)puc_rx_buff[0]) << 8) +
			(uint16_t)puc_rx_buff[1];
	puc_rx_buff += 2;
	if (newMacCallBackCmd->bufLength != 0) {
		for (us_idx = 0; us_idx < newMacCallBackCmd->bufLength; us_idx++) {
			newMacCallBackCmd->buf[us_idx] = (uint8_t)(*puc_rx_buff++);
		}
	}

	/* Store the primitive in the circular buffer */
	return(prime_MAC_ESTABLISH_response(newMacCallBackCmd->handler,
	       newMacCallBackCmd->answer, newMacCallBackCmd->buf,
	       newMacCallBackCmd->bufLength));
}

/**
 * \internal
 * \brief This function extracts the MAC SAP Redirect.Response.
 *
 * \note This function extracts the MAC SAP Redirect.Response from the
 * received packet and stores the data in the circular buffer if possible.
 *
 * \retval 0 primitive stored
 * \retval -1 primitive could not be stored
 */
static int _serial_if_mac_unpack_redirect_resp(void)
{
	uint8_t *puc_rx_buff;
	uint8_t uc_eui48_addr_valid;
	uint16_t us_idx;

	/* Extract MAC primitive */
	puc_rx_buff = puc_rx;
	newMacCallBackCmd->handler = (((uint16_t)puc_rx_buff[0]) << 8) +
			(uint16_t)puc_rx_buff[1];
	puc_rx_buff += 2;
	uc_eui48_addr_valid = (uint8_t)(*puc_rx_buff++);
	if (uc_eui48_addr_valid) {
		for (us_idx = 0; us_idx < PRIME_MAC_LENGTH; us_idx++) {
			newMacCallBackCmd->macAddr[us_idx] = (uint8_t)(*puc_rx_buff++);
		}
	} else {
		for (us_idx = 0; us_idx < PRIME_MAC_LENGTH; us_idx++) {
			newMacCallBackCmd->macAddr[us_idx] = 0;
		}
	}

	newMacCallBackCmd->bufLength = (((uint16_t)puc_rx_buff[0]) << 8) +
			(uint16_t)puc_rx_buff[1];
	puc_rx_buff += 2;
	if (newMacCallBackCmd->bufLength != 0) {
		for (us_idx = 0; us_idx < newMacCallBackCmd->bufLength; us_idx++) {
			newMacCallBackCmd->buf[us_idx] = (uint8_t)(*puc_rx_buff++);
		}
	}

	/* Store the primitive in the circular buffer */
	return(prime_MAC_REDIRECT_response(newMacCallBackCmd->handler,
	       newMacCallBackCmd->macAddr, newMacCallBackCmd->buf,
	       newMacCallBackCmd->bufLength));
}

/**
 * \internal
 * \brief This function extracts the MAC SAP Release.Request.
 *
 * \note This function extracts the MAC SAP Release.Request from the
 * received packet and stores the data in the circular buffer if possible.
 *
 * \retval 0 primitive stored
 * \retval -1 primitive could not be stored
 */
static int _serial_if_mac_unpack_release_req(void)
{
	uint8_t *puc_rx_buff;

	/* Extract MAC primitive */
	puc_rx_buff = puc_rx;
	newMacCallBackCmd->handler = (((uint16_t)puc_rx_buff[0]) << 8) +
			(uint16_t)puc_rx_buff[1];

	/* Store the primitive in the circular buffer */
	return(prime_MAC_RELEASE_request(newMacCallBackCmd->handler));
}

/**
 * \internal
 * \brief This function extracts the MAC SAP Release.Response.
 *
 * \note This function extracts the MAC SAP Release.Response from the
 * received packet and stores the data in the circular buffer if possible.
 *
 * \retval 0 primitive stored
 * \retval -1 primitive could not be stored
 */
static int _serial_if_mac_unpack_release_resp(void)
{
	uint8_t *puc_rx_buff;

	/* Extract MAC primitive */
	puc_rx_buff = puc_rx;
	newMacCallBackCmd->handler = (((uint16_t)puc_rx_buff[0]) << 8) +
			(uint16_t)puc_rx_buff[1];
	puc_rx_buff += 2;
	newMacCallBackCmd->answer = (uint8_t)(*puc_rx_buff++);

	/* Store the primitive in the circular buffer */
	return(prime_MAC_RELEASE_response(newMacCallBackCmd->handler,
	       newMacCallBackCmd->answer));
}

/**
 * \internal
 * \brief This function extracts the MAC SAP Join.Request.Base.
 *
 * \note This function extracts the MAC SAP Join.Request.Base from the
 * received packet and stores the data in the circular buffer if possible.
 *
 * \retval 0 primitive stored
 * \retval -1 primitive could not be stored
 */
static int _serial_if_mac_unpack_join_req_base(void)
{
	uint8_t *puc_rx_buff;
	uint16_t us_idx;
	uint8_t uc_eui48_addr_valid;

	/* Extract MAC primitive */
	puc_rx_buff = puc_rx;
	newMacCallBackCmd->broadcast = (uint8_t)(*puc_rx_buff++);
	newMacCallBackCmd->handler = (((uint16_t)puc_rx_buff[0]) << 8) +
			(uint16_t)puc_rx_buff[1];
	puc_rx_buff += 2;

	uc_eui48_addr_valid = (uint8_t)(*puc_rx_buff++);
	if (uc_eui48_addr_valid) {
		for (us_idx = 0; us_idx < PRIME_MAC_LENGTH; us_idx++) {
			newMacCallBackCmd->macAddr[us_idx] = (uint8_t)(*puc_rx_buff++);
		}
	} else {
		for (us_idx = 0; us_idx < PRIME_MAC_LENGTH; us_idx++) {
			newMacCallBackCmd->macAddr[us_idx] = 0;
		}
	}

	newMacCallBackCmd->type = (uint8_t)(*puc_rx_buff++);
	newMacCallBackCmd->bufLength = (((uint16_t)puc_rx_buff[0]) << 8) +
			(uint16_t)puc_rx_buff[1];
	puc_rx_buff += 2;
	if (newMacCallBackCmd->bufLength != 0) {
		for (us_idx = 0; us_idx < newMacCallBackCmd->bufLength; us_idx++) {
			newMacCallBackCmd->buf[us_idx] = (uint8_t)(*puc_rx_buff++);
		}
	}

	/* Store the primitive in the circular buffer */
	return(prime_MAC_JOIN_request_base(newMacCallBackCmd->broadcast,
	       newMacCallBackCmd->handler, newMacCallBackCmd->macAddr,
	       newMacCallBackCmd->type, newMacCallBackCmd->buf,
	       newMacCallBackCmd->bufLength));
}

/**
 * \internal
 * \brief This function extracts the MAC SAP Join.Request.Service.
 *
 * \note This function extracts the MAC SAP Join.Request.Service from the
 * received packet and stores the data in the circular buffer if possible.
 *
 * \retval 0 primitive stored
 * \retval -1 primitive could not be stored
 */
static int _serial_if_mac_unpack_join_req_service(void)
{
	uint8_t *puc_rx_buff;
	uint16_t us_idx;

	/* Extract MAC primitive */
	puc_rx_buff = puc_rx;
	newMacCallBackCmd->broadcast = (uint8_t)(*puc_rx_buff++);
	newMacCallBackCmd->type = (uint8_t)(*puc_rx_buff++);
	newMacCallBackCmd->bufLength = (((uint16_t)puc_rx_buff[0]) << 8) +
			(uint16_t)puc_rx_buff[1];
	puc_rx_buff += 2;
	if (newMacCallBackCmd->bufLength != 0) {
		for (us_idx = 0; us_idx < newMacCallBackCmd->bufLength; us_idx++) {
			newMacCallBackCmd->buf[us_idx] = (uint8_t)(*puc_rx_buff++);
		}
	}

	/* Store the primitive in the circular buffer */
	return(prime_MAC_JOIN_request_service(newMacCallBackCmd->broadcast,
	       newMacCallBackCmd->type, newMacCallBackCmd->buf,
	       newMacCallBackCmd->bufLength));
}

/**
 * \internal
 * \brief This function extracts the MAC SAP Join.Response.Base.
 *
 * \note This function extracts the MAC SAP Join.Response.Base from the
 * received packet and stores the data in the circular buffer if possible.
 *
 * \retval 0 primitive stored
 * \retval -1 primitive could not be stored
 */
static int _serial_if_mac_unpack_join_resp_base(void)
{
	uint8_t *puc_rx_buff;
	uint16_t us_idx;
	uint8_t uc_eui48_addr_valid;

	/* Extract MAC primitive */
	puc_rx_buff = puc_rx;
	newMacCallBackCmd->handler = (((uint16_t)puc_rx_buff[0]) << 8) +
			(uint16_t)puc_rx_buff[1];
	puc_rx_buff += 2;
	uc_eui48_addr_valid = (uint8_t)(*puc_rx_buff++);
	if (uc_eui48_addr_valid) {
		for (us_idx = 0; us_idx < PRIME_MAC_LENGTH; us_idx++) {
			newMacCallBackCmd->macAddr[us_idx] = (uint8_t)(*puc_rx_buff++);
		}
	} else {
		for (us_idx = 0; us_idx < PRIME_MAC_LENGTH; us_idx++) {
			newMacCallBackCmd->macAddr[us_idx] = 0;
		}
	}

	newMacCallBackCmd->answer = (uint8_t)(*puc_rx_buff++);

	/* Store the primitive in the circular buffer */
	return(prime_MAC_JOIN_response_base(newMacCallBackCmd->handler,
	       newMacCallBackCmd->macAddr, newMacCallBackCmd->answer));
}

/**
 * \internal
 * \brief This function extracts the MAC SAP Join.Response.Service.
 *
 * \note This function extracts the MAC SAP Join.Response.Service from the
 * received packet and stores the data in the circular buffer if possible.
 *
 * \retval 0 primitive stored
 * \retval -1 primitive could not be stored
 */
static int _serial_if_mac_unpack_join_resp_service(void)
{
	uint8_t *puc_rx_buff;

	/* Extract MAC primitive */
	puc_rx_buff = puc_rx;
	newMacCallBackCmd->handler = (((uint16_t)puc_rx_buff[0]) << 8) +
			(uint16_t)puc_rx_buff[1];
	puc_rx_buff += 2;
	newMacCallBackCmd->answer = (uint8_t)(*puc_rx_buff++);

	/* Store the primitive in the circular buffer */
	return(prime_MAC_JOIN_response_service(newMacCallBackCmd->handler,
	       newMacCallBackCmd->answer));
}

/**
 * \internal
 * \brief This function extracts the MAC SAP Leave.Request.Base.
 *
 * \note This function extracts the MAC SAP Leave.Request.Base from the
 *  received packet and stores the data in the circular buffer if possible.
 *
 * \retval 0 primitive stored
 * \retval -1 primitive could not be stored
 */
static int _serial_if_mac_unpack_leave_req_base(void)
{
	uint8_t *puc_rx_buff;
	uint16_t us_idx;
	uint8_t uc_eui48_addr_valid;

	/* Extract MAC primitive */
	puc_rx_buff = puc_rx;
	newMacCallBackCmd->handler = (((uint16_t)puc_rx_buff[0]) << 8) +
			(uint16_t)puc_rx_buff[1];
	puc_rx_buff += 2;
	uc_eui48_addr_valid = (uint8_t)(*puc_rx_buff++);
	if (uc_eui48_addr_valid) {
		for (us_idx = 0; us_idx < PRIME_MAC_LENGTH; us_idx++) {
			newMacCallBackCmd->macAddr[us_idx] = (uint8_t)(*puc_rx_buff++);
		}
	} else {
		for (us_idx = 0; us_idx < PRIME_MAC_LENGTH; us_idx++) {
			newMacCallBackCmd->macAddr[us_idx] = 0;
		}
	}

	/* Store the primitive in the circular buffer */
	return(prime_MAC_LEAVE_request_base(newMacCallBackCmd->handler,
	       newMacCallBackCmd->macAddr));
}

/**
 * \internal
 * \brief This function extracts the MAC SAP Leave.Request.Service.
 *
 * \note This function extracts the MAC SAP Leave.Request.Service from the
 * received packet and stores the data in the circular buffer if possible.
 *
 * \retval 0 primitive stored
 * \retval -1 primitive could not be stored
 */
static int _serial_if_mac_unpack_leave_req_service(void)
{
	uint8_t *puc_rx_buff;

	/* Extract MAC primitive */
	puc_rx_buff = puc_rx;
	newMacCallBackCmd->handler = (((uint16_t)puc_rx_buff[0]) << 8) +
			(uint16_t)puc_rx_buff[1];

	/* Store the primitive in the circular buffer */
	return(prime_MAC_LEAVE_request_service(newMacCallBackCmd->handler));
}

/**
 * \internal
 * \brief This function extracts the MAC SAP Data.Request.
 *
 * \note This function extracts the MAC SAP Data.Request from the
 * received packet and stores the data in the circular buffer if possible.
 *
 * \retval 0 primitive stored
 * \retval -1 primitive could not be stored
 */
static int _serial_if_mac_unpack_data_req(void)
{
	uint8_t *puc_rx_buff;
	uint16_t us_idx;

	/* Extract MAC primitive */
	puc_rx_buff = puc_rx;
	newMacCallBackCmd->handler = (((uint16_t)puc_rx_buff[0]) << 8) +
			(uint16_t)puc_rx_buff[1];
	puc_rx_buff += 2;
	newMacCallBackCmd->bufLength = (((uint16_t)puc_rx_buff[0]) << 8) +
			(uint16_t)puc_rx_buff[1];
	puc_rx_buff += 2;
	if (newMacCallBackCmd->bufLength != 0) {
		for (us_idx = 0; us_idx < newMacCallBackCmd->bufLength; us_idx++) {
			newMacCallBackCmd->buf[us_idx] = (uint8_t)(*puc_rx_buff++);
		}
	}

	newMacCallBackCmd->priority = (uint8_t)(*puc_rx_buff++);

	/* Store the primitive in the circular buffer */
	return(prime_MAC_DATA_request(newMacCallBackCmd->handler,
	       newMacCallBackCmd->buf, newMacCallBackCmd->bufLength,
	       newMacCallBackCmd->priority));
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
uint8_t serial_if_mac_api_parser(uint8_t *puc_rx_msg, uint16_t us_len)
{
	uint8_t uc_cmd;
	int8_t c_result = -1;

	/* Extract command */
	uc_cmd = CMD_PRIME_PROTOCOL(puc_rx_msg[0]);
	us_rx_len = us_len;

	/* Pointer to message data */
	puc_rx = (uint8_t *)&puc_rx_msg[1];

	/* Extract and store MAC data depending on the primitive */
	switch (uc_cmd) {
	case PRIME_MACSAP_ESTABLISH_REQUEST:
		c_result = _serial_if_mac_unpack_establish_req();
		break;

	case PRIME_MACSAP_ESTABLISH_RESPONSE:
		c_result = _serial_if_mac_unpack_establish_resp();
		break;

	case PRIME_MACSAP_REDIRECT_RESPONSE:
		c_result = _serial_if_mac_unpack_redirect_resp();
		break;

	case PRIME_MACSAP_RELEASE_REQUEST:
		c_result =  _serial_if_mac_unpack_release_req();
		break;

	case PRIME_MACSAP_RELEASE_RESPONSE:
		c_result = _serial_if_mac_unpack_release_resp();
		break;

	case PRIME_MACSAP_JOIN_REQUEST_BASE:
		c_result = _serial_if_mac_unpack_join_req_base();
		break;

	case PRIME_MACSAP_JOIN_REQUEST_SERVICE:
		c_result = _serial_if_mac_unpack_join_req_service();
		break;

	case PRIME_MACSAP_JOIN_RESPONSE_BASE:
		c_result = _serial_if_mac_unpack_join_resp_base();
		break;

	case PRIME_MACSAP_JOIN_RESPONSE_SERVICE:
		c_result = _serial_if_mac_unpack_join_resp_service();
		break;

	case PRIME_MACSAP_LEAVE_REQUEST_BASE:
		c_result = _serial_if_mac_unpack_leave_req_base();
		break;

	case PRIME_MACSAP_LEAVE_REQUEST_SERVICE:
		c_result = _serial_if_mac_unpack_leave_req_service();
		break;

	case PRIME_MACSAP_DATA_REQUEST:
		c_result = _serial_if_mac_unpack_data_req();
		break;

	default:
		c_result = 0;           /* Primitive is ignored */
	}

	if (c_result == 0) {
		return true;
	} else {
		return false;
	}
}

/**
 * \brief Function to encapsulate MAC Tx Control Process.
 *
 * \note This function will be called using a pointer.
 *
 * \retval 0 if the request has been made to the CS layer
 * \retval -1 if it was unable to pass the request to the CS layer
 */
int8_t serial_if_mac_process(void)
{
	int8_t c_result = -1;
	x_usi_serial_cmd_params_t x_pkt_usi;

	if (newMacCallBackCmd != NULL) {
		/* Extracts everything Non Reserved */
		if (prime_MAC_callback(newMacCallBackCmd,
				GENERIC_CALLBACK_HANDLER,
				GENERIC_CALLBACK_TYPE)) {
			/* Extract the data before packing and sending the
			 * primitive */
			switch (newMacCallBackCmd->command) {
			case PRIME_MACSAP_ESTABLISH_INDICATION:
				c_result = _serial_if_mac_pack_establish_ind(newMacCallBackCmd->handler,
						newMacCallBackCmd->macAddr,
						newMacCallBackCmd->type,
						newMacCallBackCmd->buf,
						newMacCallBackCmd->bufLength,
						newMacCallBackCmd->cfpBytes,
						newMacCallBackCmd->errorType);
				break;

			case PRIME_MACSAP_ESTABLISH_CONFIRM:
				c_result = _serial_if_mac_pack_establish_conf(newMacCallBackCmd->handler,
						newMacCallBackCmd->answer,
						newMacCallBackCmd->macAddr,
						newMacCallBackCmd->type,
						newMacCallBackCmd->buf,
						newMacCallBackCmd->bufLength,
						newMacCallBackCmd->errorType);
				break;

			case PRIME_MACSAP_RELEASE_INDICATION:
				c_result = _serial_if_mac_pack_release_ind(newMacCallBackCmd->handler,
						newMacCallBackCmd->answer,
						newMacCallBackCmd->type,
						newMacCallBackCmd->errorType);
				break;

			case PRIME_MACSAP_RELEASE_CONFIRM:
				c_result = _serial_if_mac_pack_release_conf(newMacCallBackCmd->handler,
						newMacCallBackCmd->answer,
						newMacCallBackCmd->type,
						newMacCallBackCmd->errorType);
				break;

			case PRIME_MACSAP_JOIN_CONFIRM:
				c_result = _serial_if_mac_pack_join_conf(newMacCallBackCmd->handler,
						newMacCallBackCmd->answer,
						newMacCallBackCmd->type,
						newMacCallBackCmd->errorType);
				break;

			case PRIME_MACSAP_JOIN_INDICATION_BASE:
				c_result = _serial_if_mac_pack_join_ind_base(newMacCallBackCmd->handler,
						newMacCallBackCmd->macAddr,
						newMacCallBackCmd->type,
						newMacCallBackCmd->buf,
						newMacCallBackCmd->bufLength,
						newMacCallBackCmd->errorType);
				break;

			case PRIME_MACSAP_JOIN_INDICATION_SERVICE:
				c_result = _serial_if_mac_pack_join_ind_service(newMacCallBackCmd->handler,
						newMacCallBackCmd->buf,
						newMacCallBackCmd->bufLength,
						newMacCallBackCmd->type,
						newMacCallBackCmd->errorType);
				break;

			case PRIME_MACSAP_LEAVE_CONFIRM:
				c_result = _serial_if_mac_pack_leave_conf(newMacCallBackCmd->handler,
						newMacCallBackCmd->answer,
						newMacCallBackCmd->type,
						newMacCallBackCmd->errorType);
				break;

			case PRIME_MACSAP_LEAVE_INDICATION_BASE:
				c_result = _serial_if_mac_pack_leave_ind_base(newMacCallBackCmd->handler,
						newMacCallBackCmd->macAddr,
						newMacCallBackCmd->type,
						newMacCallBackCmd->errorType);
				break;

			case PRIME_MACSAP_LEAVE_INDICATION_SERVICE:
				c_result = _serial_if_mac_pack_leave_ind_service(newMacCallBackCmd->handler,
						newMacCallBackCmd->type,
						newMacCallBackCmd->errorType);
				break;

			case PRIME_MACSAP_DATA_CONFIRM:
				c_result = _serial_if_mac_pack_data_conf(newMacCallBackCmd->handler,
						newMacCallBackCmd->buf,
						newMacCallBackCmd->answer,
						newMacCallBackCmd->type,
						newMacCallBackCmd->errorType);
				break;

			case PRIME_MACSAP_DATA_INDICATION:
				c_result = _serial_if_mac_pack_data_ind(newMacCallBackCmd->handler,
						newMacCallBackCmd->buf,
						newMacCallBackCmd->bufLength,
						newMacCallBackCmd->type,
						newMacCallBackCmd->errorType);
				break;

			default:
				c_result = -1;
				break;
			}

			if (c_result == 0) {
				/* Pack and send the primitive */
				x_pkt_usi.uc_protocol_type = PROTOCOL_MAC_PRIME;
				x_pkt_usi.ptr_buf = (uint8_t *)&commandMacToPack[0];
				x_pkt_usi.us_len = us_tx_len;
				c_result = usi_send_cmd(&x_pkt_usi);
			}
		}
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
