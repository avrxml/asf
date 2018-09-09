/**
 * \file
 *
 * \brief AVR UC3C Scope Meter and Filtering Demo
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
 */
/*! \mainpage
 * \section intro Introduction
 * This is the documentation for UC3C-EK Scope Meter and Filtering demo running
 * on the UC3C_EK development kit.
 * - Step 1: Connect J34 (1-2), J35 (1-2) and J38 (3-4) headers.
 *   Turn potentiometer P1 to have the arrow vertical (It is the trigger level).
 * - Step 2: Plug a USB cable from the USB_VC connector (J27) to one of the PC's
 *   USB ports. This provide power and USB serial communications port.
 * - Step 3: Load and run this application.
 * - Step 4: A sine wave is generated (takes part of the DAC task generation).
 * - Step 5: This signal is acquired (takes part of the ADC task acquisition).
 * - Step 6: A digital noise noise (controller by the push buttons PB0 and PB1)
 *   is then added to the signal
 * - Step 7: This noise is then filtered using a FIR filter in floating and
 *   fixed point format (takes part of the FILTER task).
 * - Step 8: All the signals are displayed on the LCD (takes part of the GUI
 *   task acquisition)
 * - Step 9: All the signals are sent to PC application (takes part of the
 *   REMOTE Task)
 * <b>Operating mode: </b>
 *   The description and the installation of the PC application, the connection
 *   of the UC3C-EK to the PC are explained in AVR32831 application note.
 *   This application note is available on www.atmel.com.
 * \section files Main Files
 * - main.c: Main File;
 * - adc_task.c: ADC Task Management;
 * - dac_task.c: DAC Task Management;
 * - noise_task.c: Noise Task Management;
 * - filter_task.c: Filter Task Management;
 * - remote_task.c: Remote Task Management;
 * - controller.c: QTouch Controller Management;
 * - gui.c: GUI Display Management;
 *
 * \section compilinfo Compilation Information
 * This software is written for GNU GCC for AVR32 and for IAR Embedded Workbench
 * for Atmel AVR32. Other compilers may or may not work.
 *
 * \section deviceinfo Device Information
 * All AVR32 AT32UC3C devices can be used.
 *
 * \section configinfo Configuration Information
 * This example has been tested with the following configuration:
 * - UC3C_EK evaluation kit;
 * - CPU clock: 16 MHz;
 * - USART2 (on UC3C_EK) abstracted with a USB CDC connection to a PC;
 * - PC terminal settings:
 *   - 115200 bps,
 *   - 8 data bits,
 *   - no parity bit,
 *   - 1 stop bit,
 *   - no flow control.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.microchip.com/design-centers/8-bit">Atmel AVR</A>.\n
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#include "compiler.h"
#include "board.h"
#include "power_clocks_lib.h"
#include "gpio.h"
#include "flashc.h"
#include "sysclk.h"
#include "sleepmgr.h"
#include "gui.h"
#include "controller.h"
#include "adc_task.h"
#include "dac_task.h"
#include "noise_task.h"
#include "filter_task.h"
#include "remote_task.h"
#include "conf_demo.h"
#include "conf_at42qt1060.h"


/*! \brief Main File Section:
 *          - Initialization (CPU, Controller Task,... )
 *          - Main loop with task management (ADC, DAC, CAN and GUI)
 */
int main(void)
{
	irq_initialize_vectors();

	/* Initialize the board.
	 * The board-specific conf_board.h file contains the configuration of
	 * the board initialization.
	 */
	board_init();

	/* Initialize the clocks.
	 * The clock-specific conf_clocks.h file contains the configuration of
	 * the clocks initialization.
	 */
	sysclk_init();

	// Initialize the sleep manager
	sleepmgr_init();
	sleepmgr_lock_mode(SLEEPMGR_IDLE);

        /* Initialize the required Task.
	 * - Initialize the DAC task to start the signal generator,
	 * - Initialize the ADC task to start the scope acquisition,
	 * - Initialize the Noise Task to add digital noise to the signal,
	 * - Initialize the Filter Task to remove the digital noise of the signal,
	 * - Initialize the GUI Task to display signals as a scope on the LCD,
	 * - Initialize the Remote Task to display signals as a scope on the LCD,
	 */
	dac_task_init();
	adc_task_init();
	noise_task_init();
	filter_task_init();
	gui_task_init();
	controller_task_init();
	remote_task_init();

	cpu_irq_enable();

	// Free running scheduler loop
	while (true) {
		// Enter Sleep Mode
		sleepmgr_enter_sleep();
		// Call ADC task
		adc_task();
		// Call DAC task
		dac_task();
		// Call Noise task
		noise_task();
		// Filter Task
		filter_task();
		// Call Gui task for update
		gui_task();
		// Call Controller Task for control Update
		controller_task();
		// Send data to the PC Application
		remote_task();
	}
}
