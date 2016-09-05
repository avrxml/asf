/**
 * \file
 *
 * \brief Health Thermometer Profile Application
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

 /**
 * \mainpage
 * \section preface Preface
 * This is the reference manual for the Health Thermometer Profile Application
 */
/*- Includes ---------------------------------------------------------------*/

#include <asf.h>
#include "profiles.h"
#include "platform.h"
#include "at_ble_api.h"
#include "htpt_app.h"
#include "console_serial.h"
#include "device_info.h"
#include "timer_hw.h"
#include "conf_extint.h"
#include "conf_serialdrv.h"
#include "at_ble_trace.h"
#include "ble_utils.h"
#include "ble_manager.h"


#ifndef BLE_DEVICE_NAME
/* BLE Device Name definitions */
#define BLE_DEVICE_NAME				"ATMEL-HTP"
#endif

/** @brief device information service handler **/
dis_gatt_service_handler_t dis_service_handler;

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
	app_pair_done_event_handler,
	NULL,
	NULL,
	NULL,
	NULL,
    app_encryption_status_changed_handler,
	NULL,
	NULL,
	NULL,
	NULL
};

static const ble_event_callback_t app_htpt_handle[] = {
	app_htpt_create_db_cfm_handler,
	app_htpt_error_ind_handler,
	app_htpt_disable_ind_handler,
	app_htpt_temp_send_cfm,
	app_htpt_meas_intv_chg_ind_handler,
	app_htpt_cfg_indntf_ind_handler,
	app_htpt_enable_rsp_handler,
	app_htpt_meas_intv_upd_rsp,
	app_htptp_meas_intv_chg_req
};

/* Initialize the BLE */
static void ble_init(void);

/* LED Status to user */



/* BLE data send event confirmation */
static void ble_data_sent_confim(void);

/* Update temperature type and location */
static void update_temperature_type_location(void);

void htpt_set_advertisement_data(void);


void ble_device_config(at_ble_addr_t *addr);

static uint8_t scan_rsp_data[SCAN_RESP_LEN] = {0x09, 0xFF, 0x00, 0x06, 0xd6, 0xb2, 0xf0, 0x05, 0xf0, 0xf8};

at_ble_LTK_t app_bond_info;
bool app_device_bond = false;
at_ble_auth_t auth_info;

htp_app_t htp_data;

volatile bool app_timer_done = false;
volatile bool button_pressed = false;

at_ble_handle_t htpt_conn_handle;

bool temp_send_notification = false;

/* BLE data send event confirmation */
static void ble_data_sent_confim(void)
{
	/* Toggle the application LED of each data sent */
	LED_Toggle(LED0);
}

/* Initialize the BLE */
static void ble_init(void)
{	
	/* initialize the ble chip  and Set the device mac address */
	ble_device_init(NULL);	
}

/* Initializing the default values for temperature and htp parameters*/
static void htp_init_defaults(htp_app_t *htp_temp)
{
	/* Initialize to default temperature value  and htp parameters*/
	htp_temp->measurement_interval = 1;
	htp_temp->temperature = 3700;
	htp_temp->temperature_type = HTP_TYPE_ARMPIT;
	htp_temp->max_meaurement_intv = 30;
	htp_temp->min_measurement_intv = 1;
	htp_temp->security_lvl = HTPT_AUTH;
	htp_temp->optional = HTPT_TEMP_TYPE_CHAR_SUP;
	htp_temp->flags = (at_ble_htpt_temp_flags)(HTPT_FLAG_CELSIUS | HTPT_FLAG_TYPE);
}

/* Sending the temperature value after reading it from IO1 Xplained Pro */
static void htp_temperature_send(htp_app_t *htp_temp)
{
	at_ble_prf_date_time_t timestamp;
	#if SAMD21 || SAML21
	float temperature;
	/* Read Temperature Value from IO1 Xplained Pro */
	temperature = at30tse_read_temperature();
	
	#endif

	#if SAMG55 || SAM4S
	double temperature;
	/* Read Temperature Value from IO1 Xplained Pro */
	at30tse_read_temperature(&temperature);
	#endif

	if(button_pressed)
	{
		update_temperature_type_location();
		button_pressed = false;
	}

	if (htp_temp->flags & HTPT_FLAG_FAHRENHEIT)
	{
		temperature = (((temperature * 9.0)/5.0) + 32.0);
	}
	
	timestamp.day = 1;
	timestamp.hour = 9;
	timestamp.min = 2;
	timestamp.month = 8;
	timestamp.sec = 36;
	timestamp.year = 15;
	
	if(at_ble_htpt_temp_send(convert_ieee754_ieee11073_float((float)temperature),
	&timestamp,
	htp_temp->flags,
	htp_temp->temperature_type,
	STABLE_TEMPERATURE_VAL
	) == AT_BLE_SUCCESS)
	{
		if (htp_temp->flags & HTPT_FLAG_FAHRENHEIT)
		{
			DBG_LOG("Temperature: %d Fahrenheit", (uint16_t)temperature);
		}
		else
		{
			DBG_LOG("Temperature: %d Deg Celsius", (uint16_t)temperature);
		}
		
	}
}


/**
 * @brief app_connected_state blemanager notifies the application about state
 * @param[in] at_ble_connected_t
 */
static at_ble_status_t app_connected_event_handler(void *params)
{
	at_ble_connected_t conn_params;
	memcpy((uint8_t *)&conn_params, params, sizeof(at_ble_connected_t));
	
	DBG_LOG("Device connected to 0x%02x%02x%02x%02x%02x%02x handle=0x%x",
	conn_params.peer_addr.addr[5],
	conn_params.peer_addr.addr[4],
	conn_params.peer_addr.addr[3],
	conn_params.peer_addr.addr[2],
	conn_params.peer_addr.addr[1],
	conn_params.peer_addr.addr[0],
	conn_params.handle);
	
	/* Switch on the application LED */
	LED_On(LED0);
	
	/* Enable the HTP Profile */
	if(at_ble_htpt_enable(conn_params.handle, HTPT_CFG_STABLE_MEAS_IND) != AT_BLE_SUCCESS)
	{
		DBG_LOG("Failure in HTP Profile Enable");
	}
	return AT_BLE_SUCCESS;
}

/**
 * @brief app_connected_state ble manager notifies the application about state
 * @param[in] connected
 */
static at_ble_status_t app_disconnected_event_handler(void *params)
{
		at_ble_disconnected_t disconnect;
		memcpy((uint8_t *)&disconnect, params, sizeof(at_ble_disconnected_t));
		
		/* Stop the hardware timer */
		hw_timer_stop();
		
		/* BLE device disconnected, indicate to user */
		/* Switch off the application LED */
		LED_Off(LED0);
		
		temp_send_notification = false;
		
		DBG_LOG("Device disconnected Reason:0x%02x Handle=0x%x", disconnect.reason, disconnect.handle);
		
		if(at_ble_adv_start(AT_BLE_ADV_TYPE_UNDIRECTED, AT_BLE_ADV_GEN_DISCOVERABLE, NULL, AT_BLE_ADV_FP_ANY,
		APP_HT_FAST_ADV, APP_HT_ADV_TIMEOUT, 0) != AT_BLE_SUCCESS)
		{
			DBG_LOG("BLE advertisement start failed");
		}
		else
		{
			DBG_LOG("BLE started LE advertisement");
		}
	
		return AT_BLE_SUCCESS;
}

static at_ble_status_t app_pair_done_event_handler(void *params)
{
	/* Pair done */
	at_ble_pair_done_t pair_params;
	at_ble_handle_t handle = 0;
	
	memcpy((uint8_t *)&pair_params, params, sizeof(at_ble_pair_done_t));
	if(pair_params.status == AT_BLE_SUCCESS)
	{
		DBG_LOG_DEV("Pairing procedure completed successfully");
		app_device_bond = true;
		auth_info = pair_params.auth;
		
		handle = pair_params.handle;
		
		/* Enable the HTP Profile */
		if(at_ble_htpt_enable(handle, HTPT_CFG_STABLE_MEAS_IND) == AT_BLE_FAILURE)
		{
			DBG_LOG("Failure in HTP Profile Enable");
		}
	}
	else
	{
		DBG_LOG("Pairing failed");
		/* Stop the hardware timer */
		hw_timer_stop();
		
		LED_Off(LED0);
		
		at_ble_disconnect(handle, AT_BLE_TERMINATED_BY_USER);
	}
	return AT_BLE_SUCCESS;
}

