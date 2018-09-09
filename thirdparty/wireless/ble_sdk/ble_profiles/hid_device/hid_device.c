/**
 * \file
 *
 * \brief HID Device Profile
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
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Atmel
 *Support</a>
 */

/**
 * \mainpage
 * \section preface Preface
 * This is the reference manual for the HID Device Profile
 */
/*- Includes ---------------------------------------------------------------*/
#include <asf.h>

#include "string.h"
#include "ble_utils.h"
#include "ble_manager.h"
#include "hid_device.h"
#include "hid.h"
#include "device_info.h"

static const ble_gap_event_cb_t hid_gap_handle = {
	.disconnected = hid_prf_disconnect_event_handler
};

static const ble_gatt_server_event_cb_t hid_gatt_server_handle = {
	.characteristic_changed = hid_prf_char_changed_handler
};

/* Notification callback function pointer */
report_ntf_callback_t report_ntf_cb;
boot_ntf_callback_t boot_ntf_cb;
protocol_mode_ntf_callback_t mode_ntf_cb;
control_point_ntf_callback_t control_point_ntf_cb;

/* Pointer to profile data reference */
hid_prf_info_t *hid_prf_dataref[HID_MAX_SERV_INST];

/* Scan response data*/
uint8_t scan_rsp_data[SCAN_RESP_LEN] = {0x09, 0xff, 0x00, 0x06, 0xd6, 0xb2, 0xf0, 0x05, 0xf0, 0xf8};

/**
* \HID device profile initialization function
*/
void hid_prf_init(void *param)
{   
	uint8_t serv_num = 0;
	uint16_t serv_handle = 0;
	dis_gatt_service_handler_t device_info_serv;
	
	#ifdef ENABLE_PTS
		DBG_LOG_PTS("Protocol Mode Characteristic Value 0x%02X", hid_prf_dataref[serv_num]->protocol_mode);
	#endif
	for(serv_num = 0; serv_num<HID_MAX_SERV_INST; serv_num++)
	{
		if(hid_prf_dataref[serv_num] != NULL)
		{
			hid_serv_def_init(serv_num);
			hid_serv_init(serv_num, hid_prf_dataref[serv_num]->hid_device, &hid_prf_dataref[serv_num]->protocol_mode, hid_prf_dataref[serv_num]->num_of_report, (uint8_t *)&hid_prf_dataref[serv_num]->report_type, &(hid_prf_dataref[serv_num]->report_val[0]), (uint8_t *)&hid_prf_dataref[serv_num]->report_len, &hid_prf_dataref[serv_num]->hid_device_info);
			hid_serv_report_map(serv_num, hid_prf_dataref[serv_num]->report_map_info.report_map, hid_prf_dataref[serv_num]->report_map_info.report_map_len);
				
			/* HID services database registration */
			serv_handle = hid_service_dbreg(serv_num, (uint8_t *)&hid_prf_dataref[serv_num]->report_type, (uint8_t *)&hid_prf_dataref[serv_num]->report_id, hid_prf_dataref[serv_num]->num_of_report);
			
			DBG_LOG_DEV("HID Service Handle %d", serv_handle);
			if(serv_handle)
			{
				hid_prf_dataref[serv_num]->serv_handle_info = serv_handle;
			}
			else
			{
				hid_prf_dataref[serv_num]->serv_handle_info = 0;
			}
		}
		else
		{
			hid_serv_def_init(serv_num);
		}
	}
	//delay_ms(1);
	/* Initialize the dis */
	dis_init_service(&device_info_serv);
	
	/* Define the primary service in the GATT server database */
	dis_primary_service_define(&device_info_serv);
	
	/* HID Profile Advertisement*/
	hid_prf_dev_adv();
	
	/* Callback registering for BLE-GAP Role */
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK, BLE_GAP_EVENT_TYPE, &hid_gap_handle);
	
	/* Callback registering for BLE-GATT-Server Role */
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK, BLE_GATT_SERVER_EVENT_TYPE, &hid_gatt_server_handle);
	
	UNUSED(param);
}

/**
* \HID device profile configuration function
*/
uint8_t hid_prf_conf(hid_prf_info_t *ref)
{
	if(ref != NULL){
		if(ref->hid_serv_instance && (ref->hid_serv_instance <= HID_MAX_SERV_INST)){
			if(hid_prf_dataref[ref->hid_serv_instance-1] != NULL){
				hid_prf_dataref[ref->hid_serv_instance-1] = NULL;
			}
			hid_prf_dataref[ref->hid_serv_instance-1] = ref;
			return HID_PRF_SUCESS;
		}else{
			return HID_PRF_INSTANCE_OUT_RANGE;
		}
	}else{
		return HID_PRF_NO_INSTANCE;
	}
}


/**
* \HID device advertisement handler function
*/
void hid_prf_dev_adv(void)
{
	/*  Set advertisement data from ble_manager*/
	if(!(ble_advertisement_data_set() == AT_BLE_SUCCESS))
	{
		DBG_LOG("Fail to set Advertisement data");
	}
	
	/* Start of advertisement */
	if(at_ble_adv_start(AT_BLE_ADV_TYPE_UNDIRECTED, AT_BLE_ADV_GEN_DISCOVERABLE, NULL, AT_BLE_ADV_FP_ANY, APP_HID_FAST_ADV, APP_HID_ADV_TIMEOUT, 0) == AT_BLE_SUCCESS){
		DBG_LOG("Device Started Advertisement");
	}else{
		DBG_LOG("Device Advertisement Failed");
	}
}

/**
* \HID device disconnected handler function
*/
at_ble_status_t hid_prf_disconnect_event_handler(void *params)
{
	if(at_ble_adv_start(AT_BLE_ADV_TYPE_UNDIRECTED, AT_BLE_ADV_GEN_DISCOVERABLE, NULL, AT_BLE_ADV_FP_ANY,
	APP_HID_FAST_ADV, APP_HID_ADV_TIMEOUT, 0) != AT_BLE_SUCCESS){
		DBG_LOG("Device Advertisement Failed");
	}else{
		DBG_LOG("Device Started Advertisement");
	}
	ALL_UNUSED(&params);
	return AT_BLE_SUCCESS;
}

/**
* \Service characteristic change handler function
*/
at_ble_status_t hid_prf_char_changed_handler(void *params)
{
	at_ble_characteristic_changed_t *change_char;
	change_char = (at_ble_characteristic_changed_t *)params;

	hid_proto_mode_ntf_t protocol_mode;
	hid_report_ntf_t reportinfo;
	hid_boot_ntf_t boot_info;
	hid_control_mode_ntf_t control_point;
	at_ble_characteristic_changed_t change_params;
	uint8_t ntf_op;
	uint8_t serv_inst;

	memcpy((uint8_t *)&change_params, change_char, sizeof(at_ble_characteristic_changed_t));
	serv_inst = hid_serv_get_instance(change_params.char_handle);
	DBG_LOG_DEV("hid_prf_char_changed_handler : Service Instance %d", serv_inst);
	ntf_op = hid_ntf_instance(serv_inst, change_params.char_handle);
    DBG_LOG_DEV("hid_prf_char_changed_handler : Notification Operation %d", ntf_op);
   
	switch(ntf_op)
	{
	   case PROTOCOL_MODE:
	   {
		   protocol_mode.serv_inst = serv_inst;
		   protocol_mode.mode = change_params.char_new_value[0];
		   hid_prf_dataref[serv_inst]->protocol_mode = change_params.char_new_value[0];
		   protocol_mode.conn_handle = change_params.conn_handle;
		   mode_ntf_cb(&protocol_mode);
	   }
	   break;
	   
	   case CHAR_REPORT_CCD:
	   {
			reportinfo.serv_inst = serv_inst;
			reportinfo.report_ID = hid_get_reportid(serv_inst, change_params.char_handle, HID_NUM_OF_REPORT);
		  	reportinfo.ntf_conf = (change_params.char_new_value[1]<<8 | change_params.char_new_value[0]);
			reportinfo.conn_handle = change_params.conn_handle;
			report_ntf_cb(&reportinfo);  
	   }
	   break;
		   
	   case (BOOT_MOUSE_INPUT_REPORT||BOOT_KEY_INPUT_REPORT):
	   {
		   boot_info.serv_inst = serv_inst;
		   boot_info.boot_value = ntf_op;
		   boot_info.ntf_conf = (change_params.char_new_value[1]<<8 | change_params.char_new_value[0]);
		   boot_info.conn_handle = change_params.conn_handle;
		   boot_ntf_cb(&boot_info);
	   }
	   break;
	   case CONTROL_POINT:
	   {
		  control_point.serv_inst = serv_inst;
		  control_point.control_value = (change_params.char_new_value[1]<<8 | change_params.char_new_value[0]);
		  control_point_ntf_cb(&control_point);
	   }
	   break;
	   default:
	   {
		   DBG_LOG_DEV("Unhandled Notification");
	   }
	   break;
   }
	return AT_BLE_SUCCESS;
}

/**
* \Notify to user about the new notification configuration set by HID host for report mode
*/
void notify_report_ntf_handler(report_ntf_callback_t report_ntf_fn)
{
	report_ntf_cb = report_ntf_fn;
}

/**
* \Notify to user about the new notification configuration set by HID host for boot mode
*/
void notify_boot_ntf_handler(boot_ntf_callback_t boot_ntf_fn)
{
	boot_ntf_cb = boot_ntf_fn;
}

/**
* \Notify to user about the new protocol mode set by HID host for boot mode
*/
void notify_protocol_mode_handler(protocol_mode_ntf_callback_t proto_mode_ntf_fn)
{
	mode_ntf_cb = proto_mode_ntf_fn;
}

/**
* \Notify to user about the control point set by HID host for boot mode
*/
void notify_control_point_handler(control_point_ntf_callback_t control_mode_ntf_fn)
{
	control_point_ntf_cb = control_mode_ntf_fn;
}

/**
* \Notify report to HID host 
*/
void hid_prf_report_update(at_ble_handle_t conn_handle, uint8_t serv_inst, uint8_t reportid, uint8_t *report, uint16_t len)
{
	hid_serv_report_update(conn_handle, serv_inst, reportid, report, len);
}

/**
* \Notify boot report for keyboard to HID host
*/
void hid_prf_boot_mousereport_update(at_ble_handle_t conn_handle, uint8_t serv_inst, uint8_t *bootreport, uint16_t len)
{
	hid_boot_mousereport_update(conn_handle, serv_inst, bootreport, len);
}

/**
* \Notify boot report for keyboard to HID host
*/
void hid_prf_boot_keyboardreport_update(at_ble_handle_t conn_handle, uint8_t serv_inst, uint8_t *bootreport, uint16_t len)
{
	hid_boot_keyboardreport_update(conn_handle, serv_inst, bootreport, len);
}
