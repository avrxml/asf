/*******************************************************************************
 *
 * AMP Agent example implementation
 *
 * Copyright (c) 2015 Proximetry, Inc.
 * All Rights Reserved
 *
 ******************************************************************************/

/**
 * @file prox_agent.c
 * @brief AMP Agent
 * */

#include <asf.h>
#include "common.h"
#include "amp_agent.h"
/* #include "prox_agent.h" */
#include "settings.h"
#include "misc.h"

#include <contiki-clock.h>
#include <ip64-addr.h>
//#include <rf233.h>
#include <sicslowpan.h>
#include <udp-socket.h>
#include <uip.h>
#include <uip-ds6.h>
//#include <rf233.h>

#include "conf_agent_app.h"
#include "prox_agent.h"
#include "sam0_sensors.h"
#include "diagnostic_button.h"

/* Static variables */


/**
 * Proximetry agent residing on the device is uniquely identified by device type (model) and software version.
 * It also has fixed lists of configuration parameters, statistics and alarms, which are reported to
 * and managed by the AirSync cloud.
 *
 * The definition of each type of device must be reflected in capability file located in the AirSync cloud.
 * Capability file is an XML file, which describes the Agent functionality in terms of statistics, parameters and alarms
 * supported by the specific device.
 * The name of capability file has the following form: metercap_<DEVICE_MODEL>_<CAPFILE_VER>.xml
 *
 * NOTE:
 * In case the user wants to add/remove configuration parameter, statistics or alarm,
 * the change should be made both in the agent code (see definitions below) and in the capability file.
 * The detailed description how to proceed is placed in the documentation for the Agent library.
 * */

#if !defined(DEVICE_MODEL) || !defined (CAPFILE_VER)
	#error "DEVICE_MODEL and CAPFILE_VER must be defined"
#endif


/**
 * AirSync Agent must by supplied with structures holding the arrays of parameters, statistics and alarms.
 * Each of those (parameters/statistics/alarms) is identified by its ID.
 * Below are the lists of all IDs and lists of defined parameters, statistics and alarms.
 * */

/* Parameters IDs */
#define PROX_AGENT_PARAM_ID__DEVICE_ID			1001		/* This is the mcu description vector not the agent device id */
#define PROX_AGENT_PARAM_ID__SERIAL_NO			1002
#define PROX_AGENT_PARAM_ID__802154_EUI64		1011
#define PROX_AGENT_PARAM_ID__802154_RF_CHANNEL	1012
#define PROX_AGENT_PARAM_ID__802154_NODE_ID		1013
#define PROX_AGENT_PARAM_ID__802154_PAN_ID		1014
#define PROX_AGENT_PARAM_ID__802154_AES_ENC		1015
#define PROX_AGENT_PARAM_ID__6LOWPAN_LL_IPV6	1016
#define PROX_AGENT_PARAM_ID__6LOWPAN_GLB_IPV6	1017
#define PROX_AGENT_PARAM_ID__LED_STATE			1020
#define PROX_AGENT_PARAM_ID__TIME_INTERVAL		1030
#define PROX_AGENT_PARAM_ID__AGENT_VERSION		1031

/* Statistics IDs */
#define PROX_AGENT_STAT_ID__TEMPERATURE			201
#define PROX_AGENT_STAT_ID__SUPPLY_VOL			202
#define PROX_AGENT_STAT_ID__ANALOG_INP			203
#define PROX_AGENT_STAT_ID__KEYSTATE			204
#define PROX_AGENT_STAT_ID__RSSI				205

/* Alarms IDs */
#define PROX_AGENT_ALARM_ID_BUTTON			  1


/**
 * Design of the Agent assumes no dynamic memory allocation, so all STRING configuration parameters must be statically allocated by the user.
 * Non-string parameters, statistics and alarms are allocated in place during their definitions in struct amp_param_t params.
 * */

