/**
 * \file
 *
 * \brief Custom Serial Chat Application declarations
 *
 * Copyright (c) 2017-2018 Microchip Technology Inc. and its subsidiaries.
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
/**
 * \mainpage CUSTOM SERIAL CHAT Example
 * \section Introduction
 * **************************** Introduction *********************************
 * + The custom serial chat allow you to send and receive data between the Atmel ATBTLC1000 with a supported host platform and the 
 *   Atmel Smart Connect mobile application. 
 * + This is a custom profile and application example implemented over GATT. 
 * + The user can type a message on the Atmel ATBTLC1000 side using the terminal console and send it to the mobile application. 
 * + Messages typed from the mobile app side are received and displayed on the console terminal at the device side.
 *- Supported Evolution Kit -
 *	+ ATSAML21-XPRO-B + ATBTLC1000 XPRO
 *	+ ATSAMD21-XPRO + ATBTLC1000 XPRO
 *	+ ATSAMG55-XPRO + ATBTLC1000 XPRO
 *	+ ATSAM4S-XPRO + ATBTLC1000 XPRO
 *- Running the Demo -
 * + 1. Build and flash the binary into supported evaluation board.
 * + 2. Open the console using TeraTerm or any serial port monitor.
 * + 3. Press the Reset button.
 * + 4. Wait for around 10 seconds for the patches to be downloaded device will initialize and start-up.
 * + 5. The device is now in advertising mode.
 * + 6. On a BLE compatible iPhone®/Android phone, enable Bluetooth in the Settings page. Start the Atmel Smart Connect App and scan for devices. 
 *      ATMEL-CSC will be appear among the devices scanned. Click on **ATMEL-CSC** to connect to supported platform.
 * + 7. Once connected, the client side will request for the pairing procedure . The console log provides a guidance for the user to 
 *      enter the pass-key.
 * + 8. After clicking on the Custom Serial Chat icon, the chat screen will appear where the user can type the text that is to be sent to 
 *       the remote device and also see the text coming from the remote device. 
 * + 9. Chat text “Hello Atmel” send to remote device.
 * + 10. The user can also write the text on the console for supported platform + ATBTLC1000 device and press the ENTER key for 
 *       transmitting the chat text to the mobile application.
 * \section Modules
 * ***************************** Modules **************************************
 *- BLE Manger -  
 *  + The Event Manager is responsible for handling the following:
 *    + Generic BLE Event Handling:-
 *       + BLE Event Manager handles the events triggered by BLE stack and also responsible 
 *  	 for invoking all registered callbacks for respective events. BLE Manager 
 *  	 handles all GAP related functionality. In addition to that handles multiple connection 
 *  	 instances, Pairing, Encryption, Scanning.
 *    + Handling Multi-role/multi-connection:-
 *  	  + BLE Event Manager is responsible for handling multiple connection instances 
 *  	  and stores bonding information and Keys to retain the bonded device. 
 *  	  BLE Manager is able to identify and remove the device information when pairing/encryption 
 *		  gets failed. In case of multi-role, it handles the state/event handling of both central and peripheral in multiple contexts.
 *    + Controlling the Advertisement data:-
 *  	  + BLE Event Manager is responsible for generating the advertisement and scan response data
 *  	  for BLE profiles/services that are attached with BLE Manager.
 *
 *- BLE Platform Services -
 *  +  Interface Settings -
 *	  + Connect ATBTLC1000 XPRO to SAML21-XPRO-B -> EXT1
 *	  + Connect ATBTLC1000 XPRO to SAMD21-XPRO -> EXT1
 *	  + Connect ATBTLC1000 XPRO to SAMG55-XPRO -> EXT1
 *	  + Connect ATBTLC1000 XPRO to SAM4S-XPRO  -> EXT1
 *  +  Serial Console COM port settings -
 *    + Baudrate 115200
 *	  + Parity None, Stop Bit 1, Start Bit 1
 *	  + No Hardware Handshake
 *	+  6-Wire Mode Connection Setup -
 *    + Pins are 1:1 match with SAML21/D21 Xpro EXT1 Header to BTLC1000 XPro Header
 *	  + UART(No Flow Control)-SAM L21/D21 XPro Pins (Rx-Pin13, Tx-Pin14)
 *	  + UART(With Flow Control)-SAM G55 Xpro Pins (Rx-Pin13, Tx-Pin14, RTS-Pin5, CTS-Pin6, Rx-Pin16, Tx-Pin17)
 *	  + BTLC1000 Wakeup Pin-SAM G55 XPro Pins(Pin4)
 *	  + BTLC1000 Chip Enable Pin-SAM G55 XPro Pins(Pin10)
 *	  + BTLC1000 Vcc Pin-SAM L21/D21/G55 Xpro Pins(Pin20)
 *	  + BTLC1000 GND Pin-SAM L21/D21/G55 Xpro Pins(Pin19)
 *  +  4-Wire Mode Connection setup -
 * 	  + UART(With Flow Control)-SAM L21/D21 XPro Pins (Rx-Pin15, Tx-Pin17, RTS-Pin16, CTS-Pin18)
 * 	  + BTLC1000 Wakeup Pin-SAM L21/D21 XPro Pins (Rx-Pin6)
 * 	  + BTLC1000 Chip Enable Pin-SAM L21/D21 XPro Pins (Rx-Pin4)
 * 	  + UART(With Flow Control)-SAM G55/4S Xpro Pins (Rx-Pin13, Tx-Pin14, RTS-Pin5, CTS-Pin6)
 * 	  + BTLC1000 Wakeup Pin-SAM G55/4S XPro Pins(Pin4)
 * 	  + BTLC1000 Chip Enable Pin-SAM G55/4S XPro Pins(Pin10)
 * 	  + BTLC1000 Vcc Pin-SAM L21/D21/G55/4S Xpro Pins(Pin20)
 * 	  + BTLC1000 GND Pin-SAM L21/D21/G55/4S Xpro Pins(Pin19)
 *
 *\section BLE SDK Package
 * ***************************** BLE SDK Package ******************************************
 *- Links for BluSDK -
 *		+ http://www.microchip.com/wwwproducts/en/ATBTLC1000?tab=documents
 *- Links for ATBTLC1000 -
 *		+ http://www.microchip.com/wwwproducts/en/ATBTLC1000
 *- Development Kit -
 *		+ http://www.microchip.com/wwwproducts/en/ATBTLC1000?tab=tools
 *- SAM L21 + BTLC1000 XPro -
 *		+ http://www.microchip.com/developmenttools/productdetails/atbtlc1000-xstk
 *- BTLC1000 XPro -
 *		+ http://www.microchip.com/developmenttools/productdetails/atbtlc1000-xpro
 *- Applications -
 *		+ http://www.microchip.com/wwwproducts/en/ATBTLC1000?tab=applications
 *- Support and FAQ - visit 
 *		+ <a href="https://www.microchip.com/support/">Microchip Support</a>
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
#include "conf_extint.h"
#include "sio2host.h"  

/* =========================== GLOBALS ============================================================ */

