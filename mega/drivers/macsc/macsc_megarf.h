/**
 * \file
 *
 * \brief AVR MEGARF MAC Symbol Counter Driver Definitions
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
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#ifndef MACSC_MEGARF_H
#define MACSC_MEGARF_H

#include <compiler.h>
#include <parts.h>
#include "status_codes.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup macsc_group MAC Symbol Counter Driver(MACSC)
 *
 * See \ref megarf_macsc_quickstart
 *
 * This is a driver for the AVR MEGARF MAC Symbol Counter Driver(MACSC).
 * It provides functions for enabling, disabling and configuring the module.
 *
 * \section dependencies Dependencies
 * This driver depends on the following modules:
 * - \ref interrupt_group for ISR definition and disabling interrupts during
 * critical code sections.
 * @{
 */

/**
 * \brief Interrupt event callback function type
 *
 * The interrupt handler can be configured to do a function callback,
 * the callback function must match the macsc_callback_t type.
 *
 */
typedef void (*macsc_callback_t)(void);

/* ! MAC symbol counter compare Channel index */
enum macsc_cc_channel {
	/* ! Channel 1 */
	MACSC_CC1 = 1,
	/* ! Channel 2 */
	MACSC_CC2 = 2,
	/* ! Channel 3 */
	MACSC_CC3 = 3,
};
/**
 * \brief MAC SC clock source select
 *
 * uses the SCCKSEL bit in SSCR register to select macsc clk src
 *
 * If the bit is one,the RTC clock from TOSC1 is selected, otherwise the symbol
 * counter operates with the clock from XTAL1.
 * During transceiver sleep modes the clock falls back to the RTC clock source,
 * regardless of the selected clock. After wakeup, it switches back to the
 * previosly
 * selected clock source.
 */

enum macsc_xtal {
	/* ! 16MHz as macsc clock */
	MACSC_16MHz = 0,
	MACSC_32KHz = 1,
};

/**
 * @brief Reads the 32-bit timer register in the required order of bytes
 *
 * @param hh hh octet of 32-bit register
 * @param hl hl octet of 32-bit register
 * @param lh lh octet of 32-bit register
 * @param ll ll octet of 32-bit register
 *
 * @returns uint32_t Value of timer register
 */
static inline uint32_t macsc_read32(volatile uint8_t *hh,
		volatile uint8_t *hl,
		volatile uint8_t *lh,
		volatile uint8_t *ll)
{
	union {
		uint8_t a[4];
		uint32_t rv;
	}
	x;

	x.a[0] = *ll;
	x.a[1] = *lh;
	x.a[2] = *hl;
	x.a[3] = *hh;

	return x.rv;
}

/* ! compare modes */
#define MACSC_ABSOLUTE_CMP 0
#define MACSC_RELATIVE_CMP 1

/** String concatenation by preprocessor used to create proper register names.
 **/
#define CONCAT(a, b) a ## b

/** Creates proper subregister names and reads the corresponding values. */
#define MACSC_READ32(reg)                  macsc_read32(&CONCAT(reg, HH), \
		&CONCAT(reg, HL), \
		&CONCAT(reg, LH), \
		&CONCAT(reg, LL))

#define MACSC_WRITE32(reg, val)	\
	do { \
		union { uint8_t a[4]; uint32_t v; } \
		x; \
		x.v = val; \
		CONCAT(reg, HH) = x.a[3]; \
		CONCAT(reg, HL) = x.a[2]; \
		CONCAT(reg, LH) = x.a[1]; \
		CONCAT(reg, LL) = x.a[0]; \
	} \
	while (0)

/**
 * \brief Enable MAC SC
 *
 * Enables the SC
 *
 * \param clk_src selection of clk source,avalable options in macsc_xtal,fixed
 *  prescalar
 * \param sleep_enable enable RTC as clock source during sleep
 * \param auto_ts enable automatic timestamping
 *
 */
void macsc_enable(void);

/**
 * \brief Check if MACSC is enabled
 *
 * check if the MACSC is enabled.
 *
 * \param none
 *
 */
bool is_macsc_enable(void);

/**
 * \brief Disable MAC SC
 *
 * Disables the MAC SC
 *
 * \param none
 *
 */
void macsc_disable(void);

/**
 * \brief Check if back-off slot counter is enabled
 *
 * check if the back-off slot counter is enabled.
 *
 * \param none
 *
 */
bool is_macsc_backoff_enable(void);

/**
 * \brief Enables compare interrupts of the MACSC
 *
 * \param channel Compare channel
 */
void macsc_enable_cmp_int(enum macsc_cc_channel channel);

/**
 * \brief Usage of Absolute compare mode of the MACSC
 *
 * \param abs_rel  0 for absoulte cmp;1 for relative cmp
 * \param cmp compare value for SCOCRx register
 * \param channel Compare channel
 */
void macsc_use_cmp(bool abs_rel, uint32_t cmp,enum macsc_cc_channel channel);

/**
 * \ingroup macsc_group
 * \defgroup macsc_interrupt_group MAC Symbol Counter (MACSC) interrupt
 * management
 * This group provides functions to configure MACSC module interrupts
 *
 * @{
 */

/**
 * \brief Set MACSC overflow interrupt callback function
 *
 * This function allows the caller to set and change the interrupt callback
 * function. Without setting a callback function the interrupt handler in the
 * driver will only clear the interrupt flags.
 *
 * \param callback Reference to a callback function
 */
void macsc_set_ovf_int_cb(macsc_callback_t callback);

/**
 * \brief Set MACSC Compare Channel 1 interrupt callback function
 *
 * This function allows the caller to set and change the interrupt callback
 * function. Without setting a callback function the interrupt handler in the
 * driver will only clear the interrupt flags.
 *
 * \param callback Reference to a callback function
 */
void macsc_set_cmp1_int_cb(macsc_callback_t callback);

/**
 * \brief Set MACSC Compare Channel 2 interrupt callback function
 *
 * This function allows the caller to set and change the interrupt callback
 * function. Without setting a callback function the interrupt handler in the
 * driver will only clear the interrupt flags.
 *
 * \param callback Reference to a callback function
 */
void macsc_set_cmp2_int_cb(macsc_callback_t callback);

/**
 * \brief Set MACSC Compare Channel 3 interrupt callback function
 *
 * This function allows the caller to set and change the interrupt callback
 * function. Without setting a callback function the interrupt handler in the
 * driver will only clear the interrupt flags.
 *
 * \param callback Reference to a callback function
 */
void macsc_set_cmp3_int_cb(macsc_callback_t callback);

/**
 * \brief Set MACSC backoff slot counter interrupt callback function
 *
 * This function allows the caller to set and change the interrupt callback
 * function. Without setting a callback function the interrupt handler in the
 * driver will only clear the interrupt flags.
 *
 * \param callback Reference to a callback function
 */
void macsc_set_backoff_slot_cntr_int_cb(macsc_callback_t callback);
//@}

/**
 * \brief Enable 32.768KHz clk using timer 2 async register
 *
 * \param none
 */

static inline void macsc_sleep_clk_enable(void)
{
	ASSR |= (1 << AS2);
}

/**
 * \brief Disable 32.768KHz clk using timer 2 async register
 *
 * \param none
 */
static inline void macsc_sleep_clk_disable(void)
{
	ASSR &= ~(1 << AS2);
}

/* @} */

/**
 * \brief Configure MAC Symbol Counter Clock Source
 *
 * \param macsc macsc clk src
 */
static inline void macsc_write_clock_source(enum macsc_xtal source)
{
	if (source == MACSC_16MHz) {
		SCCR0 |= (source << SCCKSEL);
	} else if (source == MACSC_32KHz) {
		SCCR0 &= ~(1 << SCCKSEL);
	}
}

/**
 * \brief Read MAC SC Clock Source
 *
 * \param none
 * \return macsc_xtal enum Clock source selection
 */
static inline enum macsc_xtal macsc_read_clock_source(void)
{
	return (enum macsc_xtal)(SCCR0 & (1 << SCCKSEL));
}

/**
 * \brief Write the Counter value of the MAC Symbol counter
 *
 * \param cnt_value Counter value
 */
static inline void macsc_write_count(uint32_t cnt_value)
{
	MACSC_WRITE32(SCCNT, cnt_value);
}

/**
 * \brief Reads the Counter value of the MAC Symbol counter
 *
 * \note Output the Counter value
 */
static inline uint32_t macsc_read_count(void)
{
	return (MACSC_READ32(SCCNT));
}

/**
 * \brief enable back-off slot counter
 *
 * \param none
 * \note  Enables interrupt as well	.
 * \note This counter works only if transceiver clock is running.So check Trx
 * state in app before using this function in IEEE802.15.4 applications.
 */
static inline bool macsc_backoff_slot_cnt_enable(void)
{
	if (!(PRR1 & (1 << PRTRX24))) {
		SCCR1 = (1 << SCENBO);
		SCIRQS |= (1 << IRQSBO);
		SCIRQM |= (1 << IRQMBO);
		return true;
	}
	else return false;
}

/**
 * \brief Disable back-off slot counter
 *
 * \param none
 * \note    Disables interrupt as well	.
 * \note This counter works only if transceiver clock is running.So check Trx
 * state in app before using this function in IEEE802.15.4 applications.
 */
