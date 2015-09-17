/**
 * @file sleep_mgr.c
 *
 * @brief
 *
 * Copyright (C) 2014-2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
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
 *
 */

/*
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

#include "sleep_mgr.h"
#include "sleepmgr.h"
#include "conf_sleepmgr.h"
#include "ast.h"
#include "osc.h"

uint32_t ast_alarm, ast_counter;

/**
 * \brief Callback handler for AST alarm Interrupt.
 */
static void ast_alarm_callback(void)
{
	ast_disable_interrupt(AST, AST_INTERRUPT_ALARM);

	/* After wake up, clear the Alarm0. */
	ast_clear_interrupt_flag(AST, AST_INTERRUPT_ALARM);
}

/* Configures the AST for Sleep Timers */
static void config_ast(void)
{
	struct ast_config ast_conf;

	/* Enable osc32 oscillator*/
	if (!osc_is_ready(OSC_ID_RC32K)) {
		osc_enable(OSC_ID_RC32K);
		osc_wait_ready(OSC_ID_RC32K);
	}

	/* Enable the AST */
	ast_enable(AST);

	ast_conf.mode = AST_COUNTER_MODE;
	ast_conf.osc_type = AST_OSC_32KHZ; /* OSC Type iSs OSC32 or RC32 */
	ast_conf.psel = AST_PSEL_32KHZ_1HZ;
	ast_conf.counter = 0;

	/*
	 * Using counter mode and set it to 0.
	 * Initialize the AST.
	 */
	ast_set_config(AST, &ast_conf);

	/* First clear alarm status. */
	ast_clear_interrupt_flag(AST, AST_INTERRUPT_ALARM);

	/* Enable wakeup from alarm0. */
	ast_enable_wakeup(AST, AST_WAKEUP_ALARM);

	/* Set callback for alarm0. */
	ast_set_callback(AST, AST_INTERRUPT_ALARM, ast_alarm_callback,
			AST_ALARM_IRQn, 1);

	/* Disable first interrupt for alarm0. */
	ast_disable_interrupt(AST, AST_INTERRUPT_ALARM);
}

/**
 * \brief This function Initializes the Sleep functions
 */
void sm_init(void)
{
	/* Set the sleep mode to initially lock. */
	enum sleepmgr_mode mode = SLEEPMGR_RET;

	bpm_set_clk32_source(BPM, BPM_CLK32_SOURCE_RC32K);

	config_ast();

	/* AST can wakeup the device */
	bpm_enable_wakeup_source(BPM, (1 << BPM_BKUPWEN_AST));

	/* Initialize the sleep manager, lock initial mode. */
	sleepmgr_init();
	sleepmgr_lock_mode(mode);
}

/**
 * \brief This function puts the  device to sleep
 */
void sm_sleep(uint32_t interval)
{
	ast_counter = ast_read_counter_value(AST);
	ast_alarm = ast_counter + interval;
	ast_write_alarm0_value(AST, ast_alarm);

	ast_enable_interrupt(AST, AST_INTERRUPT_ALARM);
	sleepmgr_enter_sleep();
}
