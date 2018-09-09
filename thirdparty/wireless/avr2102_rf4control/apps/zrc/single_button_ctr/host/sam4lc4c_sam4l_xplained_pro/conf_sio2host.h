/**
 * \file *********************************************************************
 *
 * \brief Serial Input & Output configuration
 *
 * Copyright (c) 2013-2018 Microchip Technology Inc. and its subsidiaries.
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

#ifndef CONF_SIO2HOST_H_INCLUDED
#define CONF_SIO2HOST_H_INCLUDED
 #define SERIAL_RX_BUF_SIZE_HOST    250

/** USART Interface */
#define USART_HOST                 USART1
/** Baudrate setting */
#define USART_HOST_BAUDRATE        115200
/** Character length setting */
#define USART_HOST_CHAR_LENGTH     US_MR_CHRL_8_BIT
/** Parity setting */
#define USART_HOST_PARITY          US_MR_PAR_NO
/** Stop bits setting */
#define USART_HOST_STOP_BITS       US_MR_NBSTOP_1_BIT

#define USART_HOST_ISR_VECT()      ISR(USART1_Handler)

#define USART_HOST_IRQn            USART1_IRQn

#define USART_HOST_RX_ISR_ENABLE() usart_enable_interrupt(USART_HOST, \
		US_IER_RXRDY); \
	NVIC_EnableIRQ(USART_HOST_IRQn);

#endif /* CONF_SIO2HOST_H_INCLUDED */
