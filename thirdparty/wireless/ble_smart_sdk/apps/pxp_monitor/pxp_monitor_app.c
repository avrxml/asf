/**
* \file
*
* \brief Proximity Monitor Profile Application
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
* Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel
*Support</a>
*/

/**
* \mainpage
* \section preface Preface
* This is the reference manual for the Proximity Monitor Profile Application
*/
/*- Includes ---------------------------------------------------------------*/

#include <asf.h>
#include "platform.h"
#include "timer.h"
#include "led.h"
#include "button.h"

#include "pxp_monitor_app.h"

#if defined IMMEDIATE_ALERT_SERVICE
#include "immediate_alert.h"
#endif

#if defined TX_POWER_SERVICE
#include "tx_power.h"
#endif

#if defined LINK_LOSS_SERVICE
#include "link_loss.h"
#endif

#if defined PROXIMITY_REPORTER
#include "pxp_reporter.h"
#endif

#define APP_STACK_SIZE  (1024)

volatile unsigned char app_stack_patch[APP_STACK_SIZE];

extern gatt_txps_char_handler_t txps_handle;
extern gatt_lls_char_handler_t lls_handle;
extern gatt_ias_char_handler_t ias_handle;

extern ble_connected_dev_info_t ble_dev_info[BLE_MAX_DEVICE_CONNECTED];
extern uint8_t pxp_connect_request_flag;

volatile bool app_timer_done = false;
pxp_current_alert_t alert_level = PXP_NO_ALERT;

volatile bool button_pressed = false;

static void button_cb(void)
{
	button_pressed = true;
	send_plf_int_msg_ind(USER_TIMER_CALLBACK, TIMER_EXPIRED_CALLBACK_TYPE_DETECT, NULL, 0);
}

/**@brief Check for Link Loss and Path Loss alert
* check for Low Alert value if crossed write Low Alert value to Immediate Alert
*Service. High Alert value if crossed write High Alert value to IAS service
*
* @param[in] conn_handle Connection handle of a connected device
*/
#ifndef ENABLE_PTS
static void rssi_update(at_ble_handle_t conn_handle)
{
	int8_t rssi_power = 0;
	at_ble_status_t status;
	app_timer_done = false;

	/* Get the Received signal strength intensity of the connect
	*device/handle*/
	if ((status = at_ble_rx_power_get(conn_handle,&rssi_power)) != AT_BLE_SUCCESS)
	{
		DBG_LOG("at_ble_rx_power_get failed,reason %d",status);
	}

	DBG_LOG("Rx Power(RSSI):%04d dBm", rssi_power);

	/* if received rssi is above no alert zone and below high alert zone */
	if ((rssi_power <= PXP_LOW_ALERT_RANGE) &&
			(rssi_power >= PXP_HIGH_ALERT_RANGE)) {
		if (!(alert_level == PXP_MID_ALERT)) {
			ias_alert_level_write(conn_handle,
					ias_handle.char_handle,
					IAS_MID_ALERT);
			alert_level = PXP_MID_ALERT;
		}

		DBG_LOG_CONT("---Mild Alert!");
		LED_Toggle(LED0);
	}
	/* if received rssi is above mild alert zone */
	else if (rssi_power < PXP_HIGH_ALERT_RANGE) {
		if (!(alert_level == PXP_HIGH_ALERT)) {
			ias_alert_level_write(conn_handle,
					ias_handle.char_handle,
					IAS_HIGH_ALERT);
			alert_level = PXP_HIGH_ALERT;
		}
		DBG_LOG_CONT("---High Alert!!!");
		LED_On(LED0);
	}
	/* if received rssi is below mild alert zone */
	else {
		if (!(alert_level == PXP_NO_ALERT)) {
			ias_alert_level_write(conn_handle,
					ias_handle.char_handle,
					IAS_NO_ALERT);
			alert_level = PXP_NO_ALERT;
			LED_Off(LED0);
		}
		DBG_LOG_CONT("---No Alert");
	}
}
#endif

/**@brief Proximity Application initialization
* start the device scanning process
*/
static void pxp_app_init(void)
{
	at_ble_status_t scan_status;

	/* Initialize the scanning procedure */
	scan_status = gap_dev_scan();

	/* Check for scan status */
	if (scan_status == AT_BLE_INVALID_PARAM) {
		DBG_LOG("Scan parameters are invalid");
	} else if (scan_status == AT_BLE_FAILURE) {
		DBG_LOG("Scanning Failed Generic error");
	}
}

/* @brief timer call back for rssi update
* enable the flags to execute the application task
*
*/
static void timer_callback_handler(void)
{
	/* Stop the timer */
	hw_timer_stop();

	/* Enable the flag the serve the task */
	app_timer_done = true;

	send_plf_int_msg_ind(USER_TIMER_CALLBACK, TIMER_EXPIRED_CALLBACK_TYPE_DETECT, NULL, 0);
}

