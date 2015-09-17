/**
 * \file
 *
 * \brief Health Thermometer Profile Application
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
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
#include "platform.h"
#include "at_ble_api.h"
#include "htpt_app.h"
#include "profiles.h"
#include "console_serial.h"
#include "timer_hw.h"
#include "conf_extint.h"

static uint8_t scan_rsp_data[SCAN_RESP_LEN] = {0x09,0xFF, 0x00, 0x06, 0xd6, 0xb2, 0xf0, 0x05, 0xf0, 0xf8};

at_ble_LTK_t app_bond_info;
bool app_device_bond = false;
uint8_t auth_info = 0;

htpt_app_t htpt_data;

void app_init(void)
{
	uint8_t port = 74;
	at_ble_addr_t addr;
    // init device
	at_ble_init(&port);
	
	at_ble_addr_get(&addr);
	/* Update the Address in scan response data*/
	memcpy(&scan_rsp_data[4], &addr.addr, 6);
	at_ble_addr_set(&addr);

	/* Initialize the htpt to default value */
	htpt_init(&htpt_data);

	/* Register the Initialized value into htpt profile */
	if(at_ble_htpt_create_db(
							htpt_data.optional,
							htpt_data.temperature_type,
							htpt_data.min_measurement_intv,
							htpt_data.max_meaurement_intv,
							htpt_data.measurement_interval,
							htpt_data.security_lvl
							) == AT_BLE_FAILURE)
	{
		DBG_LOG("\r\nHTPT Data Base creation failed");
		while(1);
	}
}


