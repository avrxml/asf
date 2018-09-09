/**
 * \file
 *
 * \brief Heart Rate Sensor Application
 *
 * Copyright (c) 2016-2018 Microchip Technology Inc. and its subsidiaries.
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
 /**
 * \mainpage Heart Rate Application
 * \section Introduction
 * ******************************Introduction ***********************************
 *
 * The Heart Rate example application bring-up the Heart Rate profile defined by
 * the Bluetooth SIG which would send the Heart Rate information to BLE enabled collector device.
 * The Heart rate Profile enables the collector device (GATT Client) to connect 
 * and interact with a Heart Rate Sensor (GATT Server) for use in fitness 
 * applications. The Heart rate sensor sends the heart rate measurement in bpm,
 * energy expended in kilojoules and R-R intervals in seconds. In addition to 
 * the heart rate service the heart rate profile also implements the Device 
 * Information Service, which provides the information about the Heart Rate Sensor Device.
 * + Features -
 *   + Device Discovery and Disconnection
 *   + Pairing / Bonding
 *   + Heart Rate Senor Measurements
 *   + Console Display
 * 
 * - Running the demo -
 *  + 1. Build and flash the binary into supported evaluation board.
 *  + 2. Open the console using TeraTerm or any serial port monitor.
 *  + 3. Press the Reset button.
 *  + 4. Wait for around 10 seconds for the patches to be downloaded device will initialize and start-up.
 *  + 5. The device is now in advertising mode.
 *  + 6. On a BLE compatible iPhone®/Android phone, enable Bluetooth in the Settings
 *       page. Start the 'Atmel Smart Connect Application' and scan for devices. ATMEL-HRP 
 *       will be appear among the devices scanned. Click on **ATMEL-HRP** to connect
 *       to supported platform.
 *  + 7. Once connected, the client side will request for the pairing procedure.
 *       The console log provides a guidance to the user to enter the pass-key.
 *  + 8. Once the device is connected, the supported services of Heart Rate and Device Information will be displayed.
 *  + 9. Once the notifications are enabled the HRM values are displayed as shown in the console and the corresponding mobile app. 
 *  + 10. User can turn off notification by clicking on Stop Notify.
 *
 * \section Modules
 * ***************************** MODULES ***************************************** 
 * - BLE Manager - 
 *  + The Event Manager is responsible for handling the following:
 *    + Generic BLE Event Handling:-
 *       + BLE Event Manager handles the events triggered by BLE stack and also responsible 
 *  	 for invoking all registered callbacks for respective events. BLE Manager 
 *  	 handles all GAP related functionality. In addition to that handles multiple connection 
 *  	 instances, Pairing, Encryption, Scanning.
 *    + Handling Multi-role/multi-connection:-
 *  	  + BLE Event Manager is responsible for handling multiple connection instances 
 *  	  and stores bonding information and Keys to retain the bonded device. 
 *  	  BLE Manager is able to identify and remove the device information when pairing/encryption 
 *		  gets failed. In case of multi-role, it handles the state/event handling of both central and peripheral in multiple contexts.
 *    + Controlling the Advertisement data:-
 *  	  + BLE Event Manager is responsible for generating the advertisement and scan response data
 *  	  for BLE profiles/services that are attached with BLE Manager.
 *
 * - BLE Profile-
 *  The Heart Rate service exposes heart rate and other data from a Heart Rate 
 *  Sensor intended for fitness applications.
 *  + Heart Rate service has three characteristics:
 *    + Heart Rate Measurement -
 *      + This characteristic is used to send a heart rate measurement.
 *    + Body Sensor Location -
 *      + The Body Sensor Location characteristic of the device is used to 
 *        describe the intended location of the heart rate measurement for the device..
 *    + Heart Rate Control Point -
 *      + The Heart Rate Control Point characteristic is used to enable Client 
 *        to write control points to a Server to control behaviour.
 *  
 * - BLE Platform Services -
 *  +  Serial Console COM port settings -
 *    + Baudrate 115200
 *	  + Parity None, Stop Bit 1, Start Bit 1
 *	  + No Hardware Handshake
 *
 * \section BluSDK Package
 * ***************************** BluSDK Package *****************************************
 * - Links for Docs -
 *		+ http://www.microchip.com/wwwproducts/en/ATSAMB11
 *		+ http://www.microchip.com/developmenttools/productdetails.aspx?partno=atsamb11-xpro
 *- Support and FAQ - visit -
 *		+ <a href="https://www.microchip.com/support/">Microchip Support</a>
 */
 
/****************************************************************************************
*							        Includes	
*                                       *
****************************************************************************************/
#include <asf.h>
#include "platform.h"
#include "at_ble_api.h"
#include "console_serial.h"
#include "timer_hw.h"
#include "button.h"
#include "led.h"
#include "ble_manager.h"
#include "ble_utils.h"
#include "hr_sensor_app.h"
#include "hr_sensor.h"
#include "samb11_delay.h"

/****************************************************************************************
*							        Globals		
*                                       *
****************************************************************************************/
volatile bool app_state = 0 ; /*!< flag to represent the application state*/
volatile bool start_advertisement = 0; /*!< flag to start advertisement*/
volatile bool advertisement_flag = false;/*!< to check if the device is in advertisement*/
volatile bool notification_flag = false; /*!< flag to start notification*/
volatile bool disconnect_flag = false;	/*!< flag for disconnection*/
volatile bool hr_initializer_flag = 1; /*!< flag for initialization of hr for each category*/
volatile bool notification_sent = true;
uint8_t second_counter = 0;	/*!< second_counter to count the time*/
uint16_t energy_expended_val = ENERGY_EXP_NORMAL; /*!< to count the energy expended*/
uint16_t energy_incrementor ;	/*!< energy incrementor for various heart rate values*/
uint16_t heart_rate_value = HEART_RATE_MIN_NORM; /*!< to count the heart rate value*/
uint16_t rr_interval_value = RR_VALUE_MIN; /*!< to count the rr interval value*/
uint8_t activity = 0; /*!< activiy which will determine the */
uint8_t prev_activity = DEFAULT_ACTIVITY;/*!< previous activity */
int8_t inc_changer	= 1;/*!< increment operator to change heart rate */
int8_t time_operator ;/*!< operator to change the seconds */
uint8_t hr_min_value;/*!<the minimum heart rate value*/
uint8_t hr_max_value;/*!<the maximum heart rate value*/
uint8_t energy_inclusion = 0;/*!<To check for including the energy in hr measurement*/
volatile bool app_init_done = false; 
extern bool button_debounce;

static const ble_gap_event_cb_t app_gap_handle = {
	.connected = app_connected_event_handler,
	.disconnected = app_disconnected_event_handler
};

static const ble_gatt_server_event_cb_t app_gatt_server_handle = {
	.notification_confirmed = app_notification_cfm_handler
};


/****************************************************************************************
*							        Functions											*
****************************************************************************************/

/** @brief notification handler function called by the profile
 *	@param[in] notification_enable which will tell the state of the
 *  application
 */
static void app_notification_handler(uint8_t notification_enable)
{
	if (notification_enable == true) {
		DBG_LOG("Notification Enabled");
		hw_timer_start(NOTIFICATION_INTERVAL);
	} else {
		hw_timer_stop();
		notification_flag = false;
		DBG_LOG("Notification Disabled");
	}
}

/** @brief hr_notification_confirmation_handler called by ble manager 
 *	to give the status of notification sent
 *  @param[in] at_ble_cmd_complete_event_t address of the cmd completion
 */	
at_ble_status_t app_notification_cfm_handler(void *params)
{
	at_ble_cmd_complete_event_t event_params;
	memcpy(&event_params,params,sizeof(at_ble_cmd_complete_event_t));
	if (event_params.status == AT_BLE_SUCCESS) {
		DBG_LOG_DEV("App Notification Successfully sent over the air");
		notification_sent = true;
	} else {
		DBG_LOG_DEV("Sending Notification over the air failed");
		notification_sent = false;
	}
	return AT_BLE_SUCCESS;
}

/** @brief energy expended handler called by profile to reset the energy values
 *
 */

static void app_reset_handler(void)
{
	energy_expended_val = 0;
	DBG_LOG("Energy Expended is made '0'on user Reset");
}

