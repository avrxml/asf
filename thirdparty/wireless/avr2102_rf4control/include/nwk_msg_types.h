/**
 * @file nwk_msg_types.h
 *
 * @brief This file defines all message structures for the RF4control stack.
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

/* Prevent double inclusion */
#ifndef NWK_MSG_TYPES_H
#define NWK_MSG_TYPES_H

/* === Includes ============================================================= */

#include <stdint.h>
#include "nwk_msg_code.h"
#include "rf4ce.h"
#include "nwk_config.h"

/* === Macros =============================================================== */

/* === Types ================================================================ */

/* Start pack the data  types, which is unaligned in some of ARM architecture */
__PACK__DATA__

/**
 * \addtogroup group_rf4control_ds
 * @{
 */

typedef union {
	/** NIB Attribute Bool */
	bool nib_value_bool;
	/** NIB Attribute 8-bit */
	uint8_t nib_value_8bit;
	/** NIB Attribute 16-bit */
	uint16_t nib_value_16bit;
	/** NIB Attribute 32-bit */
	uint32_t nib_value_32bit;
	/** NIB Attribute 64-bit */
	uint64_t nib_value_64bit;
} nib_value_t;

/**
 * @brief NLDE-DATA.request message structure.
 */
typedef struct nlde_data_req_tag {
	/** This identifies the message as @ref NLDE_DATA_REQUEST */
	nwk_msg_code_t cmdcode ALIGN8BIT;
	uint8_t PairingRef ALIGN8BIT;
	profile_id_t ProfileId ALIGN8BIT;
	uint16_t VendorId ALIGN8BIT;
	uint8_t nsduLength ALIGN8BIT;
	uint8_t TxOptions ALIGN8BIT;
	uint8_t Handle ALIGN8BIT;
	FUNC_PTR confirm_cb ALIGN8BIT;
	uint8_t nsdu[1]             ALIGN8BIT;
} nlde_data_req_t;

/**
 * @brief NLDE-DATA.confirm message structure.
 */
typedef struct nlde_data_conf_tag {
	/** This identifies the message as @ref NLDE_DATA_CONFIRM */
	nwk_msg_code_t cmdcode ALIGN8BIT;
	nwk_enum_t Status ALIGN8BIT;
	uint8_t PairingRef ALIGN8BIT;
	profile_id_t ProfileId ALIGN8BIT;
	uint8_t Handle ALIGN8BIT;
	FUNC_PTR confirm_cb ALIGN8BIT;
} nlde_data_conf_t;

/**
 * @brief NLDE-DATA.indication message structure.
 */
typedef struct nlde_data_ind_tag {
	/** This identifies the message as @ref NLDE_DATA_INDICATION */
	nwk_msg_code_t cmdcode ALIGN8BIT;
	uint8_t PairingRef ALIGN8BIT;
	profile_id_t ProfileId ALIGN8BIT;
	uint16_t VendorId ALIGN8BIT;
	uint8_t nsduLength ALIGN8BIT;
	uint8_t RxLinkQuality ALIGN8BIT;
	uint8_t RxFlags ALIGN8BIT;
	uint8_t nsdu[1]             ALIGN8BIT;
} nlde_data_ind_t;

/**
 * @brief NLME-GET.request message structure.
 */
typedef struct nlme_get_req_tag {
	/** This identifies the message as @ref NLME_GET_REQUEST */
	nwk_msg_code_t cmdcode ALIGN8BIT;
	/** The identifier of the MAC PIB attribute to get. */
	nib_attribute_t NIBAttribute ALIGN8BIT;
	uint8_t NIBAttributeIndex ALIGN8BIT;
} nlme_get_req_t;

/**
 * @brief NLME-GET.confirm message structure.
 */
typedef struct nlme_get_conf_tag {
	/** This identifies the message as @ref NLME_GET_CONFIRM */
	nwk_msg_code_t cmdcode ALIGN8BIT;
	/** The result of the request for PHY PIB attribute information.*/
	nwk_enum_t Status ALIGN8BIT;
	/** The identifier of the MAC PIB attribute to get. */
	nib_attribute_t NIBAttribute ALIGN8BIT;
	uint8_t NIBAttributeIndex ALIGN8BIT;
	/** The value of the indicated MAC PIB attribute that was read. */
	nib_value_t NIBAttributeValue;
} nlme_get_conf_t;