#define PANID_STR_LEN			10
#define NODEID_STR_LEN			10
#define RF_CHANNEL_STR_LEN		3
#define DEVICE_ID_STR_LEN		100
#define SERIALNO_STR_LEN		100
#define AGENT_VER_STR_LEN		sizeof(APP_VER)

/* Statically allocated memory for agent string parameters */
static struct str_agent_params_s {
	char device_id[DEVICE_ID_STR_LEN];
	char serial_no[SERIALNO_STR_LEN];
	char _802154_eui64[EUI64_STR_LEN];
	char _802154_rf_channel[RF_CHANNEL_STR_LEN];
	char _802154_node_id[NODEID_STR_LEN];
	char _802154_pan_id[PANID_STR_LEN];
	char _802154_ll_ipv6[IPV6_STR_LEN];
	char _802154_glb_ipv6[IPV6_STR_LEN];
	char agent_version[AGENT_VER_STR_LEN];
} str_agent_params;


static uint8_t get_key_stat(void)
{
	//TODO
	return port_pin_get_input_level(BUTTON_0_PIN) == BUTTON_0_ACTIVE ? 0 : 1;
}

/**
 * @brief Definitions of agent parameters
 *
 * To define a parameter, a param_id and proper AMP_AGENT_<TYPE>_PARAM_DEF macro should be used.
 * If the parameter type is STRING, additional statically allocated memory reference for this parameter must be given.
 * */
static amp_param_t params[] =
{
	AMP_AGENT_UINT8_PARAM_DEF(PROX_AGENT_PARAM_ID__LED_STATE),
	AMP_AGENT_STRING_PARAM_DEF(PROX_AGENT_PARAM_ID__DEVICE_ID,str_agent_params.device_id),
	AMP_AGENT_STRING_PARAM_DEF(PROX_AGENT_PARAM_ID__SERIAL_NO,str_agent_params.serial_no),
	AMP_AGENT_INT32_PARAM_DEF(PROX_AGENT_PARAM_ID__TIME_INTERVAL),
	AMP_AGENT_STRING_PARAM_DEF(PROX_AGENT_PARAM_ID__AGENT_VERSION,str_agent_params.agent_version),

	AMP_AGENT_STRING_PARAM_DEF(PROX_AGENT_PARAM_ID__802154_EUI64,str_agent_params._802154_eui64),
	AMP_AGENT_STRING_PARAM_DEF(PROX_AGENT_PARAM_ID__802154_RF_CHANNEL,str_agent_params._802154_rf_channel),
	AMP_AGENT_STRING_PARAM_DEF(PROX_AGENT_PARAM_ID__802154_NODE_ID,str_agent_params._802154_node_id),
	AMP_AGENT_STRING_PARAM_DEF(PROX_AGENT_PARAM_ID__802154_PAN_ID,str_agent_params._802154_pan_id),
	AMP_AGENT_UINT8_PARAM_DEF(PROX_AGENT_PARAM_ID__802154_AES_ENC),
	AMP_AGENT_STRING_PARAM_DEF(PROX_AGENT_PARAM_ID__6LOWPAN_LL_IPV6,str_agent_params._802154_ll_ipv6),
	AMP_AGENT_STRING_PARAM_DEF(PROX_AGENT_PARAM_ID__6LOWPAN_GLB_IPV6,str_agent_params._802154_glb_ipv6),

};

/**
 * @brief Definitions of agent statistics
 *
 * To define a statistic, a stat_id and proper AMP_AGENT_<TYPE>_STAT_DEF macro should be used.
 * */
static amp_stat_t stats[] =
{
	AMP_AGENT_FLOAT_STAT_DEF(PROX_AGENT_STAT_ID__TEMPERATURE),
	AMP_AGENT_FLOAT_STAT_DEF(PROX_AGENT_STAT_ID__SUPPLY_VOL),
	AMP_AGENT_FLOAT_STAT_DEF(PROX_AGENT_STAT_ID__ANALOG_INP),
	AMP_AGENT_UINT8_STAT_DEF(PROX_AGENT_STAT_ID__KEYSTATE),
	AMP_AGENT_INT8_STAT_DEF(PROX_AGENT_STAT_ID__RSSI),
};

