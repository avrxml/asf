/**
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
/*! \addtogroup group_compilation
 *
 * \section gcc_jtag Stand-alone GCC + JTAG
 * - The GNU Toolchain for 32-bit AVR Microcontrollers must be preliminary installed.
 *  - If not, please download it and install it from the following link:
 *    http://ww1.microchip.com/downloads/en/AppNotes/doc32177.pdf
 * - Plug the JTAGICE mkII between the PC and the EVK using the JTAG connector.
 * - Open a shell.
 * - Go to the /applications/uc3-audio-player/[...]/gcc/ directory and type:
 *  \code `make rebuild program run' \endcode
 *
 * \section gcc_bootloader Stand-alone GCC + USB DFU BOOTLOADER
 * - The GNU Toolchain for 32-bit AVR Microcontrollers must be preliminary installed.
 *  - If not, please download it and install it from the following link:
 *    http://ww1.microchip.com/downloads/en/AppNotes/doc32177.pdf
 * - Make sure the USB power cable is plugged on the left USB port of the board (USB USER).
 * - Open a shell.
 * - Go to the /applications/uc3-audio-player/[...]/gcc/ directory and type:
 *  \code `make rebuild isp program run' \endcode
 *
 * \section avr32studio AVR32Studio
 * - See "AVR32769: How to Compile the standalone AVR32 Software Framework in AVR32 Studio V2".
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */
