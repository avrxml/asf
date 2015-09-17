/**
 * \file
 *
 * \brief Function for the XMODEM transfer protocol
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

#include "xmodem.h"
#include "sysclk.h"

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/// @endcond

/** The definitions are followed by the XMODEM protocol */
#define XMDM_SOH     0x01 /**< Start of heading */
#define XMDM_EOT     0x04 /**< End of text */
#define XMDM_ACK     0x06 /**< Acknowledge  */
#define XMDM_NAK     0x15 /**< negative acknowledge */
#define XMDM_CAN     0x18 /**< Cancel */
#define XMDM_ESC     0x1b /**< Escape */

#define CRC16POLY   0x1021  /**< CRC 16 polynom */
#define PKTLEN_128  128u    /**< Packet length */

/**
 * \brief Get bytes through XMODEM protocol.
 *
 * \param usart  Base address of the USART instance.
 * \param p_data  Pointer to the data buffer.
 * \param ul_length Length of data expected.
 *
 * \return Calculated CRC value
 */
static uint16_t xmodem_get_bytes(usart_if usart, int8_t *p_data,
		uint32_t ul_length)
{
	uint16_t us_crc = 0;
	uint32_t i, j;
	uint8_t c_char;

	for (i = 0; i < ul_length; ++i) {
		usart_serial_getchar(usart, &c_char);
		us_crc = us_crc ^ (int32_t) c_char << 8;
		for (j = 0; j < 8; j++) {
			if (us_crc & 0x8000) {
				us_crc = us_crc << 1 ^ CRC16POLY;
			} else {
				us_crc = us_crc << 1;
			}
		}
		us_crc = us_crc & 0xFFFF;
		*p_data++ = c_char;
	}
	return us_crc;
}

/**
 * \brief Get a packet through XMODEM protocol
 *
 * \param usart  Base address of the USART instance.
 * \param p_data  Pointer to the data buffer.
 * \param uc_sno  Sequnce number.
 *
 * \return 0 for sucess and other value for XMODEM error
 */
static int32_t xmodem_get_packet(usart_if usart, int8_t *p_data,
		uint8_t uc_sno)
{
	uint8_t uc_cp_seq[2], uc_temp[2];
	uint16_t us_crc, us_xcrc;

	xmodem_get_bytes(usart, (int8_t *)uc_cp_seq, 2);

	us_xcrc = xmodem_get_bytes(usart, p_data, PKTLEN_128);

	/* An "endian independent way to combine the CRC bytes. */
	usart_serial_getchar(usart, &uc_temp[0]);
	usart_serial_getchar(usart, &uc_temp[1]);

	us_crc = uc_temp[0] << 8;
	us_crc += uc_temp[1];

	if ((us_crc != us_xcrc) || (uc_cp_seq[0] != uc_sno) ||
			(uc_cp_seq[1] != (uint8_t) ((~(uint32_t)uc_sno) & 0xff))) {
		usart_serial_putchar(usart, XMDM_CAN);
		return (-1);
	}

	return 0;
}

/**
 * \brief Send a packet through XMODEM protocol
 *
 * \param usart  Base address of the USART instance.
 * \param p_data  Pointer to the data buffer.
 * \param uc_sno  Sequnce number.
 *
 * \return 0 for sucess and other value for XMODEM error
 */
static uint8_t xmodem_send_packet(usart_if usart, uint8_t *p_data,
		uint8_t uc_sno)
{
	uint32_t  i, j;
	uint16_t us_check_sum;
	int8_t	 c_data;
	uint8_t	 uc_ack;

	us_check_sum = 0;

	usart_serial_putchar(usart, XMDM_SOH);
	usart_serial_putchar(usart, uc_sno);
	usart_serial_putchar(usart, ((uint8_t)(~(uc_sno))));

	for(i = 0; i < PKTLEN_128; i++) {
		c_data = *p_data++;
		usart_serial_putchar(usart, c_data);

		us_check_sum = us_check_sum ^ (int32_t) c_data << 8;
		for (j = 0; j < 8; j++) {
			if (us_check_sum & 0x8000) {
				us_check_sum = us_check_sum << 1 ^ CRC16POLY;
			} else {
				us_check_sum = us_check_sum << 1;
			}
		}
		us_check_sum = us_check_sum & 0xFFFF;
	}

	/* An "endian independent way to extract the CRC bytes. */
	usart_serial_putchar(usart, (uint8_t)(us_check_sum >> 8));
	usart_serial_putchar(usart, (uint8_t)us_check_sum);

	usart_serial_getchar(usart, &uc_ack);

	return uc_ack;	/* Wait for ack */
}


