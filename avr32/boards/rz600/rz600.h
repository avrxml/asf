/**
 * \file
 *
 * \brief AT32UC3A3 RZ600 board header file.
 *
 * Copyright (c) 2011-2015 Atmel Corporation. All rights reserved.
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

#ifndef _RZ600_H_
#define _RZ600_H_

#include "compiler.h"
#include "gpio.h"

#define MCU_SOC_NAME        "AT32UC3A3256S"
#define BOARD_NAME          "RZ600"

#ifdef __AVR32_ABI_COMPILER__ // Automatically defined when compiling for AVR32, not when assembling.
#  include "led.h"
#endif  // __AVR32_ABI_COMPILER__


/*! \name Oscillator Definitions
 */
//! @{

#define FOSC32          32768                                 //!< Osc32 frequency: Hz.
#define OSC32_STARTUP   AVR32_PM_OSCCTRL32_STARTUP_8192_RCOSC //!< Osc32 startup time: RCOsc periods.

#define FOSC0           12000000                              //!< Osc0 frequency: Hz.
#define OSC0_STARTUP    AVR32_PM_OSCCTRL0_STARTUP_2048_RCOSC  //!< Osc0 startup time: RCOsc periods.

// Osc1 crystal is not mounted by default. Set the following definitions to the
// appropriate values if a custom Osc1 crystal is mounted on your board.
//#define FOSC1           12000000                              //!< Osc1 frequency: Hz.
//#define OSC1_STARTUP    AVR32_PM_OSCCTRL1_STARTUP_2048_RCOSC  //!< Osc1 startup time: RCOsc periods.

//! @}

/* These are documented in services/basic/clock/uc3a0_a1/osc.h */
#define BOARD_OSC0_HZ           12000000
#define BOARD_OSC0_STARTUP_US   17000
#define BOARD_OSC0_IS_XTAL      true
#define BOARD_OSC32_HZ          32768
#define BOARD_OSC32_STARTUP_US  71000
#define BOARD_OSC32_IS_XTAL     true

/*! \name USB Definitions
 */
//! @{

//! Multiplexed pin used for USB_ID: AVR32_USBB_USB_ID_x_x.
//! To be selected according to the AVR32_USBB_USB_ID_x_x_PIN and
//! AVR32_USBB_USB_ID_x_x_FUNCTION definitions from <avr32/uc3axxxx.h>.
#define USB_ID                      AVR32_USBB_USB_ID_0_0

//! Multiplexed pin used for USB_VBOF: AVR32_USBB_USB_VBOF_x_x.
//! To be selected according to the AVR32_USBB_USB_VBOF_x_x_PIN and
//! AVR32_USBB_USB_VBOF_x_x_FUNCTION definitions from <avr32/uc3axxxx.h>.
#define USB_VBOF                    AVR32_USBB_USB_VBOF_0_1

//! Active level of the USB_VBOF output pin.
#define USB_VBOF_ACTIVE_LEVEL       LOW

//! USB overcurrent detection pin.
#define USB_OVERCURRENT_DETECT_PIN  AVR32_PIN_PX33

//! @}


//! Number of LEDs.
#define LED_COUNT   2

/*! \name GPIO Connections of LEDs
 */
//! @{
#define LED0_GPIO   AVR32_PIN_PX22
#define LED1_GPIO   AVR32_PIN_PX41
//! @}

/*! \name Color Identifiers of LEDs to Use with LED Functions
 */
//! @{
#define LED_RED     LED0
#define LED_GREEN   LED1
//! @}

/*! \name SPI Connections of the AT86RFX transceiver
 */
//! @{
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

#define AT86RFX_RST_PIN              (AVR32_PIN_PA17)
#define AT86RFX_MISC_PIN             
#define AT86RFX_IRQ_PIN              (AVR32_PIN_PA20)
#define AT86RFX_SLP_PIN              (AVR32_PIN_PA19)

#define AT86RFX_SPI_CS               AT86RFX_SPI_NPCS

#define AT86RFX_IRQ_PIN_GROUP        2
#define AT86RFX_IRQ_PIN_PRIORITY     1

#define AT86RFX_ISR()                ISR(ext_int_isr, AT86RFX_IRQ_PIN_GROUP, \
									 AT86RFX_IRQ_PIN_PRIORITY)

#define AT86RFX_INTC_INIT()          irq_register_handler(ext_int_isr, AVR32_GPIO_IRQ_2, 1)

/* Enables the transceiver interrupts */
#define ENABLE_TRX_IRQ()             gpio_enable_pin_interrupt(AT86RFX_IRQ_PIN, GPIO_RISING_EDGE)

/* Disable the transceiver interrupts */
#define DISABLE_TRX_IRQ()            gpio_disable_pin_interrupt(AT86RFX_IRQ_PIN)

/* Clear the transceiver interrupts */
#define CLEAR_TRX_IRQ()              gpio_clear_pin_interrupt_flag(AT86RFX_IRQ_PIN)

/*
 * This macro saves the trx interrupt status and disables the trx interrupt.
 */
#define ENTER_TRX_REGION()           DISABLE_TRX_IRQ()

/*
 *  This macro restores the transceiver interrupt status
 */
#define LEAVE_TRX_REGION()           ENABLE_TRX_IRQ()


//! @}

/*! \name SDRAM Definitions
 */
//! @{

//! Part header file of used SDRAM(s).
#define SDRAM_PART_HDR  "mt48lc16m16a2tg7e/mt48lc16m16a2tg7e.h"

//! Data bus width to use the SDRAM(s) with (16 or 32 bits; always 16 bits on
//! UC3).
#define SDRAM_DBW       16

//! @}

#endif  // _RZ600_H_
