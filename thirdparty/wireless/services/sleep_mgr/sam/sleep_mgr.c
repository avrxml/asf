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
#include "rtt.h"
#include "pmc.h"
#include "supc.h"
# include "asf.h"

void RTT_Handler(void)
{
	rtt_get_status(RTT);
	/* rtt_write_alarm_time(RTT, rtt_read_timer_value(RTT) + 2); */
}

/**
 * \brief This function Initializes the Sleep functions
 */
void sm_init(void)
{
	rtt_init(RTT, 32768);

	/* Enable RTT interrupt */
	NVIC_DisableIRQ(RTT_IRQn);
	NVIC_ClearPendingIRQ(RTT_IRQn);
	NVIC_SetPriority(RTT_IRQn, 0);
	NVIC_EnableIRQ(RTT_IRQn);
	rtt_enable_interrupt(RTT, RTT_MR_ALMIEN);

	/* Set wakeup source to rtt_alarm */
	pmc_set_fast_startup_input(PMC_FSMR_RTTAL);
	#if (!SAMG)
	supc_set_wakeup_mode(SUPC, SUPC_WUMR_RTTEN_ENABLE);
	#endif
	/* Initialize the sleep manager, lock initial mode. */
	sleepmgr_init();
	sleepmgr_lock_mode(SLEEPMGR_WAIT);
}

/**
 * \brief This function puts the  device to sleep
 */
void sm_sleep(uint32_t interval)
{
	rtt_write_alarm_time(RTT, rtt_read_timer_value(RTT) + interval);

	/*
	 * Go to sleep in the deepest allowed sleep mode (i.e. no
	 * deeper than the currently locked sleep mode).
	 */
	sleepmgr_enter_sleep();
}
