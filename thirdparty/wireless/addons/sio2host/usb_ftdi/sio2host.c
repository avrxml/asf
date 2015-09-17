/**
 * \file sio2host.c
 *
 * \brief Event handling Serial I/O  Functionalities
 *
 * Copyright (c) 2013-2015 Atmel Corporation. All rights reserved.
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

#include "conf_sio2host.h"
#include "sio2host.h"
#include "board.h"
#include "stdio.h"

#ifdef SENSOR_TERMINAL_BOARD
/* === TYPES =============================================================== */

/* === MACROS ============================================================== */

/* === PROTOTYPES ========================================================== */

/*
 * Various helper macros for the individual IO ports related to
 * attaching the FT245.
 */
#define USB_DATA_SETINP()   do { USB_DATA_DDR = 0x00; USB_DATA_PORT = 0x00; } \
	while (0)
#define USB_DATA_SETOUTP()  do { USB_DATA_DDR = 0xFF; }	\
	while (0)
#define USB_CTRL_RD_LO()    do { USB_CTRL_PORT &= ~USB_RD; } \
	while (0)
#define USB_CTRL_RD_HI()    do { USB_CTRL_PORT |= USB_RD; } \
	while (0)
#define USB_CTRL_WR_LO()    do { USB_CTRL_PORT &= ~USB_WR; } \
	while (0)
#define USB_CTRL_WR_HI()    do { USB_CTRL_PORT |= USB_WR; } \
	while (0)

/* Please dont change this type. */
typedef enum pin_state_tag {
	/** Pin state low */
	LOW,
	/** Pin state high */
	HIGH
} SHORTENUM pin_state_t;
/* ! @} */
/* === Globals ============================================================= */

/**
 * USB transmit buffer
 */
uint8_t usb_tx_buf[USB_TX_BUF_SIZE];

/**
 * USB receive buffer
 */
uint8_t usb_rx_buf[USB_RX_BUF_SIZE];

/**
 * Transmit buffer head
 */
static uint8_t usb_tx_buf_head;

/**
 * Transmit buffer tail
 */
static uint8_t usb_tx_buf_tail;

/**
 * Receive buffer head
 */
static uint8_t usb_rx_buf_head;

/**
 * Receive buffer tail
 */
static uint8_t usb_rx_buf_tail;

/**
 * Number of bytes in transmit buffer
 */
static uint8_t usb_tx_count;

/**
 * Number of bytes in receive buffer
 */
static uint8_t usb_rx_count;

static void usb_getc(void);
static void usb_putc(void);

/**
 * USB Tx busy flag: This is required in order to detect a Tx busy condition if
 * the terminal program is not running, so that the FTDI chip cannot deliver
 * its data.
 */
static bool usb_tx_busy = false;

#if (XMEGA || MEGA_RF) && defined(__GNUC__)
extern int _write(char c, int *f);
extern int _read(int *f);

#endif

/* === IMPLEMENTATION ====================================================== */

void sio2host_init(void)
{
	/* Set HIGH before switching to output to prevent RD cycle */
	USB_CTRL_PORT |= USB_RD;
	USB_CTRL_DDR |= USB_RD;

	/* Set HIGH before switching to output to prevent WR cycle */
	USB_CTRL_PORT |= USB_WR;
	USB_CTRL_DDR |= USB_WR;

	USB_CTRL_DDR &= ~USB_RXF;
	USB_CTRL_DDR &= ~USB_TXE;

	/*
	 * If the board features an address latch, trigger it to activate
	 * the address decoder.
	 */
#if defined(USB_ALE_PORT) && defined(USB_ALE_PIN) && defined(USB_FIFO_AD)
	USB_ALE_DDR |= USB_ALE_PIN;
	USB_ALE_PORT |= USB_ALE_PIN;
	USB_DATA_SETOUTP();
	USB_DATA_PORT = USB_FIFO_AD >> 8; /* high byte of FIFO address */
	nop();
	nop();
	USB_ALE_PORT &= ~USB_ALE_PIN;
#endif  /* defined(USB_ALE_PORT) && defined(USB_ALE_PIN) && defined(USB_FIFO_AD)
	**/

	/* Flush all input data from FIFO. */
	USB_DATA_SETINP();
	while ((USB_CTRL_PIN & USB_RXF) == 0) {
		USB_CTRL_RD_LO();
		USB_CTRL_RD_HI();
	}

	/*
	 * Some boards require a non-generic (board specific) initialization.
	 * If this is required, the following macro is filled with the proper
	 * implementation (see board specific file pal_config.h).
	 * If this is not required, the macro will be empty.
	 */
	USB_INIT_NON_GENERIC();

	#if (XMEGA || MEGA_RF) && defined(__GNUC__)
	/* For AVR GCC libc print redirection uses fdevopen. */
	fdevopen((int (*)(char, FILE *))(_write), (int (*)(FILE *))(_read));
	#  endif
}

uint8_t sio2host_tx(uint8_t *data, uint8_t length)
{
	uint8_t bytes_to_be_written;
	uint8_t size;
	uint8_t back;

	/*
	 * USB Tx has been marked as busy, e.g. maybe the terminal is not open.
	 * In order to continue the execution of the application even if
	 * USB Tx data cannot be delivered, we are going to return here
	 * immediately.
	 * Once USB Tx is not busy anymore, regular USB Tx procedures will
	 * be resumed.
	 */
	if (usb_tx_busy) {
		return (0);
	}

	/*
	 * Calculate available buffer space
	 */
	if (usb_tx_buf_tail >= usb_tx_buf_head) {
		size
			= (USB_TX_BUF_SIZE -
				1) - (usb_tx_buf_tail - usb_tx_buf_head);
	} else {
		size = (usb_tx_buf_head - 1) - usb_tx_buf_tail;
	}

	if (size < length) {
		/* Not enough buffer space available. Use the remaining size */
		bytes_to_be_written = size;
	} else {
		bytes_to_be_written = length;
	}

	/* Remember the number of bytes transmitted. */
	back = bytes_to_be_written;
	usb_tx_count += bytes_to_be_written;

	/* The data is copied to the transmit buffer. */
	while (bytes_to_be_written > 0) {
		usb_tx_buf[usb_tx_buf_tail] = *data;

		if ((USB_TX_BUF_SIZE - 1) == usb_tx_buf_tail) {
			/* Reached the end of buffer, revert back to beginning
			 * of buffer. */
			usb_tx_buf_tail = 0;
		} else {
			/*
			 * Increment the index to point the next character to be
			 * inserted.
			 */
			usb_tx_buf_tail++;
		}

		bytes_to_be_written--;
		data++;
	}

	/*
	 * If a transmission needs to be started, pal_usb_handler() takes
	 * care about that once it is run.
	 */
	usb_handler();
	return back;
}

