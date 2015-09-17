/**
 * @file mac_msg_types.h
 *
 * @brief This file defines all message structures for the MAC.
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

/* Prevent double inclusion */
#ifndef MAC_MSG_TYPES_H
#define MAC_MSG_TYPES_H

/* === Includes ============================================================= */

#include "mac_msg_const.h"
#include "mac_api.h"
#include "mac_msg_type_data_ind.h"
#include "tal_generic.h"
/* === Macros =============================================================== */

/* === Types ================================================================ */

/**
 * \addtogroup group_mac_ds
 * @{
 */

/**
 * @brief Union to force alignment of variable portion of scan confirm
 *        with wpan_pandescriptor_t
 *
 * In order to allow proper alignment of members of the PAN descriptor
 * list in the scan confirm for non 8-bit platforms (such as ARM),
 * the variable portion of scan confirm at the end of the primitive
 * needs to be aligned to at list of wpan_pandescriptor_t.
 * This has no impact for 8-bit platforms.
 */
__PACK__DATA__
typedef union {
	uint8_t ed_value[1];
	wpan_pandescriptor_t wpan_pan_desc;
} scan_result_list_t;

/* === MCPS-SAP messages ==================================================== */

/**
 * @brief This is the MCPS-DATA.request message structure.
 */
typedef struct mcps_data_req_tag {
	/**< This identifies the message as \ref MCPS_DATA_REQUEST */
	enum msg_code cmdcode;

	/**
	 * The source addressing mode for this primitive and subsequent MPDU.
	 * This
	 * value can take one of the following values: 0 x 00 = no address
	 * (addressingfields omitted). 0 x 01 = reserved. 0 x 02 = 16 bit short
	 *  address. 0 x 03 = 64 bit extended address.
	 */
	uint8_t SrcAddrMode;

	/**
	 * The destination addressing mode for this primitive and subsequent
	 * MPDU.
	 * This value can take one of the following values: 0 x 00 = no address
	 * (addressing fields omitted). 0 x 01 = reserved. 0 x 02 = 16 bit short
	 * address. 0 x 03 = 64 bit extended address.
	 */
	uint8_t DstAddrMode;

	/**
	 * The 16 bit PAN identifier of the entity to which the MSDU is being
	 * transferred.
	 */
	uint16_t DstPANId;

	/**
	 * The individual device address of the entity to which the MSDU is
	 * being transferred.
	 */
	uint64_t DstAddr;

	/**
	 * The handle associated with the MSDU to be transmitted by the MAC
	 * sublayer entity.
	 */
	uint8_t msduHandle;

	/**
	 * The transmission options for this MSDU. These are a bitwise OR of one
	 * or
	 * more of the following:
	 * 0 x 01 = acknowledged transmission.
	 * 0 x 04 = indirect transmission.
	 */
	uint8_t TxOptions;
#if ((defined MAC_SECURITY_ZIP)  || (defined MAC_SECURITY_2006))

	/**
	 * The security level to be used.
	 */
	uint8_t SecurityLevel;

	/**
	 * The mode used to identify the key to be used.
	 */
	uint8_t KeyIdMode;

	/**
	 * The KeySource to find the key.
	 */
	uint8_t *KeySource;

	/**
	 * The index of the key to be used.
	 */
	uint8_t KeyIndex;
#endif  /* ((MAC_SECURITY_ZIP)  || (MAC_SECURITY_2006)) */

	/**
	 * The number of octets contained in the MSDU to be transmitted by the
	 * MAC sublayer entity.
	 */
	uint8_t msduLength;

	/**
	 * The set of octets forming the MSDU to be transmitted by the MAC
	 * sublayer entity.
	 */
	uint8_t *msdu;
} mcps_data_req_t;

/**
 * @brief This is the MCPS-DATA.confirm message structure.
 */
