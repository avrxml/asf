/**
 * \file
 *
 * \brief PRIME Serial Interface for MLME
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

#include "serial_if_mlme.h"

/* From module: PRIME MAC Layer */
#include <mngl.h>
#include <prime.h>

/* From module: PLC Universal Serial Interface */
#include <usi.h>

/* / @cond 0 */
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/* / @endcond */

/**
 * \weakgroup serial_mlme_prime_group
 * @{
 */

/*  \name Address indication for the MLME primitive Register.Request */
/* @{ */
enum ADDRESS_IND {
	NOADDRESS,
	EUI48,
	SNA_REQ
};
/* @} */

/*  \name Reception variables */
/* @{ */
static uint8_t *puc_rx;             /*  Pointer to received message */
/* @} */

static uint8_t NewMlmeCallBackCmd[MLME_DATA_SIZE];
static uint8_t commandMlmeToPack[MLME_DATA_SIZE];

#pragma message ("PRIME_MAC_CFG: MLME_SERIALIZATION")

/**
 * \internal
 * \brief This function extracts the MLME SAP Register.Request.
 *
 * \note This function extracts the MLME SAP Register.Request from the
 * received packet and stores the data in the circular buffer if possible.
 *
 * \retval 0 primitive stored
 * \retval -1 primitive could not be stored
 */
static int _serial_if_mlme_unpack_register_req(void)
{
	uint8_t *puc_rx_buff;
	uint8_t uc_sna[PRIME_MAC_LENGTH];
	uint8_t uc_mac[PRIME_MAC_LENGTH];
	uint8_t uc_addr_ind;
	uint16_t us_idx;

	/* Extract MLME primitive */
	puc_rx_buff = puc_rx;
	uc_addr_ind = (uint8_t)(*puc_rx_buff++);
	switch (uc_addr_ind) {
	case EUI48:
		for (us_idx = 0; us_idx < PRIME_MAC_LENGTH; us_idx++) {
			uc_mac[us_idx] = (uint8_t)(*puc_rx_buff++);
		}
		break;

	case SNA_REQ:
		for (us_idx = 0; us_idx < PRIME_MAC_LENGTH; us_idx++) {
			uc_sna[us_idx] = (uint8_t)(*puc_rx_buff++);
		}
		break;

	case NOADDRESS:
	default:
		for (us_idx = 0; us_idx < PRIME_MAC_LENGTH; us_idx++) {
			uc_mac[us_idx] = 0;
			uc_sna[us_idx] = 0;
		}
	}

	/* Store the primitive in the circular buffer */
	return(prime_MLME_REGISTER_request(uc_sna, uc_mac));
}

/**
 * \internal
 * \brief This function extracts the MLME SAP Unregister.Request.
 *
 * \note This function extracts the MLME SAP Unregister.Request from the
 * received packet and stores the data in the circular buffer if possible.
 *
 * \retval 0 primitive stored
 * \retval -1 primitive could not be stored
 */
static int _serial_if_mlme_unpack_unregister_req_base(void)
{
	uint8_t *puc_rx_buff;
	uint8_t uc_mac[PRIME_MAC_LENGTH];

	/* Extract MLME primitive */
	puc_rx_buff = puc_rx;
	memcpy(uc_mac, (uint8_t *)puc_rx_buff, PRIME_MAC_LENGTH);

	/* Store the primitive in the circular buffer */
	return(prime_MLME_UNREGISTER_request_base(uc_mac));
}

/**
 * \internal
 * \brief This function extracts the MLME SAP Promote.Request.
 *
 * \note This function extracts the MLME SAP Promote.Request from the
 * received packet and stores the data in the circular buffer if possible.
 *
 * \retval 0 primitive stored
 * \retval -1 primitive could not be stored
 */
static int _serial_if_mlme_unpack_promote_req_base(void)
{
	uint8_t *puc_rx_buff;
	uint8_t uc_mac[PRIME_MAC_LENGTH];

	/* Extract MLME primitive */
	puc_rx_buff = puc_rx;
	memcpy(uc_mac, (uint8_t *)puc_rx_buff, PRIME_MAC_LENGTH);

	/* Store the primitive in the circular buffer */
	return(prime_MLME_PROMOTE_request_base(uc_mac));
}

