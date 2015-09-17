/*****************************************************************************
 *
 * \file
 *
 * \brief EVK1100 Control Panel Web server scenario.
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

/*! \page EthWebServer Accessing the Control Panel acting as a Web server
 * In this scenario, you are a local user who wishes to access the Control Panel
 * through its Web-server interface. You are equipped with a PC.
 *
 * \section EthPcConfig 1) PC configuration [Windows XP]
 * Configure your PC Ethernet connection IP address where the EVK1100 board will
 * be plugged to.
 * - Select Start->Settings->Network Connections \n
 * \n
 * \image html EthPcSetting1.jpg
 * \n
 * - Right-click on the network connection where the EVK1100 will be plugged to and
 * select \b Properties \n
 * \n
 * \image html EthPcSetting2.gif
 * \n
 * - Select the item <b><i>"Internet Protocol(TCP/IP)"</i></b> then click on the Properties button \n
 * \n
 * \image html EthPcSetting3.gif
 * \n
 * - Fill-in the IP address and subnet mask as pictured below then record your TCP/IP
 * settings by clicking OK. \n
 * \n
 * \image html EthPcSetting4.gif
 * \n
 * - Record your network connection settings by clicking OK. \n
 * \n
 * \image html EthPcSetting3.gif
 *
 * \section EthPlugEvk1100 2) Connect your EVK1100 board to your PC.
 * Connect the EVK1100 board to the previously configured Ethernet port of your PC.
 * The LCD screen should now display the Control Panel IP address on the second line.\n
 * \n
 * \image html EthIpAddrDisp.gif
 * \n
 * To easily check that the board is reachable from the PC, we may ping the Control
 * Panel. To do that, open a DOS Command Prompt window and type "ping ipaddress".
 * For example, if the board's IP address (i.e. the one that is displayed on the
 * second line of the LCD) is 192.168.0.2, type <b><i>"ping 192.168.0.2"</i></b>.
 * You should then get replies from the board as pictured below.\n
 * \n
 * \image html DosPing.gif
 * \n
 *
 * \section EthWebBrowser 3) Access the Control Panel Web-server.
 * Launch your favorite web browser. Type the Control Panel IP address in your
 * browser's address bar. The Control Panel default IP address is <em>192.168.0.2</em>.
 * If this address has previously been changed, the IP address of the Control Panel
 * will always be displayed on the second line from the top of the EVK1100 LCD screen,
 * as soon as the board is connected to some Ethernet network.
 * \note There are other ways to know the Control Panel IP address as described in
 * the scenarios \ref ShellUsage and \ref UsbCpMassStorageDevice.
 *
 * The Web-server entry page should appear. Here is an example of the Control
 * Panel's Web server entry page: \n
 * \n
 * \image html EthWebServer1.gif
 * \n
 *
 * - The sensors page shows the current value of each sensor managed by the Control Panel.
 * You may, for example, put a finger on the temperature sensor and see the change on the
 * sensor page in real-time.
 * Here's an example of the Control Panel Web server sensor page: \n
 * \n
 * \image html EthWsSensorPage.jpg
 * \n
 *
 * - The actuators page allows to set some actuators of the board the Control Panel is running on.
 * You may, for example, display a message on the LCD screen or set/clear a led
 * directly from the actuators web page.\n
 * Example : type a message between double quotes on the LCD zone of the actuators page then
 * click on submit:\n
 * \n
 * \image html EthWsActPage.jpg
 * \n
 * You shall then see the 4th line of the LCD screen display the message:\n
 * \n
 * \image html EthWsActLcdDisp.gif
 * \n
 *
 * - The <b><i>"Sensors Configuration"</i></b> page is used to set the log rate of some sensors, to
 * set the min and max value upon which the Control Panel will generate an alarm, and
 * to toggle the activation of the alarm for each sensor.\n
 * Here is an excerpt of the <b><i>"Sensors Configuration"</i></b> page:\n
 * \n
 * \image html EthWsSensorcPage.jpg
 * \n
 *
 * - The <b><i>"System Configuration"</i></b> page is used to set system level configuration variables.
 * Note that most of the settings on this page will require a reset of the Control
 * Panel to be taken into account.
 *
 * <HR>
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/products/AVR32/">Atmel AVR32</A>.\n
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
