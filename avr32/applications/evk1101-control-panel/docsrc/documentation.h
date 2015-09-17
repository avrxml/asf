/*****************************************************************************
 *
 * \file
 *
 * \brief EVK1101 Control Panel application.
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
 * - Boards:             EVK1101
 *
 * Copyright (c) 2009-2015 Atmel Corporation. All rights reserved.
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

/*! \mainpage
 * This manual is divided into the following sections:
 * - \ref intro "An introduction to the Control Panel Application"
 * - \ref envinfo "A short description of the Control Panel Environment"
 * - \ref Installation "PC demo Installation"
 * - \ref drivers "Driver installation"
 * - \ref PowerUpTheBoard "Power up the board"
 * - \ref usingCtrlPanel "Using the AVR32 UC3B Control Panel PC Demo Application"
 * - \ref files "Main Files"
 * - \ref knownIssues "Known issues"
 *
 * <HR>
 * \section intro An introduction to the Control Panel Application
 * \subsection intro1 Overview
 * The Control Panel application is a demonstration application running on top of the freeRTOS.org operating system.
 * Its purpose is to scan onboard
 * sensors and actuators data and events (data acquisition through ADC channels)
 * and make these available to a PC application (known as "AVR32 UC3B Control Panel PC Demo")
 * through a simple USB cable.
 *
 * \subsection introfreertos The FreeRTOS.org kernel
 * The Control Panel implementation uses the AVR&reg;32 UC3 FreeRTOS.org kernel port.
 * FreeRTOS.org is a portable, open source, mini Real Time Kernel - a free to
 * download and royalty free RTOS that can be used in commercial applications.
 * \note The AVR&reg;32 UC3 FreeRTOS.org kernel port and basic demonstrations is currently
 * available in the AT32UC3-SoftwareFramework-x.x.x.zip package under the SERVICES/FREERTOS folder.
 *
 * \subsection introusb The ATMEL AVR32 USB stack
 * The Control Panel implementation uses the USB stack (see directory DRIVERS/USBB)
 * and the Full Custom USB class all available in the AT32UC3-SoftwareFramework-x.x.x.zip package.
 *
 * The Full Custom class is a kind of HID-like class, with the exception that the Full Custom is not
 * supported by operating system, and thus needs a driver to run. More over, HID class works using INTERRUPT
 * endpoints, while the Full Custom class can use either BULK, INTERRUPT, CONTROL or ISOCHRONOUS endpoints.
 *
 * In this example, the PC application makes use the LIBUSB driver (http://libusb-win32.sourceforge.net/) to
 * communicate with the embedded application. This library simplifies the work of the PC-developper since
 * all the USB layer is managed thanks to the lib. As a consequence, the Full Custom class of the demo is
 * easily managed by the PC application.
 *
 * <HR>
 * \section envinfo Control Panel Environment
 * \subsection Evk1101
 * This demo was designed to run on the EVK1101 evaluation kit. The content of
 * the EVK1101 board is:
 *   - a USB mini A/B connector (thus allowing the board to behave as a USB device
 *   or as a USB reduced-host),
 *   - 1 USART,
 *   - 8MB Dataflash,
 *   - 4 monocolor LEDs, [ACTUATOR]
 *   - 1 temperature sensor, [SENSOR]
 *   - 2 pushbuttons, [SENSOR]
 *   - 1 joystick, [SENSOR]
 *   - 1 light sensor, [SENSOR]
 *   - 1 accelerometer [SENSOR]
 *
 * \note For some elements of the board, we added a functionality tag expressed
 * between[] (e.g. the joystick has the SENSOR functionality).
 *
 * \subsection PowerSupply Power supply.
 * The EVK1101 board is designed to be powered by a 9V DC power supply. The EVK1101
 * can be configured to use one of the following two power sources:
 *   - 9V DC power adapter connected to the power supply jack: to use this configuration,
 * set the PWR switch(the switch should be near the power supply jack) to the EXT
 * position.
 *   - 5V DC power via the USB miniAB connector: to use this configuration, set
 * the PWR switch(the switch should be near the power supply jack) to the USB position.\n
 *
 * <HR>
 * \section Installation PC demo Installation.
 *
 * First, the PC application shall be installed prior to connecting the EVK1101. This application
 * can be found in the APPLICATIONS/EVK1101-DEMO/PC_APPLI directory.
 * The application needs a Java Runtime Environment (JRE 5.0 or higher).
 * \image html AVR32CtrlPanel_icon.jpg
 *  - If you already have installed a JRE, then launch the
 * "AVR32 UC3B Control Panel PC Demo Installer.exe"
 * application and follow the instruction.
 *  -  If you do not have a JRE, install it from
 *   https://cds.sun.com/is-bin/INTERSHOP.enfinity/WFS/CDS-CDS_Developer-Site/en_US/-/USD/ViewProductDetail-Start?ProductRef=jre-6u17-oth-JPR@CDS-CDS_Developer
 *
 * Both applications are available in the AVR32 Technical Library CD bundled with the
 * EVK1101 and on atmel.com/avr32.
 * When the installation is done, the EVK1101 can be connected to the PC.
 *
 * <HR>
 * \section drivers Driver installation
 *
 * For the first plug of the EVK1101 on the PC, the operating system will detect a new
 * peripheral.
 * \n
 * \image html install_9.JPG "New hardware detected."
 * \n
 * This will open a new hardware installation window. Choose not to connect to
 * Windows Update for this installation and click `Next':
 * \n
 * \n
 * \image html install_911.JPG "New hardware detected."
 * \n
 * On the next screen, select "Install from a list or specific location (Advanced)" and click
 * `Next'. Then request to search in the usb folder of the "AVR32 UC3B Control Panel PC
 * Demo" installation directory as shown below and click `Next':
 * \n
 * \n
 * \image html install_92.JPG "Installation."
 * \n
 * Windows will then process the installation of the driver. Once completed, click `Finish'.
 * \n
 * \n
 * \image html install_931.JPG "Installation done."
 * \n
 *
 * <HR>
 * \section PowerUpTheBoard Power up the board
 *
 * - step 1: Connect the USB cable to the PC and to the EVK1101.
 * \n
 * \image html EVK1101_side.jpg "Step 1."
 * \n
 * - step 2: Set the boards' power switch to VBUS (i.e. power from USB rather than from jack).
 * \n
 * \n
 * \image html EVK1101_top_bis.jpg "Step 2."
 * \n
 * The 4 LEDS shall shortly blink: the Control Panel is up & running.\n
 *
 * <HR>
 * \section usingCtrlPanel Using the AVR32 UC3B Control Panel PC Demo Application
 * Launch the AVR32 Control Panel PC Demo application (a shortcut has been added on
 * the desktop after the installation).
 * For the initial start of the application, you must select the target. Choose "EVK1101" in
 * the list. Click "OK": the application will connect to the EVK1101.
 * \n
 * \n
 * \image html install_81.JPG "Choose the target for the first start of the appli."
 * \n
 *
 * It will connect to the EVK1101 through the  USB
 * and show the status of the states of the various sensors and actuators.
 * \n
 * \subsection sensorsTab Sensors Tab
 * The AVR32 UC3B Control Panel PC Demo shows the states of all the sensors of the EVK1100:
 *   - 1 temperature sensor (0 to 100%)
 *   - 1 light sensor (0 to 100%)
 *   - 2 pushbuttons
 *   - 1 joystick
 *   - 1 accelerometer (reports X and Y angles)
 * \n
 * \n
 * \image html install_7.jpg "AVR32 UC3B Control Panel Firmware"
 * \n
 * \subsection actuatorsTab Actuators Tab
 * The actuator tab shows the state of the LEDS. User have the possibility to control each of them (note
 * that their intensity is adapted in respect of the light sensor when tou go back to the 'sensor tab').
 *
 * \section files Main Files
 * - main.c : demo main file
 * - device_ctrl_panel_task.c : high level USB device control panel task
 * - device_ctrl_panel_task.h : high level USB device control panel task header
 * - device_mass_storage_task.c : high level USB device mass storage task (note that this
 * feature is disabled in the current release of the demo)
 * - device_mass_storage_task.h : high level USB device mass storage task header
 * - usb_descriptors.c : contains the USB parameters that uniquely identify the USB application through descriptor tables
 * - usb_descriptors.h : contains the USB definitions that uniquely identify the USB application through descriptor tables
 * - usb_specific_request.c : Processing of USB device specific enumeration requests.
 * - usb_specific_request.h : Processing of USB device specific enumeration requests.
 * - FreeRTOSConfig.h : Configuration file of the FreeRTOS kernel
 * - s_accelerometer.c : control panel module managing accelerometer
 * - s_joystick.c : control panel module managing joystick
 * - s_light.c : control panel module managing light sensor
 * - s_pushb.c : control panel module managing push button
 * - s_temperature.c : control panel module managing temperature sensor
 * - cpled.c : control panel module managing leds
 *
 * \section knownIssues Known issues
 * When the PC appli is running, if a UC3B device reset occurs, then the PC application
 * "-> select target -> EVK1101" is not working anymore.
 *
 * <HR>
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/products/AVR32/">Atmel AVR32</A>.\n
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
