/**
 * @file epd_hw_timer.h
 *
 * @brief
 *
 *  Copyright (c) 2015 Atmel Corporation. All rights reserved.
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
 */

#ifndef EPD_HW_TIMER_H
#define EPD_HW_TIMER_H

#include <stdint.h>

/** TC  Configuration structure. */
struct tc_control_reg {
	/** TC Compare Output Mode  */
	uint8_t co_mode;
	/** TC Waveform Generation Mode */
	uint8_t wg_mode;	
	/** TC Clock Selection, Prescalar select */
	uint8_t cs_select;
};

/** Timer Modes of operation */
enum tc_co_mode_enum {
	CO_NORMAL		= 0X00,
	CO_TOGGLE		= 0X01,
	CO_CLEAR		= 0X02,
	CO_SET			= 0X03,
};

typedef enum tc_cc_channel_t TC_CC_CHANNEL_t;
typedef enum tc_co_mode_enum TC_CO_MODE_t;
typedef enum TC_MODE_enum TC_WG_MODE_t;
typedef enum TC_CLKSEL_enum TC_CLKSEL_t;

void tc_disable_compare_int(volatile void *p_tc, enum tc_cc_channel_t channel_index);
void tc_disable_overflow_int(volatile void *p_tc, enum tc_cc_channel_t channel_index);
void tc_initc(volatile void *p_tc, TC_CC_CHANNEL_t channel_index, struct tc_control_reg *p_control_reg);
void tc_start(volatile void *p_tc, struct tc_control_reg *p_control_reg);
void tc_stop(volatile void *p_tc);

/**
 * @brief Sets a timer in a particular mode of operation
 * @param tc Timer Address
 * @param mode Enum value of the selected mode
 */
static inline void tc_set_co_mode(volatile void *tc, TC_CC_CHANNEL_t channel_index, TC_CO_MODE_t mode)
{
	uint8_t *reg = (uint8_t *)tc;
	switch (mode) {
		case CO_NORMAL:
			if (channel_index == TC_COMPA) {
				*(reg) &= (~(0x03 << COM1A0));
				*(reg) |=  (((0 << COM1A1)) | (0 << COM1A0));
			}else if (channel_index == TC_COMPB) {
				*(reg) &= (~(0x03 << COM1B0));
				*(reg) |=  (((0 << COM1B1)) | (0 << COM1B0));
			}else if (channel_index == TC_COMPC) {
				*(reg) &= (~(0x03 << COM1C0));
				*(reg) |=  (((0 << COM1C1)) | (0 << COM1C0));
			}else{}
		break;
		case CO_TOGGLE:
			if (channel_index == TC_COMPA) {
				*(reg) &= (~(0x03 << COM1A0));
				*(reg) |=  (((0 << COM1A1)) | (1 << COM1A0));
			}else if (channel_index == TC_COMPB) {
				*(reg) &= (~(0x03 << COM1B0));
				*(reg) |=  (((0 << COM1B1)) | (1 << COM1B0));
			}else if (channel_index == TC_COMPC) {
				*(reg) &= (~(0x03 << COM1C0));
				*(reg) |=  (((0 << COM1C1)) | (1 << COM1C0));
			}else{}
				break;
		case CO_CLEAR:
			if (channel_index == TC_COMPA) {
				*(reg) &= (~(0x03 << COM1A0));
				*(reg) |=  (((1 << COM1A1)) | (0 << COM1A0));
			}else if (channel_index == TC_COMPB) {
				*(reg) &= (~(0x03 << COM1B0));
				*(reg) |=  (((1 << COM1B1)) | (0 << COM1B0));
			}else if (channel_index == TC_COMPC) {
				*(reg) &= (~(0x03 << COM1C0));
				*(reg) |=  (((1 << COM1C1)) | (0 << COM1C0));
			}else{}
			break;
		case CO_SET:
			if (channel_index == TC_COMPA) {
				*(reg) &= (~(0x03 << COM1A0));
				*(reg) |=  (((1 << COM1A1)) | (1 << COM1A0));
			}else if (channel_index == TC_COMPB) {
				*(reg) &= (~(0x03 << COM1B0));
				*(reg) |=  (((1 << COM1B1)) | (1 << COM1B1));
			}else if (channel_index == TC_COMPC) {
				*(reg) &= (~(0x03 << COM1C0));
				*(reg) |=  (((1 << COM1C1)) | (1 << COM1C0));
			}else{}
			break;
		default:
			break;
	}
	
}

