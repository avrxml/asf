/**
 * \file
 *
 * \brief STK600 with MEGA routing board init.
 *
 * To use this board, define BOARD=STK600_MEGA.
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
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */
#include "board.h"
#include "compiler.h"
#include "stk600.h"
#include "conf_board.h"
#include "gpio.h"

/*
 * void board_init(void)
 * parameter : void
 * return : void
 * Description: to initialize the pins for input and output.
 */
void board_init(void)
{
	/*On Stk-600 LED configuration*/
	gpio_configure_pin(LED0_GPIO, IOPORT_DIR_OUTPUT |  IOPORT_INIT_HIGH);
	gpio_configure_pin(LED1_GPIO, IOPORT_DIR_OUTPUT |  IOPORT_INIT_HIGH);
	gpio_configure_pin(LED2_GPIO, IOPORT_DIR_OUTPUT |  IOPORT_INIT_HIGH);
	gpio_configure_pin(LED3_GPIO, IOPORT_DIR_OUTPUT |  IOPORT_INIT_HIGH);
	gpio_configure_pin(LED4_GPIO, IOPORT_DIR_OUTPUT |  IOPORT_INIT_HIGH);
	gpio_configure_pin(LED5_GPIO, IOPORT_DIR_OUTPUT |  IOPORT_INIT_HIGH);
	gpio_configure_pin(LED6_GPIO, IOPORT_DIR_OUTPUT |  IOPORT_INIT_HIGH);
	gpio_configure_pin(LED7_GPIO, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);

	/*On Stk-600 switch configuration*/
	gpio_configure_pin(GPIO_PUSH_BUTTON_0, IOPORT_DIR_INPUT |
			IOPORT_PULL_UP);
	gpio_configure_pin(GPIO_PUSH_BUTTON_1, IOPORT_DIR_INPUT |
			IOPORT_PULL_UP);
	gpio_configure_pin(GPIO_PUSH_BUTTON_2, IOPORT_DIR_INPUT |
			IOPORT_PULL_UP);
	gpio_configure_pin(GPIO_PUSH_BUTTON_3, IOPORT_DIR_INPUT |
			IOPORT_PULL_UP);
	gpio_configure_pin(GPIO_PUSH_BUTTON_4, IOPORT_DIR_INPUT |
			IOPORT_PULL_UP);
	gpio_configure_pin(GPIO_PUSH_BUTTON_5, IOPORT_DIR_INPUT |
			IOPORT_PULL_UP);
	gpio_configure_pin(GPIO_PUSH_BUTTON_6, IOPORT_DIR_INPUT |
			IOPORT_PULL_UP);
	gpio_configure_pin(GPIO_PUSH_BUTTON_7, IOPORT_DIR_INPUT |
			IOPORT_PULL_UP);			

#if MEGA_RF
	/*RED, GREEN and YELLOW Led configuration*/
	gpio_configure_pin(LED_RED_GPIO, IOPORT_DIR_OUTPUT |  IOPORT_INIT_HIGH);
	gpio_configure_pin(LED_GREEN_GPIO, IOPORT_DIR_OUTPUT |
			IOPORT_INIT_HIGH);
	gpio_configure_pin(LED_YELLOW_GPIO,
			IOPORT_DIR_OUTPUT |  IOPORT_INIT_HIGH);

	/*On ATmega128RFA1-EVK switch configuration*/

	gpio_configure_pin(GPIO_PUSH_BUTTON_ON_BOARD,
			IOPORT_DIR_INPUT | IOPORT_PULL_UP);
			
#ifdef CONF_BOARD_AT45DBX
	gpio_configure_pin(AT45DBX_MASTER_SCK, IOPORT_DIR_OUTPUT |
		IOPORT_INIT_HIGH);
	gpio_configure_pin(AT45DBX_MASTER_MOSI, IOPORT_DIR_OUTPUT |
		IOPORT_INIT_HIGH);
	gpio_configure_pin(AT45DBX_MASTER_MISO, IOPORT_DIR_INPUT);
	gpio_configure_pin(AT45DBX_CS, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
#endif	
		
#endif
}