typedef struct mcps_data_conf_tag {
	/**< This identifies the message as \ref MCPS_DATA_CONFIRM */
	enum msg_code cmdcode;
	/** The handle associated with the MSDU being confirmed. */
	uint8_t msduHandle;
	/** The status of the last MSDU transmission. */
	uint8_t status;
#ifdef ENABLE_TSTAMP

	/**
	 * Optional. The time, in symbols, at which the data were transmitted
	 * (see 7.5.4.1). The value of this parameter will be considered valid
	 * only
	 * if the value of the status parameter is MAC_SUCCESS; if the status
	 * parameter
	 * is not equal to MAC_SUCCESS, the value of the Timestamp parameter
	 * shall not
	 * be used for any other purpose. The symbol boundary is described by
	 * macSyncSymbolOffset (see Table 86 in 7.4.1).
	 * This is a 24-bit value, and the precision of this value shall be a
	 * minimum of 20 bits, with the lowest 4 bits being the least
	 * significant.
	 */
	uint32_t Timestamp;
#endif  /* ENABLE_TSTAMP */
} mcps_data_conf_t;

/**
 * @brief This is the MCPS-PURGE.request message structure.
 */
typedef struct mcps_purge_req_tag {
	/**< This identifies the message as \ref MCPS_PURGE_REQUEST */
	enum msg_code cmdcode;
	/** The handle of the MSDU to be purged from the transaction queue. */
	uint8_t msduHandle;
} mcps_purge_req_t;

/**
 * @brief This is the MCPS-PURGE.confirm message structure.
 */
typedef struct mcps_purge_conf_tag {
	/**< This identifies the message as \ref MCPS_PURGE_CONFIRM */
	enum msg_code cmdcode;

	/**
	 * The handle of the MSDU requested to be purge from the transaction
	 * queue.
	 */
	uint8_t msduHandle;

	/**
	 * The status of the request to be purged an MSDU from the
	 * transaction queue.
	 */
	uint8_t status;
} mcps_purge_conf_t;

/* === MLME-SAP messages ==================================================== */

/**
 * @brief This is the MLME-ASSOCIATE.request message structure.
 */
typedef struct mlme_associate_req_tag {
	/**< This identifies the message as \ref MLME_ASSOCIATE_REQUEST */
	enum msg_code cmdcode;
	/** The logical channel on which to attempt association. */
	uint8_t LogicalChannel;
	/** The channel page on which to attempt association. */
	uint8_t ChannelPage;

	/**
	 * The coordinator addressing mode for this primitive and subsequent
	 * MPDU.
	 * This value can take one of the following values:
	 * 2 = 16 bit short address. 3 = 64 bit extended address.
	 */
	uint8_t CoordAddrMode;
	/** The identifier of the PAN with which to associate. */
	uint16_t CoordPANId;
	/** The address of the coordinator with which to associate.*/
	address_field_t CoordAddress;
	/** Specifies the operational capabilities of the associating device. */
	uint8_t CapabilityInformation;
} mlme_associate_req_t;

/**
 * @brief This is the MLME-ASSOCIATE.indication message structure.
 */
typedef struct mlme_associate_ind_tag {
	/** This identifies the message as \ref MLME_ASSOCIATE_INDICATION */
	enum msg_code cmdcode;
	/** The address of the device requesting association. */
	uint64_t DeviceAddress;

	/** The operational capabilities of the device requesting association.
	**/
	uint8_t CapabilityInformation;
} mlme_associate_ind_t;

/**
 * @brief This is the MLME-ASSOCIATE.response message structure.
 */
typedef struct mlme_associate_resp_tag {
	/** This identifies the message as \ref MLME_ASSOCIATE_RESPONSE */
	enum msg_code cmdcode;
	/** The address of the device requesting association. */
	uint64_t DeviceAddress;

	/**
	 * The short device address allocated by the coordinator on successful
	 * association. This parameter is set to 0xffff if the association was
	 * unsuccessful.
	 */
	uint16_t AssocShortAddress;
	/** The status of the association attempt. */
	uint8_t status;
} mlme_associate_resp_t;

