/*****************************************************************************
 *
 * \file
 *
 * \brief AT42QT1060 driver for AVR32 UC3.
 *
 * This file is the AT42QT1060 driver.
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
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
 *****************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

//_____  I N C L U D E S ___________________________________________________
#include "board.h"
#include "compiler.h"
#include "gpio.h"
#include "conf_at42qt1060.h"
#include "at42qt1060.h"
#include "intc.h"
#if BOARD == EVK1105
#include "eic.h"
#endif
#include "cycle_counter.h"

#  if (UC3A3 || UC3C || UC3L)
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
#if defined (__GNUC__)
__attribute__ ((__interrupt__))
#elif defined (__ICCAVR32__)
__interrupt
#endif
void at42qt1060_detect_int_handler(void)
{
  if(gpio_get_pin_interrupt_flag(AT42QT1060_DETECT_PIN))
  {
    gpio_clear_pin_interrupt_flag(AT42QT1060_DETECT_PIN);
    if(at42qt1060.touch_detect_callback)
    	at42qt1060.touch_detect_callback();
  }
}

#if BOARD == EVK1105
/** \brief Interrupt handler for the EIC controller line.
 */
#if __GNUC__
__attribute__ ((__interrupt__))
#elif __ICCAVR32__
__interrupt
#endif
void at42qt1060_detect_eic_int_handler(void)
{
   eic_clear_interrupt_line(&AVR32_EIC, AT42QT1060_EIC_LINE);
   if(at42qt1060.touch_detect_callback)
   	at42qt1060.touch_detect_callback();
}
#endif

/** \brief Register a normal pin interrupt for the touch event.
 *
 */
void at42qt1060_register_int(void (*touch_detect_callback)(void))
{
  at42qt1060.touch_detect_callback = touch_detect_callback;

  Disable_global_interrupt();

  INTC_register_interrupt(&at42qt1060_detect_int_handler, AVR32_GPIO_IRQ_0 + AT42QT1060_DETECT_PIN/8, 0 );
  // For now we only react on falling edge
  // Actually this is a level interrupt (low active)
  gpio_enable_pin_interrupt(AT42QT1060_DETECT_PIN, GPIO_FALLING_EDGE);
  gpio_clear_pin_interrupt_flag(AT42QT1060_DETECT_PIN);

  Enable_global_interrupt();
  return;
}

#if BOARD == EVK1105
/** \brief Register an external interrupt handler for the touch event.
 *
 */
void at42qt1060_register_eic_int(void (*touch_detect_callback)(void))
{
  eic_options_t eic_options[1];

  at42qt1060.touch_detect_callback = touch_detect_callback;

  gpio_enable_module_pin(AT42QT1060_DETECT_PIN, AT42QT1060_EIC_EXTINT_FUNCTION);

  Disable_global_interrupt();

  INTC_register_interrupt(&at42qt1060_detect_eic_int_handler, AT42QT1060_EIC_EXTINT_IRQ,
    AT42QT1060_EIC_EXTINT_LEVEL);

  eic_options[0].eic_mode = EIC_MODE_EDGE_TRIGGERED;
  eic_options[0].eic_level = EIC_EDGE_FALLING_EDGE;
  eic_options[0].eic_async = EIC_SYNCH_MODE;
  eic_options[0].eic_line = AT42QT1060_EIC_LINE;

  eic_init(&AVR32_EIC, &eic_options[0], 1);
  eic_enable_lines(&AVR32_EIC, (1 << eic_options[0].eic_line));
  eic_enable_interrupt_lines(&AVR32_EIC, (1 << eic_options[0].eic_line));

  Enable_global_interrupt();

  return;
}
#endif

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

