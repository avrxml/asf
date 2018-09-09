/**
 * \file timer_hw.h
 *
 * \brief Handles timer functionalities
 *
 * Copyright (c) 2017-2018 Microchip Technology Inc. and its subsidiaries.
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

#ifndef __TIMER_HW_H__
#define __TIMER_HW_H__

#define STACK_HW_TIMERS		2

typedef void (*hw_timer_callback_t)(void);
typedef void (*platform_hw_timer_callback_t)(void *);

void hw_timer_init(void);
void hw_timer_register_callback(hw_timer_callback_t cb_ptr);
void hw_timer_start(uint32_t timer_val_s);
void hw_timer_start_ms(uint32_t timer_val_ms);
void hw_timer_stop(void);

void *platform_create_hw_timer(platform_hw_timer_callback_t bus_tc_cb_ptr);
void platform_start_bus_timer(void *timer_handle, uint32_t ms);
void platform_delete_bus_timer(void *timer_handle);
void platform_stop_bus_timer(void *timer_handle);
void platform_stop_stack_timers(void);

#ifdef BTLC_REINIT_SUPPORT
void platform_reset_hw_timer(void);
#endif

#endif /* __TIMER_HW_H__ */
