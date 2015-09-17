/**
 * \file
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
 */

/*
    FreeRTOS V6.0.5 - Copyright (C) 2010 Real Time Engineers Ltd.

    FEATURES AND PORTS ARE ADDED TO FREERTOS ALL THE TIME.  PLEASE VISIT 
    http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS tutorial books are available in pdf and paperback.        *
     *    Complete, revised, and edited pdf reference manuals are also       *
     *    available.                                                         *
     *                                                                       *
     *    Purchasing FreeRTOS documentation will not only help you, by       *
     *    ensuring you get running as quickly as possible and with an        *
     *    in-depth knowledge of how to use FreeRTOS, it will also help       *
     *    the FreeRTOS project to continue with its mission of providing     *
     *    professional grade, cross platform, de facto standard solutions    *
     *    for microcontrollers - completely free of charge!                  *
     *                                                                       *
     *    >>> See http://www.FreeRTOS.org/Documentation for details. <<<     *
     *                                                                       *
     *    Thank you for using FreeRTOS, and thank you for your support!      *
     *                                                                       *
    ***************************************************************************


    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation AND MODIFIED BY the FreeRTOS exception.
    >>>NOTE<<< The modification to the GPL is included to allow you to
    distribute a combined work that includes FreeRTOS without being obliged to
    provide the source code for proprietary components outside of the FreeRTOS
    kernel.  FreeRTOS is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
    or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
    more details. You should have received a copy of the GNU General Public
    License and the FreeRTOS license exception along with FreeRTOS; if not it
    can be viewed here: http://www.freertos.org/a00114.html and also obtained
    by writing to Richard Barry, contact details for whom are available on the
    FreeRTOS WEB site.

    1 tab == 4 spaces!
    
    ***************************************************************************
     *                                                                       *
     *    Having a problem?  Start by reading the FAQ "My application does   *
     *    not run, what could be wrong?"                                     *
     *                                                                       *
     *    http://www.FreeRTOS.org/FAQHelp.html                               *
     *                                                                       *
    ***************************************************************************

    
    http://www.FreeRTOS.org - Documentation, training, latest versions, license 
    and contact details.  
    
    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool.

    Real Time Engineers ltd license FreeRTOS to High Integrity Systems, who sell 
    the code with commercial support, indemnification, and middleware, under 
    the OpenRTOS brand: http://www.OpenRTOS.com.  High Integrity Systems also
    provide a safety engineered and independently SIL3 certified version under 
    the SafeRTOS brand: http://www.SafeRTOS.com.
*/

/*-----------------------------------------------------------
 * Implementation of functions defined in portable.h for the ARM CM4F port.
 *----------------------------------------------------------*/

/* Scheduler includes. */
#include "../../../include/FreeRTOS.h"
#include "../../../include/task.h"

#include <sysclk.h> /* ATMEL */
#ifndef __VFP_FP__
	#warning hardware floating point not enabled.
#endif

#ifndef configSYSTICK_CLOCK_HZ
	#define configSYSTICK_CLOCK_HZ configCPU_CLOCK_HZ
#endif

/* Constants required to manipulate the core.  Registers first... */
#define portNVIC_SYSTICK_CTRL_REG           (*((volatile unsigned long *)0xe000e010))
#define portNVIC_SYSTICK_LOAD_REG           (*((volatile unsigned long *)0xe000e014))
#define portNVIC_SYSTICK_CURRENT_VALUE_REG  (*((volatile unsigned long *)0xe000e018))
#define portNVIC_INT_CTRL_REG               (*((volatile unsigned long *)0xe000ed04))
#define portNVIC_SYSPRI2_REG                (*((volatile unsigned long *)0xe000ed20))
/* ...then bits in the registers. */
#define portNVIC_SYSTICK_CLK_BIT            (1UL << 2UL)
#define portNVIC_SYSTICK_INT_BIT            (1UL << 1UL)
#define portNVIC_SYSTICK_ENABLE_BIT         (1UL << 0UL)
#define portNVIC_SYSTICK_COUNT_FLAG_BIT     (1UL << 16UL)
#define portNVIC_PENDSVSET_BIT              (1UL << 28UL)
#define portNVIC_PENDSVCLEAR_BIT            (1UL << 27UL)
#define portNVIC_PEND_SYSTICK_CLEAR_BIT     (1UL << 25UL)

#define portNVIC_PENDSV_PRI        (((unsigned long) \
	configKERNEL_INTERRUPT_PRIORITY) << 16UL)
#define portNVIC_SYSTICK_PRI        (((unsigned long) \
	configKERNEL_INTERRUPT_PRIORITY) << 24UL)

/* Constants required to manipulate the VFP. */
/* Floating point context control register. */
#define portFPCCR        ((volatile unsigned long *)0xe000ef34)
#define portASPEN_AND_LSPEN_BITS    (0x3UL << 30UL)

/* Constants required to set up the initial stack. */
#define portINITIAL_XPSR               (0x01000000)
#define portINITIAL_EXEC_RETURN        (0xfffffffd)

/* The priority used by the kernel is assigned to a variable to make access
 * from inline assembler easier. */
const unsigned long ulKernelPriority = configKERNEL_INTERRUPT_PRIORITY;

/* Each task maintains its own interrupt status in the critical nesting
 * variable. */
static unsigned portBASE_TYPE uxCriticalNesting = 0xaaaaaaaa;

/*
 * Setup the timer to generate the tick interrupts.  The implementation in this
 * file is weak to allow application writers to change the timer used to
 * generate the tick interrupt.
 */
void vPortSetupTimerInterrupt( void );

/*
 * Exception handlers.
 */
void xPortPendSVHandler( void ) __attribute__ ((naked));
void xPortSysTickHandler( void );
void vPortSVCHandler( void ) __attribute__ ((naked));

/*
 * Start first task is a separate function so it can be tested in isolation.
 */
static void prvPortStartFirstTask( void ) __attribute__ ((naked));

/*
 * Function to enable the VFP.
 */
static void vPortEnableVFP( void ) __attribute__ ((naked));

/*-----------------------------------------------------------*/

/*
 * See header file for description.
 */
portSTACK_TYPE *pxPortInitialiseStack( portSTACK_TYPE *pxTopOfStack,
		pdTASK_CODE pxCode, void *pvParameters )
{
	/* Simulate the stack frame as it would be created by a context switch
	* interrupt. */

	/* Offset added to account for the way the MCU uses the stack on
	* entry/exit
	* of interrupts, and to ensure alignment. */
	pxTopOfStack--;

	*pxTopOfStack = portINITIAL_XPSR;    /* xPSR */
	pxTopOfStack--;
	*pxTopOfStack = (portSTACK_TYPE)pxCode;       /* PC */
	pxTopOfStack--;
	*pxTopOfStack = 0;    /* LR */

	/* Save code space by skipping register initialisation. */
	pxTopOfStack -= 5;    /* R12, R3, R2 and R1. */
	*pxTopOfStack = (portSTACK_TYPE)pvParameters;       /* R0 */

	/* A save method is being used that requires each task to maintain its
	* own exec return value. */
	pxTopOfStack--;
	*pxTopOfStack = portINITIAL_EXEC_RETURN;

	pxTopOfStack -= 8;    /* R11, R10, R9, R8, R7, R6, R5 and R4. */

	return pxTopOfStack;
}

/*-----------------------------------------------------------*/

/*void vPortSVCHandler( void )*/ /* ATMEL */
__attribute__ ((naked)) void SVC_Handler( void )
{
	__asm volatile (
		" ldr r3, pxCurrentTCBConst2      \n"   /* Restore the context. */
		" ldr r1, [r3]                    \n"   /* use pxCurrentTCBConst to get the pxCurrentTCB address. */
		" ldr r0, [r1]                    \n"   /* The first item in pxCurrentTCB is the task top of stack. */
		" ldmia r0!, {r4-r11, r14}        \n"   /* Pop the registers that are not automatically saved on exception entry and the critical nesting count. */
		" msr psp, r0                     \n"   /* Restore the task stack pointer. */
		" mov r0, #0                      \n"
		" msr basepri, r0                 \n"
		" bx r14                          \n"
		"                                   \n"
		" .align 2                        \n"
		"pxCurrentTCBConst2: .word pxCurrentTCB \n"
		);
}

/*-----------------------------------------------------------*/

static void prvPortStartFirstTask( void )
{
	__asm volatile (
		" ldr r0, =0xE000ED08     \n"                         /* Use the NVIC offset register to locate the stack. */
		" ldr r0, [r0]            \n"
		" ldr r0, [r0]            \n"
		" msr msp, r0             \n"                         /* Set the msp back to the start of the stack. */
		" cpsie i                 \n"                         /* Globally enable interrupts. */
		" svc 0                   \n"                         /* System call to start first task. */
		" nop                     \n"
		);
}

/*-----------------------------------------------------------*/

/*
 * See header file for description.
 */
portBASE_TYPE xPortStartScheduler( void )
{
	/* configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to 0.
	* See http://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html */
	configASSERT( configMAX_SYSCALL_INTERRUPT_PRIORITY );

	/* Make PendSV, CallSV and SysTick the same priroity as the kernel. */
	portNVIC_SYSPRI2_REG |= portNVIC_PENDSV_PRI;
	portNVIC_SYSPRI2_REG |= portNVIC_SYSTICK_PRI;

	/* Start the timer that generates the tick ISR.  Interrupts are disabled
	* here already. */
	vPortSetupTimerInterrupt();

	/* Initialise the critical nesting count ready for the first task. */
	uxCriticalNesting = 0;

#if defined (__VFP_FP__) && !defined(__SOFTFP__)
	/* Ensure the VFP is enabled - it should be anyway. */
	vPortEnableVFP();
#endif

	/* Lazy save always. */
	*(portFPCCR) |= portASPEN_AND_LSPEN_BITS;

	/* Start the first task. */
	prvPortStartFirstTask();

	/* Should not get here! */
	return 0;
}

/*-----------------------------------------------------------*/

void vPortEndScheduler( void )
{
	/* It is unlikely that the CM4F port will require this function as there
	* is nothing to return to.  */
}

/*-----------------------------------------------------------*/

void vPortYieldFromISR( void )
{
	/* Set a PendSV to request a context switch. */
	portNVIC_INT_CTRL_REG = portNVIC_PENDSVSET_BIT;
}

/*-----------------------------------------------------------*/

void vPortEnterCritical( void )
{
	portDISABLE_INTERRUPTS();
	uxCriticalNesting++;
}

/*-----------------------------------------------------------*/

void vPortExitCritical( void )
{
	uxCriticalNesting--;
	if (uxCriticalNesting == 0) {
		portENABLE_INTERRUPTS();
	}
}

/*-----------------------------------------------------------*/

__attribute__((naked)) unsigned long ulPortSetInterruptMask( void )
{
	__asm volatile \
	( \
		" mrs r0, basepri                   \n"	\
		" mov r1, %0                        \n"	\
		" msr basepri, r1                   \n"	\
		" bx lr                             \n"	\
		:: "i" (configMAX_SYSCALL_INTERRUPT_PRIORITY) : "r0", "r1" \
	);

	/* This return will not be reached but is necessary to prevent compiler
	 * warnings. */
	return 0;
}

/*-----------------------------------------------------------*/

__attribute__((naked)) void vPortClearInterruptMask(
		unsigned long ulNewMaskValue )
{
	__asm volatile \
	( \
		" msr basepri, r0                            \n" \
		" bx lr                                      \n" \
		::: "r0" \
	);
}

/*-----------------------------------------------------------*/

/*void xPortPendSVHandler( void )*/
__attribute__((naked)) void PendSV_Handler( void )   /* ATMEL */
{
	/* This is a naked function. */

	__asm volatile
	(
		"   mrs r0, psp                         \n"
		"                                       \n"
		"   ldr r3, pxCurrentTCBConst           \n" /* Get the location of the current TCB. */
		"   ldr r2, [r3]                        \n"
		"                                       \n"
		"   tst r14, #0x10                      \n" /* Is the task using the FPU context? If so, push high vfp registers. */
		"   it eq                               \n"
		"   vstmdbeq r0!, {s16-s31}             \n"
		"                                       \n"
		"   stmdb r0!, {r4-r11, r14}            \n" /* Save the core registers. */
		"                                       \n"
		"   str r0, [r2]                        \n" /* Save the new top of stack into the first member of the TCB. */
		"                                       \n"
		"   stmdb sp!, {r3, r14}                \n"
		"   mov r0, %0                          \n"
		"   msr basepri, r0                     \n"
		"   bl vTaskSwitchContext               \n"
		"   mov r0, #0                          \n"
		"   msr basepri, r0                     \n"
		"   ldmia sp!, {r3, r14}                \n"
		"                                       \n"
		"   ldr r1, [r3]                        \n" /* The first item in pxCurrentTCB is the task top of stack. */
		"   ldr r0, [r1]                        \n"
		"                                       \n"
		"   ldmia r0!, {r4-r11, r14}            \n" /* Pop the core registers. */
		"                                       \n"
		"   tst r14, #0x10                      \n" /* Is the task using the FPU context? If so, pop the high vfp too.*/
		"   it eq                               \n"
		"   vldmiaeq r0!, {s16-s31}             \n"
		"                                       \n"
		"   msr psp, r0                         \n"
		"   bx r14                              \n"
		"                                       \n"
		"   .align 2                            \n"
		"pxCurrentTCBConst: .word pxCurrentTCB    \n"
		::"i" (configMAX_SYSCALL_INTERRUPT_PRIORITY)
	);
}

/*-----------------------------------------------------------*/

/*void xPortSysTickHandler( void )*/
void xPortSysTickHandler( void ) /* ATMEL */
{
	/* If using preemption, also force a context switch. */
	#if configUSE_PREEMPTION == 1
	portNVIC_INT_CTRL_REG = portNVIC_PENDSVSET_BIT;
	#endif

	(void)portSET_INTERRUPT_MASK_FROM_ISR();
	{
		vTaskIncrementTick();
	}
	portCLEAR_INTERRUPT_MASK_FROM_ISR( 0 );
}

/*-----------------------------------------------------------*/

/*
 * Setup the systick timer to generate the tick interrupts at the required
 * frequency.
 */
__attribute__((weak)) void vPortSetupTimerInterrupt( void )
{
	/* Configure SysTick to interrupt at the requested rate. */
	portNVIC_SYSTICK_LOAD_REG
		= (configSYSTICK_CLOCK_HZ / configTICK_RATE_HZ) - 1UL;
	portNVIC_SYSTICK_CTRL_REG = portNVIC_SYSTICK_CLK_BIT |
			portNVIC_SYSTICK_INT_BIT | portNVIC_SYSTICK_ENABLE_BIT;
}

/*-----------------------------------------------------------*/

/* This is a naked function. */
static void vPortEnableVFP( void )
{
	__asm volatile
	(
		" ldr.w r0, =0xE000ED88      \n" /* The FPU enable bits are in the CPACR. */
		" ldr r1, [r0]               \n"
		"                            \n"
		" orr r1, r1, #( 0xf << 20 ) \n" /* Enable CP10 and CP11 coprocessors, then save back. */
		" str r1, [r0]               \n"
		" bx r14                     "
	);
}
