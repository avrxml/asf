/**
* \file
*
* \brief Phone Alert Status Application declarations
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
/*
* Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
*/

/****************************************************************************************
*							        Macros			                                 	*
****************************************************************************************/
#define BIT0_MASK					(0x1 << 0)
#define BIT1_MASK					(0x1 << 1)
#define BIT2_MASK					(0x1 << 2)

#define RINGER_SILENT				(0x0)
#define RINGER_NORMAL				(0x1)

#define DEVICE_SILENT				(1)
#define DEVICE_MUTE					(2)
#define DEVICE_NORMAL				(3)
#define READ_REQUEST				(4)

#define READ_ALERT_STATUS			(4)
#define READ_RINGER_SETTING			(5)
#define DISCOVER_ATTRIBUTES			(6)




/****************************************************************************************
*							        Function Prototype                                 	*
****************************************************************************************/
/**
 * @brief app_connected_state ble manager notifies the application about state
 * @param[in] connected
 */
static at_ble_status_t app_disconnected_event_handler(void *params);
/**
 * @brief app_connected_state blemanager notifies the application about state
 * @param[in] at_ble_connected_t
 */
static at_ble_status_t app_connected_event_handler(void *params);
