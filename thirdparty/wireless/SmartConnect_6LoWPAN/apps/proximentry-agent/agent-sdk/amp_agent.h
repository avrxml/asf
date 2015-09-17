/*******************************************************************************
 *
 * AMP Agent Library - programing interface
 *
 * Copyright (c) 2014,2015 Proximetry, Inc.
 * All Rights Reserved
 *
 ******************************************************************************/

/**
 * @file amp_agent.h
 * @brief AMP Agent Library - programing interface
 */

#ifndef __AMP_AGENT_H__
#define __AMP_AGENT_H__


#include <stdbool.h>
#include <stdint.h>


/*----------------------------------------------------------------------------*/
// AMP Agent Library static configuration
#define AMP_CFG_PROTOCOL_VERSION 4  /**< @brief AMP version (2/3/4) */
#define AMP_CFG_MTU 1500            /**< @brief MTU of underlying network protocol */
#define AMP_CFG_LOG_EXTRAS 1        /**< @brief log file name & line number (0=off, 1=on) */
#define AMP_CFG_LOG_EOL "\r\n"        /**< @brief log entry end of line character(s) */


/*----------------------------------------------------------------------------*/
// AMP Agent Library data types
/**
 * @brief Type for storing information about data type
 */
typedef enum
{
	AMP_TYPE_BOOL   = 0,
	AMP_TYPE_INT8   = 1,
	AMP_TYPE_INT16  = 2,
	AMP_TYPE_INT32  = 3,
	AMP_TYPE_UINT8  = 4,
	AMP_TYPE_UINT16 = 5,
	AMP_TYPE_UINT32 = 6,
	AMP_TYPE_FLOAT  = 7,
	AMP_TYPE_STRING = 9

} amp_type_t;

/**
 * @brief Type for storing data of multiple types
 */
typedef union
{
	bool      b;    /**< @brief boolean value */
	int8_t    i8;   /**< @brief signed 8bit integer value */
	int16_t   i16;  /**< @brief signed 16bit integer value */
	int32_t   i32;  /**< @brief signed 32bit integer value */
	uint8_t   ui8;  /**< @brief unsigned 8bit integer value */
	uint16_t  ui16; /**< @brief unsigned 16bit integer value */
	uint32_t  ui32; /**< @brief unsigned 32bit integer value */
	float     f;    /**< @brief float value */
	char     *s;    /**< @brief string value */

} amp_value_t;

/**
 * @brief Abstract type for storing data of any type
 */
typedef struct
{
	uint32_t    id;     /**< @brief data id */
	amp_type_t  type;   /**< @brief data type */
	amp_value_t value;  /**< @brief data value */

} amp_data_t;

/**
 * @brief Type for storing statistics data
 */
typedef amp_data_t amp_stat_t;

/**
 * @brief Type for storing configuration parameters data
 */
typedef amp_data_t amp_param_t;

/**
 * @brief Type for storing alarm state information
 */
typedef enum
{
	AMP_ALARM_STATE_CLEARED = 0,
	AMP_ALARM_STATE_SET     = 1

} amp_alarm_state_t;

/**
 * @brief Type for storing alarms data
 */
typedef struct
{
	uint8_t             id;         /**< @brief alarm id */
	amp_alarm_state_t   state;      /**< @brief alarm state */
	uint8_t             chg_count;  /**< @brief alarm state change counter */

} amp_alarm_t;

/**
 * @brief A structure to represent embedded system information
 */
typedef struct
{
	const char *device_id;         /**< @brief device ID */
#if AMP_CFG_PROTOCOL_VERSION > 3
	const char *device_name;       /**< @brief device name */
#endif
	uint32_t    model_id;          /**< @brief device model ID */
	uint16_t    software_ver;      /**< @brief software version */

	int         sync_msg_interval; /**< @brief sync message interval [s] */
	int         sync_msg_backoff;  /**< @brief sync message backoff [s] */
	const char *activation_code;   /**< @brief activation code */

} system_info_t;

/**
 * @brief A structure to represent embedded system reportable data
 */
typedef struct
{
	amp_param_t    *params;
	uint32_t        params_num;

	amp_stat_t     *stats;
	uint32_t        stats_num;

	amp_alarm_t    *alarms;
	uint32_t        alarms_num;

} system_data_t;


/*----------------------------------------------------------------------------*/
// AMP Agent Library macros
/**
 * @brief Define bool parameter
 * Use this macro to initialize bool params in parameters memory buffer
 * @param[in] _id     param ID, integer literal
 */
#define AMP_AGENT_BOOL_PARAM_DEF(_id) { (_id), AMP_TYPE_BOOL, {.b = false} }

/**
 * @brief Define 8bit signed integer parameter
 * Use this macro to initialize int8 params in parameters memory buffer
 * @param[in] _id     param ID, integer literal
 */
#define AMP_AGENT_INT8_PARAM_DEF(_id) { (_id), AMP_TYPE_INT8, {.i8 = 0} }

/**
 * @brief Define 16bit signed integer parameter
 * Use this macro to initialize int16 params in parameters memory buffer
 * @param[in] _id     param ID, integer literal
 */
#define AMP_AGENT_INT16_PARAM_DEF(_id) { (_id), AMP_TYPE_INT16, {.i16 = 0} }

/**
 * @brief Define 32bit signed integer parameter
 * Use this macro to initialize int32 params in parameters memory buffer
 * @param[in] _id     param ID, integer literal
 */
#define AMP_AGENT_INT32_PARAM_DEF(_id) { (_id), AMP_TYPE_INT32, {.i32 = 0} }

/**
 * @brief Define 8bit unsigned integer parameter
 * Use this macro to initialize uint8 params in parameters memory buffer
 * @param[in] _id     param ID, integer literal
 */
#define AMP_AGENT_UINT8_PARAM_DEF(_id) { (_id), AMP_TYPE_UINT8, {.ui8 = 0} }

/**
 * @brief Define 16bit unsigned integer parameter
 * Use this macro to initialize uint16 params in parameters memory buffer
 * @param[in] _id     param ID, integer literal
 */
#define AMP_AGENT_UINT16_PARAM_DEF(_id) { (_id), AMP_TYPE_UINT16, {.ui16 = 0} }

/**
 * @brief Define 32bit unsigned integer parameter
 * Use this macro to initialize uint32 params in parameters memory buffer
 * @param[in] _id     param ID, integer literal
 */
#define AMP_AGENT_UINT32_PARAM_DEF(_id) { (_id), AMP_TYPE_UINT32, {.ui32 = 0} }

/**
 * @brief Define float parameter
 * Use this macro to initialize float params in parameters memory buffer
 * @param[in] _id     param ID, integer literal
 * @param[in] _buffer buffer for string params
 */
#define AMP_AGENT_FLOAT_PARAM_DEF(_id) { (_id), AMP_TYPE_FLOAT, {.f = 0.0} }

/**
 * @brief Define string parameter
 * Use this macro to initialize string params in parameters memory buffer
 * @param[in] _id     param ID, integer literal
 * @param[in] _buffer buffer for string characters
 */
#define AMP_AGENT_STRING_PARAM_DEF(_id, _buffer) { (_id), AMP_TYPE_STRING, {.s = (_buffer)} }

/**
 * @brief Define bool statistic
 * Use this macro to initialize bool stats in statistics memory buffer
 * @param[in] _id     param ID, integer literal
 */
#define AMP_AGENT_BOOL_STAT_DEF(_id) { (_id), AMP_TYPE_BOOL, {.b = false} }

/**
 * @brief Define 8bit signed integer statistic
 * Use this macro to initialize int8 stats in statistics memory buffer
 * @param[in] _id     param ID, integer literal
 */
#define AMP_AGENT_INT8_STAT_DEF(_id) { (_id), AMP_TYPE_INT8, {.i8 = 0} }

/**
 * @brief Define 16bit signed integer statistic
 * Use this macro to initialize int16 stats in statistics memory buffer
 * @param[in] _id     param ID, integer literal
 */
#define AMP_AGENT_INT16_STAT_DEF(_id) { (_id), AMP_TYPE_INT16, {.i16 = 0} }

/**
 * @brief Define 32bit signed integer statistic
 * Use this macro to initialize int32 stats in statistics memory buffer
 * @param[in] _id     param ID, integer literal
 */
#define AMP_AGENT_INT32_STAT_DEF(_id) { (_id), AMP_TYPE_INT32, {.i32 = 0} }

/**
 * @brief Define 8bit unsigned integer statistic
 * Use this macro to initialize uint8 stats in statistics memory buffer
 * @param[in] _id     param ID, integer literal
 */
#define AMP_AGENT_UINT8_STAT_DEF(_id) { (_id), AMP_TYPE_UINT8, {.ui8 = 0} }

/**
 * @brief Define 16bit unsigned integer statistic
 * Use this macro to initialize uint16 stats in statistics memory buffer
 * @param[in] _id     param ID, integer literal
 */
#define AMP_AGENT_UINT16_STAT_DEF(_id) { (_id), AMP_TYPE_UINT16, {.ui16 = 0} }

/**
 * @brief Define 32bit unsigned integer statistic
 * Use this macro to initialize uint32 stats in statistics memory buffer
 * @param[in] _id     param ID, integer literal
 */
#define AMP_AGENT_UINT32_STAT_DEF(_id) { (_id), AMP_TYPE_UINT32, {.ui32 = 0} }

/**
 * @brief Define float statistic
 * Use this macro to initialize float stats in statistics memory buffer
 * @param[in] _id     stat ID, integer literal
 */
#define AMP_AGENT_FLOAT_STAT_DEF(_id) { (_id), AMP_TYPE_FLOAT, {.f = 0} }

/**
 * @brief Define alarm
 * @param[in]  _id    alarm ID, integer literal
 */
#define AMP_AGENT_ALARM_DEF(_id)    { _id, AMP_ALARM_STATE_CLEARED, 0 }


/*----------------------------------------------------------------------------*/
// AMP Agent Library functions
void amp_agent_init(system_info_t *sys_info, system_data_t *sys_data);
void amp_agent_send_sync_msg(void);
void amp_agent_handle_msg(uint8_t *msg, uint16_t size);
void amp_agent_sync_task(void);
void amp_agent_loop(void (*app_cb)(void), void (*sleep_cb)(uint64_t));
const char* amp_agent_get_library_version(void);
bool amp_agent_is_connected(void);
void amp_agent_param_set(void);

amp_param_t* amp_agent_get_param(uint32_t id);
amp_stat_t* amp_agent_get_stat(uint32_t id);
amp_alarm_t* amp_agent_get_alarm(uint8_t id);

int amp_agent_write_param_bool(uint32_t id, bool value);
int amp_agent_write_param_i8(uint32_t id, int8_t value);
int amp_agent_write_param_i16(uint32_t id, int16_t value);
int amp_agent_write_param_i32(uint32_t id, int32_t value);
int amp_agent_write_param_u8(uint32_t id, uint8_t value);
int amp_agent_write_param_u16(uint32_t id, uint16_t value);
int amp_agent_write_param_u32(uint32_t id, uint32_t value);
int amp_agent_write_param_float(uint32_t id, float value);
int amp_agent_write_param_string(uint32_t id, const char *value);

int amp_agent_read_param_bool(uint32_t id, bool *value);
int amp_agent_read_param_i8(uint32_t id, int8_t *value);
int amp_agent_read_param_i16(uint32_t id, int16_t *value);
int amp_agent_read_param_i32(uint32_t id, int32_t *value);
int amp_agent_read_param_u8(uint32_t id, uint8_t *value);
int amp_agent_read_param_u16(uint32_t id, uint16_t *value);
int amp_agent_read_param_u32(uint32_t id, uint32_t *value);
int amp_agent_read_param_float(uint32_t id, float *value);
int amp_agent_read_param_string(uint32_t id, char *value);

int amp_agent_write_stat_bool(uint32_t id, bool value);
int amp_agent_write_stat_i8(uint32_t id, int8_t value);
int amp_agent_write_stat_i16(uint32_t id, int16_t value);
int amp_agent_write_stat_i32(uint32_t id, int32_t value);
int amp_agent_write_stat_u8(uint32_t id, uint8_t value);
int amp_agent_write_stat_u16(uint32_t id, uint16_t value);
int amp_agent_write_stat_u32(uint32_t id, uint32_t value);
int amp_agent_write_stat_float(uint32_t id, float value);

void amp_agent_set_alarm(uint32_t id);
void amp_agent_clear_alarm(uint32_t id);


/*----------------------------------------------------------------------------*/
// Declarations of external functions required by AMP Agent Library
/**
 * @brief Current timestamp
 * @return timestamp in milliseconds
 */
extern uint64_t amp_agent_now(void);

/**
 * @brief Notify device about configuration parameter update by AS
 * @param[in] param pointer to updated param structure in global params buffer
 */
extern void amp_agent_param_changed(amp_param_t *param);

/**
 * @brief Notify device about statistic update necessity
 * This function is called by SDK when updated stats values are needed
 * to build sync message
 */
extern void amp_agent_stats_update(void);

/**
 * @brief Send message via UDP
 * @param[in] data message data buffer
 * @param[in] size message size
 * @return number of bytes sent on success or -1 on error
 */
extern int amp_agent_send_msg(uint8_t *data, uint16_t size);

/**
 * @brief Receive message via UDP
 * @param[out] data pointer to allocated buffer
 * @param[out] size total size of allocated buffer
 * @return number of bytes received on success, 0 if no message, -1 on error
 */
extern int amp_agent_recv_msg(uint8_t *data, uint16_t size);


#endif /* __AMP_AGENT_H__ */
