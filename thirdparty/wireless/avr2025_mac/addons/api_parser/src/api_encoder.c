/**
 * @file api_encoder.c
 *
 * @brief Encodes mac api's to comman byte stream
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
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

/**
 * \ingroup group_mac_api_parser
 * \defgroup group_mac_api_encoder MAC API Encoder
 * The API Encoder Module Encodes the MAC Api's into appropriate command byte
 * stream
 * @{
 */
#include <string.h>
#include "compiler.h"
#include "sio2ncp.h"
#include "api_parser.h"

/* Size constants for PHY PIB attributes */
static FLASH_DECLARE(uint8_t phy_pib_size[]) = {
	sizeof(uint8_t),            /* 0x00: phyCurrentChannel */
	sizeof(uint32_t),           /* 0x01: phyChannelsSupported */
	sizeof(uint8_t),            /* 0x02: phyTransmitPower */
	sizeof(uint8_t),            /* 0x03: phyCCAMode */
	sizeof(uint8_t),            /* 0x04: phyCurrentPage */
	sizeof(uint16_t),           /* 0x05: phyMaxFrameDuration */
	sizeof(uint8_t),            /* 0x06: phySHRDuration */
	sizeof(uint8_t)             /* 0x07: phySymbolsPerOctet */
};

/* Update this one the arry phy_pib_size is updated. */
#define MAX_PHY_PIB_ATTRIBUTE_ID            (phySymbolsPerOctet)

/* Size constants for MAC PIB attributes */
static FLASH_DECLARE(uint8_t mac_pib_size[]) = {
	sizeof(uint8_t),            /* 0x40: macAckWaitDuration */
	sizeof(uint8_t),            /* 0x41: macAssociationPermit */
	sizeof(uint8_t),            /* 0x42: macAutoRequest */
	sizeof(uint8_t),            /* 0x43: macBattLifeExt */
	sizeof(uint8_t),            /* 0x44: macBattLifeExtPeriods */
	sizeof(uint8_t),            /* 0x45: macBeaconPayload */
	sizeof(uint8_t),            /* 0x46: macBeaconPayloadLength */
	sizeof(uint8_t),            /* 0x47: macBeaconOrder */
	sizeof(uint32_t),           /* 0x48: macBeaconTxTime */
	sizeof(uint8_t),            /* 0x49: macBSN */
	sizeof(uint64_t),           /* 0x4A: macCoordExtendedAddress */
	sizeof(uint16_t),           /* 0x4B: macCoordShortAddress */
	sizeof(uint8_t),            /* 0x4C: macDSN */
	sizeof(uint8_t),            /* 0x4D: macGTSPermit */
	sizeof(uint8_t),            /* 0x4E: macMaxCSMAbackoffs */
	sizeof(uint8_t),            /* 0x4F: macMinBE */
	sizeof(uint16_t),           /* 0x50: macPANId */
	sizeof(uint8_t),            /* 0x51: macPromiscuousMode */
	sizeof(uint8_t),            /* 0x52: macRxOnWhenIdle */
	sizeof(uint16_t),           /* 0x53: macShortAddress */
	sizeof(uint8_t),            /* 0x54: macSuperframeOrder */
	sizeof(uint16_t),           /* 0x55: macTransactionPersistenceTime */
	sizeof(uint8_t),            /* 0x56: macAssociatedPANCoord */
	sizeof(uint8_t),            /* 0x57: macMaxBE */
	sizeof(uint16_t),           /* 0x58: macMaxFrameTotalWaitTime */
	sizeof(uint8_t),            /* 0x59: macMaxFrameRetries */
	sizeof(uint16_t),           /* 0x5A: macResponseWaitTime */
	sizeof(uint16_t),           /* 0x5B: macSyncSymbolOffset */
	sizeof(uint8_t),            /* 0x5C: macTimestampSupported */
	sizeof(uint8_t),            /* 0x5D: macSecurityEnabled */
	sizeof(uint8_t),            /* 0x5E: macMinLIFSPeriod */
	sizeof(uint8_t)             /* 0x5F: macMinSIFSPeriod */
};

