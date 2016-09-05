/**
 * \file
 *
 * \brief Time Information Profile Application
 *
 * Copyright (c) 2014-2016 Atmel Corporation. All rights reserved.
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

 /**
 * \mainpage
 * \section preface Preface
 * This is the reference manual for the Time Information Profile Application
 */
/***********************************************************************************
 *									Includes		                               *
 **********************************************************************************/
#include <asf.h>
#include "platform.h"
#include "at_ble_api.h"
#include "tip_app.h"
#include "time_info.h"
#include "console_serial.h"
#include "timer_hw.h"
#include "timer.h"
#include "ble_manager.h"
#include "ble_utils.h"
#include "current_time.h"
#include "next_dst.h"
#include "reference_time.h"
#include "led.h"
#include "button.h"

#define APP_STACK_SIZE  (1024)

volatile unsigned char app_stack_patch[APP_STACK_SIZE];

/***********************************************************************************
 *									Types			                               *
 **********************************************************************************/
extern gatt_cts_handler_t cts_handle;
extern gatt_dst_handler_t dst_handle;
extern gatt_rtu_handler_t rtu_handle;
extern ble_connected_dev_info_t ble_dev_info[BLE_MAX_DEVICE_CONNECTED];
volatile bool button_pressed = false;
extern volatile bool current_time_char_found;
extern volatile bool local_time_char_found;
extern volatile bool ref_time_char_found;
extern volatile bool time_with_dst_char_found;
extern volatile bool time_update_cp_char_found;
extern volatile bool time_update_state_char_found;
static bool completed_prev_read = true;
/***********************************************************************************
 *									Implementations                               *
 **********************************************************************************/
/**
 * @brief Button Press Callback 
 */
static void button_cb(void)
{
	button_pressed = true;

	send_plf_int_msg_ind(USER_TIMER_CALLBACK, TIMER_EXPIRED_CALLBACK_TYPE_DETECT, NULL, 0);
}

/**
 * @brief Timer Callback
 */
void timer_callback_handler(void)
{
	/* Free to use for User Application */
}

/**
 * @brief Callback registered for characteristic read response 
 * @param[in] char_read_resp @ref at_ble_characteristic_read_response_t
 * @return None
 *
 */
static at_ble_status_t app_read_response_cb(void *param)
{
	at_ble_characteristic_read_response_t *char_read_resp;
	char_read_resp = (at_ble_characteristic_read_response_t *)param;
	if (char_read_resp->char_handle == cts_handle.curr_char_handle) {
		if (local_time_char_found) {
			if (tis_current_time_read( char_read_resp->conn_handle,
			cts_handle.lti_char_handle )
			== AT_BLE_SUCCESS) {
				DBG_LOG_DEV("Local Time info request success");
			}
		}else {
			completed_prev_read = true;
		}
	} else if (char_read_resp->char_handle == cts_handle.lti_char_handle) {
		if (ref_time_char_found) {
			if (tis_current_time_read( char_read_resp->conn_handle,
			cts_handle.rti_char_handle )
			== AT_BLE_SUCCESS) {
				DBG_LOG_DEV("Reference Time info request success");
			}
		}else {
			completed_prev_read = true;
		}
	} else if (char_read_resp->char_handle == cts_handle.rti_char_handle) {
		if (time_with_dst_char_found) {
			if (tis_dst_change_read( char_read_resp->conn_handle,
			dst_handle.dst_char_handle )
			== AT_BLE_SUCCESS) {
				DBG_LOG_DEV("Time with DST read request success");
			}
		}else {
			completed_prev_read = true;
		}
	} else if (char_read_resp->char_handle == dst_handle.dst_char_handle) {
		if (time_update_state_char_found) {
			if (tis_rtu_update_read( char_read_resp->conn_handle,
			rtu_handle.tp_state_char_handle, 20 )
			== AT_BLE_SUCCESS) {
				DBG_LOG_DEV("Time update state request success");
			}
		}
		completed_prev_read = true;
	}
	return AT_BLE_SUCCESS;
}

static const ble_event_callback_t tip_app_gatt_client_handle[] = {
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	app_read_response_cb,
	NULL,
	NULL,
	NULL,
	NULL
};

/**
 * @brief Main Function for Time Information Callback
 */
int main (void)
{

	platform_driver_init();
	acquire_sleep_lock();

	/* Initializing the console  */
	serial_console_init();
	
	/* Initialize the button */
	gpio_init();
	button_init();
	button_register_callback(button_cb);

 	/* Initialize the LED */
	led_init();
	
	/* Initializing the hardware timer */
	hw_timer_init();
	
	/*Registration of timer callback*/
	hw_timer_register_callback(timer_callback_handler);
	
	DBG_LOG("Time Profile Application");
	
	/* initialize the BLE chip  and Set the device mac address */
	ble_device_init(NULL);
	
	time_info_init();
	
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
									BLE_GATT_CLIENT_EVENT_TYPE,
									tip_app_gatt_client_handle);
	
	while(1) {
		ble_event_task(BLE_EVENT_TIMEOUT);
		if (button_pressed && completed_prev_read) {
			//delay_ms(200);
				/* code for pts */
			#ifdef ENABLE_PTS
			DBG_LOG("Press 1 for Service discovery");
			DBG_LOG("Press 2 for Writing to time update control point");
			DBG_LOG("Press 3 for Enable/Disable notification");
			DBG_LOG("Press 4 for Read current time characteristics value");
			DBG_LOG("And press enter");
			uint8_t option = 0;
			option = getchar_b11();
			DBG_LOG("Received %d",option);
			switch (option) {
			case 1 :
				time_info_service_discovery();
				break;
			case 2 :
				DBG_LOG("Enter time update control point value [1 - 2] and press enter");
				option = getchar_b11();
				if (!(tis_rtu_update_write(ble_dev_info[0].conn_info.handle, 
											rtu_handle.tp_control_char_handle,
											(uint8_t)option) == AT_BLE_SUCCESS)) {
					DBG_LOG("Fail to write Time Update control point");
				}
				break;
			case 3 :
				DBG_LOG("Enter 0 to disable both notification & indication");
				DBG_LOG("Enter 1 to enable notification");
				DBG_LOG("Enter 2 to enable indication");
				DBG_LOG("Enter 3 to enable both notification and indication");
				DBG_LOG("And press Enter");
				option = getchar_b11();
				time_info_write_notification_handler((uint16_t)option);
				break;
			case 4 :
				if (current_time_char_found) {
					if (tis_current_time_read( ble_dev_info[0].conn_info.handle, 
											cts_handle.curr_char_handle) 
											== AT_BLE_SUCCESS) {
						LED_Toggle(LED0);
						DBG_LOG_DEV("CurrentTime info request success");
						completed_prev_read = false;
					}
				}
				break;
			}
			#else /* code for pts */		
			if (current_time_char_found) {
				if (tis_current_time_read( ble_dev_info[0].conn_info.handle, 
										cts_handle.curr_char_handle) 
										== AT_BLE_SUCCESS) {
					LED_Toggle(LED0);
					DBG_LOG_DEV("CurrentTime info request success");
					completed_prev_read = false;
				}
			}
			#endif
			button_pressed = false;
		}
	}
}
	
