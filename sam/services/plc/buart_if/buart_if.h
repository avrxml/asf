/**
 * \file
 *
 * \brief PLC UART Buffered Interface
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

#ifndef BUART_IF_H_INCLUDED
#define BUART_IF_H_INCLUDED

#include "compiler.h"
#include "conf_buart_if.h"
#include "conf_board.h"

/* @cond 0 */
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/* @endcond */

/**
 * \ingroup plc_group
 * \defgroup buart_plc_group PLC Buffered PDC UART
 *
 * This module provides configuration and utils for PLC Buffered UART service
 * interface.
 *
 * @{
 */

/* \name Frequency to poll internal uart buffer (Hz) */
/* @{ */
#define FREQ_TIMER_POLL_UART     100
/* @} */

/* \name Manage interruption priorities */

/* \note In case of use of FreeRTOS, GROUP_PRIO is same value than
 * configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY */
/* @{ */
#define TIMER_UART_PRIO         10
#define UART0_PRIO              10
#define UART1_PRIO              10
/* @} */

/* \name UART definitions */
/* @{ */
#define SERIAL_UART_0           0
#define SERIAL_UART_1           1
/* @} */

#ifdef CONF_BOARD_UART0
extern uint8_t *const uartRxBuf0;
extern uint8_t *const uartTxBuf0;
#endif

#ifdef CONF_BOARD_UART1
extern uint8_t *const uartRxBuf1;
extern uint8_t *const uartTxBuf1;
#endif

/* \name PLC UART Buffered Interface */
/* @{ */
int8_t buart_if_open(uint8_t chn, uint32_t bauds);
int8_t buart_if_close(uint8_t chn);
uint16_t buart_if_read(uint8_t chn, void *buffer, uint16_t len);
uint16_t buart_if_write(uint8_t chn, const void *buffer, uint16_t len);
int buart_if_rx_char(uint8_t chn);
uint16_t buart_if_tx_char(uint8_t chn, char data);

/* @} */
/* @} */

/* @cond 0 */
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/* @endcond */

#endif /* BUART_IF_H_INCLUDED */
