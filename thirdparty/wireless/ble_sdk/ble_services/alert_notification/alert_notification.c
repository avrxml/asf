/**
* \file
*
* \brief ALert Notification Service
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
* 4. This software may only be redistributed and used in connection with an
*    Atmel micro controller product.
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
* Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
*/

/***********************************************************************************
 *									Includes		                               *
 **********************************************************************************/
#include "at_ble_api.h"
#include "platform.h"
#include "alert_notification.h"
#include "ble_manager.h"

const char *bitmask0[] = {"Simple Alert","Email","News","Call","Missed Call","Sms/Mms",
							"Voice Mail","Schedule"};
const char *bitmask1[] = {"High Prioritized Alert","Instant Message",
							"Reserved For feature"};

/***********************************************************************************
 *									Implementation	                               *
 **********************************************************************************/

/**
 * @brief anp_alert_noti invoked by the profile for enabling or disabling notifications
 * @param[in] at_ble_handle_t connection handle
 * @param[in] at_ble_handle_t descriptor handle
 * @param[in] true for enabling notification false for disabling notifications
 * \note Called by the profile
 */
at_ble_status_t anp_alert_noti(at_ble_handle_t conn_handle,at_ble_handle_t desc_handle, bool noti)
{
	uint8_t desc_value[2] ;
	
	if(desc_handle == ANP_INVALID_CHAR_HANDLE) {
		return (AT_BLE_ATT_INVALID_HANDLE);
	}
	
	if(noti == true) {
		desc_value[0] = 1;
		desc_value[1] = 0;
		
		return(at_ble_characteristic_write(conn_handle, desc_handle, 0, 2, &desc_value[0],false, true));
	} else if(noti == false) {
		desc_value[0] = 0;
		desc_value[1] = 0;
		return(at_ble_characteristic_write(conn_handle, desc_handle, 0, 2, &desc_value[0],false, true));
	}
	
	return (AT_BLE_SUCCESS);
}

/**
 * @brief anp_alert_read invoked by ble manager
 * @param[in] at_ble_handle_t consists of connection handle
 * @param[in] at_ble_handle_t consists of char handle
 */
at_ble_status_t anp_alert_read(at_ble_handle_t conn_handle,at_ble_handle_t char_handle)
{
	if (char_handle != ANP_INVALID_CHAR_HANDLE) {
		return (at_ble_characteristic_read(conn_handle,char_handle,ANP_READ_OFFSET,ANP_READ_LENGTH));
	}
	
	return (AT_BLE_ATT_INVALID_HANDLE);
}

/**
 * @brief anp_alert_category to determine the supported alert categories
 * @param[in] data received from the peer device
 */
static void anp_alert_category(uint8_t *data)
{
	if (data[0] & BIT_MASK0) {
		DBG_LOG("%s",bitmask0[0]);
	}
	
	if (data[0] & BIT_MASK1) {
		DBG_LOG("%s",bitmask0[1]);
	}
	
	if (data[0] & BIT_MASK2) {
		DBG_LOG("%s",bitmask0[2]);
	}
	
	if (data[0] & BIT_MASK3) {
		DBG_LOG("%s",bitmask0[3]);
	}
	
	if (data[0] & BIT_MASK4) {
		DBG_LOG("%s",bitmask0[4]);
	}
	
	if (data[0] & BIT_MASK5) {
		DBG_LOG("%s",bitmask0[5]);
	}
	
	if (data[0] & BIT_MASK6) {
		DBG_LOG("%s",bitmask0[6]);
	}
	
	if (data[0] & BIT_MASK7) {
		DBG_LOG("%s",bitmask0[7]);
	}
	
	if (data[1] & BIT_MASK0) {
		DBG_LOG("%s",bitmask1[0]);
	}
	
	if (data[1] & BIT_MASK1) {
		DBG_LOG("%s",bitmask1[1]);
	}
}

/**
 * @brief anp_alert_read_response invoked by the profile once it receives read response
 * @param[in] at_ble_characteristic_read_response_t consists of characteristic handle value handle
 * @param[in] gatt_anp_handler_t consists all anp service related handles
 */
