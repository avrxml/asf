/*******************************************************************************
 *
 * Copyright (c) 2015 Proximetry Inc. All rights reserved.
 * Authors: Proximetry
 * All Rights Reserved
 *
 ******************************************************************************/
/**
 * @file diagnostic_button.c
 * @brief
 * */
#include <asf.h>
#include "common.h"
#include "timer.h"
#include "amp_agent.h"
#include "diagnostic_button.h"
#include "ip64-addr.h"
#include "simple-rpl.h"

#define DIAGNOSTIC_STATUS_WIFI_DISCONNECTED_PERIOD  (100/5/2)  // 5 Hz
#define DIAGNOSTIC_STATUS_WIFI_CONNECTED_PERIOD     (100/2/2)  // 2 Hz

enum diagnostic_diode {
	DIAGNOSTIC_INACTIVE = 0, // meas diode is controlled by EM
	DIAGNOSTIC_REQUESTED,
	DIAGNOSTIC_PRE_DELAY,
	DIAGNOSTIC_ACTIVE,
	DIAGNOSTIC_POST_DELAY,
};

enum diagnostic_status {
	DIAGNOSTIC_STATUS_UNKNOWN = 0,
	DIAGNOSTIC_STATUS_LINK_DISCONNECTED,
	DIAGNOSTIC_STATUS_LINK_CONNECTED,
	DIAGNOSTIC_STATUS_AMP_PROTOCOL_SYNCED,
};


/* diagnostic diode stuff */
static struct timer  diagnostic_timer, diagnostic_blinker;
static enum diagnostic_diode diode_function;
static enum diagnostic_status diode_status;

/**
 * @brief diagnostic_button_service_is_active
 *
 * @return true if diagnostic button service is active
 */
bool diagnostic_button_service_is_active(void)
{
	return (diode_function != DIAGNOSTIC_INACTIVE);
}


/**
 * @brief Diagnostic button led service
 *
 * Handle diagnostic button led blinking.
 * Should be issued with 1ms timer interval.
 */
void diagnostic_button_led_service(void)
{
	if ((DIAGNOSTIC_STATUS_LINK_DISCONNECTED == diode_status) ||
		(DIAGNOSTIC_STATUS_LINK_CONNECTED == diode_status))
	{
		if (timer_expired(&diagnostic_blinker))
		{
			timer_restart(&diagnostic_blinker);

			#if (SAMD20 || SAMD21 || SAMR21 )
			port_pin_toggle_output_level(LED_0_PIN);
			#elif SAM4S
			ioport_toggle_pin_level(LED_0_PIN);
			#endif
		}
	}
}


/**
 * @brief Diagnostic button is requested
 *
 * Checks if diagnostic_button is requested.
 */
static bool diagnostic_button_is_requested(void)
{

#if (SAMD20 || SAMD21 || SAMR21 )
	if (port_pin_get_input_level(BUTTON_0_PIN) == BUTTON_0_ACTIVE)
#elif SAM4S
	if (ioport_get_pin_level(BUTTON_0_PIN) == BUTTON_0_ACTIVE)
#endif
	{
		if (diode_function == DIAGNOSTIC_INACTIVE)
		{
			diode_function = DIAGNOSTIC_REQUESTED;
			return true;
		}
	}

	return false;
}

#define DELAY_500MS     50
#define DELAY_5000MS   500

/**
 * @brief Diagnostic button service
 *
 * Handle diagnostic button events.
 * Should be issued in main loop as often as possible
 */
void diagnostic_button_service(void)
{

	diagnostic_button_is_requested();

	switch (diode_function)
	{
		case DIAGNOSTIC_REQUESTED:
		{
			timer_set(&diagnostic_timer, DELAY_500MS);     // 0,5 [sec] for pre_delay
			#if (SAMD20 || SAMD21 || SAMR21 )
			port_pin_set_output_level(LED_0_PIN,1);
			#elif SAM4S
			ioport_set_pin_level(LED_0_PIN,1);
			#endif
			diode_function = DIAGNOSTIC_PRE_DELAY;
			/* set diagnostic diode status once when requested */
			if ( simple_rpl_parent() != NULL )
			{
				if (amp_agent_is_connected())
				{
					LOG("DIAGNOSTIC BUTTON: end node agent connected to Cloud Server. =============" EOL);
					diode_status = DIAGNOSTIC_STATUS_AMP_PROTOCOL_SYNCED;
				}
				else
				{
					LOG("DIAGNOSTIC BUTTON: end node connected with BR only. ======" EOL);
					diode_status = DIAGNOSTIC_STATUS_LINK_CONNECTED;
				}
			}
			else
			{
				LOG("DIAGNOSTIC BUTTON: end node disconnected from BR.=========" EOL);
				diode_status = DIAGNOSTIC_STATUS_LINK_DISCONNECTED;
			}
		}
		break;
		case DIAGNOSTIC_PRE_DELAY:
		{
			if (timer_expired(&diagnostic_timer))
			{
				timer_set(&diagnostic_timer, DELAY_5000MS); // 5,0 [sec] for active
				#if (SAMD20 || SAMD21 || SAMR21 )
				port_pin_set_output_level(LED_0_PIN,0);
				#elif SAM4S
				ioport_set_pin_level(LED_0_PIN,0);
				#endif
				diode_function = DIAGNOSTIC_ACTIVE;
				switch (diode_status)
				{
					case DIAGNOSTIC_STATUS_LINK_DISCONNECTED:
						timer_set(&diagnostic_blinker, DIAGNOSTIC_STATUS_WIFI_DISCONNECTED_PERIOD);
					break;
					case DIAGNOSTIC_STATUS_LINK_CONNECTED:
						timer_set(&diagnostic_blinker, DIAGNOSTIC_STATUS_WIFI_CONNECTED_PERIOD);
					break;
					default:
					;
					break;
				}
			}
		}
		break;
		case DIAGNOSTIC_ACTIVE:
		{
			if (timer_expired(&diagnostic_timer))
			{
				timer_set(&diagnostic_timer, DELAY_500MS);     // 0,5 [sec] for post_delay
				#if (SAMD20 || SAMD21 || SAMR21 )
				port_pin_set_output_level(LED_0_PIN,1);
				#elif SAM4S
				ioport_set_pin_level(LED_0_PIN,1);
				#endif
				diode_function = DIAGNOSTIC_POST_DELAY;
			}
		}
		break;
		case DIAGNOSTIC_POST_DELAY:
		{
			if (timer_expired(&diagnostic_timer))
			{
				diode_function = DIAGNOSTIC_INACTIVE;
			}
		}
		break;
		default:
		break;
	}
}


