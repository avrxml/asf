/**
 * \file
 *
 * \brief Operative System Support Interface
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

#ifndef OSS_IF_H_INCLUDED
#define OSS_IF_H_INCLUDED

/* @cond 0 */
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/* @endcond */

/**
 * \ingroup prime_group
 * \defgroup oss_prime_group PRIME Operative System Support
 *
 * This module provides configuration and utils for the OSS layer,
 * which is responsible of handling OS for the PRIME stack.
 *
 * @{
 */

/* \name PRIME Stack priority */
/* @{ */
#define TASK_PRIME_PRIO                    (tskIDLE_PRIORITY + 3)
/* @} */

/* \name PRIME Stack definition */
/* @{ */
#define TASK_PRIME_STACK                   (configMINIMAL_STACK_SIZE * 10)
/* @} */

/* \name Main PRIME Stack Update Period */
/* \note It must be 1 milliseconds in order to update the PRIME systick */
/* @{ */
#define PRIME_UPDATE_PROCESS_TIMER_RATE    (1 / portTICK_RATE_MS)
/* @} */

/* \name PRIME Stack Process Period */
/* @{ */
#define PRIME_PROCESS_TIMER_RATE           (5 / portTICK_RATE_MS)
/* @} */

/* \name PRIME Led Indication Period */

/* \note Defines the LED toggled to provide visual feedback that the system
 * status */
/* @{ */
#define PRIME_LED_PROCESS_TIMER_RATE       (250 / portTICK_RATE_MS)
/* @} */

/* \name PRIME OSS interface */
/* @{ */
void vPrimeStackInitTask(void);

/* @} */
/* @} */

/* / @cond 0 */
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/* / @endcond */

#endif /* OSS_IF_H_INCLUDED */
