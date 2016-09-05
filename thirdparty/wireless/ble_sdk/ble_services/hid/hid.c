/**
 * \file
 *
 * \brief HID Service
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
 * This is the reference manual for the HID Service
 */
/*- Includes ---------------------------------------------------------------*/
#include "stddef.h"
#include "stdio.h"
#include "string.h"
#include "at_ble_api.h"
#include "ble_manager.h"
#include "hid.h"
#include "hid_device.h"

at_ble_generic_att_desc_t report_desc[HID_NUM_OF_REPORT];
hid_gatt_serv_handler_t hid_inst[HID_MAX_SERV_INST];
hid_serv_t hid_serv_inst[HID_MAX_SERV_INST];   
uint8_t ctrl_point[1];

#ifdef HID_KEYBOARD_DEVICE	
uint8_t Keyb_out_report[1];
uint8_t keyb_in_report[8]; 
#endif

#ifdef HID_MOUSE_DEVICE
int8_t mouse_in_report[4];
#endif

/**
* \HID service definition initialization function
*/
void hid_serv_def_init(uint8_t servnum)
{
	uint8_t id = 0;
	hid_serv_inst[servnum].hid_dev_serv_handle = NULL;
	hid_serv_inst[servnum].hid_dev_serv_uuid = NULL;
	hid_serv_inst[servnum].hid_dev_proto_mode_char = NULL;
	hid_serv_inst[servnum].hid_dev_report_map_char = NULL;
	for(; id<HID_NUM_OF_REPORT; id++){
		hid_serv_inst[servnum].hid_dev_report_val_char[id] = NULL;
	}
#ifdef HID_KEYBOARD_DEVICE	
	hid_serv_inst[servnum].hid_dev_boot_keyboard_in_report = NULL;
	hid_serv_inst[servnum].hid_dev_boot_keyboard_out_report= NULL;
#endif

#ifdef HID_MOUSE_DEVICE
    hid_serv_inst[servnum].hid_dev_boot_mouse_in_report= NULL;
#endif
    hid_serv_inst[servnum].hid_dev_info= NULL;
    hid_serv_inst[servnum].hid_control_point= NULL;
}

