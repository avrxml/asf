/**
* \file  sysTimer.c
*
* \brief System timer implementation
*
* Copyright (c) 2018 Microchip Technology Inc. and its subsidiaries. 
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
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, 
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

/*- Includes ---------------------------------------------------------------*/
#include "compiler.h"
#include "common_hw_timer.h"
#include "sysTimer.h"


/*****************************************************************************
*****************************************************************************/
static void placeTimer(SYS_Timer_t *timer);
static void SYS_HwExpiry_Cb(void);
static void SYS_HwOverflow_Cb(void);

/*- Variables --------------------------------------------------------------*/
static SYS_Timer_t *timers;
volatile uint8_t SysTimerIrqCount;

volatile uint8_t timerExtension1,timerExtension2;

/*- Implementations --------------------------------------------------------*/

/*************************************************************************//**
*****************************************************************************/
void SYS_TimerInit(void)
{
	SysTimerIrqCount = 0;
    timerExtension1 = 0;
    timerExtension2 = 0;
    set_common_tc_overflow_callback(SYS_HwOverflow_Cb);
	set_common_tc_expiry_callback(SYS_HwExpiry_Cb);
	common_tc_init();
	common_tc_delay(SYS_TIMER_INTERVAL * MS);
	timers = NULL;
}

/*************************************************************************//**
*****************************************************************************/
void SYS_TimerStart(SYS_Timer_t *timer)
{
	if (!SYS_TimerStarted(timer)) {
		placeTimer(timer);
	}
}

/*************************************************************************//**
*****************************************************************************/
void SYS_TimerStop(SYS_Timer_t *timer)
{
	SYS_Timer_t *prev = NULL;

	for (SYS_Timer_t *t = timers; t; t = t->next) {
		if (t == timer) {
			if (prev) {
				prev->next = t->next;
			} else {
				timers = t->next;
			}

			if (t->next) {
				t->next->timeout += timer->timeout;
			}

			break;
		}

		prev = t;
	}
}

/*************************************************************************//**
*****************************************************************************/
bool SYS_TimerStarted(SYS_Timer_t *timer)
{
	for (SYS_Timer_t *t = timers; t; t = t->next) {
		if (t == timer) {
			return true;
		}
	}
	return false;
}

/*************************************************************************//**
*****************************************************************************/
void SYS_TimerTaskHandler(void)
{
	uint32_t elapsed;
	uint8_t cnt;
	irqflags_t flags;

	if (0 == SysTimerIrqCount) {
		return;
	}

	/* Enter a critical section */
	flags = cpu_irq_save();
	cnt = SysTimerIrqCount;
	SysTimerIrqCount = 0;
	/* Leave the critical section */
	cpu_irq_restore(flags);

	elapsed = cnt * SYS_TIMER_INTERVAL;

	while (timers && (timers->timeout <= elapsed)) {
		SYS_Timer_t *timer = timers;

		elapsed -= timers->timeout;
		timers = timers->next;
		if (SYS_TIMER_PERIODIC_MODE == timer->mode) {
			placeTimer(timer);
		}

		if (timer->handler) {
			timer->handler(timer);
		}
	}

	if (timers) {
		timers->timeout -= elapsed;
	}
}

/*************************************************************************//**
*****************************************************************************/
static void placeTimer(SYS_Timer_t *timer)
{
	if (timers) {
		SYS_Timer_t *prev = NULL;
		uint32_t timeout = timer->interval;

		for (SYS_Timer_t *t = timers; t; t = t->next) {
			if (timeout < t->timeout) {
				t->timeout -= timeout;
				break;
			} else {
				timeout -= t->timeout;
			}

			prev = t;
		}

		timer->timeout = timeout;

		if (prev) {
			timer->next = prev->next;
			prev->next = timer;
		} else {
			timer->next = timers;
			timers = timer;
		}
	} else {
		timer->next = NULL;
		timer->timeout = timer->interval;
		timers = timer;
	}
}

/*********************************************************************
* Function:         void MiWi_TickGet()
*
* PreCondition:     none
*
* Input:		    none
*
* Output:		    MIWI_TICK - the current symbol time
*
* Side Effects:	    The timer interrupt is disabled
*                   for several instruction cycles while the 
*                   timer value is grabbed.  This is to prevent a
*                   rollover from incrementing the timer extenders
*                   during the read of their values
*
* Overview:		    This function returns the current time
*
* Note:			    This function should never be called 
*                   when interrupts are disabled if interrupts are 
*                   disabled when this is called then the timer 
*                   might rollover and the byte extension would not 
*                   get updated.
********************************************************************/
uint32_t MiWi_TickGet(void)
{
    MIWI_TICK currentTime;
	uint8_t current_timerExtension1 = timerExtension1;
    
    /* disable the timer to prevent roll over of the lower 16 bits 
	while before/after reading of the extension */
	tmr_disable_ovf_interrupt();

	currentTime.word.w0 = common_tc_read_count();
    /* enable the timer*/
	tmr_enable_ovf_interrupt();

	/* NOP to allow the interrupt to process before copying timerExtension1 */
	nop();

	/* If interrupt occurred during interrupt disable  read count again*/
	if (current_timerExtension1 != timerExtension1)
	{
		currentTime.word.w0 = common_tc_read_count();
	}
    /* copy the byte extension */
    currentTime.byte.b2 = timerExtension1;
    currentTime.byte.b3 = timerExtension2;

    return currentTime.Val;
}

/*********************************************************************
* Function:         MIWI_TICK MiWi_TickGetDiff()
*
* PreCondition:     none
*
* Input:		    current_tick  - Recent tick read from MiWi_TickGet
*                   previous_tick - Tick read prior to current_tick for 
*                                   calculation.
*
* Output:		    uint32_t - Difference in current_tick to previous_tick
*
* Side Effects:	    none
*
* Overview:		    This function returns difference between current_tick
*                   and previous_tick
*
* Note:			    none
********************************************************************/
uint32_t MiWi_TickGetDiff(MIWI_TICK current_tick, MIWI_TICK previous_tick)
{
	uint32_t ret_val;
	if (current_tick.Val > previous_tick.Val)
	{
		/* If current > previous, no overflow in running timer*/
		ret_val = current_tick.Val - previous_tick.Val;
	}
	else
	{
		/* Handling Overflow as current tick < previous tick */
		ret_val = (0xFFFFFFFF - previous_tick.Val) + current_tick.Val;
	}
	return ret_val;
}


/*****************************************************************************
*****************************************************************************/
static void SYS_HwExpiry_Cb(void)
{
	SysTimerIrqCount++;
	common_tc_delay(SYS_TIMER_INTERVAL * MS);
}

static void SYS_HwOverflow_Cb(void)
{
	timerExtension1++;
	if(timerExtension1 == 0)
	{
		timerExtension2++;
	}
}

void SYS_TimerAdjust_SleptTime(uint32_t sleeptime)
{
	SYS_Timer_t* timer = timers;
	while (timer)
	{
		if (timer->timeout > sleeptime)
		{
			timer->timeout -= sleeptime;
		}
		else
		{
			timer->timeout = 0;
		}
		timer = timer->next;
	}
}