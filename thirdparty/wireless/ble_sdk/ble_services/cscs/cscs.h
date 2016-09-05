
/**
 * \file
 *
 * \brief Custom Serial Chat Service Declarations
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
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel
 *Support</a>
 */

// <<< Use Configuration Wizard in Context Menu >>>
// <h> Custom Serial Chat Service Configuration
// =======================
#ifndef __CSCS_H__
#define __CSCS_H__

/****************************************************************************************
*							        Macros	                                     		*
****************************************************************************************/
/** @brief Custom serial chat UUID length */
#define CSC_UUID_128_LEN				(16)

/****************************************************************************************
*							        Structures                                     		*
****************************************************************************************/
/** @brief Custom serial chat service info */
typedef struct csc_serv
{
	at_ble_uuid_t	serv_uuid; 
	at_ble_handle_t	serv_handle; 
	at_ble_characteristic_t	endpoint_chars;  
}csc_serv_t;

/****************************************************************************************
*                                       Functions                                       *
****************************************************************************************/
/** @brief Initialize the csc service initialization function
  * 
  * @param[in] buf		Buffer pointer for data to be send
  * @param[in] len      size of buffer		
  */
at_ble_status_t csc_serv_init(uint8_t *buf, uint16_t len);

/** @brief Function used for sending data to remote device
  * 
 * @param[in] buf		Buffer pointer for data to be send
 * @param[in] len       size of buffer
  */
at_ble_status_t csc_serv_send_data(uint16_t connhandle, uint8_t *databuf, uint16_t datalen);


#endif /*__CSCS_H__*/
// </h>

// <<< end of configuration section >>>
