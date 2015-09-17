/**
 * \file
 *
 * \brief AVR XMEGA XCL (XMEGA Custom Logic) TC (Timer Counter) sub-module
 *driver.
 *
 *
 * Copyright (C) 2014-2015 Atmel Corporation. All rights reserved.
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
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef _XCL_TIMER_H_
#define _XCL_TIMER_H_

#include "compiler.h"
#include "parts.h"
#include "pmic.h"
#include "conf_xcl.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \ingroup xcl_group
 * \defgroup xcl_tc_group XMEGA Custom Logic (XCL) Timer/Counter (TC) sub-module
 *
 * This group provides functions to configure Timer/Counter (TC) sub-module.
 *
 * @{
 */

#if defined(CONFIG_XCL_CALLBACK_ENABLE) || defined(__DOXYGEN__)
typedef void (*xcl_callback_t)(void);
void xcl_set_unf_interrupt_callback(xcl_callback_t callback);
void xcl_set_cc_interrupt_callback(xcl_callback_t callback);
#endif

/* ! XCL TC configuration struct */
struct xcl_tc_config_t {
	uint8_t ctrle;
	uint8_t ctrlf;
};

/* ! XCL interrupt levels */
enum xcl_int_level_t {
	XCL_INT_LVL_OFF = 0x00,
	XCL_INT_LVL_LO  = 0x01,
	XCL_INT_LVL_MED = 0x02,
	XCL_INT_LVL_HI  = 0x03,
};

/* ! XCL Timer type configurations */
enum xcl_tc_sel_t {
	/* ! 16-bit timer/counter */
	TC16 = XCL_TCSEL_TC16_gc,
	/* ! One 8-bit timer/counter with period */
	BTC0 = XCL_TCSEL_BTC0_gc,
	/* ! Two 8-bit timer/counters */
	BTC01 = XCL_TCSEL_BTC01_gc,
	/* ! One 8-bit timer/counter with period and one 8-bit transmit
	 * peripheral counter */
	BTC0PEC1 = XCL_TCSEL_BTC0PEC1_gc,
	/* ! One 8-bit timer/counter with period and one 8-bit receiver
	 * peripheral counter */
	PEC0BTC1 = XCL_TCSEL_PEC0BTC1_gc,
	/* ! Two 8-bit peripheral counters */
	PEC01 = XCL_TCSEL_PEC01_gc,
	/* ! One 8-bit timer/counter with period and two 4-bit transmit/receiver
	 * peripheral counter */
	BTC0PEC2 = XCL_TCSEL_BTC0PEC2_gc,
};

/* ! XCL Timer clock source configurations */
enum xcl_tc_clk_source_t {
	/* ! OFF */
	OFF = XCL_CLKSEL_OFF_gc,
	/* ! Clkper */
	DIV1 = XCL_CLKSEL_DIV1_gc,
	/* ! Prescaler clk/2 */
	DIV2 = XCL_CLKSEL_DIV2_gc,
	/* ! Prescaler clk/4 */
	DIV4 = XCL_CLKSEL_DIV4_gc,
	/* ! Prescaler clk/8 */
	DIV8 = XCL_CLKSEL_DIV8_gc,
	/* ! Prescaler clk/64 */
	DIV64 = XCL_CLKSEL_DIV64_gc,
	/* ! Prescaler clk/256 */
	DIV256 = XCL_CLKSEL_DIV256_gc,
	/* ! Prescaler clk/1024 */
	DIV1024 = XCL_CLKSEL_DIV1024_gc,
	/* ! Event channel 0 */
	EVCH0 = XCL_CLKSEL_EVCH0_gc,
	/* ! Event channel 1 */
	EVCH1 = XCL_CLKSEL_EVCH1_gc,
	/* ! Event channel 2 */
	EVCH2 = XCL_CLKSEL_EVCH2_gc,
	/* ! Event channel 3 */
	EVCH3 = XCL_CLKSEL_EVCH3_gc,
	/* ! Event channel 4 */
	EVCH4 = XCL_CLKSEL_EVCH4_gc,
	/* ! Event channel 5 */
	EVCH5 = XCL_CLKSEL_EVCH5_gc,
	/* ! Event channel 6 */
	EVCH6 = XCL_CLKSEL_EVCH6_gc,
	/* ! Event channel 7 */
	EVCH7 = XCL_CLKSEL_EVCH7_gc,
};

