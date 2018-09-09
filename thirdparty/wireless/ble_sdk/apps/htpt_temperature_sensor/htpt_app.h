/**
 * \file
 *
 * \brief Health Thermometer Profile Application declarations
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


#ifndef __HTPT_APP_H__
#define __HTPT_APP_H__

#include "at_ble_api.h"

#define DBG_LOG_CONT	 printf

#define DBG_LOG		     printf("\r\n");\
						 printf

#define APP_HT_FAST_ADV 1600 //1000 ms

#define APP_HT_ADV_TIMEOUT 655 // 100 Secs

#define SCAN_RESP_LEN 10
#define ADV_DATA_LEN 18

#define ADV_TYPE_LEN (0x01)

#define HT_ADV_DATA_UUID_LEN	 (2)
#define HT_ADV_DATA_UUID_TYPE	 (0x03)
#define HT_ADV_DATA_UUID_DATA	 "\x09\x18"

#define HT_ADV_DATA_APPEARANCE_LEN	 (2)
#define HT_ADV_DATA_APPEARANCE_TYPE	 (0x19)
#define HT_ADV_DATA_APPEARANCE_DATA	 "\x00\x03"

#define HT_ADV_DATA_NAME_LEN	 (9)
#define HT_ADV_DATA_NAME_TYPE	 (0x09)
#define HT_ADV_DATA_NAME_DATA	 "ATMEL-HTP"

/* Typedef for health thermometer profile -  application */
typedef struct htp_app{	
	
	/* Measured temperature value. Value may be Cecilius /Fahrenheit */
	uint32_t temperature;	
	
	/* Temperature type string */
	at_ble_htpt_temp_type temperature_type;
	
	/* Measurement Interval */
	uint16_t measurement_interval;
	
	/* Minimum measurement interval */
	uint16_t min_measurement_intv;
	
	/* Maximum measurement interval */
	uint16_t max_meaurement_intv;
	
	/* Security Level */
	at_ble_htpt_sec_level security_lvl;
	
	/* Optional Features */
	at_ble_htpt_db_config_flag optional;
	
	at_ble_htpt_temp_flags flags;
}htp_app_t;

/**@brief Temperature measurement stability type
*/
typedef enum
{
	UNSTABLE_TEMPERATURE_VAL= 0,
	STABLE_TEMPERATURE_VAL=1
}stable_temp_reading;

static at_ble_status_t app_connected_event_handler(void *params);
static at_ble_status_t app_disconnected_event_handler(void *params);
static at_ble_status_t app_pair_done_event_handler(void *params);
static at_ble_status_t app_encryption_status_changed_handler(void *params);
static at_ble_status_t  app_htpt_create_db_cfm_handler(void *params) ;
static at_ble_status_t  app_htpt_error_ind_handler(void *params);
static at_ble_status_t  app_htpt_disable_ind_handler(void *params);
static at_ble_status_t  app_htpt_temp_send_cfm(void *params);
static at_ble_status_t  app_htpt_meas_intv_chg_ind_handler(void *params);
static at_ble_status_t  app_htpt_cfg_indntf_ind_handler(void *params);
static at_ble_status_t  app_htptp_meas_intv_chg_req(void *params);
static at_ble_status_t  app_htpt_enable_rsp_handler(void *params);
static at_ble_status_t  app_htpt_meas_intv_upd_rsp(void *params);
#endif /* __HTPT_APP_H__ */
