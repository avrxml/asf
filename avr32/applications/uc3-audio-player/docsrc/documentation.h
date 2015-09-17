/*! \defgroup group_features Features
 * \defgroup group_compilation Compilation
 * \defgroup group_main_files Main Files
 * \defgroup group_getting_started Getting Started
 * \defgroup group_device_supported Supported Device List
 */

/*! \addtogroup group_compilation
 * This section describes how to compile and load the code using one of the
 * compiler supported.\n
 * This software was written for the GNU GCC and IAR Systems compiler for 32-bit
 * AVR Microcontrollers. Other compilers may or may not work.
 *
 * - See also AVR32709: AVR32 UC3 Audio Decoder Over USB application note
 *
 * - Plug the USB power cable on evaluation kit.
 * - Make sure the Power LED is on.
 */

/*! \addtogroup group_main_files
 *
 * \section core Audio Player Core
 * - main.c : Contains the main() function.
 * - com_task.c : File in charge of the HMI.
 * - audio_interface.c : The abstract layer providing a generic API for audio players.
 * - audio_mixer.c : Common API for output codec drivers.
 * - buff_player.c : Audio samples bufferization.
 *
 * \section usb_host USB Host
 * - usb_task.c : Basic USB features handler (connections, errors...).
 * - usb_host_task.c : State machine of the USB host process.
 * - usb_host_enum.c : In charge of the USB host enumeration.
 *
 */

/*! \addtogroup group_features
 *
 * - Play,Pause,Stop,Volume +/-,Next,Previous,FFW,FRW,Shuffle,Repeat features.
 *
 * - Current and total time, Metadata, Cover arts.
 *
 * - Navigation, playback and configuration views.
 */

/*! \mainpage Audio Player over USB for 32-bit AVR Microcontrollers
 *
 * \image html audio_player.jpg
 *
 * \htmlonly <hr /> \endhtmlonly
 *
 * \section license License
 * Use of this program is subject to Atmel's End User License Agreement.
 *
 * Please read the \ref license at the bottom of this page.
 *
 * \section intro Introduction
 * This is the documentation for the data structures, functions, variables, defines, enums, and
 * typedefs for the 32-bit AVRs Audio Player Software.
 * Audio solutions: www.atmel.com/audio
 * Please refer to the corresponding application note for more information.
 *
 * \section plan Covered in this document
 * - \ref group_features
 * - \ref group_getting_started
 * - \ref group_compilation
 * - \ref group_main_files
 * - \ref group_release_note
 * - \ref group_device_supported
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/products/AVR32/">Atmel 32-bit AVR Microcontrollers</A>.\n
 * Support and FAQ: http://www.atmel.com/design-support/
 *
 */

/**
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
