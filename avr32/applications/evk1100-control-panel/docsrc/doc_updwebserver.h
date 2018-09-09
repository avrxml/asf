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
 * <A href="https://www.microchip.com/design-centers/32-bit">Atmel AVR</A>.\n
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */
