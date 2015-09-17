/**
 *
 * \file
 *
 * \brief AES example for SAM.
 *
 * This file defines a useful set of functions for the AES on SAM devices.
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

 #ifndef AES_EXAMPLE_H_INCLUDED
 #define AES_EXAMPLE_H_INCLUDED
 
/** \page asfdoc_sam_drivers_aes_example1 Advanced Encryption Standard - Example Cipher Operating Modes and DMA
 *
 * \section asfdoc_sam_drivers_aes_example1_purpose Purpose
 *
 * This example demonstrates how to use the AES driver to perform:
 * - ECB encryption and decryption
 * - CBC encryption and decryption
 * - CFB encryption and decryption
 * - OFB encryption and decryption
 * - CTR encryption and decryption
 * - ECB encryption and decryption using DMA/PDC
 *
 *
 * \section asfdoc_sam_drivers_aes_example1_requirements Requirements
 * All SAM devices with an AES module can be used. This example has been
 * tested with the following evaluation kits:
 * - SAM4E EK
 * - SAM4C EK
 *
 * \section asfdoc_sam_drivers_aes_example1_description Description
 * Upon startup, the program uses the USART driver to display a menu
 * from which several encryption/decryption modes can be tested.
 *
 *
 * \section asfdoc_sam_drivers_aes_example1_files Main Files
 * - aes.c : AES driver
 * - aes.h : AES header file
 * - aes_example.c : AES code example
 *
 *
 * \section asfdoc_sam_drivers_aes_example1_compinfo Compilation Info
 * This software was written for the GNU GCC and IAR Systems compiler.
 * Other compilers may or may not work.
 *
 *
 * \section asfdoc_sam_drivers_aes_example1_setupinfo Usage
 * -# Build the program and download it into the evaluation board.
 * -# On the computer, open and configure a terminal application
 *    (e.g., HyperTerminal on Microsoft&reg; Windows&reg;) with these settings:
 *   - 115200 baud
 *   - 8 bits of data
 *   - No parity
 *   - 1 stop bit
 *   - No flow control
 * -# Start the application.
 * -# In the terminal window, the following text should appear:
 * \verbatim
       -- AES Example --
       -- xxxxxx-xx
       -- Compiled: xxx xx xxxx xx:xx:xx --

       Menu :
         -- Select operation:
         h: Display menu
         1: ECB mode test.
         2: CBC mode test.
         3: CFB128 mode test.
         4: OFB mode test.
         5: CTR mode test.
         d: ECB mode test with DMA
         p: ECB mode test with PDC \endverbatim
 */

#endif /* AES_EXAMPLE_H_INCLUDED */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
