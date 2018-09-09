/**
 * @file epd_hw_timer.h
 *
 * @brief
 *
 * Copyright (c) 2015-2018 Microchip Technology Inc. and its subsidiaries.
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


