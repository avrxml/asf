/**
 * \file
 *
 * \brief megaAVR STK600 Clock Example
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
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
 * <A href="http://www.atmel.com/">Atmel</A>.\n
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
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
