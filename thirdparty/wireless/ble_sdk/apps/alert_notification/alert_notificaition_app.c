/**
* \file
*
* \brief Alert Notification Application
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
* Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
*/

/****************************************************************************************
*							        Includes	                                     	*
****************************************************************************************/
#include <asf.h>
#include "console_serial.h"
#include "at_ble_api.h"
#include "at_ble_errno.h"
#include "at_ble_trace.h"
#include "platform.h"
#include "console_serial.h"
#include "timer_hw.h"
#include "conf_extint.h"
#include "ble_manager.h"
#include "alert_notification.h"
#include "alert_notification_profile.h"

extern gatt_anp_handler_t anp_handle;

extern ble_connected_dev_info_t ble_dev_info[BLE_MAX_DEVICE_CONNECTED];

volatile bool user_request = false;

volatile bool notification_enable = false;

volatile bool app_state = false;

/***********************************************************************************
 *									Implementations                               *
 **********************************************************************************/
/**
 * @brief Button Press Callback 
 */
void button_cb(void)
{
	user_request = true;
}

/**
 * @brief Timer Callback
 */
static void timer_callback_handler(void)
{
	/* Free to use for User Application */
}

/**
 * @brief app_connected_state profile notifies the application about state
 * @param[in] connected
 */
static void app_connected_state(bool connected)
{
	app_state = connected;
	if (connected) {
		DBG_LOG_DEV("App connected");	
	} else {
		/* Starting advertisement on disconnection */
		anp_client_adv();
	}		
}

int main(void)
{
	#if SAMG55 || SAM4S
	/* Initialize the SAM system. */
	sysclk_init();
	board_init();
	#elif SAM0
	system_init();
	#endif
	
	/* Initialize serial console */
	serial_console_init();
	
	/* Initializing the button */
	button_init();
	
	/* Initializing the hardware timer */
	hw_timer_init();
	
	/*Registration of timer callback*/
	hw_timer_register_callback(timer_callback_handler);
	
	/* Register the connected call back with the profile */
	register_connected_callback(app_connected_state);
	
	DBG_LOG("Alert Notification Profile Application");
	
	/* initialize the ble chip  and Set the device mac address */
	ble_device_init(NULL);
	
	/* Initializing the anp profile */
	anp_client_init(NULL);
	
	/* Starting the advertisement */
	anp_client_adv();
	
	/* Capturing the events  */
	while(1) {

		/* BLE Event Task */
		ble_event_task();
		if (user_request) {
	
			/* Button debounce delay*/
			delay_ms(350);
			#ifdef ENABLE_PTS
			DBG_LOG("Press 1 for service discovery");
			DBG_LOG("Press 2 for write notification");
			DBG_LOG("Press 3 for disable notification");
			DBG_LOG("Press 4 for write to alert notification control point");
			DBG_LOG("And press Enter");
			uint8_t ncp_data[2] = {0};
			int option = 0;
			scanf("%d", &option);
			DBG_LOG("Received %d",option);
			switch (option) {
			case 1 :
				alert_service_discovery();
				break;
			case 2 :
				anp_client_write_notification_handler();
				break;
			case 3 :
				anp_client_disable_notification();
				break;
 			case 4 :
				DBG_LOG("Enter alert catagory[email/news/..] and press Enter");
				scanf("%d",&option);
				if (option > 255) {
					DBG_LOG("Entered a wrong value[0-255]");
					break;
				} else {
					ncp_data[1] = (uint8_t) option;
					DBG_LOG("Alert catogory is %d",ncp_data[1]);
				}
 				DBG_LOG("Enter the command ID[0 - 5] and press Enter");
				scanf("%d", &option);
				if (option > 255) {
					DBG_LOG("Entered a wrong value[0-255]");
					break;
				} else {
					ncp_data[0] = (uint8_t) option;
					DBG_LOG("Command ID is %d",ncp_data[0]);
				}
 				anp_write_to_ncp(ncp_data);
				break;
			}
			#else
			if (notification_enable) {
				anp_client_write_notification_handler();
				notification_enable = false;
			} else {
				anp_client_disable_notification();
				notification_enable = true;
			}
			#endif
			user_request = false;
		}
	}
}
