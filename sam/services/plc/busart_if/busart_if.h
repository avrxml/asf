/**
 * \file
 *
 * \brief PLC USART Buffered Interface
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

#ifndef BUSART_IF_H_INCLUDED
#define BUSART_IF_H_INCLUDED

#include "compiler.h"
#include "conf_busart_if.h"
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
 * \defgroup busart_plc_group PLC Buffered PDC USART
 *
 * This module provides configuration and utils for PLC Buffered USART service
 * interface.
 *
 * @{
 */

/* \name Frequency to poll internal usart buffer (Hz) */
/* @{ */
#define FREQ_TIMER_POLL_USART     100
/* @} */

/* \name Manage interruption priorities */

/* \note In case of use of FreeRTOS, GROUP_PRIO is same value than
 * configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY */
/* @{ */
#define TIMER_USART_PRIO         10
#define USART0_PRIO              10
#define USART1_PRIO              10
/* @} */

/* \name USART definitions */
/* @{ */
#define SERIAL_USART_0           0
#define SERIAL_USART_1           1
/* @} */

#ifdef CONF_BOARD_USART0_RXD
extern uint8_t *const usartRxBuf0;
extern uint8_t *const usartTxBuf0;
#endif

#ifdef CONF_BOARD_USART1_RXD
extern uint8_t *const usartRxBuf1;
extern uint8_t *const usartTxBuf1;
#endif

/* \name PLC USART Buffered Interface */
/* @{ */
int8_t busart_if_open(uint8_t chn, uint32_t bauds);
int8_t busart_if_close(uint8_t chn);
uint16_t busart_if_read(uint8_t chn, void *buffer, uint16_t len);
uint16_t busart_if_write(uint8_t chn, const void *buffer, uint16_t len);
int busart_if_rx_char(uint8_t chn);
uint16_t busart_if_tx_char(uint8_t chn, char data);

/* @} */
/* @} */

/* @cond 0 */
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/* @endcond */

#endif /* BUSART_IF_H_INCLUDED */
