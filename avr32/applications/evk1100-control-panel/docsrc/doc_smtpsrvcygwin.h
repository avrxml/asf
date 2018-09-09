/*****************************************************************************
 *
 * \file
 *
 * \brief cygwin SMTP server installation & usage.
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

/*! \page SmtpServerCygwin Installation & configuration of exim (the cygwin SMTP server) on a PC
 * exim is an SMTP server [a GNU (GPL) software, included with Cygwin]. The following
 * sections assumes that cygwin is already installed on your machine (as part of
 * the AVR32 gnu toolchain).
 *
 * \section EximInstall 1) exim installation
 * - Login to the Windows machine as Administrator, or as a user with administrator
 * privilege. Make sure the user has a login password already created.
 * - Download the setup.exe program from http://www.cygwin.com/.
 * - Run the newly downloaded setup.exe program.
 * - Select and install to your already installed cygwin directory the following
 * Bin packages only : <b>exim</b> and <b>cygrunsrv</b>.
 *
 *
 * \section EximConfig 2) exim configuration
 * - Open a Cygwin window (by double clicking the  icon), a black screen pops up, type exim-config. Answer the prompts as follows:
 *    - When asked for "local postmaster", type "Administrator" or type a user name with Administrator privilege.
 *    - When asked for "fully qualified primary hostname", you can accept the default or type in your machine's FQDN. (i.e. mail.mydomain.com)
 *    - When asked to "install the exim daemon as a service", answer "yes".
 *    - When asked to "Enter the period of the queue runner", answer "15".
 *    - When asked "Do you want the daemon to run with ntsec", answer "no".
 *    - When asked "Do you want to start the exim daemon as a service now", answer "yes".
 * At this point, the Exim Mail Transfer Agent service (exim) should be running!
 *
 * - Now let's check to make sure the exim (SMTP server) is running, type:\n
 * <b><i>\verbatim netstat -an | grep  25\endverbatim </i></b>\n
 * You should see a response like:\n
 * <b><i>\verbatim TCP    0.0.0.0:25         0.0.0.0:0          LISTENING\endverbatim </i></b>
 *
 *
 * \section EximAdvancedConfig 3) exim advanced configuration
 * - To allow all internal / networked workstations within your network to use exim as an outbound SMTP server, we need to perform several tasks. The first task is to configure the firewall (if installed as part of Windows XP Service Pack 2 or higher) to allow traffic to pass through.\n
 * \n
 * Invoke the "Control Panel" and select the "Security Center" applet. If the firewall is not enabled (NOT MONITORED), then there is no need to continue with this section. If however, you have the firewall enabled, click the "Windows Firewall" in the "Manage Security Sections for:" section. In the "Exceptions" tab, click the "Add Port" button. In the dialog box, use a name of "exim" and the Port Number "25". Also, make sure to check the TCP radio button. The "Scope" of this rule should be set to "Any computer" by default.
 *
 * - Another task that needs to be completed in order to allow all internal / networked workstations within your network to use exim as an outbound SMTP server, is to edit the file c:\\cygwin\\etc\\exim.conf. Open this file in an editor and modify the following:
 *    - Find the line:\n
 *      <i>hostlist  relay_from_hosts = 127.0.0.1</i>\n
 *      and change it to\n
 *      <b><i>hostlist  relay_from_hosts = 127.0.0.1 : 192.168.0.0/24</i></b>\n
 *      \n
 *      The above modification tells exim to accept emails (for sending) from any workstation with an IP address of 192.168.0.xx. This line could be read as "relaying mail for the subnet 192.168.0.0/24". <b>Please keep in mind that your local network subnet number may be different.</b>\n
 *\n
 *    - Find the line:\n
 *      <i>host_lookup = !127.0.0.1/8:*</i>\n
 *      and change it to\n
 *      <b><i># host_lookup = !127.0.0.1/8:*</i></b>\n
 *\n
 *    - Find the line:\n
 *      <i>rfc1413_query_timeout = 30s</i>\n
 *      and change it to\n
 *      <b><i>rfc1413_query_timeout = 1s</i></b>\n
 *\n
 *    - Find the line:\n
 *      <i>require  verify = sender</i>\n
 *      and change it to\n
 *      <b><i># require  verify = sender</i></b>
 *
 * - After you modify and save the file c:\\cygwin\\etc\\exim.conf, restart the exim email server by typing:\n
 *   <b><i>net stop exim</i></b>\n
 *   then\n
 *   <b><i>net start exim</i></b>
 *
 *
 * \section EximUsage 4) exim usage
 * To start and stop the exim service, respectively use:\n
 * <b><i>net start exim</i></b>\n
 * and\n
 * <b><i>net stop exim</i></b>
 *
 * <HR>
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.microchip.com/design-centers/32-bit">Atmel AVR32</A>.\n
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */
