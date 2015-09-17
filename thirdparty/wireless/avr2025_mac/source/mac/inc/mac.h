/**
 * @file mac.h
 *
 * @brief Provides MAC API to access MAC Layer functionality.
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
#ifndef _MAC_H
#define _MAC_H

/* === Includes ============================================================= */

#include <stdint.h>
#include <stdbool.h>

#include "stack_config.h"
#include "tal_generic.h"
#include "return_val.h"
#include "mac_api.h"

#include "qmm.h"
#include "mac_build_config.h"
#ifdef ENABLE_RTB
#include "rtb.h"
#endif  /* ENABLE_RTB */

/**
 * \defgroup group_mac  IEEE 802.15.4 MAC v3.1.1
 * The IEEE standard 802.15.4 MAC Layer defines the protocol and compatible
 * interconnection for data communication devices using low data rate, low
 * power and low complexity, short-range radio frequency (RF) transmissions in a
 * wireless personal area network (WPAN).
 *
 */

/**
 * \ingroup group_mac
 * \defgroup group_mac_build   MAC Build Configurations
 * Defines MAC Build Configuration Used
 *
 */

/**
 * \ingroup group_mac
 * \defgroup group_mac_ds  MAC Data Structures
 *
 * This module defines all the MAC Data Structures
 *
 */

/**
 * \ingroup group_mac
 * \defgroup group_mac_def  MAC Definitions
 * All  Definitions used by the MAC Layer is described here.
 *
 */

/**
 * \ingroup group_mac
 * \defgroup group_mac_prot MAC Prototypes
 *
 * All MAC Prototypes are described in this module
 *
 */

/**
 * \ingroup group_mac_prot
 * \defgroup group_mcl_api MCL API's
 *
 * This module describes all Mac Core Layer(MCL) API's
 */

/**
 * \ingroup group_mcl_api
 * \defgroup group_mac_req_int MCL  Request API's
 *
 * This module describes all MCL  Request API's
 */

/**
 * \ingroup group_mcl_api
 * \defgroup group_mac_gen_int MCL  General API's
 *
 * This module describes all MCL  General API's
 */

/**
 * \ingroup group_mcl_api
 * \defgroup group_mac_conf_int MCL  Confirmation API's
 *
 * This module describes all MCL  Confirmation API's
 */

/**
 * \ingroup group_mcl_api
 * \defgroup group_mac_ind_int MCL  Indication API's
 *
 * This module describes all MCL  Indication API's
 */

/**
 * \ingroup group_mcl_api
 * \defgroup group_mac_resp_int MCL  Response API's
 *
 * This module describes all MCL  Response API's
 */

/* === Macros =============================================================== */

#if ((MAC_SCAN_ED_REQUEST_CONFIRM == 1)      ||	\
	(MAC_SCAN_ACTIVE_REQUEST_CONFIRM == 1)  || \
	(MAC_SCAN_PASSIVE_REQUEST_CONFIRM == 1) || \
	(MAC_SCAN_ORPHAN_REQUEST_CONFIRM == 1))

/**
 * Maximum allowed PANDescriptors that is calculated using the
 * large buffer size.
 */
#define MAX_ALLOWED_PAN_DESCRIPTORS     ((LARGE_BUFFER_SIZE - \
	sizeof(mlme_scan_conf_t)) / sizeof(wpan_pandescriptor_t))

/**
 * Active/passive scan: implementation-defined maximum number of
 * PANDescriptors that can be stored.
 */
#define MAX_PANDESCRIPTORS \
	(MAX_ALLOWED_PAN_DESCRIPTORS > 5 ? 5 : MAX_ALLOWED_PAN_DESCRIPTORS)
#endif

/*
 * Defines the mask for the FCF address mode
 */
#define FCF_ADDR_MASK                   (3)

/*
 * Macro to get the source address mode.
 */
#define FCF_GET_SOURCE_ADDR_MODE(x) \
	(((x) >> FCF_SOURCE_ADDR_OFFSET) & FCF_ADDR_MASK)

/*
 * Macro to get the destination address mode.
 */
#define FCF_GET_DEST_ADDR_MODE(x) \
	(((x) >> FCF_DEST_ADDR_OFFSET) & FCF_ADDR_MASK)

/* === Types ================================================================ */

/* === Externals ============================================================ */

/**
 * Queue used by MAC for communication to next higher layer.
 */
extern queue_t mac_nhle_q;

extern queue_t nhle_mac_q;

/* === Prototypes =========================================================== */

#ifdef __cplusplus
extern "C" {
#endif

/* 802.15.4 MAC layer entries */

/**
 * @brief Builds the data frame for transmission
 *
 * This function builds the data frame for transmission.
 * The NWK layer has supplied the parameters.
 * The frame_info_t data type is constructed and filled in.
 * Also the FCF is constructed based on the parameters passed.
 *
 * \ingroup group_mac_req_int
 * @param msg Pointer to the MCPS-DATA.request parameter
 */

void mcps_data_request(uint8_t *msg);

/**
 * @brief Wrapper function for messages of type mcps_data_conf_t
 *
 * This function is a callback for mcps data confirm.
 *
 * \ingroup group_mac_conf_int
 * @param m Pointer to message structure
 */

void mcps_data_conf(uint8_t *m);

/**
 * @brief Wrapper function for messages of type mcps_data_ind_t
 *
 * This function is a callback for mcps data indication
 *
 * \ingroup group_mac_ind_int
 * @param m Pointer to message structure
 */

void mcps_data_ind(uint8_t *m);

#if ((MAC_PURGE_REQUEST_CONFIRM == 1) && (MAC_INDIRECT_DATA_BASIC == 1))

/**
 * @brief Processes a MCPS-PURGE.request primitive
 *
 * This functions processes a MCPS-PURGE.request from the NHLE.
 * The MCPS-PURGE.request primitive allows the next higher layer
 * to purge an MSDU from the transaction queue.
 * On receipt of the MCPS-PURGE.request primitive, the MAC sublayer
 * attempts to find in its transaction queue the MSDU indicated by the
 * msduHandle parameter. If an MSDU matching the given handle is found,
 * the MSDU is discarded from the transaction queue, and the MAC
 * sublayer issues the MCPSPURGE. confirm primitive with a status of
 * MAC_SUCCESS. If an MSDU matching the given handle is not found, the MAC
 * sublayer issues the MCPS-PURGE.confirm primitive with a status of
 * INVALID_HANDLE.
 *
 * \ingroup group_mac_req_int
 * @param msg Pointer to the MCPS-PURGE.request parameter
 */
void mcps_purge_request(uint8_t *msg);

/**
 * @brief Wrapper function for messages of type mcps_purge_conf_t
 *
 * This function is a callback for mcps purge confirm.
 *
 * \ingroup group_mac_conf_int
 * @param m Pointer to message structure
 */
void mcps_purge_conf(uint8_t *m);

#endif  /* ((MAC_PURGE_REQUEST_CONFIRM == 1) && (MAC_INDIRECT_DATA_BASIC == 1))
        **/

#if (MAC_GET_SUPPORT == 1)

/**
 * @brief Handles an MLME-GET.request
 *
 * This function handles an MLME-GET.request.
 * The MLME-GET.request primitive requests information about a
 * given PIB attribute.
 *
 * \ingroup group_mac_req_int
 * @param m Pointer to the request structure
 */

void mlme_get_request(uint8_t *m);

/**
 * @brief Wrapper function for messages of type mlme_get_conf_t
 *
 * This function is a callback for mlme get confirm.
 *
 * \ingroup group_mac_conf_int
 * @param m Pointer to message structure
 */
void mlme_get_conf(uint8_t *m);

#endif  /* (MAC_GET_SUPPORT == 1) */

/**
 * @brief Resets the MAC layer
 *
 * The MLME-RESET.request primitive allows the next higher layer to request
 * that the MLME performs a reset operation.
 *
 * \ingroup group_mac_req_int
 * @param m Pointer to the MLME_RESET.request given by the NHLE
 */
void mlme_reset_request(uint8_t *m);

#if ((MAC_SCAN_ED_REQUEST_CONFIRM == 1) || (MAC_SCAN_ACTIVE_REQUEST_CONFIRM == \
	1)  || \
	(MAC_SCAN_PASSIVE_REQUEST_CONFIRM == 1) || \
	(MAC_SCAN_ORPHAN_REQUEST_CONFIRM == 1))

