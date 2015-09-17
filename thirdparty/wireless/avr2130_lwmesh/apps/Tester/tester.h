/**
 * \file tester.h
 *
 * \brief Tester application interface
 *
 * Copyright (C) 2012-2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
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
 *
 */

#ifndef _TESTER_H_
#define _TESTER_H_

/*- Definitions ------------------------------------------------------------*/
#define APP_UART_START_SYMBOL     0xA5
#define APP_UART_BUFFER_SIZE      500

/*- Types ------------------------------------------------------------------*/
typedef enum AppStatus_t {
	APP_STATUS_SUCESS               = 0x00,
	APP_STATUS_INVALID_CRC          = 0x01,
	APP_STATUS_UNKNOWN_COMMAND      = 0x02,
	APP_STATUS_MALFORMED_COMMAND    = 0x03,
	APP_STATUS_TABLE_IS_FULL        = 0x04,
	APP_STATUS_ENTRY_NOT_FOUND      = 0x05,
	APP_STATUS_OUT_OF_MEMORY        = 0x06,
	APP_STATUS_INVALID_PARAMETERS   = 0x07,
} AppStatus_t;

/*- Prototypes -------------------------------------------------------------*/
AppStatus_t appCommandReceived(uint8_t *buf, uint16_t size);

#endif /* _TESTER_H_ */
