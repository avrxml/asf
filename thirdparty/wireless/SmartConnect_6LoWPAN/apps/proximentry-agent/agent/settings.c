/*******************************************************************************
 *
 * Copyright (c) 2014 Proximetry Inc. All rights reserved.
 * Authors: Proximetry
 * All Rights Reserved
 *
 ******************************************************************************/
/**
 * @file settings.c
 * @brief
 * */

#include <ctype.h>
#include <limits.h>
#include <stddef.h>
#include <string.h>
#include <asf.h>
#include "common.h"
#include "misc.h"
#include "settings.h"
#include "settings_eeprom.h"
#include "conf_agent_app.h"
#include "contiki-conf.h"

static bool save_settings_f = false;
#if 0
static bool save_settings(void);
#endif

static inline uint32_t ip_int(uint8_t a, uint8_t b, uint8_t c, uint8_t d)
{
	union { uint32_t i; uint8_t ip[4]; } ip_int = {.ip = {a, b, c, d}};
	return ip_int.i;
}

/**
 * @brief Reads IPv4 address from string
 *
 * @param[in] str   address as string
 *
 * @return address as a series of 4 bytes in host endianness
 */
uint32_t read_ip(const char *str)
{
	const char *ip0 = str;
	const char *ip1 = strchr(ip0, '.');
	if (ip1++ == NULL)
		return 0;
	const char *ip2 = strchr(ip1, '.');
	if (ip2++ == NULL)
		return 0;
	const char *ip3 = strchr(ip2, '.');
	if (ip3++ == NULL)
		return 0;
	return ip_int(atoi(ip0), atoi(ip1), atoi(ip2), atoi(ip3));
}

/**
 * @brief Checks that Agent settings are within permitted ranges
 *
 * @param[in] settings   settings to be validated
 *
 * @return true when the settings are valid, false otherwise
 */
static bool validate_settings(const agent_settings_t *settings)
{
	bool ret = true;
	// server settings
	if (settings->server_ip == 0)
	{
		LOG("Invalid server ip" EOL);
		ret = false;
	}
	if (settings->server_port == 0)
	{
		LOG("Invalid server port" EOL);
		ret = false;
	}
	// other settings
	if (settings->led0_state != 0 && settings->led0_state != 1 && settings->led0_state != 2)
	{
		LOG("Invalid led0 state" EOL);
		ret = false;
	}
	if (strlen(settings->activation_code) != 74 && strlen(settings->activation_code) != 72)
	{
		LOG("Invalid activation code" EOL);
		ret = false;
	}
	if (settings->sync_interval < AGENT_MIN_SYNC_INTERVAL || settings->sync_interval > AGENT_MAX_SYNC_INTERVAL)
	{
		LOG("Invalid sync interval" EOL);
		ret = false;
	}
	// sync_interval is in seconds, AGENT_STATS_PRE_INTERVAL in milliseconds
	if (settings->sync_interval * 1000 < AGENT_STATS_PRE_INTERVAL)
	{
		LOG("Sync interval < AGENT_STATS_PRE_INTERVAL" EOL);
		ret = false;
	}

	return ret;
}


static agent_settings_t global_settings = {0};
static bool global_settings_loaded = false;

/**
 * @brief Get default settings
 *
 */
static void get_default_settings(void)
{
 	global_settings.server_ip = read_ip(DEF_SERVER_IP);
 	global_settings.server_port = DEF_SERVER_PORT;
 	global_settings.led0_state = DEF_LED0_STATE;
 	strncpy( global_settings.activation_code,DEF_ACTIVATION_CODE,sizeof(global_settings.activation_code));
 	global_settings.sync_interval = DEF_SYNC_INTERVAL;
 	strncpy (global_settings.device_name,DEF_DEVICE_NAME,sizeof(global_settings.device_name));
}


/**
 * @brief Get default settings
 *
 */
static void load_default_settings(void)
{
	LOG("INFO: Loading default settings"EOL);
	get_default_settings();
}

/**
 * @brief Dump settings
 *
 */
void dump_settings(agent_settings_t *set)
{
	LOG(EOL"Settings: "EOL);
 	LOG("   %s:\t\t"PRINT_IP_FORMAT EOL,KEY_SERVER_IP ,PRINT_IP_VALUES((uint8_t*)&set->server_ip));
 	LOG("   %s:\t\t%u"EOL,KEY_SERVER_PORT  ,set->server_port);
	LOG("   %s:\t\t%d"EOL,KEY_LED0 ,set->led0_state );
	LOG("   %s:\t%s"EOL,KEY_ACTIVATION_CODE  ,set->activation_code);
 	LOG("   %s:\t%d"EOL,KEY_SYNC_INTERVAL  ,(int)set->sync_interval);
 	LOG("   %s:\t\t%s"EOL,KEY_DEVICE_NAME  ,set->device_name);
}

/**
 * @brief Loads Agent config from the filesystem
 *
 * @param[out] settings   The read config
 *
 * @return true on success, false otherwise
 */
static bool load_settings(agent_settings_t *settings)
{
	load_default_settings();
#if (USE_EEPROM == 1)
 	configure_eeprom();

 dump_settings_form_eeprom();

	load_settings_from_eeprom(settings);
#endif
	dump_settings(&global_settings);

	if (!validate_settings(settings))
	{
		LOG("ERROR: settings are invalid - program stopped");
		triple_blink();
		while(1)
			;
	}

	return validate_settings(settings);
}

/**
 * @brief Returns pointer to the current settings
 *
 * @return current settings
 */
agent_settings_t *get_settings(void)
{
	if (!global_settings_loaded)
	{
		global_settings_loaded = load_settings(&global_settings);
	}
	return global_settings_loaded ? &global_settings : NULL;
}
#if 0
/**
 * @brief Stores Agent config
 *
 * @return true on success, false otherwise
 *
 */
static bool save_settings(void)
{
	agent_settings_t *settings = &global_settings;
	if (!validate_settings(settings))
	{
		printf("ERROR: Validate settings failed before save  settings"EOL);
		return false;
	}

	return !store_settings_to_eeprom(settings);
}
#endif
/**
 * @brief Save setting request
 */
void save_settings_request(void)
{
	save_settings_f = true;
}

/**
 * @brief Setting thread
 */
void settings_service(void)
{
	if (save_settings_f)
	{
		save_settings_f = false;
#if (USE_EEPROM == 1)
		save_settings();
		dump_settings_form_eeprom();
#else
	LOG("WARNING: This version is not eeprom enabled - settings will not be saved");
#endif

	}
}

