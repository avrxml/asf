/**
 * \file *********************************************************************
 *
 * \brief Common TRX Access Configuration
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
 */

#ifndef CONF_TRX_ACCESS_H_INCLUDED 
#define CONF_TRX_ACCESS_H_INCLUDED

#include <parts.h>
#include "board.h"
#warning \
	"Using default values. Edit this conf_trx_access.h file to modify define value according to the current board."
#warning \
	"Enable CONF_BOARD_AT86RFX Macro in conf_board.h for spi init routine in case of Atmel Board."

#ifndef AT86RFX_SPI_BAUDRATE
#define AT86RFX_SPI_BAUDRATE         (4000000)
#endif 
#if (UC3)
#include "gpio.h"

/* ! \name SPI Configuration for AT86RFX transceiver in UC3 */
/* ! @{ */
#ifndef AT86RFX_SPI
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

#endif
/* ! @} */
#endif /* UC3 */

#if (XMEGA)
/* ! \name SPI Configuration for AT86RFX transceiver in XMEGA */
/* ! @{ */
#ifndef AT86RFX_SPI
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

#endif
/* ! @} */
#endif /* XMEGA */

#if SAM4L

#ifndef AT86RFX_SPI
#define AT86RFX_SPI                  SPI
#define AT86RFX_RST_PIN              PIN_PA04
#define AT86RFX_IRQ_PIN              PIN_PA04
#define AT86RFX_SLP_PIN              PIN_PA06
#define AT86RFX_SPI_CS               0
#define AT86RFX_SPI_MOSI             PIN_PA07
#define AT86RFX_SPI_MISO             PIN_PA08
#define AT86RFX_SPI_SCK              PIN_PA09

void at86rfx_isr(void);

#define AT86RFX_INTC_INIT()         ioport_set_pin_dir(AT86RFX_IRQ_PIN,	\
		IOPORT_DIR_INPUT); \
	ioport_set_pin_sense_mode(AT86RFX_IRQ_PIN, IOPORT_SENSE_RISING); \
	gpio_set_pin_callback(AT86RFX_IRQ_PIN, at86rfx_isr, 1);

#define AT86RFX_ISR()               void at86rfx_isr(void)

/** Enables the transceiver main interrupt. */
#define ENABLE_TRX_IRQ()            gpio_enable_pin_interrupt(AT86RFX_IRQ_PIN)

/** Disables the transceiver main interrupt. */
#define DISABLE_TRX_IRQ()           gpio_disable_pin_interrupt(AT86RFX_IRQ_PIN)

/** Clears the transceiver main interrupt. */
#define CLEAR_TRX_IRQ()             gpio_clear_pin_interrupt_flag( \
		AT86RFX_IRQ_PIN)

/*
 * This macro saves the trx interrupt status and disables the trx interrupt.
 */
#define ENTER_TRX_REGION()          gpio_disable_pin_interrupt(AT86RFX_IRQ_PIN);

/*
 *  This macro restores the transceiver interrupt status
 */
#define LEAVE_TRX_REGION()         gpio_enable_pin_interrupt(AT86RFX_IRQ_PIN)

#endif

#endif /* SAM4L */

#if (SAM4S || SAM4E)
#include <pio.h>
# include "pio_handler.h"

#ifndef AT86RFX_SPI
#define AT86RFX_SPI                  SPI
#define AT86RFX_RST_PIN              IOPORT_CREATE_PIN(PIOA, 23)
#define AT86RFX_IRQ_PIN              IOPORT_CREATE_PIN(PIOA, 1)
#define AT86RFX_SLP_PIN              IOPORT_CREATE_PIN(PIOA, 6)
#define AT86RFX_SPI_CS               0
#define AT86RFX_SPI_MOSI             IOPORT_CREATE_PIN(PIOA, 13)
#define AT86RFX_SPI_MISO             IOPORT_CREATE_PIN(PIOA, 12)
#define AT86RFX_SPI_SCK              IOPORT_CREATE_PIN(PIOA, 14)
#define AT86RFX_CSD                          IOPORT_CREATE_PIN(PIOA, 24)
#define AT86RFX_CPS                  IOPORT_CREATE_PIN(PIOA, 19)

