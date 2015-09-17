/**
 * \file
 *
 * \brief AVR XMEGA Timer Counter type 4 or 5 (TC4/5) driver
 *
 * Copyright (c) 2012-2015 Atmel Corporation. All rights reserved.
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
#ifndef TC45_H
#define TC45_H

#include "asf.h"

#ifdef __cplusplus
extern "C" {
#endif

// Fix for IAR header files (missing/wrong enums)

/* Fault A Halt Action Selection */
/* Halt Action Disabled */
#define tmpfix_FAULT_HALTA_DISABLE_gc  (0x00 << 5)
/* Hardware Halt Action */
#define tmpfix_FAULT_HALTA_HW_gc  (0x01 << 5)
/* Software Halt Action */
#define tmpfix_FAULT_HALTA_SW_gc  (0x02 << 5)

/* Fault B Halt Action Selection */
/* Halt Action Disabled */
#define tmpfix_FAULT_HALTB_DISABLE_gc  (0x00 << 5)
/* Hardware Halt Action */
#define tmpfix_FAULT_HALTB_HW_gc  (0x01 << 5)
/* Software Halt Action */
#define tmpfix_FAULT_HALTB_SW_gc  (0x02 << 5)

/* Channel index Command */
/* Command Disabled */
#define tmpfix_FAULT_IDXCMD_DISABLE_gc  (0x00 << 3)
/* Force Cycle B in Next Cycle */
#define tmpfix_FAULT_IDXCMD_SET_gc  (0x01 << 3)
/* Force Cycle A in Next Cycle */
#define tmpfix_FAULT_IDXCMD_CLEAR_gc  (0x02 << 3)
/* Hold Current Cycle Index in Next Cycle */
#define tmpfix_FAULT_IDXCMD_HOLD_gc  (0x03 << 3)

/* Fix IAREW V6.12 which contains wrong values */
/* Single Slope */
#define tmpfix_TC45_WGMODE_SINGLESLOPE_gc (0x03<<0)
/* Dual Slope, Update on TOP */
#define tmpfix_TC45_WGMODE_DSTOP_gc       (0x05<<0)

// End of fix

/**
 * \defgroup tc45_group Timer Counter type 4/5(TC4/5)
 *
 * See \ref xmega_tc45_quickstart
 *
 * This is a driver for the AVR XMEGA Timer Counter type 4 and 5(TC4/5).
 * It provides functions for enabling, disabling and configuring the TC4/5
 * modules.
 *
 * \section dependencies Dependencies
 * This driver depends on the following modules:
 * - \ref sysclk_group for peripheral clock control.
 * - \ref sleepmgr_group for setting allowed sleep mode.
 * - \ref interrupt_group for ISR definition and disabling interrupts during
 * critical code sections.
 * @{
 */

/**
 * \brief Interrupt event callback function type
 *
 * The interrupt handler can be configured to do a function callback,
 * the callback function must match the tc45_callback_t type.
 *
 */
typedef void (*tc45_callback_t)(void);

/* ! Timer Counter Capture Compare Channel index */
enum tc45_cc_channel_t {
	/* ! Channel A */
	TC45_CCA = 1,
	/* ! Channel B */
	TC45_CCB = 2,
	/* ! Channel C */
	TC45_CCC = 3,
	/* ! Channel D */
	TC45_CCD = 4,
};

/* !  Timer Counter Capture Compare Channel Mode index */
enum tc45_cc_channel_mask_enable_t {
	/* ! Channel A Enable mask */
	TC45_CCACOMP = TC45_CCAMODE_COMP_gc,
	TC45_CCACAPT = TC45_CCAMODE_CAPT_gc,
	/* ! Channel B Enable mask */
	TC45_CCBCOMP = TC45_CCBMODE_COMP_gc,
	TC45_CCBCAPT = TC45_CCCMODE_CAPT_gc,
	/* ! Channel C Enable mask */
	TC45_CCCCOMP = TC45_CCCMODE_COMP_gc,
	TC45_CCCCAPT = TC45_CCCMODE_CAPT_gc,
	/* ! Channel D Enable mask */
	TC45_CCDCOMP = TC45_CCDMODE_COMP_gc,
	TC45_CCDCAPT = TC45_CCDMODE_CAPT_gc,
};

/* ! Timer Counter Direction */
enum tc45_dir_t {
	/* ! Counting up */
	TC45_UP = 0,
	/* ! Down Counting B */
	TC45_DOWN = 1
};

/* ! Timer Counter Waveform Generator mode */
enum tc45_wg_mode_t {
	/* ! TC45 in normal Mode */
	TC45_WG_NORMAL = TC45_WGMODE_NORMAL_gc,
	/* ! TC45 in Frequency Generator mode */
	TC45_WG_FRQ = TC45_WGMODE_FRQ_gc,
	/* ! TC45 in single slope PWM mode */
	TC45_WG_SS = tmpfix_TC45_WGMODE_SINGLESLOPE_gc,
	/* ! TC45 in dual slope Top PWM mode */
	TC45_WG_DS_T = tmpfix_TC45_WGMODE_DSTOP_gc,
	/* ! TC45 in dual slope Top Bottom PWM mode */
	TC45_WG_DS_TB = TC45_WGMODE_DSBOTH_gc,
	/* ! TC45 in dual slope Bottom PWM mode */
	TC45_WG_DS_B = TC45_WGMODE_DSBOTTOM_gc
};

/* ! Fault input A selection */
enum fault_sourceA_mode_t {
	/* ! OTMX in normal Mode */
	FAULT_SRCA_DEFAULT = FAULT_SRCA_DISABLE_gc,
	/* ! OTMX in normal Mode */
	FAULT_SRCA_N = FAULT_SRCA_CHN_gc,
	/* ! OTMX in normal Mode */
	FAULT_SRCA_N1 = FAULT_SRCA_CHN1_gc,
	/* ! OTMX in normal Mode */
	FAULT_SRCA_NB = FAULT_SRCA_LINK_gc,
};

/* ! Fault input B selection */
enum fault_sourceB_mode_t {
	/* ! OTMX in normal Mode */
	FAULT_SRCB_DEFAULT = FAULT_SRCB_DISABLE_gc,
	/* ! OTMX in normal Mode */
	FAULT_SRCB_N = FAULT_SRCB_CHN_gc,
	/* ! OTMX in normal Mode */
	FAULT_SRCB_N1 = FAULT_SRCB_CHN1_gc,
	/* ! OTMX in normal Mode */
	FAULT_SRCB_NB = FAULT_SRCB_LINK_gc,
};

/* ! Fault input E selection */
enum fault_sourceE_mode_t {
	/* ! OTMX in normal Mode */
	FAULT_SRCE_DEFAULT = FAULT_SRCE_DISABLE_gc,
	/* ! OTMX in normal Mode */
	FAULT_SRCE_N = FAULT_SRCE_CHN_gc,
	/* ! OTMX in normal Mode */
	FAULT_SRCE_N1 = FAULT_SRCE_CHN1_gc,
	/* ! OTMX in normal Mode */
	FAULT_SRCE_N2 = FAULT_SRCE_CHN2_gc,
};

/* ! Fault Halt A selection */
enum fault_haltA_mode_t {
	/* ! OTMX in normal Mode */
	FAULT_HALTA_DEFAULT = tmpfix_FAULT_HALTA_DISABLE_gc,
	/* ! OTMX in normal Mode */
	FAULT_HALTA_HW = tmpfix_FAULT_HALTA_HW_gc,
	/* ! OTMX in normal Mode */
	FAULT_HALTA_SW = tmpfix_FAULT_HALTA_SW_gc,
};

/* ! Fault Halt B selection */
enum fault_haltB_mode_t {
	/* ! OTMX in normal Mode */
	FAULT_HALTB_DEFAULT = tmpfix_FAULT_HALTB_DISABLE_gc,
	/* ! OTMX in normal Mode */
	FAULT_HALTB_HW = tmpfix_FAULT_HALTB_HW_gc,
	/* ! OTMX in normal Mode */
	FAULT_HALTB_SW = tmpfix_FAULT_HALTB_SW_gc,
};

/* ! Wex Output Matrix mode */
enum wex_otmx_mode_t {
	/* ! OTMX in normal Mode */
	WEX_OTMX_DEFAULT = WEX_OTMX_DEFAULT_gc,
	/* ! OTMX in normal Mode */
	WEX_OTMX_1 = WEX_OTMX_FIRST_gc,
	/* ! OTMX in normal Mode */
	WEX_OTMX_2 = WEX_OTMX_SECOND_gc,
	/* ! OTMX in normal Mode */
	WEX_OTMX_3 = WEX_OTMX_THIRD_gc,
	/* ! OTMX in normal Mode */
	WEX_OTMX_4 = WEX_OTMX_FOURTH_gc,
};

/* ! TC45 interrupt levels */
enum TC45_INT_LEVEL_t {
	TC45_INT_LVL_OFF = 0x00,
	TC45_INT_LVL_LO = 0x01,
	TC45_INT_LVL_MED = 0x02,
	TC45_INT_LVL_HI = 0x03,
};

/* ! Macro to check if the type of passed TC45 is TC5_t */
#define tc45_is_TC5(void) ((uint16_t)tc & 0x40 ? true : false)
/* ! Macro to check if the type of passed TC45 is TC4_t */
#define tc45_is_TC4(void) ((uint16_t)tc & 0x40 ? false : true)

/**
 * \brief Enable TC45
 *
 * Enables the TC45.
 *
 * \param tc Pointer to TC45 module
 *
 * \note
 * unmask TC45 clock (sysclk), but does not configure the TC45 clock source.
 */
void tc45_enable(volatile void *tc);

/**
 * \brief Disable TC45
 *
 * Disables the TC45.
 *
 * \param tc Pointer to TC45 module
 *
 * \note
 * mask TC45 clock (sysclk).
 */
void tc45_disable(volatile void *tc);

/**
 * \ingroup tc45_group
 * \defgroup tc45_interrupt_group Timer Counter (TC45) interrupt management
 * This group provides functions to configure TC45 module interrupts
 *
 *
 * @{
 */

/**
 * \brief Set TC45 overflow interrupt callback function
 *
 * This function allows the caller to set and change the interrupt callback
 * function. Without setting a callback function the interrupt handler in the
 * driver will only clear the interrupt flags.
 *
 * \note Once a callback function is set, the interrupt priority must be set
 *       via \ref tc45_set_overflow_interrupt_level() for interrupts to be generated
 *       each time the timer overflow event occurs.
 *
 * \param tc Pointer to the Timer Counter (TC) base address
 * \param callback Reference to a callback function
 */
void tc45_set_overflow_interrupt_callback(volatile void *tc,
		tc45_callback_t callback);

/**
 * \brief Set TC45 error interrupt callback function
 *
 * This function allows the caller to set and change the interrupt callback
 * function. Without setting a callback function the interrupt handler in the
 * driver will only clear the interrupt flags.
 *
 * \note Once a callback function is set, the interrupt priority must be set
 *       via \ref tc45_set_error_interrupt_level() for interrupts to be
 *       generated each time a timer error occurs.
 *
 * \param tc Pointer to the Timer Counter (TC) base address
 * \param callback Reference to a callback function
 */
void tc45_set_error_interrupt_callback(volatile void *tc,
		tc45_callback_t callback);

/**
 * \brief Set TC45 Capture Compare Channel A interrupt callback function
 *
 * This function allows the caller to set and change the interrupt callback
 * function. Without setting a callback function the interrupt handler in the
 * driver will only clear the interrupt flags.
 *
 * \note Once a callback function is set, the interrupt priority must be set
 *       via \ref tc45_set_cca_interrupt_level() for interrupts to be generated
 *       each time the timer channel A compare matches the current timer count.
 *
 * \param tc Pointer to the Timer Counter (TC) base address
 * \param callback Reference to a callback function
 */
