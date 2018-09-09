/**
 * \file
 *
 * \brief L2CAP PERIPHERAL Application
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
 * \mainpage L2CAP PERIPHERAL Example
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
//#include "aon_sleep_timer_basic.h"
#include "samb11_delay.h"
	
#define TIMER_INTERVAL									(1)
at_ble_handle_t gstHandle = 0xFFFF;

#define APP_PSM			0x80
#define APP_CID			0x40
#define APP_CREDIT		0x7FFF
#define APP_DATA_LEN	507

static uint16_t dest_credit = 0;

static uint8_t au8AdvData[] = { 0x1a, 0xff, 0x4c, 0x00, 0x02, 0x15, 0x21, 0x8A,
                                0xF6, 0x52, 0x73, 0xE3, 0x40, 0xB3, 0xB4, 0x1C,
                                0x19, 0x53, 0x24, 0x2C, 0x72, 0xf4, 0x00, 0xbb,
                                0x00, 0x44, 0xc5
                              };

static uint8_t au8ScanRspData[] = { 0x11, 0x07, 0x1b, 0xc5, 0xd5, 0xa5, 0x02, 0x00,
                                    0x37, 0xaa, 0xe3, 0x11, 0x2a, 0xdc, 0x00, 0xcd,
                                    0x30, 0x57
                                  };

static uint32_t time_count=0;
bool volatile timer_cb_done = false;		

static uint32_t gu32NumOfSend = 0;

at_ble_status_t ble_adv_report(void *params);
at_ble_status_t ble_l2cap_connected(void *params);
at_ble_status_t ble_l2cap_disconnected(void *params);
at_ble_status_t l2cap_connected(void *parma);
at_ble_status_t l2cap_send_resp(void *params);
at_ble_status_t l2cap_data_recv(void *params);
at_ble_status_t l2cap_credit_indication(void *params);
at_ble_status_t l2cap_disconnect(void *params);						  


static at_ble_status_t app_init(void)
{
    at_ble_status_t enuStatus;
    at_ble_addr_t addr = {AT_BLE_ADDRESS_PUBLIC, {0xAB, 0xCD, 0xEF, 0xBA, 0xDC, 0xFE}};
  
	at_ble_status_t status = AT_BLE_FAILURE;
	
	 enuStatus = at_ble_addr_set(&addr);
	 
	if(enuStatus != AT_BLE_SUCCESS)
	{
		return enuStatus;
	}
	
	if((status = at_ble_adv_data_set(au8AdvData, sizeof(au8AdvData), au8ScanRspData, sizeof(au8ScanRspData))) != AT_BLE_SUCCESS)
	{
		return status;
	}
	enuStatus = at_ble_addr_get(&addr);

    if(enuStatus != AT_BLE_SUCCESS)
    {
	    return enuStatus;
    }
	
	/* Start of advertisement */
	if((status = at_ble_adv_start(AT_BLE_ADV_TYPE_UNDIRECTED, AT_BLE_ADV_GEN_DISCOVERABLE, NULL, AT_BLE_ADV_FP_ANY, 0x00A0, 0, 0)) == AT_BLE_SUCCESS)
	{
		DBG_LOG("BLE Started Adv");
		return AT_BLE_SUCCESS;
	}
	else
	{
		DBG_LOG("BLE Adv start Failed reason :%d",status);
	}
	return status;
}


static void timer_callback_handler(void)
{
	//Timer call back
	send_plf_int_msg_ind(RAM_ISR_TABLE_TIMER0_INDEX, TIMER_EXPIRED_CALLBACK_TYPE_DETECT, NULL, 0);
}


static at_ble_lecb_status at_app_send_lecb(uint16_t credit, uint8_t * data)
{
	at_ble_lecb_status status = AT_BLE_LECB_FAIL_NO_AVAIL_RESOURCES;
	if (credit)
	{
		DBG_LOG("Sending Data[%d]\r\n", (int)++gu32NumOfSend);
        status = at_ble_lecb_send(gstHandle, APP_CID, APP_DATA_LEN, data);
      
        if (AT_BLE_LECB_SUCCESS == status)
        {
            dest_credit--;
		}
		if(gu32NumOfSend == 40)
		{
			LED_On(LED0);	
			hw_timer_start(TIMER_INTERVAL);
		}
	}
	return status;
}


static uint8_t	payloadCounter = 1;
uint8_t data[APP_DATA_LEN];
static const ble_gap_event_cb_t ble_mgr_l2cap_gap_handle = {
	.adv_report = ble_adv_report,
	.connected =ble_l2cap_connected,
	.disconnected=ble_l2cap_disconnected
};