/* Update this one the arry mac_pib_size is updated. */
#define MIN_MAC_PIB_ATTRIBUTE_ID            (macAckWaitDuration)
#define MAX_MAC_PIB_ATTRIBUTE_ID            (macMinSIFSPeriod)

/* Size constants for Private PIB attributes */
static FLASH_DECLARE(uint8_t private_pib_size[]) = {
	sizeof(uint64_t),            /* 0xF0: macIeeeAddress */
	0,                          /* 0xF1: Unused */
	0,                          /* 0xF2: Unused */
	sizeof(uint8_t),            /* 0xF3: macPrivateNoDataAfterAssocReq */
	sizeof(uint8_t),            /* 0xF4: macPrivateIllegalFrameType */
	0,                          /* 0xF5: Unused */
	sizeof(uint8_t),            /* 0xF6: macPrivateMACState */
	sizeof(uint8_t),            /* 0xF7: macPrivateVirtualPANs */
	sizeof(uint8_t)             /* 0xF8: macPrivateMACSyncState */
};

/* Update this one the arry private_pib_size is updated. */
#define MIN_PRIVATE_PIB_ATTRIBUTE_ID            (macIeeeAddress)

static uint8_t tx_buffer[TX_BUFFER_LENGTH] = {0};
static uint8_t *tx_buff_ptr = &tx_buffer[CMD_POS];

static uint8_t length = 0;

retval_t wpan_init(void)
{
	rcv_frame_ptr = &rcv_buffer[0] + 1;
	sio2ncp_init();
	tx_buffer[SOT_POS] = SOT;
	tx_buffer[PROTOCOL_ID_POS] = PROTOCOL_ID;
	return MAC_SUCCESS;
}

bool wpan_mcps_data_req(uint8_t SrcAddrMode,
		wpan_addr_spec_t *DstAddrSpec,
		uint8_t msduLength,
		uint8_t *msdu,
		uint8_t msduHandle,
		uint8_t TxOptions)
{
	length = 0;
	tx_buff_ptr = &tx_buffer[CMD_POS];

	*tx_buff_ptr++ = MCPS_DATA_REQUEST;
	*tx_buff_ptr++ = SrcAddrMode;
	*tx_buff_ptr++ = DstAddrSpec->AddrMode;
	*tx_buff_ptr++ = (uint8_t)DstAddrSpec->PANId;
	*tx_buff_ptr++ = (uint8_t)(DstAddrSpec->PANId >> 8);

	memcpy(tx_buff_ptr, &(DstAddrSpec->Addr).long_address,
			sizeof(uint64_t));
	tx_buff_ptr += sizeof(uint64_t);

	*tx_buff_ptr++ = msduHandle;
	*tx_buff_ptr++ = TxOptions;
	*tx_buff_ptr++ = msduLength;
	*tx_buff_ptr++ = msduLength;

	memcpy(tx_buff_ptr, msdu, msduLength);
	tx_buff_ptr += msduLength;

	*tx_buff_ptr++ = EOT;

	length = tx_buff_ptr - (uint8_t *)&tx_buffer[0];
	tx_buffer[LEN_POS] = length - 3;
	sio2ncp_tx(tx_buffer, length);
	return true;
}

#if ((MAC_PURGE_REQUEST_CONFIRM == 1) && (MAC_INDIRECT_DATA_BASIC == 1)) || \
	defined(DOXYGEN)
bool wpan_mcps_purge_req(const uint8_t msduHandle)
{
	length = 0;
	tx_buff_ptr = &tx_buffer[CMD_POS];

	*tx_buff_ptr++ = MCPS_PURGE_REQUEST;
	*tx_buff_ptr++ = msduHandle;
	*tx_buff_ptr++ = EOT;

	length = tx_buff_ptr - (uint8_t *)&tx_buffer[0];
	tx_buffer[LEN_POS] = length - 3;
	sio2ncp_tx(tx_buffer, length);
	return true;
}

#endif  /* ((MAC_PURGE_REQUEST_CONFIRM == 1) && (MAC_INDIRECT_DATA_BASIC == 1))
        **/

#if (MAC_ASSOCIATION_REQUEST_CONFIRM == 1) || defined(DOXYGEN)
bool wpan_mlme_associate_req(uint8_t LogicalChannel,
		uint8_t ChannelPage,
		wpan_addr_spec_t *CoordAddrSpec,
		uint8_t CapabilityInformation)
{
	length = 0;
	tx_buff_ptr = &tx_buffer[CMD_POS];

	*tx_buff_ptr++ = MLME_ASSOCIATE_REQUEST;
	*tx_buff_ptr++ = LogicalChannel;
	*tx_buff_ptr++ = ChannelPage;
	*tx_buff_ptr++ = CoordAddrSpec->AddrMode;
	*tx_buff_ptr++ = (uint8_t)CoordAddrSpec->PANId;
	*tx_buff_ptr++ = (uint8_t)(CoordAddrSpec->PANId >> 8);

	memcpy(tx_buff_ptr, &(CoordAddrSpec->Addr).long_address,
			sizeof(uint64_t));
	tx_buff_ptr += sizeof(uint64_t);

	*tx_buff_ptr++ = CapabilityInformation;
	*tx_buff_ptr++ = EOT;

	length = tx_buff_ptr - (uint8_t *)&tx_buffer[0];
	tx_buffer[LEN_POS] = length - 3;
	sio2ncp_tx(tx_buffer, length);
	return true;
}

#endif  /* (MAC_ASSOCIATION_REQUEST_CONFIRM == 1) */

#if (MAC_ASSOCIATION_INDICATION_RESPONSE == 1) || defined(DOXYGEN)
bool wpan_mlme_associate_resp(uint64_t DeviceAddress,
		uint16_t AssocShortAddress,
		uint8_t status)
{
	length = 0;
	tx_buff_ptr = &tx_buffer[CMD_POS];

	*tx_buff_ptr++ = MLME_ASSOCIATE_RESPONSE;

	memcpy(tx_buff_ptr, &DeviceAddress, sizeof(uint64_t));
	tx_buff_ptr += sizeof(uint64_t);

	*tx_buff_ptr++ = (uint8_t)AssocShortAddress;
	*tx_buff_ptr++ = (uint8_t)(AssocShortAddress >> 8);
	*tx_buff_ptr++ = status;
	*tx_buff_ptr++ = EOT;

	length = tx_buff_ptr - (uint8_t *)&tx_buffer[0];
	tx_buffer[LEN_POS] = length - 3;
	sio2ncp_tx(tx_buffer, length);
	return true;
}

#endif  /* (MAC_ASSOCIATION_INDICATION_RESPONSE == 1) */

#if (MAC_DISASSOCIATION_BASIC_SUPPORT == 1) || defined(DOXYGEN)
bool wpan_mlme_disassociate_req(wpan_addr_spec_t *DeviceAddrSpec,
		uint8_t DisassociateReason,
		bool TxIndirect)
{
	length = 0;
	tx_buff_ptr = &tx_buffer[CMD_POS];

	*tx_buff_ptr++ = MLME_DISASSOCIATE_REQUEST;
	*tx_buff_ptr++ = DeviceAddrSpec->AddrMode;
	*tx_buff_ptr++ = (uint8_t)DeviceAddrSpec->PANId;
	*tx_buff_ptr++ = (uint8_t)(DeviceAddrSpec->PANId >> 8);

	memcpy(tx_buff_ptr, &((DeviceAddrSpec->Addr).long_address),
			sizeof(uint64_t));
	tx_buff_ptr += sizeof(uint64_t);

	*tx_buff_ptr++ = DisassociateReason;
	*tx_buff_ptr++ = TxIndirect;
	*tx_buff_ptr++ = EOT;

	length = tx_buff_ptr - (uint8_t *)&tx_buffer[0];
	tx_buffer[LEN_POS] = length - 3;
	sio2ncp_tx(tx_buffer, length);
	return true;
}

#endif  /* (MAC_DISASSOCIATION_BASIC_SUPPORT == 1) */

#if (MAC_GET_SUPPORT == 1) || defined(DOXYGEN)
bool wpan_mlme_get_req(uint8_t PIBAttribute)
{
	length = 0;
	tx_buff_ptr = &tx_buffer[CMD_POS];

	*tx_buff_ptr++ = MLME_GET_REQUEST;
	*tx_buff_ptr++ = PIBAttribute;
	*tx_buff_ptr++ = EOT;

	length = tx_buff_ptr - (uint8_t *)&tx_buffer[0];
	tx_buffer[LEN_POS] = length - 3;
	sio2ncp_tx(tx_buffer, length);
	return true;
}

#endif  /* (MAC_GET_SUPPORT == 1) */

#if (MAC_ORPHAN_INDICATION_RESPONSE == 1) || defined(DOXYGEN)
bool wpan_mlme_orphan_resp(uint64_t OrphanAddress,
		uint16_t ShortAddress,
		bool AssociatedMember)
{
	length = 0;
	tx_buff_ptr = &tx_buffer[CMD_POS];

	*tx_buff_ptr++ = MLME_ORPHAN_RESPONSE;

	memcpy(tx_buff_ptr, &OrphanAddress, sizeof(uint64_t));
	tx_buff_ptr += sizeof(uint64_t);

	*tx_buff_ptr++ = (uint8_t)ShortAddress;
	*tx_buff_ptr++ = (uint8_t)(ShortAddress >> 8);
	*tx_buff_ptr++ = AssociatedMember;
	*tx_buff_ptr++ = EOT;

	length = tx_buff_ptr - (uint8_t *)&tx_buffer[0];
	tx_buffer[LEN_POS] = length - 3;
	sio2ncp_tx(tx_buffer, length);
	return true;
}

#endif  /* (MAC_ORPHAN_INDICATION_RESPONSE == 1) */

#if (MAC_INDIRECT_DATA_BASIC == 1) || defined(DOXYGEN)
bool wpan_mlme_poll_req(wpan_addr_spec_t *CoordAddrSpec)
{
	length = 0;
	tx_buff_ptr = &tx_buffer[CMD_POS];

	*tx_buff_ptr++ = MLME_POLL_REQUEST;
	*tx_buff_ptr++ = CoordAddrSpec->AddrMode;
	*tx_buff_ptr++ = (uint8_t)CoordAddrSpec->PANId;
	*tx_buff_ptr++ = (uint8_t)(CoordAddrSpec->PANId >> 8);

	memcpy(tx_buff_ptr, &(CoordAddrSpec->Addr).long_address,
			sizeof(uint64_t));
	tx_buff_ptr += sizeof(uint64_t);

	*tx_buff_ptr++ = EOT;

	length = tx_buff_ptr - (uint8_t *)&tx_buffer[0];
	tx_buffer[LEN_POS] = length - 3;
	sio2ncp_tx(tx_buffer, length);
	return true;
}

#endif  /* (MAC_INDIRECT_DATA_BASIC == 1) */

bool wpan_mlme_reset_req(bool SetDefaultPib)
{
	length = 0;
	tx_buff_ptr = &tx_buffer[CMD_POS];

	*tx_buff_ptr++ = MLME_RESET_REQUEST;
	*tx_buff_ptr++ = SetDefaultPib;
	*tx_buff_ptr++ = EOT;

	length = tx_buff_ptr - (uint8_t *)&tx_buffer[0];
	tx_buffer[LEN_POS] = length - 3;
	sio2ncp_tx(tx_buffer, length);
	return true;
}

