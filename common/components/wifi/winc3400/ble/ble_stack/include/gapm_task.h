/**
 * \file
 *
 * \brief GAP.
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

#ifndef GAP_TASK_H_INCLUDED
#define GAP_TASK_H_INCLUDED

#include "cmn_defs.h"
#include "ll_if.h"

#define APP_ADV_DEV_NAME	"\x4e\x4d\x49\x2D\x42\x4C\x45"
#define APP_ADV_DEV_NAME_LEN	(7)

/// Advertising data maximal length
#define ADV_DATA_MAX_SIZE           (31-3) /*32 including the size in 1st byte*/
/// Scan Response data maximal length
#define SCAN_RESP_DATA_MAX_SIZE     (31) /*32 including the size in 1st byte*/

#define AD_TYPE_NAME				(0x09)

/*32 bytes of zeros*/
#define APP_ZERO_PAD_DATA	"\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0"

/// Minimum to maximum connection interval upon any connection
/// establishment: TGAP(initial_conn_interval)
/// recommended value: 30 to 50 ms ; N * 1.25 ms
#define GAP_INIT_CONN_MIN_INTV                              0x00a0
#define GAP_INIT_CONN_MAX_INTV                              0x00a0

/// RW Defines
#define GAP_INQ_SCAN_INTV                                   0x0140
#define GAP_INQ_SCAN_WIND                                   0x00a0

/// Connection supervision timeout
/// recommended value: 20s
#define GAP_CONN_SUPERV_TIMEOUT                             0x01f4

/// Minimum connection event
/// default value: 0x0000
#define GAP_CONN_MIN_CE                                     0x0000

/// Maximum connection event
/// default value: 0xFFFF
#define GAP_CONN_MAX_CE                                     0x0140

/// Connection latency
/// default value: 0x0000
#define GAP_CONN_LATENCY                                    0x0000

#define ATTC_1ST_REQ_START_HDL		0x0001
#define ATTC_1ST_REQ_END_HDL		0xFFFF

#define EVENT_TYPE_ADVERTISING		0
#define EVENT_TYPE_SCAN_RESP		4

#define GAP_TMR_PRIV_ADDR_INT       0xEA60

typedef struct
{
	///Connection Handle
	uint16_t u16ConHdl;
	///Slave Connected Flag
	uint8_t u8Connected;
	///Address Type
    uint8_t u8AddrType;
	///BD_ADDR
	uint8_t *pu8Addr;
	///Slave Found Flag
    //uint8_t u8Found;
}peers_info;

/// GAP Specific Error
enum gap_err_code
{
    /// No error
    GAP_ERR_NO_ERROR        = 0x00,
    /// Invalid parameters set
    GAP_ERR_INVALID_PARAM   = 0x40,
    /// Problem with protocol exchange, get unexpected response
    GAP_ERR_PROTOCOL_PROBLEM,
    /// Request not supported by software configuration
    GAP_ERR_NOT_SUPPORTED,
    /// Request not allowed in current state.
    GAP_ERR_COMMAND_DISALLOWED,
    /// Requested operation canceled.
    GAP_ERR_CANCELED,
    /// Requested operation timeout.
    GAP_ERR_TIMEOUT,
    /// Link connection lost during operation.
    GAP_ERR_DISCONNECTED,
    /// Search algorithm finished, but no result found
    GAP_ERR_NOT_FOUND,
    /// Request rejected by peer device
    GAP_ERR_REJECTED,
    /// Problem with privacy configuration
    GAP_ERR_PRIVACY_CFG_PB,
    /// Duplicate or invalid advertising data
    GAP_ERR_ADV_DATA_INVALID
};
/// Device discovery search type
enum
{
    /// General Inquiry
    GAP_GEN_INQ_TYPE    = 0x00,
    /// Limited Inquiry
    GAP_LIM_INQ_TYPE,
    /// Known device Inquiry
    GAP_KNOWN_DEV_INQ_TYPE
};

/*Used to accept or reject bond request sent from peer device*/
enum
{
	GAP_BOND_ACCEPT = 0,
	GAP_BOND_REJECT = 1
};

///Filter duplicates
enum
{
    ///Disable filtering of duplicate packets
    SCAN_FILT_DUPLIC_DIS          = 0x00,
    ///Enable filtering of duplicate packets
    SCAN_FILT_DUPLIC_EN,
    ///Enumeration end value for scan duplicate filtering value check
    SCAN_FILT_DUPLIC_END
};

/// Scan mode
enum gap_scan_mode
{
    /// Mode in general discovery
    GAP_GEN_DISCOVERY,
    /// Mode in limited discovery
    GAP_LIM_DISCOVERY,
    /// Observer mode
    GAP_OBSERVER_MODE,
    /// Invalid mode
    GAP_INVALID_MODE
};

/// GAP Interface
enum
{
    /* Default event */
    /// Command Complete event
    GAPM_CMP_EVT = 0x3400,
    /// Event triggered to inform that lower layers are ready
    GAPM_DEVICE_READY_IND,

    /* Default commands */
    /// Reset link layer and the host command
    GAPM_RESET_CMD,
    /// Cancel ongoing operation
    GAPM_CANCEL_CMD,

    /* Device Configuration */
    /// Set device configuration command
    GAPM_SET_DEV_CONFIG_CMD,
    /// Set device name command
    GAPM_SET_DEV_NAME_CMD,
    /// Set device channel map
    GAPM_SET_CHANNEL_MAP_CMD,

    /* Local device information */
    /// Get local device info command
    GAPM_GET_DEV_INFO_CMD,
    /// Local device name indication event
    GAPM_DEV_NAME_IND,
    /// Local device appearance indication event
    GAPM_APPEARANCE_IND,
    /// Local device version indication event
    GAPM_DEV_VERSION_IND,//340A
    /// Local device BD Address indication event
    GAPM_DEV_BDADDR_IND,

    /* White List */
    /// White List Management Command
    GAPM_WHITE_LIST_MGT_CMD,
    /// White List Size indication event
    GAPM_WHITE_LIST_SIZE_IND,

    /* Air Operations */
    /// Set advertising mode Command
    GAPM_START_ADVERTISE_CMD,

    /// Set Scan mode Command
    GAPM_START_SCAN_CMD,
    /// Advertising or scanning report information event
    GAPM_ADV_REPORT_IND,//3410

    /// Set connection initialization Command
    GAPM_START_CONNECTION_CMD,
    /// Name of peer device indication
    GAPM_PEER_NAME_IND,
    /// Confirm connection to a specific device (Connection Operation in Selective mode)
    GAPM_CONNECTION_CFM,

    /* Privacy update events */
    /// Privacy flag value has been updated
    GAPM_UPDATED_PRIVACY_IND,
    /// Reconnection address has been updated
    GAPM_UPDATED_RECON_ADDR_IND,

    /* Security / Encryption Toolbox */
    /// Resolve address command
    GAPM_RESOLV_ADDR_CMD,
    /// Indicate that resolvable random address has been solved
    GAPM_ADDR_SOLVED_IND,
    /// Generate a random address.
    GAPM_GEN_RAND_ADDR_CMD,
    /// Use the AES-128 block in the controller
    GAPM_USE_ENC_BLOCK_CMD,
    ///  AES-128 block result indication
    GAPM_USE_ENC_BLOCK_IND,//341A
    /// Generate a 8-byte random number
    GAPM_GEN_RAND_NB_CMD,
    /// Random Number Indication
    GAPM_GEN_RAND_NB_IND,

    /* Debug  */
    /// Indication containing information about memory usage.
    GAPM_DBG_MEM_INFO_IND,

    /* Local device information -cont */
    /// Advertising channel Tx power level
    GAPM_DEV_ADV_TX_POWER_IND,

    /* Internal messages for timer events, not part of API*/
    /// Limited discoverable timeout indication
    GAPM_LIM_DISC_TO_IND,
    /// Scan timeout indication
    GAPM_SCAN_TO_IND,//3420
    /// Address renewal timeout indication
    GAPM_ADDR_RENEW_TO_IND
};

/// GAP Manager operation type - application interface
enum gapm_operation
{
    /* No Operation (if nothing has been requested)     */
    /* ************************************************ */
    /// No operation.
    GAPM_NO_OP                                     = 0x00,

    /* Default operations                               */
    /* ************************************************ */
    /// Reset BLE subsystem: LL and HL.
    GAPM_RESET,
    /// Cancel currently executed operation.
    GAPM_CANCEL,

    /* Configuration operations                         */
    /* ************************************************ */
    /// Set device configuration
    GAPM_SET_DEV_CONFIG,
    /// Set device name
    GAPM_SET_DEV_NAME,
    /// Set device channel map
    GAPM_SET_CHANNEL_MAP,

    /* Retrieve device information                      */
    /* ************************************************ */
    /// Get Local device name
    GAPM_GET_DEV_NAME,
    /// Get Local device version
    GAPM_GET_DEV_VERSION,
    /// Get Local device BD Address
    GAPM_GET_DEV_BDADDR,

    /* Operation on White list                          */
    /* ************************************************ */
    /// Get White List Size.
    GAPM_GET_WLIST_SIZE,
    /// Add devices in white list.
    GAPM_ADD_DEV_IN_WLIST,
    /// Remove devices form white list.
    GAPM_RMV_DEV_FRM_WLIST,
    /// Clear all devices from white list.
    GAPM_CLEAR_WLIST,

    /* Advertise mode operations                        */
    /* ************************************************ */
    /// Start non connectable advertising
    GAPM_ADV_NON_CONN,
    /// Start undirected connectable advertising
    GAPM_ADV_UNDIRECT,
    /// Start directed connectable advertising
    GAPM_ADV_DIRECT,

    /* Scan mode operations                             */
    /* ************************************************ */
    /// Start active scan operation
    GAPM_SCAN_ACTIVE,
    /// Start passive scan operation
    GAPM_SCAN_PASSIVE,

    /* Connection mode operations                       */
    /* ************************************************ */
    /// Direct connection operation
    GAPM_CONNECTION_DIRECT,
    /// Automatic connection operation
    GAPM_CONNECTION_AUTO,
    /// Selective connection operation
    GAPM_CONNECTION_SELECTIVE,
    /// Name Request operation (requires to start a direct connection)
    GAPM_CONNECTION_NAME_REQUEST,

    /* Security / Encryption Toolbox                    */
    /* ************************************************ */
    /// Resolve device address
    GAPM_RESOLV_ADDR,
    /// Generate a random address
    GAPM_GEN_RAND_ADDR,
    /// Use the controller's AES-128 block
    GAPM_USE_ENC_BLOCK,
    /// Generate a 8-byte random number
    GAPM_GEN_RAND_NB,

    /* DEBUG                                            */
    /* ************************************************ */
    /// Get memory usage
    GAPM_DBG_GET_MEM_INFO,
    /// Perform a platform reset
    GAPM_PLF_RESET,

    /* Retrieve device information - cont               */
    /* ************************************************ */
    /// Get device advertising power level
    GAPM_GET_DEV_ADV_TX_POWER,

    /// Last GAPM operation flag
    GAPM_LAST
};

/// Own BD address source of the device
enum gapm_own_addr_src
{
   /// Public Address
   GAPM_PUBLIC_ADDR,
   /// Provided random address
   GAPM_PROVIDED_RND_ADDR,
   /// Provided static random address
   GAPM_GEN_STATIC_RND_ADDR,
   /// Generated resolvable private random address
   GAPM_GEN_RSLV_ADDR,
   /// Generated non-resolvable private random address
   GAPM_GEN_NON_RSLV_ADDR,
   /// Provided Reconnection address
   GAPM_PROVIDED_RECON_ADDR
};


/// Advertising mode
enum gap_adv_mode
{
    /// Mode in non-discoverable
    GAP_NON_DISCOVERABLE,
    /// Mode in general discoverable
    GAP_GEN_DISCOVERABLE,
    /// Mode in limited discoverable
    GAP_LIM_DISCOVERABLE,
    /// Broadcaster mode which is a non discoverable and non connectable mode.
    GAP_BROADCASTER_MODE
};

/****************** GAP Role **********************/
enum gap_role
{
    /// No role set yet
    GAP_NO_ROLE    = 0x00,

    /// Observer role
    GAP_OBSERVER_SCA    = 0x01,

    /// Broadcaster role
    GAP_BROADCASTER_ADV = 0x02,

    /// Master/Central role
    GAP_CENTRAL_MST     = (0x04 | GAP_OBSERVER_SCA),

    /// Peripheral/Slave role
    GAP_PERIPHERAL_SLV  = (0x08 | GAP_BROADCASTER_ADV)
};

/// Device Attribute write permission requirement
enum gapm_write_att_perm
{
    /// Disable write access
    GAPM_WRITE_DISABLE  = PERM(WR, DISABLE),
    /// Enable write access
    GAPM_WRITE_ENABLE   = PERM(WR, ENABLE),
    /// Write access requires unauthenticated link
    GAPM_WRITE_UNAUTH   = PERM(WR, UNAUTH),
    /// Write access requires authenticated link
    GAPM_WRITE_AUTH     = PERM(WR, AUTH)
};

uint8_t gapm_reset_req_handler (void);

uint8_t gapm_set_dev_name_handler(uint8_t len, uint8_t* name);

uint8_t gapm_set_dev_config_cmd_handler(uint8_t u8Role, uint8_t *pu8Key, uint16_t u16Appearance, uint8_t u8AppWrPerm,
				uint8_t u8NameWrPerm, uint16_t u16MaxMTU, uint16_t u16ConIntMin, uint16_t u16ConIntMax, uint16_t u16ConLatency, 
				uint16_t u16SupervTo, uint8_t u8Flags);

void gapm_start_adv_cmd_handler (uint8_t u8OpCode, uint8_t u8AddrSrc, uint16_t u16RenewDur, 
		uint8_t *pu8BdAddr,uint8_t peer_addr_type ,uint8_t *peerBdAddr, uint16_t u16MinIntv, uint16_t u16MaxIntv, uint8_t u8ChnlMap, uint8_t u8Mode ,
	uint8_t u8AdvFiltPolicy, uint8_t u8AdvDataLen, uint8_t *pu8AdvData, uint8_t u8ScnRespLen, uint8_t *pu8ScnRespData);

void gapm_start_scan_cmd_handler (uint8_t u8OpCode, uint8_t u8AddrType, uint16_t u16RenewDur, 
		uint8_t *pu8BdAddr, uint16_t u16ScanInterval, uint16_t u16ScanWin, uint8_t u8ScanMode, uint8_t u8FiltPolicy,
		uint8_t u8FilterDuplic);

void gapm_start_connection_cmd_handler(uint8_t u8OpCode, uint8_t u8AddrType, uint16_t u16RenewDur,
			uint8_t *pu8BdAddr, uint16_t u16ScanInterval, uint16_t u16ScanWin, uint16_t u16ConIntvMin, 
			uint16_t u16ConIntvMax, uint16_t u16ConLatency, uint16_t u16SupervTO, uint16_t u16CeMin, 
			uint16_t u16CeMAx, uint8_t u8NbOfPeers, at_ble_addr_t *peers);

uint8_t gapm_cancel_cmd_handler (void);

void gapm_connection_cfm_handler ( uint8_t *pu8PeerBdAddr, uint8_t u8PeerAddrType,uint16_t u16ConIntvMin, 
			uint16_t u16ConIntvMax, uint16_t u16ConLatency, uint16_t u16SupervTO, uint16_t u16CeMin, 
			uint16_t u16CeMAx);

uint8_t  gapm_white_list_mgm_cmd(uint8_t operation, uint8_t addr_type, uint8_t* address);

void gapm_adv_report_evt_handler(uint8_t* data,
	at_ble_scan_info_t* param);

void gapm_dev_bdaddr_ind_handler(uint8_t* data,at_ble_rand_addr_changed_t* param);

void gapm_resolv_addr_cmd_handler(uint8_t nb_key , uint8_t* rand_addr , uint8_t* irk);

void gapm_addr_solved_ind_handler(uint8_t* data , at_ble_resolv_rand_addr_status_t* params);

at_ble_events_t gapm_cmp_evt(uint8_t* data, void* params);

#endif /* GAP_TASK_H_INCLUDED */
