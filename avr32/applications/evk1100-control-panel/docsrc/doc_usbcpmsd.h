/*****************************************************************************
 *
 * \file
 *
 * \brief EVK1100 Control Panel USB Mass storage device scenario.
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

/*! \page UsbCpMassStorageDevice Accessing the Control Panel acting as a USB Mass Storage device
 * In this scenario, you are a local user who wishes to access the Control Panel
 * through its USB interface. You are equipped with a PC.
 *
 * \section UsbDevSwitchToMS 1) Press the joystick to switch the Control Panel to USB device mode.
 * - As mentionned on the LCD on line 3 <b><i>"   Set as USB key   "</i></b>,
 * press the joystick to switch to Mass Storage device mode.\n
 * \n
 * \image html CpIdleScreen.gif
 * \n
 * - Once the joystick has been pressed, you may plug a USB host to the EVK1100 board :
 * note that the message on the LCD on line 3 has changed to <b><i>"Waiting for Host"</i></b>\n
 * \n
 * \image html UsbMsDevWait.gif
 * \n
 * \note \n
 *   1) If the EVK1100 board was already USB-plugged to the PC, the <b><i>"Waiting for Host"</i></b> message will just appear briefly
 * before the <b><i>"USB Mass Storage"</i></b> message is displayed (cf. picture in step 2).\n
 *   2) As long as the Control Panel is not connected to the PC, you may leave
 * this "waiting" mode by pressing on the joystick again. You will then go back
 * to the previous default menu (the one that says <b><i>"   Set as USB key   "</i></b> ).\n
 * \n
 * \warning Note that while the Control Panel is in USB device mode, the Web server
 * is considered to be in maintenance mode. An access to the web server while the
 * Control Panel is acting as a USB device will return the following web page:\n
 * \n
 * \image html UsbWebMaintenance.gif
 * \n
 *
 * \warning While the Control Panel is in USB device mode, only the USB host(i.e. the PC)
 * may access the Control Panel's file system. This is why the Web server is considered
 * to be in maintenance mode. Sensors and actuators logs are still being generated
 * but they are not stored in files in this mode. For the same reason, file system
 * commands on the USART1 shell-like interface are unavailable then.
 *
 * \section UsbDevPlug 2) Connect the PC to the EVK1100 board USB plug.
 * Use the miniB-plug-to-STD-A-plug USB cable to connect the EVK1100 board and the PC.
 * The LCD screen line 2 and 3 should change. Line 2 of the screen(the status line)
 * should display a blinking USB icon meaning that a USB connection is up and running. Line 3
 * of the screen(the menu line) should display the information string <b><i>"USB Mass Storage"</i></b>
 * meaning that the Control Panel is acting as a USB Mass Storage device. \n
 * \n
 * \image html UsbMsScreen.gif
 * \n
 *
 * \section UsbMsDevWin 3) Access the Control Panel Mass Storage memory.
 * There is no menu on the EVK1100 board in this mode. The Control Panel acts as
 * a regular USB Mass Storage key in this mode. The PC sees the Control Panel as a
 * removable disk: it has full access to the Control Panel file system.\n
 * \n
 * \image html UsbMsExplorer.gif
 * \n
 *
 * \section UsbMsDevActions 4) What can be done in the USB Mass Storage device mode
 * - Access to the sensors and actuators log files (all log files are stored under
 * the /LOG directory):\n
 * \n
 * \image html UsbMsExplorerLog.gif
 * \n
 *    - get the log files from the Control Panel to the PC
 *    - remove the log files
 *    - remove the /LOG directory. \note If the /LOG directory doesn't exist, log
 *    files won't be generated.
 *
 * - Access to the Web server files (the web server files are stored under the /WEB
 * directory):\n
 * \n
 * \image html UsbMsExplorerWeb.gif
 * \n
 *    - remove the /WEB directory. \note If the /WEB directory doesn't exist, the
 *    Web server won't serve any page.
 *    - update the /WEB directory with a new version previously stored on PC (copy/paste
 *    from PC to Control Panel)
 *
 * - Access to the configuration /CFG directory:
 * The /CFG directory may contain some Control Panel configuration files. These
 * are simple text files that the Control Panel creates to store other-than-default
 * configurations.\n
 * \n
 * \image html UsbMsExplorerCfg.gif
 * \n
 * Here is for example the content of a net.txt configuration file of the Control
 * Panel's network module:
 * \include net.txt
 * What can be done under the /CFG directory:
 *    - remove the /CFG directory. \note If the /CFG directory doesn't exist, system
 *    configurations commands won't work.
 *    - change the content of some existing .txt configuration files.
 *    - add new configuration files.
 *
 * - Use the Control Panel Mass Storage memory as a regular USB key: you may store
 * any kind of files on the Control Panel's file system.
 *
 * \section UsbMsDevLeave 5) Leaving the USB Mass Storage device mode
 * As you would for a regular USB key, it is best to:
 *   -# <b>On the PC side</b>: safely unplug the USB device through the Windows task stray,
 * \n
 * \image html UsbMsDevUnplug.gif
 * \n
 *   -# <b>On the EVK1100 side</b>, two possibilities:
 *     -# if the USB is used as the board's power supply: simply press the joystick to leave
 *        the USB device mode.
 *     -# else unplug the USB cable from the Control Panel's board.
 *
 * Once the Control Panel has left the Mass Storage Device mode, the LCD screen
 * should look like this (note that the USB icon disappeared from line 2 and that
 * the message <b><i>"   Set as USB key   "</i></b> is displayed again on line 3):
 * \n
 * \image html CpIdleScreen.gif
 *
 * \note If there are no Ethernet connection, the IP address won't show on the
 * LCD screen.
 *
 * <HR>
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/products/AVR32/">Atmel AVR32</A>.\n
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