/* ! XCL Timer mode configurations */
enum xcl_tc_mode_t {
	/* ! Normal mode with period */
	NORMAL = XCL_TCMODE_NORMAL_gc,
	/* ! Capture mode */
	CAPT = XCL_TCMODE_CAPT_gc,
	/* ! Single Slope PWM */
	PWM = XCL_TCMODE_PWM_gc,
};

/* ! XCL Timer command type (to restart both or single timer) */
enum xcl_tc_cmd_restart_type {
	/* !  Command Ignored */
	NO_CMD = XCL_CMDEN_DISABLE_gc,
	/* ! Command valid for timer/counter 0 */
	RESTART_TIMER0 = XCL_CMDEN_CMD0_gc,
	/* ! Command valid for timer/counter 1 */
	RESTART_TIMER1 = XCL_CMDEN_CMD1_gc,
	/* ! Command valid for both timer/counter 0 and 1 */
	RESTART_TIMER01 = XCL_CMDEN_CMD01_gc,
};

/**
 * \brief Generate a restart command on the selected XCL timer.
 *
 * This function will initiate a restart command for the XCL timer passed
 * as parameter.
 *
 * \param restart_type The restart type command
 */
static inline void xcl_tc_restart(enum xcl_tc_cmd_restart_type restart_type)
{
	XCL.CTRLF &= ~XCL_CMDEN_gm;
	XCL.CTRLF |= restart_type;
	XCL.CTRLE |= XCL_CMDSEL_bm;
}

/**
 * \brief Configure the XCL Timer module source clock.
 *
 * This function will configure the XCL timer source clock acording to the
 * selected source clock passed as parameter.
 *
 * \param src The selected soruce clock.
 */
static inline void xcl_tc_source_clock(enum xcl_tc_clk_source_t src)
{
	XCL.CTRLE &= ~XCL_CLKSEL_gm;
	XCL.CTRLE |= src;
}

/**
 * \brief Configure the XCL Timer module type.
 *
 * This function will configure the XCL timer mode acording to the
 * selected mode passed as parameter.
 *
 * \param tc_sel The timer selection
 */
static inline void xcl_tc_type(enum xcl_tc_sel_t tc_sel)
{
	XCL.CTRLE &= ~XCL_TCSEL_gm;
	XCL.CTRLE |= tc_sel;
}

/**
 * \brief Enable XCL timer output compare 0 output (CC0).
 *
 * This function enable the output compare 0 output of XCL BTCO timer.
 *
 */
static inline void xcl_enable_oc0(void)
{
	XCL.CTRLF |= XCL_CCEN0_bm;
}

/**
 * \brief Disable XCL timer output compare 0 output (CC0).
 *
 * This function Disable the output compare 0 output of XCL BTCO timer.
 *
 */
static inline void xcl_disable_oc0(void)
{
	XCL.CTRLF &= ~XCL_CCEN0_bm;
}

/**
 * \brief Enable XCL timer output compare 1 output (CC1).
 *
 * This function enable the output compare 1 output of XCL BTC1 timer.
 *
 */
static inline void xcl_enable_oc1(void)
{
	XCL.CTRLF |= XCL_CCEN1_bm;
}

/**
 * \brief Disable XCL timer output compare 1 output (CC1).
 *
 * This function Disable the output compare 1 output of XCL BTCO timer.
 *
 */
static inline void xcl_disable_oc1(void)
{
	XCL.CTRLF &= ~XCL_CCEN1_bm;
}

/**
 * \brief Force XCL Timer output compare 0 output (CC0).
 *
 * This function force the XCL Timer output compare 0 output pin to the
 * specified logic level.
 *
 * \note The XCL timer shold be turned off to force the output compare pin.
 *
 * \param value The logic value to be forced on CC0 pin
 */
static inline void xcl_force_oc0(bool value)
{
	if (value) {
		XCL.CTRLF |= XCL_CMP0_bm;
	} else {
		XCL.CTRLF &= ~XCL_CMP0_bm;
	}
}

/**
 * \brief Force XCL Timer output compare 1 output (CC1).
 *
 * This function force the XCL Timer output compare 1 output pin to the
 * specified logic level.
 *
 * \note The XCL timer shold be turned off to force the output compare pin.
 *
 * \param value The logic value to be forced on CC1 pin
 */
static inline void xcl_force_oc1(bool value)
{
	if (value) {
		XCL.CTRLF |= XCL_CMP1_bm;
	} else {
		XCL.CTRLF &= ~XCL_CMP1_bm;
	}
}

/**
 * \brief Configure the XCL Timer mode.
 *
 * This function will configure the XCL timer mode acording to the
 * selected mode passed as parameter.
 *
 * \param mode The timer selection
 */
static inline void xcl_tc_mode(enum xcl_tc_mode_t mode)
{
	XCL.CTRLF &= ~XCL_MODE_gm;
	XCL.CTRLF |= mode;
}

/**
 * \ingroup xcl_tc_group
 * \defgroup xcl_tc_it XMEGA Custom Logic (XCL) T/C interrupt management
 * This group provides functions to manage XCL TC interrupt Enable/Disable and
 * interrupt flags acknowledgement.
 *
 * @{
 */

/**
 * \brief Configure the XCL Timer Underflow interupt level.
 *
 * This function will configure the XCL timer Undeflow interrupt level
 * according to the level passed as parameter.
 *
 * \param level The interrupt level
 */
static inline void xcl_tc_set_underflow_interrupt_level(
		enum xcl_int_level_t level)
{
	XCL.INTCTRL &= ~XCL_UNFINTLVL_gm;
	XCL.INTCTRL |= level << XCL_UNFINTLVL_gp;
}

/**
 * \brief Configure the XCL Timer Capture Compare interupt level.
 *
 * This function will configure the XCL timer capture compare interrupt level
 * according to the level passed as parameter.
 *
 * \param level The interrupt level
 */
static inline void xcl_tc_set_compare_capture_interrupt_level(
		enum xcl_int_level_t level)
{
	XCL.INTCTRL &= ~XCL_CCINTLVL_gm;
	XCL.INTCTRL |= level << XCL_CCINTLVL_gp;
}

/**
 * \brief Enable XCL TC16 timer underflow interupt.
 *
 * This function enables the XCL TC16 underflow interupt.
 *
 */
static inline void xcl_tc16_enable_underflow_interrupt(void)
{
	XCL.INTCTRL |= XCL_UNF0IE_bm;
}

/**
 * \brief Disable XCL TC16 timer underflow interupt.
 *
 * This function disables the XCL TC16 underflow interupt.
 *
 */
static inline void xcl_tc16_disable_underflow_interrupt(void)
{
	XCL.INTCTRL &= ~XCL_UNF0IE_bm;
}

/**
 * \brief Enable XCL BTC0 timer underflow interupt.
 *
 * This function enable the XCL BTC0 underflow interupt.
 *
 */
static inline void xcl_btc0_enable_underflow_interrupt(void)
{
	XCL.INTCTRL |= XCL_UNF0IE_bm;
}

/**
 * \brief Disable XCL BTC0 timer underflow interupt.
 *
 * This function disables the XCL BTC0 underflow interupt.
 *
 */
static inline void xcl_btc0_disable_underflow_interrupt(void)
{
	XCL.INTCTRL &= ~XCL_UNF0IE_bm;
}

/**
 * \brief Enable XCL BTC1 timer underflow interupt.
 *
 * This function enables the XCL BTC1 underflow interupt.
 *
 */
static inline void xcl_btc1_enable_underflow_interrupt(void)
{
	XCL.INTCTRL |= XCL_UNF1IE_bm;
}

/**
 * \brief Disable XCL BTC1 timer underflow interupt.
 *
 * This function disables the XCL BTC1 underflow interupt.
 *
 */
static inline void xcl_btc1_disable_underflow_interrupt(void)
{
	XCL.INTCTRL &= ~XCL_UNF1IE_bm;
}

/**
 * \brief Enable XCL PEC0 timer underflow interupt.
 *
 * This function enables the XCL PEC0 underflow interupt.
 *
 */
static inline void xcl_pec0_enable_interrupt(void)
{
	XCL.INTCTRL |= XCL_PEC0IE_bm;
}

/**
 * \brief Disable XCL PEC0 timer underflow interupt.
 *
 * This function disables the XCL PEC0 underflow interupt.
 *
 */
static inline void xcl_pec0_disable_interrupt(void)
{
	XCL.INTCTRL &= ~XCL_PEC0IE_bm;
}

/**
 * \brief Enable XCL PEC1 timer underflow interupt.
 *
 * This function enables the XCL PEC1 underflow interupt.
 *
 */
static inline void xcl_pec1_enable_interrupt(void)
{
	XCL.INTCTRL |= XCL_PEC1IE_bm;
}

/**
 * \brief Disable XCL PEC1 timer underflow interupt.
 *
 * This function disables the XCL PEC1 underflow interupt.
 *
 */
static inline void xcl_pec1_disable_interrupt(void)
{
	XCL.INTCTRL &= ~XCL_PEC1IE_bm;
}

/**
 * \brief Enable XCL TC16 timer capture/compare interupt.
 *
 * This function enables the XCL TC16 capture/compate interupt.
 *
 */
static inline void xcl_tc16_enable_cc_interrupt(void)
{
	XCL.INTCTRL |= XCL_CC0IE_bm;
}

/**
 * \brief Disable XCL TC16 timer capture/compare interupt.
 *
 * This function disables the XCL TC16 capture/compate interupt.
 *
 */
static inline void xcl_tc16_disable_cc_interrupt(void)
{
	XCL.INTCTRL &= ~XCL_CC0IE_bm;
}

/**
 * \brief Enable XCL BTC1 timer capture/compare interupt.
 *
 * This function enables the XCL BCT1 capture/compate interupt.
 *
 */
static inline void xcl_btc1_enable_cc_interrupt(void)
{
	XCL.INTCTRL |= XCL_CC1IE_bm;
}

/**
 * \brief Disable XCL BTC1 timer capture/compare interupt.
 *
 * This function disables the XCL BTC1 capture/compate interupt.
 *
 */
static inline void xcl_btc1_disable_cc_interrupt(void)
{
	XCL.INTCTRL &= ~XCL_CC1IE_bm;
}

/**
 * \brief Enable XCL BTC0 timer capture/compare interupt.
 *
 * This function enables the XCL BTC0 capture/compate interupt.
 *
 */
static inline void xcl_btc0_enable_cc_interrupt(void)
{
	XCL.INTCTRL |= XCL_CC0IE_bm;
}

/**
 * \brief Disable XCL BTC0 timer capture/compare interupt.
 *
 * This function disables the XCL BTC0 capture/compate interupt.
 *
 */
static inline void xcl_btc0_disable_cc_interrupt(void)
{
	XCL.INTCTRL &= ~XCL_CC0IE_bm;
}

/**
 * \brief Clear XCL TC16 timer underflow interupt flag.
 *
 * This function clears the XCL TC16 underflow interupt flag.
 *
 */
static inline void xcl_tc16_clear_underflow_interrupt(void)
{
	XCL.INTFLAGS |= XCL_UNF0IF_bm;
}

/**
 * \brief Clear XCL BTC0 timer underflow interupt flag.
 *
 * This function clears the XCL BTC0 underflow interupt flag.
 *
 */
static inline void xcl_btc0_clear_underflow_interrupt(void)
{
	XCL.INTFLAGS |= XCL_UNF0IF_bm;
}

/**
 * \brief Clear XCL BTC1 timer underflow interupt flag.
 *
 * This function clears the XCL BTC1 underflow interupt flag.
 *
 */
static inline void xcl_btc1_clear_underflow_interrupt(void)
{
	XCL.INTFLAGS |= XCL_UNF1IF_bm;
}

/**
 * \brief Clear XCL PEC0 timer underflow interupt flag.
 *
 * This function clears the XCL PEC0 underflow interupt flag.
 *
 */
static inline void xcl_pec0_clear_interrupt(void)
{
	XCL.INTFLAGS |= XCL_PEC0IF_bm;
}

/**
 * \brief Clear XCL PEC1 timer underflow interupt flag.
 *
 * This function clears the XCL PEC1 underflow interupt flag.
 *
 */
static inline void xcl_pec1_clear_interrupt(void)
{
	XCL.INTFLAGS |= XCL_PEC1IF_bm;
}

/**
 * \brief Clear XCL TC16 timer capture/compare interupt flag.
 *
 * This function clears the XCL TC16 capture/compare interupt flag.
 *
 */
static inline void xcl_tc16_clear_cc_interrupt(void)
{
	XCL.INTFLAGS |= XCL_CC0IF_bm;
}

/**
 * \brief Clear XCL BTC0 timer capture/compare interupt flag.
 *
 * This function clears the XCL BTC0 capture/compare interupt flag.
 *
 */
static inline void xcl_btc0_clear_cc_interrupt(void)
{
	XCL.INTFLAGS |= XCL_CC0IF_bm;
}

/**
 * \brief Clear XCL BT1 timer capture/compare interupt flag.
 *
 * This function clears the XCL BTC1 capture/compare interupt flag.
 *
 */
static inline void xcl_btc1_clear_cc_interrupt(void)
{
	XCL.INTFLAGS |= XCL_CC1IF_bm;
}

/** @} */ /* xcl_tc_it */

/**
 * \brief Set XCL TC16 timer count value.
 *
 * This function loads the XCL TC16 count to a specific value.
 *
 * \param count The count value
 */
static inline void xcl_tc16_set_count(uint16_t count)
{
	XCL.CNTL = LSB(count);
	XCL.CNTH = MSB(count);
}

/**
 * \brief Set XCL BTC0 timer count value.
 *
 * This function loads the XCL BTC0 count to a specific value.
 *
 * \param count The count value
 */
static inline void xcl_btc0_set_count(uint8_t count)
{
	XCL.CNTL = count;
}

/**
 * \brief Set XCL BTC1 timer count value.
 *
 * This function loads the XCL BTC1 count to a specific value.
 *
 * \param count The count value
 */
static inline void xcl_btc1_set_count(uint8_t count)
{
	XCL.CNTH = count;
}

/**
 * \brief Set XCL PEC0 timer count value.
 *
 * This function loads the XCL PEC0 count to a specific value.
 *
 * \param count The count value
 */
static inline void xcl_pec0_set_count(uint8_t count)
{
	XCL.CNTL = count;
}

/**
 * \brief Set XCL PEC1 timer count value.
 *
 * This function loads the XCL PEC1 count to a specific value.
 *
 * \param count The count value
 */
static inline void xcl_pec1_set_count(uint8_t count)
{
	XCL.CNTH = count;
}

/**
 * \brief Set XCL PEC lenght.
 *
 * This function configures the frame lenght when XCL in PEC mode is used with
 * USART or SPI module to generate extended variable frame lenght.
 *
 * \param lenght The PEC frame lenght (in bit number)
 */
static inline void xcl_pec_set_lenght(uint8_t lenght)
{
	XCL.PLC = lenght;
}