/**
* \HID service Configuration function
*/
void hid_serv_init(uint8_t servinst, uint8_t device, uint8_t *mode, uint8_t report_num, uint8_t *report_type, uint8_t **report_val, uint8_t *report_len, hid_info_t *info)
{
	uint8_t id = 0;
	/* Configure the HID service type */
	hid_inst[servinst].serv.type = PRIMARY_SERVICE;

	/* Configure the HID service permission */
	if(BLE_PAIR_ENABLE){
		hid_inst[servinst].serv.perm = (AT_BLE_ATTR_READABLE_REQ_AUTHN_NO_AUTHR | AT_BLE_ATTR_WRITABLE_REQ_AUTHN_NO_AUTHR);
	}else{
		hid_inst[servinst].serv.perm = (AT_BLE_ATTR_READABLE_NO_AUTHN_NO_AUTHR | AT_BLE_ATTR_WRITABLE_NO_AUTHN_NO_AUTHR);
	}
	
	/* Configure the HID service handle */
	hid_inst[servinst].serv.handle = 0;
	
	/* Configure HID Service */
	hid_inst[servinst].serv.uuid.type = AT_BLE_UUID_16;
	hid_inst[servinst].serv.uuid.uuid[0] = (uint8_t) HID_SERV_UUID;
	hid_inst[servinst].serv.uuid.uuid[1] = (uint8_t) (HID_SERV_UUID>>8);	
	
	/* Configure HID include service list */
	hid_inst[servinst].serv.inc_list = NULL;
	
	/* Configure the HID include service count */
	hid_inst[servinst].serv.included_count = 0;
	
	/*Map the service UUID instance*/
	hid_serv_inst[servinst].hid_dev_serv_uuid = &hid_inst[servinst].serv.uuid;
	
	/*Map the service UUID instance*/
	hid_serv_inst[servinst].hid_dev_serv_handle = &hid_inst[servinst].serv.handle;
	
	/*Configure HID Protocol Mode Characteristic : Value related info*/
    hid_inst[servinst].serv_chars[0].char_val.handle = 0;
	hid_inst[servinst].serv_chars[0].char_val.uuid.type = AT_BLE_UUID_16;
	hid_inst[servinst].serv_chars[0].char_val.uuid.uuid[0] =(uint8_t) HID_UUID_CHAR_PROTOCOL_MODE;
	hid_inst[servinst].serv_chars[0].char_val.uuid.uuid[1] = (uint8_t)(HID_UUID_CHAR_PROTOCOL_MODE >> 8);
	hid_inst[servinst].serv_chars[0].char_val.properties = (AT_BLE_CHAR_READ|AT_BLE_CHAR_WRITE_WITHOUT_RESPONSE);
	hid_inst[servinst].serv_chars[0].char_val.init_value = mode;
	hid_inst[servinst].serv_chars[0].char_val.len = sizeof(uint8_t);
	
	/* Configure the HID characteristic value permission */
	hid_inst[servinst].serv_chars[0].char_val.permissions = (AT_BLE_ATTR_READABLE_NO_AUTHN_NO_AUTHR | AT_BLE_ATTR_WRITABLE_NO_AUTHN_NO_AUTHR);

	/*Configure HID Protocol Mode Characteristic : user descriptor related info */
	hid_inst[servinst].serv_chars[0].user_desc.user_description = NULL;
	hid_inst[servinst].serv_chars[0].user_desc.len = 0;
	hid_inst[servinst].serv_chars[0].user_desc.handle = 0;
	hid_inst[servinst].serv_chars[0].user_desc.permissions = AT_BLE_ATTR_NO_PERMISSIONS;
	
	/*Configure HID Protocol Mode Characteristic : presentation format related info */
	hid_inst[servinst].serv_chars[0].presentation_format = NULL;
	
	/*Configure HID Protocol Mode Characteristic : client config descriptor related info */
	hid_inst[servinst].serv_chars[0].client_config_desc.perm = AT_BLE_ATTR_NO_PERMISSIONS;
	hid_inst[servinst].serv_chars[0].client_config_desc.handle = 0;
	
	/*Configure HID Protocol Mode Characteristic : server config descriptor related info */
	hid_inst[servinst].serv_chars[0].server_config_desc.perm = AT_BLE_ATTR_NO_PERMISSIONS;
	hid_inst[servinst].serv_chars[0].server_config_desc.handle = 0;
	
	/*Configure HID Protocol Mode Characteristic : generic descriptor related info */
	hid_inst[servinst].serv_chars[0].additional_desc_list = NULL;
	
	/*Configure HID Protocol Mode Characteristic : count of generic descriptor */
	hid_inst[servinst].serv_chars[0].additional_desc_count = 0;
	
    /*Map the device protocol mode characteristic*/
	hid_serv_inst[servinst].hid_dev_proto_mode_char = &hid_inst[servinst].serv_chars[0];
	
	/*Configure HID Report Map Characteristic : Value related info*/
	hid_inst[servinst].serv_chars[1].char_val.handle = 0;
	hid_inst[servinst].serv_chars[1].char_val.uuid.type = AT_BLE_UUID_16;
	hid_inst[servinst].serv_chars[1].char_val.uuid.uuid[0] =(uint8_t) HID_UUID_CHAR_REPORT_MAP;
	hid_inst[servinst].serv_chars[1].char_val.uuid.uuid[1] = (uint8_t)(HID_UUID_CHAR_REPORT_MAP >> 8);
	hid_inst[servinst].serv_chars[1].char_val.properties = AT_BLE_CHAR_READ;
	
	/* Configure the HID characteristic value permission */
	if(BLE_PAIR_ENABLE){
		hid_inst[servinst].serv_chars[1].char_val.permissions = AT_BLE_ATTR_READABLE_REQ_AUTHN_NO_AUTHR;
		}else{
		hid_inst[servinst].serv_chars[1].char_val.permissions = AT_BLE_ATTR_READABLE_NO_AUTHN_NO_AUTHR;
	}
	
	/*Configure HID Report Map Characteristic : user descriptor related info */
	hid_inst[servinst].serv_chars[1].user_desc.user_description = NULL;
	hid_inst[servinst].serv_chars[1].user_desc.len = 0;
	hid_inst[servinst].serv_chars[1].user_desc.handle = 0;
	hid_inst[servinst].serv_chars[1].user_desc.permissions = AT_BLE_ATTR_NO_PERMISSIONS;
	
	/*Configure HID Report Map Characteristic : presentation format related info */
	hid_inst[servinst].serv_chars[1].presentation_format = NULL;
	
	/*Configure HID Report Map Characteristic : client config descriptor related info */
	hid_inst[servinst].serv_chars[1].client_config_desc.perm = AT_BLE_ATTR_NO_PERMISSIONS;
	hid_inst[servinst].serv_chars[1].client_config_desc.handle = 0;
	
	/*Configure HID Report Map Characteristic : server config descriptor related info */
	hid_inst[servinst].serv_chars[1].server_config_desc.perm = AT_BLE_ATTR_NO_PERMISSIONS;
	hid_inst[servinst].serv_chars[1].server_config_desc.handle = 0;
	
	/*Configure HID Report Map Characteristic : generic descriptor related info */
	hid_inst[servinst].serv_chars[1].additional_desc_list = NULL;
	
	/*Configure HID Report Map Characteristic : count of generic descriptor */
	hid_inst[servinst].serv_chars[1].additional_desc_count = 0;
	
	/*Map the device Report Map characteristic*/
	hid_serv_inst[servinst].hid_dev_report_map_char = &hid_inst[servinst].serv_chars[1];

	/*Configure number of HID report*/
	for(id = 1; id <= report_num; ++id){
		/*Configure HID Report Characteristic : Value related info*/
		hid_inst[servinst].serv_chars[id + 1].char_val.handle = 0;
		hid_inst[servinst].serv_chars[id + 1].char_val.uuid.type = AT_BLE_UUID_16;
		hid_inst[servinst].serv_chars[id + 1].char_val.uuid.uuid[0] =(uint8_t) HID_UUID_CHAR_REPORT;
		hid_inst[servinst].serv_chars[id + 1].char_val.uuid.uuid[1] = (uint8_t)(HID_UUID_CHAR_REPORT >> 8);
		hid_inst[servinst].serv_chars[id + 1].char_val.init_value = ((report_val[id-1]));
		hid_inst[servinst].serv_chars[id + 1].char_val.len = report_len[id-1];

		if(report_type[id-1] == INPUT_REPORT){

#ifdef ENABLE_PTS
		uint8_t i=0;
		DBG_LOG_PTS("Input Report Characteristic Value");
		for (i=0; i<hid_inst[servinst].serv_chars[id + 1].char_val.len; i++)
		{
			DBG_LOG_PTS(" 0x%02X ", hid_inst[servinst].serv_chars[id + 1].char_val.init_value[i]);
		}
		DBG_LOG_PTS("\r\n");
#endif // _DEBUG			
			hid_inst[servinst].serv_chars[id + 1].char_val.properties = (AT_BLE_CHAR_READ|AT_BLE_CHAR_NOTIFY);
			
			/* Configure the HID characteristic value permission */
			if(BLE_PAIR_ENABLE){
				hid_inst[servinst].serv_chars[id + 1].char_val.permissions = AT_BLE_ATTR_READABLE_REQ_AUTHN_NO_AUTHR;
				}else{
				hid_inst[servinst].serv_chars[id + 1].char_val.permissions = AT_BLE_ATTR_READABLE_NO_AUTHN_NO_AUTHR;
			}
			/*Configure HID Report Characteristic : client config descriptor related info */
			hid_inst[servinst].serv_chars[id + 1].client_config_desc.perm = (AT_BLE_ATTR_READABLE_NO_AUTHN_NO_AUTHR | AT_BLE_ATTR_WRITABLE_NO_AUTHN_NO_AUTHR);
			hid_inst[servinst].serv_chars[id + 1].client_config_desc.handle = 0;
		}else if(report_type[id-1] == OUTPUT_REPORT){
			hid_inst[servinst].serv_chars[id + 1].char_val.properties = (AT_BLE_CHAR_READ|AT_BLE_CHAR_WRITE_WITHOUT_RESPONSE|AT_BLE_CHAR_WRITE);
			
			/* Configure the HID characteristic value permission */
			if(BLE_PAIR_ENABLE){
				hid_inst[servinst].serv_chars[id + 1].char_val.permissions = (AT_BLE_ATTR_READABLE_REQ_AUTHN_NO_AUTHR | AT_BLE_ATTR_WRITABLE_REQ_AUTHN_NO_AUTHR);
				}else{
				hid_inst[servinst].serv_chars[id + 1].char_val.permissions = (AT_BLE_ATTR_READABLE_NO_AUTHN_NO_AUTHR | AT_BLE_ATTR_WRITABLE_NO_AUTHN_NO_AUTHR);
			}
			
			/*Configure HID Report Characteristic : client config descriptor related info */
			hid_inst[servinst].serv_chars[id + 1].client_config_desc.perm = AT_BLE_ATTR_NO_PERMISSIONS;
			hid_inst[servinst].serv_chars[id + 1].client_config_desc.handle = 0;
		}else if(report_type[id-1] == FEATURE_REPORT){
			hid_inst[servinst].serv_chars[id + 1].char_val.properties = (AT_BLE_CHAR_READ|AT_BLE_CHAR_WRITE);
			
			/* Configure the HID characteristic permission */
			if(BLE_PAIR_ENABLE){
				hid_inst[servinst].serv_chars[id + 1].char_val.permissions = (AT_BLE_ATTR_READABLE_REQ_AUTHN_NO_AUTHR | AT_BLE_ATTR_WRITABLE_REQ_AUTHN_NO_AUTHR);
				}else{
				hid_inst[servinst].serv_chars[id + 1].char_val.permissions = (AT_BLE_ATTR_READABLE_NO_AUTHN_NO_AUTHR | AT_BLE_ATTR_WRITABLE_NO_AUTHN_NO_AUTHR);
			}
			
			/*Configure HID Report Characteristic : client config descriptor related info */
			hid_inst[servinst].serv_chars[id + 1].client_config_desc.perm = AT_BLE_ATTR_NO_PERMISSIONS;
			hid_inst[servinst].serv_chars[id + 1].client_config_desc.handle = 0; 
		}
 
		/*Configure HID Report Characteristic : user descriptor related info */
		hid_inst[servinst].serv_chars[id + 1].user_desc.user_description = NULL;
		hid_inst[servinst].serv_chars[id + 1].user_desc.len = 0;
		hid_inst[servinst].serv_chars[id + 1].user_desc.handle = 0;
		hid_inst[servinst].serv_chars[id + 1].user_desc.permissions = AT_BLE_ATTR_NO_PERMISSIONS;
				
		/*Configure HID Report Characteristic : presentation format related info */
		hid_inst[servinst].serv_chars[id + 1].presentation_format = NULL;  
		
		/*Configure HID Report Map Characteristic : server config descriptor related info */
		hid_inst[servinst].serv_chars[id + 1].server_config_desc.perm = AT_BLE_ATTR_NO_PERMISSIONS;
		hid_inst[servinst].serv_chars[id + 1].server_config_desc.handle = 0;
		
		/* Configure HID Report Reference Descriptor*/
		hid_inst[servinst].serv_desc[id - 1].desc_val_length = (sizeof(uint8_t)*2);
		hid_inst[servinst].serv_desc[id - 1].perm = AT_BLE_ATTR_READABLE_NO_AUTHN_NO_AUTHR;
		hid_inst[servinst].serv_desc[id - 1].uuid.type = AT_BLE_UUID_16;
		hid_inst[servinst].serv_desc[id - 1].uuid.uuid[0] =(uint8_t) HID_REPORT_REF_DESC;
		hid_inst[servinst].serv_desc[id - 1].uuid.uuid[1] = (uint8_t)(HID_REPORT_REF_DESC >> 8);
		hid_inst[servinst].serv_desc[id - 1].handle   =  0;
				
		
		/*Configure HID Report Characteristic : generic descriptor related info */
		hid_inst[servinst].serv_chars[id + 1].additional_desc_list = &hid_inst[servinst].serv_desc[id - 1];
		
		/*Configure HID Report Characteristic : count of generic descriptor */
		hid_inst[servinst].serv_chars[id + 1].additional_desc_count = 1;
			
		/*Map the report characteristic*/
		hid_serv_inst[servinst].hid_dev_report_val_char[id-1] = &hid_inst[servinst].serv_chars[id + 1];
		
	}//End of for loop
	
	id++;
	
	if(device == HID_MOUSE_MODE){
#ifdef HID_MOUSE_DEVICE
		
#ifdef ENABLE_PTS
	uint8_t i=0;
	DBG_LOG_PTS("Boot Mouse Input Report Characteristic Value");
		
	for (i=0; i<sizeof(mouse_in_report); i++)
	{
		DBG_LOG_PTS(" 0x%02X ", mouse_in_report[i]);
	}
	DBG_LOG_PTS("\r\n");
#endif // _DEBUG
		
		/*Configure HID Boot Mouse Input Report Characteristic : Value related info*/
		hid_inst[servinst].serv_chars[id].char_val.handle = 0;
		hid_inst[servinst].serv_chars[id].char_val.uuid.type = AT_BLE_UUID_16;
		hid_inst[servinst].serv_chars[id].char_val.uuid.uuid[0] =(uint8_t) HID_UUID_CHAR_BOOT_MOUSE_INPUT_REPORT;
		hid_inst[servinst].serv_chars[id].char_val.uuid.uuid[1] = (uint8_t)(HID_UUID_CHAR_BOOT_MOUSE_INPUT_REPORT >> 8);
		hid_inst[servinst].serv_chars[id].char_val.init_value = (uint8_t*)&mouse_in_report;
		hid_inst[servinst].serv_chars[id].char_val.len = sizeof(mouse_in_report);
		hid_inst[servinst].serv_chars[id].char_val.properties = (AT_BLE_CHAR_READ|AT_BLE_CHAR_NOTIFY);
		
		/* Configure the HID characteristic value permission */
		if(BLE_PAIR_ENABLE){
			hid_inst[servinst].serv_chars[id].char_val.permissions = AT_BLE_ATTR_READABLE_REQ_AUTHN_NO_AUTHR;
			}else{
			hid_inst[servinst].serv_chars[id].char_val.permissions = AT_BLE_ATTR_READABLE_NO_AUTHN_NO_AUTHR;
		}
		
        /*Configure HID Boot Mouse Input Report Characteristic : user descriptor related info */
        hid_inst[servinst].serv_chars[id].user_desc.user_description = NULL;
        hid_inst[servinst].serv_chars[id].user_desc.len = 0;
        hid_inst[servinst].serv_chars[id].user_desc.handle = 0;
        hid_inst[servinst].serv_chars[id].user_desc.permissions = AT_BLE_ATTR_NO_PERMISSIONS;
        
        /*Configure HID Boot Mouse Input Report Characteristic : presentation format related info */
        hid_inst[servinst].serv_chars[id].presentation_format = NULL;
        
        /*Configure HID Boot Mouse Input Report Characteristic : client config descriptor related info */
        hid_inst[servinst].serv_chars[id].client_config_desc.perm = (AT_BLE_ATTR_READABLE_NO_AUTHN_NO_AUTHR | AT_BLE_ATTR_WRITABLE_NO_AUTHN_NO_AUTHR);
        hid_inst[servinst].serv_chars[id].client_config_desc.handle = 0;
        
        /*Configure HID Boot Mouse Input Report Characteristic : server config descriptor related info */
        hid_inst[servinst].serv_chars[id].server_config_desc.perm = AT_BLE_ATTR_NO_PERMISSIONS;
        hid_inst[servinst].serv_chars[id].server_config_desc.handle = 0;
        
        /*Configure HID Boot Mouse Input Report Characteristic : generic descriptor related info */
        hid_inst[servinst].serv_chars[id].additional_desc_list = NULL;
        
        /*Configure HID Boot Mouse Input Report Characteristic : count of generic descriptor */
        hid_inst[servinst].serv_chars[id].additional_desc_count = 0;
         
		/*Map the boot mouse input report characteristic*/
		hid_serv_inst[servinst].hid_dev_boot_mouse_in_report = &hid_inst[servinst].serv_chars[id];
		id++;
#endif
	}else if(device == HID_KEYBOARD_MODE){
#ifdef HID_KEYBOARD_DEVICE

#ifdef ENABLE_PTS
	uint8_t i=0;
	DBG_LOG_PTS("Boot Keyboard Input Report Characteristic Value");

	for (i=0; i<sizeof(keyb_in_report); i++)	
	{
		DBG_LOG_PTS(" 0x%02X ", keyb_in_report[i]);
	}
	DBG_LOG_PTS("\r\n");
	
	DBG_LOG("Boot Keyboard Output Report Characteristic Value");

	for (i=0; i<sizeof(Keyb_out_report); i++)
	{
		printf(" 0x%02X ", Keyb_out_report[i]);
	}
	printf("\r\n");
#endif // _DEBUG
        /*Configure HID Boot Keyboard Output Report Characteristic : Value related info*/
		hid_inst[servinst].serv_chars[id].char_val.handle = 0;
		hid_inst[servinst].serv_chars[id].char_val.uuid.type = AT_BLE_UUID_16;
		hid_inst[servinst].serv_chars[id].char_val.uuid.uuid[0] =(uint8_t) HID_UUID_CHAR_BOOT_KEY_OUTPUT_REPORT;
		hid_inst[servinst].serv_chars[id].char_val.uuid.uuid[1] = (uint8_t)(HID_UUID_CHAR_BOOT_KEY_OUTPUT_REPORT >> 8);
		hid_inst[servinst].serv_chars[id].char_val.init_value = (uint8_t*)&Keyb_out_report; 
		hid_inst[servinst].serv_chars[id].char_val.len = sizeof(uint8_t); 
		hid_inst[servinst].serv_chars[id].char_val.properties = (AT_BLE_CHAR_READ|AT_BLE_CHAR_WRITE_WITHOUT_RESPONSE|AT_BLE_CHAR_WRITE);
		
		/* Configure the HID characteristic value permission */
		if(BLE_PAIR_ENABLE){
			hid_inst[servinst].serv_chars[id].char_val.permissions = (AT_BLE_ATTR_READABLE_REQ_AUTHN_NO_AUTHR | AT_BLE_ATTR_WRITABLE_REQ_AUTHN_NO_AUTHR);
			}else{
			hid_inst[servinst].serv_chars[id].char_val.permissions = (AT_BLE_ATTR_READABLE_NO_AUTHN_NO_AUTHR | AT_BLE_ATTR_WRITABLE_NO_AUTHN_NO_AUTHR);
		}
		
		/*Configure HID Boot Keyboard Output Report Characteristic : user descriptor related info */
		hid_inst[servinst].serv_chars[id].user_desc.user_description = NULL;
		hid_inst[servinst].serv_chars[id].user_desc.len = 0;
		hid_inst[servinst].serv_chars[id].user_desc.handle = 0;
		hid_inst[servinst].serv_chars[id].user_desc.permissions = AT_BLE_ATTR_NO_PERMISSIONS;
		
		/*Configure HID Boot Keyboard Output Report Characteristic : presentation format related info */
		hid_inst[servinst].serv_chars[id].presentation_format = NULL;
		
		/*Configure HID Boot Keyboard Output Report Characteristic : client config descriptor related info */
		hid_inst[servinst].serv_chars[id].client_config_desc.perm = AT_BLE_ATTR_NO_PERMISSIONS;
		hid_inst[servinst].serv_chars[id].client_config_desc.handle = 0;
		
		/*Configure HID Boot Keyboard Output Report Characteristic : server config descriptor related info */
		hid_inst[servinst].serv_chars[id].server_config_desc.perm = AT_BLE_ATTR_NO_PERMISSIONS;
		hid_inst[servinst].serv_chars[id].server_config_desc.handle = 0;
		
		/*Configure HID Boot Keyboard Output Report Characteristic : generic descriptor related info */
		hid_inst[servinst].serv_chars[id].additional_desc_list = NULL;
		
		/*Configure HID Boot Keyboard Output Report Characteristic : count of generic descriptor */
		hid_inst[servinst].serv_chars[id].additional_desc_count = 0;

		 /*Map the keyboard output report characteristic*/
		 hid_serv_inst[servinst].hid_dev_boot_keyboard_out_report = &hid_inst[servinst].serv_chars[id];
		 
		 id++;
		 
		 /*Configure HID Boot Keyboard Input Report Characteristic : Value related info*/
		 hid_inst[servinst].serv_chars[id].char_val.handle = 0;
		 hid_inst[servinst].serv_chars[id].char_val.uuid.type = AT_BLE_UUID_16;
		 hid_inst[servinst].serv_chars[id].char_val.uuid.uuid[0] =(uint8_t) HID_UUID_CHAR_BOOT_KEY_INPUT_REPORT;
		 hid_inst[servinst].serv_chars[id].char_val.uuid.uuid[1] = (uint8_t)(HID_UUID_CHAR_BOOT_KEY_INPUT_REPORT >> 8);
		 hid_inst[servinst].serv_chars[id].char_val.init_value = (uint8_t*)&keyb_in_report;
		 hid_inst[servinst].serv_chars[id].char_val.len = sizeof(keyb_in_report); 
		 hid_inst[servinst].serv_chars[id].char_val.properties = (AT_BLE_CHAR_READ|AT_BLE_CHAR_NOTIFY);
		 
		 /* Configure the HID characteristic value permission */
		 if(BLE_PAIR_ENABLE){
			 hid_inst[servinst].serv_chars[id].char_val.permissions = AT_BLE_ATTR_READABLE_REQ_AUTHN_NO_AUTHR;
			 }else{
			 hid_inst[servinst].serv_chars[id].char_val.permissions = AT_BLE_ATTR_READABLE_NO_AUTHN_NO_AUTHR;
		 }
		
		 /*Configure HID Boot Keyboard Input Report Characteristic : user descriptor related info */
		 hid_inst[servinst].serv_chars[id].user_desc.user_description = NULL;
		 hid_inst[servinst].serv_chars[id].user_desc.len = 0;
		 hid_inst[servinst].serv_chars[id].user_desc.handle = 0;
		 hid_inst[servinst].serv_chars[id].user_desc.permissions = AT_BLE_ATTR_NO_PERMISSIONS;
		 
		 /*Configure HID Boot Keyboard Input Report Characteristic : presentation format related info */
		 hid_inst[servinst].serv_chars[id].presentation_format = NULL;
		 
		 /*Configure HID Boot Keyboard Input Report Characteristic : client config descriptor related info */
		 hid_inst[servinst].serv_chars[id].client_config_desc.perm = AT_BLE_ATTR_NO_PERMISSIONS;
		 hid_inst[servinst].serv_chars[id].client_config_desc.handle = 0;
		 
		 /*Configure HID Boot Keyboard Input Report Characteristic : server config descriptor related info */
		 hid_inst[servinst].serv_chars[id].server_config_desc.perm = AT_BLE_ATTR_NO_PERMISSIONS;
		 hid_inst[servinst].serv_chars[id].server_config_desc.handle = 0;
		 
		 /*Configure HID Boot Keyboard Input Report Characteristic : generic descriptor related info */
		 hid_inst[servinst].serv_chars[id].additional_desc_list = NULL;
		 
		 /*Configure HID Boot Keyboard Input Report Characteristic : count of generic descriptor */
		 hid_inst[servinst].serv_chars[id].additional_desc_count = 0;
		 
		 /*Map the keyboard input report characteristic*/
		 hid_serv_inst[servinst].hid_dev_boot_keyboard_in_report = &hid_inst[servinst].serv_chars[id];
		 
		 id++;
#endif
	}
 
    /*Configure HID Information Characteristic : Value related info*/
    hid_inst[servinst].serv_chars[id].char_val.handle = 0;
    hid_inst[servinst].serv_chars[id].char_val.uuid.type = AT_BLE_UUID_16;
    hid_inst[servinst].serv_chars[id].char_val.uuid.uuid[0] =(uint8_t) HID_UUID_CHAR_HID_INFORMATION;
    hid_inst[servinst].serv_chars[id].char_val.uuid.uuid[1] = (uint8_t)(HID_UUID_CHAR_HID_INFORMATION >> 8);
    hid_inst[servinst].serv_chars[id].char_val.init_value = (uint8_t*)info;
    hid_inst[servinst].serv_chars[id].char_val.len = sizeof(hid_info_t);
    hid_inst[servinst].serv_chars[id].char_val.properties = AT_BLE_CHAR_READ;
	
	/* Configure the HID characteristic value permission */
	if(BLE_PAIR_ENABLE){
		hid_inst[servinst].serv_chars[id].char_val.permissions = AT_BLE_ATTR_READABLE_REQ_AUTHN_NO_AUTHR;
		}else{
		hid_inst[servinst].serv_chars[id].char_val.permissions = AT_BLE_ATTR_READABLE_NO_AUTHN_NO_AUTHR;
	}

    /*Configure HID Information Characteristic : user descriptor related info */
    hid_inst[servinst].serv_chars[id].user_desc.user_description = NULL;
    hid_inst[servinst].serv_chars[id].user_desc.len = 0;
    hid_inst[servinst].serv_chars[id].user_desc.handle = 0;
    hid_inst[servinst].serv_chars[id].user_desc.permissions = AT_BLE_ATTR_NO_PERMISSIONS;
    
    /*Configure HID Information Characteristic : presentation format related info */
    hid_inst[servinst].serv_chars[id].presentation_format = NULL;
    
    /*Configure HID Information Characteristic : client config descriptor related info */
    hid_inst[servinst].serv_chars[id].client_config_desc.perm = AT_BLE_ATTR_NO_PERMISSIONS;
    hid_inst[servinst].serv_chars[id].client_config_desc.handle = 0;
    
    /*Configure HID Information Characteristic : server config descriptor related info */
    hid_inst[servinst].serv_chars[id].server_config_desc.perm = AT_BLE_ATTR_NO_PERMISSIONS;
    hid_inst[servinst].serv_chars[id].server_config_desc.handle = 0;
    
    /*Configure HID Information Characteristic : generic descriptor related info */
    hid_inst[servinst].serv_chars[id].additional_desc_list = NULL;
    
    /*Configure HID Information Characteristic : count of generic descriptor */
    hid_inst[servinst].serv_chars[id].additional_desc_count = 0;
	
	/*Map the HID Information characteristic*/
	hid_serv_inst[servinst].hid_dev_info = &hid_inst[servinst].serv_chars[id];
	
	id++;
	
	/*Configure HID Control Point Characteristic : Value related info*/
	hid_inst[servinst].serv_chars[id].char_val.handle = 0;
	hid_inst[servinst].serv_chars[id].char_val.uuid.type = AT_BLE_UUID_16;
	hid_inst[servinst].serv_chars[id].char_val.uuid.uuid[0] =(uint8_t) HID_UUID_CHAR_HID_CONTROL_POINT;
	hid_inst[servinst].serv_chars[id].char_val.uuid.uuid[1] = (uint8_t)(HID_UUID_CHAR_HID_CONTROL_POINT >> 8);
	hid_inst[servinst].serv_chars[id].char_val.init_value = (uint8_t*)&ctrl_point;
	hid_inst[servinst].serv_chars[id].char_val.len = sizeof(uint8_t);
	hid_inst[servinst].serv_chars[id].char_val.properties = AT_BLE_CHAR_WRITE_WITHOUT_RESPONSE;
	
	/* Configure the HID characteristic value permission */
	hid_inst[servinst].serv_chars[id].char_val.permissions = AT_BLE_ATTR_WRITABLE_NO_AUTHN_NO_AUTHR;

	/*Configure HID Control Point Characteristic : user descriptor related info */
	hid_inst[servinst].serv_chars[id].user_desc.user_description = NULL;
	hid_inst[servinst].serv_chars[id].user_desc.len = 0;
	hid_inst[servinst].serv_chars[id].user_desc.handle = 0;
	hid_inst[servinst].serv_chars[id].user_desc.permissions = AT_BLE_ATTR_NO_PERMISSIONS;
	
	/*Configure HID Control Point Characteristic : presentation format related info */
	hid_inst[servinst].serv_chars[id].presentation_format = NULL;
	
	/*Configure HID Control Point Characteristic : client config descriptor related info */
	hid_inst[servinst].serv_chars[id].client_config_desc.perm = AT_BLE_ATTR_NO_PERMISSIONS;
	hid_inst[servinst].serv_chars[id].client_config_desc.handle = 0;
	
	/*Configure HID Control Point Characteristic : server config descriptor related info */
	hid_inst[servinst].serv_chars[id].server_config_desc.perm = AT_BLE_ATTR_NO_PERMISSIONS;
	hid_inst[servinst].serv_chars[id].server_config_desc.handle = 0;
	
	/*Configure HID Control Point Characteristic : generic descriptor related info */
	hid_inst[servinst].serv_chars[id].additional_desc_list = NULL;
	
	/*Configure HID Control Point Characteristic : count of generic descriptor */
	hid_inst[servinst].serv_chars[id].additional_desc_count = 0;

	/*Map the HID Information characteristic*/
	hid_serv_inst[servinst].hid_control_point = &hid_inst[servinst].serv_chars[id];
	
	/* Configure HID characteristic list */
	hid_inst[servinst].serv.char_list =  hid_inst[servinst].serv_chars;
	
	/* Configure the HID characteristic count */
	hid_inst[servinst].serv.char_count = HID_CHARACTERISTIC_NUM;
	
}

/**
* \Function update the report descriptor for HID device
*/
void hid_serv_report_map(uint8_t servinst, uint8_t *report_info, uint16_t len)
{
	hid_inst[servinst].serv_chars[1].char_val.init_value = report_info;
	hid_inst[servinst].serv_chars[1].char_val.len = len;
	hid_serv_inst[servinst].hid_dev_report_map_char->char_val.init_value = report_info;
	hid_serv_inst[servinst].hid_dev_report_map_char->char_val.len = len;
}

/**
* \Function register the HID service.
*/
uint16_t hid_service_dbreg(uint8_t inst, uint8_t *report_type, uint8_t *report_id, uint8_t report_num)
{
	
	uint8_t id = 0;
	uint8_t descval[2] = {0, 0};
	at_ble_status_t status;
	delay_ms(1);	
	if( at_ble_service_define(&hid_inst[inst].serv) == AT_BLE_SUCCESS){
		DBG_LOG_DEV("HID Service :: hid_service_dbreg :: HID Service Handle Value %d", hid_inst[inst].serv.handle);
		for(; id<report_num; id++){
			descval[0] = report_id[id];
			descval[1] = report_type[id];
			if((status = at_ble_descriptor_value_set(hid_serv_inst[inst].hid_dev_report_val_char[id]->additional_desc_list->handle, &descval[0], 2)) == AT_BLE_SUCCESS){
				DBG_LOG_DEV("HID Service :: hid_service_dbreg :: Report Reference Descriptor Value Set");
			}else{
				DBG_LOG("HID Service :: hid_service_dbreg :: Report Reference Descriptor Value Set Fail Reason %d",status);
			}
		}
		return hid_inst[inst].serv.handle;
	}else{
		DBG_LOG_DEV("HID Service :: hid_service_dbreg :: HID Service Regisstration Fail");
		return (uint16_t )HID_SERV_FAILED;
	}
}