static at_ble_status_t app_encryption_status_changed_handler(void *params)
{
	at_ble_encryption_status_changed_t enc_status;
	at_ble_handle_t handle = 0;
	memcpy((uint8_t *)&enc_status, params, sizeof(at_ble_encryption_status_changed_t));
	if(enc_status.status == AT_BLE_SUCCESS)
	{
		DBG_LOG("Encryption completed successfully");
		
		handle = enc_status.handle;
		
		/* Enable the HTP Profile */
		if(at_ble_htpt_enable(handle, HTPT_CFG_STABLE_MEAS_IND) == AT_BLE_FAILURE)
		{
			DBG_LOG("Failure in HTP Profile Enable");
		}
	}
	else
	{
		DBG_LOG("Encryption failed");
	}
	return AT_BLE_SUCCESS;
}

static at_ble_status_t  app_htpt_create_db_cfm_handler(void *params) 
{
	at_ble_htpt_create_db_cfm_t create_db_params;
	
	memcpy((uint8_t *)&create_db_params, params, sizeof(at_ble_htpt_create_db_cfm_t));
	DBG_LOG("Creating HTP DB: SUCCESS: Status=0x%x", create_db_params.status);
	
	return AT_BLE_SUCCESS;	
}

static at_ble_status_t  app_htpt_error_ind_handler(void *params)
{
	prf_server_error_ind_t prf_htpt_error_ind;
	memcpy((uint8_t *)&prf_htpt_error_ind, params, sizeof(prf_server_error_ind_t));
	
	DBG_LOG("HTP Error Indication received, msg_id=0x%x, handle=0x%x, status=0x%x",
	prf_htpt_error_ind.msg_id, prf_htpt_error_ind.conhdl, prf_htpt_error_ind.status);
	
	return AT_BLE_SUCCESS;
}

static at_ble_status_t  app_htpt_disable_ind_handler(void *params)
{
	at_ble_htpt_disable_ind_t htpt_disable_ind_params;
	memcpy((uint8_t *)&htpt_disable_ind_params, params, sizeof(at_ble_htpt_disable_ind_t));
	DBG_LOG("HTP Disable Indication: conhdl=0x%x, interm_temp_ntf_en=0x%x, meas_intv=0x%x, meas_intv_ind_en=0x%x, handle=0x%x",
	htpt_disable_ind_params.conhdl,
	htpt_disable_ind_params.interm_temp_ntf_en,
	htpt_disable_ind_params.meas_intv,
	htpt_disable_ind_params.meas_intv_ind_en,
	htpt_disable_ind_params.temp_meas_ind_en);
	return AT_BLE_SUCCESS;
}

static at_ble_status_t  app_htpt_temp_send_cfm(void *params)
{
	at_ble_htpt_temp_send_cfm_t htpt_send_temp_cfm_params;
	memcpy((uint8_t *)&htpt_send_temp_cfm_params, params, sizeof(at_ble_htpt_temp_send_cfm_t));
	
	/* Temperature sent confirmation */
	ble_data_sent_confim();
	
	/* start the timer for next interval of  temperature send */
	hw_timer_start(htp_data.measurement_interval);
	while(app_timer_done == false);
	if (temp_send_notification)
	{
		htp_temperature_send(&htp_data);
	}
	app_timer_done = false;
	return AT_BLE_SUCCESS;
}

static at_ble_status_t  app_htpt_meas_intv_chg_ind_handler(void *params)
{
	at_ble_htpt_meas_intv_chg_ind_t htpt_meas_intv_chg_params;
	memcpy((uint8_t *)&htpt_meas_intv_chg_params, params, sizeof(at_ble_htpt_meas_intv_chg_ind_t));
	DBG_LOG("HTP measure Interval change Indication: Interval=%d", htpt_meas_intv_chg_params.intv);
	htp_data.measurement_interval = htpt_meas_intv_chg_params.intv;
	return AT_BLE_SUCCESS;
}

static at_ble_status_t  app_htpt_cfg_indntf_ind_handler(void *params)
{
	at_ble_htpt_cfg_indntf_ind_t htpt_cfg_indntf_ind_params;
	memcpy((uint8_t *)&htpt_cfg_indntf_ind_params, params, sizeof(at_ble_htpt_cfg_indntf_ind_t));
	if (htpt_cfg_indntf_ind_params.ntf_ind_cfg == 1)
	{
		DBG_LOG("Started HTP Temperature Indications");
		temp_send_notification = true;
		htp_temperature_send(&htp_data);
	}
	else
	{
		temp_send_notification = false;
		DBG_LOG("HTP Temperature Notification Stopped");
	}
	return AT_BLE_SUCCESS;
}

static at_ble_status_t  app_htpt_enable_rsp_handler(void *params)
{
	at_ble_htpt_enable_rsp_t at_ble_htpt_enable_rsp;
	memcpy((uint8_t *)&at_ble_htpt_enable_rsp, params, sizeof(at_ble_htpt_enable_rsp_t));
	if (!(at_ble_htpt_enable_rsp.status == AT_BLE_SUCCESS))
	{
		DBG_LOG("HTPT Enable Failed");
	}
	ALL_UNUSED(params);
	return AT_BLE_SUCCESS;
}

static at_ble_status_t  app_htpt_meas_intv_upd_rsp(void *params)
{
	ALL_UNUSED(params);
	return AT_BLE_SUCCESS;
}

static at_ble_status_t  app_htptp_meas_intv_chg_req(void *params)
{
	ALL_UNUSED(params);
	return AT_BLE_SUCCESS;
}

/* Creating database and advertisement data set and advertisement start */
static void htp_init(void)
{
	at_ble_status_t status;	
	/* Initialize the htp_data to default value */
	htp_init_defaults(&htp_data);
	
	/* Register the Initialized value into htp profile */
	if(at_ble_htpt_create_db(
							htp_data.optional,
							htp_data.temperature_type,
							htp_data.min_measurement_intv,
							htp_data.max_meaurement_intv,
							htp_data.measurement_interval,
							htp_data.security_lvl,
							&htpt_conn_handle) != AT_BLE_SUCCESS)
	{
		/* Failed to create HTP data base */
		DBG_LOG("HTP Data Base creation failed");
		while(1);
	}
	
	/* Initializing the DIS service in the host */
	dis_init_service(&dis_service_handler);
	
	if ((status = dis_primary_service_define(&dis_service_handler)) !=
	AT_BLE_SUCCESS) {
		DBG_LOG("Dis Service definition failed,reason: %x", status);
	} 
		
	htpt_set_advertisement_data();
}

/* Setting the Device address */
void ble_device_config(at_ble_addr_t *addr)
{
	at_ble_dev_config_t stDevConfig;
	at_ble_addr_t address = {AT_BLE_ADDRESS_PUBLIC, {0xAB, 0xCD, 0xEF, 0xAB, 0xCD, 0xEF}};
	at_ble_addr_t *address_ptr = addr;
	at_ble_status_t enuStatus;
	char *dev_name = NULL;

	dev_name = (char *)BLE_DEVICE_NAME;
	
	
	if (addr == NULL)
	{
		/* get BD address from BLE device */
		if(at_ble_addr_get(&address) != AT_BLE_SUCCESS)
		{
			DBG_LOG("BD address get failed");
		}
		address_ptr = &address;
	}
	
	/* Update the Address in scan response data*/
	memcpy(&scan_rsp_data[4], address_ptr->addr, 6);
	
	/* set the BD address */
	if(at_ble_addr_set(address_ptr) != AT_BLE_SUCCESS)
	{
		DBG_LOG("BD address set failed");
	}
		
	//Set device configuration
	////Device role
	stDevConfig.role = AT_BLE_ROLE_ALL;
	////device renew duration
	stDevConfig.renew_dur = AT_RENEW_DUR_VAL_MIN;
	////device address type
	stDevConfig.address = *address_ptr;
	////Attributes
	stDevConfig.att_cfg.b2NamePerm = AT_BLE_WRITE_DISABLE;
	stDevConfig.att_cfg.b2AppearancePerm = AT_BLE_WRITE_DISABLE;
	stDevConfig.att_cfg.b1EnableSpcs = 0;
	stDevConfig.att_cfg.b1EnableServiceChanged = 0;
	stDevConfig.att_cfg.b2Rfu = AT_BLE_WRITE_DISABLE;
	////Handles
	stDevConfig.gap_start_hdl = AT_BLE_AUTO_ALLOC_HANDLE;
	stDevConfig.gatt_start_hdl = AT_BLE_AUTO_ALLOC_HANDLE;
	////MTU
	stDevConfig.max_mtu = AT_MTU_VAL_RECOMMENDED;
	
	enuStatus = at_ble_set_dev_config(&stDevConfig);

    if(enuStatus != AT_BLE_SUCCESS)
    {
      DBG_LOG("BLE Device Config Failed");
    }
    else
    {
      if(at_ble_device_name_set((uint8_t *)dev_name, strlen(dev_name)) != AT_BLE_SUCCESS)
      {
        DBG_LOG("BLE Device name set failed");
      }
    }   
        
	UNUSED(enuStatus);
}

