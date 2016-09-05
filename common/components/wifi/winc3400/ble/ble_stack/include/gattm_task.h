/**
 * \file
 *
 * \brief GATTM.
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

#ifndef GATTM_TASK_H_INCLUDED
#define GATTM_TASK_H_INCLUDED

#include "cmn_defs.h"

/// maximum number of handles returned
#define GATT_MAX_HDL_LIST                            0x08

/// Local database maximum size
#define GATT_DB_MAX_SIZE                            0x50

/// size for 128-bit UUID
#define GATT_UUID_16_OCTET_LEN                      0x10

uint8_t gattm_add_svc_req_handler(uint16_t *start_hdl, uint16_t total_size, uint8_t nb_att_uuid_16,
		uint8_t nb_att_uuid_32, uint8_t nb_att_uuid_128);
uint8_t gattm_add_attribute_req_handler (uint16_t start_hdl, uint16_t max_data_size, uint16_t perm,
		uint8_t uuid_len, uint8_t *uuid, uint16_t *handle);
uint8_t gattm_att_set_value_req_handler(uint16_t handle, uint16_t length, uint8_t *value);
uint8_t gattm_att_get_value_req_handler(uint16_t handle, uint16_t* length, uint8_t *value);

uint8_t gatt_notify_req_handler(uint16_t conhdl, uint16_t charhdl);
uint8_t gatt_indicate_req_handler(uint16_t conhdl, uint16_t charhdl);
uint8_t gatt_write_attribute_req_handler (uint16_t length, uint16_t attr_hdl, uint16_t perm,
		uint8_t *val);
uint8_t gatt_clear_attribute_req_handler (uint16_t attr_hdl);
uint8_t gatt_read_attribute_req_handler  (uint16_t attr_hdl, uint16_t *len, uint16_t *perm, uint8_t *uuid_len,
		uint8_t *uuid, uint8_t *value);
void gatt_resource_access_req_handler(uint16_t con_hdl);
void gatt_resource_access_resp_handler (uint16_t con_hdl);

void gatt_disc_svc_req_handler (uint8_t u8ReqType, uint16_t u16ConHdl, uint16_t u16StartHdl, uint16_t u16EndHdl, 
								uint16_t u16AttDesValSize, uint8_t *pu8AttDesVal);
void gatt_disc_char_req_handler(uint8_t u8ReqType,uint16_t u16ConHdl,uint16_t u16StartHdl,uint16_t u16EndHdl,
									 uint16_t u16AttDesValSize, uint8_t *pu8AttDesVal);
void gatt_read_char_req_handler(uint8_t u8ReqType, uint16_t u16Offset, uint16_t u16ConHdl, uint16_t u16StartHdl, 
								uint16_t u16EndHdl, uint16_t u16NbOfUUID, uint8_t u8UUIDSize, uint8_t u8RespSize, 
								uint16_t u16UUID);
uint8_t gatt_write_char_req_handler(uint16_t u16ConHdl, uint16_t u16CharHdl, uint16_t u16WrOffset, uint16_t u16ValLen, 
									 uint8_t u8ReqType, uint8_t u8AutoExec, uint8_t *pu8Val);
/// GATT Task messages
enum gattm_msg_id
{
    /* Database Management */
    /// Add service in database request
    GATTM_ADD_SVC_REQ = 0x2C00,
    /// Add service in database response
    GATTM_ADD_SVC_RSP,
    /// Add attribute in service database request
    GATTM_ADD_ATTRIBUTE_REQ,
    /// Add attribute in service database response
    GATTM_ADD_ATTRIBUTE_RSP,

    /* Service management */
    /// Get permission settings of service request
    GATTM_SVC_GET_PERMISSION_REQ,
    /// Get permission settings of service response
    GATTM_SVC_GET_PERMISSION_RSP,
    /// Set permission settings of service request
    GATTM_SVC_SET_PERMISSION_REQ,
    /// Set permission settings of service response
    GATTM_SVC_SET_PERMISSION_RSP,

    /* Attribute Manipulation */
    /// Get permission settings of attribute request
    GATTM_ATT_GET_PERMISSION_REQ,
    /// Get permission settings of attribute response
    GATTM_ATT_GET_PERMISSION_RSP,
    /// Set permission settings of attribute request
    GATTM_ATT_SET_PERMISSION_REQ,
    /// Set permission settings of attribute response
    GATTM_ATT_SET_PERMISSION_RSP,

    /// Get attribute value request
    GATTM_ATT_GET_VALUE_REQ,
    /// Get attribute value response
    GATTM_ATT_GET_VALUE_RSP,
    /// Set attribute value request
    GATTM_ATT_SET_VALUE_REQ,
    /// Set attribute value response
    GATTM_ATT_SET_VALUE_RSP,

    /* Debug messages */
    /// DEBUG ONLY: Destroy Attribute database request
    GATTM_DESTROY_DB_REQ,
    /// DEBUG ONLY: Destroy Attribute database response
    GATTM_DESTROY_DB_RSP,
    /// DEBUG ONLY: Retrieve list of services request
    GATTM_SVC_GET_LIST_REQ,
    /// DEBUG ONLY: Retrieve list of services response
    GATTM_SVC_GET_LIST_RSP,
    /// DEBUG ONLY: Retrieve information of attribute request
    GATTM_ATT_GET_INFO_REQ,
    /// DEBUG ONLY: Retrieve information of attribute response
    GATTM_ATT_GET_INFO_RSP
};


/// request type - application interface
enum
{
    /*              Attribute Client Flags              */
    /*      Operation flags for discovery operation     */
    /* ************************************************ */
    /// Discover all services
    GATT_DISC_ALL_SVC           = 0x00,
    /// Discover services by UUID
    GATT_DISC_BY_UUID_SVC,
    /// Discover included services
    GATT_DISC_INCLUDED_SVC,
    /// Discover all characteristics
    GATT_DISC_ALL_CHAR,
    /// Discover characteristic by UUID
    GATT_DISC_BY_UUID_CHAR,
    /// Discover characteristic descriptor
    GATT_DISC_DESC_CHAR,
    /* Operation flags for reading attributes           */
    /* ************************************************ */
    /// Read characteristic
    GATT_READ_CHAR,
    /// Read characteristic by UUID
    GATT_READ_BY_UUID_CHAR,
    /// Read long characteristic
    GATT_READ_LONG_CHAR,
    /// Read multiple long characteristic
    GATT_READ_MULT_LONG_CHAR,
    /// Read descriptor
    GATT_READ_DESC,
    /// Read long descriptor
    GATT_READ_LONG_DESC,
    /* Operation flags for writing/modifying attributes */
    /* ************************************************ */
    /// Write no response
    GATT_WRITE_NO_RESPONSE,
    /// Write signed
    GATT_WRITE_SIGNED,
    /// Write characteristic
    GATT_WRITE_CHAR,
    /// Write long characteristic
    GATT_WRITE_LONG_CHAR,
    /// Write reliable characteristic
    GATT_WRITE_RELIABLE_CHAR,
    /// Write descriptor
    GATT_WRITE_DESC,
    /// Write long descriptor
    GATT_WRITE_LONG_DESC,
    /* Cancel write flag                                */
    /* ************************************************ */
    /// Write cancel characteristic
    GATT_WRITE_CANCEL_CHAR,
    /*              Attribute Server Flags              */
    /*      Operation flags for attribute notification  */
    /* ************************************************ */
    /// Handle notification: signed or unsigned
    GATT_HDL_NTF,
    /*      Operation flag for attribute indication     */
    /* ************************************************ */
    /// Handle indication
    GATT_HDL_IND,
    /// No operation
    GATT_NO_OP_REQ,
    /// Last GATT flag
    GATT_LAST
};

#endif /* GATT_TASK_H_ */