/**
 * \internal
 * \brief This function extracts the MLME SAP Demote.Request.
 *
 * \note This function extracts the MLME SAP Demote.Request from the
 * received packet and stores the data in the circular buffer if possible.
 *
 * \retval 0 primitive stored
 * \retval -1 primitive could not be stored
 */
static int _serial_if_mlme_unpack_demote_req_base(void)
{
	uint8_t *puc_rx_buff;
	uint8_t uc_mac[PRIME_MAC_LENGTH];

	/* Extract MLME primitive */
	puc_rx_buff = puc_rx;
	memcpy(uc_mac, (uint8_t *)puc_rx_buff, PRIME_MAC_LENGTH);

	/* Store the primitive in the circular buffer */
	return(prime_MLME_DEMOTE_request_base(uc_mac));
}

/**
 * \internal
 * \brief This function extracts the MLME SAP Get.Request.
 *
 * \note This function extracts the MLME SAP Get.Request from the
 * received packet and stores the data in the circular buffer if possible.
 *
 * \retval 0 primitive stored
 * \retval -1 primitive could not be stored
 */
static int _serial_if_mlme_unpack_get_req(void)
{
	uint8_t *puc_rx_buff;
	uint16_t us_pib_attr;

	/* Extract MLME primitive */
	puc_rx_buff = puc_rx;
	us_pib_attr = (((uint16_t)puc_rx_buff[0]) << 8) + (uint16_t)puc_rx_buff[1];

	/* Store the primitive in the circular buffer */
	return(prime_MLME_GET_request(us_pib_attr));
}

/**
 * \internal
 * \brief This function extracts the MLME SAP List.Get.Request.
 *
 * \note This function extracts the MLME SAP List.Get.Request from the
 * received packet and stores the data in the circular buffer if possible.
 *
 * \retval 0 primitive stored
 * \retval -1 primitive could not be stored
 */
static int _serial_if_mlme_unpack_list_get_req(void)
{
	uint8_t *puc_rx_buff;
	uint16_t us_pib_attr;

	/* Extract MLME primitive */
	puc_rx_buff = puc_rx;
	us_pib_attr = (((uint16_t)puc_rx_buff[0]) << 8) + (uint16_t)puc_rx_buff[1];

	/* Store the primitive in the circular buffer */
	return(prime_MLME_LIST_GET_request(us_pib_attr));
}

/**
 * \internal
 * \brief This function extracts the MLME SAP Set.Request.
 *
 * \note This function extracts the MLME SAP Set.Request from the
 * received packet and stores the data in the circular buffer if possible.
 *
 * \retval 0 primitive stored
 * \retval -1 primitive could not be stored
 */
