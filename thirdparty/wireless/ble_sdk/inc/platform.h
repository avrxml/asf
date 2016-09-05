/**
 * \file
 *
 * \brief Platform Abstraction layer for BLE applications
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

#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "at_ble_api.h"

 /**@ingroup platform_group_functions
  * @brief implements platform-specific initialization
  *
  * @param[in] bus_type bus type can be UART or SPI
  * @param[in] bus_flow_control_enabled bus type can have flow control Enable/Disable option
  *
  * @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS, Otherwise the function shall return @ref at_ble_status_t 
  */
at_ble_status_t platform_init(uint8_t bus_type, uint8_t bus_flow_control_enabled);

 /**@ingroup platform_group_functions
  * @brief sends a message over the platform-specific bus and blocks until Tx Completes
  *
  * Sends a message over the platform-specific bus that might be UART, SPI
  *
  * @param[in] data data to send over the interface
  * @param[in] len length of data
  *
  */
void platform_send_sync(uint8_t *data, uint32_t len);

void platform_gpio_set(at_ble_gpio_pin_t pin, at_ble_gpio_status_t status);

void platform_recv_async(void (*recv_async_callback)(uint8_t));

void platform_sleep(uint32_t ms);
bool platform_wakeup_pin_status(void);
void plaform_ble_rx_callback(void);

void *platform_create_timer(void (*timer_cb)(void *));
void platform_delete_timer(void *timer_handle);
void platform_stop_timer(void *timer_handle);
void platform_start_timer(void *timer_handle, uint32_t ms);
void platform_configure_hw_fc_uart(void);
void platform_process_rxdata(uint8_t t_rx_data);
void platform_dma_process_rxdata(uint8_t *buf, uint16_t len);

#endif // __PLATFORM_H__