static inline void macsc_backoff_slot_cnt_disable(void)
{
	SCCR1 &= ~(1 << SCENBO);
	SCIRQM &= ~(1 << IRQMBO);
}

/**
 * \brief Tests if the Backoff slot cntr interrupt flag is set
 *
 * \return  backoff slot cntr interrupt has occurred or not : IRQSBO
 */
static inline bool macsc_is_slot_cntr_interrupt_flag_set(void)
{
	return (SCIRQS & (1 << IRQSBO));
}

/**
 * \brief Clears the Backoff Slot cntr interrupt flag
 *
 * \note  IRQSBO is cleared
 */
static inline void macsc_clear_slot_cntr_interrupt_flag(void)
{
	SCIRQS |= (1 << IRQSBO);
}

/**
 * \brief Reads the SFD Timestamp register
 *
 * \return Timestamp of last received frame :SCTSR(read-only register)
 */
static inline uint32_t macsc_read_ts(void)
{
	return (MACSC_READ32(SCTSR));
}

/**
 * \brief Reads the Beacon Timestamp register
 *
 * \return Timestamp of last received beacon frame :SCBTSR
 */
static inline uint32_t macsc_read_bts(void)
{
	return (MACSC_READ32(SCBTSR));
}

/**
 * \brief Write the Beacon Timestamp register of the MAC Symbol counter
 * The manual beacon timestamping can be used in conjunction with the
 * relative compare mode of the three compare units to generate compare match
 * interrupts without having a beacon frame received
 *
 * If set to 1, the current symbol counter value is stored into the beacon
 * timestamp register.
 * The bit is cleared afterwards.
 *
 */
static inline void macsc_enable_manual_bts(void)
{
	SCCR0 |= (1 << SCMBTS);
	SCCR0 &= ~(1 << SCTSE);
}

/**
 * \brief Read the MACSC status register
 * This bit is set if a write operation to the symbol counter register is
 * pending. This bit is set after writing the counter low byte (SCCNTLL)
 * until the symbol counter is updated with the new value.
 * This update process can take up to 16 µs and during this time,
 * no read or write access to the 32 bit counter register should occur.
 *
 */
static inline bool macsc_read_status(void)
{
	return (SCSR & (1 << SCBSY));
}

/**
 * \brief Enable the Auto SFD and Beacon Timestamping feature
 * If this bit is zero,only manual beacon timestamping can be used
 *
 */
static inline void macsc_enable_auto_ts(void)
{
	SCCR0 |= (1 << SCTSE);
	SCCR0 &= ~(1 << SCMBTS);
}

/**
 * \brief Enable the MACSC Overflow interrupt
 *
 * \note  IRQMOF is set;Before enabling an interrupt, the
 * corresponding interrupt status bit should be cleared by writing a 1.
 * If the status bit is set and the IRQ gets enabled, the IRQ handler is called
 * immediately
 */
static inline void macsc_enable_overflow_interrupt(void)
{
	SCIRQS |= (1 << IRQSOF);
	SCIRQM |= (1 << IRQMOF);
}

/**
 * \brief Tests if the Overflow flag is set
 *
 * \return  overflow has occurred or not : IRQSOF
 */
static inline bool macsc_is_overflow_flag_set(void)
{
	return (SCIRQS & (1 << IRQSOF));
}

/**
 * \brief Clears the Overflow flag
 *
 * \note  IRQSOF is cleared
 */
static inline void macsc_clear_overflow_flag(void)
{
	SCIRQS |= (1 << IRQSOF);
}

/**
 * \brief Symbol Counter sync
 *
 * \note  to align macsc to within one symbol period
 */
static inline bool macsc_sync(void)
{
	if (!(SCCR0 & (1 << SCCKSEL))) {
		SCCR0 |= (1 << SCRES);
		return true;
	}
	else return false;
}

/**
 * \brief Reports if Compare interrupt has occurred
 *
 * \param channel Compare Channel
 * \return  CCx Interrupt or not
 */
static inline bool macsc_is_cmp_interrupt_flag_set(enum macsc_cc_channel channel)
{
	switch (channel) {
	case MACSC_CC1:
		return(SCIRQS && (1 << IRQSCP1));

		break;

	case MACSC_CC2:
		return(SCIRQS && (1 << IRQSCP2));

		break;

	case MACSC_CC3:
		return(SCIRQS && (1 << IRQSCP3));

		break;

	default:
		return (0);
	}
}

/**
 * \brief Clears Compare interrupt
 *
 * \param channel Compare Channel
 */
static inline void macsc_clear_cmp_interrupt_flag(enum macsc_cc_channel channel)
{
	switch (channel) {
	case MACSC_CC1:
		SCIRQS |= (1 << IRQSCP1);
		break;

	case MACSC_CC2:
		SCIRQS |= (1 << IRQSCP2);
		break;

	case MACSC_CC3:
		SCIRQS |= (1 << IRQSCP3);
		break;

	default:
		break;
	}
}

