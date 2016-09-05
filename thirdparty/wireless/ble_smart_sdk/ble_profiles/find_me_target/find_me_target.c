
/**
 * \file
 *
 * \brief Find Me Target Profile
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
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel
 * Support</a>
 */

/****************************************************************************************
*							        Includes	                                     	*
****************************************************************************************/
#include <asf.h>
#include "timer_hw.h"
#include "ble_manager.h"
#include "find_me_target.h"
#include "immediate_alert.h"


/****************************************************************************************
*							        Globals	                                     		*
****************************************************************************************/

static /*const*/ ble_event_callback_t fmp_gap_handle[] = {
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	fmp_target_connected_state_handler,
	fmp_target_disconnect_event_handler,
	NULL,
	NULL,
	NULL,
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

static /*const*/ ble_event_callback_t fmp_gatt_server_handle[] = {
	NULL,
	NULL,
	fmp_target_char_changed_handler,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};

/* Immediate alert service declaration */
#ifdef IMMEDIATE_ALERT_SERVICE
gatt_service_handler_t ias_handle;
#endif

/** @brief Alert value used for immediate alert service */
uint8_t immediate_alert_value = INVALID_IAS_PARAM;

/** @brief Callback handlers for Immediate Alert service */
find_me_callback_t immediate_alert_cb;

/****************************************************************************************
*							        Implementation	                                    *
****************************************************************************************/

/**
 * \brief registering the path loss handler of the application
 */
void register_find_me_handler(find_me_callback_t immediate_alert_fn)
{
	immediate_alert_cb = immediate_alert_fn;
}

/**
 * \brief Initializations of find me profile services
 */
void fmp_target_service_init(void)
{
	/** Initializing the mandatory immediate alert service of find me
	 *profile*/
	init_immediate_alert_service(&ias_handle);
}

/**
 * \Definition of profile immediate alert services to the attribute data base
 */
at_ble_status_t fmp_target_service_define(void)
{
	ias_primary_service_define(&ias_handle);
	DBG_LOG("The Supported Services in Find Me Profile are:");
	DBG_LOG("  -> Immediate Alert Service");
	return AT_BLE_SUCCESS;
}

/**
 * \find me advertisement initialization and adv start
 */
void fmp_target_adv(void)
{
	/*  Set advertisement data from ble_manager*/
	if(!(ble_advertisement_data_set() == AT_BLE_SUCCESS))
	{
		DBG_LOG("Fail to set Advertisement data");
	}

	/* Start of advertisement */
	if (at_ble_adv_start(AT_BLE_ADV_TYPE_UNDIRECTED,
			AT_BLE_ADV_GEN_DISCOVERABLE, NULL, AT_BLE_ADV_FP_ANY,
			APP_FMP_FAST_ADV, APP_FMP_ADV_TIMEOUT,
			0) == AT_BLE_SUCCESS) {
		#ifdef DBG_LOG
		DBG_LOG("Bluetooth device is in Advertising Mode");
		#endif
	} else {
		#ifdef DBG_LOG
		DBG_LOG("BLE Adv start Failed");
		#endif
	}
}

/**
 * \Immediate alert service characteristic change handler function
 */
at_ble_status_t fmp_target_char_changed_handler(void *params)
{
	
	at_ble_characteristic_changed_t *char_handle;
	char_handle = (at_ble_characteristic_changed_t *)params;
	
	at_ble_characteristic_changed_t change_params;
	memcpy((uint8_t *)&change_params, char_handle,
			sizeof(at_ble_characteristic_changed_t));

	immediate_alert_value
		= ias_set_alert_value(&change_params, &ias_handle);

	if (immediate_alert_value != INVALID_IAS_PARAM) {
		immediate_alert_cb(immediate_alert_value);
	}

	return AT_BLE_SUCCESS;
}

/**
 * \Find me profile connected state handler function
 */
at_ble_status_t fmp_target_connected_state_handler(void * params)
{
	at_ble_status_t status;
	uint16_t len;

	/* / * Upon connection set default value to no alert* / */
	/* immediate_alert_cb(IAS_NO_ALERT); */
	status = at_ble_characteristic_value_get(ias_handle.serv_chars.char_val_handle,
						&immediate_alert_value,
						&len);
	if (status != AT_BLE_SUCCESS) {
		DBG_LOG(
				"Read of alert value for Immediate alert service failed:reason %x",
				status);
	}

	return AT_BLE_SUCCESS;
}

/**
 * \Find me profile disconnected state handler function
 */
at_ble_status_t fmp_target_disconnect_event_handler(void * params)
{
	if (at_ble_adv_start(AT_BLE_ADV_TYPE_UNDIRECTED,
			AT_BLE_ADV_GEN_DISCOVERABLE, NULL, AT_BLE_ADV_FP_ANY,
			APP_FMP_FAST_ADV, APP_FMP_ADV_TIMEOUT,
			0) != AT_BLE_SUCCESS) {
		#ifdef DBG_LOG
		DBG_LOG("BLE Adv start Failed");
		#endif
	} else {
		DBG_LOG("Bluetooth Device is in Advertising Mode");
	}

	return AT_BLE_SUCCESS;
}


/**
 * \Find me Profile service initialization, declarations and advertisement
 */
void fmp_target_init(void *param)
{
	fmp_gap_handle[0] = NULL;
	fmp_gap_handle[1] = NULL;
	fmp_gap_handle[2] = NULL;
	fmp_gap_handle[3] = NULL;
	fmp_gap_handle[4] = NULL;
	fmp_gap_handle[5] = fmp_target_connected_state_handler;
	fmp_gap_handle[6] = fmp_target_disconnect_event_handler;
	fmp_gap_handle[7] = NULL;
	fmp_gap_handle[8] = NULL;
	fmp_gap_handle[9] = NULL;
	fmp_gap_handle[10] = NULL;
	fmp_gap_handle[11] = NULL;
	fmp_gap_handle[12] = NULL;
	fmp_gap_handle[13] = NULL;
	fmp_gap_handle[14] = NULL;
	fmp_gap_handle[15] = NULL;
	fmp_gap_handle[16] = NULL;
	fmp_gap_handle[17] = NULL;
	fmp_gap_handle[18] = NULL;

	fmp_gatt_server_handle[0] = NULL;
	fmp_gatt_server_handle[1] = NULL;
	fmp_gatt_server_handle[2] = fmp_target_char_changed_handler;
	fmp_gatt_server_handle[3] = NULL;
	fmp_gatt_server_handle[4] = NULL;
	fmp_gatt_server_handle[5] = NULL;
	fmp_gatt_server_handle[6] = NULL;
	fmp_gatt_server_handle[7] = NULL;
	fmp_gatt_server_handle[8] = NULL;
	fmp_gatt_server_handle[9] = NULL;

	/* find me services initialization*/
	fmp_target_service_init();

	/* find me services definition    */
	fmp_target_service_define();

	/* find me services advertisement */
	fmp_target_adv();
	
	/* Callback registering for BLE-GAP Role */
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK, BLE_GAP_EVENT_TYPE, fmp_gap_handle);
	
	/* Callback registering for BLE-GATT-Server Role */
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK, BLE_GATT_SERVER_EVENT_TYPE, fmp_gatt_server_handle);
}
