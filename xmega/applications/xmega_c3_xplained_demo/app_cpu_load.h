/**
 * \file
 *
 * \brief Task to manage CPU load monitoring
 *
 * Copyright (c) 2012-2018 Microchip Technology Inc. and its subsidiaries.
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
