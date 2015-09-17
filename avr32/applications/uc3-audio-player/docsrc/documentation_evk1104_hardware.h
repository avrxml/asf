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
/*! \addtogroup group_features
 *
 * - Supports the \b EVK1104 (high speed USB)
 */

/*! \addtogroup group_getting_started
 *
 * \htmlonly <hr /> \endhtmlonly
 *
 * \section evk1104 EVK1104
 *
 * \image html evk1104.jpg
 *
 * The following steps have to be achieved one by one in the proper order.
 *
 * \subsection evk1104_step1 Step1 : Load the bitmap pictures into the dataflash
 *
 * This application needs some bitmap pictures to be stored into the
 * data-flash of the evaluation kit.
 *
 * Here is the procedure for a PC using Windows XP/Vista (for other
 * platforms, please adapt this steps according to the OS):
 *
 * -# Plug a JTAGICE or AVRONE! emulator on the EVK1104.
 * -# Launch the "EVK1104_program_mass_storage.cmd" application. This file
 *    is located under the 'avr32/applications/uc3-audio-player/root/'
 *    subdirectory of the ASF tree. It will program
 *    the internal flash with a USB device mass storage application.
 * -# Connect the board to a PC using a USB cable. Use the "USB User"
 *    connector: from the 2 set of USB connectors, this the one which is on
 *    the left.
 * -# This application will start and the EVK will be seen from the PC as a
 *    removable disk that will appear on the Windows desk after few seconds.
 * -# Format this removable disk. Its size should be 8 MBytes. It represents
 *    the local dataflash (AT45DB) on the kit.
 * -# Copy the content of the 'avr32/applications/uc3-audio-player/pictures/'
 *    directory (do not copy the folder, but its content) into the removable
 *    disk.
 * -# Once this is done, "safely remove" the removable disk (green icon on
 *    the right of the Windows tool bar).
 *
 * That's it. The dataflash now contains the pictures for the Audio Player
 * application.
 *
 * \subsection evk1104_step2 Step2 : Load the application
 *
 * Load the audio decoder over USB application, refer to the AVR32709
 * application note doc7817.pdf.
 *
 * The following describes the most simple way to load the application but you
 * need the appropriate setup. For an alternative, please refer to the
 * `\ref group_compilation' section.
 *
 * -# Plug a JTAGICE or AVRONE! emulator on the evaluation kit. Power the
 *    board with a USB miniAB cable to the "USB VCP" connector (from the 2
 *    sets of USB connectors, this the one which is on the right).
 * -# Drag the appropriate "EVK1104*_audio_player_*.elf,bin" file onto the
 *    "EVK1104_program_audio_player.cmd" application. It will program the
 *    internal flash with the audio decoder application.
 * -# Wait until the programing is done.
 *
 * \subsection evk1104_step3 Step3 : Set in place the application
 *
 * - Stop the debugger if it is still running.
 *  - AVR32 Studio or IAR.
 * - Make sure the USB power cable is plugged on the right USB port of the
 *   EVK1104 board.
 * - Plug the mini A to B receptacle USB cable extension in the left USB port of
 *   the EVK1104 board.
 *  - This cable is the short black USB cable with a female USB port at one
 *    end and a mini A plug at the other end.
 * - Plug the USB device (see `\ref group_device_supported') to the other end of
 *   this USB cable.
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

