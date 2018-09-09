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

#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "at_ble_api.h"

#define MAX_PLATFORM_OS_SIGNAL (3)

typedef struct os_signal
{
  volatile uint32_t signal_value;
  uint32_t          signal_usage;
} os_signal_t;

 /**@ingroup platform_group_functions
  * @brief implements platform-specific initialization
  *
  * @param[in] bus_type bus type can be UART or SPI
  * @param[in] btlc1000_module_version BTLC1000 MR/ZR Module version
  *
  * @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS, Otherwise the function shall return @ref at_ble_status_t 
  */
at_ble_status_t platform_init(uint8_t bus_type, uint8_t btlc1000_module_version);

 /**@ingroup platform_group_functions
  * @brief sends a message over the platform-specific bus and blocks until Tx Completes
  *
  * Sends a message over the platform-specific bus that might be UART, SPI
  *
  * @param[in] data data to send over the interface
  * @param[in] len length of data
  *
  */
 void platform_configure_primary_uart(uint32_t baudrate);
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
//void platform_configure_hw_fc_uart(void);
void platform_configure_hw_fc_uart(uint32_t baudrate);
void platform_process_rxdata(uint8_t t_rx_data);
void platform_dma_process_rxdata(uint8_t *buf, uint16_t len);

void platform_enter_sleep(void);
void platform_host_set_sleep(bool sleep);

 /* functions that should be called to help cooperative multitasking scheduler 
    to switch context to other tasks */
void *platform_create_signal(void);
void platform_delete_signal(void *signal_handler);
void platform_trigger_signal(void *signal_handler);
void platform_reset_signal(void *signal_handler);
void platform_wait_for_signal(uint32_t count, void **signal_handler_list);

#ifdef BTLC_REINIT_SUPPORT
void platform_reset_timer(void);
void platform_signal_set_default(void);
#endif
#endif // __PLATFORM_H__
