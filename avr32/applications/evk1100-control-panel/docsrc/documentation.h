/*****************************************************************************
 *
 * \file
 *
 * \brief EVK1100 Control Panel application.
 *
 * The Control Panel application is a demonstration application. Its purpose is
 * to automatically log local sensors and actuators data and events and make
 * these available through the various connectivity channels supported by the
 * AVR UC3.\n
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

/*! \mainpage
 * This manual is divided into the following sections:
 * - \ref intro "An introduction to the Control Panel Application"
 * - \ref envinfo "A short description of the Control Panel Environment"
 * - \ref QuickStart "Getting started with the Control Panel"
 * - A set of demonstration scenarios using the Control Panel\n
 *    -# \ref DefaultMenu "Using the Control Panel LCD's default menu"
 *    -# \ref EthWebServer "Accessing the Control Panel acting as a Web server"
 *    -# \ref UsbCpMassStorageDevice "Accessing the Control Panel acting as a USB Mass Storage device"
 *    -# \ref UsbCpMassStorageHost "Accessing the Control Panel acting as a USB Mass Storage host"
 *    -# \ref ShellUsage "Using the Control Panel shell-like interface"
 *    -# \ref RestoringWebServerFiles "Restoring the default file system content of the Control Panel"
 *    -# \ref EthSMTPClient "Configuring and using the Control Panel SMTP client"
 * - \ref TroubleShootingGuide "Trouble shooting guide"
 *
 * <HR>
 * \section intro An introduction to the Control Panel Application
 * \subsection intro1 Overview
 * - The Control Panel application is a demonstration application running on top of the freeRTOS.org operating system.
 * Its purpose is to log local sensors and actuators data and events(<b>data acquisition</b>) and
 * make these available through the various connectivity channels supported by the
 * AVR UC3A microcontroller. The Control Panel uses most of the UC3 software framework drivers and software services.\n
 * - The logs are \b accessible locally through USART or USB (<b>Mass Storage class</b>) or on a SD/MMC card,
 * and/or remotely through the Internet (<b>Web server</b>).\n
 * - The Control Panel is locally \b configurable through USART or USB (<b>Mass
 * Storage class</b>) or remotely configurable through the Internet (<b>Web server</b>).
 *
 * \subsection intro2 Features.
 * - Core application: <b>ADC data acquisition</b> from sensors and actuators as logs.
 * These logs are saved in text files on an embedded file system FAT to the EVK1100's
 * dataflash or to an SD/MMC card (the card must be inserted in the EVK1100 SD/MMC slot).
 * - Several services are built around this core feature to access the logs and
 * maintain the system:
 *   - an embedded <b>web server</b> allowing a remote user to consult the logs
 *   and set the actuators in real time and configure the Control Panel modules
 *   through any web browser,
 *   - a configurable <b>SMTP client</b> sending mails when the current log file becomes full and is closed or if alarms occur ,
 *   - a <b>USB Mass Storage device</b>, allowing a USB host to connect to the board
 *   and browse the Control Panel embedded file system.
 *   - a <b>USB Mass Storage host</b>, allowing to plug a Mass Storage USB key onto
 *   the board. The Control Panel then acts as a USB host and the user may perform
 *   the following tasks using the local man-machine interface:
 *     - copies its data log files to the key, in a /LOG directory,
 *     - if the USB key contains a /CFG directory, it can copy its content to the
 *     onboard file system: this is a way to change the configuration of the
 *     Control Panel (will take effect upon next reset),
 *     - if the USB key contains a /WEB directory, it can copy its content to
 *     the onboard file system: this is a way to update the web server files.
 *   - a <b>shell-like interface</b> on the USART0 (57600bps/8bit data/1 stop bit/no flow control), allowing to navigate on the
 *   Control Panel file system, to configure the system, to interface with the
 *   sensors and actuators.
 *   - a RS-232 debug output on the USART1 (57600bps/8bit data/1 stop bit/no flow control)
 *
 * \subsection introfreertos The FreeRTOS.org kernel
 * The Control Panel implementation uses the AVR&reg;32 UC3 FreeRTOS.org kernel port.
 * FreeRTOS.org is a portable, open source, mini Real Time Kernel - a free to
 * download and royalty free RTOS that can be used in commercial applications.
 * \note The AVR UC3 FreeRTOS.org kernel port and basic demonstrations is currently
 * available under the services/os/freertos folder.
 *
 * \subsection introlwip The lwIP TCP/IP stack
 * The Control Panel implementation uses the lwIP TCP/IP stack ported on the FreeRTOS.org
 * kernel and on the AVR UC3A family of micro-controllers.
 * lwIP is an implementation of the TCP/IP protocol suite. The focus of the lwIP
 * TCP/IP implementation is to reduce resource usage while still having a full scale TCP.
 * - lwIP features:
 *    - IP (Internet Protocol) including packet forwarding over multiple network interfaces
 *    - ICMP (Internet Control Message Protocol) for network maintenance and debugging
 *    - UDP (User Datagram Protocol) including experimental UDP-lite extensions
 *    - TCP (Transmission Control Protocol) with congestion control, RTT estimation and fast recovery/fast retransmit
 *    - Specialized raw API for enhanced performance
 *    - Optional Berkeley-alike socket API
 *    - DHCP (Dynamic Host Configuration Protocol)
 *    - PPP (Point-to-Point Protocol)
 *    - ARP (Address Resolution Protocol) for Ethernet
 *
 * \note The AVR UC3 port of the lwIP TCP/IP stack is currently included in
 * the Control Panel source code environment (i.e. under the applications/evk1100-control-panel
 *
 *
 * \subsection introusb The ATMEL AVR UC3 USB stack
 * The Control Panel implementation uses the USB stack (see directory drivers/usbb)
 * and the Mass Storage USB class (see directory services/usb/_asf_v1/class/mass_storage).
 *
 * <HR>
 * \section envinfo Control Panel Environment
 * \subsection EVK1100
 * This demo was designed to run on the EVK1100 evaluation kit. The content of
 * the EVK1100 board is:
 *   - an Ethernet connection (RJ45),
 *   - a USB mini A/B connector (thus allowing the board to behave as a USB device
 *   or as a USB reduced-host),
 *   - 1 SD/MMC card slot,
 *   - 2 USARTS,
 *   - 8MB Dataflash,
 *   - LCD (20 columns/4 lines) [UI][ACTUATOR]
 *   - 4 monocolor LEDs, [ACTUATOR]
 *   - 2 bicolor LEDs, [ACTUATOR]
 *   - 1 potentiometer, [SENSOR]
 *   - 1 temperature sensor, [SENSOR]
 *   - 3 pushbuttons, [SENSOR]
 *   - 1 joystick, [SENSOR][UI]
 *   - 1 light sensor [SENSOR]
 *
 * \note For some elements of the board, we added a functionality tag expressed
 * between[] (e.g. the potentiometer has the SENSOR functionality while the joystick
 * has both the SENSOR and the UI(User Interface) functionality (i.e. the joystick
 * events will be logged as regular \b SENSOR events \b AND the joystick is used as
 * a menu navigator/choice \b UI device)).
 *
 * \subsection DemoTools Control Panel user's toolkit.
 * \subsubsection PowerSupply Power supply
 * The EVK1100 board is designed to be powered by a 8-20V DC power supply. The EVK1100
 * can be configured to use one of the following two power sources:
 *   - 8-20V DC power adapter connected to the power supply jack: to use this configuration,
 * set the PWR switch(the switch is near the power supply jack) to the EXT
 * position.
 *   - 5V DC power via the USB miniAB connector: to use this configuration, set
 * the PWR switch(the switch is near the power supply jack) to the USB position.\n
 *
 * \subsubsection RemoteUserToolkit Remote users toolkit
 * A remote user is a user that cannot physically interact with the Control Panel
 * (i.e. it is not in the same room as the EVK1100 board).
 * - an Ethernet cable: to communicate with the Control Panel, a remote user should
 * have a PC and run a web browser to access the Control Panel embedded web server.
 *
 * \subsubsection LocalUserToolkit Local users toolkit
 * A local user is a user that can physically interact with the Control Panel (i.e.
 * it is in the same room as the EVK1100 board).
 * - an Ethernet cable: a local user may proceed like a remote user and directly
 * plug an Ethernet cable between the Control Panel Ethernet connector and its PC
 * Ethernet port.
 * - a miniB plug to STD-A plug USB cable: to connect the Control Panel to a PC USB
 * port and have the Control Panel behave as a USB Mass Storage device.
 * - a miniA plug to STD-A Receptacle OTG cable: to connect a USB key to the Control
 * Panel and have the Control Panel behave as a USB host.
 * - an RS232 null-modem serial cable: to connect the EVK1100 USART0 DB9 connector
 * to a PC serial port; the user can then issue commands to the Control Panel on
 * a shell-like interface.
 *
 * <HR>
 * \section QuickStart Getting started with the Control Panel
 *
 * \subsection PowerUpTheBoard Power up the board.
 * Configure your AC adaptor polarity switch to match the board's. Set it to provide
 * from 9 to 12V. Plug it to the board's power supply jack. Set the board's PWR
 * switch to EXT. The LCD should then light up : the Control Panel is up & running.\n
 * \n
 * \image html LcdDefaultStartup.gif
 * \n
 *
 * \subsection LcdLinesDescription Board UI(User Interface) description
 * The board UI of the Control Panel is made with the LCD and the joystick.
 * The joystick is dedicated to act on the menu line(see below).
 * - The first line of the LCD is an arbitrary date in the format: month/day/year hour:min:sec.\n
 * - The second line is the connectivity information line: when the Control Panel
 * is plugged to an Ethernet network, its IP address is displayed on this line.
 * When the Control Panel is acting as a USB Mass Storage device or host, a sort
 * of a USB sign is displayed on the last column of this second line.\n
 * \n
 * \image html LcdIpAddrAndUsb.gif "An example of the LCD display"
 * \n
 * - The third line is the menu line : the default <b><i>"   Set as USB key   "</i></b>
 * menu message means <b><i>"press the joystick to set the Control Panel as a USB Mass Storage device"</i></b>.
 * See the scenario \ref UsbCpMassStorageDevice for related information. See the
 * scenario \ref UsbCpMassStorageHost for an advanced usage of the joystick.
 * - The fourth line is the user message zone. See the scenarios \ref EthWebServer
 * and \ref ShellUsage for examples on how to display a message on this line.
 *
 * \subsection UsingTheCp Using the Control Panel
 * From there, you can interact with the Control Panel in several ways:
 * - interaction through the Web server : see the \ref EthWebServer scenario for
 * a step-by-step description,
 * - interaction through USB:
 *    - where the Control Panel behaves as a USB key device: see the \ref
 *    UsbCpMassStorageDevice scenario for a step-by-step description, and
 *    also the \ref RestoringWebServerFiles scenario for another step-by-step
 *    example,
 *    - where the Control Panel behaves as a USB reduced host(plug a USB key to
 *    the Control Panel): see the \ref UsbCpMassStorageHost scenario for a
 *    step-by-step description,
 * - interaction through UART_0 and a shell interface: see the \ref
 * ShellUsage scenario for a step-by-step description.
 *
 * \note If for some reasons the Control Panel behavior doesn't fit with the scenarios,
 * check the \ref TroubleShootingGuide.
 *
 * <HR>
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/avr/">Atmel AVR UC3</A>.\n
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
