/**
 * \file
 *
 * \brief BNO055 configuration.
 *
 * Copyright (c) 2015 Atmel Corporation. All rights reserved.
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

/* Configuration of the mma7341 accelerometer driver */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef CONF_BNO055_H_INCLUDED
#define CONF_BNO055_H_INCLUDED

#define BNO055_I2C_SLAVE_ADDRESS  BNO055_I2C_ADDR2

/* Slave address select pin */
#define BNO055_PIN_SLAVE_ADDR_SELECT  PIO_PB3_IDX
/* Boot */
#define BNO055_PIN_BOOT               PIO_PC17_IDX
/* Reset */
#define BNO055_PIN_RESET              PIO_PD25_IDX

/* Sleep State Macros */
#define SLEEP_STATE_SLEEP    UINT8_C(1)
#define SLEEP_STATE_AWAKE    UINT8_C(0)

/* name RGB LED Macros */
#define RGB_LED_R				PIO_PC30_IDX
#define RGB_LED_G				PIO_PB2_IDX
#define RGB_LED_B				PIO_PA0_IDX
#define RGB_LED_G_ON			false
#define RGB_LED_G_OFF			true
#define RGB_LED_B_VALUE			(0xFFFF - ((bno055_euler_data.h) * 0xFFFF / 5759))
#define RBG_LED_B_OFF			UINT16_C(0xFFFF)
#define RGB_LED_R_VALUE			(0xFFFF - ((bno055_euler_data.p) * 0xFFFF / 5759))
#define RBG_LED_R_OFF			UINT16_C(0xFFFF)

/* determines the active state of BNO055 reset */
#define BNO055_RESET_ACTIVE       false
/* the delay required to wait for BNO055 chip to reset */
#define BNO055_RESET_DELAY_MS     UINT32_C(650)

#define BNO055_TWIHS_CLK          400000

#endif /* CONF_BNO055_H_INCLUDED */
