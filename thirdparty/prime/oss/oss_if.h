/**
 * \file
 *
 * \brief Operative System Support Interface
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
