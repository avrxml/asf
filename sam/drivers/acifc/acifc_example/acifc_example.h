/**
 * \file
 *
 * \brief SAM4L Analog Comparator Interfacer (ACIFC) example.
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

#ifndef ACIFC_EXAMPLE_H_INCLUDED
#define ACIFC_EXAMPLE_H_INCLUDED

/**
 * \page asfdoc_sam_drivers_acifc_example Analog Comparator Interface Controller - Example Using Comparison Interrupt
 *
 * \section asfdoc_sam_drivers_acifc_example_purpose Purpose
 *
 * This example demonstrates how to use the ACIFC module and its interrupt to get
 * a comparison result from a pair of inputs.
 *
 * \section asfdoc_sam_drivers_acifc_example_requirements Requirements
 *
 * This example has been tested on the following evaluation kits:
 * - SAM4L EK
 * - SAM4L Xplained Pro
 * - SAM4L8 Xplained Pro
 *
 * \section asfdoc_sam_drivers_acifc_example_description Description
 *
 * This example demonstrates usage of the ACIFC module. The device pins PA06
 * and PA07 are selected as inputs. The connection can be as follows:
 *
 * <b>For SAM4L EK</b>
 *
 * - PA06(J100.2) -- ADC SENSOR VBAT(J105.1)
 * - PA07(J4.4) -- VCC(J4.10)
 *
 * Or
 *
 * - PA06(J100.2) -- ADC SENSOR VBAT(J105.1)
 * - PA07(J4.4) -- GND(J4.9)
 *
 * <b>For SAM4L/SAM4L8 Xplained Pro:</b>
 * - PA06(EXT3/PIN9) -- GND(EXT3/PIN19)
 * - PA07(EXT2/PIN3) -- VCC(EXT2/PIN20)
 *
 * Or
 *
 * - PA06(EXT3/PIN9) -- VCC(EXT3/PIN20)
 * - PA07(EXT2/PIN3) -- GND(EXT2/PIN19)

 *
 * \section asfdoc_sam_drivers_acifc_example_files Main Files
 * - acifc.c: Analog Comparator Interface Controller driver
 * - acifc.h: Analog Comparator Interface Controller driver header file
 * - acifc_example.c: Analog Comparator Interface Controller example application
 *
 * \section asfdoc_sam_drivers_acigc_example_compilinfo Compilation Information
 * This software is written for GNU GCC and IAR Embedded Workbench&reg;
 * for Atmel&reg;. Other compilers may or may not work.
 *
 * \section asfdoc_sam_drivers_acifc_example_usage Usage
 *
 * -# Build the program and download it into the evaluation board.
 * -# On the computer, open, and configure a terminal application
 *    (e.g., HyperTerminal on Microsoft&reg; Windows&reg;) with these settings:
 *   - 115200 baud
 *   - 8 bits of data
 *   - No parity
 *   - 1 stop bit
 *   - No flow control
 * -# Start the application.
 * -# In the terminal window, the following text should appear:
 * \code
 * -- ACIFC IRQ Example xxx --
 * -- xxxxxx-xx
 * -- Compiled: xxx xx xxxx xx:xx:xx -- \endcode
 * -# The application will output a different message if the voltage on pin.
 * PA06 is lower or higher than the voltage on pin PA07 :
 * \code -ISR- Voltage Comparison Result: ACAP0 > ACAN0 \endcode
 *   or
 * \code -ISR- Voltage Comparison Result: ACAP0 < ACAN0 \endcode
 */
 
#endif /* ACIFC_EXAMPLE_H_INCLUDED */
