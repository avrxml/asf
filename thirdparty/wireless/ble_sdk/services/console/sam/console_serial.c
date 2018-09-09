/**
 * \file console_serial.c
 *
 * \brief Serial Console functionalities
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
#include "console_serial.h"
#include "conf_uart_serial.h"
#include "usart.h"
#include "platform.h"
#include "timer_hw.h"

/* === TYPES =============================================================== */

/* === MACROS ============================================================== */

#ifndef CONF_UART
#define CONF_UART            CONSOLE_UART
#endif

#ifndef CONF_UART_BAUDRATE
#define CONF_UART_BAUDRATE (115200UL)
#endif

#ifndef CONF_UART_CHAR_LENGTH
#define CONF_UART_CHAR_LENGTH	US_MR_CHRL_8_BIT
#endif

#ifndef CONF_UART_PARITY
#define CONF_UART_PARITY		US_MR_PAR_NO
#endif

#ifndef CONF_UART_STOP_BITS
#define CONF_UART_STOP_BITS		US_MR_NBSTOP_1_BIT
#endif

/**
 *  Configure console.
 */
void serial_console_init(void)
{
	const usart_serial_options_t uart_serial_options = {
			.baudrate = CONF_UART_BAUDRATE,
	#ifdef CONF_UART_CHAR_LENGTH
			.charlength = CONF_UART_CHAR_LENGTH,
	#endif
			.paritytype = CONF_UART_PARITY,
	#ifdef CONF_UART_STOP_BITS
			.stopbits = CONF_UART_STOP_BITS,
	#endif
		};

	/* Configure console UART. */
	sysclk_enable_peripheral_clock(CONSOLE_UART_ID);
	stdio_serial_init(CONF_UART, &uart_serial_options);
}

uint8_t getchar_timeout(uint32_t timeout)
{
	uint32_t temp = 0XFF;
	
	while((STATUS_OK != usart_read((Usart *)CONF_UART, &temp)) && timeout){
		timeout--;
		delay_ms(1);
	}

	return ((uint8_t)(temp));
}

/* EOF */
