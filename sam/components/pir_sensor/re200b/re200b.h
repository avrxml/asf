/**
 * \file
 *
 * \brief API driver for re200b pir sensor component.
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

#ifndef RE200B_H_INCLUDED
#define RE200B_H_INCLUDED

#include "compiler.h"
#include "board.h"
#include "acc.h"
#include "pmc.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * enumeration of different result for acc comparison
 */
typedef enum _e_cmp_res {
	CMP_EQUAL, /*!< equal comparison ( inp=inn) */
	CMP_GREATER, /*!< greater comparison ( inp>inn) */
	CMP_LESS /*!< equal comparison ( inp<inn) */
} e_cmp_res;

void re200b_motion_detect_handler(void);
void re200b_motion_detect_init(uint32_t ul_acc_minus, uint32_t ul_acc_plus);
void re200b_motion_detect_reset(void);
void re200b_motion_detect_enable(void);
void re200b_motion_detect_disable(void);
e_cmp_res re200b_motion_detect_get_compare_result(void);
uint32_t re200b_motion_detection(void);

#ifdef __cplusplus
}
#endif

#endif /* RE200B_H_INCLUDED */
