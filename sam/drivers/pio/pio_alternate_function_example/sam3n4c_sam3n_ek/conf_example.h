/**
 * \file
 *
 * \brief Configuration for PIO example.
 *
 * Copyright (c) 2011-2015 Atmel Corporation. All rights reserved.
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

#ifndef CONF_PIO_EXAMPLE_H_INCLUDED
#define CONF_PIO_EXAMPLE_H_INCLUDED

/** Button name. */
#define BUTTON_STRING            "USR_LEFT"

/** UART Hw ID used by the console (UART0). */
#define CONSOLE_UART_ID          ID_UART0

/** UART baudrate. */
#define BOARD_UART_BAUDRATE      115200

/** Push button pin definition. */
#define PUSH_BUTTON_PIO          PIOA
#define PUSH_BUTTON_ID           ID_PIOA
#define PUSH_BUTTON_PIN_MSK      (1 << 15)
#define PUSH_BUTTON_ATTR         (PIO_PULLUP | PIO_DEBOUNCE | PIO_IT_RISE_EDGE)

/** The erase pin mask value in the PIO mode and erase mode. */
#define PIN_PIO_MODE_MSK         CCFG_SYSIO_SYSIO12
#define PIN_ERASE_MODE_MSK       0

/** Test page start address. */
#define TEST_PAGE_ADDRESS        (IFLASH_ADDR + IFLASH_SIZE - IFLASH_PAGE_SIZE)

#endif /* CONF_PIO_EXAMPLE_H_INCLUDED */