void tc45_set_cca_interrupt_callback(volatile void *tc,
		tc45_callback_t callback);

/**
 * \brief Set TC45 Capture Compare Channel B interrupt callback function
 *
 * This function allows the caller to set and change the interrupt callback
 * function. Without setting a callback function the interrupt handler in the
 * driver will only clear the interrupt flags.
 *
 * \note Once a callback function is set, the interrupt priority must be set
 *       via \ref tc45_set_ccb_interrupt_level() for interrupts to be generated
 *       each time the timer channel B compare matches the current timer count.
 *
 * \param tc Pointer to the Timer Counter (TC) base address
 * \param callback Reference to a callback function
 */
void tc45_set_ccb_interrupt_callback(volatile void *tc,
		tc45_callback_t callback);

/**
 * \brief Set TC45 Capture Compare Channel C interrupt callback function
 *
 * This function allows the caller to set and change the interrupt callback
 * function. Without setting a callback function the interrupt handler in the
 * driver will only clear the interrupt flags.
 *
 * \note Once a callback function is set, the interrupt priority must be set
 *       via \ref tc45_set_ccc_interrupt_level() for interrupts to be generated
 *       each time the timer channel C compare matches the current timer count.
 *
 * \param tc Pointer to the Timer Counter (TC) base address
 * \param callback Reference to a callback function
 */
void tc45_set_ccc_interrupt_callback(volatile void *tc,
		tc45_callback_t callback);

/**
 * \brief Set TC45 Capture Compare Channel D interrupt callback function
 *
 * This function allows the caller to set and change the interrupt callback
 * function. Without setting a callback function the interrupt handler in the
 * driver will only clear the interrupt flags.
 *
 * \note Once a callback function is set, the interrupt priority must be set
 *       via \ref tc45_set_ccd_interrupt_level() for interrupts to be generated
 *       each time the timer channel D compare matches the current timer count.
 *
 * \param tc Pointer to the Timer Counter (TC) base address
 * \param callback Reference to a callback function
 */
void tc45_set_ccd_interrupt_callback(volatile void *tc,
		tc45_callback_t callback);

/**
 * \brief Configures TC45 overflow Interrupt level
 *
 * \param tc Pointer to TC45 module.
 * \param level Overflow interrupt level
 * \note  Configures OVFINTLVL in INTCTRLA
 */
static inline void tc45_set_overflow_interrupt_level(volatile void *tc,
		enum TC45_INT_LEVEL_t level)
{
	if (tc45_is_TC4(void *tc)) {
		((TC4_t *)tc)->INTCTRLA = ((TC4_t *)tc)->INTCTRLA &
				~TC4_OVFINTLVL_gm;
		((TC4_t *)tc)->INTCTRLA
			= ((TC4_t *)tc)->INTCTRLA | (level << TC4_OVFINTLVL_gp);
	} else if (tc45_is_TC5(void *tc)) {
		((TC5_t *)tc)->INTCTRLA = ((TC5_t *)tc)->INTCTRLA &
				~TC5_OVFINTLVL_gm;
		((TC5_t *)tc)->INTCTRLA
			= ((TC5_t *)tc)->INTCTRLA | (level << TC5_OVFINTLVL_gp);
	}
}

/**
 * \brief Configures TC45 error Interrupt level
 *
 * \param tc Pointer to TC45 module.
 * \param level Error interrupt level
 * \note  Configures ERRINTLVL in INTCTRLA
 */
static inline void tc45_set_error_interrupt_level(volatile void *tc,
		enum TC45_INT_LEVEL_t level)
{
	if (tc45_is_TC4(void *tc)) {
		((TC4_t *)tc)->INTCTRLA = ((TC4_t *)tc)->INTCTRLA &
				~TC4_ERRINTLVL_gm;
		((TC4_t *)tc)->INTCTRLA
			= ((TC4_t *)tc)->INTCTRLA | (level << TC4_ERRINTLVL_gp);
	} else if (tc45_is_TC5(void *tc)) {
		((TC5_t *)tc)->INTCTRLA = ((TC5_t *)tc)->INTCTRLA &
				~TC5_ERRINTLVL_gm;
		((TC5_t *)tc)->INTCTRLA
			= ((TC5_t *)tc)->INTCTRLA | (level << TC5_ERRINTLVL_gp);
	}
}

/**
 * \brief Configures TC45 Capture Compare A Interrupt level
 *
 * \param tc Pointer to TC45 module.
 * \param level CCA interrupt level
 * \note Configures CCAINTLVL in INTCTRLB
 */
static inline void tc45_set_cca_interrupt_level(volatile void *tc,
		enum TC45_INT_LEVEL_t level)
{
	if (tc45_is_TC4(void *tc)) {
		((TC4_t *)tc)->INTCTRLB = ((TC4_t *)tc)->INTCTRLB &
				~TC4_CCAINTLVL_gm;
		((TC4_t *)tc)->INTCTRLB
			= ((TC4_t *)tc)->INTCTRLB | (level << TC4_CCAINTLVL_gp);
	} else if (tc45_is_TC5(void *tc)) {
		((TC5_t *)tc)->INTCTRLB = ((TC5_t *)tc)->INTCTRLB &
				~TC5_CCAINTLVL_gm;
		((TC5_t *)tc)->INTCTRLB
			= ((TC5_t *)tc)->INTCTRLB | (level << TC5_CCAINTLVL_gp);
	}
}

/**
 * \brief Configures TC45 Capture Compare B Interrupt level
 *
 * \param tc Pointer to TC45 module.
 * \param level CCB interrupt level
 * \note Configures CCBINTLVL in INTCTRLB
 */
static inline void tc45_set_ccb_interrupt_level(volatile void *tc,
		enum TC45_INT_LEVEL_t level)
{
	if (tc45_is_TC4(void *tc)) {
		((TC4_t *)tc)->INTCTRLB = ((TC4_t *)tc)->INTCTRLB &
				~TC4_CCBINTLVL_gm;
		((TC4_t *)tc)->INTCTRLB
			= ((TC4_t *)tc)->INTCTRLB | (level << TC4_CCBINTLVL_gp);
	} else if (tc45_is_TC5(void *tc)) {
		((TC5_t *)tc)->INTCTRLB = ((TC5_t *)tc)->INTCTRLB &
				~TC5_CCBINTLVL_gm;
		((TC5_t *)tc)->INTCTRLB
			= ((TC5_t *)tc)->INTCTRLB | (level << TC5_CCBINTLVL_gp);
	}
}

/**
 * \brief Configures TC45 Capture Compare C Interrupt level
 *
 * \param tc Pointer to TC45 module.
 * \param level CCC interrupt level
 * \note Configures CCCINTLVL in INTCTRLB
 */
static inline void tc45_set_ccc_interrupt_level(volatile void *tc,
		enum TC45_INT_LEVEL_t level)
{
	((TC4_t *)tc)->INTCTRLB = ((TC4_t *)tc)->INTCTRLB & ~TC4_CCCINTLVL_gm;
	((TC4_t *)tc)->INTCTRLB
		= ((TC4_t *)tc)->INTCTRLB | (level << TC4_CCCINTLVL_gp);
}

/**
 * \brief Configures TC45 Capture Compare D Interrupt level
 *
 * \param tc Pointer to TC45 module.
 * \param level CCD interrupt level
 * \note Configures CCDINTLVL in INTCTRLB
 */
static inline void tc45_set_ccd_interrupt_level(volatile void *tc,
		enum TC45_INT_LEVEL_t level)
{
	((TC4_t *)tc)->INTCTRLB = ((TC4_t *)tc)->INTCTRLB & ~TC4_CCDINTLVL_gm;
	((TC4_t *)tc)->INTCTRLB
		= ((TC4_t *)tc)->INTCTRLB | (level << TC4_CCDINTLVL_gp);
}

/* @} */

/**
 * \brief Configures Synchronization on prescaler
 *
 * \param tc Pointer to TC45 module.
 * \param
 * \note Configures SYNCHEN in CTRLA
 */
static inline void tc45_write_synchen(volatile void *tc)
{
	if (tc45_is_TC4(void *tc)) {
		((TC4_t *)tc)->CTRLA
			= (((TC4_t *)tc)->CTRLA | 0x40);
	} else if (tc45_is_TC5(void *tc)) {
		((TC5_t *)tc)->CTRLA
			= (((TC5_t *)tc)->CTRLA | 0x40);
	}
}

/**
 * \brief Set Evstart bit
 *
 * \param tc Pointer to TC45 module.
 * \note
 * \note Configures EVSTART in CTRLA
 */
static inline void tc45_set_evstart(volatile void *tc)
{
	if (tc45_is_TC4(void *tc)) {
		((TC4_t *)tc)->CTRLA = (((TC4_t *)tc)->CTRLA | TC4_EVSTART_bm);
	} else if (tc45_is_TC5(void *tc)) {
		((TC5_t *)tc)->CTRLA = (((TC5_t *)tc)->CTRLA | TC5_EVSTART_bm);
	}
}

/**
 * \brief Set Upstop bit
 *
 * \param tc Pointer to TC45 module.
 * \note
 * \note Configures UPSTOP in CTRLA
 */
static inline void tc45_set_up3(volatile void *tc)
{
	if (tc45_is_TC4(void *tc)) {
		((TC4_t *)tc)->CTRLA = (((TC4_t *)tc)->CTRLA | TC4_UPSTOP_bm);
	} else if (tc45_is_TC5(void *tc)) {
		((TC5_t *)tc)->CTRLA = (((TC5_t *)tc)->CTRLA | TC5_UPSTOP_bm);
	}
}

/**
 * \brief Set Stop bit
 *
 * \param tc Pointer to TC45 module.
 * \note
 * \note Configures STOP in CTRLGSET
 */
static inline void tc45_set_stop(volatile void *tc)
{
	if (tc45_is_TC4(void *tc)) {
		((TC4_t *)tc)->CTRLGSET = TC4_STOP_bm;
	} else if (tc45_is_TC5(void *tc)) {
		((TC5_t *)tc)->CTRLGSET = TC5_STOP_bm;
	}
}

/**
 * \brief Configure Timer Clock Source
 *
 * \param tc Pointer to TC45 module.
 * \param TC45_CLKSEL_enum Clock source selection
 * \note Configuring the clock starts alos the timer
 */
static inline void tc45_write_clock_source(volatile void *tc,
		TC45_CLKSEL_t TC45_CLKSEL_enum)
{
	if (tc45_is_TC4(void *tc)) {
		((TC4_t *)tc)->CTRLA
			= (((TC4_t *)tc)->CTRLA & ~TC4_CLKSEL_gm) |
				TC45_CLKSEL_enum;
	} else if (tc45_is_TC5(void *tc)) {
		((TC5_t *)tc)->CTRLA
			= (((TC5_t *)tc)->CTRLA & ~TC5_CLKSEL_gm) |
				TC45_CLKSEL_enum;
	}
}

/**
 * \brief Read Timer Clock Source
 *
 * \param tc Pointer to TC45 module.
 *
 * \return TC45_CLKSEL_enum Clock source selection
 */
static inline TC45_CLKSEL_t tc45_read_clock_source(volatile void *tc)
{
	if (tc45_is_TC4(void *tc)) {
		return (TC45_CLKSEL_t)(((TC4_t *)tc)->CTRLA & TC4_CLKSEL_gm);
	} else if (tc45_is_TC5(void *tc)) {
		return (TC45_CLKSEL_t)(((TC5_t *)tc)->CTRLA & TC5_CLKSEL_gm);
	}

	Assert(false);
	return TC45_CLKSEL_OFF_gc;
}

