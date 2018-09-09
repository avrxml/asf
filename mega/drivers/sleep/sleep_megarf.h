/**
 * \file
 *
 * \brief Sleep controller driver for megaRF devices
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
#ifndef SLEEP_MEGARF_H
#define SLEEP_MEGARF_H

/**
 * \defgroup sleep_group Sleep controller driver
 *
 * This is a low-level driver implementation for the MEGA RF sleep controller.
 *
 * \note To minimize the code overhead, these functions do not feature
 * interrupt-protected access since they are likely to be called inside
 * interrupt handlers or in applications where such protection is not
 * necessary. If such protection is needed, it must be ensured by the calling
 * code.
 *
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__ICCAVR__) || defined(__DOXYGEN__)

/**
 * \brief Enable sleep
 */
static inline void sleep_enable(void)
{
	SMCR |= (1 << SE);
}

/**
 * \brief Disable sleep
 */
static inline void sleep_disable(void)
{
	SMCR &= ~(1 << SE);
}

#endif

/**
 * \brief Set new sleep mode
 *
 * \param mode Sleep mode, from the device IO header file.
 */
static inline void sleep_set_mode(enum SLEEP_SMODE_enum mode)
{
	SMCR = mode | (SMCR & ~((1 << SM0) | (1 << SM1) | (1 << SM2)));
}

#ifdef __cplusplus
}
#endif

/*! @} */

#endif /* SLEEP_MEGARF_H */
