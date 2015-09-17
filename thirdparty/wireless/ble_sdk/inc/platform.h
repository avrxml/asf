/**
 * \file
 *
 * \brief Platform Abstraction layer for BLE applications
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
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

#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "at_ble_api.h"

typedef struct{
	uint8_t (*wr_api32) (uint32_t memAddr,uint32_t* data,uint8_t size);
	uint8_t (*wr_api32_reset) (uint32_t memAddr,uint32_t* data,uint8_t size);
}wr_apis;
/**
@defgroup platform Platform API

@{
*/

 /** @brief implemets platform-specefic initialization
  *
  * @param[in] platform_params platform specefic params, this pointer is passed from the 
  * at_ble_init function and interpreted by the platform 
  *
  * @return AT_BLE_SUCCESS operation completed successfully
  * @return AT_BLE_FAILURE Generic error.
  */
at_ble_status_t platform_init(void* platform_params);

 /** @brief sends a message over the platform-specific bus
  *
  * Sends a message over the platform-specific bus that might be UART, SPI or other
  * if the BTLC1000 external wakeup signal is used, it is up to this function implementation
  * to assert/deassert it appropiately
  *
  * @param[in] data data to send over the interface
  * @param[in] len length of data
  *
  */
void platform_interface_send(uint8_t* data, uint32_t len);

 /** @brief the callback to upper layers to process recieved packets
  *
  * This function is implemented by the upper layers (the event loop) and it is up
  * to the platofrm implementaion to call it whenever data are recieved from the interface
  *
  * @param[in] data data received from the interface
  * @param[in] len length of data
  *
  */
void platform_interface_callback(uint8_t* data, uint32_t len);

 /** @brief fires the comand-complete signal
  *  @note more details at the platform porting guide
  *
  */
void platform_cmd_cmpl_signal(void);

 /** @brief blocks until the command-complete signal is fired
  *  @note more details at the platform porting guide
  *
  * @param[out] timeout a flag that indicates if waiting timed out
  */
void platform_cmd_cmpl_wait(bool* timeout);

 /** @brief fires the event signal
  *  @note more details at the platform porting guide
  *
  */
void platform_event_signal(void);

 /** @brief blocks until the event signal is fired
  *  @note more details at the platform porting guide
  *
  * @param[in] timeout timeout in ms passed by user
  *
  */
uint8_t platform_event_wait(uint32_t timeout);
void fw_led(bool tempo);
 /** @}*/

#endif // __PLATFORM_H__