int main (void)
{
	at_ble_events_t event;
	uint8_t params[512];

	at_ble_handle_t handle = 0;

#if SAMG55
	/* Initialize the SAM system. */
	sysclk_init();
	board_init();
#elif SAM0
	system_init();
#endif
	 
	button_init();
	serial_console_init();
	
	DBG_LOG_1LVL("\r\nInitializing HTPT Application");

	at30tse_init();
	
	at30tse_write_config_register(
			AT30TSE_CONFIG_RES(AT30TSE_CONFIG_RES_12_bit));	

	hw_timer_init();
	hw_timer_register_callback(timer_callback_handler);
	app_init();
	
	DBG_LOG_1LVL("\r\nHTPT Initialization completed. Waiting for Event");
	
	while(at_ble_event_get(&event, params, -1) == AT_BLE_SUCCESS)
	{
		switch(event)
		{
			case AT_BLE_CONNECTED:
			{
				at_ble_connected_t conn_params;
				memcpy((uint8_t *)&conn_params, params, sizeof(at_ble_connected_t));
				
				DBG_LOG("\r\nDevice connected to 0x%02x%02x%02x%02x%02x%02x handle=0x%x",
					conn_params.peer_addr.addr[5],
					conn_params.peer_addr.addr[4],
					conn_params.peer_addr.addr[3],
					conn_params.peer_addr.addr[2],
					conn_params.peer_addr.addr[1],
					conn_params.peer_addr.addr[0],
					conn_params.handle);
				handle = conn_params.handle;
				LED_On(LED0);
				
				/* Enable the HTPT Profile */
				if(at_ble_htpt_enable(handle, HTPT_CFG_STABLE_MEAS_IND) == AT_BLE_FAILURE)
				{
					DBG_LOG("\r\nFailure in HTPT Profile Enable");
				}						
			}
			break;

			case AT_BLE_DISCONNECTED:
			{
				at_ble_disconnected_t disconnect;
				memcpy((uint8_t *)&disconnect, params, sizeof(at_ble_disconnected_t));
				
				hw_timer_stop();
				LED_Off(LED0);	
				
				DBG_LOG("\r\nDevice disconnected Reason:0x%02x Handle=0x%x", disconnect.reason, disconnect.handle);
				
				if(at_ble_adv_start(AT_BLE_ADV_TYPE_UNDIRECTED, AT_BLE_ADV_GEN_DISCOVERABLE, NULL, AT_BLE_ADV_FP_ANY, 
				                   APP_HT_FAST_ADV, APP_HT_ADV_TIMEOUT, 0) != AT_BLE_SUCCESS)
				{
					DBG_LOG_1LVL("\r\nBLE Adv start Failed");
				}
				else
				{
					DBG_LOG_1LVL("\r\nBLE Started Adv");
				}								
												
			}
			break;
			
			case AT_BLE_CHARACTERISTIC_CHANGED:
			{
				at_ble_characteristic_changed_t change_params;				 	
				uint32_t i = 0;
				
				memcpy((uint8_t *)&change_params, params, sizeof(at_ble_characteristic_changed_t));

				DBG_LOG("Characteristic 0x%x changed, new_value = ", 
					change_params.char_handle);
				for(i=0; i<change_params.char_len; i++)
					DBG_LOG("0x%02x ", change_params.char_new_value[i]);
				DBG_LOG("\n");
			}
			break;	
			
			/* HTPT Health Thermometer Profile events */
			
			/** Inform APP of database creation status */
			case AT_BLE_HTPT_CREATE_DB_CFM:	
			{
				uint8_t idx = 0;
				uint8_t adv_data[HT_ADV_DATA_NAME_LEN + HT_ADV_DATA_APPEARANCE_LEN + HT_ADV_DATA_UUID_LEN + 3*2];
				
				at_ble_htpt_create_db_cfm_t create_db_params;
				
				memcpy((uint8_t *)&create_db_params, params, sizeof(at_ble_htpt_create_db_cfm_t));
								
				// start advertising
				DBG_LOG("\r\nCreating HTPT DB: SUCCESS: Status=0x%x", create_db_params.status);
				
				/* Prepare ADV Data */
				adv_data[idx++] = HT_ADV_DATA_UUID_LEN + ADV_TYPE_LEN;
				adv_data[idx++] = HT_ADV_DATA_UUID_TYPE;
				memcpy(&adv_data[idx], HT_ADV_DATA_UUID_DATA, HT_ADV_DATA_UUID_LEN);				
				idx += HT_ADV_DATA_UUID_LEN;
				
				adv_data[idx++] = HT_ADV_DATA_APPEARANCE_LEN + ADV_TYPE_LEN;
				adv_data[idx++] = HT_ADV_DATA_APPEARANCE_TYPE;
				memcpy(&adv_data[idx], HT_ADV_DATA_APPEARANCE_DATA, HT_ADV_DATA_APPEARANCE_LEN);
				idx += HT_ADV_DATA_APPEARANCE_LEN;
				
				adv_data[idx++] = HT_ADV_DATA_NAME_LEN + ADV_TYPE_LEN;
				adv_data[idx++] = HT_ADV_DATA_NAME_TYPE;
				memcpy(&adv_data[idx], HT_ADV_DATA_NAME_DATA, HT_ADV_DATA_NAME_LEN);
				idx += HT_ADV_DATA_NAME_LEN;			
				
				
				at_ble_adv_data_set(adv_data, idx, scan_rsp_data, SCAN_RESP_LEN);
				
				if(at_ble_adv_start(AT_BLE_ADV_TYPE_UNDIRECTED, AT_BLE_ADV_GEN_DISCOVERABLE, NULL, AT_BLE_ADV_FP_ANY, 
				                   APP_HT_FAST_ADV, APP_HT_ADV_TIMEOUT, 0) != AT_BLE_SUCCESS)
				{
					DBG_LOG("\r\nBLE Adv start Failed");
				}
				else
				{
					DBG_LOG("\r\nBLE Started Adv");
				}								
			}
			break;
			
			/** Error indication to APP*/
			case AT_BLE_HTPT_ERROR_IND:
			{
				prf_server_error_ind_t prf_htpt_error_ind;
				memcpy((uint8_t *)&prf_htpt_error_ind, params, sizeof(prf_server_error_ind_t));
				
				DBG_LOG("\r\n HTPT Error Indication received, msg_id=0x%x, handle=0x%x, status=0x%x",
				prf_htpt_error_ind.msg_id, prf_htpt_error_ind.conhdl, prf_htpt_error_ind.status);
			}					
			break;
			
			/** Automatically sent to the APP after a disconnection with the peer device to confirm disabled profile*/
			case AT_BLE_HTPT_DISABLE_IND:
			{
				at_ble_htpt_disable_ind_t htpt_disable_ind_params;
				memcpy((uint8_t *)&htpt_disable_ind_params, params, sizeof(at_ble_htpt_disable_ind_t));
				DBG_LOG("\r\nHTPT Disable Indication: conhdl=0x%x, interm_temp_ntf_en=0x%x, meas_intv=0x%x, meas_intv_ind_en=0x%x, handle=0x%x",
				htpt_disable_ind_params.conhdl,
				htpt_disable_ind_params.interm_temp_ntf_en,
				htpt_disable_ind_params.meas_intv,
				htpt_disable_ind_params.meas_intv_ind_en,
				htpt_disable_ind_params.temp_meas_ind_en);
			}
			break;
			
			/** Temperature value confirm to APP*/
			case AT_BLE_HTPT_TEMP_SEND_CFM:
			{
				at_ble_htpt_temp_send_cfm_t htpt_send_temp_cfm_params;
				memcpy((uint8_t *)&htpt_send_temp_cfm_params, params, sizeof(at_ble_htpt_temp_send_cfm_t));
				DBG_LOG("\r\n HTPT Temperature Send Confirm: cfm_type=0x%x, conhdl=0x%x, status=0x%x,",
					htpt_send_temp_cfm_params.cfm_type, htpt_send_temp_cfm_params.conhdl,
					htpt_send_temp_cfm_params.status);	
				hw_timer_start(htpt_data.measurement_interval);			
			}
			break;
			
			/** Inform APP of new measurement interval value */
			case AT_BLE_HTPT_MEAS_INTV_CHG_IND:
			{
				at_ble_htpt_meas_intv_chg_ind_t htpt_meas_intv_chg_params;
				memcpy((uint8_t *)&htpt_meas_intv_chg_params, params, sizeof(at_ble_htpt_meas_intv_chg_ind_t));
				DBG_LOG("\r\nHTPT measure Interval change Indication: Interval=%d", htpt_meas_intv_chg_params.intv);
				htpt_data.measurement_interval = htpt_meas_intv_chg_params.intv;							
			}
			break;
			
			/** Inform APP of new configuration value*/
			case AT_BLE_HTPT_CFG_INDNTF_IND:
			{
				at_ble_htpt_cfg_indntf_ind_t htpt_cfg_indntf_ind_params;
				memcpy((uint8_t *)&htpt_cfg_indntf_ind_params, params, sizeof(at_ble_htpt_cfg_indntf_ind_t));
				DBG_LOG("\r\nHTPT Cfg indication notification indication cfg_val=0x%x, char code=0x%x, conhdl=0x%x", htpt_cfg_indntf_ind_params.cfg_val,
				htpt_cfg_indntf_ind_params.char_code, htpt_cfg_indntf_ind_params.conhdl);
				if (htpt_cfg_indntf_ind_params.char_code == HTPT_TEMP_MEAS_CHAR &&  htpt_cfg_indntf_ind_params.cfg_val == 2)
				{
					htpt_temperature_send(&htpt_data);
				}				
			}			
			break;
			
			case AT_BLE_PAIR_REQUEST:
			{
				at_ble_pair_features_t features;
				uint8_t i = 0;
				char bond;
	
				DBG_LOG("\r\nRemote device request pairing");
			
				/* Check if we are already bonded (Only one bonded connection is supported
				in this example)*/
				if(app_device_bond)
				{
					DBG_LOG("\r\nBound relation exists with previously peer device");
					DBG_LOG("\r\nTo remove existing bonding information and accept pairing request from peer device press y else press n : ");
					do
					{
						bond = 'y';//getchar();						
						if((bond == 'Y') || (bond == 'y'))
						{
							app_device_bond = false;
							break;
						}
						else if ((bond == 'N') || (bond == 'n'))
						{
							DBG_LOG("\r\nPairing failed \n");
							break;
						}
						else
						{
							DBG_LOG("\r\nWrong value entered please try again : \n");
						}
					}while(app_device_bond);
				}
			
				if(!app_device_bond)
				{
					/* Authentication requirement is bond and MITM*/
					features.desired_auth =  AT_BLE_MODE1_L1_NOAUTH_PAIR_ENC;
					features.bond = false;
					features.mitm_protection = false;
					features.oob_avaiable = false;
					/* Device capabilities is display only , key will be generated 
					and displayed */
					features.io_cababilities = AT_BLE_IO_CAP_NO_INPUT_NO_OUTPUT;
					/* Distribution of LTK is required */
					features.initiator_keys =   AT_BLE_KEY_DIST_ENC;
					features.responder_keys =   AT_BLE_KEY_DIST_ENC;
					features.max_key_size = 16;
					features.min_key_size = 16;
				
					/* Generate LTK */
					for(i=0 ; i<8 ;i++)
					{
						app_bond_info.key[i] = rand()&0x0f;
						app_bond_info.nb[i] = rand()&0x0f;
					}

					for(i=8 ; i<16 ;i++)
					{
						app_bond_info.key[i] = rand()&0x0f;
					}

					app_bond_info.ediv = rand()&0xffff; 
					app_bond_info.key_size = 16;
					/* Send pairing response */
					DBG_LOG("\r\n Sending pairing response handle=0x%x",
							handle);
					if(at_ble_authenticate(handle, &features, &app_bond_info, NULL) != AT_BLE_SUCCESS)
					{
						features.bond = false;
						features.mitm_protection = false;
						at_ble_authenticate(handle, &features, NULL, NULL);
					}					
				}
			}	
			break;
		
			case AT_BLE_PAIR_KEY_REQUEST:
			{
				/* Passkey has fixed value in this example MSB */
				uint8_t passkey[6]={0,0,0,0,0,0};
				uint8_t passkey_ascii[6];
				uint8_t i = 0;
	
				at_ble_pair_key_request_t pair_key_request;
				memcpy((uint8_t *)&pair_key_request, params, sizeof(at_ble_pair_key_request_t));
				/* Display passkey */
				if(pair_key_request.passkey_type == AT_BLE_PAIR_PASSKEY_DISPLAY)
				{
					/* Convert passkey to ASCII format */
					for(i=0; i<AT_BLE_PASSKEY_LEN ; i++)
					{
						passkey_ascii[i] = (passkey[i] + 48); 
					}
					DBG_LOG("\r\nplease enter the following code on the other device : ");
					for(i=0; i<AT_BLE_PASSKEY_LEN ; i++)
					{
						DBG_LOG("%c",passkey_ascii[i]);
					}
					DBG_LOG("\n");	
					at_ble_pair_key_reply(pair_key_request.handle,pair_key_request.type,passkey_ascii);
				}
			
			}
			break;

			case AT_BLE_PAIR_DONE:
			{
				at_ble_pair_done_t pair_params;
				memcpy((uint8_t *)&pair_params, params, sizeof(at_ble_pair_done_t));				
				if(pair_params.status == AT_BLE_SUCCESS)
				{
					DBG_LOG("\r\nPairing procedure completed successfully \n");
					app_device_bond = true;
					auth_info = pair_params.auth;
					
					handle = pair_params.handle;
					
					/* Enable the HTPT Profile */
					if(at_ble_htpt_enable(handle, HTPT_CFG_STABLE_MEAS_IND) == AT_BLE_FAILURE)
					{
						DBG_LOG("\r\nFailure in HTPT Profile Enable");
					}
				}
				else
				{
					DBG_LOG("\r\nPairing failed \n");
				}
			}
			break;

			case AT_BLE_ENCRYPTION_REQUEST:
			{
				bool key_found = false;
	
				at_ble_encryption_request_t enc_req;
				memcpy((uint8_t *)&enc_req, params, sizeof(at_ble_encryption_request_t));
			
				/* Check if bond information is stored */
				if((enc_req.ediv == app_bond_info.ediv)
					&& !memcmp(&enc_req.nb[0],&app_bond_info.nb[0],8))
				{
					key_found = true;
				}
			
				at_ble_encryption_request_reply(handle,auth_info ,key_found,app_bond_info);
			}
			break;
		
			case AT_BLE_ENCRYPTION_STATUS_CHANGED:
			{
				at_ble_encryption_status_changed_t enc_status;
				memcpy((uint8_t *)&enc_status, params, sizeof(at_ble_encryption_status_changed_t));
				if(enc_status.status == AT_BLE_SUCCESS)
				{
					DBG_LOG("\r\nEncryption completed successfully \n");
					
					handle = enc_status.handle;
					
					/* Enable the HTPT Profile */
					if(at_ble_htpt_enable(handle, HTPT_CFG_STABLE_MEAS_IND) == AT_BLE_FAILURE)
					{
						DBG_LOG("\r\nFailure in HTPT Profile Enable");
					}
				}
				else
				{
					DBG_LOG("\r\nEncryption failed \n");
				}
			}
			break;
			
			default:
			 LED_Off(LED0);
			 DBG_LOG("\r\nUnknown event received: event=0x%x", event);
			break;
		}
	}
}