/**
 * \ingroup xcl_tc_group
 * \defgroup xcl_tc_pwm XMEGA Custom Logic (XCL) T/C in Single Slope PWM mode
 *
 * This group provides functions that can be used only when XCL TC sub-module is
 * in SINGLE SLOPE PWM mode.
 *
 * @{
 */

/**
 * \brief Set XCL TC16 compare value.
 *
 * This function loads the XCL TC16 compare register with a requested value.
 *
 * \param compare The XCL TC16 compare value
 */
static inline void xcl_tc16_set_compare(uint16_t compare)
{
	XCL.CMPL = LSB(compare);
	XCL.CMPH = MSB(compare);
}

/**
 * \brief Set XCL BTC0 compare value.
 *
 * This function loads the XCL BTC0 compare register with a requested value.
 *
 * \param compare The XCL BTC0 compare value
 */
static inline void xcl_btc0_set_compare(uint8_t compare)
{
	XCL.CMPL = LSB(compare);
}

/**
 * \brief Set XCL BTC1 compare value.
 *
 * This function loads the XCL BTC1 compare register with a requested value.
 *
 * \param compare The XCL BTC1 compare value
 */
static inline void xcl_btc1_set_compare(uint8_t compare)
{
	XCL.CMPH = LSB(compare);
}

/** @} */ /* xcl_tc_pwm */

/**
 * \ingroup xcl_tc_group
 * \defgroup xcl_tc_capture XMEGA Custom Logic (XCL) T/C in Capture mode
 *
 * This group provides functions that can be used only when XCL TC sub-module is
 * in CAPTURE mode.
 *
 * @{
 */

/**
 * \brief Get the XCL TC16 capture value.
 *
 * This function returns the XCL TC16 capture value.
 *
 * \retval The capture value
 */
static inline uint16_t  xcl_tc16_get_capture(void)
{
	return (XCL.PERCAPTH << 8) + XCL.PERCAPTL;
}

/**
 * \brief Get the XCL BT0 capture value.
 *
 * This function returns the XCL BTC0 capture value.
 *
 * \retval The capture value
 */
static inline uint8_t  xcl_btc0_get_capture(void)
{
	return XCL.PERCAPTL;
}

/**
 * \brief Get the XCL BTC1 capture value.
 *
 * This function returns the XCL BTC1 capture value.
 *
 * \retval The capture value
 */
static inline uint8_t  xcl_btc1_get_capture(void)
{
	return XCL.PERCAPTH;
}

/** @} */ /* xcl_tc_capture */

/**
 * \ingroup xcl_tc_group
 * \defgroup xcl_tc_normal XMEGA Custom Logic (XCL) T/C in Normal mode
 *
 * This group provides functions that can be used only when XCL TC sub-module is
 * in NORMAL mode.
 *
 * @{
 */

/**
 * \brief Set XCL TC16 period.
 *
 * This function configures the XCL TC16 period.
 *
 * \param period The XCL TC16 requested period
 */
static inline void xcl_tc16_set_period(uint16_t period)
{
	XCL.PERCAPTL = LSB(period);
	XCL.PERCAPTH = MSB(period);
}

/**
 * \brief Set XCL BTC0 period.
 *
 * This function configures the XCL BTC0 period.
 *
 * \param period The XCL BTC0 requested period
 */
static inline void xcl_btc0_set_period(uint8_t period)
{
	XCL.PERCAPTL = LSB(period);
}

/**
 * \brief Set XCL BTC1 period.
 *
 * This function configures the XCL BTC1 period.
 *
 * \param period The XCL BTC1 requested period
 */
static inline void xcl_btc1_set_period(uint8_t period)
{
	XCL.PERCAPTH = MSB(period);
}

/** @} */ /* xcl_tc_normal */

uint32_t xcl_tc_set_resolution(uint32_t resolution);

/** @} */

#ifdef __cplusplus
}
#endif

#endif  /* _XCL_TIMER_H_ */
