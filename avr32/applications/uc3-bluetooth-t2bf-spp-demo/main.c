/**
 * \file
 *
 * \brief AVR UC3 Bluetooth Cybercom Demo.
 *
 * Copyright (c) 2011-2018 Microchip Technology Inc. and its subsidiaries.
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
 * This is the documentation for the Cybercom Bluetooth demo. The setup of the
 * demo is explained in the getting started
 * of the bluetooth extension board (ATEXTBT).
 * <b>Operating mode: </b>
 * - Refer to the <A href="http://www.microchip.com/developmenttools/productdetails/thw1008">ATEXTBT</b></A>
 * \section files Main Files
 * - main.c: Main File
 * - appEngine_t2bf_atmel.c: The application engine for the t2BF platform
 *
 * \section compilinfo Compilation Information
 * This software is written for GNU GCC for AVR32 and for IAR Embedded Workbench
 * for Atmel AVR32. Other compilers may or may not work.
 *
 * \section deviceinfo Device Information
 * All UC3 devices can be used.
 *
 * \section configinfo Configuration Information
 * This example has been tested with the following configuration:
 * - UC3C_EK, EVK1104 and EVK1105 evaluation kit;
 *
 * \section Connection Information
 * To connect EXTBT to UC3C_EK, EVK1104, EVK1105, please refer to :
 * http://ww1.microchip.com/downloads/en/AppNotes/doc32153.pdf.
 *
 * \section Known Issues
 * The connection Bluetooth may be failed if the connection is initiated from
 * the board. The state changes to "connected" too early even if the
 * connection is not completed on the PC. If the connection is initiated by
 * the PC, the connection will succeed in all cases. Please refer to
 * http://ww1.microchip.com/downloads/en/AppNotes/doc32153.pdf.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.microchip.com/design-centers/8-bit">Atmel AVR</A>.\n
 */
/*
*******************************************************************************
*                     (c) Cybercom Sweden West AB 2009                        *
*******************************************************************************

   File name:     app.c
   Author:        Thomas Bredhammar, Daniel Persson
   Description:   The application executes the application engine.
   Compiler:      IAR EWAVR32 for AVR32

*******************************************************************************
*/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#include  <includes.h>

#include  "board.h"
#include  "power_clocks_lib.h"
#include  "appEngine_t2bf_atmel.h"

#include  "gui.h"
#include  "controller.h"

#include  "delay.h"
/*
*******************************************************************************
*                              VARIABLES
*******************************************************************************
*/

OS_STK  AppTaskStk[APP_TASK_STK_SIZE];
extern unsigned short display;
/*
********************************************************************************
*                           FUNCTION PROTOTYPES
********************************************************************************
*/

int  main(void);
static  void  app_task(void *p_arg);

static int app_engine(void);

/*$PAGE*/
/*
********************************************************************************
*                                 MAIN
********************************************************************************
*/

int  main (void)
{
#if (OS_TASK_NAME_SIZE > 7) && (OS_TASK_STAT_EN > 0)
    CPU_INT08U  err;
#endif

	// Add delay to ensure EXTBT module is correctly reset.
#if UC3C
	cpu_delay_ms(AVR32_SCIF_RCOSC_FREQUENCY, 1000);
#else
        cpu_delay_ms(AVR32_PM_RCOSC_FREQUENCY, 1000);
#endif

	CPU_IntDis(); /*Disable all interrupts */

  	sysclk_init();

	OSInit();

	OSTaskCreateExt(app_task,(void *)0,
		(OS_STK *)&AppTaskStk[APP_TASK_STK_SIZE-1], APP_TASK_START_PRIO,
		APP_TASK_START_PRIO,(OS_STK *)&AppTaskStk[0], APP_TASK_STK_SIZE,
		(void *)0, OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

#if (OS_TASK_NAME_SIZE > 7) && (OS_TASK_STAT_EN > 0)
    OSTaskNameSet(APP_TASK_START_PRIO, (CPU_CHAR *)"Startup", &err);
#endif

	OSStart();

	return (0);
}

/*$PAGE*/
/*
********************************************************************************
*                        APPLICATION TASK
*
* Description: This task initiates the BSP and executes the application engine.
*
* Arguments: p_arg   is the argument passed to 'app_task()' by 'OSTaskCreate()'.
*
* Note(s)  : 1) The first line of code is used to prevent a compiler warning
*  because 'p_arg' is not used.  The compiler should not generate any code for
*  this statement.
********************************************************************************
*/

static  void  app_task (void *p_arg)
{
	(void)p_arg;

	BSP_Init();       /* Initialize BSP functions */

#if OS_TASK_STAT_EN > 0
	OSStatInit();     /* Determine CPU capacity */
#endif

	OSTimeDlyHMSM(0, 0, 0, 200);

	app_engine(); /* This function never returns */
	while (true) {
		/* Just in case of errouneous return, an infinite loop */
		OSTimeDlyHMSM(0, 0, 0, 200);
	}
}

/*!
 * \brief init function : do init and loop (poll if configured so)
 */
static int app_engine(void)
{

	// configure GUI
	gui_init(CPU_CLK_FREQ(),CPU_CLK_FREQ(),CPU_CLK_FREQ(),PBA_CLK_FREQ());

	// configure controller
	controller_init(CPU_CLK_FREQ(),CPU_CLK_FREQ(),CPU_CLK_FREQ(),
		PBA_CLK_FREQ());

	// initializes t2BF
	AppEngineInit( gui_callback, "Atmel Spp Lite" );

	/* do a loop */
	while (true) {
		OSTimeDlyHMSM(0, 0, 0, 200);

		controller_task();

		if (controller_inquiry() == true) {
			AppEngineInquiryButton( );
		}
		else if (controller_select() == true) {
			AppEngineSelectDeviceButton( );
		}
		else if (controller_connect() == true) {
			AppEngineConnectButton( );
		}
		else if (controller_send() == true) {
			AppEngineJoystickButton( AppEngine_Left );
		}

		if (display) {
			OSTimeDlyHMSM(0, 0, 0, 200);
			// A delay so that it is humanly possible to see the
			OSTimeDlyHMSM(0, 0, 0, 200);
			// character(s) before they are cleared
			display = 0;
		}
	}
}
