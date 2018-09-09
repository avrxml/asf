/**
* \file
*
* \brief ANCS Service Declarations
*
* Copyright (c) 2016-2018 Microchip Technology Inc. and its subsidiaries.
*
* \asf_license_start
*
* \page License
*
* Subject to your compliance with these terms, you may use Microchip
* software and any derivatives exclusively with Microchip products.
* It is your responsibility to comply with third party license terms applicable
* to your use of third party software (including open source software) that
* may accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
* WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
* INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
* AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
* LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
* LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
* SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
* POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
* ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
* RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*
* \asf_license_stop
*
*/
/*
* Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
*/
#ifndef __ALERT_NOTIFICATION_H__
#define __ALERT_NOTIFICATION_H__

// <<< Use Configuration Wizard in Context Menu >>>
// <h> Alert Notification Service Configuration
// =======================

#define ANP_INVALID_CHAR_HANDLE				(0)

//  <o> Characteristics read Lenght <0-100>
//  <i> Defines number of bytes to characteristics read length .
//  <i> Default: 20
//  <id> anp_read_length
#define ANP_READ_LENGTH                     (20)

/* Link Loss read offset length*/
//  <o> Characteristics read Offset <0-100>
//  <i> Defines offset value to read characteristics.
//  <i> Default: 20
//  <id> anp_read_offset
#define ANP_READ_OFFSET                     (0)

#define SUPPORTED_NEW_ALERT_READ			(1)
#define SUPPORTED_UNREAD_ALERT_READ			(2)
#define ALERT_CONTRL_POINT_READ				(3)

#define ANS_SUP_NEW_READ_LENGTH				(20)
#define ANS_NEW_ALERT_READ_LENGTH			(20)
#define ANS_SUP_UNREA_READ_LENGTH			(20)
#define ANS_UNREA_ALERT_READ_LENGTH			(20)
#define ANS_ALERT_NP_READ_LENGTH			(20)

#define BIT_MASK0							(1)
#define BIT_MASK1							(1 << 1)
#define BIT_MASK2							(1 << 2)
#define BIT_MASK3							(1 << 3)
#define BIT_MASK4							(1 << 4)
#define BIT_MASK5							(1 << 5)
#define BIT_MASK6							(1 << 6)
#define BIT_MASK7							(1 << 7)


typedef struct gatt_anp_handler
{
	at_ble_handle_t start_handle;
	at_ble_handle_t end_handle;
	at_ble_status_t char_discovery;
	at_ble_status_t desc_discovery;
	/*Characteristics for supported new alert category*/
	at_ble_handle_t supp_new_char_handle;
	uint8_t *supp_new_char_data;
	/*Characteristics for new alert */
	at_ble_handle_t new_alert_char_handle;
	uint8_t *new_alert_char_data;
	at_ble_handle_t new_alert_desc_handle;
	/*Characteristics for supported unread alert category*/
	at_ble_handle_t supp_unread_char_handle;
	uint8_t *supp_unread_char_data;
	/*Characteristics for unread alert status */
	at_ble_handle_t unread_alert_char_handle;
	uint8_t *unread_alert_char_data;
	at_ble_handle_t unread_alert_desc_handle;
	/*Characteristics for alert notification control point */
	at_ble_handle_t alert_np_char_handle;
	uint8_t *alert_np_data;
	
}gatt_anp_handler_t;


/**
 * @brief invoked by the profile for enabling or disabling notifications
 * @param[in] at_ble_handle_t connection handle
 * @param[in] at_ble_handle_t descriptor handle
 * @param[in] true for enabling notification false for disabling notifications
 * \note Called by the profile
 */
at_ble_status_t anp_alert_noti(at_ble_handle_t conn_handle,at_ble_handle_t desc_handle, bool noti);

/**
 * @brief anp_alert_read_response invoked by the profile once it receives read response
 * @param[in] at_ble_characteristic_read_response_t consists of characteristic handle value handle
 * @param[in] gatt_anp_handler_t consists all anp service related handles
 */
uint8_t anp_alert_read_response (at_ble_characteristic_read_response_t *read_resp, gatt_anp_handler_t *anp_handler);

/**
 * @brief anp_alert_read invoked by ble manager
 * @param[in] at_ble_handle_t consists of connection handle
 * @param[in] at_ble_handle_t consists of char handle
 */
at_ble_status_t anp_alert_read(at_ble_handle_t conn_handle, at_ble_handle_t char_handle);

/**
 * @brief anp_alert_notify_response invoked by the profile after receiving notifications
 * @param[in] at_ble_notification_recieved_t consists of characteristic handle and new value
 * @param[in] gatt_anp_handler_t anp service information in peer device
 */
void anp_alert_notify_response (at_ble_notification_recieved_t *notify_resp, gatt_anp_handler_t *anp_handler);

#endif /* __ALERT_NOTIFICATION_H__ */
// </h>

// <<< end of configuration section >>>
