/**
 * \file
 *
 * \brief CAN transceiver SN65HVD234 driver.
 *
 * Copyright (c) 2011-2015 Atmel Corporation. All rights reserved.
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

#ifndef CAN_SN65HVD234_H
#define CAN_SN65HVD234_H

#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	/** PIO dedicated to RS pin index. */
	uint32_t pio_rs_idx;
	/** PIO dedicated to EN pin index. */
	uint32_t pio_en_idx;
} sn65hvd234_ctrl_t;

void sn65hvd234_set_rs(sn65hvd234_ctrl_t *p_component, uint32_t pin_idx);
void sn65hvd234_set_en(sn65hvd234_ctrl_t *p_component, uint32_t pin_idx);
void sn65hvd234_enable(sn65hvd234_ctrl_t *p_component);
void sn65hvd234_disable(sn65hvd234_ctrl_t *p_component);
void sn65hvd234_enable_low_power(sn65hvd234_ctrl_t *p_component);
void sn65hvd234_disable_low_power(sn65hvd234_ctrl_t *p_component);

#ifdef __cplusplus
}
#endif
#endif /* CAN_SN65HVD234_H */
