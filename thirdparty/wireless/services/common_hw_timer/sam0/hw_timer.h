/**
 * @file hw_timer.h
 *
 * @brief
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
 */

/* Prevent double inclusion */
#ifndef HW_TIMER_H
#define HW_TIMER_H

typedef void (*tmr_callback_t)(void);

uint16_t tmr_read_count(void);
void tmr_disable_cc_interrupt(void);
void tmr_enable_cc_interrupt(void);
void tmr_disable_ovf_interrupt(void);
void tmr_enable_ovf_interrupt(void);
void tmr_stop(void);
void tmr_write_cmpreg(uint16_t compare_value);
uint8_t save_cpu_interrupt(void);
void restore_cpu_interrupt(uint8_t flags);
uint8_t tmr_init(void);

/* void tmr_write_cmpreg(uint16_t compare_value); */

#endif /* HW_TIMER_H */
/* EOF */
