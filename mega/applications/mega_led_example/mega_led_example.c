/**
 * \file
 *
 * \brief MEGA LED Example
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
 */

/**
 * \mainpage MEGA LED Example
 * See \ref appdoc_main "here" for project documentation.
 * \copydetails appdoc_preface
 *
 *
 * \page appdoc_preface Overview
 * This application demonstrates a simple example to turn on and off the board LED.
 */

/**
 * \page appdoc_main MEGA LED Example
 *
 * Overview:
 * - \ref appdoc_mega_led_app_intro
 * - \ref appdoc_mega_led_app_usage
 * - \ref appdoc_mega_led_app_compinfo
 * - \ref appdoc_mega_led_app_contactinfo
 *
 * \section appdoc_mega_led_app_intro Introduction
 * This application demonstrates a simple example turn on and off the board LED.
 *
 * This application has been tested on following boards:
 * - ATmega328p Xplained Mini
 * - ATmega328pb Xplained Mini
 * - ATmega168pb Xplained Mini
 * - ATmega324pb Xplained PRO
 *
 * \section appdoc_mega_led_app_usage Usage
 * The application uses button to control the LED, 
 * once the button is pressed, LED0 will turn on
 * once the button is released, LED0 will turn off
 *
 * \section appdoc_mega_led_app_compinfo Compilation Info
 * This software was written for the GNU GCC and IAR for MEGA.
 * Other compilers may or may not work.
 *
 * \section appdoc_mega_led_app_contactinfo Contact Information
 * For further information, visit
 * <a href="http://www.microchip.com">http://www.microchip.com</a>.
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#include <asf.h>
int main (void)
{
	/* set board io port */
	board_init();
	bool button_state;
	while(1){
		button_state = ioport_get_pin_level(GPIO_PUSH_BUTTON_0);
		if(button_state){
			LED_Off(LED0);
		}else{
			LED_On(LED0);
		}
	}	
}
