/**
 * \file
 *
 * \brief MEGA LED Example
 *
 * Copyright (C) 2015-2016 Atmel Corporation. All rights reserved.
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
 * <a href="http://www.atmel.com">http://www.atmel.com</a>.
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
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
