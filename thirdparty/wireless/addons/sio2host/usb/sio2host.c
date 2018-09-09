/**
 * \file sio2host.c
 *
 * \brief Event handling Serial I/O  Functionalities
 *
 * Copyright (c) 2013-2018 Microchip Technology Inc. and its subsidiaries.
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

#include "conf_sio2host.h"
#include "sio2host.h"
#include "stdio_usb.h"

/* === TYPES =============================================================== */

/* === MACROS ============================================================== */

/* === PROTOTYPES ========================================================== */

/**
 * Receive buffer
 * The buffer size is defined in sio2host.h
 */
static uint8_t serial_rx_buf[SERIAL_RX_BUF_SIZE_HOST];

/**
 * Receive buffer head
 */
static uint8_t serial_rx_buf_head;

/**
 * Receive buffer tail
 */
static uint8_t serial_rx_buf_tail;

/**
 * Number of bytes in receive buffer
 */
static uint8_t serial_rx_count;

static volatile bool usb_rx_byte_rcvd = false;
static volatile bool main_b_cdc_enable = false;

/* === IMPLEMENTATION ====================================================== */

void sio2host_init(void)
{
	stdio_usb_init();
}

uint8_t sio2host_tx(uint8_t *data, uint8_t length)
{
	int status;
	uint8_t len;
	len = length;
	
   while (len) {
	  status = udi_cdc_putc(*data);
	  /* Check transmission status */
	  if(status)
	   { 
	   /* Put next byte if the previous transfer is successful */
	    len--;
	    data++;
		}
   }
   return length;
}

uint8_t sio2host_rx(uint8_t *data, uint8_t max_length)
{
	uint8_t data_received = 0;
	if (0 == serial_rx_count) {
		return 0;
	}

	if (SERIAL_RX_BUF_SIZE_HOST <= serial_rx_count) {
		/*
		 * Bytes between head and tail are overwritten by new data.
		 * The oldest data in buffer is the one to which the tail is
		 * pointing. So reading operation should start from the tail.
		 */
		serial_rx_buf_head = serial_rx_buf_tail;

		/*
		 * This is a buffer overflow case.But still only the number of
		 * bytes equivalent to
		 * full buffer size are useful.
		 */
		serial_rx_count = SERIAL_RX_BUF_SIZE_HOST;

		/* Bytes received is more than or equal to buffer. */
		if (SERIAL_RX_BUF_SIZE_HOST <= max_length) {
			/*
			 * Requested receive length (max_length) is more than
			 * the
			 * max size of receive buffer, but at max the full
			 * buffer can be read.
			 */
			max_length = SERIAL_RX_BUF_SIZE_HOST;
		}
	} else {
		/* Bytes received is less than receive buffer maximum length. */
		if (max_length > serial_rx_count) {
			/*
			 * Requested receive length (max_length) is more than
			 * the data
			 * present in receive buffer. Hence only the number of
			 * bytes
			 * present in receive buffer are read.
			 */
			max_length = serial_rx_count;
		}
	}

	data_received = max_length;
	while (max_length > 0) {
		/* Start to copy from head. */
		*data = serial_rx_buf[serial_rx_buf_head];
		serial_rx_buf_head++;
		serial_rx_count--;
		data++;
		max_length--;
		if ((SERIAL_RX_BUF_SIZE_HOST) == serial_rx_buf_head) {
			serial_rx_buf_head = 0;
		}
	}
	return data_received;
}

uint8_t sio2host_getchar(void)
{
	uint8_t c;
	while (0 == sio2host_rx(&c, 1)) {
	}
	return c;
}

void sio2host_putchar(uint8_t ch)
{
	sio2host_tx(&ch, 1);
}

int sio2host_getchar_nowait(void)
{
	uint8_t c;

	int back = sio2host_rx(&c, 1);
	if (back >= 1) {
		return c;
	} else {
		return (-1);
	}
}

bool main_cdc_enable(uint8_t port)
{
	/* Indication to application when CDC is enabled successfully */

	main_b_cdc_enable = true;

	/* Keep Compiler happy */
	port = port;

	return true;
}

void main_cdc_disable(uint8_t port)
{
	main_b_cdc_enable = false;

	/* Keep Compiler happy */
	port = port;
}

void dtr_cb(bool set)
{
	if (set) {
		stdio_usb_enable();
	} else {
		stdio_usb_disable();
	}
}

void usb_rx_notify(void)
{
	if (main_b_cdc_enable) {
		while (udi_cdc_is_rx_ready()) {
			uint8_t temp;
			temp = udi_cdc_getc();

			/* Introducing critical section to avoid buffer
			 * corruption. */
			cpu_irq_disable();

			/* The count of characters present in receive buffer is
			 * incremented. */
			serial_rx_count++;
			serial_rx_buf[serial_rx_buf_tail] = temp;

			if ((SERIAL_RX_BUF_SIZE_HOST - 1) ==
					serial_rx_buf_tail) {
				/* Reached the end of buffer, revert back to
				 * beginning of buffer. */
				serial_rx_buf_tail = 0x00;
			} else {
				serial_rx_buf_tail++;
			}

			cpu_irq_enable();
		}
	}
}

/** EOF */
