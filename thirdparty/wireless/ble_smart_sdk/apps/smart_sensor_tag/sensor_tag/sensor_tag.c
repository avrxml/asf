/**
 * \file
 *
 * \brief Sensor tag ble profile
 *
 * Copyright (c) 2018 Microchip Technology Inc. and its subsidiaries.
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

/*- Includes ---------------------------------------------------------------*/
#include "ble_manager.h"
#include "sensor_tag.h"
#include "timer_hw.h"
#include "platform.h"
#include "bhi160.h"
#include "bme280/bme280_support.h"

/* macros */
#define DUAL_TIME_CALLBACK PORT_WAKEUP_SOURCE_AON_GPIO_2

/* Environment sensor data rate */
#define ENV_SENSOR_DATA_RATE_1Hz 1000/20u //50	//1second
#define ENV_SENSOR_DATA_RATE_2Hz 500/20u //25	//500ms
#define ENV_SENSOR_DATA_RATE_4Hz 250/20u //12	//250ms
#define ENV_SENSOR_DATA_RATE_8Hz 125/20u //6	//125ms
#define ENV_SENSOR_DATA_RATE_10Hz 100/20u //5	//100ms
#define ENV_DEFAULT_BLE_TASK_PERIOD 1000/20u //5 //1second

/* motion sensor data rate */
#define MOTION_SENSOR_DATA_RATE_12_5Hz 80/20u //4	//80ms
#define MOTION_SENSOR_DATA_RATE_25Hz 40/20u //2		//40ms
#define MOTION_SENSOR_DATA_RATE_50Hz 20/20u //1		//20ms
#define MOTION_SENSOR_DATA_RATE_100Hz 20/20u //1	//10ms
#define MOTION_SENSOR_DATA_RATE_200Hz 20/20u //1	//5ms
#define MOTION_DEFAULT_BLE_TASK_PERIOD 20/20u //5	//20ms

#define NOTIFICATION_ENABLE 1
#define NOTIFICATION_DISABLE 0
#define BATTERY_THRESHOLD_LEVEL 2.3
#define SAMPLE_DEBOUNCE_COUNT 40

/* global variables */
uint8_t env_sensor_periods[5] = {
	ENV_SENSOR_DATA_RATE_1Hz,
	ENV_SENSOR_DATA_RATE_2Hz,
	ENV_SENSOR_DATA_RATE_4Hz,
	ENV_SENSOR_DATA_RATE_8Hz,
	ENV_SENSOR_DATA_RATE_10Hz};

uint8_t env_sensor_period = ENV_DEFAULT_BLE_TASK_PERIOD;

uint8_t motion_sensor_periods[5] = {
	MOTION_SENSOR_DATA_RATE_12_5Hz,
	MOTION_SENSOR_DATA_RATE_25Hz,
	MOTION_SENSOR_DATA_RATE_50Hz,
	MOTION_SENSOR_DATA_RATE_100Hz,
	MOTION_SENSOR_DATA_RATE_200Hz};
	
uint8_t motion_sensor_period = MOTION_DEFAULT_BLE_TASK_PERIOD;
bool started_advertising;
uint16_t advertisement_timer;

/* service handlers */
gatt_environment_service_handler_t environment_service_handle;
gatt_device_orientation_service_handler_t device_orientation_service_handle;
gatt_battery_service_handler_t battery_service_handle;


/* initial values for characteristics values */
uint8_t env_sensors_odr_initial_value = 1;
environment_data_t env_initial_value = {0,0,0,0};
gyro_positions_t gyro_pos_initial_value = {0,0,0};
accelero_positions_t accelero_pos_initial_value = {0,0,0};
device_rotation_vector_t device_rotation_vector_initial_value = {0,0,0,0};
uint8_t motion_odr_initial_value = 1;
uint8_t accelero_drop_down_ind_initial_value = 0;
uint8_t accelero_step_inc_ind_initial_value = 0;
uint8_t battery_level_initial_value = 0;

/** @brief Scan response data*/
uint8_t scan_rsp_data[SCAN_RESP_LEN] = {0x09, 0xff, 0x00, 0x06, 0xd6, 0xb2, 0xf0, 0x05, 0xf0, 0xf8};

bool volatile timer_cb_done = false;
volatile bool g_sent_notification = 1;
volatile bool env_sensor_enabled;
volatile bool motion_sensor_enabled;
volatile bool platform_event_triggered = false;
volatile bool g_low_battery_status = false;

/* external variables */

/** @brief information of the connected devices */
extern at_ble_connected_t ble_dev_info[BLE_MAX_DEVICE_CONNECTION];

/* static prototypes */
static void wbe_service_init(void);
static void wbe_service_define(void);
static at_ble_status_t wbe_advertisement_data_set(void);
static at_ble_status_t ble_connected_app_event(void *param);
static at_ble_status_t ble_disconnected_app_event(void *param);
static at_ble_status_t ble_notification_confirmed_app_event(void *param);
static void init_environment_service(gatt_environment_service_handler_t  *environment_serv);
static void init_device_orientation_service(gatt_device_orientation_service_handler_t *device_orientation_serv);
static void init_battery_service(gatt_battery_service_handler_t *battery_serv);
static at_ble_status_t wbe_environment_service_define(gatt_environment_service_handler_t *environment_service);
static at_ble_status_t wbe_device_orientation_service_define(gatt_device_orientation_service_handler_t *device_orientation_service);
static at_ble_status_t wbe_battery_service_define(gatt_battery_service_handler_t *battery_service);

#define DELAY_20MSEC 20

//environment data to BLE
environment_data_t env_data;
uint32_t env_tick_timer=0;
uint32_t tick_timer;
uint32_t motion_sensor_count=0;
uint8_t timer_motion_sensor_set;
uint8_t tick_40msec;
uint8_t tick_100msec;
uint8_t timer_env_sensor_set;
uint8_t tick_60sec;
uint8_t tick_1sec;

s32 env_temperature;
u32 env_pressure;
u32 env_humidity;
uint32_t als_data;

void timer_callback(void);
void get_environment_sensor_data(void);
static at_ble_status_t platform_interrupt_app_event(void *param);

/* Custom events like user defined event, platform event callback handlers for OTAU profile */
static const ble_custom_event_cb_t sensor_tag_custom_event_handle = {
	.platform_event_ready = platform_interrupt_app_event /* This event not handled in BLE Manager */
};

extern uint32_t *wakeup_event_pending;

/****************************************************************************************
*							        Implementations	                                    *
****************************************************************************************/

static const ble_gap_event_cb_t sensor_tag_app_gap_cb = {
	.connected = ble_connected_app_event,
	.disconnected = ble_disconnected_app_event,
};

static const ble_gatt_server_event_cb_t sensor_tag_app_gatt_server_cb = {
	.notification_confirmed = ble_notification_confirmed_app_event,
	.characteristic_changed = wbe_char_changed_handler,
};

/**
 * \Timer callback handler called on timer expiry
 */
void timer_callback(void)
{
	if(((*wakeup_event_pending >> 16) && 0xFF) == 0)
	{
		*wakeup_event_pending &= ~(0xFF << 16);
		*wakeup_event_pending |= (0x1 << 16);
	}
}

static void timekeeping_cb(void)
{
	env_tick_timer ++;
	motion_sensor_count ++;
	tick_timer++;
	if(motion_sensor_count == motion_sensor_period)
	{
		timer_motion_sensor_set = true;
		motion_sensor_count = 0;
		
	    #if defined DEBUG_LED
	    LED_Toggle(LED_0_PIN);
	    #endif	
	}
	
	if(env_tick_timer == env_sensor_period)
	{
		timer_env_sensor_set = true;
		env_tick_timer = 0;
	}
	
	if(tick_timer % 300 == 0)
	{
		tick_60sec = true;
	}
	
	if(tick_timer % 50 == 0)
	{
		tick_1sec = true;
	}
	
	platform_event_triggered = true;
	send_plf_int_msg_ind(RAM_ISR_TABLE_DUALTIMER_INDEX, TIMER_EXPIRED_CALLBACK_TYPE_DETECT, NULL, 0);
}

/* Callback registered for AT_PLATFORM_EVENT event from stack */
static at_ble_status_t platform_interrupt_app_event(void *param)
{
	at_ble_status_t status = AT_BLE_SUCCESS;
	platform_isr_event_t *plf_isr_event = (platform_isr_event_t *)param;
	float battery_volt = 0;
	
	/* Flag to start notification */
	if ((plf_isr_event->event_type == ((TIMER_EXPIRED_CALLBACK_TYPE_DETECT << 8)  | RAM_ISR_TABLE_DUALTIMER_INDEX)))
	{
	   platform_event_triggered = false;
	   
	   if(is_ble_connection_established())
	   {

			if (g_sent_notification) 
			{
			 	if(tick_60sec)
				{
				 at_ble_calib_get_voltage(&battery_volt);
				 tick_60sec = false;
				 if(battery_volt < BATTERY_THRESHOLD_LEVEL)
				 {
					 if(is_ble_low_bat_char_notification_enabled() && g_low_battery_status == false)
					 {
						ble_notify_low_battery();
						g_low_battery_status = true;
					 }
				 }
			    }
				
				if(tick_1sec)
				{
					tick_1sec = false;
					if(is_ble_drop_char_notification_enabled() && g_sent_notification == true)
					{
						if(device_drop_detected >SAMPLE_DEBOUNCE_COUNT)
						{
							device_drop_detected = 0;
							ble_notify_device_drop_detection();
						}
					}
				}
			
				if(timer_motion_sensor_set)
				{
					timer_motion_sensor_set = false;
					process_motion_sensor_data();
					if(step_detected && g_sent_notification == true)
					{
						step_detected = false;
						ble_notify_accelero_step_detection();
					}
				
					if(is_ble_gyro_char_notification_enabled() && g_sent_notification == true)
					{
						ble_notify_gyro_positions(&gyro_data);
					}
					if(is_ble_accelero_char_notification_enabled() && g_sent_notification == true)
					{
						ble_notify_accelero_positions(&acc_data);
					}
					if(is_ble_rotation_vector_char_notification_enabled() && g_sent_notification == true)
					{
						ble_notify_device_rotation_vector(&quaternion_data);
					}
				}

				if(timer_env_sensor_set)
				{
					timer_env_sensor_set = false;
					get_environment_sensor_data();
				
					if(is_ble_env_char_notification_enabled() && g_sent_notification == true)
					{
						ble_notify_environment_data(&env_data);
					}
				}
			}
		}
	}
	return status;
}

void get_environment_sensor_data(void)
{
	veml6080_read_alsdata(&als_data);	
	bme280_set_power_mode(BME280_FORCED_MODE);
	bme280_read_pressure_temperature_humidity(&env_pressure, &env_temperature, &env_humidity);
	/* Temperature: No data processing required for Temperature data. Data with resolution 0.01(x100) directly txd*/
	env_data.temperature = (int16_t)env_temperature;
	/* Pressure: Returns pressure in Pa as unsigned 32 bit integer. Output value of “96386” equals 96386 Pa = 963.86 hPa*/
	env_data.pressure = (uint16_t)((env_pressure / 100) & 0xFFFF);
	/*ALS: lx/step = 0.07 */
	env_data.uv = (als_data * 7000); //als_data o/p is of 16-bit range
	/* Humidity: An output value of 42313 represents 42313 / 1024 = 41.321 %rH*/
	env_data.humidity = (uint8_t)(env_humidity / 1024);
}

/**@brief connected event (AT_BLE_CONNECTED) call back
*/
static at_ble_status_t ble_connected_app_event(void *param)
{
	/* set connection indication flag */
	connection_established_flag = true;
	started_advertising = false;
	advertisement_timer = 0;
    hw_timer_start_ms(DELAY_20MSEC);
	return AT_BLE_SUCCESS;
}

/* Callback registered for AT_BLE_DISCONNECTED event from stack */
static at_ble_status_t ble_disconnected_app_event(void *param)
{
	/* re-initialize connection and notification flags */
	g_sent_notification = 1;
	connection_established_flag = false;
	env_char_notification_flag = false;
	gyro_char_notification_flag = false;
	accelero_char_notification_flag = false;
	rotation_vector_char_notification_flag = false;
	drop_char_notification_flag = false;
	step_inc_char_notification_flag = false;
	low_bat_char_notification_flag = false;
	
#if defined DEBUG_LED
	 LED_On(LED_0_PIN);
#endif
	
	env_sensor_period = ENV_DEFAULT_BLE_TASK_PERIOD;
	motion_sensor_period = MOTION_DEFAULT_BLE_TASK_PERIOD;
	env_tick_timer = 0;
	motion_sensor_count = 0;
    hw_timer_stop();
	/* start advertisement */
	wbe_start_advertisement();
	
	ALL_UNUSED(param);
	return AT_BLE_SUCCESS;
}

/* Callback registered for AT_BLE_NOTIFICATION_CONFIRMED event from stack */
static at_ble_status_t ble_notification_confirmed_app_event(void *param)
{
	at_ble_cmd_complete_event_t *notification_status = (at_ble_cmd_complete_event_t *)param;
	if(!notification_status->status)
	{  
		/* set notification sent flag (indicating previous notification is sent) */
		g_sent_notification = 1;	
		DBG_LOG_DEV("Notification sent successfully");
		return AT_BLE_SUCCESS;
	}
	
	return AT_BLE_FAILURE;
}


/** @brief Update Environment characteristics and send notification to client 
  * 
  */
at_ble_status_t ble_notify_environment_data(environment_data_t* environ_data)
{	
	/* Updating the attribute data base */
	if ((at_ble_characteristic_value_set(environment_service_handle.serv_chars[0].char_val_handle, (uint8_t*)environ_data, sizeof(environment_data_t))) == AT_BLE_FAILURE)
	{	
		DBG_LOG("updating the environment characteristic failed");
		return AT_BLE_FAILURE;
		} else {
		DBG_LOG_DEV("updating the environment characteristic value is successful");
	}

	/* sending notification to the peer about change in the environment data */
	if((at_ble_notification_send(ble_dev_info[0].handle, environment_service_handle.serv_chars[0].char_val_handle)) == AT_BLE_FAILURE) 
	{
		DBG_LOG("sending environment update notification to the peer failed");
		return AT_BLE_FAILURE;
	}
	else 
	{	
		DBG_LOG("sending environment notification");
		DBG_LOG("Temperature: %d", environ_data->temperature);
		DBG_LOG("Humidity: %d", environ_data->humidity);
		DBG_LOG("Pressure: %d", environ_data->pressure);
		DBG_LOG("UV: %u", environ_data->uv);
		g_sent_notification = false;		
		return AT_BLE_SUCCESS;
	}		
}

/** @brief Update Gyro positions characteristics and send notification to client 
  * 
  */
at_ble_status_t ble_notify_gyro_positions(gyro_positions_t* gyro_positions)
{
	/* Updating the attribute data base */
	if ((at_ble_characteristic_value_set(device_orientation_service_handle.serv_chars[0].char_val_handle, (uint8_t*)gyro_positions, sizeof(gyro_positions_t))) == AT_BLE_FAILURE)
	{
		DBG_LOG("updating the Gyro positions characteristic failed");
		return AT_BLE_FAILURE;
		} else {
		DBG_LOG_DEV("updating the Gyro positions characteristic value is successful");
	}

	/* sending notification to the peer about change in the Gyro positions */
	if((at_ble_notification_send(ble_dev_info[0].handle, device_orientation_service_handle.serv_chars[0].char_val_handle)) == AT_BLE_FAILURE)
	{
		DBG_LOG("sending Gyro positions update notification to the peer failed");
		return AT_BLE_FAILURE;
	}
	else
	{	
		DBG_LOG("sending Gyro positions notificationl");
		DBG_LOG("Gyro-X: %d", gyro_positions->x_pos);
		DBG_LOG("Gyro-Y: %d", gyro_positions->y_pos);
		DBG_LOG("Gyro-Z: %d", gyro_positions->z_pos);
		g_sent_notification = false;
		return AT_BLE_SUCCESS;
	}
}


/** @brief Update Accelero position characteristics and send notification to client 
  * 
  */
at_ble_status_t ble_notify_accelero_positions(accelero_positions_t* accelero_pos)
{
	/* Updating the attribute data base */
	if ((at_ble_characteristic_value_set(device_orientation_service_handle.serv_chars[1].char_val_handle, (uint8_t*)accelero_pos, sizeof(accelero_positions_t))) == AT_BLE_FAILURE)
	{
		DBG_LOG("updating the Accelero positions characteristic failed");
		return AT_BLE_FAILURE;
		} else {	
		DBG_LOG_DEV("updating the Accelero positions characteristic value is successful");
	}

	/* sending notification to the peer about change in the Accelero positions */
	if((at_ble_notification_send(ble_dev_info[0].handle, device_orientation_service_handle.serv_chars[1].char_val_handle)) == AT_BLE_FAILURE)
	{	
		DBG_LOG("sending Accelero positions update notification to the peer failed");
		return AT_BLE_FAILURE;
	}
	else
	{

		DBG_LOG("sending Accelero position notification");
		DBG_LOG("Accelero X Pos: %d", accelero_pos->x_pos);
		DBG_LOG("Accelero Y Pos: %d", accelero_pos->y_pos);
		DBG_LOG("Accelero Z Pos: %d", accelero_pos->z_pos);
		g_sent_notification = false;
		return AT_BLE_SUCCESS;
	}
}

/** @brief Update Device rotation vector characteristics and send notification to client 
  * 
  */
at_ble_status_t ble_notify_device_rotation_vector(device_rotation_vector_t* rotation_vector)
{
	/* Updating the attribute data base */
	if ((at_ble_characteristic_value_set(device_orientation_service_handle.serv_chars[2].char_val_handle, (uint8_t*)rotation_vector, sizeof(device_rotation_vector_t))) == AT_BLE_FAILURE)
	{
		DBG_LOG("updating the device rotation vector characteristic failed");
		return AT_BLE_FAILURE;
		} else {	
		DBG_LOG_DEV("updating the device rotation vector characteristic value is successful");
	}

	/* sending notification to the peer about change in the Accelero X-pos */
	if((at_ble_notification_send(ble_dev_info[0].handle, device_orientation_service_handle.serv_chars[2].char_val_handle)) == AT_BLE_FAILURE)
	{
		DBG_LOG("sending device rotation vector update notification to the peer failed");
		return AT_BLE_FAILURE;
	}
	else
	{	
		DBG_LOG("sending device rotation vector notification");
		DBG_LOG("rot_vector_X: %d", rotation_vector->x);
		DBG_LOG("rot_vector_Y: %d", rotation_vector->y);
		DBG_LOG("rot_vector_Z: %d", rotation_vector->z);
		DBG_LOG("rot_vector_W: %d", rotation_vector->w);
		g_sent_notification = false;
		return AT_BLE_SUCCESS;
	}
}

/** @brief Update accelerometer step detection notification to client 
  * 
  */
at_ble_status_t ble_notify_accelero_step_detection(void)
{
	/* Updating the attribute data base */
	if ((at_ble_characteristic_value_set(device_orientation_service_handle.serv_chars[5].char_val_handle, &step_detected, sizeof(step_detected))) == AT_BLE_FAILURE)
	{
		DBG_LOG("updating the step characteristic failed");
		return AT_BLE_FAILURE;
	} else 
	{	
		DBG_LOG_DEV("updating the step characteristic value is successful");
	}
	
	/* sending notification to the peer about accelerometer step detection */
	if((at_ble_notification_send(ble_dev_info[0].handle, device_orientation_service_handle.serv_chars[5].char_val_handle)) == AT_BLE_FAILURE)
	{
		DBG_LOG("sending Accelero step detection notification to the peer failed");
		return AT_BLE_FAILURE;
	}
	else
	{
		DBG_LOG("sending Accelero step detection");
		return AT_BLE_SUCCESS;
	}
}

