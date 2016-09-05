/**
 * \file console_serial.c
 *
 * \brief Serial Console functionalities
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
	uint32_t temp = NULL;
	
	while((STATUS_OK != usart_read((Usart *)CONF_UART, &temp)) && timeout){
		timeout--;
		delay_ms(1);
	}

	return ((uint8_t)(temp & 0xFF));
}

/* EOF */
