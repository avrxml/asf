/**
* \file
*
* \brief Proximity Reporter Profile Application
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
*
*/
/*
* Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
*/

/****************************************************************************************
*							        Includes	                                     	*
****************************************************************************************/


#include <asf.h>
#include "console_serial.h"
#include "at_ble_api.h"
#include "platform.h"
#include "timer_hw.h"
#include "conf_extint.h"
#include "ble_utils.h"
#include "ble_manager.h"
#include "immediate_alert.h"
#include "link_loss.h"
#include "tx_power.h"
#include "pxp_reporter.h"
#include "pxp_reporter_app.h"



/* === GLOBALS ============================================================ */
/* PXP Application LED State */
bool pxp_led_state = true;

/** @brief Timer interval for timer used for led blinking */
uint8_t timer_interval = INIT_TIMER_INTERVAL;

/* To make the app execute*/
bool app_exec = true;

/**
* \brief Timer callback handler called on timer expiry
*/
void timer_callback_handler(void)
{
	hw_timer_stop();
	if (pxp_led_state) {
		pxp_led_state = false;
		LED_Off(LED0);
		hw_timer_start(timer_interval);
	} else {
		pxp_led_state = true;
		LED_On(LED0);
		hw_timer_start(timer_interval);
	}
}


/**
 * @brief function called by the profile to notify the app connected
 *
 * @param[in] state true for notification
 *
 */
static void app_connected_state(bool state)
{
	if (state) {
		hw_timer_stop();
		LED_Off(LED0);
		pxp_led_state = 0;
	} 
}

/**
 * @brief Alerting function on link loss alert 
 *
 * @param[in] alert level of alert level characteristic of linkloss service
 *
 */
static void app_pathloss_alert(uint8_t alert_val)
{
	
		if (alert_val == IAS_HIGH_ALERT) {
			DBG_LOG("Pathloss : High Alert");
			timer_interval = PL_INTERVAL_FAST;
			LED_On(LED0);
			hw_timer_start(timer_interval);
		} else if (alert_val == IAS_MID_ALERT) {
			DBG_LOG("Pathloss : Mild Alert");
			timer_interval = PL_INTERVAL_MEDIUM;
			LED_On(LED0);
			hw_timer_start(timer_interval);
		} else if (alert_val == IAS_NO_ALERT) {
			DBG_LOG("Pathloss : No Alert");
			hw_timer_stop();
			LED_Off(LED0);
			pxp_led_state = 0;
		} 
}

/**
 * @brief Alerting function on link loss alert 
 *
 * @param[in] alert level of alert level characteristic of linkloss service
 *
 */
static void app_linkloss_alert(uint8_t alert_val)
{
	if (alert_val == LLS_NO_ALERT) {
		DBG_LOG("Link loss : No Alert  ");
		hw_timer_stop();
		LED_Off(LED0);
		pxp_led_state = 0;
	} else if (alert_val == LLS_MILD_ALERT) {
		DBG_LOG("Link loss : Mild Alert  ");
		timer_interval = LL_INTERVAL_MEDIUM;
		LED_On(LED0);
		hw_timer_start(timer_interval);
	} else if (alert_val == LLS_HIGH_ALERT) {
		DBG_LOG("Link loss : High Alert ");
		timer_interval = LL_INTERVAL_FAST;
		LED_On(LED0);
		hw_timer_start(timer_interval);
	}	
}

/**
* \brief Proximity Reporter Application main function
*/
int main(void)
{
	#if SAMG55 || SAM4S
	/* Initialize the SAM system. */
	sysclk_init();
	board_init();
	#elif SAM0
	system_init();
	#endif
	
	/* Initialize serial console */
	serial_console_init();
	
	/* Initialize the hardware timer */
	hw_timer_init();
	
	/* Register the callback */
	hw_timer_register_callback(timer_callback_handler);
	
	DBG_LOG("Initializing Proximity Reporter Application");
	
	/* initialize the ble chip  and Set the device mac address */
	ble_device_init(NULL);
	
	pxp_reporter_init(NULL);
	
	DBG_LOG("Proximity Reporter Initializing Completed");
	
	register_pathloss_handler(app_pathloss_alert);
	
	register_linkloss_handler(app_linkloss_alert);
	
	register_state_handler(app_connected_state);
		
	/* Capturing the events  */ 
	while(app_exec)
	{
		ble_event_task();
	}
	
	return 0;
}