static const ble_l2cap_event_cb_t at_ble_mgr_l2cap_event_handle = {
	.lecb_connected = l2cap_connected,
	.lecb_disconnected=l2cap_disconnect,
	.lecb_send_resp=l2cap_send_resp,
	.lecb_data_recieved=l2cap_data_recv,
	.lecb_add_credit_ind=l2cap_credit_indication
};
at_ble_status_t ble_adv_report(void *params)
{
	return AT_BLE_SUCCESS;
}

at_ble_status_t ble_l2cap_connected(void *params)
{
	at_ble_connected_t *conn_params = (at_ble_connected_t *)((void *)params);
	
	if (AT_BLE_SUCCESS == conn_params->conn_status)
	{
		gstHandle = conn_params->handle;
		//wait for server to create channel
		delay_ms(1000);
		at_ble_lecb_connect(gstHandle, APP_PSM, APP_CID, APP_CREDIT);
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
	
	enuStatus = at_ble_adv_start(AT_BLE_ADV_TYPE_UNDIRECTED, AT_BLE_ADV_GEN_DISCOVERABLE, NULL, AT_BLE_ADV_FP_ANY, 0x00A0, 0, 0);
	DBG_LOG("Adv Start ... %d\r\n", enuStatus);
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
	data[1] = payloadCounter++;
	while (AT_BLE_LECB_SUCCESS == at_app_send_lecb(dest_credit, data));
	return AT_BLE_SUCCESS;
}

at_ble_status_t l2cap_send_resp(void *params)
{
	data[1] = payloadCounter++;
	at_app_send_lecb(dest_credit, data);
	
	return AT_BLE_SUCCESS;	
}

at_ble_status_t l2cap_data_recv(void *params)
{
	at_ble_status_t enuStatus;
	uint16_t	u16LoopCntr = 0;
	
	at_ble_lecb_data_recv_t *recvdData = (at_ble_lecb_data_recv_t *)((void *)params);
	
	for (u16LoopCntr = 0; u16LoopCntr < 1/*recvdData->len*/; u16LoopCntr++)
	{
		DBG_LOG("%0x ", recvdData->data[u16LoopCntr]);
	}

	if(recvdData->src_credit <= 0x100)
	{
		DBG_LOG("Adding Credit\r\n");
		enuStatus = (at_ble_status_t)at_ble_lecb_add_credit(gstHandle, APP_PSM, 0x7EFF);
		DBG_LOG("Status: %d \r\n ", enuStatus);
	}	
	return AT_BLE_SUCCESS;
}

at_ble_status_t l2cap_credit_indication(void *params)
{
	at_ble_lecb_add_credit_ind_t *p = (at_ble_lecb_add_credit_ind_t *)((void *)params);
	
	dest_credit += p->dest_credit;
	return AT_BLE_SUCCESS;
}

at_ble_status_t l2cap_disconnect(void *params)
{
	at_ble_lecb_disconnected_t *p = (at_ble_lecb_disconnected_t *)((void *)params);
	DBG_LOG("AT_BLE_LECB_DISCONNECTED:\r\n");
	DBG_LOG("Reason :%d\n", p->reason);
	return AT_BLE_SUCCESS;
}

/* Callback registered for AT_PLATFORM_EVENT event from stack */
static at_ble_status_t platform_interrupt_app_event(void *param)
{
	at_ble_status_t status = AT_BLE_SUCCESS;
	platform_isr_event_t *plf_isr_event = (platform_isr_event_t *)param;
	at_ble_status_t enuStatus = AT_BLE_FAILURE;
	/* Wait for some time */
	if(plf_isr_event->event_type == ((TIMER_EXPIRED_CALLBACK_TYPE_DETECT << 8)  | RAM_ISR_TABLE_TIMER0_INDEX))
{
		time_count++;
		if(time_count == TIME_INTERVAL)
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
	uint16_t	u16LoopCntr = 0;
	uint32_t through_put=0;
	uint8_t	payloadSrc = 0xB0;	//Master-Peripheral
	
	//Initialize delay routines
    platform_driver_init();
	acquire_sleep_lock();
	/* Initialize serial console */
	serial_console_init();
	
	/* Initialize the hardware timer */
	hw_timer_init();
	
	/* Register the callback */
	hw_timer_register_callback(timer_callback_handler);
	
	DBG_LOG("Initializing L2CAP Peripheral Application");
	
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
	for (u16LoopCntr = 0; u16LoopCntr < APP_DATA_LEN; u16LoopCntr++)
	{
		data[u16LoopCntr] = u16LoopCntr;
	}
	data[0] = payloadSrc;
	   
    while (!timer_cb_done)
	{
		ble_event_task(0XFFFFFFFF);
	}
	through_put = (((gu32NumOfSend - 40) / TIME_INTERVAL ) * APP_DATA_LEN);
	through_put = ((through_put*8)/1024);
	DBG_LOG("ThroughPut of Peripheral : %dKbps \r\n",(int)through_put);
	
    return enuStatus;
}