/**
 * @brief NLME-SET.request message structure.
 */
typedef struct nlme_set_req_tag {
	/** This identifies the message as @ref NLME_SET_REQUEST */
	nwk_msg_code_t cmdcode ALIGN8BIT;
	/** The identifier of the MAC PIB attribute to set. */
	nib_attribute_t NIBAttribute ALIGN8BIT;
	uint8_t NIBAttributeIndex ALIGN8BIT;
	/** The value to write to the indicated MAC PIB attribute. */
	uint8_t value[1]      ALIGN8BIT;
} nlme_set_req_t;

/**
 * @brief NLME-SET.confirm message structure.
 */
typedef struct nlme_set_conf_tag {
	/** This identifies the message as @ref NLME_SET_CONFIRM */
	nwk_msg_code_t cmdcode ALIGN8BIT;
	/** The result of the request to write the MAC PIB attribute. */
	nwk_enum_t Status ALIGN8BIT;
	/** The identifier of the MAC PIB attribute that was written. */
	nib_attribute_t NIBAttribute ALIGN8BIT;
	uint8_t NIBAttributeIndex ALIGN8BIT;
} nlme_set_conf_t;

/**
 * @brief NLME-RESET.request message structure.
 */
typedef struct nlme_reset_req_tag {
	/** This identifies the message as @ref NLME_RESET_REQUEST */
	nwk_msg_code_t cmdcode ALIGN8BIT;

	/**
	 * If TRUE, the NWK sublayer is reset and all NIB attributes are set to
	 * their default values. If FALSE, the NWK sublayer is reset but all NIB
	 * attributes retain their values prior to the generation of the
	 * NLME-RESET.request primitive.
	 */
	uint8_t SetDefaultNIB ALIGN8BIT;
} nlme_reset_req_t;

/**
 * @brief NLME-RESET.confirm message structure.
 */
typedef struct nlme_reset_conf_tag {
	/** This identifies the message as @ref NLME_RESET_CONFIRM */
	nwk_msg_code_t cmdcode ALIGN8BIT;
	/** The result of the reset operation. */
	uint8_t Status ALIGN8BIT;
} nlme_reset_conf_t;

/**
 * @brief NLME-START.request message structure.
 */
typedef struct nlme_start_req_tag {
	/** This identifies the message as @ref NLME_START_REQUEST */
	nwk_msg_code_t cmdcode ALIGN8BIT;
} nlme_start_req_t;

/**
 * @brief NLME-START.confirm message structure.
 */
typedef struct nlme_start_conf_tag {
	/** This identifies the message as @ref NLME_START_CONFIRM */
	nwk_msg_code_t cmdcode ALIGN8BIT;
	/** The result of the start operation. */
	uint8_t Status ALIGN8BIT;
} nlme_start_conf_t;

/**
 * @brief NLME-DISCOVERY.request message structure.
 */
typedef struct nlme_disc_req_tag {
	/** This identifies the message as @ref NLME_DISCOVERY_REQUEST */
	nwk_msg_code_t cmdcode ALIGN8BIT;
	uint16_t DstPANId ALIGN8BIT;
	uint16_t DstNwkAddr ALIGN8BIT;
	uint8_t OrgAppCapabilities ALIGN8BIT;
	dev_type_t OrgDevTypeList[DEVICE_TYPE_LIST_SIZE]   ALIGN8BIT;
	profile_id_t OrgProfileIdList[PROFILE_ID_LIST_SIZE] ALIGN8BIT;
	dev_type_t SearchDevType ALIGN8BIT;
	uint8_t DiscProfileIdListSize ALIGN8BIT;
	profile_id_t DiscProfileIdList[PROFILE_ID_LIST_SIZE] ALIGN8BIT;
	uint32_t DiscDuration ALIGN8BIT;
} nlme_disc_req_t;

/**
 * @brief NLME-DISCOVERY.confirm message structure.
 */