/**
 * @brief Sets a timer in a particular mode of operation
 * @param tc Timer Address
 * @param mode Enum value of the selected mode
 */
static inline void tc_set_wg_mode(volatile void *tc, TC_CC_CHANNEL_t channel_index, TC_WG_MODE_t mode)
{
	uint8_t *reg = (uint8_t *)tc;
	switch (mode) {
		case CO_NORMAL:
			*(reg				) &= (~(0x03 << WGM10));
			*(reg				) |= (((0 << WGM11)) | (0 << WGM10));
			*(reg + TCCRB_OFFSET) &= (~(0x03 << WGM12));
			*(reg + TCCRB_OFFSET) |= (((0 << WGM13)) | (0 << WGM12));
			break;
		case PWM_Mode1:
			*(reg				) &= (~(0x03 << WGM10));
			*(reg				) |= (((0 << WGM11)) | (1 << WGM10));
			*(reg + TCCRB_OFFSET) &= (~(0x03 << WGM12));
			*(reg + TCCRB_OFFSET) |= (((0 << WGM13)) | (0 << WGM12));
			break;
		case PWM_Mode2:
			*(reg				) &= (~(0x03 << WGM10));
			*(reg				) |= (((1 << WGM11)) | (0 << WGM10));
			*(reg + TCCRB_OFFSET) &= (~(0x03 << WGM12));
			*(reg + TCCRB_OFFSET) |= (((0 << WGM13)) | (0 << WGM12));
			break;
		case PWM_Mode3:
			*(reg				) &= (~(0x03 << WGM10));
			*(reg				) |= (((1 << WGM11)) | (1 << WGM10));
			*(reg + TCCRB_OFFSET) &= (~(0x03 << WGM12));
			*(reg + TCCRB_OFFSET) |= (((0 << WGM13)) | (0 << WGM12));
			break;
		case CTC_Mode1:
			*(reg				) &= (~(0x03 << WGM10));
			*(reg				) |= (((0 << WGM11)) | (0 << WGM10));
			*(reg + TCCRB_OFFSET) &= (~(0x03 << WGM12));
			*(reg + TCCRB_OFFSET) |= (((0 << WGM13)) | (1 << WGM12));
			break;
		case PWM_Mode4:
			*(reg				) &= (~(0x03 << WGM10));
			*(reg				) |= (((0 << WGM11)) | (1 << WGM10));
			*(reg + TCCRB_OFFSET) &= (~(0x03 << WGM12));
			*(reg + TCCRB_OFFSET) |= (((0 << WGM13)) | (1 << WGM12));
			break;
		case PWM_Mode5:
			*(reg				) &= (~(0x03 << WGM10));
			*(reg				) |= (((1 << WGM11)) | (0 << WGM10));
			*(reg + TCCRB_OFFSET) &= (~(0x03 << WGM12));
			*(reg + TCCRB_OFFSET) |= (((0 << WGM13)) | (1 << WGM12));
			break;
		case PWM_Mode6:
			*(reg				) &= (~(0x03 << WGM10));
			*(reg				) |= (((1 << WGM11)) | (1 << WGM10));
			*(reg + TCCRB_OFFSET) &= (~(0x03 << WGM12));
			*(reg + TCCRB_OFFSET) |= (((0 << WGM13)) | (1 << WGM12));
			break;
		case PWM_Mode7:
			*(reg				) &= (~(0x03 << WGM10));
			*(reg				) |= (((0 << WGM11)) | (0 << WGM10));
			*(reg + TCCRB_OFFSET) &= (~(0x03 << WGM12));
			*(reg + TCCRB_OFFSET) |= (((1 << WGM13)) | (0 << WGM12));
			break;
		case PWM_Mode8:
			*(reg				) &= (~(0x03 << WGM10));
			*(reg				) |= (((0 << WGM11)) | (1 << WGM10));
			*(reg + TCCRB_OFFSET) &= (~(0x03 << WGM12));
			*(reg + TCCRB_OFFSET) |= (((1 << WGM13)) | (0 << WGM12));
			break;
		case PWM_Mode9:
			*(reg				) &= (~(0x03 << WGM10));
			*(reg				) |= (((1 << WGM11)) | (0 << WGM10));
			*(reg + TCCRB_OFFSET) &= (~(0x03 << WGM12));
			*(reg + TCCRB_OFFSET) |= (((1 << WGM13)) | (0 << WGM12));
			break;
		case PWM_Mode10:
			*(reg				) &= (~(0x03 << WGM10));
			*(reg				) |= (((1 << WGM11)) | (1 << WGM10));
			*(reg + TCCRB_OFFSET) &= (~(0x03 << WGM12));
			*(reg + TCCRB_OFFSET) |= (((1 << WGM13)) | (0 << WGM12));
			break;
		case CTC_Mode2:
			*(reg				) &= (~(0x03 << WGM10));
			*(reg				) |= (((0 << WGM11)) | (0 << WGM10));
			*(reg + TCCRB_OFFSET) &= (~(0x03 << WGM12));
			*(reg + TCCRB_OFFSET) |= (((1 << WGM13)) | (1 << WGM12));
			break;
		case PWM_Mode11:
			*(reg				) &= (~(0x03 << WGM10));
			*(reg				) |= (((1 << WGM11)) | (0 << WGM10));
			*(reg + TCCRB_OFFSET) &= (~(0x03 << WGM12));
			*(reg + TCCRB_OFFSET) |= (((1 << WGM13)) | (1 << WGM12));
			break;
		case PWM_Mode12:
			*(reg				) &= (~(0x03 << WGM10));
			*(reg				) |= (((1 << WGM11)) | (1 << WGM10));
			*(reg + TCCRB_OFFSET) &= (~(0x03 << WGM12));
			*(reg + TCCRB_OFFSET) |= (((1 << WGM13)) | (1 << WGM12));
			break;
		default:
			break;	
	}
	
}