uint8_t anp_alert_read_response (at_ble_characteristic_read_response_t *read_resp, gatt_anp_handler_t *anp_handler)
{
	/* Supported New Alert Category */
	if(read_resp->char_handle == anp_handler->supp_new_char_handle) {
		DBG_LOG_DEV("Support New Alert Characteristic read response %02x handler",read_resp->char_handle);
		DBG_LOG_DEV("The length is %d",read_resp->char_len);
		
		DBG_LOG("Peer device supports the following New Alert categories");
		
		anp_alert_category(&read_resp->char_value[0]);
		
		DBG_LOG_DEV("Category ID bit Mask 0 = %02x",read_resp->char_value[0]);
		DBG_LOG_DEV("Category ID bit Mask 1 = %02x",read_resp->char_value[1]);
		
		return SUPPORTED_NEW_ALERT_READ;
	}

	/* Supported Unread Alert Category */
	if(read_resp->char_handle == anp_handler->supp_unread_char_handle) {
		
		DBG_LOG_DEV("Supported UnAlert Characteristic read response %02x handler",read_resp->char_handle);
		DBG_LOG_DEV("The length is %d",read_resp->char_len);
		DBG_LOG("Peer device supports the following Unread Alert categories");
			
		anp_alert_category(&read_resp->char_value[0]);
	
		DBG_LOG_DEV("Category ID bit Mask 0 = %02x",read_resp->char_value[0]);
		DBG_LOG_DEV("Category ID bit Mask 1 = %02x",read_resp->char_value[1]);
	
		return SUPPORTED_UNREAD_ALERT_READ;
	}
		
	DBG_LOG_DEV("Category ID bit Mask 0 = %02x",read_resp->char_value[0]);
	DBG_LOG_DEV("Category ID bit Mask 1 = %02x",read_resp->char_value[1]);

	/* Alert Notification Control Point */
	if(read_resp->char_handle == anp_handler->alert_np_char_handle)	{
		DBG_LOG_DEV("Alert Notification Characteristic read response "
					"%02x handler",read_resp->char_handle);
		DBG_LOG_DEV("Category ID bit Mask 0 = %02x",read_resp->char_value[0]);
		DBG_LOG_DEV("Category ID bit Mask 1 = %02x",read_resp->char_value[1]);
		
		return ALERT_CONTRL_POINT_READ;
	}
	return 0;
}

/**
 * @brief anp_alert_type to determine the type of alert received
 * @param[in] data received from the peer device consisting alert info
 */
static void anp_alert_type(uint8_t *data)
{
	if (data[0] <= 7) {
		DBG_LOG("%s alert",bitmask0[(data[0])]);
	}
	
	if (data[0] == 8) {
		DBG_LOG("%s alert",bitmask1[(data[0])]);
	}
	
	if (data[0] == 9) {
		DBG_LOG("%s alert",bitmask1[(data[0])]);
	}
}

/**
 * @brief anp_alert_notify_response invoked by the profile after receiving notifications
 * @param[in] at_ble_notification_recieved_t consists of characteristic handle and new value
 * @param[in] gatt_anp_handler_t anp service information in peer device
 */
void anp_alert_notify_response (at_ble_notification_recieved_t *notify_resp, gatt_anp_handler_t *anp_handler)
{
	DBG_LOG_DEV("The length of notification received is %d",notify_resp->char_len);
	
	if ((notify_resp->char_handle == anp_handler->new_alert_char_handle) &&
												notify_resp->char_value[1]) {
		DBG_LOG("New Alert received");
		DBG_LOG("The no of new alerts are %d",notify_resp->char_value[1]);
		DBG_LOG("The alert type is :");
		anp_alert_type(&notify_resp->char_value[0]);
	} else if ((notify_resp->char_handle == anp_handler->unread_alert_char_handle) &&
												notify_resp->char_value[1]) {
		DBG_LOG("Unread alert received");
		DBG_LOG("The no of unread alerts are %d",notify_resp->char_value[1]);
		DBG_LOG("The alert type is :");
		anp_alert_type(&notify_resp->char_value[0]);	
	}
}