static int _serial_if_mlme_unpack_set_req(void)
{
	uint8_t *puc_rx_buff;
	uint16_t us_pib_attr;
	uint32_t ul_pib_attr_val;
	uint8_t uc_len;

	/* Extract MLME primitive */
	puc_rx_buff = puc_rx;
	us_pib_attr = (((uint16_t)puc_rx_buff[0]) << 8) + (uint16_t)puc_rx_buff[1];
	puc_rx_buff += 2;
	uc_len = 0;

	switch (us_pib_attr) {
	case PIB_MAC_EUI48:
	case PIB_MAC_BOARD_SERIAL_NUMBER:
		uc_len = PRIME_MAC_LENGTH;
		break;

	case PIB_CERTIFICATION_SEND_MSG:
		uc_len = 5;
		break;

	case PIB_MAC_ACTION_PROMOTE:
	case PIB_MAC_ACTION_DEMOTE:
	case PIB_MAC_ACTION_PRM:
	case PIB_MAC_ACTION_UNREGISTER_BN:
		uc_len = 6;
		break;

	case PIB_MAC_ACTION_MGMT_CON:
	case PIB_MAC_ACTION_MGMT_MUL:
		uc_len = 7;
		break;

	case PIB_MAC_ACTION_REJECT:
	case PIB_MAC_ACTION_CONN_CLOSE_BN:
	case PIB_MAC_ACTION_SEGMENTED_432:
		uc_len = 8;
		break;

	case PIB_MAC_ACTION_BROADCAST_BURST:
		uc_len = 9;
		break;

	case PIB_MAC_ACTION_APPEMU_DATA_BURST:
		uc_len = 12;
		break;

	default:
		uc_len = 0;
		break;
	}

	if (uc_len) {
		/* Store the primitive in the circular buffer */
		return(prime_MLME_SET_BUF_request(us_pib_attr,
				(uint8_t *)puc_rx_buff, uc_len));
	} else {
		ul_pib_attr_val = (((uint32_t)puc_rx_buff[0]) << 24) +
				(((uint32_t)puc_rx_buff[1]) << 16) +
				(((uint32_t)puc_rx_buff[2]) << 8) +
				(uint32_t)puc_rx_buff[3];
		/* Store the primitive in the circular buffer */
		return(prime_MLME_SET_request(us_pib_attr, ul_pib_attr_val));
	}
}

/**
 * \internal
 * \brief Packets a MLME Callback to avoid misalignments
 *
 * \param puc_buf_des     Pointer to the destination buffer
 * \param px_buf_org      Pointer to the source buffer
 *
 * \retval length of the buffer packet
 */
static int _serial_if_mlme_pack_reg_devices(uint8_t *puc_buf_des,
		uint8_t *px_buf_org)
{
	uint8_t *puc_packet;

	puc_packet = puc_buf_des;

	memcpy(puc_packet, px_buf_org, PRIME_MAC_LENGTH + 4); /* mac */
	puc_packet += PRIME_MAC_LENGTH + 4;
	*puc_packet++ = px_buf_org[10]; /* lnid */
	*puc_packet++ = px_buf_org[11]; /* lnid */
	*puc_packet++ = px_buf_org[12]; /* lsid */
	*puc_packet++ = px_buf_org[13]; /* lsid */
	*puc_packet++ = px_buf_org[14]; /* sid */
	*puc_packet++ = px_buf_org[15]; /* sid */

	return(puc_packet - puc_buf_des);
}

/**
 * \internal
 * \brief Packets a MLME Callback to avoid misalignments
 *
 * \param puc_buf_des     Pointer to the destination buffer
 * \param px_buf_org      Pointer to the source buffer
 *
 * \retval length of the buffer packet
 */
static int _serial_if_mlme_pack_active_conn(uint8_t *puc_buf_des,
		uint8_t *px_buf_org)
{
	uint8_t *puc_packet;

	puc_packet = puc_buf_des;

	memcpy(puc_packet, px_buf_org, PRIME_MAC_LENGTH); /* mac */
	puc_packet += PRIME_MAC_LENGTH;
	*puc_packet++ = px_buf_org[6]; /* sid */
	*puc_packet++ = px_buf_org[7]; /* sid */
	*puc_packet++ = px_buf_org[8]; /* lnid */
	*puc_packet++ = px_buf_org[9]; /* lnid */
	*puc_packet++ = px_buf_org[10]; /* lcid */

	return(puc_packet - puc_buf_des);
}

/**
 * \internal
 * \brief Packets a MLME Callback to avoid misalignments
 *
 * \param puc_buf_des     Pointer to the destination buffer
 * \param px_buf_org      Pointer to the source buffer
 *
 * \retval length of the buffer packet
 */
static int _serial_if_mlme_pack_mcast_entries(uint8_t *puc_buf_des,
		uint8_t *px_buf_org)
{
	uint8_t *puc_packet;

	puc_packet = puc_buf_des;

	*puc_packet++ = px_buf_org[0]; /* member */
	*puc_packet++ = px_buf_org[1]; /* member */
	*puc_packet++ = px_buf_org[2]; /* lcid */

	return(puc_packet - puc_buf_des);
}

