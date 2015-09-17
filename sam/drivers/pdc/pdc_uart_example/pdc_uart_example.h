/**
 * \file
 *
 * \brief SAM Peripheral DMA Controller Example.
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
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef PDC_UART_EXAMPLE_H_INCLUDED
#define PDC_UART_EXAMPLE_H_INCLUDED

/**
 * \page asfdoc_sam_drivers_pdc_example Peripheral DMA Controller Example
 *
 * \section asfdoc_sam_drivers_pdc_example_purpose Purpose
 *
 * The pdc_uart example demonstrates how to use PDC driver to receive/send
 * data from/to the UART.
 *
 * \section asfdoc_sam_drivers_pdc_example_requirements Requirements
 *
 * This example can be used on any SAM3/4 boards.
 *
 * \section asfdoc_sam_drivers_pdc_example_description Description
 *
 * The SAM controller waits for BUFFER_SIZE data to receive from the UART.
 * As soon as the expected amount of data is received, the whole buffer is
 * sent back to the terminal.
 *
 * \section asfdoc_sam_drivers_pdc_example_usage Usage
 *
 * -# Build the program and download it into the evaluation board.
 * -# On the computer, open, and configure a terminal application
 *    (e.g., HyperTerminal on Microsoft&reg; Windows&reg;) with these settings:
 *   - 115200 baud
 *   - 8 bits of data
 *   - No parity
 *   - 1 stop bit
 *   - No flow control
 * -# In the terminal window, the following text should appear (values depend
 *    on the board and chip used):
     \verbatim
      -- PDC Uart Example xxx --
      -- xxxxxx-xx
      -- Compiled: xxx xx xxxx xx:xx:xx --
     \endverbatim
 * -# The sent text should appear.
 */

 #endif /* PDC_UART_EXAMPLE_H_INCLUDED */
 
