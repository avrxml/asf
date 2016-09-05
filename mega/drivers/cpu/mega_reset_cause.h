/**
 * \file
 *
 * \brief Chip-specific reset cause functions
 *
 * Copyright (c) 2012-2016 Atmel Corporation. All rights reserved.
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