uint8_t sio2host_rx(uint8_t *data, uint8_t max_length)
{
	usb_handler();
	uint8_t data_received = 0;

	if (usb_rx_count == 0) {
		/* USB receive buffer is empty. */
		return 0;
	}

	if (USB_RX_BUF_SIZE <= usb_rx_count) {
		/*
		 * Bytes between head and tail are overwritten by new data.
		 * The oldest data in buffer is the one to which the tail is
		 * pointing. So reading operation should start from the tail.
		 */
		usb_rx_buf_head = usb_rx_buf_tail;

		/*
		 * This is a buffer overflow case. Byt still only bytes
		 * equivalent to
		 * full buffer size are useful.
		 */
		usb_rx_count = USB_RX_BUF_SIZE;

		/* Bytes received is more than or equal to buffer. */
		if (USB_RX_BUF_SIZE <= max_length) {
			/*
			 * Requested receive length (max_length) is more than
			 * the
			 * max size of receive buffer, but at max the full
			 * buffer can be read.
			 */
			max_length = USB_RX_BUF_SIZE;
		}
	} else {
		/* Bytes received is less than receive buffer maximum length. */
		if (max_length > usb_rx_count) {
			/*
			 * Requested receive length (max_length) is more than
			 * the data
			 * present in receive buffer. Hence only the number of
			 * bytes
			 * present in receive buffer are read.
			 */
			max_length = usb_rx_count;
		}
	}

	data_received = max_length;

	while (max_length > 0) {
		/* Start to copy from head. */
		*data = usb_rx_buf[usb_rx_buf_head];

		if (++usb_rx_buf_head == USB_RX_BUF_SIZE) {
			usb_rx_buf_head = 0;
		}

		usb_rx_count--;
		data++;
		max_length--;
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

/*
 * \brief Services data transmission or reception on USB
 *
 * This function polls for usb for completion of reception or transmission of
 * a byte on USB.
 */
void usb_handler(void)
{
	/*
	 * USB Status is the upper 2 bits of Port E
	 * PE6 - TXE  write is allowed if Low
	 * PE7 - RXF  Data available if Low
	 */
	uint8_t cUSBstat = USB_CTRL_PIN;

	if (LOW == (cUSBstat & USB_RXF)) {
		/* The received data is read in the PAL USB buffer */
		usb_getc();
	}

	/* Transmit data, if any */
	if (usb_tx_count > 0) {
		uint8_t bytes_tx = MAX_BYTES_FOR_USB_TX;

		while ((usb_tx_count > 0) && (bytes_tx > 0)) {
			/*
			 * To avoid slow transmission (byte per pal_task), a
			 * maximum of
			 * MAX_BYTES_FOR_USB_TX bytes or number of bytes present
			 * in USB
			 * buffer, whichever is minimum is transmitted at one
			 * go.
			 */
			if (LOW == (cUSBstat & USB_TXE)) {
				if (usb_tx_busy) {
					/*
					 * Reset USB busy handling variables and
					 * mark USB as not busy.
					 */
					usb_tx_busy = false;
				}

				usb_putc();

				bytes_tx--;
			} else {
				break;
			} /* while ((usb_tx_count > 0) && (bytes_tx > 0)) */
		}
	} /* if (usb_tx_count > 0) */
}

/**
 * \brief Puts a character onto USB FIFO
 *
 * This function transmits a byte over usb.
 * \ingroup group_pal_sio
 */
static void usb_putc(void)
{
	if (usb_tx_count > 0) {
		/* The number of bytes to be transmitted is decremented. */
		usb_tx_count--;

		USB_DATA_SETOUTP();
		USB_DATA_PORT = usb_tx_buf[usb_tx_buf_head];
		USB_CTRL_WR_LO();
		USB_CTRL_WR_HI();
		USB_DATA_SETINP();

		if ((USB_TX_BUF_SIZE - 1) == usb_tx_buf_head) {
			/* Reached the end of buffer, revert back to beginning
			 * of buffer */
			usb_tx_buf_head = 0;
		} else {
			usb_tx_buf_head++;
		}
	}
}

/**
 * \brief Gets a byte from USB FIFO
 *
 * This function receives a byte from usb.
 * \ingroup group_pal_sio
 */
static void usb_getc(void)
{
	/*
	 * Characters can only be read from USB chip, if there is still
	 * space in receive buffer.
	 */
	if (usb_rx_count < USB_RX_BUF_SIZE) {
		/* The count of characters present in receive buffer is
		 * incremented. */
		usb_rx_count++;

		/* Read a character from the fifo address. */
		USB_DATA_SETINP();
		USB_CTRL_RD_LO();
		nop();
		nop();
		usb_rx_buf[usb_rx_buf_tail] = USB_DATA_PIN;
		USB_CTRL_RD_HI();
		if ((USB_RX_BUF_SIZE - 1) == usb_rx_buf_tail) {
			/* Reached the end of buffer, revert back to beginning
			 * of buffer. */
			usb_rx_buf_tail = 0x00;
		} else {
			usb_rx_buf_tail++;
		}
	}
}

#if (defined(__ICCAVR32__) || defined(__ICCAVR__) || defined(__ICCARM__))

#include <yfuns.h>

_STD_BEGIN

#pragma module_name = "?__write"

size_t __write(int handle, const unsigned char *buffer, size_t size)
{
	if (buffer == 0) {
		return 0;
	}

	/* This implementation only writes to stdout and stderr. */
	/* For all other file handles, it returns failure. */
	if (handle != _LLIO_STDOUT && handle != _LLIO_STDERR) {
		return _LLIO_ERROR;
	}

	sio2host_tx((uint8_t *)buffer, (uint8_t)size);

	return size;
}

_STD_END

#elif (defined(__GNUC__) && !XMEGA && !MEGA)

int __attribute__((weak)) _write(int file, const char *ptr, int len);

int __attribute__((weak)) _write(int file, const char *ptr, int len)
{
	if ((file != 1) && (file != 2) && (file != 3)) {
		return -1;
	}

	sio2host_tx((uint8_t *)buffer, (uint8_t)size);
	return size;
}

#elif (defined(__GNUC__) && (XMEGA || MEGA))

int _write(char c, int *f);

int _write(char c, int *f)
{
	sio2host_tx((uint8_t *)&c, 1);
	return 1;
}

int _read(int *f);  /* Remove GCC compiler warning */

int _read(int *f)
{
	char c;
	sio2host_rx((uint8_t *)&c, 1);
	return c;
}

#endif

#endif /* #ifdef SENSOR_TERMINAL_BOARD */
/** EOF */
