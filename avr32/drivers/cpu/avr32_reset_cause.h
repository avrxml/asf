/**
 * \file
 *
 * \brief Chip-specific reset cause functions
 *
 * Copyright (c) 2010-2018 Microchip Technology Inc. and its subsidiaries.
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
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */
#ifndef AVR32_DRIVERS_CPU_RESET_CAUSE_H
#define AVR32_DRIVERS_CPU_RESET_CAUSE_H

#include "compiler.h"
#include "sysclk.h"

/**
 * \defgroup group_avr32_drivers_cpu AVR32 reset cause
 * \ingroup reset_cause_group
 *
 * See \ref reset_cause_quickstart
 *
 * @{
 */

/**
 * \brief Chip-specific reset cause type capable of holding all chip reset
 * causes. Typically reflects the size of the reset cause register.
 */
typedef uint32_t        reset_cause_t;

//! \internal \name Chip-specific reset causes
//@{

#if defined(AVR32_PM_RCAUSE_EXT_MASK) || defined(__DOXYGEN__)
//! \internal External reset cause
# define CHIP_RESET_CAUSE_EXTRST        AVR32_PM_RCAUSE_EXT_MASK
#endif

#if defined(AVR32_PM_RCAUSE_BOD_MASK) || defined(__DOXYGEN__)
//! \internal Brown-out detected on CPU power domain reset cause
# define CHIP_RESET_CAUSE_BOD_CPU       AVR32_PM_RCAUSE_BOD_MASK
#endif

#if defined(AVR32_PM_RCAUSE_BOD33_MASK) || defined(__DOXYGEN__)
//! \internal Brown-out detected on I/O power domain reset cause
# define CHIP_RESET_CAUSE_BOD_IO        AVR32_PM_RCAUSE_BOD33_MASK
#endif

#if defined(AVR32_PM_RCAUSE_OCDRST_MASK) || defined(__DOXYGEN__)
//! \internal On-chip debug system reset cause
# define CHIP_RESET_CAUSE_OCD           AVR32_PM_RCAUSE_OCDRST_MASK
#endif

#if defined(AVR32_PM_RCAUSE_POR_MASK) || defined(__DOXYGEN__)
//! \internal Power-on-reset reset cause
# define CHIP_RESET_CAUSE_POR           AVR32_PM_RCAUSE_POR_MASK
#endif

/* Figure out different kinds of JTAG (programmer interfaces) reset causes. */
#if defined(AVR32_PM_RCAUSE_JTAG_MASK) || defined(__DOXYGEN__)
# define RCAUSE_JTAG                    AVR32_PM_RCAUSE_JTAG_MASK
#else
# define RCAUSE_JTAG                    0
#endif
#if defined(AVR32_PM_RCAUSE_JTAGHARD_MASK) || defined(__DOXYGEN__)
# define RCAUSE_JTAGHARD                AVR32_PM_RCAUSE_JTAGHARD_MASK
#else
# define RCAUSE_JTAGHARD                0
#endif
#if defined(AVR32_PM_RCAUSE_AWIRE_MASK) || defined(__DOXYGEN__)
# define RCAUSE_AWIRE                   AVR32_PM_RCAUSE_AWIRE_MASK
#else
# define RCAUSE_AWIRE                   0
#endif
#if defined(AVR32_PM_RCAUSE_AWIREHARD_MASK) || defined(__DOXYGEN__)
# define RCAUSE_AWIREHARD               AVR32_PM_RCAUSE_AWIREHARD_MASK
#else
# define RCAUSE_AWIREHARD               0
#endif

//! \internal JTAG reset reset cause
#define CHIP_RESET_CAUSE_JTAG           (RCAUSE_JTAG | RCAUSE_JTAGHARD | \
		RCAUSE_AWIRE | RCAUSE_AWIREHARD)

#if defined(AVR32_PM_RCAUSE_WDT_MASK) || defined(__DOXYGEN__)
//! \internal Watchdog timeout reset cause
# define CHIP_RESET_CAUSE_WDT           AVR32_PM_RCAUSE_WDT_MASK
#endif

#if defined(AVR32_PM_RCAUSE_CPUERR_MASK) || defined(__DOXYGEN__)
//! \internal CPU error reset cause
# define CHIP_RESET_CAUSE_CPU_ERROR     AVR32_PM_RCAUSE_CPUERR_MASK
#endif

#if defined(AVR32_PM_RCAUSE_SLEEP_MASK) || defined(__DOXYGEN__)
//! \internal Wake from Shutdown sleep mode reset cause
#define CHIP_RESET_CAUSE_SLEEP          AVR32_PM_RCAUSE_SLEEP_MASK
#endif

//@}

static inline reset_cause_t reset_cause_get_causes(void)
{
	volatile avr32_pm_t     *pm = &AVR32_PM;
	return (reset_cause_t)pm->rcause;
}

static inline void reset_cause_clear_causes(reset_cause_t causes)
{
	/**
	 * \note Reset causes are not clearable on 32-bit AVR.
	 */
}

static inline void reset_do_soft_reset(void)
{
	sysclk_enable_cpu_module(SYSCLK_OCD);

	/**
	 * \note This function requires the device to be in supervisor mode.
	 */
	Set_debug_register(AVR32_DC, AVR32_DC_DBE_MASK);
	Set_debug_register(AVR32_DC, AVR32_DC_RES_MASK);

	while (1) {
		/* Intentionally empty. */
	}
}

//! @}

#endif /* AVR32_DRIVERS_CPU_RESET_CAUSE_H */
