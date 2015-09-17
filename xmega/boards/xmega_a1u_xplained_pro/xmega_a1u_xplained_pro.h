/**
 * \file
 *
 * \brief XMEGA-A1U Xplained Pro board definition
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

#ifndef XMEGA_A1U_XPLAINED_PRO_H_INCLUDED
#define XMEGA_A1U_XPLAINED_PRO_H_INCLUDED

#include <conf_board.h>
#include <compiler.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \ingroup group_common_boards
 * \defgroup xmega_a1u_xplained_pro_group XMEGA-A1U Xplained Pro board
 *
 * @{
 */

void board_init(void);

/**
 * \defgroup xmega_a1u_xplained_pro_group Features
 *
 * Symbols that describe features and capabilities of the board.
 *
 * @{
 */

/** Name string macro */
#define BOARD_NAME                "XMEGA_A1U_XPLAINED_PRO"

/** \name LED0 definitions
 *  @{ */
#define LED0_PIN                  IOPORT_CREATE_PIN(PORTQ, 3)
#define LED0_ACTIVE               false
#define LED0_INACTIVE             !LED0_ACTIVE
/** @} */

/** \name SW0 definitions
 *  @{ */
#define SW0_PIN                   IOPORT_CREATE_PIN(PORTQ, 2)
#define SW0_ACTIVE                false
#define SW0_INACTIVE              !SW0_ACTIVE
/** @} */

/** \name external osc definitions
 *  @{ */
#define BOARD_XOSC_HZ                   32768
#define BOARD_XOSC_TYPE                 XOSC_TYPE_32KHZ
#define BOARD_XOSC_STARTUP_US           1000000
/** @} */

/**
 * \name LED #0 definitions
 *
 * Wrapper macros for LED0, to ensure common naming across all Xplained Pro
 * boards.
 *
 *  @{ */
#define LED_0_NAME                "LED0 (yellow)"
#define LED_0_PIN                 LED0_PIN
#define LED_0_ACTIVE              LED0_ACTIVE
#define LED_0_INACTIVE            LED0_INACTIVE
#define LED0_GPIO 				  LED0_PIN
/** @} */

/** Number of on-board LEDs */
#define LED_COUNT                 1

/**
 * \name Button #0 definitions
 *
 * Wrapper macros for SW0, to ensure common naming across all Xplained Pro
 * boards.
 *
 *  @{ */
#define BUTTON_0_NAME             "SW0"
#define BUTTON_0_PIN              SW0_PIN
#define BUTTON_0_ACTIVE           SW0_ACTIVE
#define BUTTON_0_INACTIVE         SW0_INACTIVE
/** @} */

/** Number of on-board buttons */
#define BUTTON_COUNT 1

/** \name Extension header #1 pin definitions
 *  @{
 */
#define EXT1_PIN_3                IOPORT_CREATE_PIN(PORTA,0)
#define EXT1_PIN_4                IOPORT_CREATE_PIN(PORTA,4)
#define EXT1_PIN_5                IOPORT_CREATE_PIN(PORTE,6)
#define EXT1_PIN_6                IOPORT_CREATE_PIN(PORTE,7)
#define EXT1_PIN_7                IOPORT_CREATE_PIN(PORTE,1)
#define EXT1_PIN_8                IOPORT_CREATE_PIN(PORTE,0)
#define EXT1_PIN_9                IOPORT_CREATE_PIN(PORTR,0)
#define EXT1_PIN_10               IOPORT_CREATE_PIN(PORTR,1)
#define EXT1_PIN_11               IOPORT_CREATE_PIN(PORTC,0)
#define EXT1_PIN_12               IOPORT_CREATE_PIN(PORTC,1)
#define EXT1_PIN_13               IOPORT_CREATE_PIN(PORTC,2)
#define EXT1_PIN_14               IOPORT_CREATE_PIN(PORTC,3)
#define EXT1_PIN_15               IOPORT_CREATE_PIN(PORTC,4)
#define EXT1_PIN_16               IOPORT_CREATE_PIN(PORTC,5)
#define EXT1_PIN_17               IOPORT_CREATE_PIN(PORTC,6)
#define EXT1_PIN_18               IOPORT_CREATE_PIN(PORTC,7)
/** @} */

/** \name Extension header #1 pin definitions by function
 *  @{
 */
#define EXT1_PIN_ADC_0            EXT1_PIN_3
#define EXT1_PIN_ADC_1            EXT1_PIN_4
#define EXT1_PIN_GPIO_0           EXT1_PIN_5
#define EXT1_PIN_GPIO_1           EXT1_PIN_6
#define EXT1_PIN_PWM_0            EXT1_PIN_7
#define EXT1_PIN_PWM_1            EXT1_PIN_8
#define EXT1_PIN_IRQ              EXT1_PIN_9
#define EXT1_PIN_I2C_SDA          EXT1_PIN_11
#define EXT1_PIN_I2C_SCL          EXT1_PIN_12
#define EXT1_PIN_UART_RX          EXT1_PIN_13
#define EXT1_PIN_UART_TX          EXT1_PIN_14
#define EXT1_PIN_SPI_SS_1         EXT1_PIN_10
#define EXT1_PIN_SPI_SS_0         EXT1_PIN_15
#define EXT1_PIN_SPI_MOSI         EXT1_PIN_16
#define EXT1_PIN_SPI_MISO         EXT1_PIN_17
#define EXT1_PIN_SPI_SCK          EXT1_PIN_18
/** @} */

/** \name Extension header #1 ADC definitions
 *  @{
 */
#define EXT1_ADC_MODULE           ADCA
#define EXT1_ADC_0_CHANNEL        4
#define EXT1_ADC_1_CHANNEL        0
/** @} */

/** \name Extension header #1 PWM definitions
 *  @{
 */
#define EXT1_PWM_MODULE           TCE0
/** @} */

 /** \name Extension header #1 SPI definitions
 *  @{
 */
#define EXT1_SPI_MODULE              &SPIC
 	
/** @} */

/** \name Extension header #1 TWI definitions
 *  @{
 */
#define EXT1_TWI_MODULE              TWIC
/** @} */

/** \name Extension header #1 USART definitions
*  @{
*/
#define EXT1_USART_MODULE              &USARTC0
/** @} */


/** \name Extension header #2 pin definitions
 *  @{
 */
#define EXT2_PIN_3                IOPORT_CREATE_PIN(PORTA,1)
#define EXT2_PIN_4                IOPORT_CREATE_PIN(PORTA,6)
#define EXT2_PIN_5                IOPORT_CREATE_PIN(PORTB,4)
#define EXT2_PIN_6                IOPORT_CREATE_PIN(PORTB,5)
#define EXT2_PIN_7                IOPORT_CREATE_PIN(PORTE,5)
#define EXT2_PIN_8                IOPORT_CREATE_PIN(PORTE,4)
#define EXT2_PIN_9                IOPORT_CREATE_PIN(PORTB,6)
#define EXT2_PIN_10               IOPORT_CREATE_PIN(PORTB,7)
#define EXT2_PIN_11               IOPORT_CREATE_PIN(PORTF,0)
#define EXT2_PIN_12               IOPORT_CREATE_PIN(PORTF,1)
#define EXT2_PIN_13               IOPORT_CREATE_PIN(PORTF,2)
#define EXT2_PIN_14               IOPORT_CREATE_PIN(PORTF,3)
#define EXT2_PIN_15               IOPORT_CREATE_PIN(PORTF,4)
#define EXT2_PIN_16               IOPORT_CREATE_PIN(PORTC,5)
#define EXT2_PIN_17               IOPORT_CREATE_PIN(PORTC,6)
#define EXT2_PIN_18               IOPORT_CREATE_PIN(PORTC,7)
/** @} */