/**
 * @brief This is the MLME-ASSOCIATE.confirm message structure.
 */
typedef struct mlme_associate_conf_tag {
	/** This identifies the message as \ref MLME_ASSOCIATE_CONFIRM */
	enum msg_code cmdcode;

	/**
	 * The short device address allocated by the coordinator on successful
	 * association. This parameter will be equal to 0 x ffff if the
	 * association
	 * attempt was unsuccessful.
	 */
	uint16_t AssocShortAddress;
	/** The status of the association attempt. */
	uint8_t status;
} mlme_associate_conf_t;

/**
 * @brief This is the MLME-DISASSOCIATE.request message structure.
 */
typedef struct mlme_disassociate_req_tag {
	/** This identifies the message as \ref MLME_DISASSOCIATE_REQUEST */
	enum msg_code cmdcode;

	/** The addressing mode of the device to which to send the
	 * disassociation notification command.
	 */
	uint8_t DeviceAddrMode;

	/** The PAN identifier of the device to which to send the disassociation
	 * notification command. */
	uint16_t DevicePANId;

	/** The address of the device to which to send the disassociation
	 * notification command. */
	uint64_t DeviceAddress;
	/** The reason for the disassociation (see 7.3.1.3.2). */
	uint8_t DisassociateReason;

	/** TRUE if the disassociation notification command is to be sent
	 * indirectly. */
	uint8_t TxIndirect;
} mlme_disassociate_req_t;

/**
 * @brief This is the MLME-DISASSOCIATE.indication message structure.
 */
typedef struct mlme_disassociate_ind_tag {
	/** This identifies the message as \ref MLME_DISASSOCIATE_INDICATION */
	enum msg_code cmdcode;
	/** The address of the device requesting disassociation. */
	uint64_t DeviceAddress;
	/** The reason for the disassociation (see 7.3.1.3.2). */
	uint8_t DisassociateReason;
} mlme_disassociate_ind_t;

/**
 * @brief This is the MLME-DISASSOCIATE.confirm message structure.
 */
typedef struct mlme_disassociate_conf_tag {
	/** This identifies the message as \ref MLME_DISASSOCIATE_CONFIRM */
	enum msg_code cmdcode;
	/** The status of the disassociation attempt. */
	uint8_t status;

	/** The addressing mode of the device that has either requested
	 *  disassociation or been instructed to disassociate by its
	 * coordinator.
	 */
	uint8_t DeviceAddrMode;

	/** The PAN identifier of the device that has either requested
	 * disassociation
	 *  or been instructed to disassociate by its coordinator.
	 */
	uint16_t DevicePANId;

	/** The address of the device that has either requested disassociation
	 * or
	 *  been instructed to disassociate by its coordinator.
	 */
	uint64_t DeviceAddress;
} mlme_disassociate_conf_t;

/**
 * @brief This is the MLME-BEACON-NOTIFY.indication message structure.
 */
typedef struct mlme_beacon_notify_ind_tag {
	/** This identifies the message as \ref MLME_BEACON_NOTIFY_INDICATION */
	enum msg_code cmdcode;
	/** The beacon sequence number. */
	uint8_t BSN;
	/** The PANDescriptor for the received beacon. */
	wpan_pandescriptor_t PANDescriptor;
	/** The beacon pending address specification. */
	uint8_t PendAddrSpec;

	/**
	 * The list of addresses of the devices for which the beacon source has
	 * data. */
	uint8_t *AddrList;

	/**
	 * The number of octets contained in the beacon payload of the beacon
	 * frame
	 * received by the MAC sublayer.
	 */
	uint8_t sduLength;

	/**
	 * The set of octets comprising the beacon payload to be transferred
	 * from the
	 * MAC sublayerentity to the next higher layer.
	 */
	uint8_t *sdu;
} mlme_beacon_notify_ind_t;

/**
 * @brief This is the MLME-ORPHAN.indication message structure.
 */
