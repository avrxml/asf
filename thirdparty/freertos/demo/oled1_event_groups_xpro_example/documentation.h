/**
 * \file
 *
 * \brief FreeRTOS demo main documentation page.
 *
 * Copyright (C) 2014-2015 Atmel Corporation. All rights reserved.
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
 */

#ifndef DOCUMENTATION_H
#define DOCUMENTATION_H

/**
 * \mainpage FreeRTOS demo using event group feature on OLED1 Xplained Pro
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
 * on SAM D devices. More specifically, it demonstrates use of tasks, queues,
 * mutexes (semaphores) and event groups.
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
 * For further information, visit <a href="http://www.atmel.com/">Atmel</a>.
 */

#endif // DOCUMENTATION_H