/** \name Extension header #2 pin definitions by function
 *  @{
 */
#define EXT2_PIN_ADC_0            EXT2_PIN_3
#define EXT2_PIN_ADC_1            EXT2_PIN_4
#define EXT2_PIN_GPIO_0           EXT2_PIN_5
#define EXT2_PIN_GPIO_1           EXT2_PIN_6
#define EXT2_PIN_PWM_0            EXT2_PIN_7
#define EXT2_PIN_PWM_1            EXT2_PIN_8
#define EXT2_PIN_IRQ              EXT2_PIN_9
#define EXT2_PIN_I2C_SDA          EXT2_PIN_11
#define EXT2_PIN_I2C_SCL          EXT2_PIN_12
#define EXT2_PIN_UART_RX          EXT2_PIN_13
#define EXT2_PIN_UART_TX          EXT2_PIN_14
#define EXT2_PIN_SPI_SS_1         EXT2_PIN_10
#define EXT2_PIN_SPI_SS_0         EXT2_PIN_15
#define EXT2_PIN_SPI_MOSI         EXT2_PIN_16
#define EXT2_PIN_SPI_MISO         EXT2_PIN_17
#define EXT2_PIN_SPI_SCK          EXT2_PIN_18
/** @} */

/** \name Extension header #2 ADC definitions
 *  @{
 */
#define EXT2_ADC_MODULE           ADCA
#define EXT2_ADC_0_CHANNEL        6
#define EXT2_ADC_1_CHANNEL        1
/** @} */

/** \name Extension header #2 PWM definitions
 *  @{
 */
#define EXT2_PWM_MODULE           TCE1
/** @} */


/** \name Extension header #2 SPI definitions
 *  @{
 */
#define EXT2_SPI_MODULE              &SPIC
 	
/** @} */

/** \name Extension header #2 TWI definitions
 *  @{
 */
#define EXT2_TWI_MODULE              TWIF
/** @} */

 /** \name Extension header #2 USART definitions
 *  @{
 */
#define EXT2_USART_MODULE              &USARTF0
/** @} */

/** \name Extension header #3 pin definitions
 *  @{
 */
#define EXT3_PIN_3                IOPORT_CREATE_PIN(PORTA,3)
#define EXT3_PIN_4                IOPORT_CREATE_PIN(PORTA,7)
#define EXT3_PIN_5                IOPORT_CREATE_PIN(PORTK,0)
#define EXT3_PIN_6                IOPORT_CREATE_PIN(PORTK,1)
#define EXT3_PIN_7                IOPORT_CREATE_PIN(PORTD,5)
#define EXT3_PIN_8                IOPORT_CREATE_PIN(PORTD,4)
#define EXT3_PIN_9                IOPORT_CREATE_PIN(PORTK,2)
#define EXT3_PIN_10               IOPORT_CREATE_PIN(PORTK,3)
#define EXT3_PIN_11               IOPORT_CREATE_PIN(PORTF,0)
#define EXT3_PIN_12               IOPORT_CREATE_PIN(PORTF,1)
#define EXT3_PIN_13               IOPORT_CREATE_PIN(PORTF,6)
#define EXT3_PIN_14               IOPORT_CREATE_PIN(PORTF,7)
#define EXT3_PIN_15               IOPORT_CREATE_PIN(PORTD,0)
#define EXT3_PIN_16               IOPORT_CREATE_PIN(PORTC,5)
#define EXT3_PIN_17               IOPORT_CREATE_PIN(PORTC,6)
#define EXT3_PIN_18               IOPORT_CREATE_PIN(PORTC,7)
/** @} */

/** \name Extension header #3 pin definitions by function
 *  @{
 */
