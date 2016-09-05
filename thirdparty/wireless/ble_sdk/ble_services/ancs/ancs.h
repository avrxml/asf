/**
* \file
*
* \brief ANCS Service Declarations
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
*    Atmel micro controller product.
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
