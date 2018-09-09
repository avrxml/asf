/**
 * @file sleep_mgr.c
 *
 * @brief
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
 *
 */

/*
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */
#include "sleep_mgr.h"
#include "sleepmgr.h"
#include "conf_sleepmgr.h"
#include "sysclk.h"
#include "delay.h"

/*! \brief This macro initiates read synchronization of the RTC32 CNT register
 *
 *  Initiates synchronization of CNT register from RTC to system clock domain.
 */
#define RTC32_SyncCnt()                (RTC32.SYNCCTRL |= RTC32_SYNCCNT_bm)

/*! \brief This macro returns the RTC32 CNT read synchronization flag.
 *
 *  \return Non-Zero if RTC32 is busy synchronizing CNT from RTC to system
 *  clock domain, zero otherwise.
 */
#define RTC32_SyncCntBusy()            (RTC32.SYNCCTRL & RTC32_SYNCCNT_bm)

/*! \brief This macro returns the RTC32 CNT/CTRL write synchronization flag.
 *
 *  \return Non-Zero if RTC32 is busy synchronizing CNT/CTRL from system to
 *  RTC32 clock domain, and zero otherwise.
 */
#define RTC32_SyncBusy()               (RTC32.SYNCCTRL & RTC32_SYNCBUSY_bm)

static void RTC32_Initialize( uint32_t period, uint32_t count,
		uint32_t compareValue );
static void RTC32_SetTimeout( uint32_t alarmTimeout );

/**
 * \brief This function Initializes the Sleep functions
 */
void sm_init(void)
{
	/* Set the sleep mode to initially lock. */
	enum sleepmgr_mode mode = SLEEPMGR_PSAVE;

	sysclk_enable_module(SYSCLK_PORT_GEN, SYSCLK_RTC);

	RTC32_Initialize(0xffffffff, 0, 0);

	RTC32.INTCTRL |= RTC32_COMPINTLVL_LO_gc;

	/* Initialize the sleep manager, lock initial mode. */
	sleepmgr_init();
	sleepmgr_lock_mode(mode);
}

/**
 * \brief This function puts the  device to sleep
 */
void sm_sleep(uint32_t interval)
{
	RTC32_SetTimeout((interval * 1000));
	sleepmgr_enter_sleep();
}

/* Interrupt Service Routine definitions */
ISR(RTC32_COMP_vect)
{
}

static void RTC32_Initialize( uint32_t period, uint32_t count,
		uint32_t compareValue )
{
	/* Enable R/W access to the battery backup module. */
	VBAT.CTRL = VBAT_ACCEN_bm;

	/* Reset the module. (Reset bit is protected by CCP.) */

	{uint8_t flags = cpu_irq_save();
	 CCP = 0xD8;
	 VBAT.CTRL = VBAT_RESET_bm;
	 cpu_irq_restore(flags); }

	/* Enable the failure detection. */
	VBAT.CTRL |= VBAT_XOSCFDEN_bm;

	/* A delay is needed to give the voltage in the backup system some time
	 * to stabilise.
	 */
	delay_us(200);

	VBAT.CTRL |= VBAT_XOSCEN_bm | VBAT_XOSCSEL_bm;

	/* Disable the RTC32 module before writing to it. Wait for synch. */
	RTC32.CTRL &= ~RTC32_ENABLE_bm;
	do { } while (RTC32_SyncBusy());

	/* Write PER, COMP and CNT. */
	RTC32.PER = period - 1;
	RTC32.COMP = compareValue;
	RTC32.CNT = count;

	/* Re-enable the RTC32 module, synchronize before returning. */
	RTC32.CTRL |= RTC32_ENABLE_bm;
	do { } while (RTC32_SyncBusy());
}

static void RTC32_SetTimeout( uint32_t alarmTimeout )
{
	uint32_t compareValue;

	/* Synchronize CNT from RTC to system clock domain. */
	RTC32_SyncCnt();
	do { } while (RTC32_SyncCntBusy());

	/* Calculate compare time. */
	compareValue = RTC32.CNT + alarmTimeout;

	/* Wrap on period. */
	if (compareValue > RTC32.PER) {
		compareValue -= RTC32.PER;
	}

	/* Add the timeout value to get the absolute time of the alarm. */
	RTC32.COMP = compareValue;
}