/** @brief Update device drop detection notification to client 
  * 
  */
at_ble_status_t ble_notify_device_drop_detection(void)
{	
	/* sending notification to the peer about accelerometer drop detection */
	if((at_ble_notification_send(ble_dev_info[0].handle, device_orientation_service_handle.serv_chars[4].char_val_handle)) == AT_BLE_FAILURE)
	{
		DBG_LOG("sending device drop detection notification to the peer failed");
		return AT_BLE_FAILURE;
	}
	else
	{
		DBG_LOG("sending device drop detection");
		return AT_BLE_SUCCESS;
	}
}

/** @brief Update low battery notification to client 
  * 
  */
at_ble_status_t ble_notify_low_battery(void)
{
		
	/* sending notification to the peer about low battery */
	if((at_ble_notification_send(ble_dev_info[0].handle, battery_service_handle.serv_chars.char_val_handle)) == AT_BLE_FAILURE)
	{	
		DBG_LOG("sending Low Battery notification to the peer failed");
		return AT_BLE_FAILURE;
	}
	else
	{
		DBG_LOG("sending Low Battery notification");
		return AT_BLE_SUCCESS;
	}
}


/** @brief wbe_char_changed_handler called by the ble manager after a
 * change in the characteristic
 *  @param[in] at_ble_characteristic_changed_t which contains handle of
 *characteristic and new value
 */
at_ble_status_t wbe_char_changed_handler(void *params)
{	
	at_ble_characteristic_changed_t *change_params = (at_ble_characteristic_changed_t*)params;
		
	/* environment char notification enable\disable */
	if(change_params->char_handle == environment_service_handle.serv_chars[0].client_config_handle)
	{
		if(change_params->char_new_value[0] == NOTIFICATION_ENABLE)
		{
			enable_environmental_sensors();
			env_char_notification_flag = true;	
			DBG_LOG("env noti enabled");
		}
		else if(change_params->char_new_value[0] == NOTIFICATION_DISABLE)
		{
			disable_environmental_sensors();
			env_char_notification_flag = false;	
			DBG_LOG("env noti disabled");
		}
	}
	/* gyro char notification enable\disable */
	else if(change_params->char_handle == device_orientation_service_handle.serv_chars[0].client_config_handle)
	{
		if(change_params->char_new_value[0] == NOTIFICATION_ENABLE)
		{
			enable_gyroscope();
			gyro_char_notification_flag = true;	
			DBG_LOG("gyro noti enabled");
		}
		else if(change_params->char_new_value[0] == NOTIFICATION_DISABLE)
		{
			disable_gyroscope();
			gyro_char_notification_flag = false;	
			DBG_LOG("gyro noti disabled");
		}
	}
	/* accelerometer char notification enable\disable */
	else if(change_params->char_handle == device_orientation_service_handle.serv_chars[1].client_config_handle)
	{
		if(change_params->char_new_value[0] == NOTIFICATION_ENABLE)
		{
			enable_accelerometer();
			accelero_char_notification_flag = true;
			DBG_LOG("acc noti enabled");
		}
		else if(change_params->char_new_value[0] == NOTIFICATION_DISABLE)
		{
			disable_accelerometer();
			accelero_char_notification_flag = false;
			DBG_LOG("acc noti disabled");
		}
	}
	/* rotation vector char notification enable\disable */
	else if(change_params->char_handle == device_orientation_service_handle.serv_chars[2].client_config_handle)
	{
		if(change_params->char_new_value[0] == NOTIFICATION_ENABLE)
		{
			enable_rotation_vector();
			rotation_vector_char_notification_flag = true;
			DBG_LOG("rot noti enabled");
		}
		else if(change_params->char_new_value[0] == NOTIFICATION_DISABLE)
		{
			disable_rotation_vector();
			rotation_vector_char_notification_flag = false;
			DBG_LOG("rot noti disabled");
		}
	}
	/* device drop char notification enable\disable */
	else if(change_params->char_handle == device_orientation_service_handle.serv_chars[4].client_config_handle)
	{
		if(change_params->char_new_value[0] == NOTIFICATION_ENABLE)
		{
			enable_drop_detector();
			drop_char_notification_flag = true;	
			DBG_LOG("drop noti enabled");
		}
		else if(change_params->char_new_value[0] == NOTIFICATION_DISABLE)
		{
			disable_drop_detector();
			drop_char_notification_flag = false;
			DBG_LOG("drop noti disabled");
		}
	}
	/* step-inc char notification enable\disable */
	else if(change_params->char_handle == device_orientation_service_handle.serv_chars[5].client_config_handle)
	{
		if(change_params->char_new_value[0] == NOTIFICATION_ENABLE)
		{
  			env_char_notification_flag = true;
  			DBG_LOG("env noti enabled");			  			
			enable_step_detector();
			step_inc_char_notification_flag = true;
			DBG_LOG("step noti enabled");
		}
		else if(change_params->char_new_value[0] == NOTIFICATION_DISABLE)
		{
			disable_step_detector();
			step_inc_char_notification_flag = false;
			DBG_LOG("step noti disabled");
			env_char_notification_flag = false;
  			DBG_LOG("env noti disabled");	  
		}
	}
	/* battery char notification enable\disable */
	else if(change_params->char_handle == battery_service_handle.serv_chars.client_config_handle)
	{
		if(change_params->char_new_value[0] == NOTIFICATION_ENABLE)
		{
			low_bat_char_notification_flag = true;	
			DBG_LOG("bat noti enabled");
		}
		else if(change_params->char_new_value[0] == NOTIFICATION_DISABLE)
		{
			low_bat_char_notification_flag = false;	
			DBG_LOG("bat noti disabled");
		}
	}
	/* Environment sensor period(DOR) setting*/
	else if(change_params->char_handle == environment_service_handle.serv_chars[1].char_val_handle)
	{
		DBG_LOG("env sensor period %d",change_params->char_new_value[0]);
		env_sensor_period = env_sensor_periods[change_params->char_new_value[0] - 1];
		env_tick_timer = 0;
	}
	/* Motion sensor period(DOR) setting*/
	else if(change_params->char_handle == device_orientation_service_handle.serv_chars[3].char_val_handle)
	{	
		DBG_LOG("motion sensor period %d",change_params->char_new_value[0]);
		motion_sensor_period = motion_sensor_periods[change_params->char_new_value[0] - 1];
		motion_sensor_count	= 0;
	}
	
	/* check motion sensor enabled */
	if((gyro_char_notification_flag == false)
		&& (accelero_char_notification_flag == false)
		&& (rotation_vector_char_notification_flag == false)
		&& (drop_char_notification_flag == false)
		&& (step_inc_char_notification_flag == false))
		{
			motion_sensor_enabled = false;
		}
		else
		{
			motion_sensor_enabled = true;
		}
		
	return AT_BLE_SUCCESS;
}


/** @brief start advertisement data over air
  * 
  */
at_ble_status_t wbe_start_advertisement(void)
{
	/* Start of advertisement */
	if(at_ble_adv_start(AT_BLE_ADV_TYPE_UNDIRECTED, AT_BLE_ADV_GEN_DISCOVERABLE, \
	NULL, AT_BLE_ADV_FP_ANY, APP_FAST_ADV, APP_ADV_TIMEOUT, 0) == \
	AT_BLE_SUCCESS)
	{	
		DBG_LOG("BLE Started Advertisement");
		started_advertising = true;
		advertisement_timer = 0;
		return AT_BLE_SUCCESS;
	}
	else
	{	
		DBG_LOG("BLE Advertisement start Failed");
	}
	return AT_BLE_FAILURE;
}

/**
* \Sensor tag service advertisement initialization
*/
static at_ble_status_t wbe_advertisement_data_set(void)
{
	uint8_t idx = 0;
	
	uint8_t adv_data [ WD_ADV_DATA_NAME_LEN + ADV_DATA_UUID_LEN   + (2*2)];
	
	adv_data[idx++] = ADV_DATA_UUID_LEN + ADV_TYPE_LEN;
	adv_data[idx++] = ADV_DATA_UUID_TYPE;
	
	/* Appending the UUID */
	memcpy(&adv_data[idx], ENVIRONMENT_SERVICE_UUID, ADV_DATA_UUID_LEN);
	idx += ADV_DATA_UUID_LEN;
	
	//Appending the complete name to the Ad packet
	adv_data[idx++] = WD_ADV_DATA_NAME_LEN + ADV_TYPE_LEN;
	adv_data[idx++] = WD_ADV_DATA_NAME_TYPE;
	
	memcpy(&adv_data[idx], WD_ADV_DATA_NAME_DATA, WD_ADV_DATA_NAME_LEN );
	idx += WD_ADV_DATA_NAME_LEN ;
	
	/* Adding the advertisement data and scan response data */
	if(!(at_ble_adv_data_set(adv_data, idx, scan_rsp_data, SCAN_RESP_LEN) == AT_BLE_SUCCESS) )
	{
		#ifdef DBG_LOG
		DBG_LOG("Failed to set adv data");
		#endif
	}
	
	return AT_BLE_SUCCESS;
}

/**@brief Register a Environment service instance inside stack.
*/
static at_ble_status_t wbe_environment_service_define(gatt_environment_service_handler_t *environment_service)
{
	return(at_ble_primary_service_define(&environment_service->serv_uuid,
	&environment_service->serv_handle,
	NULL, 0,
	environment_service->serv_chars, ENVIRONMENT_CHARACTERISTIC_COUNT));
}

/**@brief Register a device orientation service instance inside stack.
*/
static at_ble_status_t wbe_device_orientation_service_define(gatt_device_orientation_service_handler_t *device_orientation_service)
{
	return(at_ble_primary_service_define(&device_orientation_service->serv_uuid,
	&device_orientation_service->serv_handle,
	NULL, 0,
	device_orientation_service->serv_chars, DEVICE_ORIENTATION_CHARACTERISTIC_COUNT));
}

/**@brief Register a Battery service instance inside stack.
*/
static at_ble_status_t wbe_battery_service_define(gatt_battery_service_handler_t *battery_service)
{
	return(at_ble_primary_service_define(&battery_service->serv_uuid,
	&battery_service->serv_handle,
	NULL, 0,
	&battery_service->serv_chars, BATTERY_LEVEL_CHARACTERISTIC_COUNT));
}

/** @brief defines the services of the profile
 *
 */
static void wbe_service_define(void)
{
	at_ble_status_t status;
	/* Environment service */		
	if((status = wbe_environment_service_define(&environment_service_handle)) == AT_BLE_SUCCESS)
	{
		DBG_LOG_DEV("env handle %d",environment_service_handle.serv_handle);
	}
	else
	{
		DBG_LOG("environment service define Failed, reason 0x%x", status);
	}
	
	/* Gyroscope service */	
	if((status = wbe_device_orientation_service_define(&device_orientation_service_handle)) == AT_BLE_SUCCESS)
	{
		DBG_LOG_DEV("Device orientation handle %d",device_orientation_service_handle.serv_handle);
	}
	else
	{	
		DBG_LOG("Device orientation service define Failed, reason 0x%x",status);
	}	
	
	/* Batter service */	
	if((status = wbe_battery_service_define(&battery_service_handle)) == AT_BLE_SUCCESS)
	{
		DBG_LOG_DEV("battery handle %d",battery_service_handle.serv_handle);
	}
	else
	{	
		DBG_LOG("Battery Service define Failed, reason 0x%x",status);
	}		
}

/**@brief Environment service and characteristic initialization
*
*/
static void init_environment_service(gatt_environment_service_handler_t  *environment_serv)
{
	environment_serv->serv_handle = 0;
	
	environment_serv->serv_uuid.type = AT_BLE_UUID_128;
	memcpy(&environment_serv->serv_uuid.uuid[0], ENVIRONMENT_SERVICE_UUID, 16);
	
	/* Characteristics - Temperature, Pressure, Humidity, UV */
	
	/* handle stored here */
	environment_serv->serv_chars[0].char_val_handle = 0;
	
	/* UUID : Temperature */
	environment_serv->serv_chars[0].uuid.type = AT_BLE_UUID_128;
	memcpy(&environment_serv->serv_chars[0].uuid.uuid[0], ENVIRONMENT_CHAR_UUID, 16);

	/* Properties */
	environment_serv->serv_chars[0].properties = AT_BLE_CHAR_READ | AT_BLE_CHAR_NOTIFY;
	/* value */
	environment_serv->serv_chars[0].init_value = (uint8_t*)&env_initial_value ;
	environment_serv->serv_chars[0].value_init_len = sizeof(environment_data_t);
	environment_serv->serv_chars[0].value_max_len = sizeof(environment_data_t);
	/* permissions */
	environment_serv->serv_chars[0].value_permissions = (AT_BLE_ATTR_READABLE_NO_AUTHN_NO_AUTHR | AT_BLE_ATTR_WRITABLE_NO_AUTHN_NO_AUTHR);
	/* user defined name */
	environment_serv->serv_chars[0].user_desc = NULL;
	environment_serv->serv_chars[0].user_desc_len = 0;
	environment_serv->serv_chars[0].user_desc_max_len = 0;
	/*user description permissions*/
	environment_serv->serv_chars[0].user_desc_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
	/*client config permissions*/
	environment_serv->serv_chars[0].client_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
	/*server config permissions*/
	environment_serv->serv_chars[0].server_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
	/*user desc handles*/
	environment_serv->serv_chars[0].user_desc_handle = 0;
	/*client config handles*/
	environment_serv->serv_chars[0].client_config_handle = 0;
	/*server config handles*/
	environment_serv->serv_chars[0].server_config_handle = 0;
	/* presentation format */
	environment_serv->serv_chars[0].presentation_format = NULL;
	
	/* Characteristics - Environment sensor ODR */
	/* handle stored here */
	environment_serv->serv_chars[1].char_val_handle = 0;
	
	/* UUID : Temperature */
	environment_serv->serv_chars[1].uuid.type = AT_BLE_UUID_128;
	memcpy(&environment_serv->serv_chars[1].uuid.uuid[0], ENV_ODR_CHAR_UUID, 16);

	/* Properties */
	environment_serv->serv_chars[1].properties = AT_BLE_CHAR_READ | AT_BLE_CHAR_WRITE;
	/* value */
	environment_serv->serv_chars[1].init_value = (uint8_t*)&env_sensors_odr_initial_value ;
	environment_serv->serv_chars[1].value_init_len = sizeof(uint8_t);
	environment_serv->serv_chars[1].value_max_len = sizeof(uint8_t);
	/* permissions */
	environment_serv->serv_chars[1].value_permissions = (AT_BLE_ATTR_READABLE_NO_AUTHN_NO_AUTHR | AT_BLE_ATTR_WRITABLE_NO_AUTHN_NO_AUTHR);
	/* user defined name */
	environment_serv->serv_chars[1].user_desc = NULL;
	environment_serv->serv_chars[1].user_desc_len = 0;
	environment_serv->serv_chars[1].user_desc_max_len = 0;
	/*user description permissions*/
	environment_serv->serv_chars[1].user_desc_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
	/*client config permissions*/
	environment_serv->serv_chars[1].client_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
	/*server config permissions*/
	environment_serv->serv_chars[1].server_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
	/*user desc handles*/
	environment_serv->serv_chars[1].user_desc_handle = 0;
	/*client config handles*/
	environment_serv->serv_chars[1].client_config_handle = 0;
	/*server config handles*/
	environment_serv->serv_chars[1].server_config_handle = 0;
	/* presentation format */
	environment_serv->serv_chars[1].presentation_format = NULL;
}