/* Received notification data structure */
csc_report_ntf_t recv_ntf_info;

/* Data length to be send over the air */
uint16_t send_length = 0;

/* Buffer data to be send over the air */
uint8_t send_data[APP_TX_BUF_SIZE];

static const ble_gap_event_cb_t app_gap_handle = {
	.connected = app_connected_event_handler,
	.disconnected = app_disconnected_event_handler
};


/**
* @brief app_connected_state blemanager notifies the application about state
* @param[in] at_ble_connected_t
*/
static at_ble_status_t app_connected_event_handler(void *params)
{
	ALL_UNUSED(params);
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
		ALL_UNUSED(params);
		return AT_BLE_SUCCESS;
}

/* Function used for receive data */
static void csc_app_recv_buf(uint8_t *recv_data, uint8_t recv_len)
{
	uint16_t ind = 0;
	if (recv_len){
		for (ind = 0; ind < recv_len; ind++){
			sio2host_putchar(recv_data[ind]);
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
	uint16_t ind = 0;
	uint16_t len = 0;
	uint8_t buff = 0;
	uint8_t counter = 0;
	
	len = sio2host_rx(&buff, 1);
	if (len){
		for (ind = 0; ind < len; ind++){
			if(buff != ENTER_BUTTON_PRESS){
				sio2host_putchar(buff);
				if (buff == BACKSPACE_BUTTON_PRESS)
				{
					sio2host_putchar(SPACE_BAR);
					sio2host_putchar(buff);
					if(send_length)
						send_length--;
				}
				  
				if(send_length < APP_TX_BUF_SIZE){
					if(buff != BACKSPACE_BUTTON_PRESS)
						send_data[send_length++] = buff;
				}else{
					for (counter = 0; counter < send_length; counter++)
					{
						if(send_data[counter] != SPACE_BAR && send_data[counter] != TAB_PRESSED){
							csc_prf_send_data(&send_data[0], send_length);
							break;
						}
					}
					send_length = 0;
				}
			}else{ // User press enter to send data
				if(send_length){
					ind = send_length;
					send_length = 0;
					for (counter = 0; counter < ind; counter++)
					{
						if(send_data[counter] != SPACE_BAR && send_data[counter] != TAB_PRESSED)
						{
						csc_prf_send_data(&send_data[0], ind);
						break;
						}
					}
					DBG_LOG("\r\n");
				}
			}
		}
	}
}

bool app_exec = true;
int main(void )
{
#if SAMG55 || SAM4S
	/* Initialize the SAM system. */
	sysclk_init();
	board_init();
#elif SAM0
	system_init();
#endif
	
	/* Initialize serial console */
	sio2host_init();
	
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
	
	/* Register the notification handler */
	notify_recv_ntf_handler(csc_prf_report_ntf_cb);
	
	/* Capturing the events  */
	while(app_exec){
		ble_event_task();
		csc_app_send_buf();	
	}
	return 0;
}



