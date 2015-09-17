/**
 *
 * \file
 *
 * \brief Generic FreeRTOS peripheral control functions
 *
 * Copyright (c) 2012-2015 Atmel Corporation. All rights reserved.
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

#ifndef DEMO_TASKS_H
#define DEMO_TASKS_H

#include "conf_example.h"

void vRegisterCLICommands(void);

#if (defined confINCLUDE_UART_CLI)
#include "uart.h"
void create_uart_cli_task(Uart *uart_base, uint16_t stack_depth_words,
		unsigned portBASE_TYPE task_priority);
void uart_cli_output(const uint8_t *message_string);
#endif

#if (defined confINCLUDE_USART_ECHO_TASKS) || (defined confINCLUDE_USART_CLI)
#include "usart.h"
void create_usart_cli_task(Usart *usart_base, uint16_t stack_depth_words,
		unsigned portBASE_TYPE task_priority);
void usart_cli_output(const uint8_t *message_string);
void create_usart_echo_test_tasks(Usart *pxUsart, uint16_t stack_depth_words,
		unsigned portBASE_TYPE task_priority);
portBASE_TYPE are_usart_echo_tasks_still_running(void);

#endif

#if (defined confINCLUDE_CDC_CLI)
void create_usb_cdc_cli_task(uint16_t stack_depth_words,
		unsigned portBASE_TYPE task_priority);
void cdc_cli_output(const uint8_t const *message_string);

#endif

#if (defined confINCLUDE_SPI_FLASH_TASK)
#include "spi.h"
void create_spi_flash_test_task(Spi *spi_base, uint16_t stack_depth_words,
		unsigned portBASE_TYPE task_priority,
		portBASE_TYPE set_asynchronous_api);
portBASE_TYPE did_spi_flash_test_pass(void);

#endif

#if (defined confINCLUDE_TWI_EEPROM_TASK)
#include "freertos_twi_master.h"
void create_twi_eeprom_test_task(Twi *twi_base, uint16_t stack_depth_words,
		unsigned portBASE_TYPE task_priority,
		portBASE_TYPE set_asynchronous_api);
portBASE_TYPE did_twi_eeprom_test_pass(void);

#endif

#endif /* DEMO_TASKS_H */
