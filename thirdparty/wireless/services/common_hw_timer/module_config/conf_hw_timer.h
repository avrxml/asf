/**
 * \file *********************************************************************
 *
 * \brief Common Hardware Timer configuration
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
