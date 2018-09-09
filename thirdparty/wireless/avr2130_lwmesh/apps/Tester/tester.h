/**
 * \file tester.h
 *
 * \brief Tester application interface
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