/**
 * @brief Definitions of agent alarms
 *
 * To define an alarm, an alarm_id and AMP_AGENT_ALARM_DEF macro should be used.
 * */
static amp_alarm_t alarms[] =
{
	AMP_AGENT_ALARM_DEF(PROX_AGENT_ALARM_ID_BUTTON)
};


/**
 * Internal storage for configuration parameters, statistics and alarms.
 *
 * */
static system_data_t agent_data =
{
	.params = params,
	.params_num = ARRAY_ELEMS(params),
	.stats = stats,
	.stats_num = ARRAY_ELEMS(stats),
	.alarms = alarms,
	.alarms_num = ARRAY_ELEMS(alarms)
};

/**
 * @brief System information utilized by the agent.
 * */
static system_info_t sys_info;

struct udp_socket agent_socket;

PROCESS(prox_agent_process, "Agent");


/********************************************************************************************************************************/
/*                                       agent callbacks utilized by agent library                                              */
/********************************************************************************************************************************/

/**
 * @brief Supply the agent with time passage indicator (1ms counter)
 *
 * This routine is invoked by agent library
 * Based on this information the agent measures the time intervals
 */
uint64_t amp_agent_now(void)
{
	return clock_seconds() * 1000;
}

/**
 * @brief Send message routine
 *
 * This routine is invoked by agent library
 * Agent utilizes this routine to send message
 */
int amp_agent_send_msg(uint8_t *data, uint16_t size)
{
	uip_ip4addr_t addr4 = *(uip_ip4addr_t*)&get_settings()->server_ip;
	/* printf(" SENDING TO "PRINT_IP_FORMAT EOL,PRINT_IP_VALUES((uint8_t*)&addr4)); */
	uip_ip6addr_t addr6;
	ip64_addr_4to6(&addr4, &addr6);
	return udp_socket_sendto(&agent_socket, (void*)data, size, &addr6, get_settings()->server_port);
}

/**
 * @brief Receive message routine
 *
 * This routine is invoked by the system if any data for the agent is received
 */
static void agent_udp_callback(struct udp_socket *c,
	void *ptr,
	const uip_ipaddr_t *source_addr,
	uint16_t source_port,
	const uip_ipaddr_t *dest_addr,
	uint16_t dest_port,
	const uint8_t *data,
	uint16_t datalen)
{
	UNUSED(c);
	UNUSED(ptr);
	UNUSED(source_addr);
	UNUSED(source_port);
	UNUSED(dest_addr);
	UNUSED(dest_port);
	/* amp_agent_write_stat_i8(STAT_RSSI, sicslowpan_get_last_rssi()); */
	amp_agent_handle_msg((uint8_t*)data, datalen);
}

/**
 * @brief Agent change parameters indicator
 *
 * This routine is invoked by the agent library if given parameter has been changed.
 * Here is the place for adding the logic/action when the parameters is changed.
 */
void amp_agent_param_changed(amp_param_t *param)
{
	if (param->id == PROX_AGENT_PARAM_ID__TIME_INTERVAL)
	{
		uint32_t sync_interval = param->value.i32;
		sys_info.sync_msg_interval = sync_interval;
		get_settings()->sync_interval = sync_interval;
		save_settings_request();
	}
	else if (param->id == PROX_AGENT_PARAM_ID__LED_STATE)
	{
		get_settings()->led0_state = param->value.i8;
		save_settings_request();
	}
}

/**
 * @brief Set statistic in agent
 *
 * This routine is invoked by the agent library
 */
