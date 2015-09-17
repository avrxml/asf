/**
 * \file
 *
 * \brief Task to manage the sensor sampling
 *
 * Copyright (C) 2014-2015 Atmel Corporation. All rights reserved.
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
#ifndef APP_SAMPLING_H_INCLUDED
#define APP_SAMPLING_H_INCLUDED

/**
 * \ingroup demo_xmegae5_xplained_group
 * \defgroup sampling_task_group Sampling task
 *
 * The schedule and the ADC conversions are done only by interrupt
 * (RTC and ADC) to guarantee a constant acquisition. The FIFO service
 * is used to save value, thus the sampling task can read FIFO, send values
 * through UART and display values without timing constraint.
 * The implementation is available in app_sampling.c/.h files.
 *
 * @{
 */

/**
 * \brief Initialize ADC channels, RTC and start RTC alarm
 */
void app_sampling_init(void);

/**
 * \brief Task to manage the sensors sampling display and control
 * The sampling task reads FIFO, send values through UART and display values.
 * Also, it updates the sampling rate following the Quadrature Encoder states.
 */
void app_sampling_task(void);

/* ! @} */

#endif /* APP_SAMPLING_H_INCLUDED */
