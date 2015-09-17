/**
 * \file
 *
 * \brief Low Power Demo for AVR XMEGA
 *
 * Copyright (C) 2012-2015 Atmel Corporation. All rights reserved.
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

#include <asf.h>
#include "conf_low_power_demo.h"

/**
 * \mainpage
 *
 * \section intro Introduction
 * This demo initializes the device for low power consumption, and cycles
 * through all sleep modes when any switch is pressed.
 *
 * \section files Main Files
 * - low_power_demo.c demo application
 * - conf_low_power_demo.h demo configuration
 *
 * \section device_info Device Info
 * ATxmega256A3BU and ATxmega128A1 devices can be used. This example has
 * been tested with the following setup:
 * - XMEGA-A1 Xplained
 * - XMEGA-A3BU Xplained
 *
 * \section description Description of the demo application
 *
 * This demo is a simple program for measuring the current consumption of XMEGA
 * Xplained boards. The demo initializes the device for low power consumption,
 * and cycles through all sleep modes when any switch is pressed. The current
 * consumption of the device can be measured with an ammeter connected between
 * the pins VCC_P3V3 and VCC_MCU_P3V3 on the board after the jumper has been
 * removed.
 *
 * \section dependencies Dependencies
 * The most relevant module dependencies of this example are:
 *   - \ref sleepmgr_group
 *   - \ref clk_group
 *   - \ref ioport_group
 *
 * \section compinfo Compilation info
 * This software was written for the GNU GCC and IAR for AVR. Other compilers
 * may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit <a href="http://www.atmel.com/">Atmel</a>.\n
 * Support and FAQ: http://www.atmel.com/design-support/
 */

static void lowpower_init(void);
static void lowpower_interrupt(void);

/**
 * \brief Initialize low power mode
 *
 * Disconnect all peripherals, enable pull-up on all I/O pins, disable watchdog
 * timer and brown out detection, and JTAG-interface (if configured in
 * \ref conf_low_power_demo.h )
 */
static void lowpower_init(void)
{
	/* Disable unused modules */
	sysclk_disable_peripheral_clock(&AES);
	sysclk_disable_peripheral_clock(&DMA);
	sysclk_disable_peripheral_clock(&EVSYS);
#if AVR8_PART_IS_DEFINED(ATxmega128A1)
	sysclk_disable_peripheral_clock(&RTC);
	sysclk_disable_peripheral_clock(&EBI);
#endif

	/* Disable TWI */
	sysclk_disable_peripheral_clock(&TWIC);
#if AVR8_PART_IS_DEFINED(ATxmega128A1)
	sysclk_disable_peripheral_clock(&TWID);
#endif
	sysclk_disable_peripheral_clock(&TWIE);
#if AVR8_PART_IS_DEFINED(ATxmega128A1)
	sysclk_disable_peripheral_clock(&TWIF);
#endif

	/* Disable SPI */
	sysclk_disable_peripheral_clock(&SPIC);
	sysclk_disable_peripheral_clock(&SPID);
#if AVR8_PART_IS_DEFINED(ATxmega128A1)
	sysclk_disable_peripheral_clock(&SPIF);
#endif

	/* Disable USART */
	sysclk_disable_peripheral_clock(&USARTC0);
	sysclk_disable_peripheral_clock(&USARTC1);
	sysclk_disable_peripheral_clock(&USARTD0);
	sysclk_disable_peripheral_clock(&USARTD1);
	sysclk_disable_peripheral_clock(&USARTE0);
#if AVR8_PART_IS_DEFINED(ATxmega128A1)
	sysclk_disable_peripheral_clock(&USARTE1);
#endif
	sysclk_disable_peripheral_clock(&USARTF0);
#if AVR8_PART_IS_DEFINED(ATxmega128A1)
	sysclk_disable_peripheral_clock(&USARTF1);
#endif

	/* Disable timers/counters */
	sysclk_disable_peripheral_clock(&TCC0);
	sysclk_disable_peripheral_clock(&TCC1);
	sysclk_disable_peripheral_clock(&TCD0);
	sysclk_disable_peripheral_clock(&TCD1);
	sysclk_disable_peripheral_clock(&TCE0);
	sysclk_disable_peripheral_clock(&TCE1);
	sysclk_disable_peripheral_clock(&TCF0);
#if AVR8_PART_IS_DEFINED(ATxmega128A1)
	sysclk_disable_peripheral_clock(&TCF1);
#endif

	/* Disable HIRES */
	sysclk_disable_peripheral_clock(&HIRESC);
	sysclk_disable_peripheral_clock(&HIRESD);
	sysclk_disable_peripheral_clock(&HIRESE);
	sysclk_disable_peripheral_clock(&HIRESF);

	/* Disable analog modules */
	sysclk_disable_peripheral_clock(&ACA);
	sysclk_disable_peripheral_clock(&ADCA);
#if AVR8_PART_IS_DEFINED(ATxmega128A1)
	sysclk_disable_peripheral_clock(&DACA);
#endif
	sysclk_disable_peripheral_clock(&ACB);
	sysclk_disable_peripheral_clock(&ADCB);
	sysclk_disable_peripheral_clock(&DACB);

	/* Enable pull-up on all I/O pins */
	ioport_configure_port_pin(&PORTA, 0xF4,
			IOPORT_DIR_INPUT | IOPORT_PULL_UP);
	ioport_configure_port_pin(&PORTA,
			0x03, IOPORT_DIR_INPUT | IOPORT_INPUT_DISABLE);
	ioport_configure_port_pin(&PORTB, 0x7F,
			IOPORT_DIR_INPUT | IOPORT_PULL_UP);
	ioport_configure_port_pin(&PORTC, 0xFF,
			IOPORT_DIR_INPUT | IOPORT_PULL_UP);
	ioport_configure_port_pin(&PORTD, 0x3F,
			IOPORT_DIR_INPUT | IOPORT_PULL_UP);
	ioport_configure_port_pin(&PORTE, 0x0F,
			IOPORT_DIR_INPUT | IOPORT_PULL_UP);
	ioport_configure_port_pin(&PORTF, 0xFF,
			IOPORT_DIR_INPUT | IOPORT_PULL_UP);
	ioport_configure_port_pin(&PORTR, 0x03,
			IOPORT_DIR_INPUT | IOPORT_PULL_UP);

#if AVR8_PART_IS_DEFINED(ATxmega256A3BU)
	/* Disable display for A3BU Xplained */
	ioport_configure_pin(NHD_C12832A1Z_RESETN, IOPORT_DIR_INPUT);
#endif

#if AVR8_PART_IS_DEFINED(ATxmega128A1)
	ioport_configure_port_pin(&PORTH, 0xFF,
			IOPORT_DIR_INPUT | IOPORT_PULL_UP);
	ioport_configure_port_pin(&PORTJ, 0xFF,
			IOPORT_DIR_INPUT | IOPORT_PULL_UP);
	ioport_configure_port_pin(&PORTK, 0xFF,
			IOPORT_DIR_INPUT | IOPORT_PULL_UP);
	ioport_configure_port_pin(&PORTQ, 0x0F,
			IOPORT_DIR_INPUT | IOPORT_PULL_UP);
#endif

	/* Disable Watchdog timer */
	wdt_disable();

	/* Enable EEPROM and Flash power reduction mode. */
	ccp_write_io((uint8_t *)&NVM.CTRLB, NVM_EPRM_bm | NVM_FPRM_bm);

#ifdef NO_JTAG
	ccp_write_io((uint8_t *)&MCU.MCUCR, MCU_JTAGD_bm);
#endif
}

/**
 * \brief RTC compare ISR
 *
 * This function updates the sleep mode locks so that the device cycles through
 * the different sleep modes.
 */