bool wpan_mlme_set_req(uint8_t PIBAttribute,
		void *PIBAttributeValue)
{
	uint8_t pib_len;
	if (macBeaconPayload == PIBAttribute) {
		pib_len = *((uint8_t *)PIBAttributeValue - 1);
	} else {
		pib_len = mac_get_pib_attribute_size(PIBAttribute);
	}

	length = 0;
	tx_buff_ptr = &tx_buffer[CMD_POS];

	*tx_buff_ptr++ = MLME_SET_REQUEST;
	*tx_buff_ptr++ = PIBAttribute;

	*tx_buff_ptr++ = pib_len;
	memcpy(tx_buff_ptr, (uint8_t *)PIBAttributeValue, pib_len);
	tx_buff_ptr += pib_len;

	*tx_buff_ptr++ = EOT;

	length = tx_buff_ptr - (uint8_t *)&tx_buffer[0];
	tx_buffer[LEN_POS] = length - 3;
	sio2ncp_tx(tx_buffer, length);
	return true;
}

#if (MAC_RX_ENABLE_SUPPORT == 1) || defined(DOXYGEN)
bool wpan_mlme_rx_enable_req(bool DeferPermit,
		uint32_t RxOnTime,
		uint32_t RxOnDuration)
{
	length = 0;
	tx_buff_ptr = &tx_buffer[CMD_POS];

	*tx_buff_ptr++ = MLME_RX_ENABLE_REQUEST;
	*tx_buff_ptr++ = DeferPermit;

	memcpy(tx_buff_ptr, &RxOnTime, sizeof(uint32_t));
	tx_buff_ptr += sizeof(uint32_t);

	memcpy(tx_buff_ptr, &RxOnDuration, sizeof(uint32_t));
	tx_buff_ptr += sizeof(uint32_t);

	*tx_buff_ptr++ = EOT;

	length = tx_buff_ptr - (uint8_t *)&tx_buffer[0];
	tx_buffer[LEN_POS] = length - 3;
	sio2ncp_tx(tx_buffer, length);
	return true;
}

#endif  /* (MAC_RX_ENABLE_SUPPORT == 1) */

#if ((MAC_SCAN_ED_REQUEST_CONFIRM == 1)      ||	\
	(MAC_SCAN_ACTIVE_REQUEST_CONFIRM == 1)  || \
	(MAC_SCAN_PASSIVE_REQUEST_CONFIRM == 1) || \
	(MAC_SCAN_ORPHAN_REQUEST_CONFIRM == 1)) || defined(DOXYGEN)
bool wpan_mlme_scan_req(uint8_t ScanType,
		uint32_t ScanChannels,
		uint8_t ScanDuration,
		uint8_t ChannelPage)
{
	length = 0;
	tx_buff_ptr = &tx_buffer[CMD_POS];

	*tx_buff_ptr++ = MLME_SCAN_REQUEST;
	*tx_buff_ptr++ = ScanType;

	memcpy(tx_buff_ptr, &ScanChannels, sizeof(uint32_t));
	tx_buff_ptr += sizeof(uint32_t);

	*tx_buff_ptr++ = ScanDuration;
	*tx_buff_ptr++ = ChannelPage;
	*tx_buff_ptr++ = EOT;

	length = tx_buff_ptr - (uint8_t *)&tx_buffer[0];
	tx_buffer[LEN_POS] = length - 3;
	sio2ncp_tx(tx_buffer, length);
	return true;
}

#endif

#if (MAC_START_REQUEST_CONFIRM == 1) || defined(DOXYGEN)
bool wpan_mlme_start_req(uint16_t PANId,
		uint8_t LogicalChannel,
		uint8_t ChannelPage,
		uint8_t BeaconOrder,
		uint8_t SuperframeOrder,
		bool PANCoordinator,
		bool BatteryLifeExtension,
		bool CoordRealignment)
{
	length = 0;
	tx_buff_ptr = &tx_buffer[CMD_POS];

	*tx_buff_ptr++ = MLME_START_REQUEST;
	*tx_buff_ptr++ = (uint8_t)PANId;
	*tx_buff_ptr++ = (uint8_t)(PANId >> 8);
	*tx_buff_ptr++ = LogicalChannel;
	*tx_buff_ptr++ = ChannelPage;
	*tx_buff_ptr++ = BeaconOrder;
	*tx_buff_ptr++ = SuperframeOrder;
	*tx_buff_ptr++ = PANCoordinator;
	*tx_buff_ptr++ = BatteryLifeExtension;
	*tx_buff_ptr++ = CoordRealignment;

	*tx_buff_ptr++ = EOT;

	length = tx_buff_ptr - (uint8_t *)&tx_buffer[0];
	tx_buffer[LEN_POS] = length - 3;
	sio2ncp_tx(tx_buffer, length);
	return true;
}

