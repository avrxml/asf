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

#include <asf.h>
#include "platform.h"
#include "conf_serialdrv.h"
#include "serial_drv.h"

static uint16_t rx_data;

static volatile uint32_t cmd_cmpl_flag = 0;
static volatile uint32_t event_flag = 0;

void serial_rx_callback(void)
{
#if SAMG55
	while(serial_read_byte(&rx_data) == STATUS_OK)
	{
		platform_interface_callback((uint8_t *)&rx_data, 1);
	}		
#endif
		
#if SAMD || SAMR21
	do 
	{
	  platform_interface_callback((uint8_t *)&rx_data, 1);
	} while (serial_read_byte(&rx_data) == STATUS_BUSY);	
#endif
}

void serial_tx_callback(void)
{
	ble_enable_pin_set_low();
}


at_ble_status_t platform_init(void* platform_params)
{
	ble_enable_pin_init();
	configure_serial_drv();
	serial_read_byte(&rx_data);
		
	return AT_BLE_SUCCESS;
}

void platform_interface_send(uint8_t* data, uint32_t len)
{
	ble_enable_pin_set_high();
	serial_drv_send(data, len);	
#if SAMG55
	ble_enable_pin_set_low();
#endif
}

void platform_cmd_cmpl_signal()
{
	cmd_cmpl_flag = 1;
}

void platform_cmd_cmpl_wait(bool* timeout)
{
	while(cmd_cmpl_flag != 1);
	cmd_cmpl_flag = 0;
	//*timeout = true; //Incase of timeout[default to 4000ms]
}

void platform_event_signal()
{
	event_flag = 1;
}

uint8_t platform_event_wait(uint32_t timeout)
{
	/* Timeout in ms */
	uint8_t status = AT_BLE_SUCCESS;//AT_BLE_TIMEOUT in case of timeout
	while(event_flag != 1);
	event_flag = 0;
	return status;
}

