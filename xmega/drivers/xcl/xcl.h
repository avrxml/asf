/**
 * \file
 *
 * \brief AVR XMEGA XCL (XMEGA Custom Logic) driver.
 *
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

#ifndef _XCL_H_
#define _XCL_H_

#include "compiler.h"
#include "parts.h"
#include "sysclk.h"
#include "sleepmgr.h"
#include "xcl_glue_logic.h"
#include "xcl_timer.h"

#ifdef __cplusplus
extern "C" {
#endif

extern bool xcl_is_synchronous;

/**
 * \defgroup xcl_group XMEGA Custom Logic (XCL)
 *
 * See \ref xmega_xcl_quickstart
 *
 * This is a driver for the AVR XMEGA Custom Logic module (XCL). It provides
 * functions for enabling, disabling and configuring the XCL module.
 *
 * \image html xcl_block.png
 *
 * The XCL is made of two main features:
 * - \ref xcl_tc_group
 * - \ref xcl_gl_group
 *
 * \section xmega_xcl_common Common XCL configuration
 * Prior configuring or using features from either XCL Timer/Counter or Glue
 * Logic sub-modules.
 * The XCL driver provides set up functions to:
 * - Enable the XCL with xcl_enable()
 * - Disable the XCL with xcl_disable()
 * - Select the XMEGA port connected to XCL module with xcl_port()
 *
 * \section dependencies Dependencies
 * This driver depends on the following modules:
 * - \ref sysclk_group for peripheral clock control.
 * - \ref sleepmgr_group for setting allowed sleep mode.
 * - \ref interrupt_group for ISR definition and disabling interrupts during
 * critical code sections.
 *
 * @{
 */

/* ! XCL Sub-module usage */
enum xcl_sub_module_used_t {
	/* ! XCL uses only Asynchonous LUT sub functions (combinationnal) */
	XCL_ASYNCHRONOUS         = 1,
	/* ! XCL uses synchronous functions (LUT with delays or Timer) */
	XCL_SYNCHRONOUS          = 2,
};

/* ! XCL PORT configuration */
enum xcl_port_sel_t {
	/* ! XCL connected to PORT C */
	PC          = XCL_PORTSEL_PC_gc,
	/* ! XCL connected to PORT D */
	PD          = XCL_PORTSEL_PD_gc,
};

/**
 * \brief Enable the XCL module
 *
 * This function will enable the required XCL source clock and allocated the
 * required sleep mode to the sleepmanager according to the XCL usage mode
 * selection passed as parameter (synchronous or not). The selected mode will
 * is stored internaly so that the xcl_disable() function takes care to
 * unallocate the right sleep mode.
 *
 * \param mode The mode (synchronous or asynchronous) the XCL is used from \ref
 * xcl_sub_module_used_t enum.
 */
static inline void xcl_enable(enum xcl_sub_module_used_t mode)
{
	sysclk_enable_peripheral_clock(&XCL);
	if (mode == XCL_ASYNCHRONOUS) {
		sleepmgr_lock_mode(SLEEPMGR_PDOWN);
		xcl_is_synchronous = false;
	} else if (mode == XCL_SYNCHRONOUS) {
		sleepmgr_lock_mode(SLEEPMGR_IDLE);
		xcl_is_synchronous = true;
	}
}

/**
 * \brief Disable the XCL module
 *
 * This function will disable the XCL source clock and unallocate the pre-locked
 * sleep mode selected with xcl_enable().
 */
static inline void xcl_disable(void)
{
	sysclk_disable_peripheral_clock(&XCL);
	if (xcl_is_synchronous) {
		sleepmgr_unlock_mode(SLEEPMGR_IDLE);
	} else {
		sleepmgr_unlock_mode(SLEEPMGR_PDOWN);
	}
}

/**
 * \brief XCL Port selection
 *
 * This function will configure the XCL module to use the selected PORT.
 *
 * \param port The XMEGA port where to map the XCL module from \ref
 * xcl_port_sel_t enum.
 */
static inline void xcl_port(enum xcl_port_sel_t port)
{
	XCL.CTRLA &= ~XCL_PORTSEL_gm;
	XCL.CTRLA |= port;
}

/** @} */

#ifdef __cplusplus
}
#endif

/**
 * \page xmega_xcl_quickstart Quick Start Guide for the XMEGA XCL Driver
 *
 * This is the quick start guide for the \ref xcl_group , with step-by-step
 * instructions on how to configure and use the driver for a specific use case.
 * The code examples can be copied into e.g the main application loop or any
 * other function that will need to control the XCL.
 *
 * The XCL module consists of two main sub-modules:
 * - \ref xcl_tc_group
 * - \ref xcl_gl_group
 *
 * This quick-start covers both sub-module usage in dedicated use cases. Each 
 * use case covers a sub-module usage but both both sub-modules can be used
 * together at the same time within the same application.
 *
 *
 * \image html xcl_block.png
 *
 * \section xmega_xcl_qs_use_cases Submodule Quick Starts:
 * - \subpage xmega_xcl_qs_lut
 * - \subpage xmega_xcl_qs_tc
 */