void htpt_init(htpt_app_t *htpt_temp)
{
	/* Initialize to default temperature value  and htpt parameters*/
	htpt_temp->measurement_interval = 1; 
	htpt_temp->temperature = 3700;
	htpt_temp->temperature_type = HTP_TYPE_BODY;
	htpt_temp->max_meaurement_intv = 30;
	htpt_temp->min_measurement_intv = 1;
	htpt_temp->security_lvl = HTPT_UNAUTH;
	htpt_temp->optional = HTPT_ALL_FEAT_SUP;
	htpt_temp->flags = HTPT_FLAG_CELSIUS | HTPT_FLAG_TYPE;
}


void htpt_temperature_send(htpt_app_t *htpt_temp)
{
	//static uint32_t temperature;
	at_ble_prf_date_time_t timestamp;
#if SAMD21
	float temperature;
	/* Read Temperature Value from IO1 Xplained Pro */
	temperature = at30tse_read_temperature();	 
	
#endif

#if SAMG55
	double temperature;
	/* Read Temperature Value from IO1 Xplained Pro */
	at30tse_read_temperature(&temperature);
#endif	

	if (htpt_temp->flags & HTPT_FLAG_FAHRENHEIT)
	{
		temperature = (((temperature * 9.0)/5.0) + 32.0);
	}
	
	timestamp.day = 1;
	timestamp.hour = 9;
	timestamp.min = 2;
	timestamp.month = 8;
	timestamp.sec = 36;
	timestamp.year = 15;
	at_ble_htpt_temp_send((uint32_t)temperature,
	                     &timestamp,
						 htpt_temp->flags,
						 htpt_temp->temperature_type,
						 STABLE_TEMPERATURE_VAL
						 );
}

void button_cb(void)
{
	htpt_data.temperature_type = ((htpt_data.temperature_type+1) % 9);
	if ((htpt_data.temperature_type == HTP_TYPE_ARMPIT) && (htpt_data.flags == (HTPT_FLAG_CELSIUS | HTPT_FLAG_TYPE)))
	{
		htpt_data.flags = HTPT_FLAG_FAHRENHEIT | HTPT_FLAG_TYPE;
	}
	else if (htpt_data.temperature_type == HTP_TYPE_ARMPIT)
	{
		htpt_data.flags = HTPT_FLAG_CELSIUS | HTPT_FLAG_TYPE;
	}
}

void timer_callback_handler(void)
{
	hw_timer_stop();
	htpt_temperature_send(&htpt_data);	
}