/**
 * \brief Select clock for a specified TC45 and resolution.
 *
 * This function configures the clock selection, as prescaled CLKper, for a
 * specified TC45 that gives a resolution at least as high as the one specified.
 * The resolution of a TC45 is synonymous with its clock frequency.
 *
 * \note It is also possible to clock TCs with event channels. This is not
 * handled by this implementation.
 *
 * \param tc   ID of TC45 to get clock selection for.
 * \param resolution Desired resolution for the TC45 in Hz.
 */
static inline void tc45_set_resolution(volatile void *tc, uint32_t resolution)
{
	uint32_t tc45_clk_rate = sysclk_get_per_hz();

	if (resolution <= (tc45_clk_rate / 1024)) {
		tc45_write_clock_source(tc, TC45_CLKSEL_DIV1024_gc);
	} else if (resolution <= (tc45_clk_rate / 256)) {
		tc45_write_clock_source(tc, TC45_CLKSEL_DIV256_gc);
	} else if (resolution <= (tc45_clk_rate / 64)) {
		tc45_write_clock_source(tc, TC45_CLKSEL_DIV64_gc);
	} else if (resolution <= (tc45_clk_rate / 8)) {
		tc45_write_clock_source(tc, TC45_CLKSEL_DIV8_gc);
	} else if (resolution <= (tc45_clk_rate / 4)) {
		tc45_write_clock_source(tc, TC45_CLKSEL_DIV4_gc);
	} else if (resolution <= (tc45_clk_rate / 2)) {
		tc45_write_clock_source(tc, TC45_CLKSEL_DIV2_gc);
	} else {
		tc45_write_clock_source(tc, TC45_CLKSEL_DIV1_gc);
	}
}

/**
 * \brief Get real resolution for a specified TC45.
 *
 * This function returns the resolution which the specified clock selection
 * of TC45 will result in. The resolution of a TC45 is synonymous with its clock
 * frequency.
 *
 * \note This function does not handle event channel clock selections.
 *
 * \param tc Pointer of TC45 module to get resolution for.
 *
 * \return The resolution of a tc.
 */
static inline uint32_t tc45_get_resolution(volatile void *tc)
{
	uint32_t tc45_clk_rate = sysclk_get_per_hz();
	switch (tc45_read_clock_source(tc)) {
	case TC45_CLKSEL_OFF_gc:
		tc45_clk_rate = 0;
		break;

	case TC45_CLKSEL_DIV1024_gc:
		tc45_clk_rate /= 1024;
		break;

	case TC45_CLKSEL_DIV256_gc:
		tc45_clk_rate /= 256;
		break;

	case TC45_CLKSEL_DIV64_gc:
		tc45_clk_rate /= 64;
		break;

	case TC45_CLKSEL_DIV8_gc:
		tc45_clk_rate /= 8;
		break;

	case TC45_CLKSEL_DIV4_gc:
		tc45_clk_rate /= 4;
		break;

	case TC45_CLKSEL_DIV2_gc:
		tc45_clk_rate /= 2;
		break;

	case TC45_CLKSEL_DIV1_gc:
		break;

	default:
		tc45_clk_rate = 0;
		break;
	}
	return (tc45_clk_rate);
}

/**
 * \brief Configure Timer Direction
 *
 * \param tc Pointer to TC45 module.
 * \param dir Timer direction
 */
static inline void tc45_set_direction(volatile void *tc, enum tc45_dir_t dir)
{
	if (dir == TC45_UP) {
		if (tc45_is_TC4(void *tc)) {
			((TC4_t *)tc)->CTRLGCLR = TC4_DIR_bm;
		} else if (tc45_is_TC5(void *tc)) {
			((TC5_t *)tc)->CTRLGCLR = TC4_DIR_bm;
		}
	} else {
		if (tc45_is_TC4(void *tc)) {
			((TC4_t *)tc)->CTRLGSET = TC4_DIR_bm;
		} else if (tc45_is_TC5(void *tc)) {
			((TC5_t *)tc)->CTRLGSET = TC5_DIR_bm;
		}
	}
}

/**
 * \brief Writes the Counter value of the Timer
 *
 * \param tc Pointer to TC45 module.
 * \param cnt_value Counter value
 */
static inline void tc45_write_count(volatile void *tc, uint16_t cnt_value)
{
	if (tc45_is_TC4(void *tc)) {
		((TC4_t *)tc)->CNT = cnt_value;
	} else if (tc45_is_TC5(void *tc)) {
		((TC5_t *)tc)->CNT = cnt_value;
	}
}

/**
 * \brief Reads the Counter value of the Timer
 *
 * \param tc Pointer to TC45 module.
 * \note Output the Counter value CNT
 *
 * \return The counter (CNT) value of a tc.
 */
static inline uint16_t tc45_read_count(volatile void *tc)
{
	if (tc45_is_TC4(void *tc)) {
		return (((TC4_t *)tc)->CNT);
	} else if (tc45_is_TC5(void *tc)) {
		return (((TC5_t *)tc)->CNT);
	}

	Assert(false);
	return 0;
}

/**
 * \brief Writes the Period value of the Timer
 *
 * \param tc Pointer to TC45 module.
 * \param per_value Period value : PER
 */
static inline void tc45_write_period(volatile void *tc, uint16_t per_value)
{
	if (tc45_is_TC4(void *tc)) {
		((TC4_t *)tc)->PER = per_value;
	} else if (tc45_is_TC5(void *tc)) {
		((TC5_t *)tc)->PER = per_value;
	}
}

/**
 * \brief Reads the Period value of the Timer
 *
 * \param tc Pointer to TC45 module.
 *
 * \return Period value : PER
 */
static inline uint16_t tc45_read_period(volatile void *tc)
{
	if (tc45_is_TC4(void *tc)) {
		return (((TC4_t *)tc)->PER);
	} else if (tc45_is_TC5(void *tc)) {
		return (((TC5_t *)tc)->PER);
	}

	Assert(false);
	return 0;
}

/**
 * \brief Writes the Period Buffer value of the Timer
 *
 * \param tc Pointer to TC45 module.
 * \param per_buf Period Buffer value : PERH/PERL
 */
static inline void tc45_write_period_buffer(volatile void *tc, uint16_t per_buf)
{
	if (tc45_is_TC4(void *tc)) {
		((TC4_t *)tc)->PERBUF = per_buf;
	} else if (tc45_is_TC5(void *tc)) {
		((TC5_t *)tc)->PERBUF = per_buf;
	}
}

/**
 * \brief Reads the Period Buffer value of the Timer
 *
 * \param tc Pointer to TC45 module.
 *
 * \return Period Buffer value : PERH/PERL
 */
static inline uint16_t tc45_read_period_buffer(volatile void *tc)
{
	if (tc45_is_TC4(void *tc)) {
		return (((TC4_t *)tc)->PERBUF);
	} else if (tc45_is_TC5(void *tc)) {
		return (((TC5_t *)tc)->PERBUF);
	}

	Assert(false);
	return 0;
}

/**
 * \brief Tests if the Period Buffer is valid
 *
 * \param tc Pointer to TC45 module.
 *
 * \return  period Buffer is valid or not:PERBV
 */
static inline bool tc45_period_buffer_is_valid(volatile void *tc)
{
	if (tc45_is_TC4(void *tc)) {
		return (((TC4_t *)tc)->CTRLHCLR & TC4_PERBV_bm);
	} else if (tc45_is_TC5(void *tc)) {
		return (((TC5_t *)tc)->CTRLHCLR & TC5_PERBV_bm);
	}

	Assert(false);
	return false;
}

/**
 * \brief Enables delay (used for 32bit timer mode)
 *
 * \param tc Pointer to TC45 module.
 * \note  enables Delay mode
 */
static inline void tc45_enable_delay(volatile void *tc)
{
	if (tc45_is_TC4(void *tc)) {
		((TC4_t *)tc)->CTRLD = (((TC4_t *)tc)->CTRLD &
				~TC4_EVDLY_bm) | (1 << TC4_EVDLY_bp);
	} else if (tc45_is_TC5(void *tc)) {
		((TC5_t *)tc)->CTRLD = (((TC5_t *)tc)->CTRLD &
				~TC5_EVDLY_bm) | (1 << TC5_EVDLY_bp);
	}
}

/**
 * \brief Disables delay
 *
 * \param tc Pointer to TC45 module.
 * \note  disables Delay mode
 */
static inline void tc45_disable_delay(volatile void *tc)
{
	if (tc45_is_TC4(void *tc)) {
		((TC4_t *)tc)->CTRLD = ((TC4_t *)tc)->CTRLD & ~TC4_EVDLY_bm;
	} else if (tc45_is_TC5(void *tc)) {
		((TC5_t *)tc)->CTRLD = ((TC5_t *)tc)->CTRLD & ~TC5_EVDLY_bm;
	}
}

/**
 * \brief Tests if the Overflow flag is set
 *
 * \param tc Pointer to TC45 module.
 *
 * \return  overflow has occured or not : OVFIF
 */
static inline bool tc45_is_overflow(volatile void *tc)
{
	if (tc45_is_TC4(void *tc)) {
		return (((TC4_t *)tc)->INTFLAGS & TC4_OVFIF_bm);
	} else if (tc45_is_TC5(void *tc)) {
		return (((TC5_t *)tc)->INTFLAGS & TC5_OVFIF_bm);
	}

	Assert(false);
	return false;
}

/**
 * \brief Clears the Overflow flag
 *
 * \param tc Pointer to TC45 module.
 * \note  OVFIF is cleared
 */
static inline void tc45_clear_overflow(volatile void *tc)
{
	if (tc45_is_TC4(void *tc)) {
		((TC4_t *)tc)->INTFLAGS |= TC4_OVFIF_bm;
	} else if (tc45_is_TC5(void *tc)) {
		((TC5_t *)tc)->INTFLAGS |= TC5_OVFIF_bm;
	}
}

/**
 * \brief Tests if the Error flag is set
 *
 * \param tc Pointer to TC45 module.
 *
 * \return  Error has occured or not : ERRIF
 */
static inline bool tc45_read_error(volatile void *tc)
{
	if (tc45_is_TC4(void *tc)) {
		return (((TC4_t *)tc)->INTFLAGS & TC4_ERRIF_bm);
	} else if (tc45_is_TC5(void *tc)) {
		return (((TC5_t *)tc)->INTFLAGS & TC5_ERRIF_bm);
	}

	Assert(false);
	return false;
}

/**
 * \brief Clears the Error flag
 *
 * \param tc Pointer to TC45 module.
 * \note  ERRIF is cleared
 */
static inline void tc45_clear_error(volatile void *tc)
{
	if (tc45_is_TC4(void *tc)) {
		((TC4_t *)tc)->INTFLAGS |= TC4_ERRIF_bm;
	} else if (tc45_is_TC5(void *tc)) {
		((TC5_t *)tc)->INTFLAGS |= TC5_ERRIF_bm;
	}
}

/**
 * \brief Restarts the Timer
 *
 * \param tc Pointer to TC45 module.
 * \note CMD[3:2] in CTRLGSET are set to 10
 */
static inline void tc45_restart(volatile void *tc)
{
	if (tc45_is_TC4(void *tc)) {
		((TC4_t *)tc)->CTRLGSET = TC45_CMD_RESTART_gc;
	} else if (tc45_is_TC5(void *tc)) {
		((TC5_t *)tc)->CTRLGSET = TC45_CMD_RESTART_gc;
	}
}

