/**
 * \file
 *
 * \brief SAM Peripheral Event Controller (PEVC) example 1.
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
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef EVENTS_EXAMPLE1_H_INCLUDED
#define EVENTS_EXAMPLE1_H_INCLUDED

/**
 * \page asfdoc_sam_events_ex1 Example for the Peripheral Event System - AST/PDCA
 *
 * \section asfdoc_sam_events_ex1_intro Introduction
 * This example shows how to use the Peripheral Event Controller.
 * In the example, the AST generates a periodic event which is transmitted
 * to the PDCA. Each time a new event occurs, a character is sent to the
 * USART without the use of the CPU. The main loop of the function is a
 * delay 500ms and toggle a LED continuously to show CPU activity.
 *
 * \section asfdoc_sam_events_ex1_files Main Files
 * - events.c: Events driver
 * - events.h: Events driver header file
 * - events_example1.c: Events example 1 application
 *
 * \section asfdoc_sam_events_ex1_compilinfo Compilation Information
 * This software is written for GNU GCC and IAR Embedded Workbench&reg;
 * for Atmel&reg;. Other compilers may or may not work.
 *
 * \section asfdoc_sam_events_ex1_deviceinfo Device Information
 * SAM4L device can be used.
 *
 * \section asfdoc_sam_events_ex1_configinfo Configuration Information
 * This example has been tested with the following configuration:
 * - PC terminal settings:
 *   - 115200 baud
 *   - 8 data bits
 *   - no parity bit
 *   - 1 stop bit
 *   - no flow control
 *
 */
 
#endif /* EVENTS_EXAMPLE1_H_INCLUDED */