#endif  /* (MAC_START_REQUEST_CONFIRM == 1) */

#if (MAC_SYNC_REQUEST == 1) || defined(DOXYGEN)
bool wpan_mlme_sync_req(uint8_t LogicalChannel,
		uint8_t ChannelPage,
		bool TrackBeacon)
{
	length = 0;
	tx_buff_ptr = &tx_buffer[CMD_POS];

	*tx_buff_ptr++ = MLME_SYNC_REQUEST;
	*tx_buff_ptr++ = LogicalChannel;
	*tx_buff_ptr++ = ChannelPage;
	*tx_buff_ptr++ = TrackBeacon;
	*tx_buff_ptr++ = EOT;

	length = tx_buff_ptr - (uint8_t *)&tx_buffer[0];
	tx_buffer[LEN_POS] = length - 3;
	sio2ncp_tx(tx_buffer, length);
	return true;
}

#endif /* (MAC_SYNC_REQUEST == 1) */

#ifdef GTS_SUPPORT
#if (MAC_GTS_REQUEST == 1) || defined(__DOXYGEN__)
bool wpan_mlme_gts_req(uint16_t DevShortAddr, gts_char_t GtsChar)
{
	length = 0;
	tx_buff_ptr = &tx_buffer[CMD_POS];

	*tx_buff_ptr++ = MLME_GTS_REQUEST;
	*tx_buff_ptr++ = (uint8_t)DevShortAddr;
	*tx_buff_ptr++ = (uint8_t)DevShortAddr >> 8;
	*tx_buff_ptr++ = *((uint8_t *)&GtsChar);
	*tx_buff_ptr++ = EOT;

	length = tx_buff_ptr - (uint8_t *)&tx_buffer[0];
	tx_buffer[LEN_POS] = length - 3;
	sio2ncp_tx(tx_buffer, length);
	return true;
}

#endif /* (MAC_GTS_REQUEST == 1) */
#endif /* GTS_SUPPORT */

uint8_t mac_get_pib_attribute_size(uint8_t pib_attribute_id)
{
#if (MAC_START_REQUEST_CONFIRM == 1)

	/*
	 * Since the current length of the beacon payload is not a contant, but
	 * a variable, it cannot be stored in a Flash table. Therefore we need
	 * to handle this PIB attribute special.
	 */
	if (macBeaconPayload == pib_attribute_id) {
		return (*(rcv_frame_ptr + 4));
	}
#endif  /* (MAC_START_REQUEST_CONFIRM == 1) */

	if (MAX_PHY_PIB_ATTRIBUTE_ID >= pib_attribute_id) {
		return (PGM_READ_BYTE(&phy_pib_size[pib_attribute_id]));
	}

	if (MIN_MAC_PIB_ATTRIBUTE_ID <= pib_attribute_id &&
			MAX_MAC_PIB_ATTRIBUTE_ID >= pib_attribute_id) {
		return(PGM_READ_BYTE(&mac_pib_size[pib_attribute_id -
		       MIN_MAC_PIB_ATTRIBUTE_ID]));
	}

	if (MIN_PRIVATE_PIB_ATTRIBUTE_ID <= pib_attribute_id) {
		return(PGM_READ_BYTE(&private_pib_size[pib_attribute_id -
		       MIN_PRIVATE_PIB_ATTRIBUTE_ID]));
	}

	return(0);
}

/* ! @} */
/* EOF */
