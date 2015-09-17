/**
 * \file
 *
 * \brief MMA7341L configuration.
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

/* Configuration of the mma7341 accelerometer driver */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef CONF_MMA7341L_H_INCLUDED
#define CONF_MMA7341L_H_INCLUDED

// Definition of MMA7341L x,y,z axis channel number
#define MMA7341L_ADC_CHANNEL_X    2
#define MMA7341L_ADC_CHANNEL_Y    6
#define MMA7341L_ADC_CHANNEL_Z    7

// MMA7341L mode set pin definitions
#define PIN_MMA7341L_MODE         PIO_PC13_IDX
#define PIN_MMA7341L_MODE_FLAG    (PIO_OUTPUT_1 | PIO_DEFAULT)

// MMA7341L X,Y,Z axis pin definitions
#define PIN_MMA7341L_X_AXIS       PIO_PB3_IDX
#define PIN_MMA7341L_X_AXIS_FLAG  (PIO_INPUT | PIO_DEFAULT)
#define PIN_MMA7341L_Y_AXIS       PIO_PC17_IDX
#define PIN_MMA7341L_Y_AXIS_FLAG  (PIO_INPUT | PIO_DEFAULT)
#define PIN_MMA7341L_Z_AXIS       PIO_PC18_IDX
#define PIN_MMA7341L_Z_AXIS_FLAG  (PIO_INPUT | PIO_DEFAULT)

#define MMA7341L_ADC_CLK   100000

#define MMA7341L_USE_ADC12
//#define MMA7341L_USE_ADC10

#endif /* CONF_MMA7341L_H_INCLUDED */