/**
* \Function to get service instance.
*/
uint8_t hid_serv_get_instance(uint16_t handle)
{
	uint8_t id = 0;
	DBG_LOG_DEV("HID Service :: hid_serv_get_instance :: Get service Instance Handle %d", handle);
	for(; id<HID_MAX_SERV_INST; id++){
		DBG_LOG_DEV("HID Service :: hid_serv_get_instance :: Service Handle %d  Characteristic Handle %d", *(hid_serv_inst[id].hid_dev_serv_handle), hid_serv_inst[id].hid_control_point->char_val.handle);
		if((handle > *(hid_serv_inst[id].hid_dev_serv_handle)) && (handle <= hid_serv_inst[id].hid_control_point->char_val.handle))
		{
			DBG_LOG_DEV("HID Service :: hid_serv_get_instance :: Service Instance %d", id);
			return id;
		}
	}
	return  0xff;
}

/**
* \Function to get the notification instance.
*/
uint8_t hid_ntf_instance(uint8_t serv_num, uint16_t char_handle)
{
	uint8_t id = 0;
	DBG_LOG_DEV("HID Service :: hid_ntf_instance :: Search Handle %d", char_handle);
	DBG_LOG_DEV("HID Service :: hid_ntf_instance :: Protocol Mode Handle %d", hid_serv_inst[serv_num].hid_dev_proto_mode_char->char_val.handle);
	if(hid_serv_inst[serv_num].hid_dev_proto_mode_char->char_val.handle == char_handle){
		return PROTOCOL_MODE;
	}

	for(; id<HID_NUM_OF_REPORT; id++){
		DBG_LOG_DEV("HID Service :: hid_ntf_instance :: Report Handle %d", hid_serv_inst[serv_num].hid_dev_report_val_char[id]->char_val.handle);
		if(char_handle == hid_serv_inst[serv_num].hid_dev_report_val_char[id]->char_val.handle){
			return CHAR_REPORT;
		}
		DBG_LOG_DEV("HID Service :: hid_ntf_instance :: Report CCD Handle %d",hid_serv_inst[serv_num].hid_dev_report_val_char[id]->client_config_desc.handle);
		if(char_handle == hid_serv_inst[serv_num].hid_dev_report_val_char[id]->client_config_desc.handle){
		   return CHAR_REPORT_CCD;
		}
	}
		
#ifdef HID_KEYBOARD_DEVICE
	DBG_LOG_DEV("HID Service :: hid_ntf_instance :: Boot Keyboard Handle %d", hid_serv_inst[serv_num].hid_dev_boot_keyboard_in_report->char_val.handle);
	if(hid_serv_inst[serv_num].hid_dev_boot_keyboard_in_report->char_val.handle == char_handle){
		return BOOT_KEY_INPUT_REPORT;
	}
#endif

#ifdef HID_MOUSE_DEVICE
	DBG_LOG_DEV("HID Service :: hid_ntf_instance :: Boot Mouse Handle %d", hid_serv_inst[serv_num].hid_dev_boot_mouse_in_report->char_val.handle);
	if(hid_serv_inst[serv_num].hid_dev_boot_mouse_in_report->char_val.handle == char_handle){
		return BOOT_MOUSE_INPUT_REPORT;
	}
#endif

	DBG_LOG_DEV("HID Service :: hid_ntf_instance :: Control Point Handle %d", hid_serv_inst[serv_num].hid_control_point->char_val.handle);
	if(hid_serv_inst[serv_num].hid_control_point->char_val.handle == char_handle){
		return CONTROL_POINT;
	}
	return 0;
}

