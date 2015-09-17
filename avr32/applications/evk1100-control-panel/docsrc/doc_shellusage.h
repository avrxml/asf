/*****************************************************************************
 *
 * \file
 *
 * \brief EVK1100 Control Panel shell-like interface over USART scenario.
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

/*! \page ShellUsage Accessing the Control Panel through its shell interface on UART_0
 * In this scenario, you are a local user who wishes to access the Control Panel
 * through its UART_0 shell interface. You are equipped with a laptop. Over
 * this interface, you have administrator right-access to the Control Panel's
 * internal file system (read, write, execute), and you have access to several
 * functions that interact with sensors & actuators.
 *
 * \section ShellConnectingToUart0 1) Connecting to the shell.
 *    - With a male-female serial cable, join your laptop COM port to the Control
 * Panel's UART_0 COM port.
 *
 *    - On your laptop, launch hyperterminal:\n
 * \n
 * \image html ShellLaunchHyperT.gif
 * \n
 *
 *    - Name the new connection, choose any icon:\n
 * \n
 * \image html ShellHyperTNewConnDescription.gif
 * \n
 *
 *    - Select COM1 (or rather the identifier of the COM port you chose on your laptop):\n
 * \n
 * \image html ShellHyperTChooseCom.gif
 * \n
 *
 *    - Fill-in the following properties to match the Control Panel's UART_0 settings
 * (Bits per second=57600, Data bits=8, Parity=None, Stop bits=1, Flow control=None):\n
 * \n
 * \image html ShellHyperTComProperties.gif
 * \n
 * Click on Apply then on OK. Your hyperterminal is now configured.
 *
 *    - Type enter in Hyperterminal; the invite prompt <b><i>$></i></b> of the
 * shell should appear:\n
 * \n
 * \image html ShellHyperTPrompt.gif
 * \n
 *
 * \section ShellAvailableCmds 2) Listing all available commands.
 * Type <b><i>help</i></b> followed by enter: this will execute the help command
 * that lists all available commands and a short description for each:\n
 * \n
 * \image html ShellHelpOutput.gif
 * \n
 * Note that the prompt <b><i>$></i></b> appears again after completion of the
 * command, waiting for the next command.
 * There are three main families of function:
 *    - one family deals with the actuators and the sensors (get/set an actuator
 * current value, get/set the configuration of a sensor, get a sensor current value),
 *    - another family deals with the Control Panel file system (operations on files
 * and directories),
 *    - another family deals with some system configuration variables (e.g. Control
 * Panel's IP address, HTTP port).
 *
 * \section ShellExampleActuatorCmd 3) An example of an actuator command : write a string to the LCD user message zone.
 * Type on the shell: <b><i>set_actuator_value actuator=lcd usrmsg="Hello Goodbye!" time=10</i></b>
 * This command writes the string "Hello Goodbye!" to the LCD user message zone
 * (i.e. the 4th line) in 10 seconds from the moment you hit enter. The "time=10"
 * argument is optional.
 *
 * \section ShellExampleSensorCmd 4) An example of a sensor command : get the current temperature.
 * Type on the shell: <b><i>get_sensor_value sensor=temp</i></b>
 * Once enter is hit, this command will display on the next line of the shell the
 * current temperature log. Example:\n
 * \n
 * \image html ShellGetTempVal.gif
 * \n
 * The above displayed log is made of three fields: the sensor name, the current date, the sensor value.
 *
 * \section ShellExampleFileSystemCmds 5) Examples of file system commands.
 *   - Suppose we want to see the content of the /WEB directory(i.e. the Web server files):
 *      - The Control Panel supports multiple drives. By default, there is only one available
 * drive, that is the dataflash. Let's display the available drives:\n
 * \n
 * \image html ShellDfCmd.gif
 * \n
 * We see that we currently have only one drive, the drive a.
 *      - Mount the a drive:\n
 * \n
 * \image html ShellMntCmd.gif
 * \n
 *      - Display the current content of the top / directory on the a drive:\n
 * \n
 * \image html ShellLs1Cmd.gif
 * \n
 *      - Go to the WEB directory: type cd WEB then enter. Then display the
 * content of the WEB directory.\n
 * \n
 * \image html ShellLs2Cmd.gif
 * \n
 * We can see that the WEB directory contains 10 files and one sub-directory.
 *
 *   - \anchor ShellCreatingLogDir Suppose we want to activate the logs activity of the Control Panel:
 * As long as the /LOG directory doesn't exist, the Control Panel doesn't save
 * the data logs into log files (all log files are saved into this directory).
 *      - Mount the a drive:\n
 *      - Display the current content of the top / directory on the a drive:\n
 * We can see that the LOG directory doesn't exist. Let's create it.
 *      - Create the LOG directory(with the mkdir command):\n
 * \n
 * \image html ShellCreateLogDir.gif
 * \n
 * The next ls command shows that the LOG directory has been created.
 *      - Let's wait a little while and then list the content of the LOG directory.
 * At least one log file should have been created : the Control Panel has started
 * logging data logs.\n
 * \n
 * \image html ShellLogDirLs.gif
 * \n
 * The file listed is a data log file. Let's list its current content (with the
 * cat command).Here is an excerpt of the output of the <b><i>"cat t021307_0901.log"</i></b>
 * command:\n
 * \n
 * \image html ShellCatLog.gif
 * \n
 * We can see that a data log file is made of several sensors and actuators logs.
 *
 * <HR>
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/products/AVR32/">Atmel AVR32</A>.\n
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