/**
 * \brief Reset the Timer
 *
 * \param tc Pointer to TC45 module.
 * \note  CMD[3:2] in CTRLGSET are set to 11
 */
static inline void tc45_reset(volatile void *tc)
{
	if (tc45_is_TC4(void *tc)) {
		((TC4_t *)tc)->CTRLGSET = TC45_CMD_RESET_gc;
	} else if (tc45_is_TC5(void *tc)) {
		((TC5_t *)tc)->CTRLGSET = TC45_CMD_RESET_gc;
	}
}

/**
 * \brief Updates the Timer
 *
 * \param tc Pointer to TC45 module.
 * \note  CMD[3:2] in CTRLGSET are set to 01
 */
static inline void tc45_update(volatile void *tc)
{
	if (tc45_is_TC4(void *tc)) {
		((TC4_t *)tc)->CTRLGSET = TC45_CMD_UPDATE_gc;
	} else if (tc45_is_TC5(void *tc)) {
		((TC5_t *)tc)->CTRLGSET = TC45_CMD_UPDATE_gc;
	}
}

/**
 * \brief Configures the Timer in Byte mode
 *
 * \param tc Pointer to TC45 module.
 * \note Configures  BYTEM in CTRLB
 */
static inline void tc45_set_8bits_mode(volatile void *tc)
{
	if (tc45_is_TC4(void *tc)) {
#ifdef TC4_BYTEM0_bm
		((TC4_t *)tc)->CTRLB |= TC4_BYTEM0_bm;
#else
		((TC4_t *)tc)->CTRLB |= TC4_BYTEM_bm;
#endif
	} else if (tc45_is_TC5(void *tc)) {
#ifdef TC5_BYTEM0_bm
		((TC5_t *)tc)->CTRLB |= TC5_BYTEM0_bm;
#else
		((TC5_t *)tc)->CTRLB |= TC5_BYTEM_bm;
#endif
	}
}

/**
 * \brief Locks the Update of the Buffered registers
 *
 * \param tc Pointer to TC45 module.
 * \note  Configures LUPD in CTRLGSET
 *
 *  */
static inline void tc45_lock_update_buffers(volatile void *tc)
{
	if (tc45_is_TC4(void *tc)) {
		((TC4_t *)tc)->CTRLGSET = TC4_LUPD_bm;
	} else if (tc45_is_TC5(void *tc)) {
		((TC5_t *)tc)->CTRLGSET = TC5_LUPD_bm;
	}
}

/**
 * \brief Unlocks the Update of the Buffered registers
 *
 * \param tc Pointer to TC45 module.
 * \note  Configures LUPD in CTRLGCLR
 */
static inline void tc45_unlock_update_buffers(volatile void *tc)
{
	if (tc45_is_TC4(void *tc)) {
		((TC4_t *)tc)->CTRLGCLR = TC4_LUPD_bm;
	} else if (tc45_is_TC5(void *tc)) {
		((TC5_t *)tc)->CTRLGCLR = TC5_LUPD_bm;
	}
}

/**
 * \brief Change the polarity of outputs
 *
 * \param tc Pointer to TC45 module.
 * \note  Configures POL bits in CTRLC
 */
static inline void tc45_pol(volatile void *tc)
{
	if (tc45_is_TC4(void *tc)) {
		((TC4_t *)tc)->CTRLC = ((TC4_t *)tc)->CTRLC | TC4_POLD_bm |
				TC4_POLC_bm | TC4_POLB_bm | TC4_POLA_bm;
	} else if (tc45_is_TC5(void *tc)) {
		((TC5_t *)tc)->CTRLC |= ((TC5_t *)tc)->CTRLC | TC5_POLB_bm |
				TC5_POLA_bm;
	}
}

/**
 * \brief Enables Compare channel
 *
 * \param tc Pointer to TC45 module.
 * \param enablemask CC channel
 */
static inline void tc45_enable_cc_channels(volatile void *tc,
		enum tc45_cc_channel_mask_enable_t enablemask)
{
	if (tc45_is_TC4(void *tc)) {
		((TC4_t *)tc)->CTRLE |= enablemask;
	} else if (tc45_is_TC5(void *tc)) {
		((TC5_t *)tc)->CTRLE
			|= enablemask &
				((uint8_t)TC45_CCAMODE_COMP_gc |
				(uint8_t)TC45_CCBMODE_COMP_gc |
				(uint8_t)TC45_CCAMODE_CAPT_gc |
				(uint8_t)TC45_CCBMODE_CAPT_gc);
	}
}

/**
 * \brief Disables Compare/Capture channel
 *
 * \param tc Pointer to TC45 module.
 * \param disablemask CC channel
 */
static inline void tc45_disable_cc_channels(volatile void *tc,
		enum tc45_cc_channel_mask_enable_t disablemask)
{
	if (tc45_is_TC4(void *tc)) {
		((TC4_t *)tc)->CTRLE &= ~disablemask;
	} else if (tc45_is_TC5(void *tc)) {
		((TC5_t *)tc)->CTRLE
			&= ~(disablemask &
				((uint8_t)TC45_CCAMODE_COMP_gc |
				(uint8_t)TC45_CCBMODE_COMP_gc));
	}
}

/**
 * \brief Enables Input capture mode
 *
 * \param tc Pointer to TC45 module.
 * \param eventsource Source for the capture
 * \param eventaction Event action capture type
 */
static inline void tc45_set_input_capture(volatile void *tc,
		TC45_EVSEL_t eventsource, TC45_EVACT_t eventaction)
{
	if (tc45_is_TC4(void *tc)) {
		((TC4_t *)tc)->CTRLD &= ~(TC4_EVSEL_gm | TC4_EVACT_gm);
		((TC4_t *)tc)->CTRLD
			|= ((uint8_t)eventsource | (uint8_t)eventaction);
	} else if (tc45_is_TC5(void *tc)) {
		((TC5_t *)tc)->CTRLD &= ~(TC5_EVSEL_gm | TC5_EVACT_gm);
		((TC5_t *)tc)->CTRLD
			|= ((uint8_t)eventsource | (uint8_t)eventaction);
	}
}

/**
 * \brief Reads the Capture value
 *
 * \param tc Pointer to TC45 module.
 * \param channel_index Channel x
 *
 * \return  Value of CCx
 */
static inline uint16_t tc45_read_cc(volatile void *tc,
		enum tc45_cc_channel_t channel_index)
{
	if (tc45_is_TC4(void *tc)) {
		switch (channel_index) {
		case TC45_CCA:
			return (((TC4_t *)tc)->CCA);

		case TC45_CCB:
			return (((TC4_t *)tc)->CCB);

		case TC45_CCC:
			return (((TC4_t *)tc)->CCC);

		case TC45_CCD:
			return (((TC4_t *)tc)->CCD);
		}
	} else if (tc45_is_TC5(void *tc)) {
		switch (channel_index) {
		case TC45_CCA:
			return (((TC5_t *)tc)->CCA);

		case TC45_CCB:
			return (((TC5_t *)tc)->CCB);

		default:
			return (0);
		}
	}

	return (0);
}

/**
 * \brief Writes the CC value
 *
 * \param tc Pointer to TC45 module.
 * \param channel_index CC Channel
 * \param value Counter value
 */
static inline void tc45_write_cc(volatile void *tc,
		enum tc45_cc_channel_t channel_index, uint16_t value)
{
	if (tc45_is_TC4(void *tc)) {
		switch (channel_index) {
		case TC45_CCA:
			((TC4_t *)tc)->CCA = value;
			break;

		case TC45_CCB:
			((TC4_t *)tc)->CCB = value;
			break;

		case TC45_CCC:
			((TC4_t *)tc)->CCC = value;
			break;

		case TC45_CCD:
			((TC4_t *)tc)->CCD = value;
			break;
		}
	} else if (tc45_is_TC5(void *tc)) {
		switch (channel_index) {
		case TC45_CCA:
			((TC5_t *)tc)->CCA = value;
			break;

		case TC45_CCB:
			((TC5_t *)tc)->CCB = value;
			break;

		default:
			return;
		}
	}
}

/**
 * \brief Writes the Capture/Compare Buffer value
 *
 * \param tc Pointer to TC45 module.
 * \param channel_index CC Channel
 * \param buffer_value Counter Buffer value
 */
static inline void tc45_write_cc_buffer(volatile void *tc,
		enum tc45_cc_channel_t channel_index, uint16_t buffer_value)
{
	if (tc45_is_TC4(void *tc)) {
		switch (channel_index) {
		case TC45_CCA:
			((TC4_t *)tc)->CCABUF = buffer_value;
			break;

		case TC45_CCB:
			((TC4_t *)tc)->CCBBUF = buffer_value;
			break;

		case TC45_CCC:
			((TC4_t *)tc)->CCCBUF = buffer_value;
			break;

		case TC45_CCD:
			((TC4_t *)tc)->CCDBUF = buffer_value;
			break;
		}
	} else if (tc45_is_TC5(void *tc)) {
		switch (channel_index) {
		case TC45_CCA:
			((TC5_t *)tc)->CCABUF = buffer_value;
			break;

		case TC45_CCB:
			((TC5_t *)tc)->CCBBUF = buffer_value;
			break;

		default:
			return;
		}
	}
}

/**
 * \brief Reads the Capture/Compare Buffer value
 *
 * \param tc Pointer to TC45 module.
 * \param channel_index CC Channel
 *
 * \return  CCx Buffer value
 */
static inline uint16_t tc45_read_cc_buffer(volatile void *tc,
		enum tc45_cc_channel_t channel_index)
{
	if (tc45_is_TC4(void *tc)) {
		switch (channel_index) {
		case TC45_CCA:
			return (((TC4_t *)tc)->CCABUF);

		case TC45_CCB:
			return (((TC4_t *)tc)->CCBBUF);

		case TC45_CCC:
			return (((TC4_t *)tc)->CCCBUF);

		case TC45_CCD:
			return (((TC4_t *)tc)->CCDBUF);
		}
	} else if (tc45_is_TC5(void *tc)) {
		switch (channel_index) {
		case TC45_CCA:
			return (((TC5_t *)tc)->CCABUF);

		case TC45_CCB:
			return (((TC5_t *)tc)->CCBBUF);

		default:
			return (0);
		}
	}

	return (0);
}

/**
 * \brief Reports if Capture/Compare Buffer is valid
 *
 * \param tc Pointer to TC45 module.
 * \param channel_index CC Channel
 *
 * \return  CCx Buffer is valid or not
 */
static inline bool tc45_cc_buffer_is_valid(volatile void *tc,
		enum tc45_cc_channel_t channel_index)
{
	if (tc45_is_TC4(void *tc)) {
		switch (channel_index) {
		case TC45_CCA:
			return ((TC4_t *)tc)->CTRLGCLR & TC4_CCABV_bm;

		case TC45_CCB:
			return ((TC4_t *)tc)->CTRLGCLR & TC4_CCBBV_bm;

		case TC45_CCC:
			return ((TC4_t *)tc)->CTRLGCLR & TC4_CCCBV_bm;

		case TC45_CCD:
			return ((TC4_t *)tc)->CTRLGCLR & TC4_CCDBV_bm;
		}
	} else if (tc45_is_TC5(void *tc)) {
		switch (channel_index) {
		case TC45_CCA:
			return (((TC5_t *)tc)->CTRLGCLR &
			       TC5_CCABV_bm);

		case TC45_CCB:
			return (((TC5_t *)tc)->CTRLGCLR &
			       TC5_CCBBV_bm);

		default:
			return (0);
		}
	}

	return (0);
}

/**
 * \brief Reports if Capture/Compare interrupt has occured
 *
 * \param tc Pointer to TC45 module.
 * \param channel_index CC Channel
 *
 * \return  CCx Interrupt or not
 */
static inline bool tc45_is_cc_interrupt(volatile void *tc,
		enum tc45_cc_channel_t channel_index)
{
	if (tc45_is_TC4(void *tc)) {
		switch (channel_index) {
		case TC45_CCA:
			return (((TC4_t *)tc)->INTFLAGS & TC4_CCAIF_bm);

		case TC45_CCB:
			return (((TC4_t *)tc)->INTFLAGS & TC4_CCBIF_bm);

		case TC45_CCC:
			return (((TC4_t *)tc)->INTFLAGS & TC4_CCCIF_bm);

		case TC45_CCD:
			return (((TC4_t *)tc)->INTFLAGS & TC4_CCDIF_bm);
		}
	} else if (tc45_is_TC5(void *tc)) {
		switch (channel_index) {
		case TC45_CCA:
			return (((TC5_t *)tc)->INTFLAGS &
			       TC5_CCAIF_bm);

		case TC45_CCB:
			return (((TC5_t *)tc)->INTFLAGS &
			       TC5_CCBIF_bm);

		default:
			return (0);
		}
	}

	return (0);
}

/**
 * \brief Clears Capture/Compare interrupt
 *
 * \param tc Pointer to TC45 module.
 * \param channel_index CC Channel
 */
static inline void tc45_clear_cc_interrupt(volatile void *tc,
		enum tc45_cc_channel_t channel_index)
{
	if (tc45_is_TC4(void *tc)) {
		switch (channel_index) {
		case TC45_CCA:
			((TC4_t *)tc)->INTFLAGS = TC4_CCAIF_bm;
			break;

		case TC45_CCB:
			((TC4_t *)tc)->INTFLAGS = TC4_CCBIF_bm;
			break;

		case TC45_CCC:
			((TC4_t *)tc)->INTFLAGS = TC4_CCCIF_bm;
			break;

		case TC45_CCD:
			((TC4_t *)tc)->INTFLAGS = TC4_CCDIF_bm;
			break;
		}
	} else if (tc45_is_TC5(void *tc)) {
		switch (channel_index) {
		case TC45_CCA:
			((TC5_t *)tc)->INTFLAGS = TC5_CCAIF_bm;
			break;

		case TC45_CCB:
			((TC5_t *)tc)->INTFLAGS = TC5_CCBIF_bm;
			break;

		default:
			return;
		}
	}
}

/**
 * \brief Configures TC45 in the specified Waveform generator mode
 *
 * \param tc Pointer to TC45 module.
 * \param wgm : waveform generator mode
 */
static inline void tc45_set_wgm(volatile void *tc, enum tc45_wg_mode_t wgm)
{
	if (tc45_is_TC4(void *tc)) {
		((TC4_t *)tc)->CTRLB
			= (((TC4_t *)tc)->CTRLB & ~TC4_WGMODE_gm) | wgm;
	} else if (tc45_is_TC5(void *tc)) {
		((TC5_t *)tc)->CTRLB
			= (((TC5_t *)tc)->CTRLB & ~TC5_WGMODE_gm) | wgm;
	}
}

/*******************************************************
*      FAULT module                                   *
*******************************************************/

/**
 * \brief FAULT module : configures the RAMP2 mode of Fault
 *
 * \param FAULT Pointer to FAULT module
 */

static inline void tc45_fault_set_ramp2_mode(FAULT_t *FAULT)
{
	((FAULT_t *)FAULT)->CTRLA |= FAULT_RAMP_RAMP2_gc;
}

/**
 * \brief FAULT module : configures the QUAL mode of Fault A
 *
 * \param FAULT Pointer to FAULT module
 */

static inline void tc45_fault_set_qualA_mode(FAULT_t *FAULT)
{
	((FAULT_t *)FAULT)->CTRLC |= FAULT_QUALA_bm;
}

/**
 * \brief FAULT module : configures the QUAL mode of Fault B
 *
 * \param FAULT Pointer to FAULT module
 */

static inline void tc45_fault_set_qualB_mode(FAULT_t *FAULT)
{
	((FAULT_t *)FAULT)->CTRLE |= FAULT_QUALB_bm;
}

/**
 * \brief FAULT module : configures the BLANK mode of Fault A
 *
 * \param FAULT Pointer to FAULT module
 */

static inline void tc45_fault_set_blankA_mode(FAULT_t *FAULT)
{
	((FAULT_t *)FAULT)->CTRLC |= FAULT_BLANKA_bm;
}

/**
 * \brief FAULT module: configures the BLANK mode of Fault B
 *
 * \param FAULT Pointer to FAULT module
 */

static inline void tc45_fault_set_blankB_mode(FAULT_t *FAULT)
{
	((FAULT_t *)FAULT)->CTRLC |= FAULT_BLANKB_bm;
}

/**
 * \brief WEX extension : configures the blanking time
 *
 * \param WEX Pointer to WEX module
 * \param value :  blanking value
 */
static inline void tc45_fault_set_blank_time(WEX_t *WEX, int16_t value)
{
	((WEX_t *)WEX)->DTLS = value; /* blanking time*/
}

/**
 * \brief FAULT module : configures the Fault A input
 *
 * \param FAULT Pointer to FAULT module
 * \ param Fault source mode : Event channel N,N+1...
 */

static inline void tc45_fault_configure_sourceA_input(FAULT_t *FAULT,
		enum fault_sourceA_mode_t source)
{
	((FAULT_t *)FAULT)->CTRLB
		= (((FAULT_t *)FAULT)->CTRLB & ~FAULT_SRCA_gm) | source;
}

/**
 * \brief FAULT module : configures the Fault B input
 *
 * \param FAULT Pointer to FAULT module
 * \ param Fault source mode : Event channel N,N+1...
 */

static inline void tc45_fault_configure_sourceB_input(FAULT_t *FAULT,
		enum fault_sourceB_mode_t source)
{
	((FAULT_t *)FAULT)->CTRLD
		= (((FAULT_t *)FAULT)->CTRLD & ~FAULT_SRCB_gm) | source;
}

/**
 * \brief FAULT module : configures the Fault E input
 *
 * \param FAULT Pointer to FAULT module
 * \ param Fault source mode : Event channel N,N+1...
 */

static inline void tc45_fault_configure_sourceE_input(FAULT_t *FAULT,
		enum fault_sourceE_mode_t source)
{
	((FAULT_t *)FAULT)->CTRLA
		= (((FAULT_t *)FAULT)->CTRLA & ~FAULT_SRCE_gm) | source;
}

/**
 * \brief FAULT module : enables the Filter on Fault E
 *
 * \param FAULT Pointer to FAULT module
 */

static inline void tc45_fault_set_faultE_filter(FAULT_t *FAULT)
{
	((FAULT_t *)FAULT)->CTRLA |= FAULT_FILTERE_bm;
}

/**
 * \brief FAULT module : enables the Filter on Fault A
 *
 * \param FAULT Pointer to FAULT module
 */

static inline void tc45_fault_set_faultA_filter(FAULT_t *FAULT)
{
	((FAULT_t *)FAULT)->CTRLC |= FAULT_FILTERA_bm;
}

/**
 * \brief FAULT module : enables the Filter on Fault B
 *
 * \param FAULT Pointer to FAULT module
 */

static inline void tc45_fault_set_faultB_filter(FAULT_t *FAULT)
{
	((FAULT_t *)FAULT)->CTRLE |= FAULT_FILTERB_bm;
}

/**
 * \brief FAULT module : Set the Restart mode of Fault A
 *
 * \param FAULT Pointer to FAULT module
 */

static inline void tc45_fault_set_restartA_mode(FAULT_t *FAULT)
{
	((FAULT_t *)FAULT)->CTRLB |= FAULT_RESTARTA_bm;
}

/**
 * \brief FAULT module : Set the Restart mode of Fault B
 *
 * \param FAULT Pointer to FAULT module
 */

static inline void tc45_fault_set_restartB_mode(FAULT_t *FAULT)
{
	((FAULT_t *)FAULT)->CTRLD |= FAULT_RESTARTB_bm;
}

/**
 * \brief FAULT module : Set the Keep mode of Fault A
 *
 * \param FAULT Pointer to FAULT module
 */

static inline void tc45_fault_set_keepA_mode(FAULT_t *FAULT)
{
	((FAULT_t *)FAULT)->CTRLB |= FAULT_KEEPA_bm;
}

/**
 * \brief FAULT module : Set the Keep mode of Fault B
 *
 * \param FAULT Pointer to FAULT module
 */

static inline void tc45_fault_set_keepB_mode(FAULT_t *FAULT)
{
	((FAULT_t *)FAULT)->CTRLD |= FAULT_KEEPB_bm;
}

/**
 * \brief FAULT module : Set the Soft mode of Fault A
 *
 * \param FAULT Pointer to FAULT module
 */

static inline void tc45_fault_set_softA_mode(FAULT_t *FAULT)
{
	((FAULT_t *)FAULT)->CTRLB |= FAULT_SOFTA_bm;
}

/**
 * \brief FAULT module : Set the Soft mode of Fault B
 *
 * \param FAULT Pointer to FAULT module
 */

static inline void tc45_fault_set_softB_mode(FAULT_t *FAULT)
{
	((FAULT_t *)FAULT)->CTRLD |= FAULT_SOFTB_bm;
}

/**
 * \brief FAULT module : Set the Capt mode of Fault A
 *
 * \param FAULT Pointer to FAULT module
 */

static inline void tc45_fault_set_captA_mode(FAULT_t *FAULT)
{
	((FAULT_t *)FAULT)->CTRLC |= FAULT_CAPTA_bm;
}

/**
 * \brief FAULT module : Set the Capt mode of Fault B
 *
 * \param FAULT Pointer to FAULT module
 */

static inline void tc45_fault_set_captB_mode(FAULT_t *FAULT)
{
	((FAULT_t *)FAULT)->CTRLE |= FAULT_CAPTB_bm;
}

/**
 * \brief FAULT module : configures the Halt mode of Fault A
 *
 * \param FAULT Pointer to FAULT module
 * \param fault Halt mode
 */

static inline void tc45_fault_set_haltA_mode(FAULT_t *FAULT,
		enum fault_haltA_mode_t fault)
{
	((FAULT_t *)FAULT)->CTRLB
		= (((FAULT_t *)FAULT)->CTRLB & ~FAULT_HALTA_gm) | fault;
}

/**
 * \brief FAULT module : configures the Halt mode of Fault B
 *
 * \param FAULT Pointer to FAULT module
 * \param fault Halt mode
 */

static inline void tc45_fault_set_haltB_mode(FAULT_t *FAULT,
		enum fault_haltB_mode_t fault)
{
	((FAULT_t *)FAULT)->CTRLD
		= (((FAULT_t *)FAULT)->CTRLD & ~FAULT_HALTB_gm) | fault;
}

/*! \brief Configures the Timer/Counter event channel source
 *
 *  \param tc               Timer/Counter module instance.
 *  \param  eventSource     Event channel base
 */
static inline void tc45_config_fault_input_evsel(volatile void *tc,
		TC45_EVSEL_t eventSource )
{
	if (tc45_is_TC4(void *tc)) {
		((TC4_t *)tc)->CTRLD
			= (((TC4_t *)tc)->CTRLD & ~(TC4_EVSEL_gm)) |
				eventSource;
	} else if (tc45_is_TC5(void *tc)) {
		((TC5_t *)tc)->CTRLD
			= (((TC5_t *)tc)->CTRLD & ~(TC5_EVSEL_gm)) |
				eventSource;
	}
}

