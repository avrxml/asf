/**
 * \file
 *
 * \brief Macros and definitions for handling errors and other configurable
 * actions.
 *
 * \par Application note:
 *      AVR1610: Guide to IEC60730 Class B compliance with XMEGA
 *
 * Copyright (c) 2011-2018 Microchip Technology Inc. and its subsidiaries.
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
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#ifndef CLASSB_ERROR_HANDLER_H
#define CLASSB_ERROR_HANDLER_H

#include <asf.h>

/** Macro for telling the compiler not to initialize a variable on restart */
#if defined(__ICCAVR__)
#  define NO_INIT __no_init
#elif defined(__GNUC__)
#  define NO_INIT __attribute__ ((section (".noinit")))
#endif

/**
 * \defgroup error_handler Error Handling
 *
 * \brief Definitions related to error handling for Class B tests.
 *
 * In order to make the library as flexible as possible, the user can configure
 * the response to the different errors that can appear during the tests.
 * Therefore, the user is responsible to handle the errors in a way such that
 * the application is safe and reliable.
 *
 * @{
 */

/**
 * \name Global variables defined in the main application.
 *
 * @{
 */

/** Global error indicator, defined in \ref main.c */
NO_INIT extern volatile enum classb_error_value classb_error;

/** @} */

/**
 * \name Prototypes for functions to be called by the error handlers.
 *
 * \note These functions should be defined in an application file.
 *
 * @{
 */

/** @} */

/** Class B error values for the global error variable */
enum classb_error_value {
	/** No error */
	CLASSB_ERROR_NONE   = 0x00,
	/** Watchdog timer error (runtime) */
	CLASSB_ERROR_WDT    = 0x01,
	/** Analog IO error */
	CLASSB_ERROR_ANALOG = 0x02,
	/** CRC error */
	CLASSB_ERROR_CRC    = 0x04,
	/** Clock frequency error */
	CLASSB_ERROR_FREQ   = 0x08,
	/** Interrupt monitor error */
	CLASSB_ERROR_INT    = 0x10,
	/** CPU register error */
	CLASSB_ERROR_CPU    = 0x20,
	/** SRAM error */
	CLASSB_ERROR_SRAM   = 0x40,
	/** Other error, e.g., detected by application */
	CLASSB_ERROR_OTHER  = 0x80,
};

/**
 * \name Error handlers for Class B tests.
 *
 * These are the statements that should be run if a test fails.
 *
 * \note The statements need to be declared as:
 * - <tt>do{\<statements\>}while(0)</tt> or similar.
 * - Example: in order to hang the device it is possible to set
 * <tt>do{}while(1)</tt>.
 *
 * @{
 */
/** Error handler for the ADC, DAC and analog multiplexer test */
#define CLASSB_ERROR_HANDLER_ANALOG()               \
	do {                                        \
		classb_error = CLASSB_ERROR_ANALOG; \
	} while (0)
/** Error handler for the CRC test */
#define CLASSB_ERROR_HANDLER_CRC()               \
	do {                                     \
		classb_error = CLASSB_ERROR_CRC; \
	} while (0)
/** Error handler for the CPU frequency test */
#define CLASSB_ERROR_HANDLER_FREQ()               \
	do {                                      \
		classb_error = CLASSB_ERROR_FREQ; \
	} while (0)
/** Error handler for the interrupt monitor */
#define CLASSB_ERROR_HANDLER_INTERRUPT()         \
	do {                                     \
		classb_error = CLASSB_ERROR_INT; \
	} while (0)
/** Error handler for the CPU registers test */
#define CLASSB_ERROR_HANDLER_REGISTERS()         \
	do {                                     \
		classb_error = CLASSB_ERROR_CPU; \
	} while (0)
/** Error handler for the SRAM test */
#define CLASSB_ERROR_HANDLER_SRAM()               \
	do {                                      \
		classb_error = CLASSB_ERROR_SRAM; \
	} while (0)
/** Error handler for watchdog timer test */
#define CLASSB_ERROR_HANDLER_WDT() \
	do{                        \
	} while (1)

/** @} */

/**
 * \name Configurable actions and conditions for Class B tests.
 *
 * Some tests have a configurable behavior.
 *
 * \note
 * - The conditions should be declared as:
 * <tt>(\<condition\>)</tt>.
 * - Statements need to be declared as:
 * <tt>do{\<statements\>}while(0)</tt> or similar.
 * - Example: in order to hang the device it is possible to set:
 * <tt>do{}while(1)</tt>.
 *
 * @{
 */

/** Condition to assign a new state */
#define CLASSB_CONDITION1_INTERRUPT (!classb_error)
/** Condition to stop checking interrupts within the monitor */
#define CLASSB_CONDITION2_INTERRUPT (classb_error)
/** Configurable actions in the RTC interrupt. */
#define CLASSB_ACTIONS_RTC() ;
/** First group of configurable actions in the watchdog timer test. */
#define CLASSB_ACTIONS_WDT_RUNTIME_FAILURE() do {                              \
	/* In our example application we set the error variable and            \
	 *  re-enable watchdog timer.                                          \
	 */                                                                    \
	classb_error = CLASSB_ERROR_WDT;                                       \
	/* WDT configuration for the main application: WDT in normal mode*/    \
	CCP = CCP_IOREG_gc;                                                    \
	WDT.CTRL =  WDT_ENABLE_bm | CLASSB_WDT_PER | WDT_CEN_bm;               \
	/* Wait until WDT Synchronized */                                      \
	while (WDT.STATUS & WDT_SYNCBUSY_bm) {                                 \
	}                                                                      \
	/* Set Closed period (when WDT cannot be reset) */                     \
	CCP = CCP_IOREG_gc;                                                    \
	WDT.WINCTRL = WDT_WEN_bm | CLASSB_WDT_WPER | WDT_WCEN_bm;              \
	/* Wait until WDT Synchronized */                                      \
	while (WDT.STATUS & WDT_SYNCBUSY_bm) {                                 \
	}                                                                      \
} while (0)
/** Second group of configurable actions in the watchdog timer test. */
#define CLASSB_ACTIONS_WDT_OTHER_FAILURE() do{ } while (0)

/** @} */

/** @} */

#endif /* CLASSB_ERROR_HANDLER_H */