/**@brief Device-orientation service and characteristic initialization
*
*/
static void init_device_orientation_service(gatt_device_orientation_service_handler_t *device_orientation_serv)
{
	device_orientation_serv->serv_handle = 0; //orientation

	device_orientation_serv->serv_uuid.type = AT_BLE_UUID_128;
	memcpy(&device_orientation_serv->serv_uuid.uuid[0], DEVICE_ORIENTATION_SERVICE_UUID, 16);
	
	/*Characteristic Info for gyroscope meter coordinate positions (X,Y,Z) Value*/
	
	/* handle stored here */
	device_orientation_serv->serv_chars[0].char_val_handle = 0;
	
	/* UUID : Gyro X-Pos */
	device_orientation_serv->serv_chars[0].uuid.type = AT_BLE_UUID_128;
	memcpy(&device_orientation_serv->serv_chars[0].uuid.uuid[0], GYRO_POSITIONS_CHAR_UUID, 16);

	/* Properties */
	device_orientation_serv->serv_chars[0].properties = AT_BLE_CHAR_READ | AT_BLE_CHAR_NOTIFY;
	/* value */
	device_orientation_serv->serv_chars[0].init_value = (uint8_t*)&gyro_pos_initial_value;
	device_orientation_serv->serv_chars[0].value_init_len = sizeof(gyro_positions_t);
	device_orientation_serv->serv_chars[0].value_max_len = sizeof(gyro_positions_t);
	/* permissions */
	device_orientation_serv->serv_chars[0].value_permissions = (AT_BLE_ATTR_READABLE_NO_AUTHN_NO_AUTHR | AT_BLE_ATTR_WRITABLE_NO_AUTHN_NO_AUTHR);
	/* user defined name */
	device_orientation_serv->serv_chars[0].user_desc = NULL;
	device_orientation_serv->serv_chars[0].user_desc_len = 0;
	device_orientation_serv->serv_chars[0].user_desc_max_len = 0;
	/*user description permissions*/
	device_orientation_serv->serv_chars[0].user_desc_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
	/*client config permissions*/
	device_orientation_serv->serv_chars[0].client_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
	/*server config permissions*/
	device_orientation_serv->serv_chars[0].server_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
	/*user desc handles*/
	device_orientation_serv->serv_chars[0].user_desc_handle = 0;
	/*client config handles*/
	device_orientation_serv->serv_chars[0].client_config_handle = 0;
	/*server config handles*/
	device_orientation_serv->serv_chars[0].server_config_handle = 0;
	/* presentation format */
	device_orientation_serv->serv_chars[0].presentation_format = NULL;
	
	/*Characteristic Info for accelerometer coordinate positions (X,Y,Z) Value*/
	
	/* handle stored here */
	device_orientation_serv->serv_chars[1].char_val_handle = 0;
	
	/* UUID : Accelero Y-Pos */
	device_orientation_serv->serv_chars[1].uuid.type = AT_BLE_UUID_128;
	memcpy(&device_orientation_serv->serv_chars[1].uuid.uuid[0], ACCELERO_POSITIONS_CHAR_UUID, 16);

	/* Properties */
	device_orientation_serv->serv_chars[1].properties = AT_BLE_CHAR_READ | AT_BLE_CHAR_NOTIFY;
	/* value */
	device_orientation_serv->serv_chars[1].init_value = (uint8_t*)&accelero_pos_initial_value ;
	device_orientation_serv->serv_chars[1].value_init_len = sizeof(accelero_positions_t);
	device_orientation_serv->serv_chars[1].value_max_len = sizeof(accelero_positions_t);
	/* permissions */
	device_orientation_serv->serv_chars[1].value_permissions = (AT_BLE_ATTR_READABLE_NO_AUTHN_NO_AUTHR | AT_BLE_ATTR_WRITABLE_NO_AUTHN_NO_AUTHR);
	/* user defined name */
	device_orientation_serv->serv_chars[1].user_desc = NULL;
	device_orientation_serv->serv_chars[1].user_desc_len = 0;
	device_orientation_serv->serv_chars[1].user_desc_max_len = 0;
	/*user description permissions*/
	device_orientation_serv->serv_chars[1].user_desc_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
	/*client config permissions*/
	device_orientation_serv->serv_chars[1].client_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
	/*server config permissions*/
	device_orientation_serv->serv_chars[1].server_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
	/*user desc handles*/
	device_orientation_serv->serv_chars[1].user_desc_handle = 0;
	/*client config handles*/
	device_orientation_serv->serv_chars[1].client_config_handle = 0;
	/*server config handles*/
	device_orientation_serv->serv_chars[1].server_config_handle = 0;
	/* presentation format */
	device_orientation_serv->serv_chars[1].presentation_format = NULL;
	
	/*Characteristic Info for deviadverce rotation vector(Quaternion+) (X,Y,Z,W) Value*/
	
	/* handle stored here */
	device_orientation_serv->serv_chars[2].char_val_handle = 0;
	
	/* UUID : Accelero Y-Pos */
	device_orientation_serv->serv_chars[2].uuid.type = AT_BLE_UUID_128;
	memcpy(&device_orientation_serv->serv_chars[2].uuid.uuid[0], ROTATION_VECTOR_CHAR_UUID, 16);

	/* Properties */
	device_orientation_serv->serv_chars[2].properties = AT_BLE_CHAR_READ | AT_BLE_CHAR_NOTIFY;
	/* value */
	device_orientation_serv->serv_chars[2].init_value = (uint8_t*)&device_rotation_vector_initial_value ;
	device_orientation_serv->serv_chars[2].value_init_len = sizeof(device_rotation_vector_t);
	device_orientation_serv->serv_chars[2].value_max_len = sizeof(device_rotation_vector_t);
	/* permissions */
	device_orientation_serv->serv_chars[2].value_permissions = (AT_BLE_ATTR_READABLE_NO_AUTHN_NO_AUTHR | AT_BLE_ATTR_WRITABLE_NO_AUTHN_NO_AUTHR);
	/* user defined name */
	device_orientation_serv->serv_chars[2].user_desc = NULL;
	device_orientation_serv->serv_chars[2].user_desc_len = 0;
	device_orientation_serv->serv_chars[2].user_desc_max_len = 0;
	/*user description permissions*/
	device_orientation_serv->serv_chars[2].user_desc_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
	/*client config permissions*/
	device_orientation_serv->serv_chars[2].client_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
	/*server config permissions*/
	device_orientation_serv->serv_chars[2].server_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
	/*user desc handles*/
	device_orientation_serv->serv_chars[2].user_desc_handle = 0;
	/*client config handles*/
	device_orientation_serv->serv_chars[2].client_config_handle = 0;
	/*server config handles*/
	device_orientation_serv->serv_chars[2].server_config_handle = 0;
	/* presentation format */
	device_orientation_serv->serv_chars[2].presentation_format = NULL;
	
	/*Characteristic Info for output data rate(ODR) Value*/
	
	/* handle stored here */
	device_orientation_serv->serv_chars[3].char_val_handle = 0;
	
	/* UUID : Accelero Y-Pos */
	device_orientation_serv->serv_chars[3].uuid.type = AT_BLE_UUID_128;
	memcpy(&device_orientation_serv->serv_chars[3].uuid.uuid[0], MOTION_ODR_CHAR_UUID, 16);

	/* Properties */
	device_orientation_serv->serv_chars[3].properties = AT_BLE_CHAR_READ | AT_BLE_CHAR_WRITE;
	/* value */
	device_orientation_serv->serv_chars[3].init_value = (uint8_t*)&motion_odr_initial_value ;
	device_orientation_serv->serv_chars[3].value_init_len = sizeof(uint8_t);
	device_orientation_serv->serv_chars[3].value_max_len = sizeof(uint8_t);
	/* permissions */
	device_orientation_serv->serv_chars[3].value_permissions = (AT_BLE_ATTR_READABLE_NO_AUTHN_NO_AUTHR | AT_BLE_ATTR_WRITABLE_NO_AUTHN_NO_AUTHR);
	/* user defined name */
	device_orientation_serv->serv_chars[3].user_desc = NULL;
	device_orientation_serv->serv_chars[3].user_desc_len = 0;
	device_orientation_serv->serv_chars[3].user_desc_max_len = 0;
	/*user description permissions*/
	device_orientation_serv->serv_chars[3].user_desc_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
	/*client config permissions*/
	device_orientation_serv->serv_chars[3].client_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
	/*server config permissions*/
	device_orientation_serv->serv_chars[3].server_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
	/*user desc handles*/
	device_orientation_serv->serv_chars[3].user_desc_handle = 0;
	/*client config handles*/
	device_orientation_serv->serv_chars[3].client_config_handle = 0;
	/*server config handles*/
	device_orientation_serv->serv_chars[3].server_config_handle = 0;
	/* presentation format */
	device_orientation_serv->serv_chars[3].presentation_format = NULL;
	
	/*Characteristic Info for drop-down indication */
	
	/* handle stored here */
	device_orientation_serv->serv_chars[4].char_val_handle = 0;
	
	/* UUID : Accelero Drop Indication */
	device_orientation_serv->serv_chars[4].uuid.type = AT_BLE_UUID_128;
	memcpy(&device_orientation_serv->serv_chars[4].uuid.uuid[0], ACCELERO_DROP_IND_UUID, 16);

	/* Properties */
	device_orientation_serv->serv_chars[4].properties = AT_BLE_CHAR_READ | AT_BLE_CHAR_NOTIFY;
	/* value */
	device_orientation_serv->serv_chars[4].init_value = &accelero_drop_down_ind_initial_value ;
	device_orientation_serv->serv_chars[4].value_init_len = sizeof(uint8_t);
	device_orientation_serv->serv_chars[4].value_max_len = sizeof(uint8_t);
	/* permissions */
	device_orientation_serv->serv_chars[4].value_permissions = (AT_BLE_ATTR_READABLE_NO_AUTHN_NO_AUTHR | AT_BLE_ATTR_WRITABLE_NO_AUTHN_NO_AUTHR);
	/* user defined name */
	device_orientation_serv->serv_chars[4].user_desc = NULL;
	device_orientation_serv->serv_chars[4].user_desc_len = 0;
	device_orientation_serv->serv_chars[4].user_desc_max_len = 0;
	/*user description permissions*/
	device_orientation_serv->serv_chars[4].user_desc_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
	/*client config permissions*/
	device_orientation_serv->serv_chars[4].client_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
	/*server config permissions*/
	device_orientation_serv->serv_chars[4].server_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
	/*user desc handles*/
	device_orientation_serv->serv_chars[4].user_desc_handle = 0;
	/*client config handles*/
	device_orientation_serv->serv_chars[4].client_config_handle = 0;
	/*server config handles*/
	device_orientation_serv->serv_chars[4].server_config_handle = 0;
	/* presentation format */
	device_orientation_serv->serv_chars[4].presentation_format = NULL;

	/*Characteristic Info for step-increment indication */
	
	/* handle stored here */
	device_orientation_serv->serv_chars[5].char_val_handle = 0;
	
	/* UUID : Accelero Step Increment indication */

	device_orientation_serv->serv_chars[5].uuid.type = AT_BLE_UUID_128;
	memcpy(&device_orientation_serv->serv_chars[5].uuid.uuid[0], ACCELERO_STEP_INC_IND_UUID, 16);

	/* Properties */
	device_orientation_serv->serv_chars[5].properties = AT_BLE_CHAR_READ | AT_BLE_CHAR_NOTIFY;
	/* value */
	device_orientation_serv->serv_chars[5].init_value = &accelero_step_inc_ind_initial_value ;
	device_orientation_serv->serv_chars[5].value_init_len = sizeof(uint8_t);
	device_orientation_serv->serv_chars[5].value_max_len = sizeof(uint8_t);
	/* permissions */
	device_orientation_serv->serv_chars[5].value_permissions = (AT_BLE_ATTR_READABLE_NO_AUTHN_NO_AUTHR | AT_BLE_ATTR_WRITABLE_NO_AUTHN_NO_AUTHR);
	/* user defined name */
	device_orientation_serv->serv_chars[5].user_desc = NULL;
	device_orientation_serv->serv_chars[5].user_desc_len = 0;
	device_orientation_serv->serv_chars[5].user_desc_max_len = 0;
	/*user description permissions*/
	device_orientation_serv->serv_chars[5].user_desc_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
	/*client config permissions*/
	device_orientation_serv->serv_chars[5].client_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
	/*server config permissions*/
	device_orientation_serv->serv_chars[5].server_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
	/*user desc handles*/
	device_orientation_serv->serv_chars[5].user_desc_handle = 0;
	/*client config handles*/
	device_orientation_serv->serv_chars[5].client_config_handle = 0;
	/*server config handles*/
	device_orientation_serv->serv_chars[5].server_config_handle = 0;
	/* presentation format */
	device_orientation_serv->serv_chars[5].presentation_format = NULL;
}

/**@brief Battery service and characteristic initialization
*
*/
static void init_battery_service(gatt_battery_service_handler_t *battery_serv)
{
	battery_serv->serv_handle = 0;

	battery_serv->serv_uuid.type = AT_BLE_UUID_128;
	memcpy(&battery_serv->serv_uuid.uuid[0], BATTERY_SERVICE_UUID, 16);
	
	/* handle stored here */
	battery_serv->serv_chars.char_val_handle = 0;
	
	/* UUID : Battery Level */
	battery_serv->serv_chars.uuid.type = AT_BLE_UUID_128;
	memcpy(&battery_serv->serv_chars.uuid.uuid[0], BATTERY_LEVEL_CHAR_UUID, 16);

	/* Properties */
	battery_serv->serv_chars.properties = AT_BLE_CHAR_READ | AT_BLE_CHAR_NOTIFY;
	/* value */
	battery_serv->serv_chars.init_value = &battery_level_initial_value ;
	battery_serv->serv_chars.value_init_len = sizeof(uint8_t);
	battery_serv->serv_chars.value_max_len = sizeof(uint8_t);
	/* permissions */
	battery_serv->serv_chars.value_permissions = (AT_BLE_ATTR_READABLE_NO_AUTHN_NO_AUTHR | AT_BLE_ATTR_WRITABLE_NO_AUTHN_NO_AUTHR);
	/* user defined name */
	battery_serv->serv_chars.user_desc = NULL;
	battery_serv->serv_chars.user_desc_len = 0;
	battery_serv->serv_chars.user_desc_max_len = 0;
	/*user description permissions*/
	battery_serv->serv_chars.user_desc_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
	/*client config permissions*/
	battery_serv->serv_chars.client_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
	/*server config permissions*/
	battery_serv->serv_chars.server_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
	/*user desc handles*/
	battery_serv->serv_chars.user_desc_handle = 0;
	/*client config handles*/
	battery_serv->serv_chars.client_config_handle = 0;
	/*server config handles*/
	battery_serv->serv_chars.server_config_handle = 0;
	/* presentation format */
	battery_serv->serv_chars.presentation_format = NULL;
}



/**
 * \brief Initialization of Sensor tag profile services
 */
static void wbe_service_init(void)
{
	/* Initialize environment service */
	init_environment_service(&environment_service_handle);
	/*Initialize Device orientation Service */
	init_device_orientation_service(&device_orientation_service_handle);
	/*Initialize battery Service */
	init_battery_service(&battery_service_handle);
		
}

/**
* \@brief initializes and defines wearable demo profile services and advertisement data
*/

void wbe_profile_init(void)
{
	/* st services initialization*/
	wbe_service_init();
	
	/* st services definition		*/
	wbe_service_define();
	
	/* set the advertisement data */
	wbe_advertisement_data_set();
	
	/* start the advertisement */
	wbe_start_advertisement();
		
	/* Register callbacks for gap related events */
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
	BLE_GAP_EVENT_TYPE,
	&sensor_tag_app_gap_cb);
		
	/* Register callbacks for gatt server related events */
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
	BLE_GATT_SERVER_EVENT_TYPE,
	&sensor_tag_app_gatt_server_cb);

	/* Register callbacks for user defined custom events */
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
	BLE_CUSTOM_EVENT_TYPE,
	&sensor_tag_custom_event_handle);
	
	/* Initiate battery voltage measurement */
	at_ble_calib_config(1,5,30);

	/* Hardware timer */
	/* Hardware timer initialization */
	hw_timer_init();
	wakeup_int_register_callback(DUAL_TIME_CALLBACK, timekeeping_cb);
	/* Register callback for timer */
	hw_timer_register_callback(timer_callback);
}