#define AT86RFX_INTC_INIT()         ioport_set_pin_dir(AT86RFX_IRQ_PIN,	\
		IOPORT_DIR_INPUT); \
	ioport_set_pin_sense_mode(AT86RFX_IRQ_PIN, IOPORT_SENSE_RISING); \
	pmc_enable_periph_clk(ID_PIOA);	\
	pio_set_debounce_filter(PIOA, PIO_PA1, 10); \
	pio_handler_set(PIOA, ID_PIOA, PIO_PA1, PIO_IT_HIGH_LEVEL, at86rfx_isr); \
	NVIC_EnableIRQ((IRQn_Type)ID_PIOA); \
	pio_enable_interrupt(PIOA, PIO_PA1);

#define AT86RFX_ISR()               void at86rfx_isr(void)

/** Enables the transceiver main interrupt. */
#define ENABLE_TRX_IRQ()            pio_enable_pin_interrupt(AT86RFX_IRQ_PIN)

/** Disables the transceiver main interrupt. */
#define DISABLE_TRX_IRQ()           pio_disable_pin_interrupt(AT86RFX_IRQ_PIN)

/** Clears the transceiver main interrupt. */
#define CLEAR_TRX_IRQ()             NVIC_ClearPendingIRQ(PIOA_IRQn);

/*
 * This macro saves the trx interrupt status and disables the trx interrupt.
 */
#define ENTER_TRX_REGION()          pio_disable_pin_interrupt(AT86RFX_IRQ_PIN);

/*
 *  This macro restores the transceiver interrupt status
 */
#define LEAVE_TRX_REGION()         pio_enable_pin_interrupt(AT86RFX_IRQ_PIN)

#endif
#endif

#if (SAMD || SAMR21 || SAML21)
#if SAMR21
#warning \
	"For SAMR21 Antenna Diversity Related Pin configurations refer to system_board_init function of SAMR21 Xplained Pro board_init.c file "
#endif
#ifndef AT86RFX_SPI
#define AT86RFX_SPI                  SERCOM0
#define AT86RFX_RST_PIN              PIN_PA23
#define AT86RFX_MISC_PIN             PIN_PA23
#define AT86RFX_IRQ_PIN              PIN_PA22
#define AT86RFX_SLP_PIN              PIN_PA24
#define AT86RFX_SPI_CS               PIN_PA19
#define AT86RFX_SPI_MOSI             PIN_PA16
#define AT86RFX_SPI_MISO             PIN_PA18
#define AT86RFX_SPI_SCK              PIN_PA17
#define AT86RFX_CSD                  PIN_PA23
#define AT86RFX_CPS                  PIN_PA23
#define LED0 LED0_PIN

#define AT86RFX_SPI_SERCOM_MUX_SETTING          SPI_SIGNAL_MUX_SETTING_A
#define AT86RFX_SPI_SERCOM_PINMUX_PAD0   PINMUX_UNUSED
#define AT86RFX_SPI_SERCOM_PINMUX_PAD1   PINMUX_UNUSED
#define AT86RFX_SPI_SERCOM_PINMUX_PAD2   PINMUX_UNUSED
#define AT86RFX_SPI_SERCOM_PINMUX_PAD3   PINMUX_UNUSED

#define AT86RFX_IRQ_CHAN             6
#define AT86RFX_IRQ_PINMUX           PINMUX_PA22A_EIC_EXTINT6

/** Enables the transceiver main interrupt. */
#define ENABLE_TRX_IRQ()                extint_chan_enable_callback( \
		AT86RFX_IRQ_CHAN, EXTINT_CALLBACK_TYPE_DETECT)

/** Disables the transceiver main interrupt. */
#define DISABLE_TRX_IRQ()               extint_chan_disable_callback( \
		AT86RFX_IRQ_CHAN, EXTINT_CALLBACK_TYPE_DETECT)

/** Clears the transceiver main interrupt. */
#define CLEAR_TRX_IRQ()                 extint_chan_clear_detected( \
		AT86RFX_IRQ_CHAN);

/*
 * This macro saves the trx interrupt status and disables the trx interrupt.
 */
#define ENTER_TRX_REGION()   { extint_chan_disable_callback(AT86RFX_IRQ_CHAN, \
					       EXTINT_CALLBACK_TYPE_DETECT)

/*
 *  This macro restores the transceiver interrupt status
 */
#define LEAVE_TRX_REGION()   extint_chan_enable_callback(AT86RFX_IRQ_CHAN, \
		EXTINT_CALLBACK_TYPE_DETECT); \
	}

#endif
#endif /* SAMD || SAMR21 || SAML21 */
#endif /* CONF_TRX_ACCESS_H_INCLUDED */