typedef struct mlme_orphan_ind_tag {
	/** This identifies the message as \ref  MLME_ORPHAN_INDICATION */
	enum msg_code cmdcode;
	/** The address of the orphaned device. */
	uint64_t OrphanAddress;
} mlme_orphan_ind_t;

/**
 * @brief This is the MLME-ORPHAN.response message structure.
 */
typedef struct mlme_orphan_resp_tag {
	/** This identifies the message as \ref  MLME_ORPHAN_RESPONSE */
	enum msg_code cmdcode;
	/** The address of the orphaned device. */
	uint64_t OrphanAddress;

	/**
	 * The short address allocated to the orphaned device if it is
	 * associated
	 * with this coordinator. The special short address 0 x fffe indicates
	 * that
	 * no short address was allocated, and the device will use its 64 bit
	 * extended address in all communications. If the device was not
	 * associated
	 *  with thiscoordinator, this field will contain the value 0 x ffff and
	 * be ignored on receipt.
	 */
	uint16_t ShortAddress;

	/**
	 * TRUE if the orphaned device is associated with this coordinator or
	 * FALSE otherwise.
	 */
	uint8_t AssociatedMember;
} mlme_orphan_resp_t;

/**
 * @brief This is the MLME-RESET.request message structure.
 */
typedef struct mlme_reset_req_tag {
	/**< This identifies the message as \ref MLME_RESET_REQUEST */
	enum msg_code cmdcode;

	/**
	 * If TRUE, the MAC sublayer is reset and all MAC PIB attributes are set
	 * to
	 * their default values. If FALSE, the MAC sublayer is reset but all MAC
	 * PIB
	 * attributes retain their values prior to the generation of the
	 * MLME-RESET.request primitive.
	 */
	uint8_t SetDefaultPIB;
} mlme_reset_req_t;

/**
 * @brief This is the MLME-RESET.confirm message structure.
 */
typedef struct mlme_reset_conf_tag {
	/**< This identifies the message as \ref MLME_RESET_CONFIRM */
	enum msg_code cmdcode;
	/** The result of the reset operation. */
	uint8_t status;
} mlme_reset_conf_t;

/**
 * @brief This is the MLME-GET.request message structure.
 */
typedef struct mlme_get_req_tag {
	/**< This identifies the message as \ref MLME_GET_REQUEST */
	enum msg_code cmdcode;
	/**< The identifier of the MAC PIB attribute to get. */
	uint8_t PIBAttribute;
#if ((defined MAC_SECURITY_ZIP)  || (defined MAC_SECURITY_2006))

	/**< The index within the table of the specified MAC PIB attribute to
	 * set. */
	uint8_t PIBAttributeIndex;
#endif  /* (MAC_SECURITY_ZIP || MAC_SECURITY_2006) */
} mlme_get_req_t;

/**
 * @brief This is the MLME-GET.confirm message structure.
 */
typedef struct mlme_get_conf_tag {
	/**< This identifies the message as \ref MLME_GET_CONFIRM */
	enum msg_code cmdcode;
	/**< The result of the request for PHY PIB attribute information.*/
	uint8_t status;
	/**< The identifier of the MAC PIB attribute to get. */
	uint8_t PIBAttribute;
#if ((defined MAC_SECURITY_ZIP)  || (defined MAC_SECURITY_2006))

	/**< The index within the table of the specified MAC PIB attribute to
	 * set. */
	uint8_t PIBAttributeIndex;
#endif  /* (MAC_SECURITY_ZIP || MAC_SECURITY_2006) */
	/**< The value of the indicated MAC PIB attribute that was read. */
	pib_value_t PIBAttributeValue;
} mlme_get_conf_t;

#if (HIGHEST_STACK_LAYER == MAC)

/**
 * @brief This is the MLME-SET.request message structure.
 */
typedef struct mlme_set_req_tag {
	/**< This identifies the message as \ref MLME_SET_REQUEST */
	enum msg_code cmdcode;
	/**< The identifier of the MAC PIB attribute to set. */
	uint8_t PIBAttribute;
#if ((defined MAC_SECURITY_ZIP)  || (defined MAC_SECURITY_2006))

	/**< The index within the table of the specified MAC PIB attribute to
	 * set. */
	uint8_t PIBAttributeIndex;
#endif  /* (MAC_SECURITY_ZIP || MAC_SECURITY_2006) */
	/**< The value to write to the indicated MAC PIB attribute. */
	pib_value_t PIBAttributeValue;
} mlme_set_req_t;
#endif /* (HIGHEST_STACK_LAYER == MAC) */

#if (HIGHEST_STACK_LAYER == MAC)

/**
 * @brief This is the MLME-SET.confirm message structure.
 */
typedef struct mlme_set_conf_tag {
	/**< This identifies the message as \ref MLME_SET_CONFIRM */
	enum msg_code cmdcode;
	/**< The result of the request to write the MAC PIB attribute. */
	uint8_t status;
	/**< The identifier of the MAC PIB attribute that was written. */
	uint8_t PIBAttribute;
#if ((defined MAC_SECURITY_ZIP)  || (defined MAC_SECURITY_2006))

	/**< The index within the table of the specified MAC PIB attribute to
	 * set. */
	uint8_t PIBAttributeIndex;
#endif  /* (MAC_SECURITY_ZIP || MAC_SECURITY_2006) */
} mlme_set_conf_t;
#endif /* (HIGHEST_STACK_LAYER == MAC) */

/**
 * @brief This is the MLME-RX-ENABLE.request message structure.
 */
typedef struct mlme_rx_enable_req_tag {
	/** This identifies the message as \ref MLME_RX_ENABLE_REQUEST */
	enum msg_code cmdcode;

	/**
	 * TRUE if the receiver enable can be deferred until during the next
	 * superframe if the requested time has already passed. FALSE if the
	 * receiver enable is only to be attempted in the current superframe.
	 * This parameter is ignored for nonbeacon-enabled PANs.
	 */
	uint8_t DeferPermit;

	/**
	 * The number of symbols from the start of the superframe before the
	 * receiver is to be enabled. The precision of this value is a minimum
	 * of
	 * 20 bits, with the lowest 4 bits being the least significant. This
	 * parameter is ignored for nonbeacon-enabled PANs.
	 */
	uint32_t RxOnTime;
	/** The number of symbols for which the receiver is to be enabled. */
	uint32_t RxOnDuration;
} mlme_rx_enable_req_t;

/**
 * @brief This is the MLME-RX-ENABLE.confirm message structure.
 */
typedef struct mlme_rx_enable_conf_tag {
	/** This identifies the message as \ref MLME_RX_ENABLE_CONFIRM */
	enum msg_code cmdcode;
	/** The result of the receiver enable request. */
	uint8_t status;
} mlme_rx_enable_conf_t;

/**
 * @brief This is the MLME-SCAN.request message structure.
 */
typedef struct mlme_scan_req_tag {
	/** This identifies the message as \ref MLME_SCAN_REQUEST */
	enum msg_code cmdcode;

	/**
	 * Indicates the type of scan performed: 0 x 00 = ED scan (FFD only).
	 * 0 x 01 = active scan (FFD only). 0 x 02 = passive scan. 0 x 03 =
	 * orphan
	 * scan.
	 */
	uint8_t ScanType;

	/**
	 * The 5 MSBs (b27, ... , b31) are reserved. The 27 LSBs (b0, b1, ...
	 * b26)
	 * indicate which channels are to be scanned (1 = scan, 0 = do not scan)
	 * for
	 * each of the 27 valid channels (see 6.1.2).
	 */
	uint32_t ScanChannels;

	/**
	 * A value used to calculate the length of time to spend scanning each
	 * channel for ED, active, and passive scans. This parameter is ignored
	 * for
	 * orphan scans. The time spent scanning each channel is
	 * [aBaseSuperframeDuration * (2n + 1)] symbols, where n is the value of
	 * the
	 * ScanDuration parameter.
	 */
	uint8_t ScanDuration;

	/**
	 * The channel page on which to perform the scan.
	 */
	uint8_t ChannelPage;
} mlme_scan_req_t;

