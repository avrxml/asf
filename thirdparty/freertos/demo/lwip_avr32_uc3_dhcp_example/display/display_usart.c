/**
 * \file
 *
 * \brief Display Usart Interface
 *
 * Copyright (c) 2011-2018 Microchip Technology Inc. and its subsidiaries.
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
#include <stdio.h>
#include "board.h"
#include "display.h"
#include "gpio.h"
#include "delay.h"
#include "stdio_usb.h"

/*!
 * \brief Initialize Display function
 *          - Initializa stdio interface using the USB CDC Interface,
 *          - Print startup message.
 * \param fcpu_hz CPU Frequency Value in Hz.
 */
void display_init(uint32_t fcpu_hz)
{
 	/* Start and attach USB CDC device interface for devices with
 	 * integrated USB interfaces.
 	 */
	stdio_usb_init(NULL);

	// Display default message.
	puts("\x1B[5;1H");
	puts("ATMEL  UC3\r\n");
	puts("lwIP with DHCP demo \r\n");
	puts("Not Connected \r\n");
}

/*!
 * \brief Display string message
 * \param str String input.
 */
void display_print(const char *str)
{
	puts(str);
	puts("\r\n");
}
