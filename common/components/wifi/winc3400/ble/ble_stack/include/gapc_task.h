/**
 * \file
 *
 * \brief GAPC.
 *
 * Copyright (c) 2016 Atmel Corporation. All rights reserved.
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

#ifndef GAPC_TASK_H_INCLUDED
#define GAPC_TASK_H_INCLUDED

#include "cmn_defs.h"

typedef struct
{
	/// RSSI value
	uint8_t rssi;
}gapc_con_rssi_ind;

void gapc_connection_cfm_handler(uint8_t *pu8LocalKey,  uint32_t u32LocalSignCntr, uint8_t *pu8RemoteKey, 
								 uint32_t u32RemoteSignCntr, uint8_t u8Auth, uint8_t u8Authorize, uint16_t u16ConHdl);

void gapc_bond_cfm_handler_pair_resp(uint8_t u8Accept, uint8_t u8IoCap, uint8_t u8OOB, uint8_t u8Auth, 
								 uint8_t u8KeySize, uint8_t u8IkeyDist, uint8_t u8RkeyDist, uint8_t u8SecReq,
								 uint16_t u16ConHdl);

void gapc_get_tx_pwr_req_handler(uint16_t u16ConHdl);

void gapc_set_tx_pwr_req_handler(uint16_t u16ConHdl, uint8_t u8txpow);

void gapc_disconnect_cmd_handler(uint8_t reason, uint16_t handle);

void gapc_param_update_cmd_handler(uint16_t handle,
	uint16_t con_intv_min, uint16_t con_intv_max, uint16_t con_latency, uint16_t superv_to,
	uint16_t ce_len_min, uint16_t ce_len_max);

void gapc_param_update_cfm_handler(uint16_t conn_handle, 
		uint16_t ce_len_min, uint16_t ce_len_max);

void gapc_bond_cmd_handler(uint16_t conn_handle, uint8_t io_cababilities,
	uint8_t oob_avaiable, uint8_t auth_req, uint8_t max_key_size,
	uint8_t initiator_keys, uint8_t responder_keys, uint8_t desired_auth);

void gapc_security_cmd_handler(uint16_t conn_handle, uint8_t auth_req);

void gapc_bond_cfm_handler_key_exch(uint8_t u8Req,  uint8_t u8Accept, uint8_t* key,
				uint16_t u16ConHdl);

void gapc_encrypt_cmd_handler(uint16_t conn_handle, uint8_t* key, 
	uint16_t ediv, uint8_t* rand, uint8_t key_size , uint8_t auth);

void gapc_encrypt_cfm_handler(uint16_t conn_handle, uint8_t auth ,uint8_t key_found, 
	uint8_t* key, uint8_t key_size);

void gapc_con_req_ind(uint8_t* data);

void gapc_disconnect_ind(uint8_t* data, at_ble_disconnected_t* param);

void gapc_param_updated_ind(uint16_t src, uint8_t* data, at_ble_conn_param_update_done_t* param);

void gapc_param_update_req_ind(uint16_t src, uint8_t* data,
	at_ble_conn_param_update_request_t *param);

void gapc_bond_req(uint16_t src, at_ble_pair_request_t* param);

void gapc_key_exch(uint8_t u8Req, uint16_t ConHdl);

uint8_t gapc_bond_req_ind(uint16_t src, uint8_t* data, void*param);

at_ble_events_t gapc_bond_ind(uint16_t src, uint8_t* data, at_ble_pair_done_t* param);

at_ble_events_t gapc_cmp_evt(uint16_t src, uint8_t* data,
	at_ble_encryption_status_changed_t* params);

void gapc_encrypt_req_ind(uint16_t src, uint8_t* data, 
	at_ble_encryption_request_t* params);

void gapc_encrypt_ind(uint16_t src ,uint8_t* data ,at_ble_encryption_status_changed_t* params);

void gapc_sec_req_ind(uint16_t src, uint8_t* data,at_ble_slave_sec_request_t* params);

void gapc_get_info_cmd_handler(uint16_t conn_handle, uint8_t operation);

void gapc_con_rssi_ind_parser(uint16_t src, uint8_t* data, gapc_con_rssi_ind* params);

enum gapc_msg_id
{
    /* Default event */
    /// Command Complete event
    GAPC_CMP_EVT = 0x3800,

    /* Connection state information */
    /// Indicate that a connection has been established
    GAPC_CONNECTION_REQ_IND,
    /// Set specific link data configuration.
    GAPC_CONNECTION_CFM,

    /// Indicate that a link has been disconnected
    GAPC_DISCONNECT_IND,

    /* Link management command */
    /// Request disconnection of current link command.
    GAPC_DISCONNECT_CMD,

    /* Peer device info */
    /// Retrieve information command
    GAPC_GET_INFO_CMD,
    /// Name of peer device indication
    GAPC_PEER_NAME_IND,
    /// Indication of peer version info
    GAPC_PEER_VERSION_IND,
    /// Indication of peer features info
    GAPC_PEER_FEATURES_IND,

    /// Indication of ongoing connection RSSI
    GAPC_CON_RSSI_IND,
    /// Indication of peer privacy info
    GAPC_PRIVACY_IND,//380A
    /// Indication of peer reconnection address info
    GAPC_RECON_ADDR_IND,

    /* Privacy configuration */
    /// Set Privacy flag command.
    GAPC_SET_PRIVACY_CMD,
    /// Set Reconnection Address Value command.
    GAPC_SET_RECON_ADDR_CMD,

    /* Connection parameters update */
    /// Perform update of connection parameters command
    GAPC_PARAM_UPDATE_CMD,
    /// Request of updating connection parameters indication
    GAPC_PARAM_UPDATE_REQ_IND,
    /// Master confirm or not that parameters proposed by slave are accepted or not
    GAPC_PARAM_UPDATE_CFM,//3810
    /// Connection parameters updated indication
    GAPC_PARAM_UPDATED_IND,

    /* Bonding procedure */
    /// Start Bonding command procedure
    GAPC_BOND_CMD,
    /// Bonding requested by peer device indication message.
    GAPC_BOND_REQ_IND,
    /// Confirm requested bond information.
    GAPC_BOND_CFM,
    /// Bonding information indication message
    GAPC_BOND_IND,

    /* Encryption procedure */
    /// Start Encryption command procedure
    GAPC_ENCRYPT_CMD,
    /// Encryption requested by peer device indication message.
    GAPC_ENCRYPT_REQ_IND,
    /// Confirm requested Encryption information.
    GAPC_ENCRYPT_CFM,
    /// Encryption information indication message
    GAPC_ENCRYPT_IND,

    /* Security request procedure */
    /// Start Security Request command procedure
    GAPC_SECURITY_CMD,//381A
    /// Security requested by peer device indication message
    GAPC_SECURITY_IND,

    /* Signature procedure */
    /// Indicate the current sign counters to the application
    GAPC_SIGN_COUNTER_IND,

    /* Device information */
    /// Indication of ongoing connection Channel Map
    GAPC_CON_CHANNEL_MAP_IND,


    /* Internal messages for timer events, not part of API*/
    /// Parameter update procedure timeout indication
    GAPC_PARAM_UPDATE_TO_IND,
	   /// Get Tx Power value
    GAPC_GET_TX_PWR_REQ,
    /// Get Tx Power value complete event
    GAPC_GET_TX_PWR_REQ_CMP_EVT,//3820
     /// SET Tx Power value
    GAPC_SET_TX_PWR_REQ,
    /// Read Tx Power value complete event
    GAPC_SET_TX_PWR_REQ_CMP_EVT

};


/// request operation type - application interface
enum gapc_operation
{
    /*                 Operation Flags                  */
    /* No Operation (if nothing has been requested)     */
    /* ************************************************ */
    /// No operation
    GAPC_NO_OP                                    = 0x00,

    /* Connection management */
    /// Disconnect link
    GAPC_DISCONNECT,

    /* Connection information */
    /// Retrieve name of peer device.
    GAPC_GET_PEER_NAME,
    /// Retrieve peer device version info.
    GAPC_GET_PEER_VERSION,
    /// Retrieve peer device features.
    GAPC_GET_PEER_FEATURES,
    /// Retrieve connection RSSI.
    GAPC_GET_CON_RSSI,
    /// Retrieve Privacy Info.
    GAPC_GET_PRIVACY,
    /// Retrieve Reconnection Address Value.
    GAPC_GET_RECON_ADDR,

    /* Privacy Configuration */
    /// Set Privacy flag.
    GAPC_SET_PRIVACY,
    /// Set Reconnection Address Value.
    GAPC_SET_RECON_ADDR,

    /* Connection parameters update */
    /// Perform update of connection parameters.
    GAPC_UPDATE_PARAMS,

    /* Security procedures */
    /// Start bonding procedure.
    GAPC_BOND,
    /// Start encryption procedure.
    GAPC_ENCRYPT,
    /// Start security request procedure
    GAPC_SECURITY_REQ,

    /* Connection information */
    /// Retrieve Connection Channel MAP.
    GAPC_GET_CON_CHANNEL_MAP,


    /// Last GAPC operation flag
    GAPC_LAST
};
/// Authentication mask
enum gap_auth_mask
{
    /// No Flag set
    GAP_AUTH_NONE = 0,
    /// Bond authentication
    GAP_AUTH_BOND = (1 << 0),
    /// Man In the middle protection
    GAP_AUTH_MITM = (1 << 2)
};
/// Authentication Requirements
enum gap_auth
{
    /// No MITM No Bonding
    GAP_AUTH_REQ_NO_MITM_NO_BOND = (GAP_AUTH_NONE),
    /// No MITM Bonding
    GAP_AUTH_REQ_NO_MITM_BOND    = (GAP_AUTH_BOND),
    /// MITM No Bonding
    GAP_AUTH_REQ_MITM_NO_BOND    = (GAP_AUTH_MITM),
    /// MITM and Bonding
    GAP_AUTH_REQ_MITM_BOND       = (GAP_AUTH_MITM | GAP_AUTH_BOND),
    GAP_AUTH_REQ_LAST
};


/// Bond event type.
enum gapc_bond
{
    /// Bond Pairing request
    GAPC_PAIRING_REQ,
    /// Respond to Pairing request
    GAPC_PAIRING_RSP,

    /// Pairing Finished information
    GAPC_PAIRING_SUCCEED,
    /// Pairing Failed information
    GAPC_PAIRING_FAILED,

    /// Used to retrieve pairing Temporary Key
    GAPC_TK_EXCH,
    /// Used for Identity Resolving Key exchange
    GAPC_IRK_EXCH,
    /// Used for Connection Signature Resolving Key exchange
    GAPC_CSRK_EXCH,
    /// Used for Long Term Key exchange
    GAPC_LTK_EXCH,

    /// Bond Pairing request issue, Repeated attempt
    GAPC_REPEATED_ATTEMPT
};


/// OOB Data Present Flag Values
enum gap_oob
{
    /// OOB Data not present
    GAP_OOB_AUTH_DATA_NOT_PRESENT = 0x00,
    /// OOB data present
    GAP_OOB_AUTH_DATA_PRESENT,
    GAP_OOB_AUTH_DATA_LAST
};

/// Key Distribution Flags
enum gap_kdist
{
    /// No Keys to distribute
    GAP_KDIST_NONE = 0x00,
    /// Encryption key in distribution
    GAP_KDIST_ENCKEY = (1 << 0),
    /// IRK (ID key)in distribution
    GAP_KDIST_IDKEY  = (1 << 1),
    /// CSRK(Signature key) in distribution
    GAP_KDIST_SIGNKEY= (1 << 2),

    GAP_KDIST_LAST =   (1 << 3)
};

/// Security Defines
enum gap_sec_req
{
    /// No security (no authentication and encryption)
    GAP_NO_SEC = 0x00,
    /// Unauthenticated pairing with encryption
    GAP_SEC1_NOAUTH_PAIR_ENC,
    /// Authenticated pairing with encryption
    GAP_SEC1_AUTH_PAIR_ENC,
    /// Unauthenticated pairing with data signing
    GAP_SEC2_NOAUTH_DATA_SGN,
    /// Authentication pairing with data signing
    GAP_SEC2_AUTH_DATA_SGN,
    /// Unrecognized security
    GAP_SEC_UNDEFINED
};

/// IO Capability Values
enum gap_io_cap
{
    /// Display Only
    GAP_IO_CAP_DISPLAY_ONLY = 0x00,
    /// Display Yes No
    GAP_IO_CAP_DISPLAY_YES_NO,
    /// Keyboard Only
    GAP_IO_CAP_KB_ONLY,
    /// No Input No Output
    GAP_IO_CAP_NO_INPUT_NO_OUTPUT,
    /// Keyboard Display
    GAP_IO_CAP_KB_DISPLAY,
    GAP_IO_CAP_LAST
};

/// TK Type
enum gap_tk_type
{
    ///  TK get from out of band method
    GAP_TK_OOB         = 0x00,
    /// TK generated and shall be displayed by local device
    GAP_TK_DISPLAY,
    /// TK shall be entered by user using device keyboard
    GAP_TK_KEY_ENTRY
};

#endif /* GAPC_TASK_H_INCLUDED */
