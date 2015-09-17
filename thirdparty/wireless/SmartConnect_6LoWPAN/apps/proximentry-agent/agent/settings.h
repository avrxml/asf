/*******************************************************************************
 *
 * Copyright (c) 2015 Proximetry Inc. All rights reserved.
 * Authors: Proximetry
 * All Rights Reserved
 *
 ******************************************************************************/
/**
 * @file settings.h
 * @brief
 * */

#ifndef AGENT_SETTINGS_H
#define AGENT_SETTINGS_H

#include <stddef.h>
#include <stdint.h>
#include "common.h"


#define MAX_ACTIVATION_CODE_LEN     (100+1)
#define MAX_SSID_LEN                100
#define MAX_PASSWD_LEN              100
#define MAX_DEVICE_NAME_LEN         (32+1)


// server settings
#define KEY_SERVER_IP       "server_ip"
#define KEY_SERVER_PORT     "server_port"
// other settings
#define KEY_LED0            "led0"
#define KEY_ACTIVATION_CODE "activation_code"
#define KEY_SYNC_INTERVAL   "sync_msg_interval"
#define KEY_DEVICE_NAME     "device_name"

// settings
typedef struct agent_settings_s {
	uint32_t server_ip;
	uint16_t server_port;
	uint8_t led0_state;
	char activation_code[MAX_ACTIVATION_CODE_LEN];
	uint32_t sync_interval;
	char device_name[MAX_DEVICE_NAME_LEN];
} agent_settings_t;

agent_settings_t *get_settings(void);
void save_settings_request(void);
void settings_service(void);
void dump_settings(agent_settings_t *s);

uint32_t read_ip(const char *str);
const char *get_sec_type_string(int sec_type);

#define AGENT_FIRST_SYNC_MSG_TIME   1000*10    // in [ms]
#define AGENT_DEFAULT_INTERVAL      10   	  // in [s]
#define AGENT_MIN_SYNC_INTERVAL		1  	  	  // in [s]
#define AGENT_MAX_SYNC_INTERVAL		900  	  // in [s] 15 min
#define AGENT_STATS_PRE_INTERVAL    200       // in [ms] for stats requests preceeding status message

#if (AGENT_DEFAULT_INTERVAL*1000) <= AGENT_STATS_PRE_INTERVAL
#error AGENT_DEFAULT_INTERVAL is too small
#endif

#endif /* MISC_H */