/**
 * @brief The MLME-SCAN.request primitive makes a request for a node to
 * start a scan procedure.
 *
 * 802.15.4. Section 7.1.11.1.
 *
 * The MLME-SCAN.request primitive is used to initiate a channel scan over a
 * given list of channels. A device can use a channel scan to measure the
 * energy on the channel, search for the coordinator with which it associated,
 * or search for all coordinators transmitting beacon frames within the
 * POS of the scanning device.
 *
 * The MLME-SCAN.request primitive is generated by the next higher layer and
 * issued to its MLME to initiate a channel scan to search for activity within
 * the POS of the device. This primitive can be used to perform an ED scan to
 * determine channel usage, an active or passive scan to locate beacon frames
 * containing any PAN identifier, or an orphan scan to locate a PAN to which
 * the device is currently associated.
 *
 * ED or active scans can be performed before an FFD starts operation as a
 * PAN coordinator. Active or passive scans can be performed prior to selecting
 * a PAN for association. Orphan scans can be performed to attempt to locate a
 * specific coordinator with which communication has been lost.
 *
 * All devices shall be capable of performing passive scans and orphan scans;
 * ED scans and active scans are optional for an RFD.
 *
 * When the MLME receives the MLME-SCAN.request primitive, it initiates a scan
 * in all channels specified in the ScanChannels parameter. The MLME suspends
 * all beacon transmissions for the duration of the scan. During a scan, the
 * MAC sublayer only accepts frames received over the PHY data service that are
 * relevant to the scan being performed (see 7.5.2.1).
 *
 * An ED scan allows a device to obtain a measure of the peak energy in each
 * requested channel. The ED scan is performed on each channel by the MLMEs
 * repeatedly issuing the PLME-ED.request primitive to the PHY until
 * [aBaseSuperframeDuration * (2n + 1)] symbols, where n is the value of the
 * ScanDuration parameter, have elapsed. The MLME notes the maximum energy
 * measurement and moves on to the next channel in the channel list. A device
 * will be able to store between one and an implementation-specified maximum
 * number of channel ED measurements. The ED scan terminates when the number
 * of channel ED measurements stored equals this implementation-specified
 * maximum or when every channel specified in the channel list has been scanned.
 *
 * An active scan is used by an FFD to locate all coordinators transmitting
 * beacon frames within its POS. The active scan is performed on each channel
 * by the MLMEs first sending a beacon request command (see 7.3.2.4). The MLME
 * then enables the receiver and records the information contained in each
 * received beacon in a PAN descriptor structure (see Table 41 in 7.1.5.1.1).
 * A device will be able to store between one and an implementation-specified
 * maximum number of PAN descriptors. The active scan on a particular channel
 * terminates when the number of PAN descriptors stored equals this
 * implementation-specified maximum or when [aBaseSuperframeDuration*(2n + 1)]
 * symbols, where n is the value of the ScanDuration parameter, have elapsed.
 * If the latter condition has been satisfied, the channel is considered to
 * have been scanned. Where possible, the scan is repeated on each channel and
 * terminates when the number of PAN descriptors stored equals the
 * implementation-specified maximum or when every channel in the set of
 *  available channels has been scanned.
 *
 * A passive scan, like an active scan, is used to locate all coordinators
 * transmitting beacon frames within the POS of the scanning device; the
 * difference is that the passive scan is a receive-only operation and does not
 * transmit the beacon request command. The passive scan is performed on each
 * channel by the MLMEs enabling its receiver and recording the information
 * contained in each received beacon in a PAN descriptor structure
 * (see Table 41 in 7.1.5.1.1). A device will be able to store between one and
 * an implementation-specified maximum number of PAN descriptors. The passive
 * scan on a particular channel terminates when the number of PAN descriptors
 * stored equals this implementation-specified maximum or when
 * [aBaseSuperframeDuration * (2n + 1)] symbols, where n is the value of the
 * ScanDuration parameter, have elapsed. If the latter condition has been
 * satisfied, the channel is considered to have been scanned. Where possible,
 * the scan is repeated on each channel and terminates when the number of PAN
 * descriptors stored equals the implementation-specified maximum or when
 * every channel in the set of available channels has been scanned.
 *
 * An orphan scan is used to locate the coordinator with which the scanning
 * device had previously associated. The orphan scan is performed on each
 * channel by the MLME first sending an orphan notification command
 * (see 7.3.2.3). The MLME then enables its receiver for at most
 * aResponseWaitTime symbols. If the device receives a coordinator realignment
 * command within this time, the MLME will disable its receiver. Otherwise, the
 * scan is repeated on the next channel in the channel list. The scan
 * terminates when the device receives a coordinator realignment command
 * (see 7.3.2.5) or when every channel in the set of available channels has
 * been scanned.
 *
 * The results of an ED scan are recorded in a list of ED values and reported
 * to the next higher layer through the MLME-SCAN.confirm primitive with a
 * status of MAC_SUCCESS. The results of an active or passive scan are recorded
 * in a set of PAN descriptor values and reported to the next higher layer
 * through the MLME-SCAN.confirm primitive. If no beacons were found, the
 * MLME-SCAN.confirm primitive will contain a null set of PAN descriptor
 * values and a status of NO_BEACON. Otherwise, the MLME-SCAN.confirm primitive
 * will contain the set of PANDescriptor values found, a list of unscanned
 * channels, and a status of MAC_SUCCESS.
 *
 * The results of an orphan scan are reported to the next higher layer through
 * the MLME-SCAN.confirm primitive. If successful, the MLME-SCAN.confirm
 * primitive will contain a status of MAC_SUCCESS. If the device did not receive
 * a
 * coordinator realignment command, MLME-SCAN.confirm primitive will contain
 * a status of NO_BEACON. In either case, the PANDescriptorList and
 * EnergyDetectList parameters of the MLMESCAN.confirm primitive are null.
 *
 * If any parameter in the MLME-SCAN.request primitive is not supported or is
 * out of range, the MAC sublayer will issue the MLME-SCAN.confirm primitive
 * with a status of MAC_INVALID_PARAMETER.
 *
 * \ingroup group_mac_req_int
 * @param m The MLME_SCAN.request message
 */

