/*****************************************************************************
 *
 * \file
 *
 * \brief AT42QT1060 driver for AVR32 UC3.
 *
 * This file is the AT42QT1060 driver.
 *
 * Copyright (c) 2009-2018 Microchip Technology Inc. and its subsidiaries.
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
 *****************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

//_____  I N C L U D E S ___________________________________________________
#include "board.h"
#include "compiler.h"
#include "gpio.h"
#include "conf_at42qt1060.h"
#include "at42qt1060.h"
#include "cycle_counter.h"
#if (UC3A3 || UC3A4 || UC3C || UC3L)
#include "twim.h"
#else
#include "twi.h"
#endif
/*! \brief Local driver data.
 */
static struct {
  void (*touch_detect_callback)(void);
} at42qt1060 =
{
  .touch_detect_callback = NULL
};

static uint32_t cpu_hz;

/*! \brief Write device register content.
 *
 * \param reg_index Register address. Use macros as defined in the header file.
 * \param data Data that should be written to the device register.
 */
void at42qt1060_write_reg(uint8_t reg_index, uint8_t data)
{
uint8_t pack[2];
twi_package_t twi_package;

	pack[0] = reg_index;
	pack[1] = data;

	twi_package.chip = AT42QT1060_TWI_ADDRESS;
	twi_package.addr_length = 0;
	twi_package.buffer = &pack;
	twi_package.length = sizeof(pack);

	while(twi_master_write(AT42QT1060_TWI, &twi_package)!=TWI_SUCCESS);

	return;
}

/*! \brief Read device register content.
 *
 * \param reg_index Register address.
 * \returns Register content.
 */
uint8_t at42qt1060_read_reg(uint8_t reg_index)
{
uint8_t data;
twi_package_t twi_package;

	twi_package.chip = AT42QT1060_TWI_ADDRESS;
	twi_package.addr_length = 0;
	twi_package.buffer = &reg_index;
	twi_package.length = 1;
	while(twi_master_write(AT42QT1060_TWI, &twi_package)!=TWI_SUCCESS);
	/* We need a delay here to make this work although this is not
	* specified in the datasheet.
	* Also there seems to be a bug in the TWI module or the driver
	* since some delay here (code or real delay) adds about 500us
	* between the write and the next read cycle.
	*/
	cpu_delay_us(20, cpu_hz);

	twi_package.chip = AT42QT1060_TWI_ADDRESS;
	twi_package.addr_length = 0;
	twi_package.buffer = &data;
	twi_package.length = 1;
	while(twi_master_read(AT42QT1060_TWI, &twi_package)!=TWI_SUCCESS);

	return data;
}

uint8_t at42qt1060_get_status(void)
{
twi_package_t twi_package;
uint16_t status_data;

	twi_package.chip = AT42QT1060_TWI_ADDRESS;
	twi_package.addr_length = 0;
	twi_package.buffer = &status_data;
	twi_package.length = 2;
	twi_master_read(AT42QT1060_TWI, &twi_package);

	return MSB(status_data);
}

uint8_t at42qt1060_get_detect_status(void)
{
uint8_t status;
	/* We need to read both status registers to reset the CHG line */
	status = at42qt1060_read_reg(AT42QT1060_DETECTION_STATUS);
	at42qt1060_read_reg(AT42QT1060_INPUT_PORT_STATUS);
	return status;
}

/** \brief Interrupt handler for the pin interrupt-
 */
ISR(at42qt1060_detect_int_handler,AVR32_GPIO_IRQ_GROUP,0)
{
	if(gpio_get_pin_interrupt_flag(AT42QT1060_DETECT_PIN)) {
		gpio_clear_pin_interrupt_flag(AT42QT1060_DETECT_PIN);
		if(at42qt1060.touch_detect_callback)
			at42qt1060.touch_detect_callback();
	}
}

/** \brief Register a normal pin interrupt for the touch event.
 *
 */
void at42qt1060_register_int(void (*touch_detect_callback)(void))
{
	at42qt1060.touch_detect_callback = touch_detect_callback;

	// register Register at42qt1060_detect_int_handler interrupt on level 0
	irqflags_t flags = cpu_irq_save();
	irq_register_handler(at42qt1060_detect_int_handler,
			AVR32_GPIO_IRQ_0 + AT42QT1060_DETECT_PIN/8, 0);

	// For now we only react on falling edge
	// Actually this is a level interrupt (low active)
	gpio_enable_pin_interrupt(AT42QT1060_DETECT_PIN, GPIO_FALLING_EDGE);
	gpio_clear_pin_interrupt_flag(AT42QT1060_DETECT_PIN);

	cpu_irq_restore(flags);

	return;
}

void at42qt1060_init(int32_t fcpu)
{
	volatile uint8_t tmp1, tmp2, tmp3;

	/* Store cpu frequency locally*/
	cpu_hz = fcpu;

	/* set I/O pins as outputs in order to not let them float
	 * This will trigger a change on the detect line although not
	 * documented in datasheet
	 */
  	at42qt1060_write_reg(AT42QT1060_IO_MASK, 0xFF);

	/* Set keys that will trigger a change on the detect line
	 */
	at42qt1060_write_reg(AT42QT1060_KEY_MASK, AT42QT1060_KEY_MASK_VALUE);

  	at42qt1060_write_reg(AT42QT1060_DI, AT42QT1060_DETECT_INTEGRATOR_VALUE);
	// Set detect thresholds
	at42qt1060_write_reg(AT42QT1060_KEY_0_NTHR,
		AT42QT1060_KEY_0_NTHR_VALUE);
	at42qt1060_write_reg(AT42QT1060_KEY_1_NTHR,
		AT42QT1060_KEY_1_NTHR_VALUE);
	at42qt1060_write_reg(AT42QT1060_KEY_2_NTHR,
		AT42QT1060_KEY_2_NTHR_VALUE);
	at42qt1060_write_reg(AT42QT1060_KEY_3_NTHR,
		AT42QT1060_KEY_3_NTHR_VALUE);
	at42qt1060_write_reg(AT42QT1060_KEY_4_NTHR,
		AT42QT1060_KEY_4_NTHR_VALUE);
	at42qt1060_write_reg(AT42QT1060_KEY_5_NTHR,
		AT42QT1060_KEY_5_NTHR_VALUE);

	tmp1 = at42qt1060_read_reg(AT42QT1060_IO_MASK);
	tmp2 = at42qt1060_read_reg(AT42QT1060_KEY_MASK);
	tmp3 = at42qt1060_read_reg(AT42QT1060_LP_MODE);

	/* Read out touch status to reset detect line */
	tmp1 = at42qt1060_get_detect_status();
}

