/**
 * \file
 *
 * \brief External Interrupt for megaRF.
 *
 * Copyright (c) 2012-2015 Atmel Corporation. All rights reserved.
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
#ifndef _EXT_INT_MEGARF_H_
#define _EXT_INT_MEGARF_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "compiler.h"
#include "ioport.h"

 /**
 * \defgroup megarf_interrupt_group External Interrupt Driver
 *
 * See \ref megarf_interrupt_quickstart
 *
 * This is a driver for the AVR MEGARF external interrupt. It provides functions
 * for enabling, disabling and configuring the external interrupt modules.
 *
 * \section dependencies Dependencies
 * This driver depends on the following modules:
 * - \ref sysclk_group for peripheral clock control.
 * - \ref ioport_group for IOPORT pin configuration
 * @{
 */

/**
 * \internal
 * \def EXT_INTx
 * \brief External Interrupt input pin numbers
 */

enum ext_int_pins {
	EXT_INT0_PIN = IOPORT_CREATE_PIN(PORTD,0),     /*!< External Interrupt INT0  */
	EXT_INT1_PIN = IOPORT_CREATE_PIN(PORTD,1),      /*!< External Interrupt INT1 */
	EXT_INT2_PIN = IOPORT_CREATE_PIN(PORTD,2),     /*!< External Interrupt INT2 */
	EXT_INT3_PIN = IOPORT_CREATE_PIN(PORTD,3),    /*!< External Interrupt INT3 */
	EXT_INT4_PIN = IOPORT_CREATE_PIN(PORTE,4),    /*!< External Interrupt INT4 */
	EXT_INT5_PIN = IOPORT_CREATE_PIN(PORTE,5),    /*!< External Interrupt INT5 */
	EXT_INT6_PIN = IOPORT_CREATE_PIN(PORTE,6),    /*!< External Interrupt INT6 */
	EXT_INT7_PIN = IOPORT_CREATE_PIN(PORTE,7),    /*!< External Interrupt INT7 */					
};

/**
 * \internal
 * \def PC_INTx
 * \brief External Pin Change Interrupt input pin numbers
 */

enum ext_pcint_pins {
	PC_INT0_PIN = IOPORT_CREATE_PIN(PORTB,0),     /*!< External PC Interrupt PCINT0  */
	PC_INT1_PIN = IOPORT_CREATE_PIN(PORTB,1),      /*!< External PC Interrupt PCINT1 */
	PC_INT2_PIN = IOPORT_CREATE_PIN(PORTB,2),     /*!< External PC Interrupt PCINT2 */
	PC_INT3_PIN = IOPORT_CREATE_PIN(PORTB,3),    /*!< External PC Interrupt PCINT3 */
	PC_INT4_PIN = IOPORT_CREATE_PIN(PORTB,4),    /*!<External PC Interrupt PCINT4 */
	PC_INT5_PIN = IOPORT_CREATE_PIN(PORTB,5),    /*!< External PC Interrupt PCINT5 */
	PC_INT6_PIN = IOPORT_CREATE_PIN(PORTB,6),    /*!< External PC Interrupt PCINT6 */
	PC_INT7_PIN = IOPORT_CREATE_PIN(PORTB,7),    /*!< External PC Interrupt PCINT7 */	
	PC_INT8_PIN = IOPORT_CREATE_PIN(PORTE,0),    /*!< External PC Interrupt PCINT8 */				
};

/* Mask EXT_INT Port Pin position  */
#define EXT_INT_PORT_MASK    0x07

/**
 * \internal
 * \def CLEAR_PCINT_FLAGx
 * \brief Clears the pcint flag
 */
/* Clears the pcint flag0 */
#define CLEAR_PCINT_FLAG0() PCIFR |= (1 << PCIF0)

/* Clears the pcint flag1 */
#define CLEAR_PCINT_FLAG1() PCIFR |= (1 << PCIF1)

/**
 * \internal
 * \def CLEAR_INT_FLAG
 * \brief Clears the int flag
 */
#define CLEAR_INT_FLAG(pin) EIFR |= (1 << (pin & EXT_INT_PORT_MASK))

/**
 * \internal
 * \def INT_PIN_MASK
 * \brief Masks the required interrupt pin
 */
#define INT_PIN_MASK(pin) (1 << (pin & EXT_INT_PORT_MASK))

/**
 * \brief Interrupt event callback function type
 *
 * The interrupt handler can be configured to do a function callback,
 * the callback function must match the ext_int_callback_t type.
 *
 */
typedef void (*ext_int_callback_t) (void);

/**
 * \brief Clear ext Interrupt flag
 *
 * \param pin External Interrupt Pin
 */
void ext_int_pcint_clear_flag(ioport_pin_t pin);

/**
 * \brief Enable ext PC Interrupt
 *
 * \param pin External PC Interrupt Pin
 */
void ext_int_pcint_enable(ioport_pin_t pin);

/**
 * \brief Disable ext PC Interrupt
 *
 * \param pin External PC Interrupt Pin
 */
void ext_int_pcint_disable(ioport_pin_t pin);

/**
 * \brief Clear ext Interrupt flag
 *
 * \param pin input External Interrupt Pin
 */
void ext_int_clear_flag(ioport_pin_t pin);

/**
 * \brief Disable ext Interrupt
 *
 * \param pin input External Interrupt Pin
 */
void ext_int_disable(ioport_pin_t pin);

/**
 * \brief Enable ext Interrupt
 *
 * \param pin input External Interrupt Pin
 */
void ext_int_enable(ioport_pin_t pin);

/**
 * \brief Initializes ext Interrupt for the required interrupt Pin and trigger
 *level
 *  Configures the interrupt pin as input pin, enables the pull of the pin, set
 *the trigger mode
 *  clears the interrupt flag before enabling and enables the interrupt
 * \param pin,trigmode External Interrupt Pin and trigmode
 */
void ext_int_init(ioport_pin_t pin, enum ioport_sense trigmode);

/**
 * \brief Initializes ext pin change Interrupt for the required interrupt Pin
 *  Configures the interrupt pin as input pin, enables the pull of the pin,
 *  clears the interrupt flag before enabling and enables the interrupt
 * \param pin External Interrupt Pin and trigmode
 */
void ext_int_pcint_init(ioport_pin_t pin);

/**
 * \brief Set external interrupt callback function
 *
 * This function allows the caller to set and change the interrupt callback
 * function. Without setting a callback function the interrupt handler in the
 * driver will only clear the interrupt flags.
 *
 * \param ext_int interrupt number
 * \param callback Reference to a callback function
 */
void ext_int_set_interrupt_callback(ioport_pin_t ext_int, ext_int_callback_t callback);
/** @} */

#ifdef __cplusplus
}
#endif

