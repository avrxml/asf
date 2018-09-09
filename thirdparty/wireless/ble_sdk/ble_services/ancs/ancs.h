/**
* \file
*
* \brief ANCS Service Declarations
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

// <<< Use Configuration Wizard in Context Menu >>>
// <h> ANCS Service Configuration
// =======================
#ifndef __ANCS_H__
#define __ANCS_H__

typedef struct ancs_prf{
	
	/*Instance for ANCS*/
	at_ble_primary_service_found_t ancs_serv;
	
	/*Instance for notification source characteristic*/
	at_ble_characteristic_found_t notification_source_char;
	
	/*Instance for control point characteristic*/
	at_ble_characteristic_found_t control_point_char;
	
	/*Instance for data source characteristic*/
	at_ble_characteristic_found_t data_source_char;
	
	/*Instance for notification source descriptor*/
	at_ble_descriptor_found_t notification_source_desc;
	
	/*Instance for notification source descriptor*/
	at_ble_descriptor_found_t data_source_desc;
	
}ancs_prf_t;

/**@brief ANCS Service Solicitation Info*/
#define ANCS_SERV_UUID	 "\xD0\x00\x2D\x12\x1E\x4B\x0F\xA4\x99\x4E\xCE\xB5\x31\xF4\x05\x79"

/**@brief UUID Type & Length*/
#define UUID_16BIT_LEN							(2)
#define UUID_32BIT_LEN							(4)
#define UUID_128BIT_LEN							(16)

/**
 * @brief Initializing Ancs data 
 * @param[in] Instance of ancs data
 * \note Called by the profile
 */
void ancs_init(ancs_prf_t *ancs_data);

/**
 * @brief Enabling ANCS service 
 * @param[in] Instance of ancs data
 * \note Called by the profile
 */
void ancs_enable(ancs_prf_t *anp_data, at_ble_handle_t handle);

#endif /* __ANCS_H__ */
// </h>

// <<< end of configuration section >>>
