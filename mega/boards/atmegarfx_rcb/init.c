/**
 * \file
 *
 * \brief ATMEGA256RFR2 Xplained Pro board init.
 *
 * To use this board, define BOARD=ATMEGA256RFR2_XPLAINED_PRO.
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
 *
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
	ioport_set_port_level(IOPORT_PORTE,KEY_RC_IO_MASK,IOPORT_PIN_LEVEL_HIGH);
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
	ioport_set_port_level(IOPORT_PORTD,BB_SIO_MASK,IOPORT_PIN_LEVEL_HIGH);
#endif
#endif

}