/**
 * \internal
 * \brief Packets a MLME Callback to avoid misaligments
 *
 * \param puc_buf_des     Pointer to the destination buffer
 * \param px_buf_org      Pointer to the source buffer
 *
 * \retval length of the buffer packet
 */
static int _serial_if_mlme_pack_switch_table(uint8_t *puc_buf_des,
		uint8_t *px_buf_org)
{
	uint8_t *puc_packet;

	puc_packet = puc_buf_des;

	*puc_packet++ = px_buf_org[0]; /* sid */
	*puc_packet++ = px_buf_org[1]; /* sid */

	return(puc_packet - puc_buf_des);
}

/**
 * \internal
 * \brief Packets a MLME Callback to avoid misalignments
 *
 * \param puc_buf_des     Pointer to the destination buffer
 * \param px_buf_org      Pointer to the source buffer
 *
 * \retval length of the buffer packet
 */
static int _serial_if_mlme_pack_direct_conn(uint8_t *puc_buf_des,
		uint8_t *px_buf_org)
{
	uint8_t *puc_packet;

	puc_packet = puc_buf_des;

	memcpy(puc_packet, px_buf_org, PRIME_MAC_LENGTH);  /* srcmac */
	puc_packet += PRIME_MAC_LENGTH;
	*puc_packet++ = px_buf_org[6]; /* srcsid */
	*puc_packet++ = px_buf_org[7]; /* srcsid */
	*puc_packet++ = px_buf_org[8]; /* srclnid */
	*puc_packet++ = px_buf_org[9]; /* srclnid */
	*puc_packet++ = px_buf_org[10]; /* srclcid */
	memcpy(puc_packet, &px_buf_org[11], PRIME_MAC_LENGTH);  /* dstmac */
	puc_packet += PRIME_MAC_LENGTH;
	*puc_packet++ = px_buf_org[17]; /* dstsid */
	*puc_packet++ = px_buf_org[18]; /* dstsid */
	*puc_packet++ = px_buf_org[19]; /* dstlnid */
	*puc_packet++ = px_buf_org[20]; /* dstlnid */
	*puc_packet++ = px_buf_org[21]; /* dstlcid */
	memcpy(puc_packet, &px_buf_org[22], PRIME_MAC_LENGTH);  /* dmac */
	puc_packet += PRIME_MAC_LENGTH;
	*puc_packet++ = px_buf_org[28]; /* dsid */
	*puc_packet++ = px_buf_org[29]; /* dsid */

	return(puc_packet - puc_buf_des);
}

/**
 * \internal
 * \brief Packets a MLME Callback to avoid misalignments
 *
 * \param puc_buf_des     Pointer to the destination buffer
 * \param px_buf_org      Pointer to the source buffer
 *
 * \retval length of the buffer packet
 */
static int _serial_if_mlme_pack_direct_switch_table(uint8_t *puc_buf_des,
		uint8_t *px_buf_org)
{
	uint8_t *puc_packet;

	puc_packet = puc_buf_des;

	memcpy(puc_packet, px_buf_org, PRIME_MAC_LENGTH); /* dsmac */
	puc_packet += PRIME_MAC_LENGTH;
	*puc_packet++ = px_buf_org[6]; /* srcsid */
	*puc_packet++ = px_buf_org[7]; /* srcsid */
	*puc_packet++ = px_buf_org[8]; /* srclnid */
	*puc_packet++ = px_buf_org[9]; /* srclnid */
	*puc_packet++ = px_buf_org[10]; /* srclcid */
	*puc_packet++ = px_buf_org[11]; /* dstsid */
	*puc_packet++ = px_buf_org[12]; /* dstsid */
	*puc_packet++ = px_buf_org[13]; /* dstlnid */
	*puc_packet++ = px_buf_org[14]; /* dstlnid */
	*puc_packet++ = px_buf_org[15]; /* dstlcid */

	return(puc_packet - puc_buf_des);
}

