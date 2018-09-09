/**
 * \file
 *
 * \brief Custom Serial Chat Application declarations
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
 /**
  * \mainpage CUSTOM SERIAL CHAT Example
  * \section Introduction
  * **************************** Introduction *********************************
  * + The custom serial chat allow you to send and receive data between the Atmel SAMB11 with a supported host platform and the 
  *   Atmel Smart Connect mobile application. 
  * + This is a custom profile and application example implemented over GATT. 
  * + The user can type a message on the Atmel ATBTLC1000 side using the terminal console and send it to the mobile application. 
  * + Messages typed from the mobile app side are received and displayed on the console terminal at the device side.
  *- Running the Demo -
  * + 1. Build and flash the binary into supported evaluation board.
  * + 2. Open the console using TeraTerm or any serial port monitor.
  * + 3. Press the Reset button.
  * + 4. Wait for around 10 seconds for the patches to be downloaded device will initialize and start-up.
  * + 5. The device is now in advertising mode.
  * + 6. On a BLE compatible iPhone®/Android phone, enable Bluetooth in the Settings page. Start the Atmel Smart Connect App and scan for devices. 
  * 	 ATMEL-CSC will be appear among the devices scanned. Click on **ATMEL-CSC** to connect to supported platform.
  * + 7. Once connected, the client side will request for the pairing procedure . The console log provides a guidance for the user to 
  * 	 enter the pass-key.
  * + 8. After clicking on the Custom Serial Chat icon, the chat screen will appear where the user can type the text that is to be sent to 
  * 	  the remote device and also see the text coming from the remote device. 
  * + 9. Chat text “Hello Atmel” send to remote device.
  * + 10. The user can also write the text on the console for supported platform + SAMB11 device and press the ENTER key for 
  * 	  transmitting the chat text to the mobile application.
  * \section Modules
  * ***************************** Modules **************************************
  *- BLE Manger -  
  *  + The Event Manager is responsible for handling the following:
  *    + Generic BLE Event Handling:-
  * 	  + BLE Event Manager handles the events triggered by BLE stack and also responsible 
  * 	  for invoking all registered callbacks for respective events. BLE Manager 
  * 	  handles all GAP related functionality. In addition to that handles multiple connection 
  * 	  instances, Pairing, Encryption, Scanning.
  *    + Handling Multi-role/multi-connection:-
  * 	   + BLE Event Manager is responsible for handling multiple connection instances 
  * 	   and stores bonding information and Keys to retain the bonded device. 
  * 	   BLE Manager is able to identify and remove the device information when pairing/encryption 
  * 	   gets failed. In case of multi-role, it handles the state/event handling of both central and peripheral in multiple contexts.
  *    + Controlling the Advertisement data:-
  * 	   + BLE Event Manager is responsible for generating the advertisement and scan response data
  * 	   for BLE profiles/services that are attached with BLE Manager.
  *
  *- BLE Platform Services -
  *  +	Serial Console COM port settings -
  *    + Baudrate 115200
  *    + Parity None, Stop Bit 1, Start Bit 1
  *    + No Hardware Handshake
  *
  *\section BLE SDK Package
  * ***************************** BLE SDK Package ******************************************
  * - Links for Docs -
  *		+ http://www.microchip.com/wwwproducts/en/ATSAMB11
  *		+ http://www.microchip.com/developmenttools/productdetails.aspx?partno=atsamb11-xpro
  *- Support and FAQ - visit -
 *		+ <a href="https://www.microchip.com/support/">Microchip Support</a>
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

/* Received notification data structure */
csc_report_ntf_t recv_ntf_info;

/* Data length to be send over the air */
uint16_t send_length = 0;

/* Buffer data to be send over the air */
uint8_t send_data[APP_TX_BUF_SIZE];
volatile bool notification_sent = true;

static const ble_gap_event_cb_t app_gap_handle = {
	.connected = app_connected_event_handler,
	.disconnected = app_disconnected_event_handler
};

static const ble_event_callback_t app_gatt_server_handle[] = {
	app_notification_cfm_handler,
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
	ALL_UNUSED(params);
	notification_sent = true;
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

/** @brief CSC_Notification confirmation handler called by ble manager
*	to give the status of notification sent
*  @param[in] at_ble_cmd_complete_event_t address of the cmd completion
*/
at_ble_status_t app_notification_cfm_handler(void *params)
{
	at_ble_cmd_complete_event_t event_params;
	memcpy(&event_params,params,sizeof(at_ble_cmd_complete_event_t));
	if (event_params.status == AT_BLE_SUCCESS) {
		DBG_LOG_DEV("App Notification Successfully sent over the air");
		notification_sent = true;
		} else {
		DBG_LOG_DEV("Sending Notification over the air failed");
		notification_sent = false;
	}
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
static at_ble_status_t platform_interrupt_app_event(void *param)
{
	at_ble_status_t status = AT_BLE_SUCCESS;
	platform_isr_event_t *plf_isr_event = (platform_isr_event_t *)param;
	if(plf_isr_event->event_type == ((UART_RX_INTERRUPT_MASK_RX_FIFO_NOT_EMPTY_MASK << 8) | UART_RX_COMPLETE)) {
		if (notification_sent) {
			csc_prf_send_data(plf_isr_event->event_data, plf_isr_event->event_data_len);
				notification_sent =  false;
			}
		else
			DBG_LOG("Previous notification not sent");	
	}
	else
		status = AT_BLE_FAILURE;
	return status;
}

/* Custom events like user defined event, platform event callback handlers for CSC profile */
static const ble_custom_event_cb_t CSC_custom_event_handle = {
	.platform_event_ready = platform_interrupt_app_event /* This event not handled in BLE Manager */
};

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
	&app_gap_handle);

	ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
	BLE_GATT_SERVER_EVENT_TYPE,
	&app_gatt_server_handle);
	
	/* Register the notification handler */
	notify_recv_ntf_handler(csc_prf_report_ntf_cb);
	
	/* Register the user event handler */
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
	BLE_CUSTOM_EVENT_TYPE,
	&CSC_custom_event_handle);
	
	register_uart_callback(uart_rx_callback);
	
	/* Capturing the events  */
	while(app_exec){
		ble_event_task(BLE_EVENT_TIMEOUT);
	}
	return 0;
}



