/**
 * \file
 *
 * \brief USART driver
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
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


#include "usart.h"

/**
 * \defgroup usart_driver_group USART driver
 *
 * The USART driver is used to send or receive characters using USART1.
 *
 * @{
 */

/** \brief Function to get a char from the USART if it is ready.
 *
 * \retval UDR1     Returns USART1 data register
 */
uint8_t usart1_getchar(void)
{
	// Wait for incoming data
	while (!(UCSR1A & (1 << RXC1)));

	// Return the read data
	return UDR1;
}

/** \brief Function to transmit a char using USART1 if it is ready.
 *
 * \param c     Variable used to transmit a character using USART1
 *
 */
int usart1_putchar(int c)
{
	UCSR1A |= (1 << TXC1);
	if (c == '\n') {
		putchar('\r');
	}
	// Wait for empty transmit buffer
	while (!(UCSR1A & (1 << UDRE1)));

	// Start transmission
	return (UDR1 = c);
}

 //! @}
