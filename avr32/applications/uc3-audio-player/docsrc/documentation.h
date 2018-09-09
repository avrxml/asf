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
 * <A href="http://www.microchip.com/design-centers/32-bit /">Atmel 32-bit AVR Microcontrollers</A>.\n
 * Support and FAQ: https://www.microchip.com/support/
 *
 */

/**
 *
 * Copyright (c) 2011-2018 Microchip Technology Inc. and its subsidiaries.
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
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */
