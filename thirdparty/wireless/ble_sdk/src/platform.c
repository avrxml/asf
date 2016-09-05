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

#include <asf.h>
#include "platform.h"
#include "conf_serialdrv.h"
#include "serial_drv.h"
#include "serial_fifo.h"
#include "ble_utils.h"
#include "timer_hw.h"

static uint8_t platform_bus_type;
static void (*recv_async_cb)(uint8_t) = NULL;
static volatile bool platform_timer_used = false;

#if ((UART_FLOWCONTROL_4WIRE_MODE == true) && (UART_FLOWCONTROL_6WIRE_MODE == true))
#error "Invalid UART Flow Control mode Configuration. Choose only one mode"
#endif

//#define BLE_DBG_ENABLE
#define DBG_LOG_BLE		DBG_LOG

at_ble_status_t platform_init(uint8_t bus_type, uint8_t bus_flow_control_enabled)
{	
        (void)bus_flow_control_enabled;
	platform_bus_type = bus_type;
	if (platform_bus_type == AT_BLE_UART)
	{
		ble_configure_control_pin();
		
		configure_serial_drv();
		
		return AT_BLE_SUCCESS;
	}
	return AT_BLE_INVALID_PARAM;	
}

void platform_send_sync(uint8_t *data, uint32_t len)
{

#ifdef BLE_DBG_ENABLE
	uint32_t i;
	DBG_LOG_BLE("\r\nTxLen:%d: ", len);
	for (i = 0; i < len; i++)
	{
		DBG_LOG_CONT("0x%X, ", data[i]);
		
	}
#endif
	 if (AT_BLE_UART == platform_bus_type)
	 {
		serial_drv_send(data, (uint16_t)len);
	 }
}

void platform_gpio_set(at_ble_gpio_pin_t pin, at_ble_gpio_status_t status)
{
	if (pin == AT_BLE_CHIP_ENABLE)
	{
		if (status == AT_BLE_HIGH)
		{
			ble_enable_pin_set_high();
		}
		else
		{
			ble_enable_pin_set_low();
		}
	}
	else if (pin == AT_BLE_EXTERNAL_WAKEUP)
	{
		if (status == AT_BLE_HIGH)
		{
			ble_wakeup_pin_set_high();
		} 
		else
		{
			ble_wakeup_pin_set_low();
		}
	}
}

void platform_recv_async(void (*recv_async_callback)(uint8_t))
{
	if (AT_BLE_UART == platform_bus_type)
    {
		recv_async_cb = recv_async_callback;
        platfrom_start_rx();		
    }
}

void platform_process_rxdata(uint8_t t_rx_data)
{
	if (AT_BLE_UART == platform_bus_type)
	{
		if(recv_async_cb != NULL)
        {
			recv_async_cb(t_rx_data);
        }
	}
}

void platform_dma_process_rxdata(uint8_t *buf, uint16_t len)
{
	if (AT_BLE_UART == platform_bus_type)
	{
		uint16_t idx;
		for (idx = 0; idx < len; idx++)
		{
			recv_async_cb(buf[idx]);
		}
	}
}

void platform_configure_hw_fc_uart(void)
{
	configure_usart_after_patch();
}

void *platform_create_timer(void (*timer_cb)(void *))
{
	return (platform_configure_timer(timer_cb));
}

void platform_delete_timer(void *timer_handle)
{
  platform_enter_critical_section();
  platform_delete_bus_timer(timer_handle);
  platform_leave_critical_section();
}

void platform_start_timer(void *timer_handle, uint32_t ms)
{
	platform_enter_critical_section();
	platform_start_bus_timer(timer_handle, ms);
	platform_leave_critical_section();
}

void platform_stop_timer(void *timer_handle)
{
	platform_enter_critical_section();
	platform_stop_bus_timer(timer_handle);
	platform_leave_critical_section();
}

void platform_sleep(uint32_t ms)
{
	delay_ms(ms);
}

bool platform_wakeup_pin_status(void)
{
	return (ble_wakeup_pin_level());
}

void plaform_ble_rx_callback(void)
{
	ble_wakeup_pin_set_high();
}

