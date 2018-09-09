/**
 * @file hw_timer.c
 *
 * @brief
 *
 *
 * Copyright (c) 2013-2018 Microchip Technology Inc. and its subsidiaries.
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

#include <compiler.h>
#include <parts.h>
#include "tc.h"
#include "tc_interrupt.h"
#include "hw_timer.h"
#if SAMD || SAMR21 || SAML21 || SAMR30
#include "clock.h"
#include <system_interrupt.h>
#else
#include "sysclk.h"
#endif
#include "common_hw_timer.h"
#include "conf_hw_timer.h"

#define TIMER_PERIOD  UINT16_MAX

struct tc_config timer_config;
struct tc_module module_inst;
extern bool sys_sleep;
#if 0
void common_tc_delay(uint16_t value)
{
	value = value;
}

void common_tc_compare_stop(void)
{
}

uint16_t common_tc_read_count(void)
{
	return ((uint16_t)tc_get_count_value(&module_inst));
}

#endif

/*! \brief  read the actual timer count from register
 */
uint16_t tmr_read_count(void)
{
	return ((uint16_t)tc_get_count_value(&module_inst));
}

/*! \brief  to disable compare interrupt
 */
void tmr_disable_cc_interrupt(void)
{
	tc_disable_callback(&module_inst, TC_CALLBACK_CC_CHANNEL0);
}

/*! \brief  to enable compare interrupt
 */
void tmr_enable_cc_interrupt(void)
{
	tc_enable_callback(&module_inst, TC_CALLBACK_CC_CHANNEL0);
}

/*! \brief  to disable overflow interrupt
 */
void tmr_disable_ovf_interrupt(void)
{
	tc_disable_callback(&module_inst, TC_CALLBACK_OVERFLOW);
}

/*! \brief  to disable overflow interrupt
 */
void tmr_enable_ovf_interrupt(void)
{
	tc_enable_callback(&module_inst, TC_CALLBACK_OVERFLOW);
}

/*! \brief  to stop the running timer
 */
void tmr_stop(void)
{
	tc_disable(&module_inst);
}

/*! \brief  to load compare value in channel compare register
 */
void tmr_write_cmpreg(uint16_t compare_value)
{
	tc_set_compare_value(&module_inst, TC_COMPARE_CAPTURE_CHANNEL_0,
			(uint32_t)compare_value);
}

/*! \brief  to save current interrupts status
 */
uint8_t save_cpu_interrupt(void)
{
	return cpu_irq_save();
}

/*! \brief  to restore saved interrupts status
 *  \param  saved interrupt status
 */
void restore_cpu_interrupt(uint8_t flags)
{
	cpu_irq_restore((uint32_t)flags);
}

static void tc_ovf_callback(struct tc_module *const module_instance)
{
	tmr_ovf_callback();
}

/*! \brief  hw timer compare callback
 */
static void tc_cca_callback(struct tc_module *const module_instance)
{
	tmr_cca_callback();
}

/*! \brief  to initialize hw timer
 */
uint8_t tmr_init(void)
{
	uint8_t timer_multiplier;
	tc_get_config_defaults(&timer_config);
	#ifdef ENABLE_SLEEP
	if (sys_sleep == true) {
		timer_config.clock_source = GCLK_GENERATOR_1;
		timer_config.clock_prescaler = TC_CLOCK_PRESCALER_DIV2;
		timer_config.run_in_standby = true;
	}
	#endif
	timer_config.counter_16_bit.compare_capture_channel[0] = TIMER_PERIOD;
	tc_init(&module_inst, TIMER, &timer_config);
	tc_register_callback(&module_inst, tc_ovf_callback,
			TC_CALLBACK_OVERFLOW);
	tc_register_callback(&module_inst, tc_cca_callback,
			TC_CALLBACK_CC_CHANNEL0);
	tc_enable_callback(&module_inst, TC_CALLBACK_OVERFLOW);
	tc_enable_callback(&module_inst, TC_CALLBACK_CC_CHANNEL0);

	tc_enable(&module_inst);

	/* calculate how faster the timer with current clk freq compared to
	 * timer with 1Mhz */
	#ifdef ENABLE_SLEEP
	if (sys_sleep == true) {
		timer_multiplier = system_gclk_gen_get_hz(1) / 2000000;
	} else {
		timer_multiplier = system_gclk_gen_get_hz(0) / DEF_1MHZ;
	}

    #else
	timer_multiplier = system_gclk_gen_get_hz(0) / DEF_1MHZ;
	#endif
	return timer_multiplier;
}