#ifdef __cplusplus
}
#endif

/**
 * \page megarf_macsc_quickstart Quick Start Guide for the MEGARF MACSC Driver
 *
 * This is the quick start guide for the \ref macscc_group , with step-by-step
 * instructions on how to configure and use the driver for a specific use case.
 * The code examples can be used in the MAC stack to control super-frame timing
 * or a generic application as a general pupose 32-bit timer/counter.
 *
 *
 * \section megarf_macsc_qs_use_cases Use cases
 * - \ref megarf_macsc_qs_ovf
 * - \ref megarf_macsc_qs_cmp
 * - \ref megarf_macsc_qs_backoff_slot_cntr
 * - \ref megarf_macsc_qs_adaptive_sleep
 *
 *
 * \section megarf_macsc_qs_ovf MACSC overflow (interrupt based)
 *
 * This use case will prepare the MACSC to trigger a interrupt when the timer
 * overflows. The interrupt is handled by a customisable call back function.
 *
 * We will setup the MACSC in this mode:
 * - Use the system clock as clock source;provision to use the 32.768KHz clock
 * is provided
 * - Sleep and Automatic timestamping can be enabled as per application
 * requirement
 * - Overflow interrupt enable and callback
 *
 *
 * \section megarf_macsc_qs_cmp Setup steps
 *
 * \subsection megarf_macsc_qs_cmp_usage_prereq Prequisites
 *
 * For the setup code of this use case to work, the following must
 * be added to the project:
 * - \ref interrupt_group
 *
 * \section megarf_macsc_qs_cmp MACSC compare match (interrupt based)
 *
 * This use case will prepare the MACSC to trigger three independent interrupts
 * when it reaches three different compare values.
 * The three compare matches will be handled by three separate
 * interrupts implemented in call back functions.
 * Absolute compare mode of the MACSC is used.
 * \ref COMPARE_MODE in conf_example.h
 *
 * We will setup the MACSC in this mode:
 * - Use the system clock as clock source;provision to use the 32.768KHz clock
 * is provided \ref RTC_CLK_SRC in conf_example.h
 * - Sleep and Automatic timestamping can be enabled as per application
 * requirement
 * - Compare match 1 interrupt triggers after quarter of a second
 * - Compare match 2 interrupt trigger after half a second
 * - Compare match 3 interrupt triggers after one second
 * \ref CONFIG_MACSC_TIMEOUT_TICK_HZ in conf_example.h
 *
 * \section megarf_macsc_qs_backoff_slot_cntr MACSC back off slot counter usage
 *(interrupt based)
 *
 * This use case will prepare the MACSC to use the backoff slot counter module
 * interrupt implemented in call back function.
 *
 * We will setup the MACSC in this mode:
 * - Use the system clock as clock source;provision to use the 32.768KHz clock
 * is provided
 * - Sleep and Automatic timestamping can be enabled as per application
 * requirement \ref ENABLE_SLEEP and \ref ENABLE_AUTO_TIMESTAMP in conf_example.h
 * - Back-off slot counter interrupt occurs every 320us(20 IEEE 802.15.4 symbol
 * periods)
 *
 * \section megarf_macsc_qs_adaptive_sleep MACSC sleep-wakeup scheme (interrupt
 * based)
 *
 * This use case will prepare the MACSC to use the relative compare mode to
 * remain awake during the
 * Contention Access Period(CAP) and required GTS of the superframe and
 * sleep during the sleep period.All 3 compare modules are used along with
 * Beacon Timestamp register.
 * Ideally,Beacon Timestamp register is updated with SCCNT for every incoming
 * IEEE802.15.4 frame.
 * However,this application simulates the scenario by enable manual write of the
 * Beacon Timestamp register to demonstrate relative compare.
 * \ref COMPARE_MODE in conf_example.h
 * interrupts implemented in call back function.
 *
 * We will setup the MACSC in this mode:
 * - Use the system clock as clock source;provision to use the 32.768KHz clock
 * is provided during sleep
 * - Sleep and Automatic/Manual timestamping can be enabled as per application
 * requirement \ref ENABLE_SLEEP and \ref ENABLE_AUTO_TIMESTAMP in conf_example.h
 * - Set Beacon Interval and Superframe Interval as per application requirement.
 * - In real networks,Superframe order and Beacon order are to be retrieved from
 * incoming frame's header.
 * \ref BEACON_INTERVAL,\ref ACTIVE_PERIOD,\ref SLEEP_PERIOD in conf_example.h
 */

#endif /* MACSC_MEGARF_H */