/* Advertisement data set and advertisement start */
void htpt_set_advertisement_data(void)
{
	at_ble_status_t status;
	status = ble_advertisement_data_set();
	
	if (status != AT_BLE_SUCCESS) {
		DBG_LOG("Advertisement set failed reason %d",status);
	}
	
	if(at_ble_adv_start(AT_BLE_ADV_TYPE_UNDIRECTED, AT_BLE_ADV_GEN_DISCOVERABLE, NULL, AT_BLE_ADV_FP_ANY, 
	                   APP_HT_FAST_ADV, APP_HT_ADV_TIMEOUT, 0) != AT_BLE_SUCCESS)
	{
		DBG_LOG("BLE advertisement start failed");
	}
	else
	{
		DBG_LOG("BLE started LE advertisement");
	}							
}

void button_cb(void)
{
	button_pressed = true;
}

/* Updating the location to read the temperature */
static void update_temperature_type_location(void)
{
	htp_data.temperature_type = (at_ble_htpt_temp_type)((htp_data.temperature_type+1) % 9);
	if ((htp_data.temperature_type == HTP_TYPE_ARMPIT) && (htp_data.flags == (HTPT_FLAG_CELSIUS | HTPT_FLAG_TYPE)))
	{
		htp_data.flags = (at_ble_htpt_temp_flags)(HTPT_FLAG_FAHRENHEIT | HTPT_FLAG_TYPE);
	}
	else if (htp_data.temperature_type == HTP_TYPE_ARMPIT)
	{
		htp_data.flags = (at_ble_htpt_temp_flags)(HTPT_FLAG_CELSIUS | HTPT_FLAG_TYPE);
	}
}
/* Timer callback */
static void timer_callback_handler(void)
{
	hw_timer_stop();
	app_timer_done = true;	
}

uint8_t app_exec = true;

int main (void)
{
	#if SAMG55 || SAM4S
		/* Initialize the SAM system. */
		sysclk_init();
		board_init();
	#elif SAM0
		system_init();
	#endif
	
	/* Initialize the button */
	button_init();
	
	/* Initialize serial console */
	serial_console_init();
	
	DBG_LOG("Initializing HTP Application");
	
	/* Initialize the temperature sensor */
	at30tse_init();
	
	/* configure the temperature sensor ADC */
	at30tse_write_config_register(
			AT30TSE_CONFIG_RES(AT30TSE_CONFIG_RES_12_bit));	
	
	/* Initialize the hardware timer */
	hw_timer_init();
	
	/* Register the callback */
	hw_timer_register_callback(timer_callback_handler);	
	
	/* initialize the ble chip */
	ble_init();	
	
	ble_device_config(NULL);
	
	/* Initialize the htp profile */
	htp_init();	
	
	DBG_LOG("HTP Initialization completed. Waiting for Event");
	
	DBG_LOG_PTS("The Temperature type is %d",htp_data.temperature_type);
	
	/* Registering the call backs for events with the ble manager */
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
	BLE_GAP_EVENT_TYPE,
	app_gap_handle);

	/* Registering the call backs for events with the ble manager */
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
	BLE_GATT_HTPT_EVENT_TYPE,
	app_htpt_handle);
		
	while(app_exec) {
		/* BLE Event Task */
		ble_event_task();
	}

	return true;
}
