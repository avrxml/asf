/**
 * @file common_hw_timer.h
 *
 * @brief
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