/**
* \Function to get the report ID.
*/
uint8_t hid_get_reportid(uint8_t serv, uint16_t handle, uint8_t reportnum)
{
	uint8_t status;
	uint8_t id = 0;
	uint8_t descval[2] = {0, 0};
	uint16_t len = 2;	
	
	DBG_LOG_DEV("HID Service :: hid_get_reportid :: Report Number %d", reportnum);
	for(id = 0; id < reportnum; id++){
		DBG_LOG_DEV("HID Service :: hid_get_reportid :: id %d Search Handle %d, CCD Handle %d", id, handle, hid_serv_inst[serv].hid_dev_report_val_char[id]->client_config_desc.handle);
		if(handle == hid_serv_inst[serv].hid_dev_report_val_char[id]->client_config_desc.handle){
			DBG_LOG_DEV("HID Service :: hid_get_reportid :: Report ID Descriptor Handle %d :: id %d :: serv %d", hid_serv_inst[serv].hid_dev_report_val_char[id]->additional_desc_list->handle, id, serv);
			status = at_ble_descriptor_value_get(hid_serv_inst[serv].hid_dev_report_val_char[id]->additional_desc_list->handle, &descval[0], &len);
			if (status != AT_BLE_SUCCESS){
				DBG_LOG_DEV("HID Service :: hid_get_reportid :: Decriptor value get failed");
			}else{
				DBG_LOG_DEV("HID Service :: hid_get_reportid :: Descriptor value Report ID %d Type %d", descval[0], descval[1]);	
			}
			return descval[0];
		}
	}
	return HID_INVALID_INST;
}

/**
* \Function to get the report characteristic id.
*/
uint8_t hid_get_reportchar(uint16_t handle, uint8_t serv, uint8_t reportid)
{
	uint8_t id = 0;
	uint8_t descval[2] = {0, 0};
	uint16_t len = 2;
	at_ble_status_t status ;

	DBG_LOG_DEV("HID Service :: hid_get_reportchar :: Handle %d Service Instance %d Report ID %d", handle, serv, reportid);	
	for(id = 0; id < HID_NUM_OF_REPORT; id++){
		DBG_LOG_DEV("HID Service :: hid_get_reportchar :: Report ID Descriptor Handle %d :: id %d :: serv %d", hid_serv_inst[serv].hid_dev_report_val_char[id]->additional_desc_list->handle, id, serv);
		status = at_ble_descriptor_value_get(hid_serv_inst[serv].hid_dev_report_val_char[id]->additional_desc_list->handle, &descval[0], &len);
		if (status != AT_BLE_SUCCESS){
			DBG_LOG_DEV("HID Service :: hid_get_reportchar :: Decriptor value get failed Reason %d", status);
		}else{
			DBG_LOG_DEV("HID Service :: hid_get_reportchar :: Report Value ID %d Type %d", descval[0], descval[1]);	
		}
		if(descval[0] == reportid){
			DBG_LOG_DEV("HID Service :: hid_get_reportchar :: Report Characteristic ID %d", id);
			return id;
		}
	}
    return HID_INVALID_INST;
}

/**
* \Function to update the report.
*/
void hid_serv_report_update(uint16_t conn_handle, uint8_t serv_inst, uint8_t reportid, uint8_t *report, uint16_t len)
{
	uint8_t id;
	uint16_t status = 0;
	
	id = hid_get_reportchar(conn_handle, serv_inst, reportid);
	
	if(id != HID_INVALID_INST)
	{
		DBG_LOG_DEV("HID Service :: hid_serv_report_update :: Send report");
		if((status = at_ble_characteristic_value_set(hid_serv_inst[serv_inst].hid_dev_report_val_char[id]->char_val.handle, report, len))==AT_BLE_SUCCESS){
			DBG_LOG_DEV("HID Service :: hid_serv_report_update :: Notify Value conn_handle %d", conn_handle);
			//Need to check for connection handle
			status = at_ble_notification_send(conn_handle, hid_serv_inst[serv_inst].hid_dev_report_val_char[id]->char_val.handle);
			if (status != AT_BLE_SUCCESS){
				DBG_LOG_DEV("HID Service :: hid_serv_report_update :: Send notification fail Reason %d", status);
			}
			}else{
			DBG_LOG("HID Service :: hid_serv_report_update :: Characteristic Value Set Failed Reason %d", status);
		}	
	}
}

/**
* \Function to update the boot mouse report.
*/
void hid_boot_mousereport_update(at_ble_handle_t conn_handle, uint8_t serv_inst, uint8_t *bootreport, uint16_t len)
{
#ifdef HID_MOUSE_DEVICE	
	uint8_t value = 0;
	uint8_t status;
	uint16_t length = 2;
	status = at_ble_characteristic_value_get(hid_serv_inst[serv_inst].hid_dev_boot_mouse_in_report->client_config_desc.handle, &value, &length);
	if (status != AT_BLE_SUCCESS){
		DBG_LOG_DEV("HID Service :: hid_boot_mousereport_update :: Descriptor value get fail Reason %d", status);
	}
	//If Notification Enabled
	if(value == 1){
		status = at_ble_characteristic_value_set(hid_serv_inst[serv_inst].hid_dev_boot_mouse_in_report->char_val.handle, bootreport, len);
		if (status != AT_BLE_SUCCESS){
			DBG_LOG_DEV("HID Service :: hid_boot_mousereport_update :: Characteristic value get fail Reason %d", status);
		}
		//Need to check for connection handle
		status = at_ble_notification_send(conn_handle, hid_serv_inst[serv_inst].hid_dev_boot_mouse_in_report->char_val.handle);
		if (status != AT_BLE_SUCCESS){
			DBG_LOG_DEV("HID Service :: hid_boot_mousereport_update :: Fail to send notification Reason %d", status);
		}
	}
#endif	
	ALL_UNUSED(len);
    ALL_UNUSED(bootreport);
    ALL_UNUSED(serv_inst);
    ALL_UNUSED(conn_handle);
}

