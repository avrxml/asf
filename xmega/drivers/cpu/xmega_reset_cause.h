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
#ifndef XMEGA_DRIVERS_CPU_RESET_CAUSE_H
#define XMEGA_DRIVERS_CPU_RESET_CAUSE_H

#include "compiler.h"
#include "ccp.h"

/**
 * \ingroup reset_cause_group
 * \defgroup xmega_reset_cause_group XMEGA reset cause
 *
 * See \ref reset_cause_quickstart
 *
 * @{
 */

/**
 * \brief Chip-specific reset cause type capable of holding all chip reset
 * causes. Typically reflects the size of the reset cause register.
 */
typedef uint8_t         reset_cause_t;

//! \internal \name Chip-specific reset causes
//@{
//! \internal External reset cause
#define CHIP_RESET_CAUSE_EXTRST         RST_EXTRF_bm
//! \internal brown-out detected reset cause, same as for CPU
#define CHIP_RESET_CAUSE_BOD_IO         RST_BORF_bm
//! \internal Brown-out detected reset cause, same as for I/O
#define CHIP_RESET_CAUSE_BOD_CPU        RST_BORF_bm
//! \internal On-chip debug system reset cause
#define CHIP_RESET_CAUSE_OCD            RST_PDIRF_bm
//! \internal Power-on-reset reset cause
#define CHIP_RESET_CAUSE_POR            RST_PORF_bm
//! \internal Software reset reset cause
#define CHIP_RESET_CAUSE_SOFT           RST_SRF_bm
//! \internal Spike detected reset cause
#define CHIP_RESET_CAUSE_SPIKE          RST_SDRF_bm
//! \internal Watchdog timeout reset cause
#define CHIP_RESET_CAUSE_WDT            RST_WDRF_bm
//@}

static inline reset_cause_t reset_cause_get_causes(void)
{
	return (reset_cause_t)RST.STATUS;
}

static inline void reset_cause_clear_causes(reset_cause_t causes)
{
	RST.STATUS = causes;
}

static inline void reset_do_soft_reset(void)
{
	ccp_write_io((void *)&RST.CTRL, RST_SWRST_bm);

	while (1) {
		/* Intentionally empty. */
	}
}

//! @}

#endif /* XMEGA_DRIVERS_CPU_RESET_CAUSE_H */
