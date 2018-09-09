/**
 * \file
 *
 * \brief Chip-specific reset cause functions
 *
 * Copyright (c) 2012-2018 Microchip Technology Inc. and its subsidiaries.
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
#ifndef MEGA_DRIVERS_CPU_RESET_CAUSE_H
#define MEGA_DRIVERS_CPU_RESET_CAUSE_H

#include "compiler.h"


/**
 * \ingroup reset_cause_group
 * \defgroup mega_rf_reset_cause_group MEGA reset cause
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
	#define CHIP_RESET_CAUSE_EXTRST         (1<<EXTRF)
	//! \internal brown-out detected reset cause, same as for CPU
	#define CHIP_RESET_CAUSE_BOD_CPU        (1<<BORF)
	//! \internal Power-on-reset reset cause
	#define CHIP_RESET_CAUSE_POR            (1<<PORF)
	//! \internal Watchdog timeout reset cause
	#define CHIP_RESET_CAUSE_WDT            (1<<WDRF)
#if !MEGA_XX8 && !MEGA_XX8_A
	//! \internal Software reset reset cause
	#define CHIP_RESET_CAUSE_JTAG           (1<<JTRF)
#endif
//@}
static inline reset_cause_t reset_cause_get_causes(void)
{
#if (MEGA_XX4 ||MEGA_XX4_A || MEGA_XX8 || MEGA_XX8_A || \
	MEGA_XX || MEGA_XX_UN2 || MEGA_XX0_1 || MEGA_RF || MEGA_UNCATEGORIZED) && !MEGA_XX_UN0 && !MEGA_XX_UN1
	uint8_t temp_mcsr = MCUSR ;
	return temp_mcsr;
#else	
	uint8_t temp_mcsr = MCUCSR ;
	return temp_mcsr;
#endif 	
}

static inline void reset_cause_clear_causes(reset_cause_t causes)
{
#if (MEGA_XX4 ||MEGA_XX4_A || MEGA_XX8 || MEGA_XX8_A || \
	MEGA_XX || MEGA_XX_UN2 || MEGA_XX0_1 || MEGA_RF || MEGA_UNCATEGORIZED) && !MEGA_XX_UN0 && !MEGA_XX_UN1
	MCUSR &= ~causes;
#else	
	MCUCSR &= ~causes;
#endif
}

//! @}

#endif /* MEGA_DRIVERS_CPU_RESET_CAUSE_H */
