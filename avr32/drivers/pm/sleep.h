/**
 * \file
 *
 * \brief Sleep mode access
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
 *
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */
#ifndef SLEEP_H
#define SLEEP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <compiler.h>

#ifndef AVR32_PM_SMODE_GMCLEAR_MASK
#define AVR32_PM_SMODE_GMCLEAR_MASK      0x80
#else
#warning Define already present in the system toolchain header files!
#endif

/**
 * \defgroup sleep_group Power Manager (PM)
 *
 * This is a stub on the AVR UC3 Power Manager(PM) for the sleepmgr service.
 *
 * \note To minimize the code overhead, these functions do not feature
 * interrupt-protected access since they are likely to be called inside
 * interrupt handlers or in applications where such protection is not
 * necessary. If such protection is needed, it must be ensured by the calling
 * code.
 *
 * @{
 */

#if defined(__DOXYGEN__)
/**
 * \brief Sets the MCU in the specified sleep mode
 * \param sleep_mode Sleep mode to set.
 */
#endif

#if (UC3A || UC3B)  // For AVR UC3 A0/A1, UC3 B, UC3 A3 series

# ifndef AVR32_PM_SMODE_DEEPSTOP
#  define AVR32_PM_SMODE_DEEPSTOP          0x00000004
# endif
# include "pm.h"
#elif UC3C          // For AVR UC3 C series
# include "pm_uc3c.h"
#elif UC3D          // For AVR UC3 D series
# include "pm_uc3d.h"
#elif UC3L          // For AVR UC3 L series
# include "pm_uc3l.h"
#else
# error Unsupported AVR UC3 series.
#endif

static inline void pm_sleep(int sleep_mode)
{
	switch (sleep_mode) {
	case AVR32_PM_SMODE_IDLE:
		SLEEP(AVR32_PM_SMODE_IDLE);
		break;
	case AVR32_PM_SMODE_GMCLEAR_MASK | AVR32_PM_SMODE_IDLE:
		SLEEP(AVR32_PM_SMODE_GMCLEAR_MASK | AVR32_PM_SMODE_IDLE);
		break;
	case AVR32_PM_SMODE_FROZEN:
		SLEEP(AVR32_PM_SMODE_FROZEN);
		break;
	case AVR32_PM_SMODE_GMCLEAR_MASK | AVR32_PM_SMODE_FROZEN:
		SLEEP(AVR32_PM_SMODE_GMCLEAR_MASK | AVR32_PM_SMODE_FROZEN);
		break;
	case AVR32_PM_SMODE_STANDBY:
		SLEEP(AVR32_PM_SMODE_STANDBY);
		break;
	case AVR32_PM_SMODE_GMCLEAR_MASK | AVR32_PM_SMODE_STANDBY:
		SLEEP(AVR32_PM_SMODE_GMCLEAR_MASK | AVR32_PM_SMODE_STANDBY);
		break;
	case AVR32_PM_SMODE_STOP:
		SLEEP(AVR32_PM_SMODE_STOP);
		break;
	case AVR32_PM_SMODE_GMCLEAR_MASK | AVR32_PM_SMODE_STOP:
		SLEEP(AVR32_PM_SMODE_GMCLEAR_MASK | AVR32_PM_SMODE_STOP);
		break;
	case AVR32_PM_SMODE_DEEPSTOP:
		SLEEP(AVR32_PM_SMODE_DEEPSTOP);
		break;
	case AVR32_PM_SMODE_GMCLEAR_MASK | AVR32_PM_SMODE_DEEPSTOP:
		SLEEP(AVR32_PM_SMODE_GMCLEAR_MASK | AVR32_PM_SMODE_DEEPSTOP);
		break;
	case AVR32_PM_SMODE_STATIC:
		SLEEP(AVR32_PM_SMODE_STATIC);
		break;
	case AVR32_PM_SMODE_GMCLEAR_MASK | AVR32_PM_SMODE_STATIC:
		SLEEP(AVR32_PM_SMODE_GMCLEAR_MASK | AVR32_PM_SMODE_STATIC);
		break;
#if UC3L
	case AVR32_PM_SMODE_SHUTDOWN:
		SLEEP(AVR32_PM_SMODE_SHUTDOWN);
		break;
	case AVR32_PM_SMODE_GMCLEAR_MASK | AVR32_PM_SMODE_SHUTDOWN:
		SLEEP(AVR32_PM_SMODE_GMCLEAR_MASK | AVR32_PM_SMODE_SHUTDOWN);
		break;
#endif
	default:
		break;
	}
}


//! @}

#ifdef __cplusplus
}
#endif

#endif /* SLEEP_H */