/** 
 * \page xmega_xcl_qs_lut XCL Glue Logic usage to perform XOR
 *
 * The XCL Glue Logic sub-module is made of two truth tables with configurable
 * delay elements and sequential logic functions such as D-type flip-flop
 * or D-latch. 
 * The XCL implements two programmable lookup tables (LUT). Each LUT defines 
 * the truth table corresponding to the logical condition between two inputs.
 * Any combinatorial function logic is possible.
 * The LUT inputs can be connected to I/O pins or to event system channels. 
 * Connecting together the LUT units, RS Latch or any combinatorial logic 
 * between two operands can be enabled.
 * In addition, a delay element (DLY) can be configured (position and number 
 * of cycles) on either each LUT input or outpout.
 * Compared to the XCL TC, the LUT works in all sleep modes up to power down mode.
 *
 * The XCL Glue-Logic drivers function set allows to configure all elements
 * related to this sub-module:
 * - LUT type selection
 * - LUT inputs and output configuration
 * - LUT delays configuration
 * - LUT function configuration (AND, NAND, OR, NOR, XOR, XNOR, NOT...)
 *
 * The following use case will set up the XCL to perform a logic XOR function
 * between two external input pins.
 * \image html xcl_example1.png
 *
 * \section xmega_xcl_qs_lut_setup Setup steps
 *
 * \subsection xmega_xcl_qs_lut_usage_prereq Prequisites
 *
 * This module requires \ref clk_group initialization and supports
 * \ref sleepmgr_group.
 * The usage of \ref sleepmgr_group sleepmgr service is optional, but
 * recommended to reduce power consumption.
 *
 * Add to the initialization code:
 * \code
	sysclk_init();
	board_init();
	sleepmgr_init(); // Optional
\endcode
 * Add to the main IDLE loop:
 * \code
	sleepmgr_enter_sleep(); // Optional
\endcode
 *
 * \subsection xmega_xcl_qs_lut_setup_code Example code
 *
 * Prior to use the XCL module, the module should be enabled and connected to
 * the propoer XMEGA port. When no delay elements are used and the LUT type does 
 * not use any clk_per source clock, the XCL can be enables in XCL_ASYNCHRONOUS
 * mode using the xcl_enable() function.
 * The xcl_port() function allows to configure map the XCL module to a specific
 * XMEGA port.
 *
 * \code
	xcl_enable(XCL_ASYNCHRONOUS);
	xcl_port(PD);
\endcode
 * Then the XCL glue logic can be configured:
 * \code
	xcl_lut_type(LUT_2LUT2IN);
	xcl_lut_in0(LUT_IN_PINL);
	xcl_lut_in1(LUT_IN_PINL);
	xcl_lut0_output(LUT0_OUT_PIN4);
	xcl_lut_config_delay(DLY11, LUT_DLY_DISABLE, LUT_DLY_DISABLE);
	xcl_lut0_truth(XOR);
	// Here the Glue Logic is ready
	while(1) {
	    sleepmgr_enter_sleep(); // Optional
	}
\endcode
 *
 * \subsection xmega_xcl_qs_lut_setup_code_workflow Workflow
 *
 * -#  Enable XCL module in asynchronous mode
 *  - \code xcl_enable(XCL_ASYNCHRONOUS); \endcode
 *  - \note This will enable the clock system for the module if needed
 * -# Configure XCL to use XMEGA port D.
 *  - \code xcl_port(PD); \endcode
 * -# Configure XCL LUT type as two LUTs with two separates inputs. 
 *  - \code xcl_lut_type(LUT_2LUT2IN); \endcode
 *  - \note Only one LUT is used in this use case (LUT0), the second LUT is ignored (LUT1).
 * -# Configure XCL LUT0 inputs (IN0 and IN1) to use low nible pins of XMEGA port D.
 *  - \code xcl_lut_in0(LUT_IN_PINL); \endcode
 *  - \code xcl_lut_in1(LUT_IN_PINL); \endcode
 *  - \note The pin number is specified by the device pinout (see datasheet), example for port D:
 *    - When low nible is selected: IN0 = PD2, IN1 = PD0, IN2 = PD1, IN3 = PD3.
 *    - When high nible is selected: IN0 = PD6, IN1 = PD4, IN2 = PD5, IN3 = PD7.
 * -# Configure XCL LUT0 output as pin4 of XMEGA port D.
 *  - \code xcl_lut0_output(LUT0_OUT_PIN4); \endcode
 * -# Configure XCL delays elements (no delays on LUT0 and LUT1).
 *  - \code xcl_lut_config_delay(DLY11, LUT_DLY_DISABLE, LUT_DLY_DISABLE); \endcode
 * -# Configure XCL LUT0 to perfom XOR operation.
 *  - \code  xcl_lut0_truth(XOR); \endcode
 * -# The XCL glue logic will run in the background without any CPU and clock usage.
 *  - \code
	while(1) {
	    sleepmgr_enter_sleep(); // Optional
	}
\endcode
 */