/*! \brief Configures the Timer/Counter Event operation.
 *  \param tc               Timer/Counter module instance.
 *  \param eventAction      Capture, qdec....
 */
static inline void tc45_config_fault_input_evact(volatile void *tc,
		TC45_EVSEL_t eventAction )
{
	if (tc45_is_TC4(void *tc)) {
		((TC4_t *)tc)->CTRLD
			= (((TC4_t *)tc)->CTRLD & ~(TC4_EVACT_gm)) |
				eventAction;
	} else if (tc45_is_TC5(void *tc)) {
		((TC5_t *)tc)->CTRLD
			= (((TC5_t *)tc)->CTRLD & ~(TC5_EVACT_gm)) |
				eventAction;
	}
}

/*! \brief Configures the Timer/Counter FMODE1.
 *
 *  \param tc               Timer/Counter module instance.
 *  \param eventSource      Event channel base
 */
static inline void tc45_config_fault_input_fmode1(volatile void *tc,
		TC45_EVSEL_t eventSource )
{
	if (tc45_is_TC4(void *tc)) {
		((TC4_t *)tc)->CTRLD
			= (((TC4_t *)tc)->CTRLD &
				~(TC4_EVSEL_gm | TC4_EVACT_gm)) |
				eventSource |
				TC45_EVACT_FMODE1_gc;
	} else if (tc45_is_TC5(void *tc)) {
		((TC5_t *)tc)->CTRLD
			= (((TC5_t *)tc)->CTRLD &
				~(TC5_EVSEL_gm | TC5_EVACT_gm)) |
				eventSource |
				TC45_EVACT_FMODE1_gc;
	}
}

/*! \brief Configures the Timer/Counter FMODE2.
 *
 *  \param tc               Timer/Counter module instance.
 *  \param eventSource      Event channel base
 */
static inline void tc45_config_fault_input_fmode2(volatile void *tc,
		TC45_EVSEL_t eventSource )
{
	if (tc45_is_TC4(void *tc)) {
		((TC4_t *)tc)->CTRLD
			= (((TC4_t *)tc)->CTRLD &
				~(TC4_EVSEL_gm | TC4_EVACT_gm)) |
				eventSource | TC45_EVACT_FMODE2_gc;
	} else if (tc45_is_TC5(void *tc)) {
		((TC5_t *)tc)->CTRLD
			= (((TC5_t *)tc)->CTRLD &
				~(TC5_EVSEL_gm | TC5_EVACT_gm)) |
				eventSource | TC45_EVACT_FMODE2_gc;
	}
}

/**
 * \brief FAULT module : returns if FaultA is present
 *
 * \param FAULT Pointer to FAULT module
 *
 * \return  Fault A is present or not
 */

static inline bool tc45_fault_faultAin_is_detected(FAULT_t *FAULT)
{
	return ((FAULT_t *)FAULT)->STATUS & FAULT_FAULTAIN_bm;
}

/**
 * \brief FAULT module : returns if FaultB is present
 *
 * \param FAULT Pointer to FAULT module
 *
 * \return  Fault B is present or not
 */

static inline bool tc45_fault_faultBin_is_detected(FAULT_t *FAULT)
{
	return ((FAULT_t *)FAULT)->STATUS & FAULT_FAULTBIN_bm;
}

/**
 * \brief FAULT module : returns if FaultE is present
 *
 * \param FAULT Pointer to FAULT module
 *
 * \return  Fault E is present or not
 */

static inline bool tc45_fault_faultEin_is_detected(FAULT_t *FAULT)
{
	return ((FAULT_t *)FAULT)->STATUS & FAULT_FAULTEIN_bm;
}

/**
 * \brief FAULT module : returns if FaultA is detected in STATEA
 *
 * \param FAULT Pointer to FAULT module
 *
 * \return  Fault A state
 */

static inline bool tc45_fault_stateA_is_detected(FAULT_t *FAULT)
{
	return ((FAULT_t *)FAULT)->STATUS & FAULT_STATEA_bm;
}

/**
 * \brief FAULT module : returns if FaultB is detected in STATEB
 *
 * \param FAULT Pointer to FAULT module
 *
 * \return  Fault B state
 */

static inline bool tc45_fault_stateB_is_detected(FAULT_t *FAULT)
{
	return ((FAULT_t *)FAULT)->STATUS & FAULT_STATEB_bm;
}

/**
 * \brief FAULT module : returns if fault is detected in STATEE
 *
 * \param FAULT Pointer to FAULT module
 *
 * \return  Fault E state
 */

static inline bool tc45_fault_stateE_is_detected(FAULT_t *FAULT)
{
	return ((FAULT_t *)FAULT)->STATUS & FAULT_STATEE_bm;
}

/**
 * \brief FAULT module : clears the HALTA Fault status
 *
 * \param FAULT Pointer to FAULT module
 */

static inline void tc45_fault_clear_haltA(FAULT_t *FAULT)
{
	((FAULT_t *)FAULT)->CTRLGCLR = FAULT_HALTACLR_bm;
}

/**
 * \brief FAULT module : clears the HALTB Fault status
 *
 * \param FAULT Pointer to FAULT module
 */
static inline void tc45_fault_clear_haltB(FAULT_t *FAULT)
{
	((FAULT_t *)FAULT)->CTRLGCLR = FAULT_HALTBCLR_bm;
}

/**
 * \brief FAULT module : clears the HALTE Fault status
 *
 * \param FAULT Pointer to FAULT module
 */
static inline void tc45_fault_clear_stateE(FAULT_t *FAULT)
{
	((FAULT_t *)FAULT)->CTRLGCLR = FAULT_STATEECLR_bm;
}

/**
 * \brief FAULT module : clears the FAULTA Fault detection
 *
 * \param FAULT Pointer to FAULT module
 */

static inline void tc45_fault_clear_faultA(FAULT_t *FAULT)
{
	((FAULT_t *)FAULT)->CTRLGCLR |= FAULT_FAULTA_bm;
}

/**
 * \brief FAULT module : clears the FAULTB Fault detection
 *
 * \param FAULT Pointer to FAULT module
 */

static inline void tc45_fault_clear_faultB(FAULT_t *FAULT)
{
	((FAULT_t *)FAULT)->CTRLGCLR |= FAULT_FAULTB_bm;
}

/**
 * \brief FAULT module : clears the FAULTE Fault detection
 *
 * \param FAULT Pointer to FAULT module
 */

static inline void tc45_fault_clear_faultE(FAULT_t *FAULT)
{
	((FAULT_t *)FAULT)->CTRLGCLR |= FAULT_FAULTE_bm;
}

/**
 * \brief FAULT module : set the Software Fault input A
 *
 * \param FAULT Pointer to FAULT module
 */

static inline void tc45_fault_set_softwareA(FAULT_t *FAULT)
{
	((FAULT_t *)FAULT)->CTRLGSET |= FAULT_FAULTASW_bm;
}

/**
 * \brief FAULT module : set the Software Fault input B
 *
 * \param FAULT Pointer to FAULT module
 */

static inline void tc45_fault_set_softwareB(FAULT_t *FAULT)
{
	((FAULT_t *)FAULT)->CTRLGSET |= FAULT_FAULTBSW_bm;
}

/**
 * \brief FAULT module : set the Software Fault input E
 *
 * \param FAULT Pointer to FAULT module
 */

static inline void tc45_fault_set_softwareE(FAULT_t *FAULT)
{
	((FAULT_t *)FAULT)->CTRLGSET |= FAULT_FAULTESW_bm;
}

/**
 * \brief FAULT module : set the Port control from Fault module
 *
 * \param FAULT Pointer to FAULT module
 */
static inline void tc45_fault_set_portcontrol(FAULT_t *FAULT)
{
	((FAULT_t *)FAULT)->CTRLA |= FAULT_PORTCTRL_bm;
}

/**
 * \brief FAULT module : reset the Fault Port control
 *
 * \param FAULT Pointer to FAULT module
 */

static inline void tc45_fault_reset_portcontrol(FAULT_t *FAULT)
{
	((FAULT_t *)FAULT)->CTRLA &= ~FAULT_PORTCTRL_bm;
}

/**
 * \brief FAULT module : forces the Cycle B (IDX set) in RAMP2 mode
 *
 * \param FAULT Pointer to FAULT module
 */

static inline void tc45_fault_set_idx(FAULT_t *FAULT)
{
	((FAULT_t *)FAULT)->CTRLGSET
		= (((FAULT_t *)FAULT)->CTRLGSET &
			~FAULT_IDXCMD_gm) | tmpfix_FAULT_IDXCMD_SET_gc;
}

/**
 * \brief FAULT module : forces the Cycle A (IDX clear) in RAMP2 mode
 *
 * \param FAULT Pointer to FAULT module
 */

static inline void tc45_fault_clear_idx(FAULT_t *FAULT)
{
	((FAULT_t *)FAULT)->CTRLGSET
		= (((FAULT_t *)FAULT)->CTRLGSET &
			~FAULT_IDXCMD_gm) | tmpfix_FAULT_IDXCMD_CLEAR_gc;
}

/**
 * \brief FAULT module : Holds the Cycle N (IDX Hold) in RAMP2 mode
 *
 * \param FAULT Pointer to FAULT module
 */

static inline void tc45_fault_hold_idx(FAULT_t *FAULT)
{
	((FAULT_t *)FAULT)->CTRLGSET
		= (((FAULT_t *)FAULT)->CTRLGSET &
			~FAULT_IDXCMD_gm) | tmpfix_FAULT_IDXCMD_HOLD_gc;
}

/**
 * \brief FAULT module : Default IDX in RAMP2 mode
 *
 * \param FAULT Pointer to FAULT module
 */

static inline void tc45_fault_default_idx(FAULT_t *FAULT)
{
	((FAULT_t *)FAULT)->CTRLGSET
		= (((FAULT_t *)FAULT)->CTRLGSET & ~FAULT_IDXCMD_gm);
}

/**
 * \brief FAULT module : reads IDX in RAMP2 mode
 *
 * \param FAULT Pointer to FAULT module
 *
 * \return  IDX configuration
 */

static inline bool tc45_fault_read_idx(FAULT_t *FAULT)
{
	return ((FAULT_t *)FAULT)->STATUS & FAULT_IDX_bm;
}

/*******************************************************
*      AWEX extension                                 *
*******************************************************/

/**
 * \brief WEX extension : Configures WEX in the specified output matrix mode
 *
 * \param WEX   Pointer to WEX module
 * \param otmx  Output Matrix mode
 *
 */
static inline void tc45_wex_set_otmx(WEX_t *WEX, enum wex_otmx_mode_t otmx)
{
	((WEX_t *)WEX)->CTRL = (((WEX_t *)WEX)->CTRL & ~WEX_OTMX_gm) | otmx;
}

/**
 * \brief WEX extension : Reads the output matrix mode
 *
 * \param WEX Pointer to WEX module
 *
 */
static inline uint16_t tc45_wex_read_otmx(WEX_t *WEX)
{
	return (((WEX_t *)WEX)->CTRL & WEX_OTMX_gm);
}

/**
 * \brief WEX extension : enables Deadtime insertion on ccA
 *
 * \param WEX Pointer to WEX module
 */
static inline void tc45_wex_enable_cca_deadtime(WEX_t *WEX)
{
	((WEX_t *)WEX)->CTRL |= WEX_DTI0EN_bm;
}

/**
 * \brief WEX extension : disables Deadtime insertion on ccA
 *
 * \param WEX Pointer to WEX module
 */
