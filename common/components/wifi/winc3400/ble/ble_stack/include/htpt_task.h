/**
 * \file
 *
 * \brief HTPT.
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

#ifndef HTPT_TASK_H_INCLUDED
#define HTPT_TASK_H_INCLUDED

#include "cmn_defs.h"
#include "profiles.h"

/*
 * msgs from APP to HTPT
 *
 *
 */

at_ble_status_t htpt_create_db_req_handler(uint8_t  temp_type, 
											uint16_t valid_range_min,
											uint16_t valid_range_max,
											uint8_t features);

at_ble_status_t htpt_enable_req_handler( at_ble_handle_t conn_handle,
										uint8_t sec_lvl,
										uint8_t con_type,
										uint16_t temp_meas_ind_en,
										uint16_t interm_temp_ntf_en,
										uint16_t meas_intv_ind_en,
										uint16_t meas_intv);


at_ble_status_t htpt_temp_send_req_handler(uint32_t temp,
												at_ble_prf_date_time_t* time_stamp,
												uint8_t  flags,
												uint8_t  type,
												at_ble_handle_t conn_handle,
												uint8_t flag_stable_meas);



at_ble_status_t htpt_meas_intv_upd_req_handler(	at_ble_handle_t conn_handle,
										uint16_t meas_intv);
at_ble_status_t htpt_temp_type_upd_req_handler( at_ble_handle_t conn_handle,
											   uint8_t value);

/*
 * events from HTPT to APP
 *
 *
 */
void htpt_create_db_cfm_handler(uint16_t src,uint8_t *data,at_ble_htpt_create_db_cfm_t *param);
void htpt_disable_ind_handler(uint16_t src,uint8_t *data,at_ble_htpt_disable_ind_t *param);
void htpt_error_ind_handler(uint16_t src,uint8_t *data, at_ble_prf_server_error_ind_t  *param);
void htpt_temp_send_cfm_handler(uint16_t src, uint8_t *data,at_ble_htpt_temp_send_cfm_t *param);
void htpt_meas_intv_chg_ind_handler(uint16_t src,uint8_t *data,at_ble_htpt_meas_intv_chg_ind_t *param);
void htpt_cfg_indntf_ind_handler(uint16_t src,uint8_t *data,at_ble_htpt_cfg_indntf_ind_t *param);

/// Messages for Health Thermometer Profile Thermometer
enum
{
    ///Add a HTS instance into the database
    HTPT_CREATE_DB_REQ = 20480,
    ///inform APP of database creation status
    HTPT_CREATE_DB_CFM,

    ///Start the Health Thermometer Profile Thermometer profile - at connection
    HTPT_ENABLE_REQ,

    ///Automatically sent to the APP after a disconnection with the peer device
    HTPT_DISABLE_IND,

    /// Error indication to Host
    HTPT_ERROR_IND,

    ///Send temperature value from APP
    HTPT_TEMP_SEND_REQ,
    ///Send temperature value confirm to APP so stable values can be erased if correctly sent.
    HTPT_TEMP_SEND_CFM,

    ///Indicate Measurement Interval
    HTPT_MEAS_INTV_UPD_REQ,
    ///Inform APP of new measurement interval value
    HTPT_MEAS_INTV_CHG_IND,

    ///Inform APP of new configuration value
    HTPT_CFG_INDNTF_IND,

    ///Update Temperature Type Value
    HTPT_TEMP_TYPE_UPD_REQ,
};

#endif /* HTPT_TASK_H_INCLUDED */
