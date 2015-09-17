/*****************************************************************************
 *
 * \file
 *
 * \brief FreeRTOS Configuration file for the FreeRTOS AVR32 UC3 example.
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
 ******************************************************************************/


#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include "board.h"


/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
 *
 * See http://www.freertos.org/a00110.html.
 *----------------------------------------------------------*/

#define configUSE_PREEMPTION      1
#define configUSE_IDLE_HOOK       0
#define configUSE_TICK_HOOK       0
#if (BOARD == UC3L_EK)
	#define EXAMPLE_TARGET_MCUCLK_FREQ_HZ   12000000  // MCU clock target frequency, in Hz
	#define EXAMPLE_TARGET_PBACLK_FREQ_HZ   12000000  // PBA clock target frequency, in Hz

	#define configCPU_CLOCK_HZ        ( EXAMPLE_TARGET_MCUCLK_FREQ_HZ ) /* Hz clk gen */
	#define configPBA_CLOCK_HZ        ( EXAMPLE_TARGET_PBACLK_FREQ_HZ )
#else
	#define configCPU_CLOCK_HZ        ( FOSC0 ) /* Hz clk gen */
	#define configPBA_CLOCK_HZ        ( FOSC0 )
#endif
#define configTICK_RATE_HZ        ( ( portTickType ) 1000 )
#define configMAX_PRIORITIES      ( ( unsigned portBASE_TYPE ) 8 )
#define configMINIMAL_STACK_SIZE  ( ( unsigned portSHORT ) 256 )
/* configTOTAL_HEAP_SIZE is not used when heap_3.c is used. */
#define configTOTAL_HEAP_SIZE     ( ( size_t ) ( 1024*25 ) )
#define configMAX_TASK_NAME_LEN   ( 16 )
#define configUSE_TRACE_FACILITY  0
#define configUSE_16_BIT_TICKS    0
#define configIDLE_SHOULD_YIELD   1

/* Co-routine definitions. */
#define configUSE_CO_ROUTINES     0
#define configMAX_CO_ROUTINE_PRIORITIES ( 1 )

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */

#define INCLUDE_vTaskPrioritySet            1
#define INCLUDE_uxTaskPriorityGet           1
#define INCLUDE_vTaskDelete                 1
#define INCLUDE_vTaskCleanUpResources       0
#define INCLUDE_vTaskSuspend                1
#define INCLUDE_vTaskDelayUntil             1
#define INCLUDE_vTaskDelay                  1
#define INCLUDE_xTaskGetCurrentTaskHandle   0
#define INCLUDE_xTaskGetSchedulerState      1

/* configTICK_USE_TC is a boolean indicating whether to use a Timer Counter or
   the CPU Cycle Counter for the tick generation.
   Both methods will generate an accurate tick.
   0: Use of the CPU Cycle Counter.
   1: Use of the Timer Counter (configTICK_TC_CHANNEL is the TC channel). */
#define configTICK_USE_TC             0
#define configTICK_TC_CHANNEL         2

/* configHEAP_INIT is a boolean indicating whether to initialize the heap with
   0xA5 in order to be able to determine the maximal heap consumption. */
#define configHEAP_INIT               0

// #define configUSE_MUTEXES                   0
// #define configUSE_RECURSIVE_MUTEXES         0
// #define configUSE_COUNTING_SEMAPHORES       0
// #define configUSE_ALTERNATIVE_API           0
// #define configCHECK_FOR_STACK_OVERFLOW      0
// #define configQUEUE_REGISTRY_SIZE           10
// #define configGENERATE_RUN_TIME_STATS       0

// #define INCLUDE_vResumeFromISR              1
// #define INCLUDE_uxTaskGetStackHighWaterMark 0

#endif /* FREERTOS_CONFIG_H */