/**
 * @brief This is the MLME-SCAN.confirm  message structure.
 */
typedef struct mlme_scan_conf_tag {
	/** This identifies the message as \ref MLME_SCAN_CONFIRM */
	enum msg_code cmdcode;
	/** The status of the scan request. */
	uint8_t status;

	/**
	 * ScanType Integer 0 x 00 0 x 03 Indicates if the type of scan
	 * performed:
	 * 0 x 00 = ED scan (FFD only). 0 x 01 = active scan (FFD only). 0 x 02
	 *=
	 * passive scan. 0 x 03 = orphan scan.
	 */
	uint8_t ScanType;
	/** The channel page on which the scan was performed. */
	uint8_t ChannelPage;

	/**
	 * Indicates which channels given in the request were not scanned (1 =
	 * not
	 * scanned, 0 = scanned or not requested). This parameter is only valid
	 * for
	 * passive or active scans.
	 */
	uint32_t UnscannedChannels;

	/**
	 * The number of elements returned in the appropriate result
	 * lists. This value is 0 for the result of an orphan scan.
	 */
	uint8_t ResultListSize;

	/**
	 * ResultListSize - The number of elements returned in the appropriate
	 * result lists. This value is 0 for the result of an orphan scan.
	 * EnergyDetectList - The number of elements returned in the appropriate
	 * result lists. This value is 0 for the result of an orphan scan.
	 * PANDescriptorList - The list of PAN descriptors, one for each beacon
	 * found during an active or passive scan. This parameter is null for ED
	 * and orphan scans.
	 */
	scan_result_list_t scan_result_list[1];
} mlme_scan_conf_t;

/**
 * @brief This is the MLME-COMM-STATUS.indication message structure.
 */
typedef struct mlme_comm_status_ind_tag {
	/** This identifies the message as \ref MLME_COMM_STATUS_INDICATION */
	enum msg_code cmdcode;

	/** The 16 bit PAN identifier of the device from which the frame was
	 * received or to which the frame was being sent. */
	uint16_t PANId;

	/**
	 * The source addressing mode for this primitive. This value can take
	 * one
	 * of the following values: 0 = no address (addressing fields omitted).
	 * 0 x 01 = reserved. 0 x 02 = 16 bit short address. 0 x 03 = 64 bit
	 *  extended address.
	 */
	uint8_t SrcAddrMode;

	/**
	 * The source addressing mode for this primitive. This value can take
	 * one
	 * of the following values: 0 = no address (addressing fields omitted).
	 * 0 x 01 = reserved. 0 x 02 = 16 bit short address. 0 x 03 = 64 bit
	 * extended address.
	 */
	uint64_t SrcAddr;

	/**
	 * The destination addressing mode for this primitive. This value can
	 * take
	 * one of the following values: 0 x 00 = no address (addressing fields
	 * omitted). 0 x 01 = reserved. 0 x 02 = 16 bit short address.
	 * 0 x 03 = 64 bit extended address.
	 */
	uint8_t DstAddrMode;

	/**
	 * The individual device address of the device for which the frame was
	 * intended.
	 */
	uint64_t DstAddr;
	/** The communications status. */
	uint8_t status;
} mlme_comm_status_ind_t;

/**
 * @brief This is the MLME-START.request message structure.
 */
typedef struct mlme_start_req_tag {
	/** This identifies the message as \ref MLME_START_REQUEST */
	enum msg_code cmdcode;
	/** The PAN identifier to be used by the device. */
	uint16_t PANId;

	/** The logical channel on which to start using the new superframe
	 * configuration. */
	uint8_t LogicalChannel;

	/** The channel page on which to begin using the new superframe
	 * configuration. */
	uint8_t ChannelPage;

	/**
	 * How often the beacon is to be transmitted. The beacon order, BO, and
	 * the
	 * beacon interval, BI, are related as follows: for 0 d BO d 14, BI =
	 * BaseSuperframeDuration * 2^BO symbols. If BO = 15, the coordinator
	 * will
	 * not transmit a beacon, and the SuperframeOrder parameter value is
	 * ignored.
	 */
	uint8_t BeaconOrder;

	/**
	 * The length of the active portion of the superframe, including the
	 * beacon
	 * frame. The superframe order, SO, and the superframe duration, SD, are
	 * related as follows: for 0 d SO d BO d 14, SD =
	 * aBaseSuperframeDuration *
	 * 2^SO symbols. If SO = 15, the superframe will not be active after the
	 * beacon.
	 */
	uint8_t SuperframeOrder;

	/**
	 * If this value is TRUE, the device will become the PAN coordinator of
	 * a
	 * new PAN. If this value is FALSE, the device will begin transmitting
	 * beacons on the PAN with which it is associated.
	 */
	uint8_t PANCoordinator;

	/**
	 * If this value is TRUE, the receiver of the beaconing device is
	 * disabled
	 * macBattLifeExtPeriods full backoff periods after the interframe
	 * spacing
	 * (IFS) period of the beacon frame. If this value is FALSE, the
	 * receiver of
	 * the beaconing device remains enabled for the entire CAP.
	 */
	uint8_t BatteryLifeExtension;

	/**
	 * TRUE if a coordinator realignment command is to be transmitted prior
	 * to
	 * changing the superframe configuration or FALSE otherwise.
	 */
	uint8_t CoordRealignment;

#ifdef MAC_SECURITY_BEACON

	/**
	 * The security level to be used for coordinator realignment command
	 * frames (see Table95 in 7.6.2.2.1).
	 * Valid values are 0x007
	 */
	uint8_t CoordRealignSecurityLevel;

	/**
	 * The mode used to identify the key to be used (see Table 96 in
	 * 7.6.2.2.2).
	 * This parameter is ignored if the CoordRealignSecurityLevel parameter
	 * is set to 0x00.
	 */
	uint8_t CoordRealignKeyIdMode;

	/**
	 * The originator of the key to be used (see 7.6.2.4.1). This parameter
	 * is
	 * ignored if the CoordRealignKeyIdMode parameter is ignored or set to
	 * 0x00.
	 */
	uint8_t *CoordRealignKeySource;

	/**
	 * The index of the key to be used (see 7.6.2.4.2). This parameter is
	 * ignored if the CoordRealignKeyIdMode
	 * parameter is ignored or set to 0x00.
	 */
	uint8_t CoordRealignKeyIndex;

	/**
	 * The security level to be used for beacon frames (see Table 95 in
	 * 7.6.2.2.1).
	 */
	uint8_t BeaconSecurityLevel;

	/**
	 * The mode used to identify the key to be used
	 * (see Table 96 in 7.6.2.2.2). This parameter is ignored
	 * if the BeaconSecurityLevel parameter is set
	 * to 0x00.
	 */
	uint8_t BeaconKeyIdMode;

	/**
	 * The originator of the key to be used (see 7.6.2.4.1).
	 * This parameter is ignored if the BeaconKeyIdMode
	 * parameter is ignored or set to 0x00.
	 */
	uint8_t *BeaconKeySource;

	/**
	 * The index of the key to be used (see 7.6.2.4.2).
	 * This parameter is ignored if the BeaconKeyIdMode
	 * parameter is ignored or set to 0x00.
	 */
	uint8_t BeaconKeyIndex;
#endif
} mlme_start_req_t;

/**
 * @brief This is the MLME-START.confirm message structure.
 */
typedef struct mlme_start_conf_tag {
	/** This identifies the message as \ref MLME_START_CONFIRM */
	enum msg_code cmdcode;

	/** The result of the attempt to start using an updated superframe
	 * configuration. */
	uint8_t status;
} mlme_start_conf_t;

/**
 * @brief This is the MLME-SYNC.request message structure.
 */
typedef struct mlme_sync_req_tag {
	/** This identifies the message as \ref MLME_SYNC_REQUEST */
	enum msg_code cmdcode;

	/** The logical channel on which to attempt coordinator synchronization.
	**/
	uint8_t LogicalChannel;
	/** The channel page on which to attempt coordinator synchronization. */
	uint8_t ChannelPage;

	/**
	 * TRUE if the MLME is to synchronize with the next beacon and attempt
	 * to
	 * track all future beacons. FALSE if the MLME is to synchronize with
	 * only
	 * the next beacon.
	 */
	bool TrackBeacon;
} mlme_sync_req_t;

/**
 * @brief This is the MLME-SYNC-LOSS.indication message structure.
 */
typedef struct mlme_sync_loss_ind_tag {
	/** This identifies the message as \ref MLME_SYNC_LOSS_INDICATION */
	enum msg_code cmdcode;
	/** The reason that synchronization was lost. */
	uint8_t LossReason;

	/** The PAN identifier with which the device lost synchronization or
	 *  to which it was realigned.
	 */
	uint16_t PANId;

	/** The logical channel on which the device lost synchronization or
	 *  to which it was realigned.
	 */
	uint8_t LogicalChannel;

	/** The channel page on which the device lost synchronization or
	 * to which it was realigned.
	 */
	uint8_t ChannelPage;
} mlme_sync_loss_ind_t;

/**
 * @brief This is the MLME-POLL.request message structure.
 */
typedef struct mlme_poll_req_tag {
	/** This identifies the message as \ref MLME_POLL_REQUEST */
	enum msg_code cmdcode;

	/**
	 * The addressing mode of the coordinator to which the poll is intended.
	 * This parameter can take one of the following values: 2 = 16 bit short
	 * address, 3 = 64 bit extended address.
	 */
	uint8_t CoordAddrMode;

	/** The PAN identifier of the coordinator to which the poll is intended.
	**/
	uint16_t CoordPANId; /** The address of the coordinator to which the
	                      * poll is intended. */
	/** The address of the coordinator to which the poll is intended. */
	uint64_t CoordAddress;
} mlme_poll_req_t;

/**
 * @brief This is the MLME-POLL.confirm message structure.
 */
typedef struct mlme_poll_conf_tag {
	/** This identifies the message as \ref MLME_POLL_CONFIRM */
	enum msg_code cmdcode;
	/** The status of the data request. */
	uint8_t status;
} mlme_poll_conf_t;

#ifdef GTS_SUPPORT

/**
 * @brief This is the MLME-GTS.request message structure.
 */
typedef struct mlme_gts_req_tag {
	/** This identifies the message as \ref MLME_GTS_REQUEST */
	enum msg_code cmdcode;
	uint16_t DeviceShortAddr;
	gts_char_t GtsChar;
} mlme_gts_req_t;

/**
 * @brief This is the MLME-GTS.confirm message structure.
 */
typedef struct mlme_gts_conf_tag {
	/** This identifies the message as \ref MLME_GTS_CONFIRM */
	enum msg_code cmdcode;
	gts_char_t GtsChar;
	/** The status of the data request. */
	uint8_t status;
} mlme_gts_conf_t;

/**
 * @brief This is the MLME-GTS.indication message structure.
 */
typedef struct mlme_gts_ind_tag {
	/** This identifies the message as \ref MLME_GTS_INDICATION */
	enum msg_code cmdcode;
	uint16_t DeviceAddr;
	/** The status of the data request. */
	gts_char_t GtsChar;
} mlme_gts_ind_t;
#endif /* GTS_SUPPORT */
__PACK__RST_DATA__
/* ! @} */
/* === Externals ============================================================ */

/* === Prototypes =========================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /* MAC_MSG_TYPES_H */
/* EOF */
