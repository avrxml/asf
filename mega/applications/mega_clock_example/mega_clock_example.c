/**
 * \file
 *
 * \brief megaAVR STK600 Clock Example
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
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

 /**
 * \mainpage
 * \section board STK600 development board
 * \section intro Introduction
 * This example demonstrates how to set up correct clock settings for
 * the megaAVR devices. The goal of this example is to use the libc delay
 * functions.
 *
 * \section files Files:
 * - mega_clock_example.c: megaAVR STK600 clock example application
 *
 * \section exampledescription Brief description of the example application
 * The example application set up the correct clock setting to use the
 * libc delay functions. The only thing this application will do is to
 * demonstrate how to call the libc delay functions. There are no output
 * in form of leds. To verify operation of this example it is recommended
 * to use debugging and break on different lines.
 *
 * \note Optimization <i>must</i> to be enabled for the delay functions to work correctly,
 * please refer to
 * <A href="http://www.nongnu.org/avr-libc/user-manual/group__util__delay.html">libc</A>
 * for more information.
 *
 * \section compinfo Compilation Info
 * This software was written for the <A href="http://gcc.gnu.org/">GNU GCC</A>
 * for AVR. \n
 * Other compilers may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.microchip.com/">Microchip</A>.\n
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */
#include "compiler.h"

/* set the correct clock frequency based on fuse settings or external clock/crystal
 * has to be done before including delay.h */
#include "conf_clock.h"
#include <util/delay.h>

#ifndef __OPTIMIZE__
#warning "Please use compiler optimization for correct operation of the delay \
functions."
#endif

 /**
 * \brief Maximum possible delay is 262.14 ms / F_CPU in MHz.
 *
 * Please refer to
 * <A href="http://www.nongnu.org/avr-libc/user-manual/group__util__delay.html">
 * libc</A> for more information.
 */
#define MAX_MS_DELAY (262.14 / (F_CPU / 1000000UL))

/**
 * \brief Maximum possible delay is 768 us / F_CPU in MHz.
 *
 * Please refer to
 * <A href="http://www.nongnu.org/avr-libc/user-manual/group__util__delay.html">
 * libc</A> for more information.
 */
#define MAX_US_DELAY (768 / (F_CPU / 1000000UL))

/**
 * \brief Example application on how to use the libc delay functions.
 *
 * This application shows the basic usage of the delay functions from the
 * avr libc library.
 *
 * \note The delay functions will not operate correctly if compiled without
 * optimization.
 */
int main(void)
{
	// busy wait the maximum number of milliseconds
	_delay_ms(MAX_MS_DELAY);

	// busy wait the maximum number of microseconds
	_delay_us(MAX_US_DELAY);

	// busy wait for 10us
	_delay_us(10);

	// busy wait for 10ms
	_delay_ms(10);

	// loop for ever
	while (true);
}