/**
 * \page xmega_xcl_qs_tc XCL Timer sub-module usage to generate underflow interrupts
 *
 * The XCL TC is made of two two 8-bits timer/counter that can be configured in
 * a wide set of operating modes (ormal, input capture, PWM, 16-bit cascade
 * timer/counter or peripheral counter (PEC) ...) 
 * 
 * The XCL TC drivers function set allows to configure all elements
 * related to this sub-module:
 * - XCL TC Type and source clock configuration
 * - Manage interupts (underflow, capture) in all operating modes
 * - Acces count and capture value
 *
 * This use case will configure the XCL TC in a 16-bit cascade timer/counter 
 * (BTC16) mode and configure its underflow interrupt to call a user-defined
 * callback function.
 *
 * \section xmega_xcl_qs_tc_setup Setup steps
 *
 * \subsection xmega_xcl_qs_tc_usage_prereq Prequisites
 *
 * This module requires \ref clk_group initialization and supports
 * \ref sleepmgr_group.
 * The usage of \ref sleepmgr_group sleepmgr service is optional, but
 * recommended to reduce power consumption.
 * Also, the interrupts must be authorized for this use case.
 *
 * Add to the initialization code:
 * \code
	sysclk_init();
	board_init();
	irq_initialize_vectors();
	cpu_irq_enable();
	sleepmgr_init(); // Optional
\endcode
 * Add to the main IDLE loop:
 * \code
	sleepmgr_enter_sleep(); // Optional
\endcode

 *
 * \subsection xmega_xcl_qs_tc_setup_code Example code
 *
 * Content of conf_xcl.h:
 * \code
	#define CONFIG_XCL_CALLBACK_ENABLE
\endcode
 *
 * Add a callback functions:
 * \code
	static void my_xcl_tc16_interrupt_callback(void)
	{
	    // Add your code here.
	    // This code will be executed when an underflow interrupt occurs
	    // on XCL BTC16 timer.
	}
\endcode
 *
 * Add to, e.g., the main loop in the application C-file:
 * \code
	void main(void)
	{
	    sysclk_init();
	    board_init();
	    irq_initialize_vectors();
	    cpu_irq_enable();

	    xcl_enable(XCL_SYNCHRONOUS);
	    xcl_port(PD);
	    xcl_tc_type(TC16);
	    xcl_set_unf_interrupt_callback(my_xcl_tc16_interrupt_callback);
	    xcl_tc_set_underflow_interrupt_level(XCL_INT_LVL_LO);
	    xcl_tc_set_resolution(7800);
	    xcl_tc16_set_period(0x7FFF);
	    xcl_tc16_set_count(0x7FFF);
	    xcl_tc16_enable_underflow_interrupt();

	    while(1) {
	    }
	}
\endcode
 *
 * \subsection xmega_xcl_qs_tc_code_workflow Workflow
 *
 * -# To allow ISR declaration and XCL call back functions, the XCL driver 
 * should be configured to allow ISR usage within XCL module driver. This is 
 * achieved adding the following define in conf_xcl.h file:
 *   - \code #define CONFIG_XCL_CALLBACK_ENABLE \endcode
 * -# Add a callback functions that will be executed when underflow interrupt
 * occurs:
 *   - \code static void my_xcl_tc16_interrupt_callback(void) { ... } \endcode
 * -#  Enable XCL module in synchronous mode
 *  - \code xcl_enable(XCL_SYNCHRONOUS); \endcode
 *  - \note This will enable the clock system for the module if needed
 * -# Configure XCL to use XMEGA port D.
 *  - \code xcl_port(PD); \endcode
 * -# Configure XCL TC in 16-bit BT16 mode.
 *  - \code xcl_tc_type(TC16); \endcode
 * -# Set call back function for underflow interrupt.
 *  - \code xcl_set_unf_interrupt_callback(my_xcl_tc16_interrupt_callback); \endcode
 * -# Configure XCL TC underflow interrupt level.
 *  - \code xcl_tc_set_underflow_interrupt_level(XCL_INT_LVL_LO); \endcode
 * -# Set XCL TC period and initial count
 *  - \code xcl_tc16_set_period(0x7FFF); \endcode
 *  - \code xcl_tc16_set_count(0x7FFF); \endcode
 * -# Set XCL TC source clock to achieve the requested resolution.
 *  - \code real_resolution = xcl_tc_set_resolution(7800); \endcode
 *  - \warning When the clock source is set, the timer will start counting
 * -# Enable XCL TC16 underflow interrupt
 *  - \code xcl_tc16_enable_underflow_interrupt(); \endcode
 * -# The timer will run in the background, and the code written in the
 *   call back function will execute each time the timer overflows.
 *  - \code  while(1) {} \endcode
 */

#endif  /* _XCL_H_ */
