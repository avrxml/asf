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
 * Copyright (c) 2013-2018 Microchip Technology Inc. and its subsidiaries.
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
