/**
 * \file
 *
 * \brief AltBeacon Application Declarations
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
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Atmel
 *Support</a>
 */
#ifndef __ALT_BEACON_APP_H__
#define __ALT_BEACON_APP_H__


/****************************************************************************************
*							        Macros	                                     		*
****************************************************************************************/

/* Beacon Advertisement timeout */
#define ALT_BEACON_ADV_TIMEOUT	(0) 

/* Beacon Advertise at exact interval */
#define ALT_BEACON_ABSOLUTE_INTERVAL_ADV (1)

/* Beacon Advertisement data */
#define MANUFACTURER_SPECIFIC_TYPE					(0xff)
#define MANUFACTURER_ID                             ("\x13\x00")
#define MANUFACTURER_ID_LENGTH						(0x02)
#define ALT_BEACON_CODE								("\xbe\xac")
#define ALT_BEACON_CODE_LENGTH						(0x02)
#define ALT_BEACON_16BYTE_UUID						("\x00\x00\x00\x00\x00\x00\x10\x00\x80\x00\x00\x80\x5f\x9b\x34\xfb")
#define ALT_BEACON_16BYTE_UUID_LENGTH				(0x10)
#define ALT_BEACON_MAJOR_NUM						("\x00\x01")
#define ALT_BEACON_MINOR_NUM						("\x00\x01")
#define ALT_BEACON_2BYTE_LEN						(0x02)
#define REF_RSSI									(0xd3)
#define MFG_RSVD								    (0x00)
#define ALT_BEACON_ADV_LENGTH						(0x1b)
#define ALT_BEACON_ADV_TYPE_LENGTH					(0x01)
#define ALT_BEACON_SCAN_RESPONSE_LENGTH				(0x00)

/* Debug Log printer */
#define DBG_LOG printf("\r\n");\
				printf

/**
 * @brief Initialize the ble module
 */				
static void ble_init(void);

/**
 * @brief Initialize the AltBeacon advertisement data and start advertisement.
 */	
static void altbeacon_init(void);

/**
 * @brief Set BLE address 
 * @param[in] at_ble_addr_t If address is NULL then it will use BD public address.
 */
static void ble_set_address(at_ble_addr_t *addr);				

#endif /* __ALT_BEACON_APP_H__ */
