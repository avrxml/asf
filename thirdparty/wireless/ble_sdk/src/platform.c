/**
 * \file
 *
 * \brief Platform Abstraction layer for BLE applications
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

#include <asf.h>
#include "platform.h"
#include "serial_drv.h"
#include "serial_fifo.h"
#include "ble_utils.h"
#include "conf_serialdrv.h"
#include "timer_hw.h"

static void (*recv_async_cb)(uint8_t) = NULL;
static volatile bool platform_timer_used = false;
static volatile bool host_sleep_flag = false;

static os_signal_t platform_os_signals[MAX_PLATFORM_OS_SIGNAL];

/* This variable helpfull for only SAMG55 and SAM4S platforms
	where the BTLC1000 wakeup time is just high before data send*/
extern volatile bool wakeup_pin_status;

//#define BLE_DBG_ENABLE
#define DBG_LOG_BLE		DBG_LOG

at_ble_status_t platform_init(uint8_t bus_type, uint8_t btlc1000_module_version)
{	
    at_ble_status_t status = AT_BLE_INVALID_PARAM;
	
	if (bus_type == AT_BLE_UART)
	{	
		#ifdef BTLC_REINIT_SUPPORT	
		platform_signal_set_default();
		platform_reset_timer();
		#endif
				
		ble_configure_control_pin();
		
		configure_serial_drv(CONF_UART_BAUDRATE);
				
		btlc1000_host_wakeup_config();
	
		platform_configure_sleep_manager();
		
		status =  AT_BLE_SUCCESS;
	}
	else
	{
		ble_assert(false);               
	}
	
	ALL_UNUSED(btlc1000_module_version);
	return status;             
       
}

void platform_send_sync(uint8_t *data, uint32_t len)
{
	ble_assert(data != NULL);
	ble_assert(!((len == 0) || (len > 65535)));
#ifdef BLE_DBG_ENABLE
	uint32_t i;
	DBG_LOG_BLE("\r\nTxLen:%d: ", len);
	for (i = 0; i < len; i++)
	{
		DBG_LOG_CONT("0x%X, ", data[i]);
		
	}
#endif

	if(!platform_wakeup_pin_status())
	{
		ble_wakeup_pin_set_high();
		delay_ms(5);
	}					 
	serial_drv_send(data, (uint16_t)len);

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
			#if defined(ENABLE_POWER_SAVE)
			bool host_pin_status = true;
			#if (HOST_SLEEP_ENABLE == true)
			host_pin_status = host_event_data_ready_pin_level();
			#endif
			if((!serial_drive_rx_data_count()) && (host_pin_status))
			{
				ble_wakeup_pin_set_low();
			}
			#endif
		}
	}
	else
	{
		ble_assert(false);
	}
}

void platform_recv_async(void (*recv_async_callback)(uint8_t))
{
	recv_async_cb = recv_async_callback;
	platform_start_rx();	
}

void platform_process_rxdata(uint8_t t_rx_data)
{
	Assert((recv_async_cb != NULL));
	recv_async_cb(t_rx_data);
}

void platform_dma_process_rxdata(uint8_t *buf, uint16_t len)
{
	uint16_t idx = 0;
	while(idx < len)
	{
		recv_async_cb(buf[idx++]);
	}
}

void platform_configure_primary_uart(uint32_t baudrate)
{
	 configure_serial_drv(baudrate);
}

void platform_configure_hw_fc_uart(uint32_t baudrate)
{
	configure_usart_after_patch(baudrate);
}

void platform_host_set_sleep(bool sleep)
{
	host_sleep_flag = sleep;
}


void platform_enter_sleep(void)
{
	if (!host_sleep_flag) {
		return;
	}	

#if defined HOST_DEEP_SLEEP_ENABLED	
	/* When No event and External Interrupt Pin will be High(No Event in Queue) */
	if ((host_event_data_ready_pin_level()) && (!ble_wakeup_pin_level())
		&& (!serial_drive_rx_data_count()))
	{
		/* RTS is Low Set to High */
		if (!ble_host_rts_pin_level())
		{
			/* Set Host RTS to High */
			platform_set_ble_rts_high();
			
			if(!host_event_data_ready_pin_level())
			{
				platform_set_ble_rts_low();
				return;
			}
		}
		platform_set_hostsleep();
		
		platform_restore_from_sleep();
		
		/* Set Host RTS Low to receive the data */
		platform_set_ble_rts_low();
	}
	else
#endif
	if ((!ble_wakeup_pin_level())
	&& (!serial_drive_rx_data_count()))
	{
		platform_set_hostsleep();
		platform_restore_from_sleep();
	}
}

void platform_host_wake_interrupt_handler(void)
{
	/* Keep BTLC1000 Wakeup line high */
	ble_wakeup_pin_set_high();
}

void *platform_create_timer(void (*timer_cb)(void *))
{
	return (platform_create_hw_timer(timer_cb));
}

void platform_delete_timer(void *timer_handle)
{
  platform_delete_bus_timer(timer_handle);
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

void platform_wait_for_signal(uint32_t count, void **signal_handler_list)
{
	uint32_t idx;
	bool signal_presence = false;
	os_signal_t **os_signal_list = (os_signal_t **)signal_handler_list;
	if (count > 0 && NULL != os_signal_list)
	{
		do
		{
			for (idx = 0; idx < count; idx++)
			{
				if ((NULL != os_signal_list[idx]) &&
				(1 == os_signal_list[idx]->signal_usage) &&
				(1 == os_signal_list[idx]->signal_value))
				{
					signal_presence = true;
					break;
				}				
			}			
			if(!signal_presence)
			{
			    /************ Points to Remeber to before putting Host MCU to sleep *******************/
			   /*	Host MCU can go to sleep with wakeup sources (Host Wakeup Pin/UART/Bus Timer/Event Timeout Timer) 
					configured with below conditions are met.
				1. Host MCU should not go to sleep when ongoing data transfers between the Host MCU and BTLC1000 
				2. Host MCU should not have any unprocessed BTLC1000 data in its buffer (The received event data frame is incomplete 
					and Host MCU never process until it wakes up some other Host MCU interrupt source)
				3. Host MCU should not go to deep sleep until Host Wakeup line goes high (to avoid latencies)
				4. Host MCU should not go to deep sleep until BTLC1000 wakeup line goes low (to avoid latencies)
				5. In RTOS Task can yield until signals are triggered, But Host MCU should meet the above condition 
					before putting Host MCU to sleep */
					platform_enter_sleep();
			}
		} while (!signal_presence);
	}
}

void *platform_create_signal(void)
{
	uint32_t idx;
	for (idx = 0; idx < sizeof(platform_os_signals) / sizeof(os_signal_t); idx++)
	{
		if (platform_os_signals[idx].signal_usage == 0)
		{
			platform_os_signals[idx].signal_usage = 1;
			platform_os_signals[idx].signal_value = 0;
			return &platform_os_signals[idx];
		}
	}
	return NULL;
}

void platform_delete_signal(void *signal_handler)
{
	os_signal_t *os_signal_instance = (os_signal_t *)signal_handler;
	if (NULL != os_signal_instance)
	{
		os_signal_instance->signal_usage = 0;
		os_signal_instance->signal_value = 0;
	}
}

void platform_trigger_signal(void *signal_handler)
{
	os_signal_t *os_signal_instance = (os_signal_t *)signal_handler;
	if ((NULL != os_signal_instance) &&
	(1 == os_signal_instance->signal_usage))
	{
		os_signal_instance->signal_value = 1;
	}
}

void platform_reset_signal(void *signal_handler)
{
	os_signal_t *os_signal_instance = (os_signal_t *)signal_handler;
	if ((NULL != os_signal_instance) &&
	(1 == os_signal_instance->signal_usage))
	{
		os_signal_instance->signal_value = 0;
	}
}

#ifdef BTLC_REINIT_SUPPORT
void platform_signal_set_default(void)
{
	uint32_t idx;
	for (idx = 0; idx < sizeof(platform_os_signals) / sizeof(os_signal_t); idx++)
	{
		platform_os_signals[idx].signal_usage = 0;
		platform_os_signals[idx].signal_value = 0;
	}
}

void platform_reset_timer(void)
{
	platform_reset_hw_timer();
}
#endif
