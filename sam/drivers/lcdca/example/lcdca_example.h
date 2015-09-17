/**
 * \file
 *
 * \brief SAM4L LCDCA example.
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

#ifndef LCDCA_EXAMPLE_H_INCLUDED
#define LCDCA_EXAMPLE_H_INCLUDED

/**
 * \page asfdoc_sam_drivers_lcdca_example Liquid Crystal Controller - Example Interfacing to a C42048A Display
 *
 * \section asfdoc_sam_drivers_lcdca_example_intro Purpose
 * This example demonstrates how to use the LCDCA driver to interface to an external
 * LCD (C42048A).
 *
 *
 * \section asfdoc_sam_drivers_lcdca_example_requirements Requirements
 * This example can be used with the SAM4L EK evaluation kit.
 *
 *
 * \section asfdoc_sam_drivers_lcdca_example_description Description
 *
 * The example configures the LCDCA Controller in a mode defined by the LCD
 * glass board connections, and the technical characteristics of the LCD
 * component used. It uses the following LCDCA controller features:
 *  - LCD contrast control
 *  - Hardware blinking
 *  - Autonomous segment animation
 *  - Automated character (sequential and scrolling)
 *  - ASCII digit encoder
 *  - LCD beginning of frame interrupt
 *
 * The LCDCA is set up to use the external 32.768kHz clock to generate LCD frames at
 * 64Hz, using a low power waveform to reduce toggle activity, and hence power
 * consumption. In order to show the LCD Controller's capability of running in
 * a power-saving mode, sleep mode is entered whenever possible.
 *
 *
 * \section asfdoc_sam_drivers_lcdca_example_files Main Files
 *  - lcdca.c: Liquid Crystal Display Controller driver
 *  - lcdca.h: Liquid Crystal Display Controller driver header file
 *  - lcdca_example.c: Liquid Crystal Display Controller example application
 *  - conf_example.h: Liquid Crystal Display Controller example configuration header file
 *
 *
 * \section asfdoc_sam_drivers_lcdca_example_compilinfo Compilation Information
 * This software is written for GNU GCC and IAR Embedded Workbench&reg;
 * for Atmel&reg;. Other compilers may or may not work.
 *
 *
 * \section asfdoc_sam_drivers_lcdca_example_usage Usage
 * -# Build the program, and download it onto the evaluation board.
 * -# On the computer, open, and configure a terminal application
 *    (e.g., HyperTerminal on Microsoft&reg; Windows&reg;) with these settings:
 *   - 115200 baud
 *   - 8 bits of data
 *   - No parity
 *   - 1 stop bit
 *   - No flow control
 * -# Start the application.
 * -# In the terminal window, the following text should appear:
 * \verbatim
   -- LCDCA Controller Example --
   -- xxxxxx-xx
   -- Compiled: xxx xx xxxx xx:xx:xx --

   Press PB0 to stop automated sequential mode and continue. \endverbatim
 */

#endif /* LCDCA_EXAMPLE_H_INCLUDED */
