/**
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
/*! \addtogroup group_compilation
 *
 * \section gcc_jtag Stand-alone GCC + JTAG
 * - The GNU Toolchain for 32-bit AVR Microcontrollers must be preliminary installed.
 *  - If not, please download it and install it from the following link:
 *    http://www.atmel.com/dyn/products/tools_card.asp?tool_id=4118
 * - Plug the JTAGICE mkII between the PC and the EVK using the JTAG connector.
 * - Open a shell.
 * - Go to the /applications/uc3-audio-player/[...]/gcc/ directory and type:
 *  \code `make rebuild program run' \endcode
 *
 * \section gcc_bootloader Stand-alone GCC + USB DFU BOOTLOADER
 * - The GNU Toolchain for 32-bit AVR Microcontrollers must be preliminary installed.
 *  - If not, please download it and install it from the following link:
 *    http://www.atmel.com/dyn/products/tools_card.asp?tool_id=4118
 * - Make sure the USB power cable is plugged on the left USB port of the board (USB USER).
 * - Open a shell.
 * - Go to the /applications/uc3-audio-player/[...]/gcc/ directory and type:
 *  \code `make rebuild isp program run' \endcode
 *
 * \section avr32studio AVR32Studio
 * - See "AVR32769: How to Compile the standalone AVR32 Software Framework in AVR32 Studio V2".
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
