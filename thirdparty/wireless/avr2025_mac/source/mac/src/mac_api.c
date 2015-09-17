/**
 * @file mac_api.c
 *
 * @brief This file contains MAC API functions.
 *
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
 *
 */

/*
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

/**
 * \ingroup group_mac_prot
 * \defgroup group_mac_api MAC API's
 *
 * The application interfaces the MAC stack via the MAC-API
 *
 */

/**
 * \ingroup group_mac_api
 * \defgroup group_mac_req MAC Request API's
 *
 * This module describes all MAC Request API's
 */

/**
 * \ingroup group_mac_api
 * \defgroup group_mac_gen MAC General API's
 *
 * This module describes all MAC General API's
 */

/**
 * \ingroup group_mac_api
 * \defgroup group_mac_resp MAC Response API's
 *
 * This module describes all MAC Response API's
 */

/* === Includes ============================================================ */
#include <compiler.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "return_val.h"
#include "pal.h"
#include "bmm.h"
#include "qmm.h"
#include "ieee_const.h"
#include "mac_api.h"
#include "stack_config.h"
#include "mac.h"
#include "mac_build_config.h"
#include "pal.h"
#include "mac_internal.h"
#include "mac_msg_types.h"
#include "mac_msg_const.h"
/* === Types =============================================================== */

/* === Macros ============================================================== */

/* === Globals ============================================================= */

/**
 * Queue used by MAC for communication to next higher layer.
 */
queue_t mac_nhle_q;

/* === Prototypes ========================================================== */

/* === Implementation ====================================================== */

retval_t wpan_init(void)
{
	/* Init queue used for MAC to next higher layer communication */
#ifdef ENABLE_QUEUE_CAPACITY
	qmm_queue_init(&mac_nhle_q, MAC_NHLE_QUEUE_CAPACITY);
#else
	qmm_queue_init(&mac_nhle_q);
#endif  /* ENABLE_QUEUE_CAPACITY */

	/*
	 * Initialize MAC.
	 */
	return mac_init();
}

bool wpan_task(void)
{
	bool event_processed;
	uint8_t *event = NULL;

	/* mac_task returns true if a request was processed completely */
	event_processed = mac_task();

	/*
	 * MAC to NHLE event queue should be dispatched
	 * irrespective of the dispatcher state.
	 */
	event = (uint8_t *)qmm_queue_remove(&mac_nhle_q, NULL);

	/* If an event has been detected, handle it. */
	if (NULL != event) {
		dispatch_event(event);
		event_processed = true;
	}

#ifdef ENABLE_RTB
	rtb_task();
#endif  /* ENABLE_RTB */
	tal_task();
	pal_task();

	return (event_processed);
}

/* MAC level API */

#if ((defined MAC_SECURITY_ZIP)  || (defined MAC_SECURITY_2006))
bool wpan_mcps_data_req(uint8_t SrcAddrMode,
		wpan_addr_spec_t *DstAddrSpec,
		uint8_t msduLength,
		uint8_t *msdu,
		uint8_t msduHandle,
		uint8_t TxOptions,
		uint8_t SecurityLevel,
		uint8_t *KeySource,
		uint8_t KeyIdMode,
		uint8_t KeyIndex)
#else   /* No MAC_SECURITY */
bool wpan_mcps_data_req(uint8_t SrcAddrMode,
		wpan_addr_spec_t *DstAddrSpec,
		uint8_t msduLength,
		uint8_t *msdu,
		uint8_t msduHandle,
		uint8_t TxOptions)
#endif  /* MAC_SECURITY */
{
	buffer_t *buffer_header;
	mcps_data_req_t *mcps_data_req;
	uint8_t *payload_pos;

	if (msduLength > aMaxMACPayloadSize) {
		/* Frame is too long and thus rejected immediately */
		return false;
	}

	/* Allocate a large buffer for mcps data request */
	buffer_header = bmm_buffer_alloc(LARGE_BUFFER_SIZE);

	if (NULL == buffer_header) {
		/* Buffer is not available */
		return false;
	}

	/* Get the buffer body from buffer header */

	mcps_data_req =  (mcps_data_req_t *)BMM_BUFFER_POINTER(buffer_header);

	/* Construct mcps_data_req_t message */
	mcps_data_req->cmdcode = MCPS_DATA_REQUEST;

	/* Source addr mode */
	mcps_data_req->SrcAddrMode = SrcAddrMode;

	/* Destination addr spec */
	mcps_data_req->DstAddrMode = DstAddrSpec->AddrMode;

#ifdef TEST_HARNESS_BIG_ENDIAN
	mcps_data_req->DstPANId = CPU_ENDIAN_TO_LE16(DstAddrSpec->PANId);
#else
	mcps_data_req->DstPANId = DstAddrSpec->PANId;
#endif

	if ((WPAN_ADDRMODE_SHORT == mcps_data_req->DstAddrMode) ||
			(WPAN_ADDRMODE_LONG  == mcps_data_req->DstAddrMode)) {
		/*
		 * In case a short address is indicated, but the address is not
		 * properly set, the entire address is first cleared.
		 */
		mcps_data_req->DstAddr = 0;
		ADDR_COPY_DST_SRC_64(mcps_data_req->DstAddr,
				DstAddrSpec->Addr.long_address);
	}

	/* Other fields */
	mcps_data_req->msduHandle = msduHandle;
	mcps_data_req->TxOptions = TxOptions;
#if ((defined MAC_SECURITY_ZIP)  || (defined MAC_SECURITY_2006))
	mcps_data_req->SecurityLevel = SecurityLevel;
	mcps_data_req->KeySource = KeySource;
	mcps_data_req->KeyIdMode = KeyIdMode;
	mcps_data_req->KeyIndex = KeyIndex;
#endif  /* (MAC_SECURITY_ZIP || MAC_SECURITY_2006) */

	mcps_data_req->msduLength = msduLength;

	/* Find the position where the data payload is to be updated */
	payload_pos = ((uint8_t *)mcps_data_req) +
			(LARGE_BUFFER_SIZE - FCS_LEN - msduLength);

	/* Copy the payload to the end of buffer */
	memcpy(payload_pos, msdu, msduLength);

#ifdef ENABLE_QUEUE_CAPACITY
	if (MAC_SUCCESS != qmm_queue_append(&nhle_mac_q, buffer_header)) {
		/*
		 * MCPS-DATA.request is not appended into NHLE MAC
		 * queue, hence free the buffer allocated and return false
		 */
		bmm_buffer_free(buffer_header);
		return false;
	}

#else
	qmm_queue_append(&nhle_mac_q, buffer_header);
#endif  /* ENABLE_QUEUE_CAPACITY */

	return true;
}

#if ((MAC_PURGE_REQUEST_CONFIRM == 1) && (MAC_INDIRECT_DATA_BASIC == 1))
bool wpan_mcps_purge_req(uint8_t msduHandle)
{
	buffer_t *buffer_header;
	mcps_purge_req_t *mcps_purge_req;

	/* Allocate small buffer for mcps purge request */
	buffer_header = bmm_buffer_alloc(LARGE_BUFFER_SIZE);

	if (NULL == buffer_header) {
		/* Buffer is not available */
		return false;
	}

	/* Get the buffer body from buffer header */
	mcps_purge_req = (mcps_purge_req_t *)BMM_BUFFER_POINTER(buffer_header);

	/* Update the purge request structure */
	mcps_purge_req->cmdcode = MCPS_PURGE_REQUEST;
	mcps_purge_req->msduHandle = msduHandle;

#ifdef ENABLE_QUEUE_CAPACITY
	if (MAC_SUCCESS != qmm_queue_append(&nhle_mac_q, buffer_header)) {
		/*
		 * MCPS-PURGE.request is not appended into NHLE MAC
		 * queue, hence free the buffer allocated and return false
		 */
		bmm_buffer_free(buffer_header);
		return false;
	}

#else
	qmm_queue_append(&nhle_mac_q, buffer_header);
#endif  /* ENABLE_QUEUE_CAPACITY */

	return true;
}

#endif /* ((MAC_PURGE_REQUEST_CONFIRM == 1) && (MAC_INDIRECT_DATA_BASIC == 1))
       **/

