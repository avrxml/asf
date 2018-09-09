/*
 * Copyright (C) 2014 BlueKitchen GmbH
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holders nor the names of
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 * 4. Any redistribution, use, or modification is done solely for
 *    personal benefit and not for any commercial purpose or for
 *    monetary gain.
 *
 * THIS SOFTWARE IS PROVIDED BY BLUEKITCHEN GMBH AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL MATTHIAS
 * RINGWALD OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * Please inquire about commercial licensing options at 
 * contact@bluekitchen-gmbh.com
 *
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "malloc.h"

#include "le_transparent.h"
#include "btstack.h"

static tss_state_t state = TSS_IDLE;	
static  Packet_Buffer 	PacketIn[MAX_PACKET_BUFFER_LIST];
static  App_List		appList;
static  Handle_List		localHandleList;
static btstack_packet_callback_registration_t hci_event_callback_registration;
static void packet_handler (uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size);
static uint16_t att_read_callback(hci_con_handle_t con_handle, uint16_t att_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size);
static int att_write_callback(hci_con_handle_t con_handle, uint16_t att_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size);
static uint16_t tss_server_return_credit(uint8_t credit);
uint8_t payload_data[5];

const uint8_t tss_adv_data[] = {
    // Flags general discoverable, BR/EDR not supported
    0x02, BLUETOOTH_DATA_TYPE_FLAGS, 0x06, 
    // Name
   	0x0f, BLUETOOTH_DATA_TYPE_COMPLETE_LOCAL_NAME, 'L', 'E', '-', 'T', 'R', 'A', 'N', 'S', 'P', 'A', 'R', 'E', 'N', 'T',
	// Incomplete List of 16-bit Service Class UUIDs -- 5343 - only valid for testing!
    0x03, BLUETOOTH_DATA_TYPE_INCOMPLETE_LIST_OF_16_BIT_SERVICE_CLASS_UUIDS, 0x43, 0x53
};
const uint8_t adv_data_len = sizeof(tss_adv_data);

void tss_start_advertising(void)
{
	// setup advertisements
	uint16_t adv_int_min = 0x0030;
	uint16_t adv_int_max = 0x0030;
	uint8_t adv_type = 0;
	bd_addr_t null_addr;
	memset(null_addr, 0, 6);
	gap_advertisements_set_params(adv_int_min, adv_int_max, adv_type, 0, null_addr, 0x07, 0x00);
	gap_advertisements_set_data(adv_data_len, (uint8_t*) tss_adv_data);
	gap_advertisements_enable(1);
}

void tss_app_initialize(void)
{
	appList.attMtu= 23;
	appList.appState=APP_STATE_STANDBY;
	appList.creditBasedEnable=CREDIT_BASED_FLOW_CONTROL_DISABLED;
	appList.credit=0;
	appList.creditBasedStoMEnable=CREDIT_BASED_FLOW_CONTROL_DISABLED;
	appList.creditStoM=0;
	tss_start_advertising();
}

void tss_app_setup(void)
{

    // register for HCI events
    hci_event_callback_registration.callback = &packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);

    l2cap_init();

    // setup le device db
    le_device_db_init();

    // setup SM: Display only
    sm_init();

    // setup ATT server
    att_server_init(profile_data, att_read_callback, att_write_callback);    
    att_server_register_packet_handler(packet_handler);
	
	localHandleList.transTx		=	ATT_CHARACTERISTIC_TX_CHARATERISITC_VALUE_HANDLE;
	localHandleList.txCCCD		=	ATT_CHARACTERISTIC_TX_CHARATERISITC_CCD_VALUE_HANDLE;
	localHandleList.transRx		=	ATT_CHARACTERISTIC_RX_CHARATERISITC_VALUE_HANDLE;
	localHandleList.transCtrl	=	ATT_CHARACTERISTIC_CONTROL_POINT_CHARATERISITC_VALUE_HANDLE;
	localHandleList.ctrlCCCD	=	ATT_CHARACTERISTIC_CONTROL_POINT_CHARATERISITC_CCD_VALUE_HANDLE;

    tss_app_initialize();
   
}

static void packet_handler (uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size){
    UNUSED(channel);
    UNUSED(size);
     
    switch (packet_type) {
        case HCI_EVENT_PACKET:
            switch (hci_event_packet_get_type(packet))
			{
                case HCI_EVENT_LE_META:
					switch(hci_event_le_meta_get_subevent_code(packet))
					{
						case HCI_SUBEVENT_LE_CONNECTION_COMPLETE:
							appList.appState=APP_STATE_LE_CONNECTED;
							appList.connHandle = hci_subevent_le_connection_complete_get_connection_handle(packet);
							appList.role=hci_subevent_le_connection_complete_get_role(packet);
							if(appList.role==BLE_GAP_ROLE_SLAVE)
							{
								appList.appState=APP_STATE_LE_OPEN; 
							}
						break;
					}
				break;
				case HCI_EVENT_DISCONNECTION_COMPLETE:
					 tss_app_initialize();
                     break;
				case ATT_EVENT_MTU_EXCHANGE_COMPLETE:
				    appList.attMtu = ((packet[5]<<8) | packet[4]); 
					printf("\r\nATT MTU %d\n", appList.attMtu);
					break;	 
                case ATT_EVENT_CAN_SEND_NOW:
				     switch (state) {
						 case TSS_IDLE:
						 break;
						 case TSS_RETURN_CREDIT_S_TO_C:
							if((att_server_notify(appList.connHandle, localHandleList.transCtrl, payload_data, 0x05)) == TSS_RES_SUCCESS)
							{
								appList.credit=0;
							}
							else
							{
								log_info("TSS_RETURN_CREDIT_S_TO_C : Fail to notify");
							}	 
						 break;
						 
						 case TSS_RETURN_DATA_S_TO_C:
						    if((att_server_notify(appList.connHandle, localHandleList.transTx, PacketIn[appList.inReadIndex].packet, PacketIn[appList.inReadIndex].length)) == TSS_RES_SUCCESS)
							{
								free(PacketIn[appList.inReadIndex].packet);
								if(appList.role==BLE_GAP_ROLE_SLAVE)/*Slave receive data from Master*/
								{
									if(appList.creditBasedEnable==CREDIT_BASED_FLOW_CONTROL_ENABLED)
									{
										appList.credit++;
										tss_server_return_credit(appList.credit);
									}
								}
								appList.inUsedNum--;
								appList.inReadIndex++;
								if(appList.inReadIndex==MAX_PACKET_BUFFER_LIST)
								{
									appList.inReadIndex=0;
								}
								if(appList.inUsedNum)
								{
									process_data();
								}
								
							}
							else
							{
								log_info("TSS_RETURN_DATA_S_TO_C : Fail to notify");
							}
						 break;
						 default:
						 break;
					 }
				    break;
            }
            break;
    }
}

static uint16_t att_read_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size){
    UNUSED(connection_handle);
    return 0;
}
/* LISTING_END */

void process_data(void)
{
	if(appList.role==BLE_GAP_ROLE_SLAVE)
	{
		state = TSS_RETURN_DATA_S_TO_C;
		att_server_request_can_send_now_event(appList.connHandle);
	}
}

static uint16_t tss_server_return_credit(uint8_t credit)
{
	if(credit==0)
	{
		return TSS_RES_SUCCESS;	
	}
	
	payload_data[0]=CREDIT_CTRL_STATUS_SUCCESS;
	payload_data[1]=TR_READ_MTU_SIZE_AND_ENABLE_CREDIT_CTRL;
	payload_data[2]=appList.attMtu>>8;
	payload_data[3]=appList.attMtu;
	payload_data[4]=credit;
	
	state = TSS_RETURN_CREDIT_S_TO_C;
	att_server_request_can_send_now_event(appList.connHandle);
	return TSS_RES_SUCCESS;
}

void app_process_incomming_packet(void) 
{
	process_data();
}

static void push_into_receive_buffer(uint8_t len, uint8_t * buf)
{
	appList.inUsedNum++;
	PacketIn[appList.inWriteIndex].length=len;
	PacketIn[appList.inWriteIndex].packet=buf;
	appList.inWriteIndex++;
	if(appList.inWriteIndex==MAX_PACKET_BUFFER_LIST)
	{
		appList.inWriteIndex=0;
	}
	app_process_incomming_packet();
}

static int att_write_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size){
    uint16_t size = buffer_size;
    uint16_t i = 0;
	UNUSED(transaction_mode);
    UNUSED(offset);
	
    // Receive Write Request
	if(att_handle == localHandleList.txCCCD)
	{
		uint16_t cccd;
		cccd = little_endian_read_16(buffer, 0);
		if(cccd == GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_NOTIFICATION)
		{
			appList.appState=APP_STATE_LE_OPEN;			
		}
		else
		{
			appList.appState=APP_STATE_LE_CONNECTED;
		}
		
	}
	else if (att_handle == localHandleList.transRx)
	{			
		printf("\rReceive data from client\n\n");
		for(i=0; i<size; i++)
		{
			printf("%c", buffer[i]);
		}
		printf("\r\n\n");
		
		if(appList.inUsedNum<MAX_PACKET_BUFFER_LIST)
		{
			uint8_t *p_buffer;
			p_buffer=malloc(buffer_size);
			if(p_buffer!=NULL)
			{
				memcpy(p_buffer, buffer, buffer_size);
				push_into_receive_buffer(buffer_size, p_buffer);
			}
			else
			{
				printf("Fail to allocate memory");
			}
		}
	}
	else if (att_handle == localHandleList.transCtrl)
	{
		if(buffer[0] == TR_READ_MTU_SIZE_AND_ENABLE_CREDIT_CTRL)
		{
			appList.creditBasedEnable=CREDIT_BASED_FLOW_CONTROL_ENABLED;
			tss_server_return_credit(MAX_PACKET_BUFFER_LIST);
		}
	}
    
    return 0;
}
/* LISTING_END */

int btstack_main(void);
int btstack_main(void)
{
    tss_app_setup();

    // turn on!
	hci_power_control(HCI_POWER_ON);
	    
    return 0;
}
/* EXAMPLE_END */