/**
 * \internal
 * \brief Packets a MLME Callback to avoid misalignments
 *
 * \param puc_buf_des     Pointer to the destination buffer
 * \param px_buf_org      Pointer to the source buffer
 *
 * \retval length of the buffer packet
 */
static int _serial_if_mlme_pack_node_432_table(uint8_t *puc_buf_des,
		uint8_t *px_buf_org)
{
	uint8_t *puc_packet;

	puc_packet = puc_buf_des;

	*puc_packet++ = px_buf_org[0]; /* address */
	*puc_packet++ = px_buf_org[1]; /* address */
	memcpy(puc_packet, &px_buf_org[2], SERIAL_NUMBER_432_MAC);
	puc_packet += SERIAL_NUMBER_432_MAC;
	*puc_packet++ = px_buf_org[18]; /* lenSerial */
	memcpy(puc_packet, &px_buf_org[19], PRIME_MAC_LENGTH);  /* mac */
	puc_packet += PRIME_MAC_LENGTH;
	*puc_packet++ = px_buf_org[25]; /* arq */

	return(puc_packet - puc_buf_des);
}

/**
 * \internal
 * \brief Packets a MLME Callback to avoid misalignments
 *
 * \param puc_buf_des     Pointer to the destination buffer
 * \param px_buf_org      Pointer to the source buffer
 *
 * \retval length of the buffer packet
 */
static int _serial_if_mlme_pack_available_switch_table(uint8_t *puc_buf_des,
		uint8_t *px_buf_org)
{
	uint8_t *puc_packet;

	puc_packet = puc_buf_des;

	memcpy(puc_packet, px_buf_org, PRIME_MAC_LENGTH); /* sna */
	puc_packet += PRIME_MAC_LENGTH;
	*puc_packet++ = px_buf_org[6]; /* sid */
	*puc_packet++ = px_buf_org[7]; /* sid */
	*puc_packet++ = px_buf_org[8]; /* level */
	*puc_packet++ = px_buf_org[9]; /* rxLevel */
	*puc_packet++ = px_buf_org[10]; /* rxSNR */

	return(puc_packet - puc_buf_des);
}

/**
 * \internal
 * \brief Packets a MLME Callback to avoid misalignments
 *
 * \param puc_buf_des     Pointer to the destination buffer
 * \param px_buf_org      Pointer to the source buffer
 *
 * \retval length of the buffer packet
 */
static int _serial_if_mlme_pack_list_phy_comm(uint8_t *puc_buf_des,
		uint8_t *px_buf_org)
{
	uint8_t *puc_packet;

	puc_packet = puc_buf_des;

	memcpy(puc_packet, px_buf_org, PRIME_MAC_LENGTH); /* eui48 */
	puc_packet += PRIME_MAC_LENGTH;
	*puc_packet++ = px_buf_org[6]; /* txPower */
	*puc_packet++ = px_buf_org[7]; /* txCoding */
	*puc_packet++ = px_buf_org[8]; /* rxCoding */
	*puc_packet++ = px_buf_org[9]; /* rxLevel */
	*puc_packet++ = px_buf_org[10]; /* snr */
	*puc_packet++ = px_buf_org[11]; /* txPowerMod */
	*puc_packet++ = px_buf_org[12]; /* txCodingMod */
	*puc_packet++ = px_buf_org[13]; /* rxCodingMod */

	return(puc_packet - puc_buf_des);
}

/**
 * \internal
 * \brief Packets a MLME Callback to avoid misalignments
 *
 * \param puc_buf_des     Pointer to the destination buffer
 * \param px_buf_org      Pointer to the source buffer
 *
 * \retval length of the buffer packet
 */
static int _serial_if_mlme_pack_active_conn_ex(uint8_t *puc_buf_des,
		uint8_t *px_buf_org)
{
	uint8_t *puc_packet;

	puc_packet = puc_buf_des;

	memcpy(puc_packet, px_buf_org, PRIME_MAC_LENGTH); /* mac */
	puc_packet += PRIME_MAC_LENGTH;
	*puc_packet++ = px_buf_org[6]; /* sid */
	*puc_packet++ = px_buf_org[7]; /* sid */
	*puc_packet++ = px_buf_org[8]; /* lnid */
	*puc_packet++ = px_buf_org[9]; /* lnid */
	*puc_packet++ = px_buf_org[10]; /* lcid */
	*puc_packet++ = px_buf_org[11]; /* lcid */
	*puc_packet++ = px_buf_org[12]; /* type */

	return(puc_packet - puc_buf_des);
}

/**
 * \internal
 * \brief Packets a MLME Callback to avoid misalignments
 *
 * \param puc_buf_des     Pointer to the destination buffer
 * \param px_buf_org      Pointer to the source buffer
 *
 * \retval length of the buffer packet
 */
static int _serial_if_mlme_pack_ex_snr_table(uint8_t *puc_buf_des,
		uint8_t *px_buf_org)
{
	uint8_t *puc_packet;

	puc_packet = puc_buf_des;

	*puc_packet++ = px_buf_org[0]; /* sid */
	*puc_packet++ = px_buf_org[1]; /* lnid */
	*puc_packet++ = px_buf_org[2]; /* lnid */
	*puc_packet++ = px_buf_org[3]; /* exSNR */

	return(puc_packet - puc_buf_des);
}

/**
 * \internal
 * \brief Packets a MLME Callback to avoid misalignments
 *
 * \param puc_mlme_command     Pointer to the MLME command
 * \param l_len                Command length
 *
 * \retval length of the buffer packet
 */
