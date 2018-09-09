/**
 *
 * \file
 *
 * \brief Provision AP with BLE example.
 *
 * Copyright (c) 2018 Atmel Corporation. All rights reserved.
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

/** \mainpage
 * \section intro Introduction
 * This example demonstrates how to provision Wi-Fi via BLE using the WILC3000
 * Wi-Fi/BLE module. <br>
 * It uses the following hardware:
 * - SAMV71 Xplained Ultra.
 * - The WILC3000 module.
 * - Thirdparty Android device.
 *
 * \section files Main Files
 * - main.c : Initialize the SAM board and create BT and Wifi task.
 *
 * \section usage Usage
 * -# To connect the SAM board to the internet Access Point over Wi-Fi, the user must
 * -# provide credentials (SSID and passphrase) over BLE. As a first step, it is required
 * -# that the user install the Atmel BLE provision application on an Android device.
 * -# Then, power up the SAM board and run the Android application: perform a scan, select
 * -# an SSID and enter the passphrase.
 * -# Finally, press provision to connect.
 * \endcode
 *
 */
#include "asf.h"
#include "console_serial.h"
#include "conf_board.h"
#include "conf_clock.h"
#include "btstack_chipset_atwilc3000.h"
#include "btstack_debug.h"
#include "btstack_memory.h"
#include "btstack_run_loop.h"
#include "btstack_run_loop_freertos.h" 
#include "classic/btstack_link_key_db.h"
#include "hci.h"
#include "hci_dump.h"
#include "le_prov.h"
#include "lwip/ip.h"
#include "lwip/api.h"
#include "lwip/tcpip.h"
#include "lwip/dns.h"
#include "os/include/net_init.h"
#include "os/include/m2m_wifi_ex.h"
#include "tinyservices.h"
#include "conf_wilc.h"
#include "wifi_prov.h"
 
#define TASK_BT_STACK_SIZE				   ((2048*2)/sizeof(portSTACK_TYPE))
#define TASK_BT_STACK_PRIORITY			   (2)

#define TASK_WIFI_STACK_SIZE			   (4096 / sizeof(portSTACK_TYPE))
#define TASK_WIFI_STACK_PRIORITY		   (3)


extern void vApplicationStackOverflowHook(xTaskHandle *pxTask, signed char *pcTaskName);
extern void vApplicationTickHook(void);
extern void vApplicationMallocFailedHook(void);
extern void xPortSysTickHandler(void);

/**
 * \brief This function is called on every Sys tick
 */
void SysTick_Handler(void)
{
	xPortSysTickHandler();
}

/**
 * \brief Called if stack overflow during execution
 */
extern void vApplicationStackOverflowHook(xTaskHandle *pxTask,
		signed char *pcTaskName)
{
	/* If the parameters have been corrupted then inspect pxCurrentTCB to
	 * identify which task has overflowed its stack.
	 */
	for (;;) {
	}
}

/**
 * \brief This function is called by FreeRTOS each tick
 */
extern void vApplicationTickHook(void)
{
}

extern void vApplicationMallocFailedHook(void)
{
	/* Called if a call to pvPortMalloc() fails because there is insufficient
	free memory available in the FreeRTOS heap.  pvPortMalloc() is called
	internally by FreeRTOS API functions that create tasks, queues, software
	timers, and semaphores.  The size of the FreeRTOS heap is set by the
	configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */

	/* Force an assert. */
	configASSERT( ( volatile void * ) NULL );
}

int main(void)
{
	/* Initialize the SAM system */
	sysclk_init();
	board_init();

	/* Initialize serial console */
	serial_console_init();
	
	printf("\r\nBTstack on SAMV71 Xplained Ultra with ATWILC3000\n");
	
	/* Create semaphore for scanning procedure */	
	vSemaphoreCreateBinary(wifi_bt_if_sem);
	
	/* Create semaphore for connection procedure */
	vSemaphoreCreateBinary(wifi_bt_conn_if_sem);
	
	/* Create wifi task */
	if(xTaskCreate(wifi_task, (const void *)"Wifi_task", TASK_WIFI_STACK_SIZE, 0, TASK_WIFI_STACK_PRIORITY, &wifi_task_handle) != pdPASS)
	{
		printf("\r\nFailed to create wifi task\n");
	}
	
	/* Create bt task */
	if (xTaskCreate(bt_task, "bt_task", TASK_BT_STACK_SIZE, NULL, TASK_BT_STACK_PRIORITY, &bt_task_handle) != pdPASS)
	{
		printf("\r\nFailed to create bt task\n");
	}
	
	/* Start the scheduler. */
	vTaskStartScheduler();
	
    while(1);
}