#define EXT3_PIN_ADC_0            EXT3_PIN_3
#define EXT3_PIN_ADC_1            EXT3_PIN_4
#define EXT3_PIN_GPIO_0           EXT3_PIN_5
#define EXT3_PIN_GPIO_1           EXT3_PIN_6
#define EXT3_PIN_PWM_0            EXT3_PIN_7
#define EXT3_PIN_PWM_1            EXT3_PIN_8
#define EXT3_PIN_IRQ              EXT3_PIN_9
#define EXT3_PIN_I2C_SDA          EXT3_PIN_11
#define EXT3_PIN_I2C_SCL          EXT3_PIN_12
#define EXT3_PIN_UART_RX          EXT3_PIN_13
#define EXT3_PIN_UART_TX          EXT3_PIN_14
#define EXT3_PIN_SPI_SS_1         EXT3_PIN_10
#define EXT3_PIN_SPI_SS_0         EXT3_PIN_15
#define EXT3_PIN_SPI_MOSI         EXT3_PIN_16
#define EXT3_PIN_SPI_MISO         EXT3_PIN_17
#define EXT3_PIN_SPI_SCK          EXT3_PIN_18
/** @} */

/** \name Extension header #3 ADC definitions
 *  @{
 */
#define EXT3_ADC_MODULE           ADCA
#define EXT3_ADC_0_CHANNEL        7
#define EXT3_ADC_1_CHANNEL        3
/** @} */

/** \name Extension header #3 PWM definitions
 *  @{
 */
#define EXT3_PWM_MODULE           TCD1
/** @} */


/** \name Extension header #3 SPI definitions
 *  @{
 */
#define EXT3_SPI_MODULE              &SPIC

/** @} */

/** \name Extension header #3 TWI definitions
 *  @{
 */
#define EXT3_TWI_MODULE              TWIF
/** @} */

/** \name Extension header #3 USART definitions
 *  @{
 */
#define EXT3_USART_MODULE              &USARTF1
/** @} */

/** \name Embedded debugger GPIO interface definitions
 * @{
 */
#define EDBG_GPIO0_PIN            IOPORT_CREATE_PIN(PORTK,4)
#define EDBG_GPIO1_PIN            IOPORT_CREATE_PIN(PORTK,5)
#define EDBG_GPIO2_PIN            IOPORT_CREATE_PIN(PORTK,6)
#define EDBG_GPIO3_PIN            IOPORT_CREATE_PIN(PORTK,7)
/** @} */

/** \name Embedded debugger USART interface definitions
 * @{
 */
#define EDBG_UART_MODULE          &USARTE0
/** @} */

/** \name Embedded debugger TWI interface definitions
 * @{
 */
#define EDBG_TWI_MODULE              TWIC
/** @} */

/** \name EBI_DATA_ADDR definitions
 * @{
 */
#define EBI_DATA_ADDR_D7_A7_A15		    IOPORT_CREATE_PIN(PORTJ,7) 
#define EBI_DATA_ADDR_D6_A6_A14		    IOPORT_CREATE_PIN(PORTJ,6) 
#define EBI_DATA_ADDR_D5_A5_A13	  		IOPORT_CREATE_PIN(PORTJ,5) 
#define EBI_DATA_ADDR_D4_A4_A12	  		IOPORT_CREATE_PIN(PORTJ,4) 
#define EBI_DATA_ADDR_D3_A3_A11	  		IOPORT_CREATE_PIN(PORTJ,3) 
#define EBI_DATA_ADDR_D2_A2_A10	 	    IOPORT_CREATE_PIN(PORTJ,2)
#define EBI_DATA_ADDR_D1_A1_A9	  		IOPORT_CREATE_PIN(PORTJ,1)
#define EBI_DATA_ADDR_D0_A0_A8	  		IOPORT_CREATE_PIN(PORTJ,0)
/** @} */ 

/** \name EBI_CTRL_ADDR definitions
 * @{
 */
#define EBI_CTRL_ADDR_ALE1			IOPORT_CREATE_PIN(PORTH,2)
#define EBI_CTRL_ADDR_ALE2			IOPORT_CREATE_PIN(PORTH,3)
#define EBI_CTRL_ADDR_WE 			IOPORT_CREATE_PIN(PORTH,0)
#define EBI_CTRL_ADDR_RE 			IOPORT_CREATE_PIN(PORTH,1)
#define EBI_CTRL_ADDR_CS 			IOPORT_CREATE_PIN(PORTH,6)
#define EBI_CTRL_ADDR_A18			IOPORT_CREATE_PIN(PORTK,7)
#define EBI_CTRL_ADDR_A17 			IOPORT_CREATE_PIN(PORTH,5)
#define EBI_CTRL_ADDR_A16	    	IOPORT_CREATE_PIN(PORTH,4)
/** @} */ 

/** \name LCD_CTRL definitions
 * @{
 */
#define LCD_BACKLIGHT_PWM		  IOPORT_CREATE_PIN(PORTF,5) 
#define LCD_DISPLAY_CS			  IOPORT_CREATE_PIN(PORTH,7) 
#define LCD_DISPLAY_ENABLE		  IOPORT_CREATE_PIN(PORTK,5) 
#define LCD_DISPLAY_RESET  		  IOPORT_CREATE_PIN(PORTA,5) 
#define LCD_DISPLAY_WE			  IOPORT_CREATE_PIN(PORTH,0) 
#define LCD_DISPLAY_RE		 	  IOPORT_CREATE_PIN(PORTH,1) 
#define LCD_TWI_SDA				  IOPORT_CREATE_PIN(PORTC,0) 
#define LCD_TWI_SCL				  IOPORT_CREATE_PIN(PORTC,1) 
#define LCD_IRQ1				  IOPORT_CREATE_PIN(PORTK,4) 
#define LCD_IRQ2				  IOPORT_CREATE_PIN(PORTK,6) 
#define LCD_SPI_MOSI 			  IOPORT_CREATE_PIN(PORTC,5) 
#define LCD_SPI_MISO 			  IOPORT_CREATE_PIN(PORTC,6) 
#define LCD_SPI_SCK 			  IOPORT_CREATE_PIN(PORTC,7) 
#define LCD_SPI_CS 				  IOPORT_CREATE_PIN(PORTB,1) 
/** @} */ 

/** \name AT86RFX definitions
 * @{
 */
#define AT86RFX_SPI                  EXT1_SPI_MODULE
#define AT86RFX_RST_PIN              EXT1_PIN_7
#define AT86RFX_MISC_PIN             EXT1_PIN_12
#define AT86RFX_IRQ_PIN              EXT1_PIN_9
#define AT86RFX_SLP_PIN              EXT1_PIN_10
#define AT86RFX_SPI_CS               EXT1_PIN_15
#define AT86RFX_SPI_MOSI             EXT1_PIN_16
#define AT86RFX_SPI_MISO             EXT1_PIN_17
#define AT86RFX_SPI_SCK              EXT1_PIN_18
#define AT86RFX_CSD                  EXT1_PIN_5
#define AT86RFX_CPS                  EXT1_PIN_8
/** @} */ 

/*! \name SPI Connections of external AT45DBX Data Flash Memory
 *
 * This is mainly used for proper board compilation
 *
 */

//! @{
#define AT45DBX_SPI           &SPIC
#define AT45DBX_CS            EXT1_PIN_GPIO_0
#define AT45DBX_MASTER_SCK    EXT1_PIN_SPI_SCK // SCK as output
#define AT45DBX_MASTER_SS     EXT1_PIN_SPI_SS_1  // SS as output
#define AT45DBX_MASTER_MOSI   EXT1_PIN_SPI_MOSI // MOSI as output
#define AT45DBX_MASTER_MISO   EXT1_PIN_SPI_MISO // MISO as input
#define AT45DBX_SPI_CLK_MASK  SYSCLK_PORT_C
//! @}

#ifdef __cplusplus
}
#endif

#endif  /* XMEGA_A1U_XPLAINED_PRO_H_INCLUDED */
