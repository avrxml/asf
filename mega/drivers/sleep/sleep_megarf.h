/**
 * \file
 *
 * \brief Sleep controller driver for megaRF devices
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