/**
 * \brief Receive the files through XMODEM protocol
 *
 * \param usart  Base address of the USART instance.
 * \param p_buffer  Pointer to receive buffer
 *
 * \return received file size
 */
uint32_t xmodem_receive_file(usart_if usart, int8_t *p_buffer)
{
	uint32_t ul_timeout;
	uint8_t c_char;
	int32_t l_done;
	uint8_t uc_sno = 0x01;
	uint32_t ul_size = 0;

	/* Wait and put 'C' till start XMODEM transfer */
	while (1) {
		usart_serial_putchar(usart, 'C');
		ul_timeout = (sysclk_get_peripheral_hz() / 10);
		while (!(usart_serial_is_rx_ready(usart)) && ul_timeout) {
			ul_timeout--;
		}
		if (usart_serial_is_rx_ready(usart)) {
			break;
		}
	}

	/* Begin to receive the data */
	l_done = 0;
	while (l_done == 0) {
		usart_serial_getchar(usart, &c_char);

		switch (c_char) {
		/* Start of transfer */
		case XMDM_SOH:
			l_done = xmodem_get_packet(usart, p_buffer+ul_size, uc_sno);
			if (l_done == 0) {
				uc_sno++;
				ul_size += PKTLEN_128;
			}
			usart_serial_putchar(usart, XMDM_ACK);
			break;

		/* End of transfer */
		case XMDM_EOT:
			usart_serial_putchar(usart, XMDM_ACK);
			l_done = ul_size;
			break;

		case XMDM_CAN:
		case XMDM_ESC:
		default:
			l_done = -1;
			break;
		}
	}
	return ul_size;
}

/**
 * \brief Send the files through XMODEM protocol
 *
 * \param usart  Base address of the USART instance.
 * \param p_buffer  Pointer to send buffer
 * \param ul_length transfer file size
 */
void xmodem_send_file(usart_if usart, int8_t *p_buffer, uint32_t ul_length)
{
	uint8_t c_char, uc_sno = 1;
	int32_t l_done;
	uint32_t ul_timeout = (sysclk_get_peripheral_hz() / 10);

	if (ul_length & (PKTLEN_128-1)) {
		ul_length += PKTLEN_128;
		ul_length &= ~(PKTLEN_128-1);
	}

	/* Startup synchronization... */
	/* Wait to receive a NAK or 'C' from receiver. */
	l_done = 0;
	while(!l_done) {
		usart_serial_getchar(usart, &c_char);
		switch (c_char) {
		case XMDM_NAK:
			l_done = 1;
			break;
		case 'C':
			l_done = 1;
			break;
		case 'q':	/* ELS addition, not part of XMODEM spec. */
			return;
		default:
			break;
		}
	}

	l_done = 0;
	uc_sno = 1;
	while (!l_done) {
		c_char = xmodem_send_packet(usart, (uint8_t *)p_buffer, uc_sno);
		switch(c_char) {
		case XMDM_ACK:
			++uc_sno;
			ul_length -= PKTLEN_128;
			p_buffer += PKTLEN_128;
			break;
		case XMDM_NAK:
			break;
		case XMDM_CAN:
		case XMDM_EOT:
		default:
			l_done = -1;
			break;
		}
		if (!ul_length) {
			usart_serial_putchar(usart, XMDM_EOT);
			/* Flush the ACK */
			usart_serial_getchar(usart, &c_char);
			break;
		}
	}
}

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/// @endcond