static inline void tc45_wex_disable_cca_deadtime(WEX_t *WEX)
{
	((WEX_t *)WEX)->CTRL &= ~WEX_DTI0EN_bm;
}

/**
 * \brief WEX extension : enables Deadtime insertion on ccB
 *
 * \param WEX Pointer to WEX module
 */
static inline void tc45_wex_enable_ccb_deadtime(WEX_t *WEX)
{
	((WEX_t *)WEX)->CTRL |= WEX_DTI1EN_bm;
}

/**
 * \brief WEX extension : disables Deadtime insertion on ccB
 *
 * \param WEX Pointer to WEX module
 */
static inline void tc45_wex_disable_ccb_deadtime(WEX_t *WEX)
{
	((WEX_t *)WEX)->CTRL &= ~WEX_DTI1EN_bm;
}

/**
 * \brief WEX extension : enables Deadtime insertion on ccC
 *
 * \param WEX Pointer to WEX module
 */
static inline void tc45_wex_enable_ccc_deadtime(WEX_t *WEX)
{
	((WEX_t *)WEX)->CTRL |= WEX_DTI2EN_bm;
}

/**
 * \brief WEX extension : disables Deadtime insertion on ccD
 *
 * \param WEX Pointer to WEX module
 */
static inline void tc45_wex_disable_ccc_deadtime(WEX_t *WEX)
{
	((WEX_t *)WEX)->CTRL &= ~WEX_DTI2EN_bm;
}

/**
 * \brief WEX extension : enables Deadtime insertion on ccD
 *
 * \param WEX Pointer to WEX module
 */
static inline void tc45_wex_enable_ccd_deadtime(WEX_t *WEX)
{
	((WEX_t *)WEX)->CTRL |= WEX_DTI3EN_bm;
}

/**
 * \brief WEX extension : disables Deadtime insertion on ccD
 *
 * \param WEX Pointer to WEX module
 */
static inline void tc45_wex_disable_ccd_deadtime(WEX_t *WEX)
{
	((WEX_t *)WEX)->CTRL &= ~WEX_DTI3EN_bm;
}

/**
 * \brief WEX extension : configures high side deadtime
 *
 * \param WEX Pointer to WEX module
 * \param value : deadtime value
 */
static inline void tc45_wex_set_dti_high(WEX_t *WEX, int16_t value)
{
	((WEX_t *)WEX)->DTHS = value;
}

/**
 * \brief WEX extension : configures low side deadtime
 *
 * \param WEX Pointer to WEX module
 * \param value : deadtime value
 */
static inline void tc45_wex_set_dti_low(WEX_t *WEX, int16_t value)
{
	((WEX_t *)WEX)->DTLS = value;
}

/**
 * \brief WEX extension : configures symetrical deadtime
 *
 * \param WEX Pointer to WEX module
 * \param value : deadtime value
 */
static inline void tc45_wex_set_dti_both(WEX_t *WEX, int16_t value)
{
	((WEX_t *)WEX)->DTBOTH = value;
}

/**
 * \brief WEX extension : Enables Swap on OTMX 0  and 1
 *
 * \param WEX Pointer to WEX module
 *
 */
static inline void tc45_wex_enable_swap0(WEX_t *WEX)
{
	((WEX_t *)WEX)->SWAP |= WEX_SWAP0_bm;
}

/**
 * \brief WEX extension : Disables Swap on OTMX 0  and 1
 *
 * \param WEX Pointer to WEX module
 *
 */
static inline void tc45_wex_disable_swap0(WEX_t *WEX)
{
	((WEX_t *)WEX)->SWAP &= ~WEX_SWAP0_bm;
}

/**
 * \brief WEX extension : Enables Swap Buffer on OTMX 0  and 1
 *
 * \param WEX Pointer to WEX module
 *
 */
static inline void tc45_wex_enable_swap0_buffer(WEX_t *WEX)
{
	((WEX_t *)WEX)->SWAPBUF |= WEX_SWAP0BUF_bm;
}

/**
 * \brief WEX extension : Disables Swap on OTMX 0  and 1
 *
 * \param WEX Pointer to WEX module
 *
 */
static inline void tc45_wex_disable_swap0_buffer(WEX_t *WEX)
{
	((WEX_t *)WEX)->SWAPBUF &= ~WEX_SWAP0BUF_bm;
}

/**
 * \brief WEX extension : Enables Swap on OTMX 2  and 3
 *
 * \param WEX Pointer to WEX module
 *
 */
static inline void tc45_wex_enable_swap1(WEX_t *WEX)
{
	((WEX_t *)WEX)->SWAP |= WEX_SWAP1_bm;
}

/**
 * \brief WEX extension : Disables Swap on OTMX 2  and 3
 *
 * \param WEX Pointer to WEX module
 *
 */
static inline void tc45_wex_disable_swap1(WEX_t *WEX)
{
	((WEX_t *)WEX)->SWAP &= ~WEX_SWAP1_bm;
}

/**
 * \brief WEX extension : Enables Swap buffer on OTMX 2  and 3
 *
 * \param WEX Pointer to WEX module
 *
 */
static inline void tc45_wex_enable_swap1_buffer(WEX_t *WEX)
{
	((WEX_t *)WEX)->SWAPBUF |= WEX_SWAP1BUF_bm;
}

/**
 * \brief WEX extension : Disables Swap on OTMX 2  and 3
 *
 * \param WEX Pointer to WEX module
 *
 */
static inline void tc45_wex_disable_swap1_buffer(WEX_t *WEX)
{
	((WEX_t *)WEX)->SWAPBUF &= ~WEX_SWAP1BUF_bm;
}

/**
 * \brief WEX extension : Enables Swap on OTMX 4  and 5
 *
 * \param WEX Pointer to WEX module
 *
 */
static inline void tc45_wex_enable_swap2(WEX_t *WEX)
{
	((WEX_t *)WEX)->SWAP |= WEX_SWAP2_bm;
}

/**
 * \brief WEX extension : Disables Swap on OTMX 4  and 5
 *
 * \param WEX Pointer to WEX module
 *
 */
static inline void tc45_wex_disable_swap2(WEX_t *WEX)
{
	((WEX_t *)WEX)->SWAP &= ~WEX_SWAP2_bm;
}

/**
 * \brief WEX extension : Ennables Swap buffer on OTMX 4  and 5
 *
 * \param WEX Pointer to WEX module
 *
 */
static inline void tc45_wex_enable_swap2_buffer(WEX_t *WEX)
{
	((WEX_t *)WEX)->SWAPBUF |= WEX_SWAP2BUF_bm;
}

/**
 * \brief WEX extension : Disables Swap on OTMX 4  and 5
 *
 * \param WEX Pointer to WEX module
 *
 */
static inline void tc45_wex_disable_swap2_buffer(WEX_t *WEX)
{
	((WEX_t *)WEX)->SWAPBUF &= ~WEX_SWAP2BUF_bm;
}

/**
 * \brief WEX extension : Enables Swap on OTMX 6  and 7
 *
 * \param WEX Pointer to WEX module
 *
 */
static inline void tc45_wex_enable_swap3(WEX_t *WEX)
{
	((WEX_t *)WEX)->SWAP |= WEX_SWAP3_bm;
}

/**
 * \brief WEX extension : Disables Swap on OTMX 6  and 7
 *
 * \param WEX Pointer to WEX module
 *
 */
static inline void tc45_wex_disable_swap3(WEX_t *WEX)
{
	((WEX_t *)WEX)->SWAP &= ~WEX_SWAP3_bm;
}

/**
 * \brief WEX extension : Enables Swap buffer on OTMX 6  and 7
 *
 * \param WEX Pointer to WEX module
 *
 */
static inline void tc45_wex_enable_swap3_buffer(WEX_t *WEX)
{
	((WEX_t *)WEX)->SWAPBUF |= WEX_SWAP3BUF_bm;
}

/**
 * \brief WEX extension : Disables Swap on OTMX 6  and 7
 *
 * \param WEX Pointer to WEX module
 *
 */
static inline void tc45_wex_disable_swap3_buffer(WEX_t *WEX)
{
	((WEX_t *)WEX)->SWAPBUF &= ~WEX_SWAP3BUF_bm;
}

/**
 * \brief WEX extension : configures Pattern Generator output enable
 *
 * \param WEX    Pointer to WEX module
 * \param value  Pattern Genarator enable
 */

static inline void tc45_wex_write_pgo(WEX_t *WEX, int16_t value)
{
	((WEX_t *)WEX)->PGO = value;
}

/**
 * \brief WEX extension : configures Pattern Generator Buffer output enable
 *
 * \param WEX    Pointer to WEX module
 * \param value  Pattern Genarator enable
 */

static inline void tc45_wex_write_pgo_buffer(WEX_t *WEX, int16_t value)
{
	((WEX_t *)WEX)->PGOBUF = value;
}

/**
 * \brief Reads the Pattern generator Output buffer
 *
 * \param WEX Pointer to WEX module
 * \return Patter Generator Buffer PGOBUF
 */
static inline uint16_t tc45_wex_read_pgo_buffer(volatile void *WEX)
{
	return (((WEX_t *)WEX)->PGOBUF);
}

/**
 * \brief WEX extension : configures Pattern Generator Value
 *
 * \param WEX    Pointer to WEX module
 * \param value  PGO value
 */

static inline void tc45_wex_write_pgv(WEX_t *WEX, uint8_t value)
{
	((WEX_t *)WEX)->PGV = value;
}

/**
 * \brief WEX extension : configures Pattern Generator Value Buffer
 *
 * \param WEX    Pointer to WEX module
 * \param value  PGO value
 */

static inline void tc45_wex_write_pgv_buffer(WEX_t *WEX, uint8_t value)
{
	((WEX_t *)WEX)->PGVBUF = value;
}

/**
 * \brief WEX extension : configures Output override
 *
 * \param WEX    Pointer to WEX module
 * \param value  Override value
 */

static inline void tc45_wex_set_output_override(WEX_t *WEX, uint8_t value)
{
	((WEX_t *)WEX)->OUTOVDIS = value;
}
/** @} */

/**
 * \ingroup tc45_group
 * \defgroup tc45_hires_group Hi-Res extension driver
 * This group provides low level drivers to configure Hi-Res extension
 * @{
 */

/**
 * \brief Hi-Res Extension : configures the Hi-Res
 *
 * \param hires         Pointer to HIRES module
 * \param hi_res_mode   HIRES configuration
 */
static inline void tc45_hires_set_mode(HIRES_t *hires, HIRES_HREN_t hi_res_mode)
{
	((HIRES_t *)hires)->CTRLA = hi_res_mode;
}
/** @} */

#ifdef __cplusplus
}
#endif

