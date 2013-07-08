/**
 * \file *********************************************************************
 *
 * \brief Platform Abstraction Layer configuration
 *
 * Copyright (c) 2013 Atmel Corporation. All rights reserved.
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
 */

#ifndef CONF_PAL_H_INCLUDED
#define CONF_PAL_H_INCLUDED

#include <parts.h>

#warning \
	"Using default values. Edit this conf_pal.h file to modify define value according to the current board."

#if (UC3)
#include "gpio.h"

/* ! \name SPI Configuration for AT86RFX transceiver in UC3 */
/* ! @{ */
#define AT86RFX_SPI                  (&AVR32_SPI0)
#define AT86RFX_SPI_NPCS             0
#define AT86RFX_SPI_SCK_PIN          AVR32_SPI0_SCK_0_0_PIN
#define AT86RFX_SPI_SCK_FUNCTION     AVR32_SPI0_SCK_0_0_FUNCTION
#define AT86RFX_SPI_MISO_PIN         AVR32_SPI0_MISO_0_0_PIN
#define AT86RFX_SPI_MISO_FUNCTION    AVR32_SPI0_MISO_0_0_FUNCTION
#define AT86RFX_SPI_MOSI_PIN         AVR32_SPI0_MOSI_0_0_PIN
#define AT86RFX_SPI_MOSI_FUNCTION    AVR32_SPI0_MOSI_0_0_FUNCTION
#define AT86RFX_SPI_NPCS_PIN         AVR32_SPI0_NPCS_0_0_PIN
#define AT86RFX_SPI_NPCS_FUNCTION    AVR32_SPI0_NPCS_0_0_FUNCTION

#define AT86RFX_RST_PIN              (AVR32_PIN_PA00)
#define AT86RFX_MISC_PIN
#define AT86RFX_IRQ_PIN              (AVR32_PIN_PA01)
#define AT86RFX_SLP_PIN              (AVR32_PIN_PA02)

#define AT86RFX_SPI_CS               AT86RFX_SPI_NPCS

#define AT86RFX_IRQ_PIN_GROUP        2
#define AT86RFX_IRQ_PIN_PRIORITY     1

#define AT86RFX_ISR()                ISR(ext_int_isr, AT86RFX_IRQ_PIN_GROUP, \
		AT86RFX_IRQ_PIN_PRIORITY)

#define AT86RFX_INTC_INIT()          irq_register_handler(ext_int_isr, \
		AVR32_GPIO_IRQ_2, 1)

/** Enables the transceiver interrupts */
#define ENABLE_TRX_IRQ()             gpio_enable_pin_interrupt(AT86RFX_IRQ_PIN,	\
		GPIO_RISING_EDGE)

/** Disable the transceiver interrupts */
#define DISABLE_TRX_IRQ()            gpio_disable_pin_interrupt(AT86RFX_IRQ_PIN)

/** Clear the transceiver interrupts */
#define CLEAR_TRX_IRQ()              gpio_clear_pin_interrupt_flag( \
		AT86RFX_IRQ_PIN)

/** This macro saves the trx interrupt status and disables the trx interrupt. */
#define ENTER_TRX_REGION()           DISABLE_TRX_IRQ()
/** This macro restores the transceiver interrupt status. */
#define LEAVE_TRX_REGION()           ENABLE_TRX_IRQ()

#define AT86RFX_SPI_BAUDRATE         (3000000)
/* ! @} */
#endif /* UC3 */

#if (XMEGA)
/* ! \name SPI Configuration for AT86RFX transceiver in XMEGA */
/* ! @{ */
#define AT86RFX_SPI                  &SPIC
#define AT86RFX_RST_PIN              IOPORT_CREATE_PIN(PORTC, 0)
#define AT86RFX_MISC_PIN             IOPORT_CREATE_PIN(PORTC, 1)
#define AT86RFX_IRQ_PIN              IOPORT_CREATE_PIN(PORTC, 2)
#define AT86RFX_SLP_PIN              IOPORT_CREATE_PIN(PORTC, 3)
#define AT86RFX_SPI_CS               IOPORT_CREATE_PIN(PORTC, 4)
#define AT86RFX_SPI_MOSI             IOPORT_CREATE_PIN(PORTC, 5)
#define AT86RFX_SPI_MISO             IOPORT_CREATE_PIN(PORTC, 6)
#define AT86RFX_SPI_SCK              IOPORT_CREATE_PIN(PORTC, 7)

#define AT86RFX_INTC_INIT()          ioport_configure_pin(AT86RFX_IRQ_PIN, \
		IOPORT_DIR_INPUT); \
	PORTC.PIN2CTRL = PORT_ISC0_bm; \
	PORTC.INT0MASK = PIN2_bm; \
	PORTC.INTFLAGS = PORT_INT0IF_bm;

#define AT86RFX_ISR()                ISR(PORTC_INT0_vect)

/** Enables the transceiver main interrupt. */
#define ENABLE_TRX_IRQ()             (PORTC.INTCTRL |= PORT_INT0LVL_gm)

/** Disables the transceiver main interrupt. */
#define DISABLE_TRX_IRQ()            (PORTC.INTCTRL &= ~PORT_INT0LVL_gm)

/** Clears the transceiver main interrupt. */
#define CLEAR_TRX_IRQ()              (PORTC.INTFLAGS = PORT_INT0IF_bm)
/** This macro saves the trx interrupt status and disables the trx interrupt. */
#define ENTER_TRX_REGION()   { uint8_t irq_mask = PORTC.INTCTRL; \
			       PORTC.INTCTRL &= ~PORT_INT0LVL_gm
/** This macro restores the transceiver interrupt status. */
#define LEAVE_TRX_REGION()   PORTC.INTCTRL = irq_mask; }

#define AT86RFX_SPI_BAUDRATE         (3000000)
/* ! @} */
#endif /* XMEGA */

#if SAM

#define AT86RFX_SPI                  SPI
#define AT86RFX_RST_PIN              PIN_PA04
#define AT86RFX_IRQ_PIN              PIN_PA04
#define AT86RFX_SLP_PIN              PIN_PA06
#define AT86RFX_SPI_CS               0
#define AT86RFX_SPI_MOSI             PIN_PA07
#define AT86RFX_SPI_MISO             PIN_PA08
#define AT86RFX_SPI_SCK              PIN_PA09

#define AT86RFX_INTC_INIT()         ioport_set_pin_dir(AT86RFX_IRQ_PIN,	\
		IOPORT_DIR_INPUT); \
	ioport_set_pin_sense_mode(AT86RFX_IRQ_PIN, IOPORT_SENSE_RISING); \
	arch_ioport_pin_to_base(AT86RFX_IRQ_PIN)->GPIO_IERS \
		= arch_ioport_pin_to_mask(AT86RFX_IRQ_PIN); \
	arch_ioport_pin_to_base(AT86RFX_IRQ_PIN)->GPIO_IMR0S \
		= arch_ioport_pin_to_mask(AT86RFX_IRQ_PIN); \
	NVIC_EnableIRQ(GPIO_11_IRQn)

#define AT86RFX_ISR()               ISR(GPIO_11_Handler)

/** Enables the transceiver main interrupt. */
#define ENABLE_TRX_IRQ()            arch_ioport_pin_to_base(AT86RFX_IRQ_PIN)-> \
	GPIO_IERS = arch_ioport_pin_to_mask(AT86RFX_IRQ_PIN)

/** Disables the transceiver main interrupt. */
#define DISABLE_TRX_IRQ()           arch_ioport_pin_to_base(AT86RFX_IRQ_PIN)-> \
	GPIO_IERC = arch_ioport_pin_to_mask(AT86RFX_IRQ_PIN)

/** Clears the transceiver main interrupt. */
#define CLEAR_TRX_IRQ()             arch_ioport_pin_to_base(AT86RFX_IRQ_PIN)-> \
	GPIO_IFRC = arch_ioport_pin_to_mask(AT86RFX_IRQ_PIN)

/*
 * This macro saves the trx interrupt status and disables the trx interrupt.
 */
#define ENTER_TRX_REGION()         NVIC_DisableIRQ(GPIO_11_IRQn)

/*
 *  This macro restores the transceiver interrupt status
 */
#define LEAVE_TRX_REGION()         NVIC_EnableIRQ(GPIO_11_IRQn)

#define AT86RFX_SPI_BAUDRATE         (3000000)

#endif /* SAM */
#endif /* CONF_PAL_H_INCLUDED */