typedef struct nlme_disc_conf_tag {
	/** This identifies the message as @ref NLME_DISCOVERY_CONFIRM */
	nwk_msg_code_t cmdcode ALIGN8BIT;
	nwk_enum_t Status ALIGN8BIT;
	uint8_t NumNodes ALIGN8BIT;
	node_desc_t NodeDescList[3]   ALIGN8BIT;
	/* 3 = (LARGE_BUFFER_SIZE - 3) / sizeof(node_desc_t) */
} nlme_disc_conf_t;

/**
 * @brief NLME-DISCOVERY.response message structure.
 */
typedef struct nlme_disc_resp_tag {
	/** This identifies the message as @ref NLME_DISCOVERY_RESPONSE */
	nwk_msg_code_t cmdcode ALIGN8BIT;
	nwk_enum_t Status ALIGN8BIT;
	uint64_t DstIEEEAddr ALIGN8BIT;
	uint8_t RecAppCapabilities ALIGN8BIT;
	dev_type_t RecDevTypeList[DEVICE_TYPE_LIST_SIZE]   ALIGN8BIT;
	profile_id_t RecProfileIdList[PROFILE_ID_LIST_SIZE]  ALIGN8BIT;
	uint8_t DiscReqLQI ALIGN8BIT;
} nlme_disc_resp_t;

/**
 * @brief NLME-DISCOVERY.indication message structure.
 */
typedef struct nlme_disc_ind_tag {
	/** This identifies the message as @ref NLME_DISCOVERY_INDICATION */
	nwk_msg_code_t cmdcode ALIGN8BIT;
	nwk_enum_t Status ALIGN8BIT;
	uint64_t SrcIEEEAddr ALIGN8BIT;
	uint8_t OrgNodeCapabilities ALIGN8BIT;
	uint16_t OrgVendorId ALIGN8BIT;
	uint8_t OrgVendorString[7]  ALIGN8BIT;
	uint8_t OrgAppCapabilities ALIGN8BIT;
	uint8_t OrgUserString[15]   ALIGN8BIT;
	dev_type_t OrgDevTypeList[DEVICE_TYPE_LIST_SIZE]   ALIGN8BIT;
	profile_id_t OrgProfileIdList[PROFILE_ID_LIST_SIZE]  ALIGN8BIT;
	uint8_t SearchDevType ALIGN8BIT;
	uint8_t RxLinkQuality ALIGN8BIT;
} nlme_disc_ind_t;

/**
 * @brief NLME-RX-ENABLE.request message structure.
 */
typedef struct nlme_rx_enable_req_tag {
	/** This identifies the message as @ref NLME_RX_ENABLE_REQUEST */
	nwk_msg_code_t cmdcode ALIGN8BIT;
	uint32_t RxOnDuration ALIGN8BIT;
} nlme_rx_enable_req_t;

/**
 * @brief NLME-RX-ENABLE.confirm message structure.
 */
typedef struct nlme_rx_enable_conf_tag {
	/** This identifies the message as @ref NLME_RX_ENABLE_CONFIRM */
	nwk_msg_code_t cmdcode ALIGN8BIT;
	nwk_enum_t Status ALIGN8BIT;
} nlme_rx_enable_conf_t;

/**
 * @brief NLME-PAIR.request message structure.
 */
typedef struct nlme_pair_req_tag {
	/** This identifies the message as @ref NLME_PAIR_REQUEST */
	nwk_msg_code_t cmdcode ALIGN8BIT;
	uint8_t LogicalChannel ALIGN8BIT;
	uint16_t DstPANId ALIGN8BIT;
	uint64_t DstIEEEAddr ALIGN8BIT;
	uint8_t OrgAppCapabilities ALIGN8BIT;
	dev_type_t OrgDevTypeList[DEVICE_TYPE_LIST_SIZE]   ALIGN8BIT;
	profile_id_t OrgProfileIdList[PROFILE_ID_LIST_SIZE]  ALIGN8BIT;
	uint8_t KeyExTransferCount ALIGN8BIT;
} nlme_pair_req_t;

/**
 * @brief NLME-PAIR.indication message structure.
 */
