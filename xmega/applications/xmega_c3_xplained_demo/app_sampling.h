/**
 * \file
 *
 * \brief Task to manage the sensor sampling
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
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */
#ifndef APP_SAMPLING_H_INCLUDED
#define APP_SAMPLING_H_INCLUDED

/**
 * \ingroup demo_xmegac3_xplained_group
 * \defgroup sampling_task_group Sampling task
 *
 * The schedule and the ADC conversions are done only by interrupt
 * (RTC and ADC) to guarantee a constant acquisition. The FIFO service
 * is used to save value, thus the sampling task can read FIFO, send values
 * through USB, writes values on microSD and display values without timing
 * constraint. The implementation is available in app_sampling.c/.h files.
 *
 * @{
 */

/**
 * \brief Initialize ADC channels, RTC and start RTC alarm
 */
void app_sampling_init(void);

/**
 * \brief Task to manage the sensors sampling display and control
 * The sampling task reads FIFO, send values through USB, writes values
 * on microSD and display values. Also, it updates the sampling rate
 * following the QTouch button states.
 */
void app_sampling_task(void);

/* ! @} */

#endif /* APP_SAMPLING_H_INCLUDED */