void amp_agent_stats_update(void)
{
	sensor_result_t temp;
	sensor_result_t svol;
	sensor_result_t ainp;
	get_sensor_value(TEMPERATURE,&temp);
	get_sensor_value(IO_SUPPLY,&svol);
	get_sensor_value(ANALOG_PIN,&ainp);
	uint32_t stat_key_state = (get_key_stat() == BUTTON_0_ACTIVE);
	int8_t rssi  =  sicslowpan_get_last_rssi();

	amp_agent_write_stat_float(PROX_AGENT_STAT_ID__TEMPERATURE, temp.sensor_value);
	amp_agent_write_stat_float(PROX_AGENT_STAT_ID__SUPPLY_VOL, svol.sensor_value);
	amp_agent_write_stat_float(PROX_AGENT_STAT_ID__ANALOG_INP, ainp.sensor_value);
	amp_agent_write_stat_u8(PROX_AGENT_STAT_ID__KEYSTATE, stat_key_state);
	amp_agent_write_stat_i8(PROX_AGENT_STAT_ID__RSSI, rssi);

	LOG( "PROX_AGENT_STAT_ID__TEMPERATURE: 0x%x" EOL, temp.adc_value);
	LOG( "PROX_AGENT_STAT_ID__SUPPLY_VOL: 0x%x" EOL,svol.adc_value);
	LOG( "PROX_AGENT_STAT_ID__ANALOG_INP: 0x%x" EOL,ainp.adc_value);
	LOG( "PROX_AGENT_STAT_ID__KEYSTATE: %d" EOL,(int)stat_key_state);
	LOG( "PROX_AGENT_STAT_ID__RSSI: %d" EOL, (int)rssi);
}



/********************************************************************************************************************************/
/*                                       agent misc routines                                                                    */
/********************************************************************************************************************************/

/**
 * @brief Sets parameter values in agent
 *
 * Fills the agent parameters structures based on the system information.
 * L3 related parameters are updated by update_net_ifc_info().
 */
static int update_agent_params(void)
{

	char _eui64[EUI64_STR_LEN] = {0};
	char _rf_channel[RF_CHANNEL_STR_LEN] =  {0};
	char _node_id[NODEID_STR_LEN] = {0};
	char _pan_id[PANID_STR_LEN] = {0};
	uint8_t _aes_enc;
	char _6lowpan_ll_ipv6[IPV6_STR_LEN] = {0};
	char _6lowpan_glb_ipv6[IPV6_STR_LEN] = {0};

	/* writing parameters value to prox agent lib */
	amp_agent_write_param_string(PROX_AGENT_PARAM_ID__DEVICE_ID, get_hw_sw_info()->device_id);
	amp_agent_write_param_string(PROX_AGENT_PARAM_ID__SERIAL_NO, get_hw_sw_info()->device_serial_no);
	amp_agent_write_param_string(PROX_AGENT_PARAM_ID__AGENT_VERSION,APP_VER);
	amp_agent_write_param_i32(PROX_AGENT_PARAM_ID__TIME_INTERVAL, get_settings()->sync_interval);
	uint32_t led0_state = get_settings()->led0_state;
	amp_agent_write_param_u8(PROX_AGENT_PARAM_ID__LED_STATE,led0_state);

	eui64_to_str(_eui64,eui64);
	snprintf(_rf_channel,sizeof(_rf_channel),"%d",rf_get_channel());
	snprintf(_node_id,sizeof(_node_id),"%X",get_nodeid());
	snprintf(_pan_id,sizeof(_pan_id),"%X", IEEE802154_CONF_PANID);
	_aes_enc = 0;
	ipv6_addr_to_str(_6lowpan_glb_ipv6, &uip_ds6_get_global(-1)->ipaddr);
	ipv6_addr_to_str(_6lowpan_ll_ipv6, &uip_ds6_get_link_local(-1)->ipaddr);

	/* writing parameters value to prox agent lib */
	amp_agent_write_param_string(PROX_AGENT_PARAM_ID__802154_EUI64,_eui64);
	amp_agent_write_param_string(PROX_AGENT_PARAM_ID__802154_RF_CHANNEL,_rf_channel);
	amp_agent_write_param_string(PROX_AGENT_PARAM_ID__802154_NODE_ID,_node_id);
	amp_agent_write_param_string(PROX_AGENT_PARAM_ID__802154_PAN_ID,_pan_id);
	amp_agent_write_param_u8(PROX_AGENT_PARAM_ID__802154_AES_ENC,_aes_enc);
	amp_agent_write_param_string(PROX_AGENT_PARAM_ID__6LOWPAN_LL_IPV6,_6lowpan_ll_ipv6);
	amp_agent_write_param_string(PROX_AGENT_PARAM_ID__6LOWPAN_GLB_IPV6,_6lowpan_glb_ipv6);

	return 0;
}

