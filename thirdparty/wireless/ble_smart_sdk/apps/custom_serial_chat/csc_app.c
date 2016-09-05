/**
 * \file
 *
 * \brief Custom Serial Chat Application declarations
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
 * This is the reference manual for the Custom Serial Chat Application declarations
 */
/*- Includes -----------------------------------------------------------------------*/
#include <asf.h>
#include "platform.h"
#include "console_serial.h"
#include "at_ble_api.h"
#include "ble_manager.h"
#include "csc_app.h"
#include "cscp.h"
#include "cscs.h"

/* =========================== GLOBALS ============================================================ */

#define APP_STACK_SIZE  (1024)

volatile unsigned char app_stack_patch[APP_STACK_SIZE];

/* Received notification data structure */
csc_report_ntf_t recv_ntf_info;

/* Data length to be send over the air */
uint16_t send_length = 0;

/* Buffer data to be send over the air */
uint8_t send_data[APP_TX_BUF_SIZE];


static const ble_event_callback_t app_gap_handle[] = {
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	app_connected_event_handler,
	app_disconnected_event_handler,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};

static void uart_rx_callback(uint8_t input)
{
	if(input == '\r') {
		if(send_length) {
			send_plf_int_msg_ind(UART_RX_COMPLETE, UART_RX_INTERRUPT_MASK_RX_FIFO_NOT_EMPTY_MASK, send_data, send_length);
			memset(send_data, 0, APP_TX_BUF_SIZE);
			send_length = 0;
			DBG_LOG(" ");
		}
	}
	else {
		send_data[send_length++] = input;
		DBG_LOG_CONT("%c", input);
		
		if(send_length >= APP_TX_BUF_SIZE) {
			send_plf_int_msg_ind(UART_RX_COMPLETE, UART_RX_INTERRUPT_MASK_RX_FIFO_NOT_EMPTY_MASK, send_data, send_length);
			memset(send_data, 0, APP_TX_BUF_SIZE);
			send_length = 0;
		}
	}
}

/**
* @brief app_connected_state blemanager notifies the application about state
* @param[in] at_ble_connected_t
*/
static at_ble_status_t app_connected_event_handler(void *params)
{
	return AT_BLE_SUCCESS;
}

/**
 * @brief app_connected_state ble manager notifies the application about state
 * @param[in] connected
 */
static at_ble_status_t app_disconnected_event_handler(void *params)
{
		/* Started advertisement */
		csc_prf_dev_adv();		

		return AT_BLE_SUCCESS;
}

/* Function used for receive data */
static void csc_app_recv_buf(uint8_t *recv_data, uint8_t recv_len)
{
	uint16_t ind = 0;
	if (recv_len){
		for (ind = 0; ind < recv_len; ind++){
			DBG_LOG_CONT("%c", recv_data[ind]);
		}
		DBG_LOG("\r\n");
	}
}

/* Callback called for new data from remote device */
static void csc_prf_report_ntf_cb(csc_report_ntf_t *report_info)
{
	DBG_LOG("\r\n");
	csc_app_recv_buf(report_info->recv_buff, report_info->recv_buff_len);
}

/* Function used for send data */
static void csc_app_send_buf(void)
{
	uint16_t plf_event_type;
	uint16_t plf_event_data_len;
	uint8_t plf_event_data[APP_TX_BUF_SIZE] = {0, };

	platform_event_get(&plf_event_type, plf_event_data, &plf_event_data_len);
	
	if(plf_event_type == ((UART_RX_INTERRUPT_MASK_RX_FIFO_NOT_EMPTY_MASK << 8) | UART_RX_COMPLETE)) {
		csc_prf_send_data(plf_event_data, plf_event_data_len);
	}

}

bool app_exec = true;
int main(void )
{
	platform_driver_init();
	acquire_sleep_lock();

	/* Initialize serial console  */
	serial_console_init();

	DBG_LOG("Initializing Custom Serial Chat Application");
	
	/* Initialize the buffer address and buffer length based on user input */
	csc_prf_buf_init(&send_data[0], APP_TX_BUF_SIZE);
	
	/* initialize the ble chip  and Set the device mac address */
	ble_device_init(NULL);
	
	/* Initializing the profile */
	csc_prf_init(NULL);
	
	/* Started advertisement */
	csc_prf_dev_adv();
	
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
	BLE_GAP_EVENT_TYPE,
	app_gap_handle);
	
	/* Register the notification handler */
	notify_recv_ntf_handler(csc_prf_report_ntf_cb);
	
	/* Register the user event handler */
	register_ble_user_event_cb(csc_app_send_buf);
	
	register_uart_callback(uart_rx_callback);
	
	/* Capturing the events  */
	while(app_exec){
		ble_event_task(BLE_EVENT_TIMEOUT);
	}
	return 0;
}



