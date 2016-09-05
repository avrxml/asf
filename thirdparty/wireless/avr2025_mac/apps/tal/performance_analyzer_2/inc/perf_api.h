/**
 * \file perf_api.h
 *
 * \brief API function protypes - Performance Analyzer application for AT86RF215
 *
 * Copyright (c) 2015 Atmel Corporation. All rights reserved.
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
 */

/*
 * Copyright (c) 2015, Atmel Corporation All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

/* Prevent double inclusion */
#ifndef PERF_API_H
#define PERF_API_H

/**
 * \addtogroup group_serial_parser
 * \{
 */

/* === Includes ============================================================= */

#include "return_val.h"
#include "qmm.h"
#include "perf_msg_const.h"
#include "app_per_mode.h"
/* === Macros =============================================================== */

/*======================Extern===============================================*/

/*========================Prototypes========================================= */

/**
 * \name Functions for User Request Primitives
 * \{
 */

/**
 * *\brief Function to set the various configuration parameters for PER Test
 *
 * \param param_type    Parameter type to be set
 * \param param_value   Pointer to the parameter value to be set
 */
void perf_set_req(trx_id_t trx, uint8_t set_param_type,
		param_value_t *param_value);

/**
 * *\brief Function to set the various configuration parameters in SUN PAGE for
 * PER Test
 *
 * \param sun_page    Pointer to the configurable parameter value structure
 */
void perf_set_sun_page(trx_id_t trx, uint8_t *sun_page);

/**
 * \brief Function to get the various configuration parameters for PER Test
 *
 * \param param_type    Parameter type to be read
 */
void perf_get_req(trx_id_t trx, uint8_t param_type);

/**
 * \brief Initiates the test procedure
 */
void initiate_per_test(trx_id_t trx);

/**
 * \brief Function to start the ED scan
 *
 * \param scan_duration paramter which is used to calculate the scan time
 *        on each channel
 */
void start_ed_scan(trx_id_t trx, uint8_t scan_duration,
		uint32_t channel_sel_mask);

/**
 * \brief Function to get the Sensor data like Battery volatge
 */
void get_sensor_data(trx_id_t trx);

/**
 * \brief prints the hardware details
 */
void get_board_details(trx_id_t trx);

/**
 * \brief Identifying peer node
 */
void identify_peer_node(trx_id_t trx);

#if ((TAL_TYPE != AT86RF230B) || ((TAL_TYPE == AT86RF230B) && (defined TFA_CW)))

/**
 * \brief Send an energy pulse on current channel page
 */
void pulse_cw_transmission(trx_id_t trx);

/**
 * \brief Start CW transmission on current channel page
 * \param tx_mode  Continuous transmission mode
 */
void start_cw_transmission(trx_id_t trx, uint8_t tx_mode);

/**
 * \brief Stop CW transmission on current channel page
 * \param tx_mode  Continuous transmission mode
 */
void stop_cw_transmission(trx_id_t trx, uint8_t tx_mode);

#endif /*#if ((TAL_TYPE != AT86RF230B) || ((TAL_TYPE == AT86RF230B) && (defined
        * TFA_CW))) */

/**
 * \brief Read transceiver register
 * \param reg_addr  The address of the register to be read
 */
void read_trx_registers(trx_id_t trx, uint16_t reg_addr);

/**
 * \brief write Transceiver registers
 * \param reg_addr  The address of the register to be read
 * \param reg_val   The value of the register to be written
 */
void write_trx_registers(trx_id_t trx, uint16_t reg_addr, uint8_t reg_val);

/**
 * \brief Read a set of registers
 * \param start_reg_addr  The start address of the group of registers to be read
 * \param end_reg_addr    The end register of the group of registers to be read
 */
void dump_trx_register_values(trx_id_t trx, uint16_t start_reg_addr,
		uint16_t end_reg_addr);

/**
 * \brief Function to request the peer to get disconnected from source node
 */
void disconnect_peer_node(trx_id_t trx);

/**
 * \brief Function to set the default values of
 *
 * all configurable paramters on source and peer node
 */
void set_default_configuration(trx_id_t trx);

/**
 * \brief Function to get the current values of the all configurabel patameters
 *
 * in the Performance Anlayzer application
 */
void get_current_configuration(trx_id_t trx);

/**
 * \brief Function used to check the error conditions before
 *
 * processing the received command
 *
 * \return the erroe codee based onthe currently ongoing operation,if any
 */
uint8_t check_error_conditions(trx_id_t trx);

/*
 * \brief get the parameter length based on the paramter tye
 *
 * \param param_type Paramter type
 */
uint8_t get_param_length(uint8_t parameter_type);

/* ! \} */

/**
 * \name Functions for User Confirm/Indication  Messages generated in response
 * to the Request Primitives
 * \{
 */

/**
 * Function to generate Perf Start confirmation frame that must be sent to
 * host application via serial interface.
 * Called by Performance application as confirmation for perf_start_req
 * \param status                Result for requested Perf start request
 * \param start mode            In which mode the transmitter is started
 *                              i.e.single node or PER mode
 * \param trx_config_params     configurable param values structure
 * \param peer_ic_type          Peer Ic Type whether it is SOC or MCU
 * \param peer_soc_mcu_name     Peer MCU or SOC name as string
 * \param peer_trx_name         Peer Transceiver's name as string
 * \param peer_board_name Name  Peer HW Board name as string
 * \param peer_mac_address      MAC address of the Peer node
 * \return void
 */
void usr_perf_start_confirm(trx_id_t trx,
		uint8_t status,
		uint8_t start_mode,
		trx_config_params_t *trx_config_params,
		uint8_t peer_ic_type,
		char *peer_soc_mcu_name,
		char *peer_trx_name,
		char *peer_board_name,
		uint64_t peer_mac_address,
		float peer_fw_version, uint32_t peer_feature_mask);

/**
 * Function to generate Per Test Start confirmation frame that must be sent to
 * host application via serial interface.
 * Called by Performance application as confirmation for per_test_start_req
 * request
 * \param status      Result for requested per_test_start_req
 *
 * \return void
 */
void usr_per_test_start_confirm(trx_id_t trx, uint8_t status);

/**
 * Function to generate Per test End Indication frame that must be sent to
 * host application via serial interface.
 * Called by Performance application as Indication afetr completion of PER test
 * \param status                Result for PER test intiated by
 * per_test_start_req
 * \param avg_rssi              Average RSSI meausred for PER Test
 * \param avg_lqi               Average LQI meausred for PER Test
 * \param frames_transmitted    No.of transmitted pkts in the PER Test
 * \param frames_received       No.of received pkts in the PER Test
 * \param frames_failure        No. Of pkts failed to transmit
 * \param frames_no_ack         No. Of pkts transmitted without ACK
 * \param frames_channel_access_failure  No. Channel access failures
 * \param test_duration_sec     Time taken for the PER test
 * \param test_net_data_rate    Net data rate for the test
 *
 * \return void
 */
void usr_per_test_end_indication(trx_id_t trx,
		uint8_t status,
		int8_t rssi_val,
		uint8_t aver_lqi,
		uint32_t frames_transmitted,
		uint32_t frames_received,
		uint32_t frames_failure,
		uint32_t frames_no_ack,
		uint32_t frames_channel_access_failure,
		uint32_t frames_with_wrong_crc,
		float test_duration_sec,
		float test_net_data_rate);

/**
 * Function to generate ED scan confirm test End Indication frame that must be
 * sent to
 * host application via serial interface.
 * Called by Performance application as Indication before starting the ED scan
 * \param status                Confirmation to the ED scan req
 * \param scan_time_min         Approx time to be taken for ed can if timme is
 * more than a minute
 * \param scan_time_sec         Approx time to be taken for ed can if timme is
 * less than a minute
 *
 * \return void
 */
void usr_ed_scan_start_confirm(trx_id_t trx, uint8_t status,
		uint8_t scan_time_min, float scan_time_sec);

/**
 * Function to generate ED scan Indication frame that must be sent to
 * host application via serial interface.
 * Called by Performance application as Indication after completion of the ED
 * scan
 * \param no_of_channels  No. of channels scanned
 * \param ed_scan_result  List of Energy values along with the channel numbers
 *
 * \return void
 */
void usr_ed_scan_end_indication(trx_id_t trx, uint8_t no_of_channels,
		ed_scan_result_t *ed_scan_result );

/**
 * Function to generate Sensor data confirm frame that must be sent to
 * host application via serial interface.
 * Called by Performance application as confirmation after getting the sensor
 * data
 * \param status           Result for the Sensor data get req
 * \param bat_voltage      Battery voltage vlaue
 * \param temperature      temperature value
 *
 * \return void
 */
void usr_sensor_data_get_confirm(trx_id_t trx, uint8_t status,
		float bat_voltage, float temperature);

/**
 * Function to generate Identify Board confirm frame that must be sent to
 * host application via serial interface.
 * Called by Performance application as confirmation to the board identify req
 *
 * \param status           Result for the set default config req
 *
 * \param ic_type         Ic Type whether it is SOC or MCU
 * \param mcu_soc_name    MCU or SOC name as string
 * \param trx_name        Transceiver's name as string
 * \param Board Name      HW Board name as string
 * \param mac_address     MAC address of the source node
 * \param fw_version      Firmware version
 * \param fw_feature_mask Features supported by the firmware
 * \return void
 */
void usr_identify_board_confirm(trx_id_t trx, uint8_t status,
		uint8_t ic_type,
		const char *mcu_soc_name,
		const char *trx_name,
		const char *board_name,
		uint64_t mac_address,
		float fw_version,
		uint32_t fw_feature_mask);

/**
 * Function to generate Perf Set confirmation frame that must be sent to
 * host application via serial interface.
 * Called by Performance application as confirmation for perf_set_req request
 * \param status        Result for requested perf_set_req
 * \param param_type    Type of the parameter that has been set
 * \param parame_value  Pointer to the value of the parameter that has been set
 *
 * \return void
 */
void usr_perf_set_confirm(trx_id_t trx, uint8_t status, uint8_t param_type,
		param_value_t *param_value);

/**
 * Function to generate Perf Get confirmation frame that must be sent to
 * host application via serial interface.
 * Called by Performance application as confirmation for perf_get_req request
 * \param status        Result for requested perf_get_req
 * \param param_type    Type of the parameter that has been read
 * \param parame_value  Pointer to the value of the parameter has been read
 *
 * \return void
 */
void usr_perf_get_confirm(trx_id_t trx, uint8_t status, uint8_t parameter_type,
		param_value_t *parame_value);

/**
 * Function to generate Identify_peer_cnode_confirm that must be sent to
 * host application via serial interface.
 * Called by Performance application as reply to the identify_peer_node_req
 *
 * \param status  Result for identify_peer_node_req
 */
void usr_identify_peer_node_confirm(trx_id_t trx, uint8_t status);

/**
 * Function to generate cont_pulse_tx_confirm that must be sent to
 * host application via serial interface.
 * Called by Performance application as reply to the cont_pulse_tx_req
 *
 * \param status  Result for cont_pulse_tx_req
 */
void usr_cont_pulse_tx_confirm(trx_id_t trx, uint8_t status);

/**
 * Function to generate cont_wave_tx_confirm that must be sent to
 * host application via serial interface.
 * Called by Performance application as reply to the cont_wave_tx_req
 *
 * \param status            Result for cont_wave_tx_req
 * \param start_stop_param  Parameter to start or stop cont tx
 * \param tx_mode           Indicates CW or PRBS
 */
void usr_cont_wave_tx_confirm(trx_id_t trx, uint8_t status,
		bool start_stop_param, uint8_t tx_mode);

/**
 * Function to generate usr_register_read_confirm that must be sent to
 * host application via serial interface.
 * Called by Performance application as reply to the usr_register_read_req
 *
 * \param status    Result for usr_register_read_req
 * \param reg_addr  Register address that has been read
 * \param reg_val   Register value
 */
void usr_register_read_confirm(trx_id_t trx, uint8_t status, uint16_t reg_addr,
		uint8_t reg_val);

/**
 * Function to generate usr_register_write_confirm that must be sent to
 * host application via serial interface.
 * Called by Performance application as reply to the usr_register_write_req
 *
 * \param status    Result for usr_register_write_req
 * \param reg_addr  Register address that has been set
 * \param reg_val   Register value
 */
void usr_register_write_confirm(trx_id_t trx, uint8_t status, uint16_t reg_addr,
		uint8_t reg_val);

/**
 * Function to generate usr_register_dump_confirm that must be sent to
 * host application via serial interface.
 * Called by Performance application as reply to the usr_register_dump_req
 *
 * \param status         Result for usr_register_dump_req
 * \param start_reg_addr Start Register address that has to be read
 * \param end_reg_addr   End Register address that has to be read
 * \param *reg_val       Arrray of Register values
 */
void usr_register_dump_confirm(trx_id_t trx, uint8_t status,
		uint16_t start_reg_addr, uint16_t end_reg_addr,
		uint8_t *reg_val);

/**
 * Function to generate Disconnect Confirm frame that must be sent to
 * host application via serial interface.
 * Called by Performance application as confirmation after disconnecting from
 * the
 * peer node, if exists
 * \param status           Result for the Peer Disconnect Req
 *
 * \return void
 */
void usr_peer_disconnect_confirm(trx_id_t trx, uint8_t status);

/**
 * Function to generate Set default config Confirm frame that must be sent to
 * host application via serial interface.
 * Called by Performance application as confirmation after setting the all
 * configurable paramters on source and peer node, if exists
 * \param status           Result for the set default config req
 *
 * \param default_trx config params    structure of configurable
 *                                     paramters with default values
 * \return void
 */
void usr_set_default_config_confirm(trx_id_t trx, uint8_t status,
		trx_config_params_t *default_trx_config_params);

/**
 * Function to generate Get Current config Confirm frame that must be sent to
 * host application via serial interface.
 * Called by Performance application as confirmation after getting the all
 * configurable paramters on source and peer node, if exists
 * \param status           Result for the set default config req
 *
 * \param curr_trx config params    structure of configurable
 *                                     paramters with current values
 * \return void
 */
void usr_get_current_config_confirm(trx_id_t trx, uint8_t status,
		trx_config_params_t *curr_trx_conf_params);

/**
 * \brief Initiates the Range test procedure
 */
void initiate_range_test(trx_id_t trx);

/** Function to  Stop the  range Test in PER Mode */
void stop_range_test(trx_id_t trx);

/**
 * \brief Function to send  the Received Range Test Response frame to the Host
 * application
 * \param frame Pointer to the actual frame Received
 * \param lqi_h LQI of the received response calculated at host
 * \param ed_h ED value  of the received response calculated at host
 * \param lqi_r LQI of the sent range test packet calculated at receptor
 * \param ed_r ED value  of the sent range test packet calculated at receptor
 */
void usr_range_test_beacon_rsp(trx_id_t trx, frame_info_t *frame, uint8_t lqi_h,
		int8_t ed_h,
		uint8_t lqi_r, int8_t ed_r);

/**
 * \brief Function to send  the Marker Indication frame to the Host application
 * \param frame Pointer to the actual marker frame Received
 * \param lqi LQI of the received marker packet
 * \param ed_value ED value  of the received marker packet
 */
void usr_range_test_marker_ind(trx_id_t trx, frame_info_t *frame, uint8_t lqi,
		int8_t ed_value);

/**
 * Function to generate Range Test Start confirmation frame that must be sent to
 * host application via serial interface.
 * Called by Performance application as confirmation for range_test_start_req
 * request
 * \param status      Result for requested range_test_start_req
 *
 * \return void
 */
void usr_range_test_start_confirm(trx_id_t trx, uint8_t status);

/**
 * Function to generate Range Test Stop confirmation frame that must be sent to
 * host application via serial interface.
 * Called by Performance application as confirmation for range_test_start_req
 * request
 * \param status      Result for requested range_test_stop_req
 *
 * \return void
 */
void usr_range_test_stop_confirm(trx_id_t trx, uint8_t status);

/**
 * \brief Function to send  the transmitted frame to the Host application
 * \param frame Pointer to the actual frame transmitted
 */
void usr_range_test_beacon_tx(trx_id_t trx, frame_info_t *frame);

/**
 * \brief The reverse_float is used for reversing a float variable for
 * supporting BIG ENDIAN systems
 * \param float_val Float variable to be reversed
 */
float reverse_float( const float float_val );

/* ! \} */
/* ! \} */
#endif
