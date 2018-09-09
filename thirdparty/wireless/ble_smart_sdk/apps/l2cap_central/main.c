/**
 * \file
 *
 * \brief L2cap Central Application
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
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
 * \mainpage L2CAP CENTRAL Example
 * \section Introduction
 * **************************** Introduction *********************************

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
 
 *- BLE Platform Services -
 *  +  Serial Console COM port settings -
 *    + Baudrate 115200
 *	  + Parity None, Stop Bit 1, Start Bit 1
 *	  + No Hardware Handshake
 *
 *\section BLE SDK Package
 * ***************************** BLE SDK Package ******************************************
 * - Links for Docs -
 *		+ http://www.microchip.com/wwwproducts/en/ATSAMB11
 *		+ http://www.microchip.com/developmenttools/productdetails.aspx?partno=atsamb11-xpro
 *- Support and FAQ - visit -
 *		+ <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

/****************************************************************************************
*							        Includes											 *
****************************************************************************************/
#include <asf.h>
#include "platform.h"
#include "console_serial.h"
#include "timer_hw.h"
#include "ble_utils.h"
#include "at_ble_api.h"
#include "ble_manager.h"
#include "at_ble_trace.h"

#define AT_BLE_INQ_SCAN_INTV    (0x0140)
#define AT_BLE_INQ_SCAN_WIND    (0x0050)

at_ble_handle_t gstHandle = 0xFFFF;

#define APP_PSM			0x80
#define APP_CID			0x40
#define APP_CREDIT		0x7FFF
#define APP_DATA_LEN	507


#define CONN_INTERVAL_7_5_MS	0x0006
#define CONN_INTERVAL_10_0_MS	0x0008
#define CONN_INTERVAL_100_0_MS	0x0050
#define CONN_INTERVAL_1000_0_MS	0x0320

#define CONN_EVT_7_5_MS			0x000C	
#define CONN_EVT_10_5_MS		0x0010
#define CONN_EVT_100_5_MS		0x00A0
#define CONN_EVT_1000_5_MS		0x0640

#define CONN_INTERVAL	CONN_INTERVAL_7_5_MS
#define CONN_EVENT		(2 * CONN_INTERVAL)

#define TIMER_INTERVAL									(1)

static uint16_t dest_credit = 0;
static uint32_t gu32NumOfRecv = 0;
//static uint8_t AT_BLE_L2CAP_AD_DATA[] =  {'A','T','-','L','2','C','A','P','\0'};
	
static uint32_t time_count=0;
bool volatile timer_cb_done = false;

at_ble_status_t ble_scan_info(void *params);

at_ble_status_t ble_l2cap_connected(void *params);
at_ble_status_t ble_l2cap_disconnected(void *params);

at_ble_status_t l2cap_connected(void *parma);
at_ble_status_t l2cap_send_resp(void *params);
at_ble_status_t l2cap_data_recv(void *params);
at_ble_status_t l2cap_credit_indication(void *params);
at_ble_status_t l2cap_conn_req(void *params);
at_ble_status_t l2cap_disconnect(void *params);

at_ble_addr_t device1 = {AT_BLE_ADDRESS_PUBLIC, {0xAB, 0xCD, 0xEF, 0xBA, 0xDC, 0xFE}};
	


static at_ble_status_t app_init(void)
{
    at_ble_status_t enuStatus;
    at_ble_addr_t addr = {AT_BLE_ADDRESS_PUBLIC, {0xAB, 0xCD, 0xEF, 0xAB, 0xCD, 0xEF}};
		
	enuStatus = at_ble_addr_set(&addr);
   if(enuStatus != AT_BLE_SUCCESS)
	{
		return enuStatus;
	}
	//Get chip address
	enuStatus = at_ble_addr_get(&addr);

    if(enuStatus != AT_BLE_SUCCESS)
	{
		return enuStatus;
	}
	enuStatus = at_ble_whitelist_add(&device1);
	
    enuStatus = at_ble_scan_start(AT_BLE_INQ_SCAN_INTV, AT_BLE_INQ_SCAN_WIND, 0, AT_BLE_SCAN_ACTIVE, AT_BLE_SCAN_OBSERVER_MODE, true, false);
    DBG_LOG("SCAN Start ... 0x%02X\r\n",   enuStatus);
	
    return enuStatus;
}

static void timer_callback_handler(void)
{
	//timer_cb_done = true;
	send_plf_int_msg_ind(RAM_ISR_TABLE_TIMER0_INDEX, TIMER_EXPIRED_CALLBACK_TYPE_DETECT, NULL, 0);
}

static const ble_gap_event_cb_t ble_mgr_l2cap_gap_handle = {
	.scan_info=ble_scan_info,
	.connected =ble_l2cap_connected,
	.disconnected=ble_l2cap_disconnected
};