/**
 * @brief Updates alarm states in agent
 *
 * This routine is invoked by the agent thread for updating the alarm states
 * based on the system state.
 */
static void update_agent_alarms(void)
{

	if (diagnostic_button_service_is_active())
		return;

	if (BUTTON_0_ACTIVE == get_key_stat())
		amp_agent_set_alarm(PROX_AGENT_ALARM_ID_BUTTON);
	else
		amp_agent_clear_alarm(PROX_AGENT_ALARM_ID_BUTTON);
}


/**
 * @brief Prints agent parameters values - logging purpose
 */
static void dump_agent_prams(void)
{
	char device_id[DEVICE_ID_STR_LEN];
	char agent_ver[AGENT_VER_STR_LEN];
	char serial_no[SERIALNO_STR_LEN];
	uint8_t led_state;
	int32_t sync_interval;
	char _eui64[EUI64_STR_LEN];
	char _rf_channel[RF_CHANNEL_STR_LEN];
	char _node_id[NODEID_STR_LEN];
	char _pan_id[PANID_STR_LEN];
	uint8_t aes_enc;
	char _6lowpan_ll_ipv6[IPV6_STR_LEN];
	char _6lowpan_glb_ipv6[IPV6_STR_LEN];


	LOG("--- Agent params values ---"EOL);

	amp_agent_read_param_string(PROX_AGENT_PARAM_ID__DEVICE_ID,device_id);
	amp_agent_read_param_string(PROX_AGENT_PARAM_ID__SERIAL_NO,serial_no);
	amp_agent_read_param_i32(PROX_AGENT_PARAM_ID__TIME_INTERVAL,&sync_interval);
	amp_agent_read_param_u8(PROX_AGENT_PARAM_ID__LED_STATE,&led_state);
	amp_agent_read_param_string(PROX_AGENT_PARAM_ID__AGENT_VERSION,agent_ver);

	LOG("   id:%4d agent version:     %s"EOL,PROX_AGENT_PARAM_ID__AGENT_VERSION,agent_ver);
	LOG("   id:%4d device_id:         %s"EOL,PROX_AGENT_PARAM_ID__DEVICE_ID,device_id);
	LOG("   id:%4d serial_no:         %s"EOL,PROX_AGENT_PARAM_ID__SERIAL_NO,serial_no);
	LOG("   id:%4d sync interval:     %d"EOL,PROX_AGENT_PARAM_ID__TIME_INTERVAL,(int)sync_interval);
	LOG("   id:%4d led state:         %d"EOL,PROX_AGENT_PARAM_ID__LED_STATE,led_state);

	amp_agent_read_param_string(PROX_AGENT_PARAM_ID__802154_EUI64,_eui64);
	amp_agent_read_param_string(PROX_AGENT_PARAM_ID__802154_RF_CHANNEL,_rf_channel);
	amp_agent_read_param_string(PROX_AGENT_PARAM_ID__802154_NODE_ID,_node_id);
	amp_agent_read_param_string(PROX_AGENT_PARAM_ID__802154_PAN_ID,_pan_id);
	amp_agent_read_param_string(PROX_AGENT_PARAM_ID__6LOWPAN_LL_IPV6,_6lowpan_ll_ipv6);
	amp_agent_read_param_string(PROX_AGENT_PARAM_ID__6LOWPAN_GLB_IPV6,_6lowpan_glb_ipv6);
	amp_agent_read_param_u8(PROX_AGENT_PARAM_ID__802154_AES_ENC,&aes_enc);

	LOG("   id:%4d 802154 eui64:      %s"EOL,PROX_AGENT_PARAM_ID__802154_EUI64,_eui64);
	LOG("   id:%4d 802154 rf_channel: %s"EOL,PROX_AGENT_PARAM_ID__802154_RF_CHANNEL,_rf_channel);
	LOG("   id:%4d 802154 node_id:    %s"EOL,PROX_AGENT_PARAM_ID__802154_NODE_ID,_node_id);
	LOG("   id:%4d 802154 pan_id:     %s"EOL,PROX_AGENT_PARAM_ID__802154_PAN_ID,_pan_id);
	LOG("   id:%4d 802154 aes_enc:    %s"EOL,PROX_AGENT_PARAM_ID__802154_AES_ENC,aes_enc ? "enable" : "disable");
	LOG("   id:%4d 6loWPan ll_ipv6:   %s"EOL,PROX_AGENT_PARAM_ID__6LOWPAN_LL_IPV6,_6lowpan_ll_ipv6);
	LOG("   id:%4d 6loWPan glb_ipv6:  %s"EOL,PROX_AGENT_PARAM_ID__6LOWPAN_GLB_IPV6,_6lowpan_glb_ipv6);
}

/**
 * @brief Prints agent parameters values - logging purpose
 */
static void dump_sys_info(void)
{
	LOG("--- Agent system info ---"EOL);
	LOG("   Device id:         %s"EOL,sys_info.device_id);
	LOG("   Device name:       %s"EOL,sys_info.device_name);
	LOG("   Model id:          0x%X"EOL,(int)sys_info.model_id);
	LOG("   Software ver:      0x%X"EOL,(int)sys_info.software_ver);
	LOG("   Sync.msg.interval  %d"EOL,sys_info.sync_msg_interval);
	LOG("   Activation code:   %s"EOL,sys_info.activation_code);
}

/********************************************************************************************************************************/
/*                                       agent main routines                                                                    */
/********************************************************************************************************************************/

/**
 * @brief Agent process routine
 *
 * - allocates network resources for the agent,
 * - fills the agent system info structure,
 * - initializes the agent with sys_info and data structures (params,stats,alarms),
 * - updates the agent parameters values
 */
PROCESS_THREAD(prox_agent_process, ev, data)
{

	UNUSED(data);
	UNUSED(ev);
	PROCESS_BEGIN();

	LOG("agent_start" EOL);

	agent_settings_t *settings = get_settings();

	/* allocate network resources */
	udp_socket_register(&agent_socket, NULL, agent_udp_callback);
	udp_socket_bind(&agent_socket, 1237);

	/* fill the agent system info structure */
	const device_hw_info_t *h = get_hw_sw_info();
	sys_info.device_id = h->device_serial_no;
	sys_info.device_name = get_device_name();
	sys_info.model_id = DEVICE_MODEL;
	sys_info.software_ver = CAPFILE_VER;
	sys_info.sync_msg_interval = settings->sync_interval;
	sys_info.activation_code = settings->activation_code;
	sys_info.sync_msg_backoff = -1;

	/* initialize agent */
	amp_agent_init(&sys_info, &agent_data);

	/* update the agent parameters values */
	update_agent_params();

	dump_agent_prams();
	dump_sys_info();
	uint8_t *print_ip = (uint8_t *)&settings->server_ip;
	LOG("Using Proximetry Server: %u.%u.%u.%u:%u" EOL,
		print_ip[0], print_ip[1], print_ip[2], print_ip[3], settings->server_port);


	while(1)
	{
		update_agent_alarms();
		amp_agent_sync_task();
		settings_service();
		PROCESS_PAUSE();
	}

	PROCESS_END();
}

