/**
 * \file
 *
 * \brief API driver for MMA7341L accelerometer component.
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

#ifndef MMA7341L_H_INCLUDED
#define MMA7341L_H_INCLUDED

#include "compiler.h"
#include "board.h"
#include "conf_mma7341l.h"

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
 extern "C" {
#endif
/**INDENT-ON**/
/// @endcond

/** Accelerometer filtering value */
#define MMA7341L_SHIFT  0
/** Accelerometer 0g output voltage value is 1.65v, converted digital value 0x400/2 (10-bit ADC) */
#define MMA7341L_ZERO  (0x1FF >> MMA7341L_SHIFT)
/** Accelerometer delta 1g digital value is 0.44/3.3 * 0x400 (10-bit ADC) */
#define MMA7341L_1G    (0x88 >> MMA7341L_SHIFT)

#define MMA7341L_ZERO_X  MMA7341L_ZERO
#define MMA7341L_ZERO_Y  MMA7341L_ZERO
#define MMA7341L_ZERO_Z  MMA7341L_ZERO

/* The rotation direction definition */
#define  MMA7341L_UNCHANGED  0
#define  MMA7341L_UP         1
#define  MMA7341L_DOWN       2
#define  MMA7341L_LEFT       3
#define  MMA7341L_RIGHT      4
#define  MMA7341L_UP_RIGHT   5
#define  MMA7341L_UP_LEFT    6
#define  MMA7341L_DOWN_LEFT  7
#define  MMA7341L_DOWM_RIGHT 8

/** Definition of MMA7341L working mode */
#define MMA7341L_NORMAL_MODE   1
#define MMA7341L_SLEEP_MODE    0

#ifndef MMA7341L_ADC_CHANNEL_X
#warning an ADC channel is not defined as X axis channel. Using default settings.
#define MMA7341L_ADC_CHANNEL_X   2
#endif
#ifndef MMA7341L_ADC_CHANNEL_Y
#warning an ADC channel is not defined as Y axis channel. Using default settings.
#define MMA7341L_ADC_CHANNEL_Y   6
#endif
#ifndef MMA7341L_ADC_CHANNEL_Z
#warning an ADC channel is not defined as Z axis channel. Using default settings.
#define MMA7341L_ADC_CHANNEL_Z   7
#endif

#define MMA7341L_EOC_X    (0x1u << MMA7341L_ADC_CHANNEL_X)
#define MMA7341L_EOC_Y    (0x1u << MMA7341L_ADC_CHANNEL_Y)
#define MMA7341L_EOC_Z    (0x1u << MMA7341L_ADC_CHANNEL_Z)

typedef struct {
	int32_t x;
	int32_t y;
	int32_t z;
} xyz_t;

typedef struct {
	float x_g;
	float y_g;
	float z_g;
} xyz_g_t;

typedef struct {
	xyz_t m;  /* Immediate acceleration */
	xyz_t k;   /* Calibration basis */
	xyz_t ak; /* Acceleration difference */
} acc_t;

void mma7341l_calibration(void);
void mma7341l_init(void);
void mma7341l_set_mode(uint8_t uc_mode);
xyz_g_t mma7341l_get_gravity_value(void);
uint32_t mma7341l_get_rotation_dir(void);

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/// @endcond

#endif /* MMA7341L_H_INCLUDED */