/**
 * \page xmega_tc45_quickstart Quick Start Guide for the XMEGA TC45 Driver
 *
 * This is the quick start guide for the \ref tc45_group , with step-by-step 
 * instructions on how to configure and use the driver for a specific use case. 
 * The code examples can be copied into e.g the main application loop or any
 * other function that will need to control the timer/counters.
 *
 *
 * \section xmega_tc45_qs_use_cases Use cases
 * - \ref xmega_tc45_qs_ovf
 * - \ref xmega_tc45_qs_cc
 * - \ref xmega_tc45_qs_pwm
 *
 *
 * \section xmega_tc45_qs_ovf Timer/counter overflow (interrupt based)
 *
 * This use case will prepare a timer to trigger an interrupt when the timer
 * overflows. The interrupt is handled by a cutomisable callback function.
 *
 * We will setup the timer in this mode:
 * - Normal WGM mode (incrementing timer)
 * - Use the system clock as clock source
 * - No prescaling (clock divider set to 1)
 * - Overflow interrupt after 1000 counts. This will be done by setting the top
 *   value to 1000.
 *
 *
 * \section xmega_tc45_qs_ovf_setup Setup steps
 *
 * \subsection xmega_tc45_qs_ovf_usage_prereq Prequisites
 *
 * For the setup code of this use case to work, the following must
 * be added to the project:
 * - \ref interrupt_group "Global Interrupt Management"
 * - \ref clk_group "Clock Management"
 *
 * \subsection xmega_tc45_qs_ovf_setup_code Example code
 *
 * Add a callback function that will be executed when the overflow interrupt
 * trigger.
 * \code
	static void my_callback(void)
	{
	    // User code to execute when the overflow occurs here
	}
\endcode
 * Add to, e.g., the main loop in the application C-file:
 * \code
	sysclk_init();
	tc45_enable(&TCC4);
	tc45_set_overflow_interrupt_callback(&TCC4, my_callback);
	tc45_set_wgm(&TCC4, TC45_WG_NORMAL);
	tc45_write_period(&TCC4, 1000);
	tc45_set_overflow_interrupt_level(&TCC4, TC45_INT_LVL_LO);
	irq_initialize_vectors();
	cpu_irq_enable();
	tc45_write_clock_source(&TCC4, TC45_CLKSEL_DIV1_gc);
\endcode
 *
 * \subsection xmega_tc45_qs_ovf_setup_code_workflow Workflow
 *
 * -# Enable the interrupt controller:
 *  - \code irq_initialize_vectors(); \endcode
 * -# Enable the clock system:
 *  - \code sysclk_init(); \endcode
 * -#  Enable timer/counter TCC4
 *  - \code tc45_enable(&TCC4); \endcode
 *    \note This will enable the clock system for the module
 * -# Set the callback function for overflow interrupt
 *  - \code tc45_set_overflow_interrupt_callback(&TCC4, my_callback); \endcode
 *    \warning This function requires that the my_callback function is defined
 * -# Set the desired waveform mode
 *  - \code tc45_set_wgm(&TCC4, TC45_WG_NORMAL); \endcode
 *    \note In this case, we use normal mode where the timer increments it
            count value until the TOP value is reached. The timer then reset
            its count value to 0.
 * -# Set the period 
 *  - \code tc45_write_period(&TCC4, 1000); \endcode
 *    \note This will specify the TOP value of the counter. The timer will
 *          overflow and reset when this value is reached.
 * -# Set the overflow interrupt level 
 *   - \code tc45_set_overflow_interrupt_level(&TCC4, TC45_INT_LVL_LO); \endcode
 * -# Enable interrupts:
 *  - \code cpu_irq_enable(); \endcode
 * -# Set the clock source
 *  - \code tc45_write_clock_source(&TCC4, TC45_CLKSEL_DIV1_gc); \endcode
 *    \warning When the clock source is set, the timer will start counting
 *
 * \section xmega_tc45_qs_ovf_usage Usage steps
 *
 * - None. The timer will run in the background, and the code written in the 
 *   call back function will execute each time the timer overflows. 
 *
 *
 * \section xmega_tc45_qs_cc Timer/counter compare match (interrupt based)
 *
 * This use case will prepare a timer to trigger two independent interrupts
 * when it reaches two different compare values. The period of the timer
 * is customizable and the two compare matches will be handled by two separate
 * interrupts implemented in call back functions.
 *
 * We will setup the timer in this mode:
 * - Normal WGM mode - incrementing timer
 * - Use the system clock as clock source
 * - No prescaling (divider set to 1)
 * - Period of timer 10000 counts
 * - Compare match A interrupt trigger after 100 counts
 * - Compare match B interrupt trigger after 1000 counts
 * - If compare A and compare B match occurs simultaneously, compare B
 *   should have higher priority
 *
 *
 * \section xmega_tc45_qs_cc_setup Setup steps
 *
 * \subsection xmega_tc45_qs_cc_usage_prereq Prequisites
 * For the setup code of this use case to work, the following must
 * be added to the project:
 * - \ref interrupt_group "Global Interrupt Management"
 * - \ref clk_group "Clock Management"
 *
 * \subsection xmega_tc45_qs_cc_setup_code Example code
 *
 * Add two callback functions that will be executed when compare match A and 
 * compare match B occurs
 * \code
	static void my_cca_callback(void)
	{
	   // User code here to execute when a channel A compare match occurs
	}
	static void my_ccb_callback(void)
	{
	   // User code here to execute when a channel B compare match occurs
	}
\endcode
 * Add to, e.g., the main loop in the application C-file:
 * \code
	sysclk_init();
	irq_initialize_vectors();
	cpu_irq_enable();
	tc45_enable(&TCC4);
	tc45_set_cca_interrupt_callback(&TCC4, my_cca_callback);
	tc45_set_ccb_interrupt_callback(&TCC4, my_ccb_callback);
	tc45_set_wgm(&TCC4, TC45_WG_NORMAL);
	tc45_write_period(&TCC4, 10000);
	tc45_write_cc(&TCC4, TC45_CCA, 100);
	tc45_write_cc(&TCC4, TC45_CCB, 1000);
	tc45_enable_cc_channels(&TCC4,(TC45_CCAEN | TC45_CCBEN));
	tc45_set_cca_interrupt_level(&TCC4, TC45_INT_LVL_LO);
	tc45_set_ccb_interrupt_level(&TCC4, TC45_INT_LVL_MED);
	tc45_write_clock_source(&TCC4, TC45_CLKSEL_DIV1_gc);
\endcode
 *
 * \subsection xmega_tc45_qs_cc_setup_code_workflow Workflow
 *
 * -# Enable the interrupt controller:
 *  - \code irq_initialize_vectors(); \endcode
 * -# Enable the clock system:
 *  - \code sysclk_init(); \endcode
 * -# Enable interrupts:
 *  - \code cpu_irq_enable(); \endcode
 * -#  Enable timer/counter TCC4
 *  - \code tc45_enable(&TCC4); \endcode
 *    \note This will enable the clock system for the module
 * -# Set call back function for CCA interrupt
 *  - \code tc45_set_cca_interrupt_callback(&TCC4, my_cca_callback); \endcode
 *    \warning This function requires that the call back function is defined
 * -# Set call back function for CCB interrupt
 *  - \code tc45_set_ccb_interrupt_callback(&TCC4, my_ccb_callback); \endcode
 *    \warning This function requires that the call back function is defined
 * -# Set the desired waveform mode
 *  - \code tc45_set_wgm(&TCC4, TC45_WG_NORMAL); \endcode
 *    \note In this case, we use normal mode where the timer increments it
            count value until the TOP value is reached. The timer then reset
            its count value to 0.
 * -# Set the period 
 *  - \code tc45_write_period(&TCC4, 10000); \endcode
 *    \note This will specify the TOP value of the counter. The timer will
 *          overflow and reset when this value is reached.
 * -# Set compare match value on CCA
 *   - \code tc45_write_cc(&TCC4, TC45_CCA, 100); \endcode
 * -# Set compare match value on CCB
 *   - \code tc45_write_cc(&TCC4, TC45_CCB, 1000); \endcode
 * -# Enable compare channel A and compare channel B
 *  -\code tc45_enable_cc_channels(&TCC4, (TC45_CCAEN | TC45_CCBEN)); \endcode
 * -# Set interrupt level on channel A (low priority, see \ref TC45_INT_LEVEL_t)
 *   - \code tc45_set_cca_interrupt_level(&TCC4, TC45_INT_LVL_LO); \endcode
 * -# Set interrupt level on channel B (medium priority \ref TC45_INT_LEVEL_t)
 *   - \code tc45_set_ccb_interrupt_level(&TCC4, TC45_INT_LVL_MED); \endcode
 * -# Set the clock source
 *  - \code tc45_write_clock_source(&TCC4, TC45_CLKSEL_DIV1_gc); \endcode
 *    \warning When the clock source is set, the timer will start counting
 *
 * \section xmega_tc45_qs_cc_usage Usage steps
 *
 * - None. The timer will run in the background, and the code written in the 
 *   call back functions will execute each time a compare match occur.
 *
 *
 * \section xmega_tc45_qs_pwm Timer/counter PWM
 * 
 * This use case will setup a timer in PWM mode. For more details you can 
 * also look at the XMEGA PWM service.
 *
 * We will setup the timer in this mode:
 * - Normal WGM mode - incrementing timer
 * - Use the 2MHz oscillator as clock source (default)
 * - 1Hz PWM frequency (2MHz clock, 1024x prescale, TOP value 1950) 
 * - 10% duty cycle (1:10 ratio between PER and CC register)
 * - Output the PWM signal to a I/O port
 *
 * \section xmega_tc45_qs_pwm_setup Setup steps
 *
 * \subsection xmega_tc45_qs_pwm_usage_prereq Prequisites
 * For the setup code of this use case to work, the following must
 * be added to the project:
 * - \ref clk_group "Clock Management"
 *
 * \subsection xmega_tc45_qs_pwm_setup_code Example code
 *
 * Add to, e.g., the main loop in the application C-file:
 * \code
	board_init();
	sysclk_init();
	tc45_enable(&TCC4);
	tc45_set_wgm(&TCC4, TC45_WG_SS);
	tc45_write_period(&TCC4, 1950);
	tc45_write_cc(&TCC4, TC45_CCA, 195);
	tc45_enable_cc_channels(&TCC4,TC45_CCAEN);
	tc45_write_clock_source(&TCC4, TC45_CLKSEL_DIV1024_gc);
\endcode
 *
 * \subsection xmega_tc45_qs_pwm_setup_code_workflow Workflow
 *
 * -# Ensure that PWM I/O pin is configured as output
 *  - \code board_init(); \endcode
 *    \note The board_init(); function configures the I/O pins. If this function
 *          is not executed, the I/O pin must be configured as output manually
 * -# Enable the clock system:
 *  - \code sysclk_init(); \endcode
 * -#  Enable timer/counter TCC4
 *  - \code tc45_enable(&TCC4); \endcode
 *    \note This will enable the clock system for the module
 * -# Set the desired waveform mode
 *  - \code tc45_set_wgm(&TCC4, TC45_WG_NORMAL); \endcode
 *    \note In this case, we use normal mode where the timer increments it
 *          count value until the TOP value is reached. The timer then reset
  *         its count value to 0.
 * -# Set the period 
 *  - \code tc45_write_period(&TCC4, 1950); \endcode
 *    \note This will specify the TOP value of the counter. The timer will
 *          overflow and reset when this value is reached.
 * -# Set compare match value on CCA
 *   - \code tc45_write_cc(&TCC4, TC45_CCA, 195); \endcode
 *     \note The PWM duty cycle will be the ratio between PER and CCA, which
 *           is set by the tc45_write_period() and tc45_write_cc() functions. Use
 *           tc45_write_cc() to change duty cycle run time (e.g to dim a LED).
 *           When CCA = 0, the duty cycle will be 0%. When CCA = PER (top value)
 *           the duty cycle will be 100%.
 * -# Enable compare channel A 
 *  -\code tc45_enable_cc_channels(&TCC4,TC45_CCAEN); \endcode
 * -# Set the clock source
 *  - \code tc45_write_clock_source(&TCC4, TC45_CLKSEL_DIV1024_gc); \endcode
 *    \warning When the clock source is set, the timer will start counting
 *
 * \section xmega_tc45_qs_pwm_usage Usage steps
 *  - Use tc45_write_cc() to change the duty cycle of the PWM signal
 *  - Use tc45_write_period() to change the PWM frequency
 */


#endif /* TC45_H */
