/**
 * \file
 *
 * \brief Eddystone beacon configuration
 *
 * Copyright (c) 2017-2018 Microchip Technology Inc. and its subsidiaries.
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

#ifndef CONF_EDDYSTONE_H_INCLUDED
#define CONF_EDDYSTONE_H_INCLUDED

/****************************************************************************************
*				        Default Configuration Defines	                                     	     *
****************************************************************************************/

/** Eddystone-UID defines */

/** @brief  Namespace ID - 10 bytes of the Eddystone-UID */
#define CONF_ES_NAMESPACE_ID						("\xB3\x75\xE9\x7F\x93\x66\xE7\xC2\xA6\xE1")

/** @brief  Instance ID - 6 bytes of the Eddystone-UID */
#define CONF_ES_INSTANCE_ID							("\x00\x00\x00\x00\x00\x01")

/** @brief  TX power level at 0 meters [-100 dBm to +20 dBm] - used for beacon ranging */
#define CONF_ES_CALIB_TX_PWR_0m						(-4)

/** @brief  Radio TX power setting for the device to achieve @ref CONF_ES_CALIB_TX_PWR_0m level */
#define CONF_ES_RADIO_TX_PWR_0m						(AT_BLE_TX_PWR_LVL_ZERO_DB)


/** Eddystone-URL defines */

/** @brief  Default URL value in encoded form 
 *  \x02goo.gl/SJmAjy = Shortened URL for "http://www.microchip.com/design-centers/wireless-connectivity/bluetooth"
 */
#define CONF_ES_DEFAULT_URL							("\x02goo.gl/SJmAjy")

/** @brief  Number of bytes in the default URL */
#define CONF_ES_DEFAULT_URL_SIZE					(14)

/** @brief  Beacon interval in milliseconds [100ms to 10240ms (10.24s)] */
#define CONF_ES_DEFAULT_BEACON_PERIOD_MS			(200)

/** @brief  Advertised TX Power levels at 0 meters in dBm [-100 dBm to +20 dBm] */
#define CONF_ES_DEFAULT_ADV_TX_PWR_HIGH				(-1)
#define CONF_ES_DEFAULT_ADV_TX_PWR_MEDIUM			(-4)
#define CONF_ES_DEFAULT_ADV_TX_PWR_LOW				(-10)
#define CONF_ES_DEFAULT_ADV_TX_PWR_LOWEST			(-18)

/** @brief  Radio TX Power levels @ref at_ble_tx_power_level_t to achieve advertised TX power */
#define CONF_ES_DEFAULT_RADIO_TX_PWR_HIGH			(AT_BLE_TX_PWR_LVL_POS_03_DB)
#define CONF_ES_DEFAULT_RADIO_TX_PWR_MEDIUM			(AT_BLE_TX_PWR_LVL_ZERO_DB)
#define CONF_ES_DEFAULT_RADIO_TX_PWR_LOW			(AT_BLE_TX_PWR_LVL_NEG_06_DB)
#define CONF_ES_DEFAULT_RADIO_TX_PWR_LOWEST			(AT_BLE_TX_PWR_LVL_NEG_14_DB)

/** @brief  Enable/disable the lock feature.  
 * This controls the inclusion of the optional 'lock' and 'unlock' characteristics in the Eddystone-URL configuration service 
 *  0 - Disable lock feature;  1 - Enable lock feature.
 */
#define ES_URL_CONF_ENABLE_LOCK						(1)

#if (ES_URL_CONF_ENABLE_LOCK == 1)
	/** brief  Default lock state (1 = LOCKED;  0 = UNLOCKED) when lock feature is enabled */
	#define CONF_ES_DEFAULT_LOCK_STATE					(0)
#else
	/** brief  Default lock state when lock feature is disabled; Must be set to UNLOCKED always. */ 
	#define CONF_ES_DEFAULT_LOCK_STATE					(0)
#endif

/** @brief  Default lock code of 16 bytes */
#define CONF_ES_DEFAULT_LOCK_CODE					"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"

/** @brief  Number of bytes in the lock code */
#define CONF_ES_DEFAULT_LOCK_CODE_SIZE				(16)

/** @brief  Local name advertised in the Eddystone-URL configuration  mode */
#define CONF_ES_CONF_ADV_DATA_COMPLETE_LOCAL_NAME	"Atmel-Eddystone"

#endif /* CONF_EDDYSTONE_H_INCLUDED */