#if (MAC_ASSOCIATION_REQUEST_CONFIRM == 1)
bool wpan_mlme_associate_req(uint8_t LogicalChannel,
		uint8_t ChannelPage,
		wpan_addr_spec_t *CoordAddrSpec,
		uint8_t CapabilityInformation)
{
	buffer_t *buffer_header;
	mlme_associate_req_t *mlme_associate_req;

	/* Allocate a buffer for mlme associate request */
	buffer_header = bmm_buffer_alloc(LARGE_BUFFER_SIZE);

	/* Check for buffer availability */
	if (NULL == buffer_header) {
		return false;
	}

	/* Get the buffer body from buffer header */
	mlme_associate_req = (mlme_associate_req_t *)BMM_BUFFER_POINTER(
			buffer_header);

	/* Construct mlme_associate_req_t message */
	mlme_associate_req->cmdcode = MLME_ASSOCIATE_REQUEST;

	/* Operating channel */
	mlme_associate_req->LogicalChannel = LogicalChannel;

	/* Coordinator address spec */
	mlme_associate_req->CoordAddrMode = CoordAddrSpec->AddrMode;

#ifdef TEST_HARNESS_BIG_ENDIAN
	mlme_associate_req->CoordPANId
		= CPU_ENDIAN_TO_LE16(CoordAddrSpec->PANId);
#else
	mlme_associate_req->CoordPANId = CoordAddrSpec->PANId;
#endif

	ADDR_COPY_DST_SRC_64(mlme_associate_req->CoordAddress.long_address,
			CoordAddrSpec->Addr.long_address);

	/* Other fields */
	mlme_associate_req->CapabilityInformation = CapabilityInformation;
	mlme_associate_req->ChannelPage = ChannelPage;

	/* Insert service message into NHLE MLME queue */
#ifdef ENABLE_QUEUE_CAPACITY
	if (MAC_SUCCESS != qmm_queue_append(&nhle_mac_q, buffer_header)) {
		/*
		 * MLME-ASSOCIATE.request is not appended into NHLE MAC
		 * queue, hence free the buffer allocated
		 */
		bmm_buffer_free(buffer_header);
		return false;
	}

#else
	qmm_queue_append(&nhle_mac_q, buffer_header);
#endif  /* ENABLE_QUEUE_CAPACITY */

	return true;
}

#endif /* (MAC_ASSOCIATION_REQUEST_CONFIRM == 1) */

#if (MAC_ASSOCIATION_INDICATION_RESPONSE == 1)
bool wpan_mlme_associate_resp(uint64_t DeviceAddress,
		uint16_t AssocShortAddress,
		uint8_t status)
{
	buffer_t *buffer_header;
	mlme_associate_resp_t *mlme_associate_resp;

	/* Allocate a small buffer for association response */
	buffer_header = bmm_buffer_alloc(LARGE_BUFFER_SIZE);

	if (NULL == buffer_header) {
		/* Buffer is not available */
		return false;
	}

	/* Get the buffer body from buffer header */
	mlme_associate_resp = (mlme_associate_resp_t *)BMM_BUFFER_POINTER(
			buffer_header);

	/* Construct mlme_associate_resp_t message */
	mlme_associate_resp->cmdcode = MLME_ASSOCIATE_RESPONSE;

	/* Other fields */
#ifdef TEST_HARNESS_BIG_ENDIAN
	mlme_associate_resp->DeviceAddress  = CPU_ENDIAN_TO_LE64(DeviceAddress);
	mlme_associate_resp->AssocShortAddress = CPU_ENDIAN_TO_LE16(
			AssocShortAddress);
#else
	mlme_associate_resp->DeviceAddress  = DeviceAddress;
	mlme_associate_resp->AssocShortAddress = AssocShortAddress;
#endif
	mlme_associate_resp->status = status;

	/* Insert mlme_associate_resp_t into NHLE MAC queue */
#ifdef ENABLE_QUEUE_CAPACITY
	if (MAC_SUCCESS != qmm_queue_append(&nhle_mac_q, buffer_header)) {
		/*
		 * MLME-ASSOCIATE.response is not appended into NHLE MAC
		 * queue, hence free the buffer allocated
		 */
		bmm_buffer_free(buffer_header);
		return false;
	}

#else
	qmm_queue_append(&nhle_mac_q, buffer_header);
#endif  /* ENABLE_QUEUE_CAPACITY */

	return true;
}

#endif /* (MAC_ASSOCIATION_INDICATION_RESPONSE == 1) */

#if (MAC_DISASSOCIATION_BASIC_SUPPORT == 1)
bool wpan_mlme_disassociate_req(wpan_addr_spec_t *DeviceAddrSpec,
		uint8_t DisassociateReason,
		bool TxIndirect)
{
	buffer_t *buffer_header;
	mlme_disassociate_req_t *mlme_disassociate_req;

	/* Allocate a small buffer for disassociation request */
	buffer_header = bmm_buffer_alloc(LARGE_BUFFER_SIZE);

	if (NULL == buffer_header) {
		/* Buffer is not available */
		return false;
	}

	/* Get the buffer body from buffer header */
	mlme_disassociate_req = (mlme_disassociate_req_t *)BMM_BUFFER_POINTER(
			buffer_header);

	/* Update the disassociate request structure */
	mlme_disassociate_req->cmdcode = MLME_DISASSOCIATE_REQUEST;
	mlme_disassociate_req->DisassociateReason = DisassociateReason;
	mlme_disassociate_req->DeviceAddrMode = DeviceAddrSpec->AddrMode;
#ifdef TEST_HARNESS_BIG_ENDIAN
	mlme_disassociate_req->DevicePANId = CPU_ENDIAN_TO_LE16(
			DeviceAddrSpec->PANId);
#else
	mlme_disassociate_req->DevicePANId = DeviceAddrSpec->PANId;
#endif
	ADDR_COPY_DST_SRC_64(mlme_disassociate_req->DeviceAddress,
			DeviceAddrSpec->Addr.long_address);
	mlme_disassociate_req->TxIndirect = TxIndirect;

#ifdef ENABLE_QUEUE_CAPACITY
	if (MAC_SUCCESS != qmm_queue_append(&nhle_mac_q, buffer_header)) {
		/*
		 * MLME-DISASSOCIATE.request is not appended into NHLE MAC
		 * queue, hence free the buffer allocated and return false
		 */
		bmm_buffer_free(buffer_header);
		return false;
	}

#else
	qmm_queue_append(&nhle_mac_q, buffer_header);
#endif  /* ENABLE_QUEUE_CAPACITY */

	return true;
}

#endif /* (MAC_DISASSOCIATION_BASIC_SUPPORT == 1) */

#if (MAC_ORPHAN_INDICATION_RESPONSE == 1)
bool wpan_mlme_orphan_resp(uint64_t OrphanAddress,
		uint16_t ShortAddress,
		bool AssociatedMember)
{
	buffer_t *buffer_header;
	mlme_orphan_resp_t *mlme_orphan_resp;

	/* Allocate a small buffer for orphan response */
	buffer_header = bmm_buffer_alloc(LARGE_BUFFER_SIZE);

	if (NULL == buffer_header) {
		/* Buffer is not available */
		return false;
	}

	/* Get the buffer body from buffer header */
	mlme_orphan_resp = (mlme_orphan_resp_t *)BMM_BUFFER_POINTER(
			buffer_header);

	/* Update the orphan response structure */
	mlme_orphan_resp->cmdcode = MLME_ORPHAN_RESPONSE;
#ifdef TEST_HARNESS_BIG_ENDIAN
	mlme_orphan_resp->OrphanAddress = CPU_ENDIAN_TO_LE64(OrphanAddress);
	mlme_orphan_resp->ShortAddress  = CPU_ENDIAN_TO_LE16(ShortAddress);
#else
	mlme_orphan_resp->OrphanAddress = OrphanAddress;
	mlme_orphan_resp->ShortAddress  = ShortAddress;
#endif
	mlme_orphan_resp->AssociatedMember = AssociatedMember;

#ifdef ENABLE_QUEUE_CAPACITY
	if (MAC_SUCCESS != qmm_queue_append(&nhle_mac_q, buffer_header)) {
		/*
		 * MLME-ORPHAN.response is not appended into NHLE MAC
		 * queue, hence free the buffer allocated and return false
		 */
		bmm_buffer_free(buffer_header);
		return false;
	}

#else
	qmm_queue_append(&nhle_mac_q, buffer_header);
#endif  /* ENABLE_QUEUE_CAPACITY */

	return true;
}

#endif /* (MAC_ORPHAN_INDICATION_RESPONSE == 1) */

bool wpan_mlme_reset_req(bool SetDefaultPib)
{
	buffer_t *buffer_header;
	mlme_reset_req_t *mlme_reset_req;

	/* Allocate a small buffer for reset request */
	buffer_header = bmm_buffer_alloc(LARGE_BUFFER_SIZE);

	if (NULL == buffer_header) {
		/* Buffer is not available */
		return false;
	}

	/* Get the buffer body from buffer header */
	mlme_reset_req = (mlme_reset_req_t *)BMM_BUFFER_POINTER(buffer_header);

	/* Update the reset request structure */
	mlme_reset_req->cmdcode = MLME_RESET_REQUEST;
	mlme_reset_req->SetDefaultPIB = SetDefaultPib;

#ifdef ENABLE_QUEUE_CAPACITY
	if (MAC_SUCCESS != qmm_queue_append(&nhle_mac_q, buffer_header)) {
		/*
		 * MLME-RESET.request is not appended into NHLE MAC
		 * queue, hence free the buffer allocated and return false
		 */
		bmm_buffer_free(buffer_header);
		return false;
	}

#else
	qmm_queue_append(&nhle_mac_q, buffer_header);
#endif  /* ENABLE_QUEUE_CAPACITY */

	return true;
}

#if (MAC_GET_SUPPORT == 1)
#if ((defined MAC_SECURITY_ZIP)  || (defined MAC_SECURITY_2006))
bool wpan_mlme_get_req(uint8_t PIBAttribute, uint8_t PIBAttributeIndex)
#else
bool wpan_mlme_get_req(uint8_t PIBAttribute)
#endif  /* (MAC_SECURITY_ZIP || MAC_SECURITY_2006) */
{
	buffer_t *buffer_header;
	mlme_get_req_t *mlme_get_req;

	/* Allocate a large buffer for get request as maximum beacon payload
	 * should be accommodated */
	buffer_header = bmm_buffer_alloc(LARGE_BUFFER_SIZE);

	/* Check for buffer availability */
	if (NULL == buffer_header) {
		return false;
	}

	/* Get the buffer body from buffer header */
	mlme_get_req = (mlme_get_req_t *)BMM_BUFFER_POINTER(buffer_header);

	/* Update the get request structure */
	mlme_get_req->cmdcode = MLME_GET_REQUEST;
	mlme_get_req->PIBAttribute = PIBAttribute;
#if ((defined MAC_SECURITY_ZIP)  || (defined MAC_SECURITY_2006))
	mlme_get_req->PIBAttributeIndex = PIBAttributeIndex;
#endif  /* (MAC_SECURITY_ZIP || MAC_SECURITY_2006) */

#ifdef ENABLE_QUEUE_CAPACITY
	if (MAC_SUCCESS != qmm_queue_append(&nhle_mac_q, buffer_header)) {
		/*
		 * MLME-GET.request is not appended into NHLE MAC
		 * queue, hence free the buffer allocated and return false
		 */
		bmm_buffer_free(buffer_header);
		return false;
	}

#else
	qmm_queue_append(&nhle_mac_q, buffer_header);
#endif  /* ENABLE_QUEUE_CAPACITY */

	return true;
}

#endif  /* (MAC_GET_SUPPORT == 1) */

#if ((defined MAC_SECURITY_ZIP)  || (defined MAC_SECURITY_2006))
bool wpan_mlme_set_req(uint8_t PIBAttribute,
		uint8_t PIBAttributeIndex,
		void *PIBAttributeValue)
#else
bool wpan_mlme_set_req(uint8_t PIBAttribute,
		void *PIBAttributeValue)
#endif  /* (MAC_SECURITY_ZIP || MAC_SECURITY_2006) */
{
	buffer_t *buffer_header;
	mlme_set_req_t *mlme_set_req;
	uint8_t pib_attribute_octet_no;

	/*
	 * Allocate a large buffer for set request as maximum beacon payload
	 * should be accommodated
	 */
	buffer_header = bmm_buffer_alloc(LARGE_BUFFER_SIZE);

	/* Check for buffer availability */
	if (NULL == buffer_header) {
		return false;
	}

	/* Get size of PIB attribute to be set */
	pib_attribute_octet_no = mac_get_pib_attribute_size(PIBAttribute);

	/* Get the buffer body from buffer header */
	mlme_set_req = (mlme_set_req_t *)BMM_BUFFER_POINTER(buffer_header);

	/* Construct mlme_set_req_t message */
	mlme_set_req->cmdcode = MLME_SET_REQUEST;

	/* Attribute and attribute value length */
	mlme_set_req->PIBAttribute = PIBAttribute;
#if ((defined MAC_SECURITY_ZIP)  || (defined MAC_SECURITY_2006))
	mlme_set_req->PIBAttributeIndex = PIBAttributeIndex;
#endif  /* (MAC_SECURITY_ZIP || MAC_SECURITY_2006) */

	/* Attribute value */

#ifdef TEST_HARNESS_BIG_ENDIAN
	if ((macMaxFrameTotalWaitTime == PIBAttribute)      ||
			(macResponseWaitTime == PIBAttribute)           ||
			(macTransactionPersistenceTime == PIBAttribute) ||
			(macBeaconTxTime == PIBAttribute)
#if ((defined MAC_SECURITY_ZIP)  || (defined MAC_SECURITY_2006))
			||
			(macFrameCounter == PIBAttribute) ||
			(macDeviceTableEntries == PIBAttribute)
#endif
			) {
		memcpy_be((void *)&(mlme_set_req->PIBAttributeValue),
				(void *)PIBAttributeValue,
				(size_t)pib_attribute_octet_no);
	} else {
		memcpy((void *)&(mlme_set_req->PIBAttributeValue),
				(void *)PIBAttributeValue,
				(size_t)pib_attribute_octet_no);
	}

#else
	memcpy((void *)&(mlme_set_req->PIBAttributeValue),
			(void *)PIBAttributeValue,
			(size_t)pib_attribute_octet_no);
#endif

	/* Insert message into NHLE MAC queue */
#ifdef ENABLE_QUEUE_CAPACITY
	if (MAC_SUCCESS != qmm_queue_append(&nhle_mac_q, buffer_header)) {
		/*
		 * MLME-SET.request is not appended into NHLE MAC
		 * queue, hence free the buffer allocated and return false
		 */
		bmm_buffer_free(buffer_header);
		return false;
	}

#else
	qmm_queue_append(&nhle_mac_q, buffer_header);
#endif  /* ENABLE_QUEUE_CAPACITY */

	return true;
}

#if (MAC_RX_ENABLE_SUPPORT == 1)
bool wpan_mlme_rx_enable_req(bool DeferPermit,
		uint32_t RxOnTime,
		uint32_t RxOnDuration)
{
	buffer_t *buffer_header;
	mlme_rx_enable_req_t *mlme_rx_enable_req;

	/* Allocate a small buffer for rx enable request */
	buffer_header = bmm_buffer_alloc(LARGE_BUFFER_SIZE);

	if (NULL == buffer_header) {
		/* Buffer is not available */
		return false;
	}

	/* Get the buffer body from buffer header */
	mlme_rx_enable_req = (mlme_rx_enable_req_t *)BMM_BUFFER_POINTER(
			buffer_header);

	/* Update the rx enable request structure */
	mlme_rx_enable_req->cmdcode = MLME_RX_ENABLE_REQUEST;
	mlme_rx_enable_req->DeferPermit = DeferPermit;
	mlme_rx_enable_req->RxOnTime = RxOnTime;
	mlme_rx_enable_req->RxOnDuration = RxOnDuration;

#ifdef ENABLE_QUEUE_CAPACITY
	if (MAC_SUCCESS != qmm_queue_append(&nhle_mac_q, buffer_header)) {
		/*
		 * MLME-RX-ENABLE.request is not appended into NHLE MAC
		 * queue, hence free the buffer allocated and return false
		 */
		bmm_buffer_free(buffer_header);
		return false;
	}

#else
	qmm_queue_append(&nhle_mac_q, buffer_header);
#endif  /* ENABLE_QUEUE_CAPACITY */

	return true;
}

#endif /* (MAC_RX_ENABLE_SUPPORT == 1) */

#if (MAC_SCAN_SUPPORT == 1)
bool wpan_mlme_scan_req(uint8_t ScanType,
		uint32_t ScanChannels,
		uint8_t ScanDuration,
		uint8_t ChannelPage)
{
	buffer_t *buffer_header;
	mlme_scan_req_t *mlme_scan_req;

	/* Allocate a small buffer for scan request */
	buffer_header = bmm_buffer_alloc(LARGE_BUFFER_SIZE);

	if (NULL == buffer_header) {
		/* Buffer is not available */
		return false;
	}

	/* Get the buffer body from buffer header */
	mlme_scan_req = (mlme_scan_req_t *)BMM_BUFFER_POINTER(buffer_header);

	/* Update the scan request structure */
	mlme_scan_req->cmdcode = MLME_SCAN_REQUEST;
	mlme_scan_req->ScanType = ScanType;
	mlme_scan_req->ScanChannels = ScanChannels;
	mlme_scan_req->ScanDuration = ScanDuration;
	mlme_scan_req->ChannelPage = ChannelPage;

#ifdef ENABLE_QUEUE_CAPACITY
	if (MAC_SUCCESS != qmm_queue_append(&nhle_mac_q, buffer_header)) {
		/*
		 * MLME-SCAN.request is not appended into NHLE MAC
		 * queue, hence free the buffer allocated and return false
		 */
		bmm_buffer_free(buffer_header);
		return false;
	}

#else
	qmm_queue_append(&nhle_mac_q, buffer_header);
#endif  /* ENABLE_QUEUE_CAPACITY */

	return true;
}

#endif /* (MAC_SCAN_SUPPORT == 1) */

#if (MAC_START_REQUEST_CONFIRM == 1)
bool wpan_mlme_start_req(uint16_t PANId,
		uint8_t LogicalChannel,
		uint8_t ChannelPage,
		uint8_t BeaconOrder,
		uint8_t SuperframeOrder,
		bool PANCoordinator,
		bool BatteryLifeExtension,
		bool CoordRealignment
#ifdef MAC_SECURITY_BEACON
		, uint8_t CoordRealignSecurityLevel,
		uint8_t CoordRealignKeyIdMode,
		uint8_t *CoordRealignKeySource,
		uint8_t CoordRealignKeyIndex,
		uint8_t BeaconSecurityLevel,
		uint8_t BeaconKeyIdMode,
		uint8_t *BeaconKeySource,
		uint8_t BeaconKeyIndex
#endif
		)
{
	buffer_t *buffer_header;
	mlme_start_req_t *mlme_start_req;

	/* Allocate a small buffer for start request */
	buffer_header = bmm_buffer_alloc(LARGE_BUFFER_SIZE);

	if (NULL == buffer_header) {
		/* Buffer is not available */
		return false;
	}

	/* Get the buffer body from buffer header */
	mlme_start_req = (mlme_start_req_t *)BMM_BUFFER_POINTER(buffer_header);

	/* Update the start request structure */
	mlme_start_req->cmdcode = MLME_START_REQUEST;

#ifdef TEST_HARNESS_BIG_ENDIAN
	mlme_start_req->PANId = CPU_ENDIAN_TO_LE16(PANId);
#else
	mlme_start_req->PANId = PANId;
#endif
	mlme_start_req->LogicalChannel = LogicalChannel;
	mlme_start_req->BeaconOrder = BeaconOrder;
	mlme_start_req->SuperframeOrder = SuperframeOrder;
	mlme_start_req->PANCoordinator = PANCoordinator;
	mlme_start_req->BatteryLifeExtension = BatteryLifeExtension;
	mlme_start_req->CoordRealignment = CoordRealignment;
	mlme_start_req->ChannelPage = ChannelPage;

#ifdef MAC_SECURITY_BEACON
	mlme_start_req->CoordRealignSecurityLevel = CoordRealignSecurityLevel;
	mlme_start_req->CoordRealignKeyIdMode = CoordRealignKeyIdMode;
	mlme_start_req->CoordRealignKeySource = CoordRealignKeySource;
	mlme_start_req->CoordRealignKeyIndex = CoordRealignKeyIndex;
	mlme_start_req->BeaconSecurityLevel = BeaconSecurityLevel;
	mlme_start_req->BeaconKeyIdMode = BeaconKeyIdMode;
	mlme_start_req->BeaconKeySource = BeaconKeySource;
	mlme_start_req->BeaconKeyIndex = BeaconKeyIndex;
#endif

#ifdef ENABLE_QUEUE_CAPACITY
	if (MAC_SUCCESS != qmm_queue_append(&nhle_mac_q, buffer_header)) {
		/*
		 * MLME-START.request is not appended into NHLE MAC
		 * queue, hence free the buffer allocated ad return false
		 */
		bmm_buffer_free(buffer_header);
		return false;
	}

#else
	qmm_queue_append(&nhle_mac_q, buffer_header);
#endif  /* ENABLE_QUEUE_CAPACITY */

	return true;
}

#endif /* (MAC_START_REQUEST_CONFIRM == 1) */

#if (MAC_SYNC_REQUEST == 1)
bool wpan_mlme_sync_req(uint8_t LogicalChannel,
		uint8_t ChannelPage,
		bool TrackBeacon)
{
	buffer_t *buffer_header;
	mlme_sync_req_t *mlme_sync_req;

	/* Allocate a small buffer for sync request */
	buffer_header = bmm_buffer_alloc(LARGE_BUFFER_SIZE);

	if (NULL == buffer_header) {
		/* Buffer is not available */
		return false;
	}

	/* Get the buffer body from buffer header */
	mlme_sync_req = (mlme_sync_req_t *)BMM_BUFFER_POINTER(buffer_header);

	/* Update the sync request structure */
	mlme_sync_req->cmdcode = MLME_SYNC_REQUEST;
	mlme_sync_req->LogicalChannel = LogicalChannel;
	mlme_sync_req->ChannelPage = ChannelPage;
	mlme_sync_req->TrackBeacon = TrackBeacon;

#ifdef ENABLE_QUEUE_CAPACITY
	if (MAC_SUCCESS != qmm_queue_append(&nhle_mac_q, buffer_header)) {
		/*
		 * MLME-SYNC.request is not appended into NHLE MAC
		 * queue, hence free the buffer allocated and return false
		 */
		bmm_buffer_free(buffer_header);
		return false;
	}

#else
	qmm_queue_append(&nhle_mac_q, buffer_header);
#endif  /* ENABLE_QUEUE_CAPACITY */

	return true;
}

#endif /* (MAC_SYNC_REQUEST == 1) */

#if (MAC_INDIRECT_DATA_BASIC == 1)
bool wpan_mlme_poll_req(wpan_addr_spec_t *CoordAddrSpec)
{
	buffer_t *buffer_header;
	mlme_poll_req_t *mlme_poll_req;

	/* Allocate a small buffer for poll request */
	buffer_header = bmm_buffer_alloc(LARGE_BUFFER_SIZE);

	if (NULL == buffer_header) {
		/* Buffer is not available */
		return false;
	}

	/* Get the buffer body from buffer header */
	mlme_poll_req = (mlme_poll_req_t *)BMM_BUFFER_POINTER(buffer_header);

	/* construct mlme_poll_req_t message */
	mlme_poll_req->cmdcode = MLME_POLL_REQUEST;

	/* Other fileds. */
	mlme_poll_req->CoordAddrMode = CoordAddrSpec->AddrMode;
#ifdef TEST_HARNESS_BIG_ENDIAN
	mlme_poll_req->CoordPANId = CPU_ENDIAN_TO_LE16(CoordAddrSpec->PANId);
#else
	mlme_poll_req->CoordPANId = CoordAddrSpec->PANId;
#endif
	if (WPAN_ADDRMODE_SHORT == CoordAddrSpec->AddrMode) {
		ADDR_COPY_DST_SRC_16(mlme_poll_req->CoordAddress,
				CoordAddrSpec->Addr.short_address);
	} else {
		ADDR_COPY_DST_SRC_64(mlme_poll_req->CoordAddress,
				CoordAddrSpec->Addr.long_address);
	}

#ifdef ENABLE_QUEUE_CAPACITY
	if (MAC_SUCCESS != qmm_queue_append(&nhle_mac_q, buffer_header)) {
		/*
		 * MLME-POLL.request is not appended into NHLE MAC
		 * queue, hence free the buffer allocated and return false
		 */
		bmm_buffer_free(buffer_header);
		return false;
	}

#else
	qmm_queue_append(&nhle_mac_q, buffer_header);
#endif  /* ENABLE_QUEUE_CAPACITY */

	return true;
}

#endif /* (MAC_INDIRECT_DATA_BASIC == 1) */

#if (MAC_GTS_REQUEST == 1)
bool wpan_mlme_gts_req(uint16_t DevShortAddr, gts_char_t GtsChar)
{
#ifdef GTS_SUPPORT
	buffer_t *buffer_header;
	mlme_gts_req_t *mlme_gts_req;

	/* Allocate a small buffer for gts request */
	buffer_header = bmm_buffer_alloc(LARGE_BUFFER_SIZE);

	if (NULL == buffer_header) {
		/* Buffer is not available */
		return false;
	}

	/* Get the buffer body from buffer header */
	mlme_gts_req = (mlme_gts_req_t *)BMM_BUFFER_POINTER(buffer_header);

	/* construct mlme_gts_req_t message */
	mlme_gts_req->cmdcode = MLME_GTS_REQUEST;

	mlme_gts_req->DeviceShortAddr = CPU_ENDIAN_TO_LE16(DevShortAddr);
	/* Other fields. */
	mlme_gts_req->GtsChar = GtsChar;
#ifdef ENABLE_QUEUE_CAPACITY
	if (MAC_SUCCESS != qmm_queue_append(&nhle_mac_q, buffer_header)) {
		/*
		 * MLME-POLL.request is not appended into NHLE MAC
		 * queue, hence free the buffer allocated and return false
		 */
		bmm_buffer_free(buffer_header);
		return false;
	}

#else
	qmm_queue_append(&nhle_mac_q, buffer_header);
#endif  /* ENABLE_QUEUE_CAPACITY */
	return true;
#endif /* GTS_SUPPORT */
}

#endif /* MAC_GTS_REQUEST */
/* EOF */
