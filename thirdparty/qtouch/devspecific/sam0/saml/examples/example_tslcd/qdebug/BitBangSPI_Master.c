/* This source file is part of the ATMEL QTouch Library 5.0.5 */

/*****************************************************************************
 *
 * \file
 *
 * \brief  This file contains the QDebug bitbang API that can be used to
 * transfer data from a Touch Device to QTouch Studio using the QT600
 * USB Bridge.
 *
 *
 * - Userguide:          QTouch Library Peripheral Touch Controller User Guide.
 * - Support email:      www.atmel.com/design-support/
 *
 *
 * Copyright (c) 2015-2018 Microchip Technology Inc. and its subsidiaries.
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
 ******************************************************************************/

#include "asf.h"

#include "BitBangSPI_Master.h"
#include "QDebugSettings.h"
#include "QDebugTransport.h"

/* ! compile file only when QDebug is enabled. */
#if DEF_TOUCH_QDEBUG_ENABLE == 1

#if (defined(_SAMD11_))
#ifndef PIN_PA27
#define PIN_PA27                          27
#define PORT_PA27                 (1u << 27)
#endif
#endif

#ifndef PORTA
#define PORTA (&(*((volatile Gpio *)GPIO)).GPIO_PORT[0])
#endif
#ifndef PORTB
#define PORTB (&(*((volatile Gpio *)GPIO)).GPIO_PORT[1])
#endif
#ifndef PORTC
#define PORTC (&(*((volatile Gpio *)GPIO)).GPIO_PORT[2])
#endif

/*============================ IMPLEMENTATION ================================*/
static uint8_t BitBangSPI_Send_Byte(uint8_t c);

/*============================================================================
 *  Name    :   BitBangSPI_Master_Init
 *
 *
 *------------------------------------------------------------------------------
 *  Purpose :   Initialize BitBangSPI Interface
 *  Input   :   n/a
 *  Output  :   n/a
 *  Notes   :	Called from QDebug_Init in QDebug.c
 *
 *
 *============================================================================*/
void BitBangSPI_Master_Init(void)
{
	struct port_config config_port_pin;

	port_get_config_defaults(&config_port_pin);

	config_port_pin.direction  = PORT_PIN_DIR_OUTPUT;

	port_pin_set_config(CONCAT(PIN_P, SPI_BB_SS, SS_BB), &config_port_pin);
	port_pin_set_config(CONCAT(PIN_P, SPI_BB_MOSI,
			MOSI_BB), &config_port_pin);
	port_pin_set_config(CONCAT(PIN_P, SPI_BB_SCK,
			SCK_BB), &config_port_pin);

	config_port_pin.direction  = PORT_PIN_DIR_INPUT;
	config_port_pin.input_pull = PORT_PIN_PULL_UP;
	port_pin_set_config(CONCAT(PIN_P, SPI_BB_MISO,
			MISO_BB), &config_port_pin);

	port_pin_set_output_level(CONCAT(PIN_P, SPI_BB_SS, SS_BB), 0u);
	port_pin_set_output_level(CONCAT(PIN_P, SPI_BB_MISO, MISO_BB), 1u);
	port_pin_set_output_level(CONCAT(PIN_P, SPI_BB_MOSI, MOSI_BB), 0u);
	port_pin_set_output_level(CONCAT(PIN_P, SPI_BB_SCK, SCK_BB), 0u);
}

/*============================================================================
 *  Name    :   BitBangSPI_Send_Byte
 *
 *
 *------------------------------------------------------------------------------
 *  Purpose :   Send and Read one byte using BitBangSPI Interface
 *  Input   :   Data to send to slave
 *  Output  :   Data read from slave
 *  Notes   :	Called from BitBangSPI_Send_Message in this file
 *
 *
 *============================================================================*/
uint8_t BitBangSPI_Send_Byte(uint8_t c)
{
	uint32_t bit;
	bool i;
	for (bit = 0u; bit < 8u; bit++) {
		/* write MOSI on trailing edge of previous clock */
		if (c & 0x80u) {
			port_pin_set_output_level(CONCAT(PIN_P, SPI_BB_MOSI,
					MOSI_BB), 1u);
		} else {
			port_pin_set_output_level(CONCAT(PIN_P, SPI_BB_MOSI,
					MOSI_BB), 0u);
		}

		c <<= 1u;

		port_pin_set_output_level(CONCAT(PIN_P, SPI_BB_SCK,
				SCK_BB), 1u);

		/* read MISO on trailing edge */
		i
			= (port_pin_get_input_level(CONCAT(PIN_P, SPI_BB_MISO,
				MISO_BB)));
		if (i == true) {
			c |= (0x01u);
		} else {
		}

		port_pin_set_output_level(CONCAT(PIN_P, SPI_BB_SCK,
				SCK_BB), 0u);
	}

	port_pin_set_output_level(CONCAT(PIN_P, SPI_BB_MOSI, MOSI_BB), 0u);


	return c;
}

/*============================================================================
 *  Name    :   BitBangSPI_Send_Message
 *
 *
 *------------------------------------------------------------------------------
 *  Purpose :   Send and Read one frame using BitBangSPI Interface
 *  Input   :   n/a
 *  Output  :   n/a
 *  Notes   :	Called from Send_Message in QDebugTransport.c
 *
 *
 *============================================================================*/
void BitBangSPI_Send_Message(void)
{
	unsigned int i;
	uint8_t FrameInProgress = 0;

	/* Send our message upstream */
	for (i = 0; i <= TX_index; i++) {
		FrameInProgress = RxHandler(BitBangSPI_Send_Byte(TX_Buffer[i]));
	}

	/* Do we need to receive even more bytes? */
	while (FrameInProgress) {
		FrameInProgress = RxHandler(BitBangSPI_Send_Byte(0));
	}
}

#endif   /* #if DEF_TOUCH_QDEBUG_ENABLE == 1 */
