/**
 * \file
 *
 * \brief Task to manage CPU load monitoring
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

#ifndef APP_CPU_LOAD_H_INCLUDED
#define APP_CPU_LOAD_H_INCLUDED

/**
 * \ingroup demo_xmegac3_xplained_group
 * \defgroup cpu_load_task_group CPU load task
 *
 * The CPU load task monitors the CPU active time and CPU sleep time through a
 * TC counter. The counter is saved and reseted when the CPU enters and exits
 * of the sleep mode. Each 250ms, the ratio is displayed through a bargraph on
 * the OLED display. The implementation is available in app_cpu_load.c/.h files.
 *
 * @{
 */

/**
 * \def ISR
 * \brief Redefinition of define service routine ISR() to call a sub routine
 * before execute the interrupt function.
 *
 * Usage:
 * \code
	ISR(FOO_vect)
	{
	    app_cpu_load_exit_sleep();
	    ...
	}
\endcode
 *
 * \param vect Interrupt vector name as found in the device header files.
 */
#if defined(__DOXYGEN__)
#  define ISR(vect)
#elif defined(__GNUC__)
#  include <avr/interrupt.h>
#  undef ISR
#  define ISR(vect) \
	static void handler_##vect(void); \
	ISR_GCC(vect) \
	{ \
	app_cpu_load_exit_sleep(); \
	handler_##vect(); \
	} \
	static void handler_##vect(void)
#  define ISR_GCC(vector, ...)            \
	void vector (void) __attribute__ ((signal,__INTR_ATTRS)) __VA_ARGS__; \
	void vector (void)
#elif defined(__ICCAVR__)
#  define __ISR(x) _Pragma(#x)
#  define ISR(vect) \
	static void handler_##vect(void); \
	__ISR(vector=vect) __interrupt void prehandler_##vect(void) \
	{ \
	app_cpu_load_exit_sleep(); \
	handler_##vect(); \
	} \
	static void handler_##vect(void)
#endif

/**
 * \brief Initialize the CPU load task
 */
void app_cpu_load_init(void);

/**
 * \brief Notifys that the CPU go to sleep mode
 */
void app_cpu_load_enter_sleep(void);

/**
 * \brief Notifys that the CPU is in active mode
 */
void app_cpu_load_exit_sleep(void);

/**
 * \brief Task to update the CPU load bargraph each 250ms
 */
void app_cpu_load_task(void);

/* ! @} */

#endif /* APP_CPU_LOAD_H_INCLUDED */
