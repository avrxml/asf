/*****************************************************************************
 *
 * Configures application-specific settings
 * Copyright (c) 2015 Proximetry Inc. All rights reserved.
 * Authors: Proximetry
 * All Rights Reserved
 *
 ******************************************************************************/


#ifndef CONF_APP_H
#define CONF_APP_H

#define LED_BLINK_FREQUENCY 5  //Hz
#define LED_TOGGLE_INTERVAL (100/LED_BLINK_FREQUENCY/2)	// 1000ms/freq/2 - led toggle interval

#define APP_VER "0.0.1"

/* Agent device type (model) identification */
#define DEVICE_MODEL 0x80146
#define CAPFILE_VER  0x2

 /*
 * Set USE_EEPROM==1 to persistently store configuration parameters in EEPROM.
 * The following parameters are stored in EEPROM:
 * - DEF_SYNC_INTERVAL,
 * - DEF_LED0_STATE.
 */

#define USE_EEPROM		0						// 1-use eeprom, 0-use default

#define	DEF_SYNC_INTERVAL		10
#define	DEF_LED0_STATE			2						// Default LED0 state: 0-off, 1-on, 2-blinking.

/* Atmel Cloud Configuration */
#define	DEF_SERVER_IP			"54.191.88.84"			// Atmel Cloud Service IP Address, DO NOT MODIFY.
#define	DEF_SERVER_PORT			5050					// Atmel Cloud Service Port, DO NOT MODIFY.
#define	DEF_ACTIVATION_CODE		"DEFAULT_ACTIVATION_CODE"	// Device Activation Code. 74-character hex string.
#define	DEF_DEVICE_NAME			"DEVICE_NAME"	// Device Name.
#define	DEF_SYNC_INTERVAL		10

#endif /* CONF_APP_H */
