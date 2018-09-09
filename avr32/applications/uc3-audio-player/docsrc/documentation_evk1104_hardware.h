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
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