static int _serial_if_mlme_pack_mlme_callback(uint8_t *puc_mlme_command,
		int l_len)
{
	uint8_t *puc_command;
	uint16_t us_pibAttrib;
	uint8_t uc_idx;
	uint8_t uc_list_idx;

	puc_command = puc_mlme_command;
	memcpy(commandMlmeToPack, puc_mlme_command, l_len);
	switch (((MlmeCommand *)puc_command)->cmd) {
	case PRIME_MLME_UNREGISTER_CONFIRM:
	case PRIME_MLME_PROMOTE_CONFIRM:
	case PRIME_MLME_DEMOTE_CONFIRM:
	case PRIME_MLME_RESET_CONFIRM:
		*puc_command++ = ((MlmeResult *)commandMlmeToPack)->cmd;
		*puc_command++ = ((MlmeResult *)commandMlmeToPack)->result;
		return(puc_command - puc_mlme_command);

	case PRIME_MLME_SET_CONFIRM:
		puc_command += 2 * sizeof(uint8_t);         /* (cmd + result) */
		*puc_command++ = commandMlmeToPack[2];      /* pibAttrib high part */
		*puc_command++ = commandMlmeToPack[3];      /* pibAttrib low part */
		return(puc_command - puc_mlme_command);

	case PRIME_MLME_UNREGISTER_INDICATION:
	case PRIME_MLME_PROMOTE_INDICATION:
	case PRIME_MLME_DEMOTE_INDICATION:
		*puc_command++ = ((MlmeCommand *)commandMlmeToPack)->cmd;
		return(puc_command - puc_mlme_command);

	case PRIME_MLME_REGISTER_CONFIRM:
		*puc_command++ = ((MlmeRegisterConfirm *)commandMlmeToPack)->cmd;
		*puc_command++ = ((MlmeRegisterConfirm *)commandMlmeToPack)->result;
		memcpy(puc_command, ((MlmeRegisterConfirm *)commandMlmeToPack)->sna, PRIME_MAC_LENGTH);
		puc_command += PRIME_MAC_LENGTH;
		memcpy(puc_command, ((MlmeRegisterConfirm *)commandMlmeToPack)->mac, PRIME_MAC_LENGTH);
		puc_command += PRIME_MAC_LENGTH;
		return(puc_command - puc_mlme_command);

	case PRIME_MLME_REGISTER_INDICATION:
		*puc_command++ = ((MlmeRegisterConfirm *)commandMlmeToPack)->cmd;
		memcpy(puc_command, ((MlmeRegisterConfirm *)commandMlmeToPack)->sna, PRIME_MAC_LENGTH);
		puc_command += PRIME_MAC_LENGTH;
		memcpy(puc_command, ((MlmeRegisterConfirm *)commandMlmeToPack)->mac, PRIME_MAC_LENGTH);
		puc_command += PRIME_MAC_LENGTH;
		return(puc_command - puc_mlme_command);

	case PRIME_MLME_GET_CONFIRM:         /* MlmeGetValue */
		if (l_len == sizeof(MlmeGetValue)) {
			puc_command += 2 * sizeof(uint8_t);
			*puc_command++ = commandMlmeToPack[2];
			*puc_command++ = commandMlmeToPack[3];
			*puc_command++ = commandMlmeToPack[4];
			*puc_command++ = commandMlmeToPack[5];
			*puc_command++ = commandMlmeToPack[6];
			*puc_command++ = commandMlmeToPack[7];
			return(puc_command - puc_mlme_command);
		} else {         /* MlmeGetArray */
			puc_command += 2 * sizeof(uint8_t);
			*puc_command++ = commandMlmeToPack[2];
			*puc_command++ = commandMlmeToPack[3];
			memcpy(puc_command, &commandMlmeToPack[4], l_len - (sizeof(MlmeGetArray) - 1));
			puc_command += l_len - (sizeof(MlmeGetArray) - 1);
			return(puc_command - puc_mlme_command);
		}

	case PRIME_MLME_LIST_GET_CONFIRM:
		puc_command += 2 * sizeof(uint8_t);
		*puc_command++ = commandMlmeToPack[2];
		*puc_command++ = commandMlmeToPack[3];
		puc_command += 2 * sizeof(uint8_t);
		/* Check numItems */
		if (commandMlmeToPack[5] == 0) {
			return(puc_command - puc_mlme_command);
		}

		uc_list_idx = 6;   /* commandMlmeToPack[6] corresponds to the first list item */
		for (uc_idx = 0; uc_idx < commandMlmeToPack[5]; uc_idx++) {
			us_pibAttrib = (uint16_t)(commandMlmeToPack[2] << 8) + (uint16_t)commandMlmeToPack[3];
			switch (us_pibAttrib) {
			case PIB_MAC_LIST_REGISTER_DEVICES:
				puc_command += _serial_if_mlme_pack_reg_devices(puc_command,
						&commandMlmeToPack[uc_list_idx]);
				uc_list_idx += sizeof(MlmeRegDevice);
				break;

			case PIB_MAC_LIST_ACTIVE_CONN:
				puc_command += _serial_if_mlme_pack_active_conn(puc_command,
						&commandMlmeToPack[uc_list_idx]);
				uc_list_idx += sizeof(MlmeActiveConn);
				break;

			case PIB_MAC_LIST_MCAST_ENTRIES:
				puc_command += _serial_if_mlme_pack_mcast_entries(puc_command,
						&commandMlmeToPack[uc_list_idx]);
				uc_list_idx += sizeof(MlmeMcastEntrie);
				break;

			case PIB_MAC_LIST_SWITCH_TABLE:
				puc_command += _serial_if_mlme_pack_switch_table(puc_command,
						&commandMlmeToPack[uc_list_idx]);
				uc_list_idx += sizeof(MlmeSwitchNode);
				break;

			case PIB_MAC_LIST_DIRECT_CONN:
				puc_command += _serial_if_mlme_pack_direct_conn(puc_command,
						&commandMlmeToPack[uc_list_idx]);
				uc_list_idx += sizeof(MlmeDirectConn);
				break;

			case PIB_MAC_LIST_DIRECT_TABLE:
				puc_command += _serial_if_mlme_pack_direct_switch_table(puc_command,
						&commandMlmeToPack[uc_list_idx]);
				uc_list_idx += sizeof(MlmeDirectSwitchNode);
				break;

			case PIB_MAC_LIST_AVAILABLE_SWITCHES:
				puc_command += _serial_if_mlme_pack_available_switch_table(puc_command,
						&commandMlmeToPack[uc_list_idx]);
				uc_list_idx += sizeof(MlmeAvailableSwitchNode);
				break;

			case PIB_MAC_LIST_PHY_COMM:
				puc_command += _serial_if_mlme_pack_list_phy_comm(puc_command,
						&commandMlmeToPack[uc_list_idx]);
				uc_list_idx += sizeof(MlmePhyCommDevice);
				break;

			case PIB_MAC_LIST_ACTIVE_CONN_EX:
				puc_command += _serial_if_mlme_pack_active_conn_ex(puc_command,
						&commandMlmeToPack[uc_list_idx]);
				uc_list_idx += sizeof(MlmeActiveConnEx);
				break;

			case PIB_MAC_LIST_EX_SNR:
				puc_command += _serial_if_mlme_pack_ex_snr_table(puc_command,
						&commandMlmeToPack[uc_list_idx]);
				uc_list_idx += sizeof(ExSNRElementType);
				break;

			case PIB_432_LIST_NODES:
				puc_command += _serial_if_mlme_pack_node_432_table(puc_command,
						&commandMlmeToPack[uc_list_idx]);
				uc_list_idx += sizeof(Node432Type);
				break;

			default:
				break;
			}
		}
		return(puc_command - puc_mlme_command);
	}
	/* This point of code shouldn't be reached */
	return(-1);
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
uint8_t serial_if_mlme_api_parser(uint8_t *puc_rx_msg, uint16_t us_len)
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
	case PRIME_MLME_REGISTER_REQUEST:
		c_result = _serial_if_mlme_unpack_register_req();
		break;

	case PRIME_MLME_UNREGISTER_REQUEST_BASE:
		c_result = _serial_if_mlme_unpack_unregister_req_base();
		break;

	case PRIME_MLME_UNREGISTER_REQUEST_SERVICE:
		c_result = prime_MLME_UNREGISTER_request_service();
		break;

	case PRIME_MLME_PROMOTE_REQUEST_BASE:
		c_result = _serial_if_mlme_unpack_promote_req_base();
		break;

	case PRIME_MLME_PROMOTE_REQUEST_SERVICE:
		c_result = prime_MLME_PROMOTE_request_service();
		break;

	case PRIME_MLME_DEMOTE_REQUEST_BASE:
		c_result = _serial_if_mlme_unpack_demote_req_base();
		break;

	case PRIME_MLME_DEMOTE_REQUEST_SERVICE:
		c_result = prime_MLME_DEMOTE_request_service();
		break;

	case PRIME_MLME_RESET_REQUEST:
		c_result = prime_MLME_RESET_request();
		break;

	case PRIME_MLME_GET_REQUEST:
		c_result = _serial_if_mlme_unpack_get_req();
		break;

	case PRIME_MLME_LIST_GET_REQUEST:
		c_result = _serial_if_mlme_unpack_list_get_req();
		break;

	case PRIME_MLME_SET_REQUEST:
		c_result = _serial_if_mlme_unpack_set_req();
		break;

	default:
		c_result = 0;
	}

	if (c_result == 0) {
		return 1;
	} else {
		return 0;
	}
}

/**
 * \brief Function to encapsulate MLME Tx Control Process
 *
 * \note This function will be called using a pointer
 *
 * \retval 0 if the request has been sent
 * \retval -1 if the request has not been sent
 */
int8_t serial_if_mlme_process(void)
{
	int l_len, l_packet_len;
	int8_t c_result = -1;
	x_usi_serial_cmd_params_t x_pkt_usi;

	l_len = prime_MLME_callback(NewMlmeCallBackCmd);
	if (l_len > 0) {
		l_packet_len = _serial_if_mlme_pack_mlme_callback(NewMlmeCallBackCmd, l_len);
		/* pack and send the primitive */
		x_pkt_usi.uc_protocol_type = PROTOCOL_MLME_PRIME;
		x_pkt_usi.ptr_buf = &NewMlmeCallBackCmd[0];
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
