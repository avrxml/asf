/**
 * @file common_hw_timer.h
 *
 * @brief
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

/* Prevent double inclusion */
#ifndef COMMON_HW_TIMER_H
#define COMMON_HW_TIMER_H

#include <stdint.h>
#include "hw_timer.h"

/**
 * \ingroup group_pal_common_sw_timer
 * \defgroup group_pal_common_hw_timer
 * Implements a common api interface for accessing timer drivers of all
 * architectures
 * @{
 */

#define DEF_1MHZ        (1000000)
#define TIMER_PERIOD  UINT16_MAX

/*! \brief  To initialize common hw timer
 */
void common_tc_init(void);
void hw_overflow_cb(void);
void hw_expiry_cb(void);

/*! \brief  To read current count of the timer after issuing common tc delay
 */
uint16_t common_tc_read_count(void);

/*! \brief  To create a non blocking delay
 *  \param  value delay in us
 */
void common_tc_delay(uint16_t value);

/*! \brief  To disable timer compare interrupt
 */
void common_tc_compare_stop(void);

/*! \brief  To disable timer overflow interrupt
 */
void common_tc_overflow_stop(void);

/*! \brief  To disable timer
 */
void common_tc_stop(void);

/*! \brief  to set common hw timer overflow callback
 */
void set_common_tc_overflow_callback(tmr_callback_t callback);

/*! \brief  to set common hw timer compare callback
 */
void set_common_tc_expiry_callback(tmr_callback_t callback);

/*! \brief  timer overflow interrupt callback
 */
void tmr_ovf_callback(void);

/*! \brief  timer compare interrupt callback
 */
void tmr_cca_callback(void);

/* ! @} */
#endif /* COMMON_HW_TIMER_H */
/* EOF */
