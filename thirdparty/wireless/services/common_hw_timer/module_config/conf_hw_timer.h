/**
 * \file *********************************************************************
 *
 * \brief Common Hardware Timer configuration
 *
 * Copyright (c) 2014-2016 Atmel Corporation. All rights reserved.
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
 */

#ifndef CONF_HW_TIMER_H_INCLUDED
#define CONF_HW_TIMER_H_INCLUDED

/* ! \name Configuration for XMEGA */
/* ! @{ */
#if (XMEGA)
#define TIMER     (&TCC0)
#endif /* XMEGA */
/* ! @} */

/* ! \name Configuration for UC3 */
/* ! @{ */
#if (UC3)
#define TIMER                (&AVR32_TC0)
#define TIMER_CHANNEL_ID     0
#endif /* UC3 */
/* ! @} */

/* ! \name Configuration for SAM4L */
/* ! @{ */
#if (SAM4L)
#define TIMER                (TC0)
#define TIMER_CHANNEL_ID     (0)
#endif /* SAM4L */

#if (SAM4S || SAM4E)
#define TIMER                (TC0)
#define TIMER_CHANNEL_ID     0
#define ID_TC                (ID_TC0)
#endif /* SAM4S */
/* ! @} */

/* ! \name Configuration for MEGARF */
/* ! @{ */
#if (MEGA_RF)
#define TIMER     (&TCCR1A)
#endif /* MEGA_RF */
/* ! @} */
/* ! \name Configuration for SAMD20 */
/* ! @{ */
#if (SAMD20)
#define TIMER     (TC0)
#endif /* SAMD */
/* ! @} */
/* ! \name Configuration for SAMD21/SAMR21 */
/* ! @{ */
#if (SAMD21 || SAMR21)
#define TIMER                (TC3)
#define TIMER_CHANNEL_ID     0
#endif
/* ! @} */
/* ! \name Configuration for SAML21 */
/* ! @{ */
#if (SAML21 || SAMR30)
#define TIMER      (TC0)
#endif
/* ! @} */

#endif /* CONF_HW_TIMER_H_INCLUDED */