/**
 * \page megarf_interrupt_quickstart Quick Start Guide for the MEGARF ext int Driver
 *
 * This is the quick start guide for the \ref interrupt_group , with step-by-step 
 * instructions on how to configure and use the driver for a specific use case. 
 * The code examples can be copied into e.g the main application loop or any
 * other function that will need to control the external interrupt.
 *
 *
 * \section megarf_interrupt_qs_use_cases Use cases
 * - \ref megarf_interrupt_qs_extint
 * - \ref megarf_interrupt_qs_pcint
 *
 *
 * \section megarf_interrupt_qs_extint External Interrupt
 *
 * This use case will prepare a external interrupt pin to trigger an interrupt when the 
 * there is a change in pin state. The interrupt is handled by a cutomisable callback
 * function.
 * We will setup the external interrupt in this mode:
 * - Enable the interrupt for falling edge(for example)
 * - Use the system clock as clock source
 * - Enable the Global Interrupt
 *
 *
 * \section megarf_interrupt_qs_extint_setup Setup steps
 *
 * \subsection megarf_interrupt_qs_extint_usage_prereq Prequisites
 *
 * For the setup code of this use case to work, the following must
 * be added to the project:
 * - \ref interrupt_group "Global Interrupt Management"
 * - \ref clk_group "Clock Management"
 *
 * \subsection megarf_interrupt_qs_extint_setup_code Example code
 *
 * Add a callback function that will be executed when the interrupt
 * trigger.
 * \code
	static void ext_int_callback(void)
	{
	    // User code to execute when the interrupt occurs here
	}
\endcode
 * Add to, e.g., the main loop in the application C-file:
 * \code
	sysclk_init();
	ext_int_init(EXT_INT0, IOPORT_SENSE_FALLING);
	ext_int_set_interrupt_callback(EXT_INT0, ext_int_callback);
	cpu_irq_enable();
\endcode
 *
 * \subsection megarf_interrupt_qs_extint_setup_code_workflow Workflow
 *
 * -# Enable the clock system:
 *  - \code sysclk_init(); \endcode
 * -#  Enable External Interrupt INT0
 *  - \code ext_int_init(EXT_INT0, IOPORT_SENSE_FALLING); \endcode
 * -# Set the callback function for interrupt
 *  - \code ext_int_set_interrupt_callback(EXT_INT0, ext_int_callback); \endcode
 *    \warning This function requires that the ext_int_callback function is defined
 * -# Enable interrupts:
 *  - \code cpu_irq_enable(); \endcode
 *
 * \section megarf_interrupt_qs_extint_usage Usage steps
 *
 * - None. The code written in the call back function will execute each time 
 * external interrupt pin state changes to falling edge. 
 *
 *
 * \section megarf_interrupt_qs_pcint external pin change interrupt
 *
 * This use case will prepare a external pin change interrupt to trigger an interrupt
 * when the there is a change in pin state. The interrupt is handled by a 
 * cutomisable callback function.
 *
 * We will setup the external interrupt in this mode:
 * - Enable the pin change interrupt
 * - Use the system clock as clock source
 * - Enable the Global Interrupt
 *
 * \section megarf_interrupt_qs_pcint_setup Setup steps
 *
 * \subsection megarf_interrupt_qs_pcint_usage_prereq Prequisites
 *
 * For the setup code of this use case to work, the following must
 * be added to the project:
 * - \ref interrupt_group "Global Interrupt Management"
 * - \ref clk_group "Clock Management"
 *
 * \subsection megarf_interrupt_qs_pcint_setup_code Example code
 *
 * Add a callback function that will be executed when the interrupt
 * trigger.
 * \code
	static void ext_pcint_callback(void)
	{
	    // User code to execute when the interrupt occurs here
	}
\endcode
 * Add to, e.g., the main loop in the application C-file:
 * \code
	sysclk_init();
	ext_int_pcint_init(PC_INT8);
	ext_int_set_interrupt_callback(PC_INT8, ext_pcint_callback);
	cpu_irq_enable();
\endcode
 *
 * \subsection megarf_interrupt_qs_pcint_setup_code_workflow Workflow
 *
 * -# Enable the clock system:
 *  - \code sysclk_init(); \endcode
 * -#  Enable External Interrupt INT0
 *  - \code ext_int_pcint_init(PC_INT8); \endcode
 * -# Set the callback function for interrupt
 *  - \code ext_int_set_interrupt_callback(PC_INT8, ext_pcint_callback); \endcode
 *    \warning This function requires that the ext_pcint_callback function is defined
 * -# Enable interrupts:
 *  - \code cpu_irq_enable(); \endcode
 *
 * \section megarf_interrupt_qs_pcint_usage Usage steps
 *
 * - None. The code written in the call back function will execute each time
 * external interrupt pin state changes.
 */
#endif /* _EXT_INT_MEGARF_H_ */
