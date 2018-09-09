/**
 * @file hw_timer.c
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
 */

#include <compiler.h>
#include <parts.h>
#include "tc.h"
#include "hw_timer.h"
#include "common_hw_timer.h"
#include "conf_hw_timer.h"
#include "sysclk.h"

extern uint8_t timer_multiplier;
tmr_callback_t tmr_callback;

/* === Prototypes =========================================================== */
#ifndef SAM4L

/*extern void sysclk_enable_peripheral_clock(const volatile void *module);
 #else*/
extern void sysclk_enable_peripheral_clock(uint32_t ul_id);

#endif
/*extern uint32_t sysclk_get_peripheral_bus_hz(const volatile void *module);*/
static void configure_NVIC(Tc *cmn_hw_timer, uint8_t cmn_hw_timer_ch);
static void tc_callback(void);

/*! \brief  read the actual timer count from register
 */
uint16_t tmr_read_count(void)
{
	return tc_read_cv(TIMER, TIMER_CHANNEL_ID);
}

/*! \brief  to save current interrupt status
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
	cpu_irq_restore(flags);
}

/*! \brief  to stop the running timer
 */
void tmr_stop(void)
{
	tc_stop(TIMER, TIMER_CHANNEL_ID);
}

/*! \brief  to load compare value in channel compare register
 */
void tmr_write_cmpreg(uint16_t compare_value)
{
#if SAM4E
	tc_write_ra(TIMER, TIMER_CHANNEL_ID, compare_value);
#else
	tc_write_rc(TIMER, TIMER_CHANNEL_ID, compare_value);
#endif
}

/*! \brief  to initialiaze hw timer
 */
uint8_t tmr_init(void)
{
	uint8_t tmr_mul;
	/* Configure clock service. */
	#if SAM4L
	sysclk_enable_peripheral_clock(TIMER);
	#else
	sysclk_enable_peripheral_clock(ID_TC);
	#endif

	/* Get system clock. */
	tmr_mul = sysclk_get_peripheral_bus_hz(TIMER) / DEF_1MHZ;
	tmr_mul = tmr_mul >> 1;
	#if SAM4L
	tc_init(TIMER, TIMER_CHANNEL_ID,
			TC_CMR_TCCLKS_TIMER_CLOCK2 | TC_CMR_WAVE |
			TC_CMR_WAVSEL_UP_NO_AUTO);
	#elif SAM4E
	tc_init(TIMER, TIMER_CHANNEL_ID,
			TC_CMR_TCCLKS_TIMER_CLOCK1 | TC_CMR_WAVE |
			TC_CMR_WAVSEL_UP_RC);
	#else
	tc_init(TIMER, TIMER_CHANNEL_ID,
			TC_CMR_TCCLKS_TIMER_CLOCK1 | TC_CMR_WAVE |
			TC_CMR_WAVSEL_UP);
	#endif

	/* Configure and enable interrupt on RC compare. */
	configure_NVIC(TIMER, TIMER_CHANNEL_ID);
#if SAM4E
	tc_get_status(TIMER, TIMER_CHANNEL_ID);
	tc_enable_interrupt(TIMER, TIMER_CHANNEL_ID, TC_IER_CPCS);
	tc_write_rc(TIMER, TIMER_CHANNEL_ID, UINT16_MAX);
#else
	tc_get_status(TIMER, TIMER_CHANNEL_ID);
	tc_enable_interrupt(TIMER, TIMER_CHANNEL_ID, TC_IER_COVFS);
#endif
	tmr_disable_cc_interrupt();
	tc_start(TIMER, TIMER_CHANNEL_ID);
	return tmr_mul;
}

/*! \brief  to disable compare interrupt
 */
void tmr_disable_cc_interrupt(void)
{
	tc_get_status(TIMER, TIMER_CHANNEL_ID);
#if SAM4E
	tc_disable_interrupt(TIMER, TIMER_CHANNEL_ID, TC_IDR_CPAS);
#else
	tc_disable_interrupt(TIMER, TIMER_CHANNEL_ID, TC_IDR_CPCS);
#endif
}

/*! \brief  to enable compare interrupt
 */
void tmr_enable_cc_interrupt(void)
{
	tc_get_status(TIMER, TIMER_CHANNEL_ID);
#if SAM4E
	tc_enable_interrupt(TIMER, TIMER_CHANNEL_ID, TC_IDR_CPAS);
#else
	tc_enable_interrupt(TIMER, TIMER_CHANNEL_ID, TC_IDR_CPCS);
#endif
}

/*! \brief  to disable overflow interrupt
 */
void tmr_disable_ovf_interrupt(void)
{
	/*! \brief  to enable overflow interrupt
	 */
	tc_get_status(TIMER, TIMER_CHANNEL_ID);
#if SAM4E
	tc_disable_interrupt(TIMER, TIMER_CHANNEL_ID, TC_IDR_CPCS);
#else
	tc_disable_interrupt(TIMER, TIMER_CHANNEL_ID, TC_IDR_COVFS);
#endif
}

/*! \brief  to enable the corresponding IRQ in NVIC and set the tme callback
 */

void configure_NVIC(Tc *cmn_hw_timer, uint8_t cmn_hw_timer_ch)
{
	if (TC0 == cmn_hw_timer) {
		switch (cmn_hw_timer_ch) {
#if SAM4L
		case 0:
			NVIC_EnableIRQ(TC00_IRQn);
			break;

		case 1:
			NVIC_EnableIRQ(TC01_IRQn);
			break;

		case 2:
			NVIC_EnableIRQ(TC02_IRQn);
			break;

#else
		case 0:
			NVIC_EnableIRQ(TC0_IRQn);
			break;

		case 1:
			NVIC_EnableIRQ(TC1_IRQn);
			break;

		case 2:
			NVIC_EnableIRQ(TC2_IRQn);
			break;
#endif
		default:
			break;
		}
	} else if (TC1 == cmn_hw_timer) {
		switch (cmn_hw_timer_ch) {
#if SAM4L
		case 0:
			NVIC_EnableIRQ(TC10_IRQn);
			break;

		case 1:
			NVIC_EnableIRQ(TC11_IRQn);
			break;

		case 2:
			NVIC_EnableIRQ(TC12_IRQn);
			break;

#else
		case 0:
			NVIC_EnableIRQ(TC3_IRQn);
			break;

		case 1:
			NVIC_EnableIRQ(TC4_IRQn);
			break;

		case 2:
			NVIC_EnableIRQ(TC5_IRQn);
			break;
#endif
		default:
			break;
		}
	}

	tmr_callback = tc_callback;
}

/**
 * \brief callback for tmr interrupt
 */
void tc_callback(void)
{
	uint32_t ul_status;
	/* Read TC0 Status. */
	ul_status = tc_get_status(TIMER, TIMER_CHANNEL_ID);
	ul_status &= tc_get_interrupt_mask(TIMER, TIMER_CHANNEL_ID);
#if SAM4E
	if (TC_SR_CPAS == (ul_status & TC_SR_CPAS)) {
		tmr_cca_callback();
	}

	if (TC_SR_CPCS == (ul_status & TC_SR_CPCS)) {
		tmr_ovf_callback();
	}

#else
	if (TC_SR_CPCS == (ul_status & TC_SR_CPCS)) {
		tmr_cca_callback();
	}

	/* Overflow */
	if (TC_SR_COVFS == (ul_status & TC_SR_COVFS)) {
		tmr_ovf_callback();
	}
#endif
}

/**
 * \brief Interrupt handler for TC00
 */
#if SAM4L
void TC00_Handler(void)
#else
void TC0_Handler(void)
#endif
{
	if (tmr_callback) {
		tmr_callback();
	}
}

/**
 * \brief Interrupt handler for TC01
 */
#if SAM4L
void TC01_Handler(void)
#else
void TC1_Handler(void)
#endif
{
	if (tmr_callback) {
		tmr_callback();
	}
}

/**
 * \brief Interrupt handler for TC02
 */
#if SAM4L
void TC02_Handler(void)
#else
void TC2_Handler(void)
#endif
{
	if (tmr_callback) {
		tmr_callback();
	}
}

/**
 * \brief Interrupt handlers for TC10
 */
#if SAM4L
void TC10_Handler(void)
#else
void TC3_Handler(void)
#endif
{
	if (tmr_callback) {
		tmr_callback();
	}
}

/**
 * \brief Interrupt handler for TC11
 */
#if SAM4L
void TC11_Handler(void)
#else
void TC4_Handler(void)
#endif
{
	if (tmr_callback) {
		tmr_callback();
	}
}

/**
 * \brief Interrupt handler for TC12
 */
#if SAM4L
void TC12_Handler(void)
#else
void TC5_Handler(void)
#endif
{
	if (tmr_callback) {
		tmr_callback();
	}
}

/* EOF */