/** @brief heart_rate_value_init will initializes the heart rate values
 *	 for simulation.
 *	 Based on the time different heart rate values are chosen to indicate
 *	 different activity.
 */
static void heart_rate_value_init(void)
{
	activity = second_counter / 40;

	if (activity != prev_activity) {		
		switch(activity) {
		case ACTIVITY_NORMAL:
			hr_min_value = HEART_RATE_MIN_NORM;
			hr_max_value = HEART_RATE_MAX_NORM;
			heart_rate_value = hr_min_value;
			energy_incrementor = ENERGY_EXP_NORMAL;
			break;
			
		case ACTIVITY_WALKING:
			hr_min_value = HEART_RATE_MIN_WALKING;
			hr_max_value = HEART_RATE_MAX_WALKING;
			heart_rate_value = hr_min_value;
			energy_incrementor = ENERGY_EXP_WALKING;
			break;
			
		case ACTIVITY_BRISK_WALKING:
			hr_min_value = HEART_RATE_MIN_BRISK_WALK;
			hr_max_value = HEART_RATE_MAX_BRISK_WALK;
			heart_rate_value = hr_min_value;
			energy_incrementor = ENERGY_EXP_BRISK_WALKING;
			break;
			
		case ACTIVITY_RUNNING:
			hr_min_value = HEART_RATE_MIN_RUNNING;
			hr_max_value = HEART_RATE_MAX_RUNNING;
			heart_rate_value = hr_min_value;
			energy_incrementor = ENERGY_EXP_RUNNING;
			break;
			
		case ACTIVITY_FAST_RUNNING:
			hr_min_value = HEART_RATE_MIN_FAST_RUNNING;
			hr_max_value = HEART_RATE_MAX_FAST_RUNNING;
			heart_rate_value = hr_min_value;
			energy_incrementor = ENERGY_EXP_FAST_RUNNING;
			break;
		}
		prev_activity = activity;
	}
	
	if (heart_rate_value == hr_max_value) {
		inc_changer = -1;
	} else if (heart_rate_value == hr_min_value) {
		inc_changer = 1;
	}
}

/** @brief connected state handler
 *  @param[in] status of the application
 */
static at_ble_status_t app_connected_event_handler(void *params)
{
	app_state = true;
	LED_On(LED0);
	DBG_LOG("Enable the notification in app to listen "
	"heart rate or press the button to disconnect");
	advertisement_flag = false;
	notification_sent = true;
        ALL_UNUSED(params);
	return AT_BLE_SUCCESS;
}

static at_ble_status_t app_disconnected_event_handler(void *params)
{
	app_state = false;
	hw_timer_stop();
	notification_flag = false;
	energy_expended_val = ENERGY_EXP_NORMAL;
	second_counter = 0;
	activity = ACTIVITY_NORMAL;
	prev_activity = DEFAULT_ACTIVITY;
	energy_inclusion = 0;
	heart_rate_value_init();
	LED_Off(LED0);
	DBG_LOG("Press button to advertise");
	ALL_UNUSED(params);
	return AT_BLE_SUCCESS;
}

/** @brief hr_measurment_send sends the notifications after adding the hr values
 *	heart rate values starts @60bpm increments by 1 goes upto 255 bpm and
 *	restarts @60
 *  Energy Expended will be sent on every 10th notification,it starts @ 0 and
 *	increments by 20
 *  rr interval values, two rr interval values will be sent in every
 *	notification
 */