typedef struct nlme_pair_ind_tag {
	/** This identifies the message as @ref NLME_PAIR_INDICATION */
	nwk_msg_code_t cmdcode ALIGN8BIT;
	nwk_enum_t Status ALIGN8BIT;
	uint16_t SrcPANId ALIGN8BIT;
	uint64_t SrcIEEEAddr ALIGN8BIT;
	uint8_t OrgNodeCapabilities ALIGN8BIT;
	uint16_t OrgVendorId ALIGN8BIT;
	uint8_t OrgVendorString[7]  ALIGN8BIT;
	uint8_t OrgAppCapabilities ALIGN8BIT;
	uint8_t OrgUserString[15]   ALIGN8BIT;
	dev_type_t OrgDevTypeList[DEVICE_TYPE_LIST_SIZE]   ALIGN8BIT;
	profile_id_t OrgProfileIdList[PROFILE_ID_LIST_SIZE]  ALIGN8BIT;
	uint8_t KeyExTransferCount ALIGN8BIT;
	uint8_t ProvPairingRef ALIGN8BIT;
} nlme_pair_ind_t;

/**
 * @brief NLME-PAIR.response message structure.
 */
typedef struct nlme_pair_resp_tag {
	/** This identifies the message as @ref NLME_PAIR_RESPONSE */
	nwk_msg_code_t cmdcode ALIGN8BIT;
	nwk_enum_t Status ALIGN8BIT;
	uint16_t DstPANId ALIGN8BIT;
	uint64_t DstIEEEAddr ALIGN8BIT;
	uint8_t RecAppCapabilities ALIGN8BIT;
	dev_type_t RecDevTypeList[DEVICE_TYPE_LIST_SIZE]   ALIGN8BIT;
	profile_id_t RecProfileIdList[PROFILE_ID_LIST_SIZE]  ALIGN8BIT;
	uint8_t ProvPairingRef ALIGN8BIT;
} nlme_pair_resp_t;

/**
 * @brief NLME-PAIR.confirm message structure.
 */
typedef struct nlme_pair_conf_tag {
	/** This identifies the message as @ref NLME_PAIR_CONFIRM */
	nwk_msg_code_t cmdcode ALIGN8BIT;
	nwk_enum_t Status ALIGN8BIT;
	uint8_t PairingRef ALIGN8BIT;
	uint16_t RecVendorId ALIGN8BIT;
	uint8_t RecVendorString[7]  ALIGN8BIT;
	uint8_t RecAppCapabilities ALIGN8BIT;
	uint8_t RecUserString[15]   ALIGN8BIT;
	dev_type_t RecDevTypeList[DEVICE_TYPE_LIST_SIZE]   ALIGN8BIT;
	profile_id_t RecProfileIdList[PROFILE_ID_LIST_SIZE]  ALIGN8BIT;
} nlme_pair_conf_t;

/**
 * @brief NLME-COMM-STATUS.indication message structure.
 */
typedef struct nlme_comm_status_ind_tag {
	/** This identifies the message as @ref NLME_COMM_STATUS_INDICATION */
	nwk_msg_code_t cmdcode ALIGN8BIT;
	nwk_enum_t Status ALIGN8BIT;
	uint8_t PairingRef ALIGN8BIT;
	uint16_t DstPANId ALIGN8BIT;
	uint8_t DstAddrMode ALIGN8BIT;
	uint64_t DstAddr ALIGN8BIT;
} nlme_comm_status_ind_t;

/**
 * @brief NLME-UNPAIR.request message structure.
 */
typedef struct nlme_unpair_req_tag {
	/** This identifies the message as @ref NLME_UNPAIR_REQUEST */
	nwk_msg_code_t cmdcode ALIGN8BIT;
	uint8_t PairingRef ALIGN8BIT;
} nlme_unpair_req_t;

/**
 * @brief NLME-UNPAIR.indication message structure.
 */
typedef struct nlme_unpair_ind_tag {
	/** This identifies the message as @ref NLME_UNPAIR_INDICATION */
	nwk_msg_code_t cmdcode ALIGN8BIT;
	uint8_t PairingRef ALIGN8BIT;
} nlme_unpair_ind_t;

/**
 * @brief NLME-UNPAIR.response message structure.
 */
