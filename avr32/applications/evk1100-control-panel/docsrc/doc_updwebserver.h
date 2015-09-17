/*****************************************************************************
 *
 * \file
 *
 * \brief EVK1100 Control Panel Updating the Web server files through USB scenario.
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

/*! \page RestoringWebServerFiles Restoring the default file system content of the Control Panel
 * In this scenario, you are a local user who wishes to access the Control Panel
 * through its USB interface. You are equipped with a PC.
 *
 * \section RestoringWsSwitchToMS 1) Press the joystick to switch the Control Panel to USB device mode.
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
 * \note If the EVK1100 board was already USB-plugged to the PC, the <b><i>"Waiting for Host"</i></b> message will just appear briefly
 * before the <b><i>"USB Mass Storage"</i></b> message is displayed (cf. picture in step 2).
 *
 * \section RestoringWsUsbPlug 2) Connect the PC to the EVK1100 board USB plug.
 * \note You can bypass this step if you've already plugged the PC to the EVK1100 board USB plug.
 *
 * Use the miniB-plug-to-STD-A-plug USB cable to connect the EVK1100 board and the PC.
 * The LCD screen line 2 and 3 should change. Line 2 of the screen(the status line)
 * should display a blinking USB icon meaning that a USB connection is up and running. Line 3
 * of the screen(the menu line) should display the information string <b><i>"USB Mass Storage"</i></b>
 * meaning that the Control Panel is acting as a USB Mass Storage device. \n
 * \n
 * \image html UsbMsScreen.gif
 * \n
 *
 * \section RestoringWsWin 3) Access the Control Panel Mass Storage memory.
 * There is no menu on the EVK1100 board in this mode. The Control Panel acts as
 * a regular USB Mass Storage key in this mode. The PC sees the Control Panel as a
 * removable disk: it has full access to the Control Panel file system.\n
 * \n
 * \image html UsbMsExplorerG.gif
 * \n
 *
 * \section RestoringWsDeleteOld 4) Delete the CFG and WEB directories from the Control Panel's memory
 * \n
 * \image html UpdatingWsDeleteOld.gif
 * \n
 *
 * \section RestoringWsCopyNew 5) Copy the default versions previously stored on PC (copy/paste from PC to Control Panel)
 * The default WEB and CFG directories of the Control Panel are stored under the
 * avr32/applications/evk1100-control-panel/dataflash_content directory.
 * \n
 * \image html RestoringWsCopyNew.gif
 * \n
 * Wait for the end of the copy.
 *
 * \section RestoringWsSafelyUnplug 6) Safely unplug the Control Panel from the PC.
 * \n
 * \image html UsbMsDevUnplug.gif
 * \n
 *
 * \section RestoringWsPhyUnplug 7) Unplug the USB cable from the board.
 * To effectively leave the USB device mode, unplug the USB cable from the board or
 * if the USB is used as the board's power supply, simply press the joystick to leave
 * the USB device mode.
 *
 * \note You may now do a Web server session : check the \ref EthWebServer scenario.
 *
 * <HR>
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/products/avr/">Atmel AVR</A>.\n
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
