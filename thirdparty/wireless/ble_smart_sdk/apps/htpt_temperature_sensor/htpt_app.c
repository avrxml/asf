/**
 * \file
 *
 * \brief Health Thermometer Profile Application
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
 * \mainpage Health Thermometer application
 * \section Introduction
 * ******************************Introduction ***********************************
 *
 *    The Health thermometer example application bring-up the Health thermometer 
 *    profile defined by the Bluetooth SIG which would enables a Collector device
 *    to connect and interact with a Thermometer sensor to use in healthcare 
 *    applications.
 *
 * - Running the demo -
 *  + 1. Build and flash the binary into supported evaluation board.
 *  + 2. Open the console using TeraTerm or any serial port monitor.
 *  + 3. Press the Reset button.
 *  + 4. Wait for around 10 seconds for the patches to be downloaded device will initialize and start-up.
 *  + 5. The device is now in advertising mode.
 *  + 6. On a BLE compatible iPhone®/Android phone, enable Bluetooth in the Settings 
 *       page. Start the 'Atmel Smart Connect Application' and scan for devices. ATMEL-HTPT will 
 *       be appear among the devices scanned. Click on "ATMEL-HTPT" to connect to 
 *       supported platform.
 *  + 7. Once connected, Cient side will request for pairing procedure . 
 *       The console log provides a guidance for the user to enter the pass-key.
 *  + 8. Once paired, the application displays the Health Thermometer service.
 *  + 9. On clicking the Health Thermometer service, user keeps getting notifications 
 *       for Temperature measurement value, user can disable notifications. 
 *  + 10.Press the SW0 button for different temperature types.
 *
 * \section Modules
 * ***************************** MODULES ***************************************** 
 * - BLE Manager - 
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
 * - BLE Profile-
 *    + The Health Thermometer Profile is used to enable a data collection device
 *      to obtain data from a thermometer sensor that exposes the Health 
 *      Thermometer Service.
 *    + The Health Thermometer profile following service characteristics.
 *     + Temperature measurement
 *     + Temperature type
 *
 * - BLE Platform Services -
 *  +  Serial Console COM port settings -
 *    + Baudrate 115200
 *	  + Parity None, Stop Bit 1, Start Bit 1
 *	  + No Hardware Handshake
 *
 * \section BluSDK Package
 * ***************************** BluSDK Package *****************************************
 * - Links for Docs -
  * 	 + http://www.microchip.com/wwwproducts/en/ATSAMB11
  * 	 + http://www.microchip.com/developmenttools/productdetails.aspx?partno=atsamb11-xpro
  *- Support and FAQ - visit -
  * 	 + <a href="https://www.microchip.com/support/">Microchip Support</a>
/*- Includes ---------------------------------------------------------------*/

#include <asf.h>
#include "profiles.h"
#include "platform.h"
#include "at_ble_api.h"
#include "htpt_app.h"
#include "console_serial.h"
#include "device_info.h"
#include "timer_hw.h"
#include "at_ble_trace.h"
#include "ble_utils.h"
#include "ble_manager.h"
#include "button.h"
#include "led.h"
#include "samb11_delay.h"


#ifndef BLE_DEVICE_NAME
/* BLE Device Name definitions */
#define BLE_DEVICE_NAME				"ATMEL-HTP"
#endif

/* Temperature measurement Locations */
static const char *temp_measurement_loc[] = 
		{"Armpit", "Body", "Ear", "Finger", "Gastro-intestinal Tract",
		 "Mouth", "Rectum","Toe","Tympanum"};

/** @brief device information service handler **/
dis_gatt_service_handler_t dis_service_handler;

static const ble_gap_event_cb_t app_gap_handle = {
	.connected = app_connected_event_handler,
	.disconnected = app_disconnected_event_handler,
	.pair_done = app_pair_done_event_handler,
	.encryption_status_changed = app_encryption_status_changed_handler
};