static void hr_measurment_send(void)
{
	uint8_t hr_data[HR_CHAR_VALUE_LEN];
	uint8_t idx = 0;
	
	if ((energy_expended_val == ENERGY_RESET) || (second_counter % 10 == energy_inclusion)) {
		hr_data[idx] = (RR_INTERVAL_VALUE_PRESENT | ENERGY_EXPENDED_FIELD_PRESENT);
		
		/* To send energy expended after 10 notifications after reset */
		if (energy_expended_val == ENERGY_RESET) {
			energy_inclusion = second_counter % 10 ;
		}
	} else {
		hr_data[idx] = RR_INTERVAL_VALUE_PRESENT ;
	}
	idx += 1;			
	DBG_LOG("Heart Rate: %d bpm", heart_rate_value);
	heart_rate_value += (inc_changer);

	/* Heart Rate Value 8bit*/
	hr_data[idx++] = (uint8_t)heart_rate_value ;
	if (hr_data[0] & ENERGY_EXPENDED_FIELD_PRESENT) {
		memcpy(&hr_data[idx], &energy_expended_val, 2);
		idx += 2;	
	}
	
	/* Appending RR interval values*/	
	if (rr_interval_value >= RR_VALUE_MAX) {
		rr_interval_value = (uint8_t) RR_VALUE_MIN; 
	}	
	DBG_LOG_CONT("\tRR Values:(%d,%d)msec",
				rr_interval_value, rr_interval_value + 200);
	memcpy(&hr_data[idx], &rr_interval_value, 2);
	idx += 2;
	rr_interval_value += 200;
	memcpy(&hr_data[idx], &rr_interval_value, 2);
	idx += 2;
	rr_interval_value += 200;
	
	/*printing the user activity,simulation*/
	switch(activity) {
	case ACTIVITY_NORMAL:
		DBG_LOG_CONT(" User Status:Idle");
		break;
		
	case ACTIVITY_WALKING:
		DBG_LOG_CONT(" User Status:Walking");
		break;
		
	case ACTIVITY_BRISK_WALKING:
		DBG_LOG_CONT(" User status:Brisk walking");
		break;
		
	case ACTIVITY_RUNNING:
		DBG_LOG_CONT(" User status:Running");
		break;
		
	case ACTIVITY_FAST_RUNNING:
		DBG_LOG_CONT(" User Status:Fast Running");
		break;	
	}
	
	/* Printing the energy*/
	if ((hr_data[0] & ENERGY_EXPENDED_FIELD_PRESENT)) {
		DBG_LOG("Energy Expended :%d KJ\n", energy_expended_val);
		energy_expended_val += energy_incrementor;
	}
	
	/* Sending the data for notifications*/
	hr_sensor_send_notification(hr_data, idx);
}



/* Callback registered for AT_PLATFORM_EVENT event from stack */
static at_ble_status_t platform_interrupt_app_event(void *param)
{
	at_ble_status_t status = AT_BLE_SUCCESS;
	platform_isr_event_t *plf_isr_event = (platform_isr_event_t *)param;
	if((start_advertisement == true || disconnect_flag == true) && (plf_isr_event->event_type == ((PORTINT_CALLBACK_TYPE_DETECT << 8)  | RAM_ISR_TABLE_PORT0_COMB_INDEX)))
		{/*delay_ms(350);*/}
	
	/* Flag to start notification */
	if ((plf_isr_event->event_type == ((TIMER_EXPIRED_CALLBACK_TYPE_DETECT << 8)  | RAM_ISR_TABLE_TIMER0_INDEX)) && notification_flag )
	{
		LED_Toggle(LED0);
		if (notification_sent) {
			hr_measurment_send();
			} else {
			DBG_LOG("Previous notification not sent");
			status = AT_BLE_FAILURE;
		}
		notification_flag = false;		
	}
	if((plf_isr_event->event_type == ((PORTINT_CALLBACK_TYPE_DETECT << 8)  | RAM_ISR_TABLE_PORT0_COMB_INDEX)) && start_advertisement )
	{
		hr_sensor_adv();
		start_advertisement = false;
	}
	/* Flag to disconnect with the peer device */
	if((plf_isr_event->event_type == ((PORTINT_CALLBACK_TYPE_DETECT << 8)  | RAM_ISR_TABLE_PORT0_COMB_INDEX)) && disconnect_flag )
	{
		hr_sensor_disconnect();
		app_state = false;
		disconnect_flag = false;
	}
	delay_ms(200);
	button_debounce = true;
	return status;		
}
/**
 * \brief Timer callback handler called on timer expiry
 */
static void timer_callback_handler(void)
{
	if(!app_init_done)
	{
		//do nothing
	}
	else
	{
		if (second_counter == START_OF_FIRST_ACTIVITY) 
			time_operator = 1;
		else if (second_counter == END_OF_LAST_ACTIVITY) 
			time_operator = -1;
		second_counter += (time_operator);
		heart_rate_value_init();
		notification_flag = true;
		send_plf_int_msg_ind(RAM_ISR_TABLE_TIMER0_INDEX, TIMER_EXPIRED_CALLBACK_TYPE_DETECT, NULL, 0);
	}
}

static void button_cb(void)
{
	if(!app_init_done)
	{
		//do nothing
	}
	else
	{
		if(app_state)
		{
			DBG_LOG_DEV("Going to disconnect ");
			disconnect_flag = true;
		} 
		else if (app_state == false && advertisement_flag == false) 
		{
			/* To check if the device is in advertisement */
			DBG_LOG_DEV("Going to advertisement");
			start_advertisement = true;
			advertisement_flag = true;
		}
		send_plf_int_msg_ind(RAM_ISR_TABLE_PORT0_COMB_INDEX, PORTINT_CALLBACK_TYPE_DETECT, NULL, 0);
	}
}

/* Custom events like user defined event, platform event callback handlers for OTAU profile */
static const ble_custom_event_cb_t hr_sensor_app_custom_event_handle = {
	.platform_event_ready = platform_interrupt_app_event /* This event not handled in BLE Manager */
};
/* to make app executing continuously*/
bool app_exec = true;
/**
 * \brief Heart Rate Sensor Application main function
 */
int main(void)
{
	app_state = 0;  /*!< flag to represent the application state*/
	start_advertisement = 0; /*!< flag to start advertisement*/
	advertisement_flag = false; /*!< to check if the device is in advertisement*/
	notification_flag = false; /*!< flag to start notification*/
	disconnect_flag = false;        /*!< flag for disconnection*/
	hr_initializer_flag = 1; /*!< flag for initialization of hr for each category*/
	second_counter = 0;     /*!< second_counter to count the time*/
	energy_expended_val = ENERGY_EXP_NORMAL; /*!< to count the energy expended*/
	energy_incrementor = 0;
	heart_rate_value = HEART_RATE_MIN_NORM; /*!< to count the heart rate value*/
	rr_interval_value = RR_VALUE_MIN; /*!< to count the rr interval value*/
	activity = 0; /*!< activiy which will determine the */
	prev_activity = DEFAULT_ACTIVITY; /*!< previous activity */
	inc_changer     = 1; /*!< increment operator to change heart rate */
	time_operator = 0; /*!< operator to change the seconds */
	hr_min_value = 0; /*!<the minimum heart rate value*/
	hr_max_value = 0; /*!<the maximum heart rate value*/

	app_exec = true;

	platform_driver_init();
	acquire_sleep_lock();

	/* Initialize the button */
	gpio_init();
	button_init();
	button_register_callback(button_cb);

	/* Initialize the LED */
	led_init();

	/* Initialize serial console */
	serial_console_init();

	/* Initialize the hardware timer */
	hw_timer_init();

	/* Register the callback */
	hw_timer_register_callback(timer_callback_handler);

	DBG_LOG("Initializing Heart Rate Sensor Application");

	/* initialize the ble chip  and Set the device mac address */
	ble_device_init(NULL);

	app_init_done = true;	
	/* Initialize the profile */
	hr_sensor_init(NULL);
	
	DBG_LOG("Press the button to start advertisement");

	/* Registering the app_notification_handler with the profile */
	register_hr_notification_handler(app_notification_handler);

	/* Registering the app_reset_handler with the profile */
	register_hr_reset_handler(app_reset_handler);
	
	/* Registering the call backs for events with the ble manager */
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
	BLE_GAP_EVENT_TYPE,
	&app_gap_handle);
	
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
	BLE_GATT_SERVER_EVENT_TYPE,
	&app_gatt_server_handle);
	
	/* Register callbacks for user defined custom events */
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
	BLE_CUSTOM_EVENT_TYPE,
	&hr_sensor_app_custom_event_handle);

	/* Capturing the events  */
	while (app_exec) {
		/* BLE Event Task */
		ble_event_task(0xFFFFFFFF);
	}
	return 0;
}