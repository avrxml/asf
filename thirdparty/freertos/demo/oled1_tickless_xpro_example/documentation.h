/**
 * \file
 *
 * \brief FreeRTOS demo main documentation page.
 *
 * Copyright (c) 2013-2018 Microchip Technology Inc. and its subsidiaries.
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

#ifndef DOCUMENTATION_H_INCLUDED
#define DOCUMENTATION_H_INCLUDED

/**
 * \mainpage FreeRTOS tickless demo using OLED1 Xplained Pro
 *
 * \section appdoc_overview Overview
 * - \ref appdoc_intro
 * - \ref appdoc_compinfo
 * - \ref appdoc_continfo
 *
 *
 * \section appdoc_intro Introduction
 *
 * This application demonstrates basic use of the
 * <a href="http://www.freertos.org">FreeRTOS</a> real-time operating system
 * on SAM D devices. More specifically, it demonstrates use of tasks, queues
 * and mutexes (semaphores). In addition the tickless feature, which allows
 * longer sleep periods by shutting down the OS tick when it is not needed,
 * can be toggled on or off to compare power consumption.
 *
 * The application is designed to run on a SAM D Xplained Pro board with a
 * OLED1 Xplained Pro wing board connected to EXT port.
 *
 * After start-up, the application will display a pseudo-random graph that is
 * continuously updated on the OLED, along with a menu bar at the bottom. The
 * menu bar shows the selection of screens that the user has. By pressing the
 * corresponding buttons on the OLED1 Xplained Pro wing board, the user can
 * select between:
 * - "Graph": pseudo-random graph.
 * - "Term.": text received from terminal, i.e., the EDBG Virtual COM Port.
 * - "About": short description of the application and use of terminal-screen.
 *   This also toggles tickless operation on or off.
 *
 * Further, the LEDs on the OLED1 Xplained Pro wing board are lit up for the
 * duration of the various tasks' loops, simply to give a visual representation
 * of the task switching:
 * - LED1: update of graph and handling of incoming terminal characters.
 * - LED2: printing terminal and about text.
 * - LED3: checking user selection, handling display buffer and menu drawing.
 *
 * Note that several LEDs can be lit up at one time if one task is waiting for
 * another task to release a resource. In this application, the resources would
 * be the mutexes for the display and terminal text buffer.
 *
 * To write text to the terminal window, one must connect to the EDBG Virtual
 * COM Port with a terminal emulator at a baud rate of 9.6 kbaud. The demo will
 * echo back the received characters.
 *
 * For more details on the application:
 * - \ref freertos_sam0_demo_tasks_group
 * - \ref oled1_xpro_io_group
 * - \ref edbg_cdc_rx_group
 *
 * For details on FreeRTOS and its API, go to the
 * <a href="http://www.freertos.org">official FreeRTOS website</a>.
 *
 *
 * \section appdoc_compinfo Compiler information
 * This software has been tested with the GNU GCC.
 * Other compilers may or may not work.
 *
 *
 * \section appdoc_continfo Contact information
 * For further information, visit <a href="http://www.microchip.com/">Microchip</a>.
 */

#endif // DOCUMENTATION_H_INCLUDED