int main(void)
{	
	platform_driver_init();
	acquire_sleep_lock();

	/* Initialize serial console */
	serial_console_init();
	
	/* Initialize LED */
	led_init();
	
	/* Initialize button */
	gpio_init();
	button_init();
	button_register_callback(button_cb);

	/* Initialize the hardware timer */
	hw_timer_init();

	/* Register the callback */
	hw_timer_register_callback(timer_callback_handler);
	register_hw_timer_start_func_cb(hw_timer_start);
	register_hw_timer_stop_func_cb(hw_timer_stop);

	/* initialize the BLE chip  and Set the device mac address */
	ble_device_init(NULL);
	
	pxp_monitor_init(NULL);

	DBG_LOG("Initializing Proximity Monitor Application");

	/* Initialize the pxp service */
	pxp_app_init();

	while (1) {
		/* BLE Event Task */
		ble_event_task(BLE_EVENT_TIMEOUT);

		#ifdef ENABLE_PTS
			if (button_pressed) {
				DBG_LOG("Press 1 for service discovery");
				DBG_LOG("Press 2 for encryption start");
				DBG_LOG("Press 3 for send MID ALERT on Immediate Alert Service");
				DBG_LOG("Press 4 for send HIGH ALERT on Immediate Alert Service");
				DBG_LOG("Press 5 for send NO ALERT on Immediate Alert Service");
				DBG_LOG("Press 6 for send MILD ALERT on Link Loss Service");
				DBG_LOG("Press 7 for send HIGH ALERT on Link Loss Service");
				DBG_LOG("Press 8 for send NO ALERT on Link Loss Service");
				DBG_LOG("Press 9 for disconnection");
				uint8_t option = getchar_b11();
				switch (option) {
				case '1':
					pxp_monitor_service_discover(ble_dev_info[0].conn_info.handle);
					break;
				case '2':
					at_ble_encryption_start(ble_dev_info[0].conn_info.handle,
								&ble_dev_info[0].bond_info.peer_ltk,
								ble_dev_info[0].bond_info.auth);
					break;
				case '3':
					ias_alert_level_write(ble_dev_info[0].conn_info.handle,
								ias_handle.char_handle,
								IAS_MID_ALERT);
					break;
				case '4':
					ias_alert_level_write(ble_dev_info[0].conn_info.handle,
								ias_handle.char_handle,
								IAS_HIGH_ALERT);
					break;
				case '5':
					ias_alert_level_write(ble_dev_info[0].conn_info.handle,
								ias_handle.char_handle,
								IAS_NO_ALERT);
					break;
				case '6':
					lls_alert_level_write(ble_dev_info[0].conn_info.handle,
								lls_handle.char_handle,
								LLS_MILD_ALERT);
					break;
				case '7':
					lls_alert_level_write(ble_dev_info[0].conn_info.handle,
								lls_handle.char_handle,
								LLS_HIGH_ALERT);
					break;
				case '8':
					lls_alert_level_write(ble_dev_info[0].conn_info.handle,
								lls_handle.char_handle,
								LLS_NO_ALERT);
					break;
				case '9':
					at_ble_disconnect(ble_dev_info[0].conn_info.handle, AT_BLE_TERMINATED_BY_USER);
					break;
				}
				button_pressed = false;
			}
		#else		
			if (button_pressed)
			{
				at_ble_disconnect(ble_dev_info[0].conn_info.handle, AT_BLE_TERMINATED_BY_USER);
				button_pressed = false;
				app_timer_done = false;
			}
	
			/* Application Task */
			if (app_timer_done) {
				if (pxp_connect_request_flag == PXP_DEV_CONNECTING) {
					at_ble_disconnected_t pxp_connect_request_fail;
					pxp_connect_request_fail.reason
						= AT_BLE_TERMINATED_BY_USER;
					pxp_connect_request_fail.handle = ble_dev_info[0].conn_info.handle;
					pxp_connect_request_flag = PXP_DEV_UNCONNECTED;
					if (at_ble_connect_cancel() == AT_BLE_SUCCESS) {
						DBG_LOG("Connection Timeout");
						pxp_disconnect_event_handler(
								&pxp_connect_request_fail);
					} else {
						DBG_LOG(
								"Unable to connect with device. Reseting the device");
						ble_device_init(NULL);
						pxp_app_init();
					}
				} else if (pxp_connect_request_flag == PXP_DEV_SERVICE_FOUND) {
					rssi_update(ble_dev_info[0].conn_info.handle);
					hw_timer_start(PXP_RSSI_UPDATE_INTERVAL);
				}
	
				app_timer_done = false;
			}
		#endif
	}
}
