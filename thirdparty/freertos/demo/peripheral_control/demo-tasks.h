/**
 *
 * \file
 *
 * \brief Generic FreeRTOS peripheral control functions
 *
 * Copyright (c) 2012-2018 Microchip Technology Inc. and its subsidiaries.
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
