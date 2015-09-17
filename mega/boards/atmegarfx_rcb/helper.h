/**
 * \file
 *
 * \brief ATmegaRFX RCB board header file.
 *
 * This file contains definitions and services related to the features of the
 * ATmega256RFR2 Xplained Pro board.
 *
 * To use this board, define BOARD= ATMEGA256RFR2_XPLAINED_PRO.
 *
 * Copyright (c) 2013-2015 Atmel Corporation. All rights reserved.
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
#ifndef _HELPER_
#define _HELPER_
#include "compiler.h"
#include "conf_board.h"
#include "board.h"
#include "gpio.h"
#include "led.h"


#ifdef ADC_ACCELEROMETER
/* Enumerations used to identify ADC Channels */
typedef enum adc_channel_tag
{
	X_AXIS,
	Y_AXIS,
	Z_AXIS,
	ADC_REF
} SHORTENUM adc_channel_t;

/* Enumerations states for enabling or disabling the Accelerometer */
typedef enum acc_status_tag
{
	ACC_OFF,
	ACC_ON
} SHORTENUM acc_status_t;

#endif
#ifdef KEY_RC_BOARD
#define button_id_t             uint32_t
void pulse_latch(void);
void set_button_pins_for_normal_mode(void);
void led_ctrl(led_id_t led_no, led_action_t led_setting);
#ifdef ADC_ACCELEROMETER
void adc_init(void);
void acc_init(void);
void acc_disable(void);
uint16_t adc_read(adc_channel_t channel);
void read_acc(uint16_t *x, uint16_t *y, uint16_t *z, uint16_t *ref);

#endif
button_id_t button_scan(void);
void update_latch_status(void);
#else /* KEY_RC_BOARD */

typedef enum
{
    PLAIN,
    SENSOR_TERM_BOARD    
} board_t;

typedef struct
{
    uint16_t addr;
    uint8_t val;
} mem_test_t;


#define NUM_CHECK 10



/**
 * \brief Read XRAM
 *
 * \param
 */
 uint8_t xram_read(uint16_t addr);
 
void xram_write(uint16_t addr, uint8_t data);

void board_identify(void);

bool stb_button_read(void);

void led_helper_func(void);

void led_ctrl(led_id_t led_no, led_action_t led_setting);
#endif /* KEY_RC_BOARD */

#endif  /* _HELPER_ */
