/**
 * \file serial_drv.c
 *
 * \brief Handles Serial driver functionalities
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
 */

/* === INCLUDES ============================================================ */

#include "asf.h"
#include "serial_drv.h"
#include "conf_serialdrv.h"
#include "serial_fifo.h"
#include "ble_utils.h"

/* === TYPES =============================================================== */

/* === MACROS ============================================================== */
	
/* === PROTOTYPES ========================================================== */
static void serial_drv_read_cb(struct usart_module *const usart_module);
static void serial_drv_write_cb(struct usart_module *const usart_module);


/* === GLOBALS ========================================================== */
struct usart_module usart_instance;

static uint16_t rx_data;


/* === IMPLEMENTATION ====================================================== */
static inline void usart_configure_flowcontrol(void)
{
	struct usart_config config_usart;
#if UART_FLOWCONTROL_6WIRE_MODE == true
	usart_disable(&usart_instance);
	usart_reset(&usart_instance);
#endif
	usart_get_config_defaults(&config_usart);

	config_usart.baudrate = CONF_FLCR_BLE_BAUDRATE;
	config_usart.generator_source = CONF_FLCR_BLE_UART_CLOCK;
	config_usart.mux_setting = CONF_FLCR_BLE_MUX_SETTING;
	config_usart.pinmux_pad0 = CONF_FLCR_BLE_PINMUX_PAD0;
	config_usart.pinmux_pad1 = CONF_FLCR_BLE_PINMUX_PAD1;
	config_usart.pinmux_pad2 = CONF_FLCR_BLE_PINMUX_PAD2;
	config_usart.pinmux_pad3 = CONF_FLCR_BLE_PINMUX_PAD3;

	while (usart_init(&usart_instance, CONF_FLCR_BLE_USART_MODULE, &config_usart) != STATUS_OK);

	usart_enable(&usart_instance);
	
	/* register and enable usart callbacks */
	usart_register_callback(&usart_instance,
	serial_drv_read_cb, USART_CALLBACK_BUFFER_RECEIVED);
	usart_register_callback(&usart_instance,
	serial_drv_write_cb, USART_CALLBACK_BUFFER_TRANSMITTED);
	usart_enable_callback(&usart_instance, USART_CALLBACK_BUFFER_RECEIVED);
	usart_enable_callback(&usart_instance, USART_CALLBACK_BUFFER_TRANSMITTED);
	serial_read_byte(&rx_data);
}

uint8_t configure_serial_drv(void)
{
	#if UART_FLOWCONTROL_4WIRE_MODE == true
		usart_configure_flowcontrol();
		#warning "This mode works only if Flow Control Permanently Enabled in the BTLC1000"
	#else
	struct usart_config config_usart;
	usart_get_config_defaults(&config_usart);
	config_usart.baudrate = CONF_BLE_BAUDRATE;
	config_usart.generator_source = CONF_BLE_UART_CLOCK;
	config_usart.mux_setting = CONF_BLE_MUX_SETTING;
	config_usart.pinmux_pad0 = CONF_BLE_PINMUX_PAD0;
	config_usart.pinmux_pad1 = CONF_BLE_PINMUX_PAD1;
	config_usart.pinmux_pad2 = CONF_BLE_PINMUX_PAD2;
	config_usart.pinmux_pad3 = CONF_BLE_PINMUX_PAD3;

	while (usart_init(&usart_instance, CONF_BLE_USART_MODULE, &config_usart) != STATUS_OK);

	usart_enable(&usart_instance);
	
	/* register and enable usart callbacks */
	usart_register_callback(&usart_instance,
		serial_drv_read_cb, USART_CALLBACK_BUFFER_RECEIVED);
	usart_register_callback(&usart_instance,
		serial_drv_write_cb, USART_CALLBACK_BUFFER_TRANSMITTED);
	usart_enable_callback(&usart_instance, USART_CALLBACK_BUFFER_RECEIVED);
	usart_enable_callback(&usart_instance, USART_CALLBACK_BUFFER_TRANSMITTED);
	serial_read_byte(&rx_data);
	#endif
	
	return STATUS_OK;
}

void configure_usart_after_patch(void)
{
	#if UART_FLOWCONTROL_6WIRE_MODE == true
	usart_configure_flowcontrol();
	#endif	
}

uint16_t serial_drv_send(uint8_t* data, uint16_t len)
{  
	while (STATUS_OK != usart_write_buffer_job(&usart_instance, data, len));
	/* Wait for ongoing transmission complete */
	while (STATUS_OK != usart_get_job_status(&usart_instance, USART_TRANSCEIVER_TX));
	
	return STATUS_OK;
}

extern void platform_process_rxdata(uint8_t t_rx_data);
static void serial_drv_read_cb(struct usart_module *const module)
{	 
	platform_process_rxdata((uint8_t)rx_data);
}

uint8_t serial_read_data(uint8_t* data, uint16_t max_len)
{
 return usart_read_buffer_job(&usart_instance, data, max_len);
}

void platfrom_start_rx(void)
{
	serial_read_byte(&rx_data);
}

uint8_t serial_read_byte(uint16_t* data)
{
	return usart_read_job(&usart_instance, data);
}

static void serial_drv_write_cb(struct usart_module *const usart_module)
{
	/* USART Tx callback */	
}

uint32_t platform_serial_drv_tx_status(void)
{
	return(usart_get_job_status(&usart_instance, USART_TRANSCEIVER_TX));
}

void platform_enter_critical_section(void)
{
	system_interrupt_enter_critical_section();
}

void platform_leave_critical_section(void)
{
	system_interrupt_leave_critical_section();
}

/* EOF */