static const ble_htpt_event_cb_t app_htpt_handle = {
	.htpt_create_db_cfm = app_htpt_create_db_cfm_handler,
	.htpt_error_ind = app_htpt_error_ind_handler,
	.htpt_disable_ind = app_htpt_disable_ind_handler,
	.htpt_temp_send_cfm = app_htpt_temp_send_cfm,
	.htpt_meas_intv_chg_ind = app_htpt_meas_intv_chg_ind_handler,
	.htpt_cfg_indntf_ind = app_htpt_cfg_indntf_ind_handler,
	.htpt_enable_rsp = app_htpt_enable_rsp_handler,
	.htpt_meas_intv_upd_rsp = app_htpt_meas_intv_upd_rsp,
	.htpt_meas_intv_chg_req = app_htptp_meas_intv_chg_req
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
volatile bool app_init_done = false;
extern bool button_debounce;


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
	at_ble_prf_date_time_t timestamp =
	{
		.day = 1,
		.hour = 9,
		.min = 2,
		.month = 6,
		.sec = 36,
		.year = 16
	};
	
	at_ble_status_t status;
	#if SAMD21 || SAML21|| SAMB11
	float temperature;
	/* Read Temperature Value from IO1 Xplained Pro */
	temperature = at30tse_read_temperature();
	
	#endif

	#if SAMG55 || SAM4S
	double temperature;
	/* Read Temperature Value from IO1 Xplained Pro */
	at30tse_read_temperature(&temperature);
	#endif

	if (htp_temp->flags & HTPT_FLAG_FAHRENHEIT)
	{
		temperature = (((temperature * 9.0)/5.0) + 32.0);
	}	
	
	if(button_pressed)
	{
		update_temperature_type_location();		
		button_pressed = false;
	}
		
	status = at_ble_htpt_temp_send(convert_ieee754_ieee11073_float((float)temperature),
	&timestamp,
	htp_temp->flags,
	htp_temp->temperature_type,
	STABLE_TEMPERATURE_VAL);
	
	DBG_LOG("Temperature%s: %d %s, Location: %s", 
	((status == AT_BLE_SUCCESS)?"":" send failed"),
	(uint16_t)temperature,
	((htp_temp->flags & HTPT_FLAG_FAHRENHEIT)?"Fahrenheit":"Deg Celsius"),
	 temp_measurement_loc[htp_temp->temperature_type-1]);

	/* Stop the timer and wait for indication confirmation */
	hw_timer_stop();
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
	at_ble_htpt_temp_send_cfm_t *htpt_send_temp_cfm_params = (at_ble_htpt_temp_send_cfm_t *)params;
	
	if (htpt_send_temp_cfm_params->status == AT_BLE_SUCCESS)
	{
		/* Temperature sent confirmation */
		ble_data_sent_confim();
		
		/* start the timer for next interval of  temperature send */
		app_timer_done = false;
		hw_timer_start(htp_data.measurement_interval);
		DBG_LOG_DEV("Temperature Indication sent successfully");
	}
	else
	{
		DBG_LOG("Temperature Indication send failed, status:%d", htpt_send_temp_cfm_params->status);
	}
	
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
/* Callback registered for AT_PLATFORM_EVENT event from stack */
static at_ble_status_t platform_interrupt_app_event(void *param)
{
	at_ble_status_t status = AT_BLE_SUCCESS;
	platform_isr_event_t *plf_isr_event = (platform_isr_event_t *)param;
	
	if ( (plf_isr_event->event_type == ((TIMER_EXPIRED_CALLBACK_TYPE_DETECT << 8)  | RAM_ISR_TABLE_TIMER0_INDEX)) && app_timer_done )
	{
		if (temp_send_notification)
		{
			htp_temperature_send(&htp_data);
		}
		app_timer_done = false;		
	}
	else if( (plf_isr_event->event_type == ((PORTINT_CALLBACK_TYPE_DETECT << 8)  | RAM_ISR_TABLE_PORT0_COMB_INDEX)) )
	{
		button_pressed = true;
		status = AT_BLE_SUCCESS;
		delay_ms(200);
		button_debounce = true;
	}
	else
		status = AT_BLE_FAILURE;	
	return status;
}
static void button_cb(void)
{
	if(app_init_done && !button_pressed){
		send_plf_int_msg_ind(RAM_ISR_TABLE_PORT0_COMB_INDEX, PORTINT_CALLBACK_TYPE_DETECT, NULL, 0);
	}
}

/* Updating the location to read the temperature */
static void update_temperature_type_location(void)
{
	htp_data.temperature_type = (at_ble_htpt_temp_type)((htp_data.temperature_type % 9) + 1);
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
	if(app_init_done){
		hw_timer_stop();
		app_timer_done = true;
		send_plf_int_msg_ind(RAM_ISR_TABLE_TIMER0_INDEX, TIMER_EXPIRED_CALLBACK_TYPE_DETECT, NULL, 0);
	}
}

/* Custom events like user defined event, platform event callback handlers for OTAU profile */
static const ble_custom_event_cb_t htpt_app_custom_event_handle = {
	.platform_event_ready = platform_interrupt_app_event /* This event not handled in BLE Manager */
};

uint8_t app_exec = true;

int main (void)
{
	platform_driver_init();
	acquire_sleep_lock();

	/* Initialize the button */
	gpio_init();
	button_init();
	button_register_callback(button_cb);

	/* Initialize the LED */
	led_init();

	/* Initialize serial console */
	serial_console_init();
	
	DBG_LOG("Initializing HTP Application");
	
	/*Enable I2C  system clock */
	system_clock_peripheral_enable(PERIPHERAL_I2C0_CORE);
	
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
	
	app_init_done = true;
	/* Initialize the htp profile */
	htp_init();	
	
	DBG_LOG("HTP Initialization completed. Waiting for Event");
	
	DBG_LOG_PTS("The Temperature type is %d",htp_data.temperature_type);
	
	/* Registering the call backs for events with the ble manager */
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
	BLE_GAP_EVENT_TYPE,
	&app_gap_handle);

	/* Registering the call backs for events with the ble manager */
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
	BLE_GATT_HTPT_EVENT_TYPE,
	&app_htpt_handle);
	
	/* Register callbacks for custom related events */
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
	BLE_CUSTOM_EVENT_TYPE,
	&htpt_app_custom_event_handle);	
	
	while(app_exec) {
		/* BLE Event Task */
		ble_event_task(0XFFFFFFFF);
	}

	return 0;
}
