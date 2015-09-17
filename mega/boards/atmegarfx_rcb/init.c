/**
 * \file
 *
 * \brief ATMEGA256RFR2 Xplained Pro board init.
 *
 * To use this board, define BOARD=ATMEGA256RFR2_XPLAINED_PRO.
 *
 * Copyright (c) 2013-2015 Atmel Corporation. All rights reserved.
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
#include <board.h>
#include <compiler.h>
#include <conf_board.h>
#include "gpio.h"
#include "led.h"
#include "helper.h"

void board_init(void)
{
#ifdef KEY_RC_BOARD
    /* On board Button initialization */
	ioport_configure_pin(BUTTON_IRQ_PIN_1,IOPORT_DIR_INPUT | IOPORT_PULL_UP);
	ioport_configure_pin(BUTTON_IRQ_PIN_2,IOPORT_DIR_INPUT | IOPORT_PULL_UP);
	ioport_configure_pin(BUTTON_IRQ_PIN_3,IOPORT_DIR_INPUT | IOPORT_PULL_UP);
	
    set_button_pins_for_normal_mode();

    /* Initialize the IRQ lines' interrupt behaviour. */
    DISABLE_ALL_BUTTON_IRQS();
		
	/* LED Init */
	/* LCD initialization for inactive use */    
    /* On board LED initialization */
	ioport_configure_pin(LCD_CS_ON_BOARD,	
	IOPORT_DIR_OUTPUT |  IOPORT_INIT_HIGH);
	
	ioport_set_port_dir(IOPORT_PORTE,KEY_RC_IO_MASK,IOPORT_DIR_OUTPUT);
	ioport_set_port_level(IOPORT_PORTE,KEY_RC_IO_MASK,KEY_RC_IO_MASK);
	ioport_set_pin_dir(IOPORT_CREATE_PIN(PORTG , 2),IOPORT_DIR_INPUT);
	ioport_set_pin_mode(IOPORT_CREATE_PIN(PORTG , 2), IOPORT_MODE_PULLUP);	

    LATCH_INIT();   
	/* Init ADC for the Accelerometer */
	 adc_init();

	// LATCH_INIT();
	 /* Enable Accelerometer by enabling the PWR pin in the Latch */
	 acc_init();
 
	update_latch_status();   

    /* Apply latch pulse to set LED status */
    pulse_latch();
	
#else
    /* To identify if it is a plain or STB*/
    board_identify();

	/* On board LED initialization */
	ioport_configure_pin(LED0_RCB,IOPORT_DIR_OUTPUT |  IOPORT_INIT_HIGH);
	ioport_configure_pin(LED1_RCB,IOPORT_DIR_OUTPUT |  IOPORT_INIT_HIGH);

	ioport_configure_pin(LED2_RCB,IOPORT_DIR_OUTPUT |  IOPORT_INIT_HIGH);

	/* On board Switch initialization */
	ioport_configure_pin(GPIO_PUSH_BUTTON_0,IOPORT_DIR_INPUT | IOPORT_PULL_UP);      

#ifdef BREAKOUT_BOARD
  //Enable RCB_BB RS232 level converter
	ioport_set_port_dir(IOPORT_PORTD,BB_SIO_MASK,IOPORT_DIR_OUTPUT);
	ioport_set_port_level(IOPORT_PORTD,BB_SIO_MASK,BB_SIO_VAL);
#endif
#endif

}


