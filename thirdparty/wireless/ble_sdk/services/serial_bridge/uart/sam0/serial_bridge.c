/**
 * \file serial_bridge.c
 *
 * \brief Handles Serial bridge driver functionalities
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
 */

/* === INCLUDES ============================================================ */

#include "asf.h"
#include "serial_bridge.h"
#include "conf_serialbridge.h"
#include "serial_fifo.h"
#include "ble_utils.h"
#include "conf_serialdrv.h"

/* === TYPES =============================================================== */

/* === MACROS ============================================================== */
	
/* === PROTOTYPES ========================================================== */

static void serial_drv_edbg_read_cb(struct usart_module *const module);
static void serial_drv_edbg_write_cb(struct usart_module *const module);
static uint8_t serial_eread_byte(uint16_t* data);

/* === GLOBALS ========================================================== */

static struct usart_module eusart_instance;

static ser_fifo_desc_t ble_eusart_tx_fifo;
static  uint8_t ble_eusart_tx_buf[BLE_MAX_TX_PAYLOAD_SIZE];
static  uint8_t ble_usart_tx_buf[BLE_MAX_TX_PAYLOAD_SIZE];

static ser_fifo_desc_t ble_eusart_rx_fifo;
static uint8_t ble_eusart_rx_buf[BLE_MAX_RX_PAYLOAD_SIZE];

extern struct usart_module usart_instance;

ser_fifo_desc_t ble_usart_tx_fifo;

ser_fifo_desc_t ble_usart_rx_fifo;
uint8_t ble_usart_rx_buf[BLE_MAX_RX_PAYLOAD_SIZE];

static uint16_t g_etxdata;
static uint16_t g_txdata;
static uint16_t rx_edata;

/* === Serial Bridge IMPLEMENTATION ====================================== */
/* initialize the EDBG VCOM UART */
uint8_t serial_bridge_init(void)
{
	struct usart_config config_usart;
	usart_get_config_defaults(&config_usart);
	config_usart.baudrate = CONF_SB_BAUDRATE;
	config_usart.generator_source = GCLK_GENERATOR_0;
	config_usart.mux_setting = CONF_SB_MUX_SETTING;
	config_usart.pinmux_pad0 = CONF_SB_PINMUX_PAD0;
	config_usart.pinmux_pad1 = CONF_SB_PINMUX_PAD1;
	config_usart.pinmux_pad2 = CONF_SB_PINMUX_PAD2;
	config_usart.pinmux_pad3 = CONF_SB_PINMUX_PAD3;

	while (usart_init(&eusart_instance, CONF_SB_USART_MODULE, &config_usart) != STATUS_OK);

	usart_enable(&eusart_instance);
	
	ser_fifo_init(&ble_eusart_rx_fifo, ble_eusart_rx_buf, BLE_MAX_RX_PAYLOAD_SIZE);
	ser_fifo_init(&ble_eusart_tx_fifo, ble_eusart_tx_buf, BLE_MAX_TX_PAYLOAD_SIZE);
	ser_fifo_init(&ble_usart_tx_fifo, ble_usart_tx_buf, BLE_MAX_TX_PAYLOAD_SIZE);
	ser_fifo_init(&ble_usart_rx_fifo, ble_usart_rx_buf, BLE_MAX_RX_PAYLOAD_SIZE);
	

	/* register and enable usart callbacks */
	usart_register_callback(&eusart_instance,
	serial_drv_edbg_read_cb, USART_CALLBACK_BUFFER_RECEIVED);
	usart_register_callback(&eusart_instance,
	serial_drv_edbg_write_cb, USART_CALLBACK_BUFFER_TRANSMITTED);
	usart_enable_callback(&eusart_instance, USART_CALLBACK_BUFFER_RECEIVED);
	usart_enable_callback(&eusart_instance, USART_CALLBACK_BUFFER_TRANSMITTED);
	serial_eread_byte(&rx_edata);
	return STATUS_OK;
}

/* EDBG UART Rx Callback function */
static void serial_drv_edbg_read_cb(struct usart_module *const module)
{
	 do
	 {
		 ser_fifo_push_uint8(&ble_eusart_rx_fifo, (uint8_t)rx_edata);
	 }while(serial_eread_byte(&rx_edata) == STATUS_BUSY);
}

/* EDBG UART Tx Callback function */
static void serial_drv_edbg_write_cb(struct usart_module *const module)
{
	uint8_t txdata;
	if(ser_fifo_pull_uint8(&ble_eusart_tx_fifo, &txdata) == SER_FIFO_OK)
	{
		g_etxdata = txdata;
		while(STATUS_OK != usart_write_job(&eusart_instance, &g_etxdata));
	}
}

/* EDBG UART Rx Read byte function */
static uint8_t serial_eread_byte(uint16_t* data)
{
	return usart_read_job(&eusart_instance, data);
}

/* Serial Bridge handler - Handles the two UART Tx and Rx data */
void serial_bridge_task(void)
{
	uint8_t t_rx_data;

	if(ser_fifo_pull_uint8(&ble_usart_rx_fifo, (uint8_t *)&t_rx_data) == SER_FIFO_OK)
	{
		
		LED_Toggle(LED0);
		ser_fifo_push_uint8(&ble_eusart_tx_fifo, (uint8_t)t_rx_data);
		if(ser_fifo_pull_uint8(&ble_eusart_tx_fifo, &t_rx_data) == SER_FIFO_OK)
		{			
			g_etxdata = t_rx_data;
			while(STATUS_OK != usart_write_job(&eusart_instance, &g_etxdata));
			
			/* Wait for ongoing transmission complete */
			while (STATUS_OK != usart_get_job_status(&eusart_instance, USART_TRANSCEIVER_TX));
		}
	}	
	
	if(ser_fifo_pull_uint8(&ble_eusart_rx_fifo, (uint8_t *)&t_rx_data) == SER_FIFO_OK)
	{
		LED_Toggle(LED0);
		ser_fifo_push_uint8(&ble_usart_tx_fifo, (uint8_t)t_rx_data);
		if(ser_fifo_pull_uint8(&ble_usart_tx_fifo, &t_rx_data) == SER_FIFO_OK)
		{
			g_txdata = t_rx_data;
			while(STATUS_OK != usart_write_job(&usart_instance, &g_txdata));
			
			/* Wait for ongoing transmission complete */
			while (STATUS_OK != usart_get_job_status(&usart_instance, USART_TRANSCEIVER_TX));
		}
	}
}

extern void platform_start_rx(void);
void platform_dtm_interface_receive(uint8_t rx_data)
{
	platform_start_rx();
	ser_fifo_push_uint8(&ble_usart_rx_fifo, (uint8_t)rx_data);	
}

/* EOF */
