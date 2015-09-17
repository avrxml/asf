/*****************************************************************************
 *
 * \file
 *
 * \brief EVK1100 Control Panel Default menu presentation.
 *
 * The Control Panel application is a demonstration application. Its purpose is
 * to automatically log local sensors and actuators data and events and make
 * these available through the various connectivity channels supported by the
 * AVR32.\n
 * The logs are accessible locally through USART or USB (Mass Storage class),
 * and/or remotely through the Internet (Web server).\n
 * The Control Panel is locally configurable through USART or USB (Mass Storage
 * class) or remotely configurable through the Internet (Web server).
 *
 * - OS:                 FreeRTOS port on AVR32 UC3.
 * - Boards:             EVK1100
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
 *****************************************************************************/

/*! \page DefaultMenu Using the Control Panel LCD's default menu
 * In this scenario, you are a local user who accesses the Control Panel through
 * its on-board MMI interface.
 *
 * \section DefaultMenuAvail Default menu availability and purpose
 * The default menu is available just after reset or while there is no USB device
 * nor host device plugged to the EVK1100 board. This menu offers three actions:
 * - switch the Control Panel to USB key mode (<b><i>"Set as USB key"</i></b>),
 * - format the file system on the DataFlash(<b><i>"Format a:(DF)"</i></b>),
 * - create the LOG directory in the file system of the DataFlash(<b><i>"Create a:/LOG"</i></b>),
 * - set the logs to be stored to the DataFlash(<b><i>"Log to a:(DF)"</i></b>),
 * - format the file system on the SD/MMC card(<b><i>"Format b:(SDMMC)"</i></b>),
 * - create the LOG directory in the file system of the SD/MMC card(<b><i>"Create b:/LOG"</i></b>),
 * - set the logs to be stored to the SD/MMC card(<b><i>"Log to b:(SDMMC)"</i></b>)\n
 *
 * The menu organization is the following: \n
 * <b><i>Set as USB key > < Format a:(DF) > < Create a:/LOG > < Log to a:(DF) > < Format b:(SDMMC) > < Create b:/LOG > < Log to b:(SDMMC)</i></b>
 *
 * \note <b><i>a</i></b> is the drive letter associated with the DataFlash memory. \n <b><i>b</i></b> is the drive letter associated with the SD/MMC card memory.
 *
 * \n
 * \image html CpIdleScreen.gif "Example of the LCD's default menu"
 * \n
 *
 * \section SwitchToUsbKey Set as USB key
 * Press the joystick to switch the Control Panel to behave as a USB key. For a
 * step-by-step description of this mode see the \ref UsbCpMassStorageDevice scenario.
 * \note Note the right arrow on the last column of the third line : this means
 * that there is another menu item that is reachable with a right-stroke on the joystick.
 *
 * \section Formata Format a:(DF) or Format b:(SDMMC)
 * Press the joystick to perform a format of the selected memory filesystem.
 * \n
 * \image html FormatScreen.gif "LCD screen display example to format the filesystem on the DataFlash"
 * \n
 * The menu line will display a message while it is performing the requested action.
 * \n
 * \image html FormatingScreen.gif "LCD screen display example while formatting the filesystem on the DataFlash"
 * \n
 * Another way to perform the format of a: is through the shell command <i>"format a:"</i>; same for the SDMMC memory (replace a: with b:)
 * (see the \ref ShellUsage scenario for details on the Shell command line interface).
 * \note Note the left arrow on the first column of the third line : the previous
 * action is reachable by giving a left stroke to the joystick. Note the right arrow
 * on the last column of the third line : this means that there is another menu
 * that is reachable by giving a right-stroke to the joystick.
 *
 * \section CreateLog Create a:/LOG or Create b:/LOG
 * Press the joystick to create the /LOG directory to the required memory.
 * Once this directory exists, the Control Panel will create log files there.
 * \n
 * \image html CreateLogScreen.gif LCD screen display example to create the LOG directory on the DataFlash
 * \n
 * The menu line will display a message while it is performing the requested action.
 * \n
 * \image html CreatingLogScreen.gif LCD screen display example while the LOG directory is being created on the DataFlash
 * \n
 * \note The action is done so fast that you may not see that screen.
 *
 * Another way to create the LOG directory is through the <i>mkdir</i> shell
 * command (see the \ref ShellUsage scenario for details on the Shell command line
 * interface).
 * \note Note the left arrow on the first column of the third line : the previous
 * action is reachable by giving a left stroke to the joystick.
 *
 * \section LogToDst Log to a:(DF) or Log to b:(SDMMC)
 * \n
 * \image html LogToA.gif Press the joystick to store the logs to the DataFlash
 * \image html LogToB.gif Press the joystick to store the logs to the SD/MMC card
 * \n
 *
 * <HR>
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/products/AVR32/">Atmel AVR32</A>.\n
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
