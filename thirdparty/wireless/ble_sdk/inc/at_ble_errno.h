/**
 ****************************************************************************************
 *
 * @file at_ble_errno.h
 *
 * @brief Atmel BLE Host Error Codes
 *
 * This module contains the an enumeration of error codes reported by chip to host and their descriptions
 *
 *
 *
 * Copyright (c) 2016 Atmel Corporation. All rights reserved.
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice, this
 *  list of conditions and the following disclaimer.
 *
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *  this list of conditions and the following disclaimer in the documentation
 *  and/or other materials provided with the distribution.
 *
 *  3. The name of Atmel may not be used to endorse or promote products derived from this software
 *  without specific prior written permission.
 *
 *  4. This software may only be redistributed and used in connection with an Atmel microcontroller product.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
  * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 *  OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 *  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 *  TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ****************************************************************************************
 */


#ifndef _AT_BLE_ERRNO_H
#define _AT_BLE_ERRNO_H


/** @ingroup error_codes_group
 *  @brief Enumeration for BLE Error Codes
 */
typedef enum
{
    ///Generic errors
    ///Success
    AT_BLE_SUCCESS  = 0x00,

    ///ATT Error Codes
    ///Handle is invalid
    AT_BLE_ATT_INVALID_HANDLE  = 0x01,
    ///Read permission disabled
    AT_BLE_ATT_READ_NOT_PERMITTED  = 0x02,
    ///Write permission disabled
    AT_BLE_ATT_WRITE_NOT_PERMITTED  = 0x03,
    ///Incorrect PDU
    AT_BLE_ATT_INVALID_PDU  = 0x04,
    ///Authentication privilege not enough
    AT_BLE_ATT_INSUFF_AUTHEN  = 0x05,
    ///Request not supported or not understood
    AT_BLE_ATT_REQUEST_NOT_SUPPORTED  = 0x06,
    ///Incorrect offset value
    AT_BLE_ATT_INVALID_OFFSET  = 0x07,
    ///Authorization privilege not enough
    AT_BLE_ATT_INSUFF_AUTHOR  = 0x08,
    ///Capacity queue for reliable write reached
    AT_BLE_ATT_PREPARE_QUEUE_FULL  = 0x09,
    ///Attribute requested not existing
    AT_BLE_ATT_ATTRIBUTE_NOT_FOUND  = 0x0A,
    ///Attribute requested not long
    AT_BLE_ATT_ATTRIBUTE_NOT_LONG  = 0x0B,
    ///Encryption size not sufficient
    AT_BLE_ATT_INSUFF_ENC_KEY_SIZE  = 0x0C,
    ///Invalid length of the attribute value
    AT_BLE_ATT_INVALID_ATTRIBUTE_VAL_LEN  = 0x0D,
    ///Operation not fit to condition
    AT_BLE_ATT_UNLIKELY_ERR = 0x0E,
    ///Attribute requires encryption before operation
    AT_BLE_ATT_INSUFF_ENC  = 0x0F,
    ///Attribute grouping not supported
    AT_BLE_ATT_UNSUPP_GRP_TYPE  = 0x10,
    ///Resources not sufficient to complete the request
    AT_BLE_ATT_INSUFF_RESOURCE  = 0x11,

    ///L2C Specific Error
    ///Message cannot be sent because connection lost. (disconnected)
    AT_BLE_L2C_CONNECTION_LOST  = 0x30,
    ///Invalid PDU length exceed MTU
    AT_BLE_L2C_INVALID_MTU_EXCEED  = 0x31,
    ///Invalid PDU length exceed MPS
    AT_BLE_L2C_ERR_INVALID_MPS_EXCEED        = 0x32,
    ///Invalid Channel ID
    AT_BLE_L2C_INVALID_CID  = 0x33,
    ///Invalid PDU
    AT_BLE_L2C_INVALID_PDU  = 0x34,
    /// Connection refused - no resources available
    AT_BLE_L2C_ERR_NO_RES_AVAIL              = 0x35,
    /// Connection refused - insufficient authentication
    AT_BLE_L2C_ERR_INSUFF_AUTHEN             = 0x36,
    /// Connection refused - insufficient authorization
    AT_BLE_L2C_ERR_INSUFF_AUTHOR             = 0x37,
    /// Connection refused - insufficient encryption key size
    AT_BLE_L2C_ERR_INSUFF_ENC_KEY_SIZE       = 0x38,
    /// Connection Refused - insufficient encryption
    AT_BLE_L2C_ERR_INSUFF_ENC                = 0x39,
    /// Connection refused - LE_PSM not supported
    AT_BLE_L2C_ERR_LEPSM_NOT_SUPP            = 0x3A,
    /// No more credit
    AT_BLE_L2C_ERR_INSUFF_CREDIT              = 0x3B,
    /// Command not understood by peer device
    AT_BLE_L2C_ERR_NOT_UNDERSTOOD            = 0x3C,

    ///GAP Error Codes
    ///Invalid parameters set
    AT_BLE_GAP_INVALID_PARAM  = 0x40,
    ///Problem with protocol exchange, get unexpected response
    AT_BLE_GAP_PROTOCOL_PROBLEM  = 0x41,
    ///Request not supported by software configuration
    AT_BLE_GAP_NOT_SUPPORTED  = 0x42,
    ///Request not allowed in current state.
    AT_BLE_GAP_COMMAND_DISALLOWED  = 0x43,
    ///Requested operation canceled.
    AT_BLE_GAP_CANCELED  = 0x44,
    ///Requested operation timeout.
    AT_BLE_GAP_TIMEOUT  = 0x45,
    ///Link connection lost during operation.
    AT_BLE_GAP_DISCONNECTED  = 0x46,
    ///Search algorithm finished, but no result found
    AT_BLE_GAP_NOT_FOUND  = 0x47,
    ///Request rejected by peer device
    AT_BLE_GAP_REJECTED  = 0x48,
    ///Problem with privacy configuration
    AT_BLE_GAP_PRIVACY_CFG_PB  = 0x49,
    ///Duplicate or invalid advertising data
    AT_BLE_GAP_ADV_DATA_INVALID  = 0x4A,
    ///Insufficient resources
    AT_BLE_GAP_INSUFF_RESOURCES  = 0x4B,
    ///Unexpected Error
    AT_BLE_GAP_UNEXPECTED  = 0x4C,

    ///GATT Error Codes
    ///Problem with ATTC protocol response
    AT_BLE_GATT_INVALID_ATT_LEN  = 0x50 ,
    ///Error in service search
    AT_BLE_GATT_INVALID_TYPE_IN_SVC_SEARCH  = 0x51,
    ///Invalid write data
    AT_BLE_GATT_WRITE  = 0x52,
    ///Signed write error
    AT_BLE_GATT_SIGNED_WRITE  = 0x53,
    ///No attribute client defined
    AT_BLE_GATT_ATT_CLIENT_MISSING  = 0x54,
    ///No attribute server defined
    AT_BLE_GATT_ATT_SERVER_MISSING  = 0x55,
    ///Permission set in service/attribute are invalid
    AT_BLE_GATT_INVALID_PERM  = 0x56,

    ///SMP Errors detected on local device
    ///The user input of passkey failed, for example, the user cancelled the operation.
    AT_BLE_SMP_LOC_PASSKEY_ENTRY_FAILED  = 0x61,
    ///The OOB Data is not available.
    AT_BLE_SMP_LOC_OOB_NOT_AVAILABLE  = 0x62,
    ///The pairing procedure cannot be performed as authentication requirements cannot be met due to IO capabilities of one or both devices.
    AT_BLE_SMP_LOC_AUTH_REQ  = 0x63,
    ///The confirm value does not match the calculated confirm value.
    AT_BLE_SMP_LOC_CONF_VAL_FAILED  = 0x64,
    ///Pairing is not supported by the device.
    AT_BLE_SMP_LOC_PAIRING_NOT_SUPP  = 0x65,
    ///The resultant encryption key size is insufficient for the security requirements of this device.
    AT_BLE_SMP_LOC_ENC_KEY_SIZE  = 0x66,
    ///The SMP command received is not supported on this device.
    AT_BLE_SMP_LOC_CMD_NOT_SUPPORTED  = 0x67,
    ///Pairing failed due to an unspecified reason.
    AT_BLE_SMP_LOC_UNSPECIFIED_REASON  = 0x68,
    ///Pairing or Authentication procedure is disallowed because too little time has elapsed since last pairing request or security request.
    AT_BLE_SMP_LOC_REPEATED_ATTEMPTS  = 0x69,
    ///The command length is invalid or a parameter is outside of the specified range.
    AT_BLE_SMP_LOC_INVALID_PARAM   = 0x6A ,
    ///The request sent by the HL cannot be handled for some reasons (unauthorized source task, role, ...)
    AT_BLE_SMP_LOC_REQ_DISALLOWED  = 0x6B,
    ///An error has been received from the controller upon an encryption request.
    AT_BLE_SMP_LOC_LL_ERROR = 0x6C,
    ///The provided resolvable address has not been resolved.
    AT_BLE_SMP_LOC_ADDR_RESOLV_FAIL  = 0x6D,
    ///The Signature Verification Failed.
    AT_BLE_SMP_LOC_SIGN_VERIF_FAIL  = 0x6E,
    ///The command cannot be executed because a SMP timeout has been raised during the connection.
    AT_BLE_SMP_LOC_TIMEOUT = 0x6F,

    ///SMP Errors detected by remote device
    ///The user input of passkey failed, for example, the user cancelled the operation.
    AT_BLE_SMP_REM_PASSKEY_ENTRY_FAILED  = 0x71,
    ///The OOB Data is not available.
    AT_BLE_SMP_REM_OOB_NOT_AVAILABLE  = 0x72,
    ///The pairing procedure cannot be performed as authentication requirements cannot be met due to IO capabilities of one or both
    AT_BLE_SMP_REM_AUTH_REQ  = 0x73,
    ///The confirm value does not match the calculated confirm value.
    AT_BLE_SMP_REM_CONF_VAL_FAILED = 0x74,
    ///Pairing is not supported by the device.
    AT_BLE_SMP_REM_PAIRING_NOT_SUPP  = 0x75,
    ///The resultant encryption key size is insufficient for the security requirements of this device.
    AT_BLE_SMP_REM_ENC_KEY_SIZE  = 0x76,
    ///The SMP command received is not supported on this device.
    AT_BLE_SMP_REM_CMD_NOT_SUPPORTED  = 0x77 ,
    ///Pairing failed due to an unspecified reason.
    AT_BLE_SMP_REM_UNSPECIFIED_REASON  = 0x78,
    ///Pairing or Authentication procedure is disallowed because too little time has elapsed since last pairing request or security request.
    AT_BLE_SMP_REM_REPEATED_ATTEMPTS  = 0x79,
    ///The command length is invalid or a parameter is outside of the specified range.
    AT_BLE_SMP_REM_INVALID_PARAM  = 0x7A,
    ///The encryption procedure failed because the slave device didn't find the LTK needed to start an encryption session.
    AT_BLE_SMP_REM_ENC_KEY_MISSING  = 0x7B,
    ///The encryption procedure failed because the slave device doesn't support the encryption feature.
    AT_BLE_SMP_REM_ENC_NOT_SUPPORTED = 0x7C,
    ///A timeout has occurred during the start encryption session.
    AT_BLE_SMP_REM_ENC_TIMEOUT  = 0x7D,

    /// Profile error codes
    ///Application Error
    AT_BLE_ATT_APP_ERROR  = 0x80,
    ///Application Error
    AT_BLE_PRF_APP_ERROR  = 0x80,
    ///Invalid parameter in request
    AT_BLE_PRF_INVALID_PARAM  = 0x81,
    ///Nonexistent handle for sending a read/write characteristic request
    AT_BLE_PRF_INEXISTENT_HDL  = 0x82,
    ///Discovery stopped due to missing attribute according to specification
    AT_BLE_PRF_STOP_DISC_CHAR_MISSING  = 0x83,
    ///Too many SVC instances found -> protocol violation
    AT_BLE_PRF_MULTIPLE_SVC  = 0x84,
    ///Discovery stopped due to found attribute with incorrect properties
    AT_BLE_PRF_STOP_DISC_WRONG_CHAR_PROP  = 0x85,
    ///Too many Char. instances found-> protocol violation
    AT_BLE_PRF_MULTIPLE_CHAR  = 0x86,
    ///Attribute write not allowed
    AT_BLE_PRF_NOT_WRITABLE  = 0x87,
    ///Attribute read not allowed
    AT_BLE_PRF_NOT_READABLE  = 0x88,
    ///Request not allowed
    AT_BLE_PRF_REQ_DISALLOWED  = 0x89,
    ///Notification Not Enabled
    AT_BLE_PRF_NTF_DISABLED  = 0x8A,
    ///Indication Not Enabled
    AT_BLE_PRF_IND_DISABLED  = 0x8B,
    ///Feature not supported by profile
    AT_BLE_PRF_FEATURE_NOT_SUPPORTED  = 0x8C,
    ///Read value has an unexpected length
    AT_BLE_PRF_UNEXPECTED_LEN  = 0x8D,
    ///Disconnection occurs
    AT_BLE_PRF_DISCONNECTED  = 0x8E,
    ///Procedure Timeout
    AT_BLE_PRF_PROC_TIMEOUT  = 0x8F,

    ///LL Error codes conveyed to upper layer
    ///Unknown HCI Command
    AT_BLE_LL_UNKNOWN_HCI_COMMAND  = 0x91,

    ///Unknown Connection Identifier
    AT_BLE_LL_UNKNOWN_CONNECTION_ID  = 0x92,
    ///Hardware Failure
    AT_BLE_LL_HARDWARE_FAILURE  = 0x93,
    ///BT Page Timeout
    AT_BLE_LL_PAGE_TIMEOUT  = 0x94,
    ///Authentication failure
    AT_BLE_LL_AUTH_FAILURE  = 0x95,
    ///Pin code missing
    AT_BLE_LL_PIN_MISSING  = 0x96,
    ///Memory capacity exceed
    AT_BLE_LL_MEMORY_CAPA_EXCEED  = 0x97,
    ///Connection Timeout
    AT_BLE_LL_CON_TIMEOUT  = 0x98,
    ///Connection limit Exceed
    AT_BLE_LL_CON_LIMIT_EXCEED  = 0x99,
    ///Synchronous Connection limit exceed
    AT_BLE_LL_SYNC_CON_LIMIT_DEV_EXCEED  = 0x9A,
    ///ACL Connection exits
    AT_BLE_LL_ACL_CON_EXISTS  = 0x9B,
    ///Command Disallowed
    AT_BLE_LL_COMMAND_DISALLOWED  = 0x9C,
    ///Connection rejected due to limited resources
    AT_BLE_LL_CONN_REJ_LIMITED_RESOURCES  = 0x9D,
    ///Connection rejected due to security reason
    AT_BLE_LL_CONN_REJ_SECURITY_REASONS  = 0x9E,
    ///Connection rejected due to unacceptable BD Addr
    AT_BLE_LL_CONN_REJ_UNACCEPTABLE_BDADDR  = 0x9F,
    ///Connection rejected due to Accept connection timeout
    AT_BLE_LL_CONN_ACCEPT_TIMEOUT_EXCEED  = 0xA0,
    ///Not Supported
    AT_BLE_LL_UNSUPPORTED  = 0xA1,
    ///invalid parameters
    AT_BLE_LL_INVALID_HCI_PARAM  = 0xA2,
    ///Remote user terminate connection
    AT_BLE_LL_REMOTE_USER_TERM_CON  = 0xA3,
    ///Remote device terminate connection due to low resources
    AT_BLE_LL_REMOTE_DEV_TERM_LOW_RESOURCES  = 0xA4,
    ///Remote device terminate connection due to power off
    AT_BLE_LL_REMOTE_DEV_POWER_OFF  = 0xA5,
    ///Connection terminated by local host
    AT_BLE_LL_CON_TERM_BY_LOCAL_HOST  = 0xA6,
    ///Repeated attempts
    AT_BLE_LL_REPEATED_ATTEMPTS  = 0xA7,
    ///Pairing not Allowed
    AT_BLE_LL_PAIRING_NOT_ALLOWED  = 0xA8,
    ///Unknown PDU Error
    AT_BLE_LL_UNKNOWN_LMP_PDU  = 0xA9,
    ///Unsupported remote feature
    AT_BLE_LL_UNSUPPORTED_REMOTE_FEATURE  = 0xAA,
    ///Sco Offset rejected
    AT_BLE_LL_SCO_OFFSET_REJECTED  = 0xAB,
    ///SCO Interval Rejected
    AT_BLE_LL_SCO_INTERVAL_REJECTED  = 0xAC,
    ///SCO air mode Rejected
    AT_BLE_LL_SCO_AIR_MODE_REJECTED  = 0xAD,
    ///Invalid LMP parameters
    AT_BLE_LL_INVALID_LMP_PARAM  = 0xAE,
    ///Unspecified error
    AT_BLE_LL_UNSPECIFIED_ERROR  = 0xAF,
    ///Unsupported LMP Parameter value
    AT_BLE_LL_UNSUPPORTED_LMP_PARAM_VALUE  = 0xB0,
    ///Role Change Not allowed
    AT_BLE_LL_ROLE_CHANGE_NOT_ALLOWED  = 0xB1,
    ///LMP Response timeout
    AT_BLE_LL_LMP_RSP_TIMEOUT  = 0xB2,
    ///LMP Collision
    AT_BLE_LL_LMP_COLLISION  = 0xB3,
    ///LMP Pdu not allowed
    AT_BLE_LL_LMP_PDU_NOT_ALLOWED  = 0xB4,
    ///Encryption mode not accepted
    AT_BLE_LL_ENC_MODE_NOT_ACCEPT  = 0xB5,
    ///Link Key Cannot be changed
    AT_BLE_LL_LINK_KEY_CANT_CHANGE  = 0xB6,
    ///Quality of Service not supported
    AT_BLE_LL_QOS_NOT_SUPPORTED  = 0xB7,
    ///Error, instant passed
    AT_BLE_LL_INSTANT_PASSED  = 0xB8 ,
    ///Pairing with unit key not supported
    AT_BLE_LL_PAIRING_WITH_UNIT_KEY_NOT_SUP  = 0xB9,
    ///Transaction collision
    AT_BLE_LL_DIFF_TRANSACTION_COLLISION  = 0xBA,
    ///Unacceptable parameters
    AT_BLE_LL_QOS_UNACCEPTABLE_PARAM  = 0xBC,
    ///Quality of Service rejected
    AT_BLE_LL_QOS_REJECTED  = 0xBD,
    ///Channel class not supported
    AT_BLE_LL_CHANNEL_CLASS_NOT_SUP  = 0xBE,
    ///Insufficient security
    AT_BLE_LL_INSUFFICIENT_SECURITY  = 0xBF,
    ///Parameters out of mandatory range
    AT_BLE_LL_PARAM_OUT_OF_MAND_RANGE  = 0xC0,
    ///Role switch pending
    AT_BLE_LL_ROLE_SWITCH_PEND  = 0xC2,
    ///Reserved slot violation
    AT_BLE_LL_RESERVED_SLOT_VIOLATION  = 0xC4,
    ///Role Switch fail
    AT_BLE_LL_ROLE_SWITCH_FAIL  = 0xC5,
    ///Error, EIR too large
    AT_BLE_LL_EIR_TOO_LARGE  = 0xC6,
    ///Simple pairing not supported by host
    AT_BLE_LL_SP_NOT_SUPPORTED_HOST  = 0xC7,
    ///Host pairing is busy
    AT_BLE_LL_HOST_BUSY_PAIRING  = 0xC8,
    ///Controller is busy
    AT_BLE_LL_CONTROLLER_BUSY  = 0xCA,
    ///Unacceptable connection initialization
    AT_BLE_LL_UNACCEPTABLE_CONN_INT  = 0xCB,
    ///Direct Advertising Timeout
    AT_BLE_LL_DIRECT_ADV_TO  = 0xCC,
    ///Connection Terminated due to a MIC failure
    AT_BLE_LL_TERMINATED_MIC_FAILURE  = 0xCD,
    ///Connection failed to be established
    AT_BLE_LL_CONN_FAILED_TO_BE_EST = 0xCE,
    ///Invalid input argument to function
    AT_BLE_INVALID_PARAM = 0xCF,
    ///Timeout has been occurred while waiting for a specific event
    AT_BLE_TIMEOUT = 0xD0,
    ///Invalid bus initialization
    AT_BLE_BUS_TIMEOUT = 0xD1,
    ///This operation can't be handled now, wait for its event first
    AT_BLE_BUSY = 0xD2,
    ///Data base memory hasn't been assigned, it is a must to call @ref at_ble_init with proper parameter @ref at_ble_init_config_t
    AT_BLE_DB_MEMORY_NOT_SETTED = 0xD3,

    /// Operation disallowed in this state
    AT_BLE_INVALID_STATE = 0xE0,
    /// Error On bus or undefined state
    AT_BLE_BUS_FAILURE = 0XE1,
    ///Generic Failure
    AT_BLE_FAILURE  = 0xE2,
    ///Client characteristic configuration improperly configured
    AT_BLE_PRF_CCCD_IMPR_CONFIGURED  = 0xFD,
    ///Procedure already in progress
    AT_BLE_PRF_PROC_IN_PROGRESS  = 0xFE,
    ///Out of Range
    AT_BLE_PRF_OUT_OF_RANGE  = 0xFF,
} at_ble_err_status_t;


#endif // _AT_BLE_ERRNO_H