void mlme_scan_request(uint8_t *m);

/**
 * @brief Wrapper function for messages of type mlme_scan_conf_t
 *
 * This function is a callback for mlme scan confirm.
 *
 * \ingroup group_mac_conf_int
 * @param m Pointer to message structure
 */
void mlme_scan_conf(uint8_t *m);

#endif

#if (MAC_START_REQUEST_CONFIRM == 1)

/**
 * @brief The MLME-START.request primitive makes a request for the device to
 * start using a new superframe configuration
 *
 * \ingroup group_mac_req_int
 * @param m Pointer to MLME_START.request message issued by the NHLE
 */
void mlme_start_request(uint8_t *m);

/**
 * @brief Wrapper function for messages of type mlme_start_conf_t
 *
 * This function is a callback for mlme start confirm.
 *
 * \ingroup group_mac_conf_int
 * @param m Pointer to message structure
 */
void mlme_start_conf(uint8_t *m);

#endif /* (MAC_START_REQUEST_CONFIRM == 1) */

#if (MAC_ASSOCIATION_REQUEST_CONFIRM == 1)

/**
 * @brief Handles the MLME associate request command from the NWK layer
 *
 * The MLME associate request primitive is generated by the next higher layer
 * of an unassociated device and issued to its MAC to request an association
 * with a coordinator.
 *
 * \ingroup group_mac_req_int
 * @param m Pointer to MLME association request parameters
 */

void mlme_associate_request(uint8_t *m);

/**
 * @brief Wrapper function for messages of type mlme_associate_conf_t
 *
 * This function is a callback for mlme associate confirm.
 *
 * \ingroup group_mac_conf_int
 * @param m Pointer to message structure
 */
void mlme_associate_conf(uint8_t *m);

#endif  /* (MAC_ASSOCIATION_REQUEST_CONFIRM == 1) */
#if (MAC_ASSOCIATION_INDICATION_RESPONSE == 1)

/**
 * @brief Entry point from the stack for MLME associate response
 *
 * The MLME associate response primitive is used to initiate a response to a
 * MLME association indication primitive.
 *
 * \ingroup group_mac_resp_int
 * @param m Pointer to association response parameters
 */
void mlme_associate_response(uint8_t *m);

/**
 * @brief Wrapper function for messages of type mlme_associate_ind_t
 *
 * This function is a callback for mlme associate indication.
 *
 * \ingroup group_mac_ind_int
 * @param m Pointer to message structure
 */
void mlme_associate_ind(uint8_t *m);

#endif /* (MAC_ASSOCIATION_INDICATION_RESPONSE == 1) */
#if (MAC_DISASSOCIATION_BASIC_SUPPORT == 1)

/**
 * @brief Handles the MLME disassociate request command from the NWK layer
 *
 * The MLME-DISASSOCIATE.request primitive is generated by the next
 * higher layer of an associated device and issued to its MLME to
 * request disassociation from the PAN. It is also generated by the
 * next higher layer of the coordinator and issued to its MLME to
 * instruct an associated device to leave the PAN.
 *
 * \ingroup group_mac_req_int
 * @param m Pointer to the MLME-DISASSOCIATION.Request message passed by NHLE
 */
void mlme_disassociate_request(uint8_t *m);

/**
 * @brief Wrapper function for messages of type mlme_disassociate_conf_t
 *
 * This function is a callback for mlme disassociate confirm.
 *
 * \ingroup group_mac_conf_int
 * @param m Pointer to message structure
 */
void mlme_disassociate_conf(uint8_t *m);

/**
 * @brief Wrapper function for messages of type mlme_disassociate_ind_t
 *
 * This function is a callback for mlme disassociate indication.
 *
 * \ingroup group_mac_ind_int
 * @param m Pointer to message structure
 */
void mlme_disassociate_ind(uint8_t *m);

#endif  /* (MAC_DISASSOCIATION_BASIC_SUPPORT == 1) */
#if (MAC_ORPHAN_INDICATION_RESPONSE == 1)

/**
 * @brief Implements the MLME-ORPHAN.response
 *
 * The MLME-ORPHAN.response primitive allows the next higher layer
 * of a coordinator to respond to the MLME-ORPHAN.indication primitive.
 * The MLME-ORPHAN.response primitive is generated by the next higher
 * layer and issued to its MLME when it reaches a decision about whether
 * the orphaned device indicated in the MLME-ORPHAN.indication primitive
 * is associated.
 *
 * \ingroup group_mac_resp_int
 * @param m Pointer to the message.
 */
void mlme_orphan_response(uint8_t *m);

/**
 * @brief Wrapper function for messages of type mlme_orphan_ind_t
 *
 * This function is a callback for mlme orphan indication.
 *
 * \ingroup group_mac_ind_int
 * @param m Pointer to message structure
 */
void mlme_orphan_ind(uint8_t *m);

#endif /* (MAC_ORPHAN_INDICATION_RESPONSE == 1) */
#if (MAC_INDIRECT_DATA_BASIC == 1)

/**
 * @brief Implements MLME-POLL.request
 *
 * This function handles an MLME-POLL.request primitive.
 * The MLME-POLL.request primitive is generated by the next
 * higher layer and issued to its MLME when data are to be
 * requested from a coordinator.
 *
 * \ingroup group_mac_req_int
 * @param m Pointer to the message
 */
void mlme_poll_request(uint8_t *m);

/**
 * @brief Wrapper function for messages of type mlme_poll_conf_t
 *
 * This function is a callback for mlme poll confirm.
 *
 * \ingroup group_mac_conf_int
 * @param m Pointer to message structure
 */
void mlme_poll_conf(uint8_t *m);

#endif  /* (MAC_INDIRECT_DATA_BASIC == 1) */
#if (MAC_RX_ENABLE_SUPPORT == 1)

/**
 * @brief Implement the MLME-RX-ENABLE.request primitive.
 *
 * The MLME-RX-ENABLE.request primitive is generated by the next
 * higher layer and issued to MAC to enable the receiver for a
 * fixed duration, at a time relative to the start of the current or
 * next superframe on a beacon-enabled PAN or immediately on a
 * nonbeacon-enabled PAN. The receiver is enabled exactly once per
 * primitive request.
 *
 * \ingroup group_mac_req_int
 * @param m Pointer to the MLME-RX-ENABLE.request message
 */
void mlme_rx_enable_request(uint8_t *m);

/**
 * @brief Wrapper function for messages of type mlme_rx_enable_conf_t
 *
 * This function is a callback for mlme rx enable confirm.
 *
 * \ingroup group_mac_conf_int
 * @param m Pointer to message structure
 */
void mlme_rx_enable_conf(uint8_t *m);

#endif  /* (MAC_RX_ENABLE_SUPPORT == 1) */

/**
 * @brief Implements the MLME-SYNC request.
 *
 * The MLME-SYNC.request primitive requests to synchronize with the
 * coordinator by acquiring and, if specified, tracking its beacons.
 * The MLME-SYNC.request primitive is generated by the next higher layer of a
 * device on a beacon-enabled PAN and issued to its MLME to synchronize with
 * the coordinator.
 *
 * Enable receiver and search for beacons for at most an interval of
 * [aBaseSuperframeDuration * ((2 ^ (n))+ 1)] symbols where n is the value of
 * macBeaconOrder. If a beacon frame containing the current PAN identifier of
 * the device is not received, the MLME shall repeat this search. Once the
 * number of missed beacons reaches aMaxLostBeacons, the MLME shall notify
 * the next higher layer by issuing the MLME-SYNC-LOSS.indication primitive
 * with a loss reason of BEACON_LOSS.
 *
 * \ingroup group_mac_req_int
 * @param m Pointer to the MLME sync request parameters.
 */
void mlme_sync_request(uint8_t *m);

#if (MAC_BEACON_NOTIFY_INDICATION == 1)

/**
 * @brief Wrapper function for messages of type mlme_beacon_notify_ind_t
 *
 * This function is a callback for mlme beacon notify indication.
 *
 * \ingroup group_mac_ind_int
 * @param m Pointer to message structure
 */

void mlme_beacon_notify_ind(uint8_t *m);

#endif  /* (MAC_BEACON_NOTIFY_INDICATION == 1) */
#if ((MAC_ORPHAN_INDICATION_RESPONSE == 1) || \
	(MAC_ASSOCIATION_INDICATION_RESPONSE == 1))

/**
 * @brief Wrapper function for messages of type mlme_comm_status_ind_t
 *
 * This function is a callback for mlme comm status indication.
 *
 * \ingroup group_mac_ind_int
 * @param m Pointer to message structure
 */
void mlme_comm_status_ind(uint8_t *m);

#endif  /* ((MAC_ORPHAN_INDICATION_RESPONSE == 1) ||
         *(MAC_ASSOCIATION_INDICATION_RESPONSE == 1)) */

/**
 * @brief Wrapper function for messages of type mlme_reset_conf_t
 *
 * This function is a callback for mlme reset confirm.
 *
 * \ingroup group_mac_conf_int
 * @param m Pointer to message structure
 */
void mlme_reset_conf(uint8_t *m);

/**
 * @brief Wrapper function for messages of type mlme_sync_loss_ind_t
 *
 * This function is a callback for mlme sync loss indication.
 *
 * \ingroup group_mac_ind_int
 * @param m Pointer to message structure
 */
void mlme_sync_loss_ind(uint8_t *m);

#ifdef GTS_SUPPORT
void mlme_gts_request(uint8_t *m);
void mlme_gts_conf(uint8_t *m);
void mlme_gts_ind(uint8_t *m);

#endif /* GTS_SUPPORT */

#if (HIGHEST_STACK_LAYER == MAC)

/**
 * @brief Handles an MLME-SET.request primitive
 *
 * This function handles the MLME-SET.request. The MLME-SET.request primitive
 * attempts to write the given value to the indicated PIB attribute.
 *
 * \ingroup group_mac_req_int
 * @param m Pointer to the request structure
 */
void mlme_set_request(uint8_t *m);

/**
 * @brief Wrapper function for messages of type mlme_set_conf_t
 *
 * This function is a callback for mlme set confirm.
 *
 * \ingroup group_mac_conf_int
 * @param m Pointer to message structure
 */

void mlme_set_conf(uint8_t *m);

#endif  /* (HIGHEST_STACK_LAYER == MAC) */
#if (defined MAC_SECURITY_ZIP) || (defined MAC_SECURITY_2006)

/**
 * @brief Setting of MAC PIB attributes via functional access
 *
 * In case the highest stack layer is above MAC (e.g. NWK or even
 * higher), it is not efficient to change PIB attributes using
 * the standard request / confirm primitive concept via the NHLE_MAC
 * queue. In order to allow a more efficient way to change PIB attributes
 * residing in MAC or TAL, this function replaces the standard primitive
 * access via a functional interface.
 *
 * An additional parameter allows for forcing the transceiver back to sleep
 * after PIB setting. Otherwise the transceiver will stay awake (if it has been
 * woken up before).
 * This enables the higher layer to change several PIB attributes without
 * waking up the transceiver and putting it back to sleep several times.
 *
 * @param attribute PIB attribute to be set
 * @param attribute_index Index of the PIB attribute to be set
 * @param attribute_value Attribute value to be set
 * @param set_trx_to_sleep Set TRX back to sleep after this PIB access if it was
 *        before starting this TRX access. Otherwise the transceiver state will
 *        remain as it is, i.e. in case the transceiver was woken up, it will
 * stay
 *        awake.
 *        The default value for just changing one variable is true, i.e. the
 *        transceiver will be put back to sleep if it has been woken up.
 *
 * @return Status of the attempt to set the TAL PIB attribute:
 *         MAC_UNSUPPORTED_ATTRIBUTE if the PIB attribute was not found
 *         MAC_SUCCESS if the attempt to set the PIB attribute was successful
 *         TAL_BUSY if the TAL is not in an idle state to change PIB attributes
 * \ingroup group_mac_gen_int
 */

retval_t mlme_set(uint8_t attribute, uint8_t attribute_index,
		pib_value_t *attribute_value, bool set_trx_to_sleep);

#else
retval_t mlme_set(uint8_t attribute, pib_value_t *attribute_value,
		bool set_trx_to_sleep);

#endif  /* (MAC_SECURITY_ZIP || MAC_SECURITY_2006)  */

#if (defined MAC_SECURITY_ZIP) || (defined MAC_SECURITY_2006)

/**
 * \ingroup group_mac_gen_int
 */
retval_t mlme_get(uint8_t attribute, pib_value_t *attribute_value,
		uint8_t attribute_index);

#else
retval_t mlme_get(uint8_t attribute, pib_value_t *attribute_value);

#endif  /* (MAC_SECURITY_ZIP || MAC_SECURITY_2006)  */

/**
 * @brief Initializes the MAC sublayer
 *
 * \ingroup group_mac_gen_int
 * @return MAC_SUCCESS  if TAL is intialized successfully else FAILURE
 */
retval_t mac_init(void);

/**
 * @brief Runs the MAC scheduler
 *
 * This function runs the MAC scheduler.
 *
 * MLME and MCPS queues are removed alternately, starting with MLME queue.
 *
 * \ingroup group_mac_gen_int
 * @return true if event is dispatched, false if no event to dispatch.
 */
bool mac_task(void);

/*
 * @brief Helper function to extract the complete address information
 *        of the received frame
 *
 * @param frame_ptr Pointer to first octet of Addressing fields of received
 * frame
 *        (See IEEE 802.15.4-2006 Figure 41)
 *
 * \ingroup group_mac_gen_int
 * @return bool Length of Addressing fields
 */
uint8_t mac_extract_mhr_addr_info(uint8_t *frame_ptr);

/*@}*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _MAC_H */
/* EOF */
