/**
 * \file
 *
 * \brief Provisioning.
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

#ifndef WIFIPROV_TASK_H_INCLUDED
#define WIFIPROV_TASK_H_INCLUDED

#include "cmn_defs.h"

#define MAX_WIPROVTASK_AP_NUM         15
#define MAX_WIPROVTASK_SSID_LENGTH    32
#define MAX_WIPROVTASK_PASS_LENGTH    63

/*
 * msgs from APP to wifiprov
 *
 *
 */
/// Messages for WiFi Provisioning Profile
enum
{
	WIFIPROV_CONFIGURE_REQ = 0xDC00,
	WIFIPROV_CONFIGURE_CFM,
	/// Add WIFIPROV into the database
	WIFIPROV_CREATE_DB_REQ,
	/// Inform APP about DB creation status
	WIFIPROV_CREATE_DB_CFM,
	/// Start provisioning
	WIFIPROV_START_CMD,
	/// Disable provisioning (eg timeout, bonding fail or bad provisioning details)
	WIFIPROV_DISABLE_CMD,
	/// Provisioning complete (success or fail)
	WIFIPROV_COMPLETE_IND,
	/// Receiving scanlist from Wifi side
	WIFIPROV_SCANLIST_IND,
	/// Inform APP about_connection_state changes
	WIFIPROV_CONNECTION_STATE_CHANGE_IND,
	///  Inform App about Scan Mode changes
	WIFIPROV_SCAN_MODE_CHANGE_IND,
};

// When scan mode is used to send indication from App to the WiFi Prov profile,
// WIFIPROV_SCANMODE_SCANNING indicates Scanning is in progress
// WIFIPROV_SCANMODE_DONE indicates scanning is completed - note that this 
//	      				  indication should only be used when no scan results were obtained during the scan
// These result in the database being updated
// When scan mode is used to send indication from WiFi Prov profile to the App,
// WIFIPROV_SCANMODE_SCANNING request to start WiFi scanning and supply scan results
enum
{
	WIFIPROV_SCANMODE_SCANNING = 1,
	WIFIPROV_SCANMODE_DONE	// Scanning is indicated as 
};
typedef struct
{
    /// Scan Mode
    uint8_t scanmode;
}at_ble_wifiprov_scan_mode_change_ind_t;

//store the scan list and then later populate when creating the database
typedef struct _scanitem {
	uint8_t sec_type;
	uint8_t rssi;
	uint8_t ssid[MAX_WIPROVTASK_SSID_LENGTH];
} scanitem;

struct wifiprov_scanlist_ind
{
	uint8_t num_valid;
	scanitem scandetails[MAX_WIPROVTASK_AP_NUM];
};

at_ble_status_t wifiprov_scan_mode_change_ind_send(uint8_t scanmode);
at_ble_status_t wifiprov_scan_list_ind_send(struct wifiprov_scanlist_ind *param);

typedef struct 
{
	uint8_t status;
	uint8_t sec_type;
	uint8_t ssid_length;
	uint8_t ssid[MAX_WIPROVTASK_SSID_LENGTH];
	uint8_t passphrase_length;
	uint8_t passphrase[MAX_WIPROVTASK_PASS_LENGTH];
} at_ble_wifiprov_complete_ind;

void wifiprov_complete_ind_handler(uint8_t *data, at_ble_wifiprov_complete_ind *param);

enum
{
    WIFIPROV_CON_STATE_DISCONNECTED,
    WIFIPROV_CON_STATE_CONNECTING,
    WIFIPROV_CON_STATE_CONNECTED
};
at_ble_status_t wifiprov_wifi_con_update(uint8_t con_state);

at_ble_status_t wifiprov_configure_provisioning(void);
at_ble_status_t wifiprov_create_db(void);
at_ble_status_t wifiprov_start(uint8_t *pin, uint8_t len);
at_ble_status_t wifiprov_disable(void);

// Internal API
void wifiprov_scan_mode_change_ind_handler(uint8_t *data, at_ble_wifiprov_scan_mode_change_ind_t *param);

#endif /* WIFIPROV_TASK_H_INCLUDED */
