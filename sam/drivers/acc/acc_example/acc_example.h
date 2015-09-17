/**
 * \file
 *
 * \brief Analog Comparator Controller (ACC) example for SAM.
 *
 * Copyright (c) 2011-2015 Atmel Corporation. All rights reserved.
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

 #ifndef ACC_EXAMPLE_H_INCLUDED
 #define ACC_EXAMPLE_H_INCLUDED
 
/**
 * \page asfdoc_sam_drivers_acc_example_irq Analog Comparator Controller - Example Interrupt Event Generation
 *
 * \section asfdoc_sam_drivers_acc_example_irq_purpose Purpose
 *
 * This example demonstrates how to use the ACC driver to perform a voltage
 * comparison on a pair of inputs and generate an interrupt.
 *
 * \section asfdoc_sam_drivers_acc_example_irq_requirements Requirements
 *
 * This example can be used on the following evaluation kits:
 * - SAM4S EK
 * - SAM4S EK2
 *
 * \section asfdoc_sam_drivers_acc_example_irq_description Description
 *
 * The DAC0 and AD5 signals are selected as the inputs to the analog comparator.
 * The user can change the output voltage of DAC0 and also change the voltage
 * on AD5 by adjusting VR1 on the evaluation kit.
 *
 * The DAC0 output voltage can be adjusted between (1/6)*ADVREF and (5/6)*ADVREF
 * in software, whilst the input voltage on AD5 can vary between 0 and ADVREF.
 *
 * A comparison interrupt event is generated when the input voltages are
 * no longer equal.
 *
 * \section asfdoc_sam_drivers_acc_example_irq_files Main Files
 * - acc.c: Analog Comparator Controller driver
 * - acc.h: Analog Comparator Controller driver header file
 * - acc_example.c: Analog Comparator Controller example application
 *
 * \section asfdoc_sam_drivers_acc_example_irq_compilinfo Compilation Information
 * This software is written for GNU GCC and IAR Embedded Workbench&reg;
 * for Atmel&reg;. Other compilers may or may not work.

 * \section asfdoc_sam_drivers_acc_example_irq_usage Usage
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
 * \verbatim
   -- ACC IRQ Example xxx --
   -- xxxxxx-xx
   -- Compiled: xxx xx xxxx xx:xx:xx --

   -- Menu Choices for this example--
   -- s: Set new DAC0 output voltage.--
   -- v: Get voltage on potentiometer.--
   -- m: Display this menu again.-- \endverbatim
 * -# Enter a character on the terminal to select a menu option.
 * -# Change the voltage on AD5 by adjusting VR1 on the evaluation kit in order.
 * To see what comparison events occur.
 */

#endif /* ACC_EXAMPLE_H_INCLUDED */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
