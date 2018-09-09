/**
 * \file serial_bridge.c
 *
 * \brief Handles Serial Bridge driver functionalities
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
#include "serial_drv.h"

/* === GLOBALS ========================================================== */
extern volatile bool ble_usart_tx_cmpl;
volatile bool ble_eusart_tx_cmpl = true;

static ser_fifo_desc_t ble_eusart_tx_fifo;
static  uint8_t ble_eusart_tx_buf[BLE_MAX_TX_PAYLOAD_SIZE];
static  uint8_t ble_usart_tx_buf[BLE_MAX_TX_PAYLOAD_SIZE];


static ser_fifo_desc_t ble_eusart_rx_fifo;
static uint8_t ble_eusart_rx_buf[BLE_MAX_TX_PAYLOAD_SIZE];

ser_fifo_desc_t ble_usart_tx_fifo;
ser_fifo_desc_t ble_usart_rx_fifo;
uint8_t ble_usart_rx_buf[BLE_MAX_RX_PAYLOAD_SIZE];

/* === TYPES =============================================================== */

/* === MACROS ============================================================== */

/**
 * \brief Set peripheral mode for one single IOPORT pin.
 * It will configure port mode and disable pin mode (but enable peripheral).
 * \param pin IOPORT pin to configure
 * \param mode Mode masks to configure for the specified pin (\ref ioport_modes)
 */
#define ioport_set_pin_peripheral_mode(pin, mode) \
	do {\
		ioport_set_pin_mode(pin, mode);\
		ioport_disable_pin(pin);\
	} while (0)
	
/* === PROTOTYPES ========================================================== */

/* === GLOBALS ========================================================== */

/* === IMPLEMENTATION ====================================================== */




uint8_t serial_bridge_init(void)
{
#if SAMG55	
	/* Usart async mode 8 bits transfer test */
	sam_usart_opt_t usart_settings = {
		.baudrate     = CONF_UART_BAUDRATE,
		.char_length  = US_MR_CHRL_8_BIT,
		.parity_type  = US_MR_PAR_NO,
		.stop_bits    = US_MR_NBSTOP_1_BIT,
		.channel_mode = US_MR_CHMODE_NORMAL,
		/* This field is only used in IrDA mode. */
		.irda_filter  = 0
	};
#endif
#if SAM4S
	const sam_uart_opt_t uart_settings = {
		.ul_mck			= sysclk_get_peripheral_hz(),
		.ul_baudrate   		= CONF_UART_BAUDRATE,
		.ul_mode		= US_MR_CHMODE_NORMAL		
	};
#endif
#if SAMG55
	/* Enable the peripheral and set USART mode. */
	flexcom_enable(SB_USART_FLEXCOM);
	flexcom_set_opmode(SB_USART_FLEXCOM, FLEXCOM_USART);

	/* Configure USART */
	usart_init_rs232(SB_UART, &usart_settings,
	sysclk_get_peripheral_hz());
	
	/* Enable the receiver and transmitter. */
	usart_enable_tx(SB_UART);
	usart_enable_rx(SB_UART);

	/* Enable UART IRQ */
	usart_enable_interrupt(SB_UART, US_IER_RXRDY);
#endif	
#if SAM4S
	/* Enabling the peripheral clock */
	sysclk_enable_peripheral_clock(SB_UART_ID);
	
	/* Enabling Pull-Up for UART RXD pin */
	pio_pull_up(PIOB,PIO_PB2A_URXD1,PIO_PULLUP);
	
	/* Initialize UART */
	uart_init(SB_UART,&uart_settings);
	
	/* Enable UART IRQ */
	uart_enable_interrupt(SB_UART, US_IER_RXRDY);
#endif

	ser_fifo_init(&ble_eusart_rx_fifo, ble_eusart_rx_buf, BLE_MAX_RX_PAYLOAD_SIZE);
	ser_fifo_init(&ble_eusart_tx_fifo, ble_eusart_tx_buf, BLE_MAX_TX_PAYLOAD_SIZE);
	ser_fifo_init(&ble_usart_tx_fifo, ble_usart_tx_buf, BLE_MAX_TX_PAYLOAD_SIZE);
	ser_fifo_init(&ble_usart_rx_fifo, ble_usart_rx_buf, BLE_MAX_RX_PAYLOAD_SIZE);

	/* Enable UART interrupt */
	NVIC_EnableIRQ(SB_UART_IRQn);

	return STATUS_OK;
}

void SB_UART_Handler(void)
{
#if SAMG55	
	if (usart_is_rx_ready(SB_UART))
	{
		uint32_t rx_data = 0;
		if (usart_read(SB_UART, &rx_data) == 0)
		{
			ser_fifo_push_uint8(&ble_eusart_rx_fifo, rx_data);
		}
	}
	
	if ((usart_is_tx_empty(SB_UART))  && (!ble_eusart_tx_cmpl))
	{
		uint8_t txdata;
		if(ser_fifo_pull_uint8(&ble_eusart_tx_fifo, &txdata) == SER_FIFO_OK)
		{
			usart_putchar(SB_UART, txdata);
		}
		else
		{
			usart_disable_interrupt(SB_UART, US_IER_TXEMPTY);
			ble_eusart_tx_cmpl  = true;
		}
	}
#endif
#if SAM4S
	if (uart_is_rx_ready(SB_UART))
	{
		uint32_t rx_data = 0;
		if (uart_read(SB_UART, (uint8_t *)&rx_data) == 0)
		{
			ser_fifo_push_uint8(&ble_eusart_rx_fifo, rx_data);
		}
	}
	
	if ((uart_is_tx_empty(SB_UART))  && (!ble_eusart_tx_cmpl))
	{
		uint8_t txdata;
		if(ser_fifo_pull_uint8(&ble_eusart_tx_fifo, &txdata) == SER_FIFO_OK)
		{
			uart_write(SB_UART, txdata);
		}
		else
		{
			uart_disable_interrupt(SB_UART, US_IER_TXEMPTY);
			ble_eusart_tx_cmpl  = true;
		}
	}
#endif
}

extern void ble_pdc_send_data(uint8_t *buf, uint16_t len);

void serial_bridge_task(void)
{
	uint8_t t_rx_data;

	/* Check the UART Rx data from BLE UART */
	if(ser_fifo_pull_uint8(&ble_usart_rx_fifo, (uint8_t *)&t_rx_data) == SER_FIFO_OK)
	{		
		LED_Toggle(LED0);
		/* Write to the EDBG UART Buffer */
		ser_fifo_push_uint8(&ble_eusart_tx_fifo, (uint8_t)t_rx_data);
		if(ble_eusart_tx_cmpl)
		{			
		  if(ser_fifo_pull_uint8(&ble_eusart_tx_fifo, &t_rx_data) == SER_FIFO_OK)
		  {	
			  ble_eusart_tx_cmpl = false;
			  #if SAMG55
			  usart_putchar(SB_UART, t_rx_data);
			  //Enable the USART Empty Interrupt
			  usart_enable_interrupt(SB_UART, US_IER_TXEMPTY);				
			  #endif
			  #if SAM4S
			  uart_write(SB_UART, t_rx_data);
			  //Enable the USART Empty Interrupt
			  uart_enable_interrupt(SB_UART, US_IER_TXEMPTY);
			  #endif				  
			  while(false == ble_eusart_tx_cmpl);		  
		  }
		}
	}
	
	/* Check the UART Rx data from EDBG UART */
	if(ser_fifo_pull_uint8(&ble_eusart_rx_fifo, (uint8_t *)&t_rx_data) == SER_FIFO_OK)
	{
		LED_Toggle(LED0);
		/* Write to BLE UART Buffer */
		ser_fifo_push_uint8(&ble_usart_tx_fifo, (uint8_t)t_rx_data);
		if (ble_usart_tx_cmpl)
		{
			static uint8_t ble_usart_tx_byte[2] = {0, 0};
			if(ser_fifo_pull_uint8(&ble_usart_tx_fifo, &t_rx_data) == SER_FIFO_OK)
			{
				ble_usart_tx_byte[0] = t_rx_data;
				ble_usart_tx_cmpl = false;
				ble_pdc_send_data(ble_usart_tx_byte, 1);
				while(false == ble_usart_tx_cmpl);
			}
		}		
	}
}

void platform_dtm_interface_receive(uint8_t rx_data)
{
	platform_start_rx();
	ser_fifo_push_uint8(&ble_usart_rx_fifo, (uint8_t)rx_data);
}

/* EOF */