typedef struct nlme_unpair_resp_tag {
	/** This identifies the message as @ref NLME_UNPAIR_RESPONSE */
	nwk_msg_code_t cmdcode ALIGN8BIT;
	uint8_t PairingRef ALIGN8BIT;
} nlme_unpair_resp_t;

/**
 * @brief NLME-UNPAIR.confirm message structure.
 */
typedef struct nlme_unpair_conf_tag {
	/** This identifies the message as @ref NLME_UNPAIR_CONFIRM */
	nwk_msg_code_t cmdcode ALIGN8BIT;
	uint8_t Status ALIGN8BIT;
	uint8_t PairingRef ALIGN8BIT;
} nlme_unpair_conf_t;

/**
 * @brief NLME-UPDATE-KEY.request message structure.
 */
typedef struct nlme_update_key_req_tag {
	/** This identifies the message as @ref NLME_UPDATE_KEY_REQUEST */
	nwk_msg_code_t cmdcode ALIGN8BIT;
	uint8_t PairingRef ALIGN8BIT;
	uint8_t NewLinkKey[16]      ALIGN8BIT;
} nlme_update_key_req_t;

/**
 * @brief NLME-UPDATE-KEY.confirm message structure.
 */
typedef struct nlme_update_key_conf_tag {
	/** This identifies the message as @ref NLME_UPDATE_KEY_CONFIRM */
	nwk_msg_code_t cmdcode ALIGN8BIT;
	nwk_enum_t Status ALIGN8BIT;
	uint8_t PairingRef ALIGN8BIT;
} nlme_update_key_conf_t;

/**
 * @brief NLME-AUTO-DISCOVERY.request message structure.
 */
typedef struct nlme_auto_disc_req_tag {
	/** This identifies the message as @ref NLME_AUTO_DISCOVERY_REQUEST */
	nwk_msg_code_t cmdcode ALIGN8BIT;
	uint8_t RecAppCapabilities ALIGN8BIT;
	uint8_t RecDevTypeList[DEVICE_TYPE_LIST_SIZE]   ALIGN8BIT;
	profile_id_t RecProfileIdList[PROFILE_ID_LIST_SIZE]  ALIGN8BIT;
	uint32_t AutoDiscDuration ALIGN8BIT;
} nlme_auto_disc_req_t;

/**
 * @brief NLME-AUTO-DISCOVERY.confirm message structure.
 */
typedef struct nlme_auto_disc_conf_tag {
	/** This identifies the message as @ref NLME_AUTO_DISCOVERY_CONFIRM */
	nwk_msg_code_t cmdcode ALIGN8BIT;
	nwk_enum_t Status ALIGN8BIT;
	uint64_t SrcIEEEAddr ALIGN8BIT;
} nlme_auto_disc_conf_t;

/**
 * @brief NWK_CH_AGILITY.request message structure.
 */
typedef struct nwk_ch_agility_req_tag {
	/** This identifies the message as @ref NWK_CH_AGILITY_REQUEST */
	nwk_msg_code_t cmdcode ALIGN8BIT;
	nwk_agility_mode_t AgilityMode ALIGN8BIT;
} nwk_ch_agility_req_t;

/**
 * @brief NWK_CH_AGILITY.indication message structure.
 */
typedef struct nwk_ch_agility_ind_tag {
	/** This identifies the message as @ref NWK_CH_AGILITY_INDICATION */
	nwk_msg_code_t cmdcode ALIGN8BIT;
	uint8_t LogicalChannel ALIGN8BIT;
} nwk_ch_agility_ind_t;

/**
 * @brief NWK_CH_AGILITY.confirm message structure.
 */
typedef struct nwk_ch_agility_conf_tag {
	/** This identifies the message as @ref NWK_CH_AGILITY_CONFIRM */
	nwk_msg_code_t cmdcode ALIGN8BIT;
	nwk_enum_t Status ALIGN8BIT;
	bool ChannelChanged ALIGN8BIT;
	uint8_t LogicalChannel ALIGN8BIT;
} nwk_ch_agility_conf_t;

__PACK__RST_DATA__
/* Packing  of the data types ends here */

#endif /* NWK_MSG_TYPES_H */