/**
 * @brief Sets a timer in a particular mode of operation
 * @param tc Timer Address
 * @param mode Enum value of the selected mode
 */
static inline void tc_set_clk_source(volatile void *tc, TC_CC_CHANNEL_t channel_index, TC_CLKSEL_t mode)
{
	uint8_t *reg = (uint8_t *)tc;
	
	*(reg + TCCRB_OFFSET) &= (~(0x07 << CS10));
	*(reg + TCCRB_OFFSET) |= (mode);
	
}

/**
 * @brief Write Compare register with timer Value
 * @param tc Timer Address
 * @param channel_index Compare Channel to be used
 * @param value Compare value to be written
 */
 static inline void tc_write_ic(volatile void *tc,
 enum tc_cc_channel_t channel_index, uint16_t value)
 {
	if ((uintptr_t)tc == (uintptr_t)&TCCR1A) {
	 ICR1 = value;
	 } else if ((uintptr_t)tc == (uintptr_t)&TCCR3A) {
	 ICR3 = value;
	 } else if ((uintptr_t)tc == (uintptr_t)&TCCR4A) {
	 ICR4 = value;
	 } else if ((uintptr_t)tc == (uintptr_t)&TCCR5A) {
	 ICR5 = value;
	 } else {}
 }


/**
 * @brief Write Compare register with timer Value
 * @param tc Timer Address
 * @param channel_index Compare Channel to be used
 * @param value Compare value to be written
 */
 static inline void tc_clear_ic(volatile void *tc,
 enum tc_cc_channel_t channel_index, uint16_t value)
 {
	value=0;
	if ((uintptr_t)tc == (uintptr_t)&TCCR1A) {
	 ICR1 = value;
	 } else if ((uintptr_t)tc == (uintptr_t)&TCCR3A) {
	 ICR3 = value;
	 } else if ((uintptr_t)tc == (uintptr_t)&TCCR4A) {
	 ICR4 = value;
	 } else if ((uintptr_t)tc == (uintptr_t)&TCCR5A) {
	 ICR5 = value;
	 } else {}
 }
 
 /**
 * @brief Write Compare register with timer Value
 * @param tc Timer Address
 * @param channel_index Compare Channel to be used
 * @param value Compare value to be written
 */
static inline void tc_clear_cc(volatile void *tc,
		enum tc_cc_channel_t channel_index, uint16_t value)
{
	value=0;
	uint8_t *reg = (uint8_t *)tc;
	*(reg + channel_index + 1) =  (value >> 8);
	*(reg + channel_index) =  value;
}

#endif /* EPD_HW_TIMER_H */
/** EOF */