static void lowpower_interrupt(void)
{
	static uint8_t state = 0;

	switch (state) {
	/* The device starts out in active mode. Go to Idle. */
	default:
	case 0:
		sleepmgr_unlock_mode(SLEEPMGR_ACTIVE);
		sleepmgr_lock_mode(SLEEPMGR_IDLE);
		++state;
		break;

	/* Go to extended Standby */
	case 1:
		sleepmgr_unlock_mode(SLEEPMGR_IDLE);
		sleepmgr_lock_mode(SLEEPMGR_ESTDBY);
		++state;
		break;

	/* Go to power save */
	case 2:
		sleepmgr_unlock_mode(SLEEPMGR_ESTDBY);
		sleepmgr_lock_mode(SLEEPMGR_PSAVE);
		++state;
		break;

	/* Go to standby */
	case 3:
		sleepmgr_unlock_mode(SLEEPMGR_PSAVE);
		sleepmgr_lock_mode(SLEEPMGR_STDBY);
		++state;
		break;

	/* Go to power down */
	case 4:
		sleepmgr_unlock_mode(SLEEPMGR_STDBY);
		sleepmgr_lock_mode(SLEEPMGR_PDOWN);
		++state;
		break;

	/* Go back to idle */
	case 5:
		sleepmgr_unlock_mode(SLEEPMGR_PDOWN);
		sleepmgr_lock_mode(SLEEPMGR_IDLE);
		state = 1;
		break;
	}
}

/**
 * \brief ISR for switches on \ref SWITCHPORT0
 *
 * This ISR calls the \ref lowpower_interrupt function, and handles bouncing
 * from the switches to not generate more than one interrupt per press.
 * It also waits till a switch is no longer pressed before going back to the
 * main routine.
 */
ISR(SWITCH0_INT_VECT)
{
	uint8_t switchport;

	cpu_irq_disable();
	delay_ms(10);
	lowpower_interrupt();

	/* Make sure button is released before continuing */
	switchport = SWITCHPORT0.IN | ~SWITCHMASK0;
	while (switchport != 0xFF) {
		switchport = (SWITCHPORT0.IN | ~SWITCHMASK0);
	}
	delay_ms(10);

	/* Clear interrupt that was generated due to bouncing */
	SWITCHPORT0.INTFLAGS = PORT_INT0IF_bm;
	cpu_irq_enable();
}

/**
 * \brief ISR for switches on \ref SWITCHPORT1
 *
 * This ISR calls the \ref lowpower_interrupt function, and handles bouncing
 * from the switches to not generate more than one interrupt per press.
 * It also waits till a switch is no longer pressed before going back to the
 * main routine.
 */
ISR(SWITCH1_INT_VECT)
{
	uint8_t switchport;
	cpu_irq_disable();
	delay_ms(10);
	lowpower_interrupt();

	/* Make sure button is released before continuing */
	switchport = SWITCHPORT1.IN | ~SWITCHMASK1;
	while (switchport != 0xFF) {
		switchport = (SWITCHPORT1.IN | ~SWITCHMASK1);
	}
	delay_ms(10);

	/* Clear interrupt that was generated due to bouncing */
	SWITCHPORT1.INTFLAGS = PORT_INT0IF_bm;
	cpu_irq_enable();
}

/**
 * \brief Run low power demo
 *
 * This function initializes the XMEGA to the least power consuming state,
 * before initializing the sleep manager interrupts on switchports.
 * The device is then put to sleep, and the sleep mode is configured by the
 * interrupt routines.
 */
int main(void)
{
	board_init();
	sysclk_init();
	lowpower_init();

	/* Initialize the sleep manager. */
	sleepmgr_init();

	/* Enable interrupts from switchports on
	 * low level to sense all interrupts */
	pmic_init();
	SWITCHPORT0.INTCTRL = SWITCHPORT_INT_LEVEL;
	SWITCHPORT1.INTCTRL = SWITCHPORT_INT_LEVEL;
	SWITCHPORT0.INT0MASK = SWITCHMASK0;
	SWITCHPORT1.INT0MASK = SWITCHMASK1;
	ioport_configure_port_pin(&SWITCHPORT0,
			SWITCHMASK0, IOPORT_LEVEL | IOPORT_PULL_UP);
	ioport_configure_port_pin(&SWITCHPORT1,
			SWITCHMASK1, IOPORT_LEVEL | IOPORT_PULL_UP);
	cpu_irq_enable();

	/* Start in active mode */
	sleepmgr_lock_mode(SLEEPMGR_ACTIVE);

	/* Go to sleep, sleep modes are configured by interrupt routines. */
	while (1) {
		sleepmgr_enter_sleep();
	}
}
