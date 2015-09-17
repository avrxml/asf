/**
 * \file
 *
 * \brief SAM Asynchronous Timer (AST) Example 1 Calendar Mode
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

#ifndef AST_EXAMPLE1_H_INCLUDED
#define AST_EXAMPLE1_H_INCLUDED
 
/**
 * \page asfdoc_sam_drivers_ast_example1 Asynchronous Timer (AST) -  Example 1 Calendar Mode
 *
 * \section asfdoc_sam_drivers_ast_example1_purpose Purpose
 * This example demonstrates how to use the AST driver with the 32kHz
 * oscillator crystal.
 *
 * \section asfdoc_sam_drivers_ast_example1_requirements Requirements
 * This example can be used with SAM4L evaluation kits.
 *
 * \section asfdoc_sam_drivers_ast_example1_description Description
 * Upon startup, the program uses the USART driver to display a real time
 * counter incremented every second.
 * The display is triggered by the interrupt controller module.
 *
 * \section asfdoc_sam_drivers_ast_example1_files Main Files
 * - ast.c: Asynchronous Timer driver
 * - ast.h: Asynchronous Timer driver header file
 * - ast_example1.c: Asynchronous Timer example application
 *
 * \section asfdoc_sam_drivers_ast_example1_compilinfo Compilation Information
 * This software is written for GNU GCC and IAR Embedded Workbench&reg;
 * for Atmel&reg;. Other compilers may or may not work.
 *
 * \section asfdoc_sam_drivers_ast_example1_configinfo Usage
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
 *     \code
 *     -- AST Example 1 in Calendar Mode --
 *     -- xxxxxx-xx
 *     -- Compiled: xxx xx xxxx xx:xx:xx --
 *
 *     Config AST with 32 KHz oscillator.
 *
 *     Calendar: Year:XX Month:XX Day:XX, XXhXXmXXs \endcode
 * -# Approximately every second the time should increment.
 *
 */

#endif /* AST_EXAMPLE1_H_INCLUDED */