static const ble_l2cap_event_cb_t at_ble_mgr_l2cap_event_handle = {
	.lecb_connected = l2cap_connected,
	.lecb_disconnected=l2cap_disconnect,
	.lecb_send_resp=l2cap_send_resp,
	.lecb_data_recieved=l2cap_data_recv,
	.lecb_conn_req=l2cap_conn_req,
	.lecb_add_credit_ind=l2cap_credit_indication
};


at_ble_status_t ble_scan_info(void *params)
{
	at_ble_status_t enuStatus;
	
	at_ble_scan_info_t *scanData = (at_ble_scan_info_t *)((void *)params);
	
	if(memcmp((uint8_t *)device1.addr, (uint8_t *)scanData->dev_addr.addr, 6))
	{
		return AT_BLE_SUCCESS;
	}
	//Stop scanning before connection
	enuStatus = at_ble_scan_stop();
	if (AT_BLE_SUCCESS == enuStatus)
	{
		at_ble_connection_params_t conParams;
		conParams.con_intv_min  = CONN_INTERVAL;
		conParams.con_intv_max  = CONN_INTERVAL;
		conParams.con_latency   = 0x0000;
		conParams.superv_to     = 0x01F4;
		conParams.ce_len_min    = CONN_EVENT;
		conParams.ce_len_max    = CONN_EVENT;

		//Set connection mode to be direct mode
		enuStatus = at_ble_set_conn_mode(AT_BLE_CONNECTION_DIRECT);
		enuStatus = at_ble_connect(&(scanData->dev_addr), 1, 1000, 1000, &conParams);
		if (AT_BLE_SUCCESS == enuStatus)
		{
			DBG_LOG("Request sent\r\n");
		}
		else
		{
			DBG_LOG("Unable to send connect request\r\n");
			return enuStatus;
			
		}
	}
	else
	{
		DBG_LOG("Unable to stop scan\r\n");
	}
 
	return AT_BLE_SUCCESS;
}
at_ble_status_t ble_l2cap_connected(void *params)
{
	at_ble_connected_t *conn_params = (at_ble_connected_t *)((void *)params);
	
	if (AT_BLE_SUCCESS == conn_params->conn_status)
	{
		gstHandle = conn_params->handle;
		//Create LECB channel
		at_ble_lecb_create(gstHandle, AT_BLE_LECB_DISABLE, APP_PSM, APP_CID, APP_CREDIT);
		DBG_LOG("AT_BLE_CONNECTED:\r\n");
	}
	else
	{
		DBG_LOG("Unable to connect to device:\r\n");
		DBG_LOG("Status : %d\r\n", conn_params->conn_status);
	}
	return AT_BLE_SUCCESS;
}

at_ble_status_t ble_l2cap_disconnected(void *params)
{
	at_ble_status_t enuStatus;
	
	 enuStatus = (at_ble_status_t)at_ble_lecb_destroy(gstHandle, APP_PSM);
	 enuStatus = at_ble_scan_start(AT_BLE_INQ_SCAN_INTV, AT_BLE_INQ_SCAN_WIND, 0, AT_BLE_SCAN_ACTIVE, AT_BLE_SCAN_OBSERVER_MODE, true, false);
	return enuStatus;
}

at_ble_status_t l2cap_connected(void *params)
{
	at_ble_lecb_connected_t *p = (at_ble_lecb_connected_t *)((void *)params);
	DBG_LOG("AT_BLE_LECB_CONNECTED:\r\n");
	DBG_LOG("Connected CID : 0x%0X\r\n", p->dest_cid);
	DBG_LOG("DST. Credit   : 0x%0X\r\n", p->dest_credit);
	DBG_LOG("LE PSM        : 0x%0X\r\n", p->le_psm);
	DBG_LOG("MAX. SDU      : 0x%0X\r\n", p->max_sdu);
	dest_credit = p->dest_credit;
	return AT_BLE_SUCCESS;
}

at_ble_status_t l2cap_send_resp(void *params)
{
	at_ble_lecb_send_rsp_t *p = (at_ble_lecb_send_rsp_t *)((void *)params);
	DBG_LOG("RESP: %d\r\n",p->status);
	return AT_BLE_SUCCESS;
} 

at_ble_status_t l2cap_data_recv(void *params)
{
	at_ble_status_t enuStatus;
	
	at_ble_lecb_data_recv_t *recvdData = (at_ble_lecb_data_recv_t *)((void *)params);
	DBG_LOG("DATA_REC:[%d]\r\n", (int)++gu32NumOfRecv);
	if(gu32NumOfRecv == 40)
	{
		LED_On(LED0);
		hw_timer_start(TIMER_INTERVAL);
	}
	if(recvdData->src_credit <= 0x100)
	{
		DBG_LOG_DEV("Adding Credit\r\n");
		enuStatus = (at_ble_status_t)at_ble_lecb_add_credit(gstHandle, APP_PSM, 0x7EFF);
		DBG_LOG("Status: %d \r\n ", enuStatus);
	}
	return AT_BLE_SUCCESS;
}

at_ble_status_t l2cap_credit_indication(void *params)
{
	at_ble_lecb_add_credit_ind_t *p = (at_ble_lecb_add_credit_ind_t *)((void *)params);
	DBG_LOG("AT_BLE_LECB_ADD_CREDIT_IND:\r\n");
	DBG_LOG("Dest. Device credit : %d\r\n", p->dest_credit);
	dest_credit += p->dest_credit;
	return AT_BLE_SUCCESS;
}

at_ble_status_t l2cap_conn_req(void *params)
{
	at_ble_status_t enuStatus;
	at_ble_lecb_conn_req_t *p = (at_ble_lecb_conn_req_t *)((void *)params);
	enuStatus = (at_ble_status_t)at_ble_lecb_cfm(gstHandle, APP_PSM, AT_BLE_LECB_SUCCESS);
	DBG_LOG_DEV("Status: %d \r\n ", enuStatus);
	dest_credit = p->dest_credit;
	return enuStatus;
}

at_ble_status_t l2cap_disconnect(void *params)
{
	 at_ble_status_t enuStatus;
	at_ble_lecb_disconnected_t *p = (at_ble_lecb_disconnected_t *)((void *)params);
	DBG_LOG("AT_BLE_LECB_DISCONNECTED:\r\n");
	DBG_LOG("Reason :%d\n", p->reason);
	DBG_LOG("Destroy connection :%d\n", p->reason);
	enuStatus = (at_ble_status_t)at_ble_lecb_destroy(gstHandle, APP_PSM);
	DBG_LOG("Status: %d \r\n ", enuStatus);
	return enuStatus;
}

/* Callback registered for AT_PLATFORM_EVENT event from stack */
static at_ble_status_t platform_interrupt_app_event(void *param)
{
	at_ble_status_t status = AT_BLE_SUCCESS;
	platform_isr_event_t *plf_isr_event = (platform_isr_event_t *)param;
	
	/* Wait for some time */
	if (plf_isr_event->event_type == ((TIMER_EXPIRED_CALLBACK_TYPE_DETECT << 8)  | RAM_ISR_TABLE_TIMER0_INDEX))
	{
		LED_On(LED0);
		time_count++;
		if(time_count >= TIME_INTERVAL)
		{
			timer_cb_done = true;
			LED_Off(LED0);
			hw_timer_stop();
		}
	}
	else
	{
		status = AT_BLE_FAILURE;
	}
	return status;
}


/* Custom events like user defined event, platform event callback handlers for PXP Monitor profile */
static const ble_custom_event_cb_t app_custom_event_handle = {
	.platform_event_ready = platform_interrupt_app_event /* This event not handled in BLE Manager */
};



int main(void)
{
    at_ble_status_t enuStatus;
	uint32_t through_put=0;
	  
    //Initialize delay routines
    //delay_init();
	platform_driver_init();
	acquire_sleep_lock();
    //Initialize serial for logs
	serial_console_init();
	
	/* Initialize the hardware timer */
	hw_timer_init();
	
	/* Register the callback */
	hw_timer_register_callback(timer_callback_handler);
	
	DBG_LOG("Initializing L2CAP Central Application");
	
	/* initialize the ble chip  and Set the device mac address */
	ble_device_init(NULL);
	
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
	BLE_GAP_EVENT_TYPE,
	&ble_mgr_l2cap_gap_handle);
	
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
	BLE_L2CAP_EVENT_TYPE,
	&at_ble_mgr_l2cap_event_handle);
	
	/* Register callbacks for user defined custom events */
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
									BLE_CUSTOM_EVENT_TYPE,
									&app_custom_event_handle);
	
    enuStatus = app_init();
    if(enuStatus != AT_BLE_SUCCESS)
    {
	    DBG_LOG(">>Unable to start demo!!\r\n");
	    while (1);
    }
    while (!timer_cb_done)
	{
		ble_event_task(0XFFFFFFFF);
	}
	through_put = (((gu32NumOfRecv - 40) / TIME_INTERVAL ) * APP_DATA_LEN);
	through_put = ((through_put*8)/1024);
	DBG_LOG("ThroughPut of Central : %dKbps \r\n",(int)through_put);
	
    return enuStatus;
}