/**
* \Function to update the boot keyboard report.
*/
void hid_boot_keyboardreport_update(at_ble_handle_t conn_handle, uint8_t serv_inst, uint8_t *bootreport, uint16_t len)
{
#ifdef HID_KEYBOARD_DEVICE	
	at_ble_status_t status;
	uint8_t value = 0;
	uint16_t length = 2;
	status = at_ble_characteristic_value_get(hid_serv_inst[serv_inst].hid_dev_boot_keyboard_in_report->client_config_desc.handle, &value, &length);
	if (status != AT_BLE_SUCCESS){
		DBG_LOG_DEV("HID Service :: hid_boot_keyboardreport_update :: Characteristic value get fail Reason %d", status); 
	}
	//If Notification Enabled
	if(value == 1){
		status = at_ble_characteristic_value_set(hid_serv_inst[serv_inst].hid_dev_boot_keyboard_in_report->char_val.handle, bootreport, len);
		if (status != AT_BLE_SUCCESS){
			DBG_LOG_DEV("HID Service :: hid_boot_keyboardreport_update :: Characteristic value set fail Reason %d", status);
		}
		//Need to check for connection handle
		status = at_ble_notification_send(conn_handle, hid_serv_inst[serv_inst].hid_dev_boot_keyboard_in_report->char_val.handle);
		if (status != AT_BLE_SUCCESS){
			DBG_LOG_DEV("HID Service :: hid_boot_keyboardreport_update :: Notification send failed %d", status);
		}
	}
#endif	
    ALL_UNUSED(len);
    ALL_UNUSED(bootreport);
    ALL_UNUSED(serv_inst);
    ALL_UNUSED(conn_handle);
}



