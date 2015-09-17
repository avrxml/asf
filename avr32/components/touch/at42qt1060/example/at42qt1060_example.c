/*****************************************************************************
 *
 * \file
 *
 * \brief Example for AT42QT1060 touch device.
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

/*! \mainpage
 * \section intro Introduction
 * This is the documentation for the AT42QT1060 driver.
 *
 * This example demonstrates how to use the AT42QT1060 interface.
 *
 * \section files Main Files
 * - at42qt1060_example.c: the AT42QT1060 example
 * - at42qt1060.h: the driver interface for the AT42QT1060
 *
 * \section compilinfo Compilation Information
 * This software is written for GNU GCC for AVR32 and for IAR Embedded Workbench
 * for Atmel AVR32. Other compilers may or may not work.
 *
 * \section deviceinfo Device Information
 * All AVR32 devices with a TWI or TWIM can be used.
 *
 * \section configinfo Configuration Information
 * This example has been tested with the following configuration:
 * - EVK1105 or UC3C_EK
 * - CPU clock: 12 MHz;
 * - USART abstracted with a USB CDC connection to a PC; port via a standard RS232 DB9 cable;
 * - PC terminal settings:
 *   - 57600 bps,
 *   - 8 data bits,
 *   - no parity bit,
 *   - 1 stop bit,
 *   - no flow control.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/products/AVR32/">Atmel AVR32</A>.\n
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

// Include Files
#include <avr32/io.h>
#include "compiler.h"
#include "print_funcs.h"
#include "board.h"
#include "power_clocks_lib.h"

#if UC3C // UC3C uses the TWIM module
#include "twim.h"
#else // default use the TWI module
#include "eic.h"
#include "twi.h"
#endif

#include "gpio.h"
#include "cycle_counter.h"
#include "flashc.h"
#include "at42qt1060.h"
#include "conf_at42qt1060.h"
#include <stdio.h>

#if BOARD == UC3C_EK
#define FCPU_HZ		60000000
#define FPBA_HZ		60000000
#else
#define FCPU_HZ		66000000
#define FPBA_HZ		66000000
#endif


volatile bool touch_detect = false;

struct at42qt1060_data {
  uint8_t detect_status;
  uint16_t key_signal[6];
  uint8_t key_threshold[6];
  uint16_t key_ref_value[6];
};
/*
struct at42qt1060_status {
	uint8_t detect_status;
	uint8_t input_port_status;
};
*/


/*! \name System Clock Frequencies
 */
//! @{
static pcl_freq_param_t pcl_freq_param =
{
  .cpu_f        = FCPU_HZ,
  .pba_f        = FPBA_HZ,
  .osc0_f       = FOSC0,
  .osc0_startup = OSC0_STARTUP
};
//! @}

/*! \brief Initializes the MCU system clocks.
*/
static void init_sys_clocks(void)
{
  // Configure system clocks.
  if (pcl_configure_clocks(&pcl_freq_param) != PASS)
    while(1);
}

static void twi_init(void)
{
  const gpio_map_t AT42QT1060_TWI_GPIO_MAP =
  {
  {AT42QT1060_TWI_SCL_PIN, AT42QT1060_TWI_SCL_FUNCTION},
  {AT42QT1060_TWI_SDA_PIN, AT42QT1060_TWI_SDA_FUNCTION}
  };

  const twi_options_t AT42QT1060_TWI_OPTIONS =
  {
    .pba_hz = FPBA_HZ,
    .speed = AT42QT1060_TWI_MASTER_SPEED,
    .chip = AT42QT1060_TWI_ADDRESS
  };

  // Assign I/Os to SPI.
  gpio_enable_module(AT42QT1060_TWI_GPIO_MAP,
    sizeof(AT42QT1060_TWI_GPIO_MAP) / sizeof(AT42QT1060_TWI_GPIO_MAP[0]));
  // Initialize as master.
  twi_master_init(AT42QT1060_TWI, &AT42QT1060_TWI_OPTIONS);

}
void get_key_signal_values(struct at42qt1060_data *touch_data)
{
	LSB(touch_data->key_signal[0]) =
		at42qt1060_read_reg(AT42QT1060_KEY_0_SIGNAL_LSB);
	MSB(touch_data->key_signal[0]) =
		at42qt1060_read_reg(AT42QT1060_KEY_0_SIGNAL_MSB);
	LSB(touch_data->key_signal[1]) =
		at42qt1060_read_reg(AT42QT1060_KEY_1_SIGNAL_LSB);
	MSB(touch_data->key_signal[1]) =
		at42qt1060_read_reg(AT42QT1060_KEY_1_SIGNAL_MSB);
	LSB(touch_data->key_signal[2]) =
		at42qt1060_read_reg(AT42QT1060_KEY_2_SIGNAL_LSB);
	MSB(touch_data->key_signal[2]) =
		at42qt1060_read_reg(AT42QT1060_KEY_2_SIGNAL_MSB);
	LSB(touch_data->key_signal[3]) =
		at42qt1060_read_reg(AT42QT1060_KEY_3_SIGNAL_LSB);
	MSB(touch_data->key_signal[3]) =
		at42qt1060_read_reg(AT42QT1060_KEY_3_SIGNAL_MSB);
	LSB(touch_data->key_signal[4]) =
		at42qt1060_read_reg(AT42QT1060_KEY_4_SIGNAL_LSB);
	MSB(touch_data->key_signal[4]) =
		at42qt1060_read_reg(AT42QT1060_KEY_4_SIGNAL_MSB);
	LSB(touch_data->key_signal[5]) =
		at42qt1060_read_reg(AT42QT1060_KEY_5_SIGNAL_LSB);
	MSB(touch_data->key_signal[5]) =
		at42qt1060_read_reg(AT42QT1060_KEY_5_SIGNAL_MSB);
}

void get_key_ref_values(struct at42qt1060_data *touch_data)
{
	LSB(touch_data->key_ref_value[0]) =
		at42qt1060_read_reg(AT42QT1060_KEY_0_REF_DATA_LSB);
	MSB(touch_data->key_ref_value[0]) =
		at42qt1060_read_reg(AT42QT1060_KEY_0_REF_DATA_MSB);
	LSB(touch_data->key_ref_value[1]) =
		at42qt1060_read_reg(AT42QT1060_KEY_1_REF_DATA_LSB);
	MSB(touch_data->key_ref_value[1]) =
		at42qt1060_read_reg(AT42QT1060_KEY_1_REF_DATA_MSB);
	LSB(touch_data->key_ref_value[2]) =
		at42qt1060_read_reg(AT42QT1060_KEY_2_REF_DATA_LSB);
	MSB(touch_data->key_ref_value[2]) =
		at42qt1060_read_reg(AT42QT1060_KEY_2_REF_DATA_MSB);
	LSB(touch_data->key_ref_value[3]) =
		at42qt1060_read_reg(AT42QT1060_KEY_3_REF_DATA_LSB);
	MSB(touch_data->key_ref_value[3]) =
		at42qt1060_read_reg(AT42QT1060_KEY_3_REF_DATA_MSB);
	LSB(touch_data->key_ref_value[4]) =
		at42qt1060_read_reg(AT42QT1060_KEY_4_REF_DATA_LSB);
	MSB(touch_data->key_ref_value[4]) =
		at42qt1060_read_reg(AT42QT1060_KEY_4_REF_DATA_MSB);
	LSB(touch_data->key_ref_value[5]) =
		at42qt1060_read_reg(AT42QT1060_KEY_5_REF_DATA_LSB);
	MSB(touch_data->key_ref_value[5]) =
		at42qt1060_read_reg(AT42QT1060_KEY_5_REF_DATA_MSB);
}

/**
 * \brief Print touch key status to debug output.
 *
 * \param key Key index for key data in \a touch_data.
 * \param key_name Pointer to key name. Should be less than 7 chars or the
 * table will not be aligned.
 * \param touch_data Pointer to touch data structure.
 */
void print_key_data(int key, const char *key_name,
	struct at42qt1060_data *touch_data)
{
	char tmp[128];
	char detect  = (touch_data->detect_status & (1 << key)) ?
		'T' : '-';

	sprintf(tmp, "| %6s |  %4u  |   %4u    | %4d  |   %c    |\r\n",
		key_name,
		touch_data->key_signal[key],
		touch_data->key_ref_value[key],
		(signed short) touch_data->key_ref_value[key] -
		touch_data->key_signal[key],
		detect
		);
	print_dbg(tmp);
}

/**
 * \brief Print key touch data to debug output.
 *
 * \param touch_data Pointer to structure that holds the touch data.
 */
void print_touch_data(struct at42qt1060_data *touch_data)
{

	print_dbg("\f"); // start with an empty display
	print_dbg("|  Key   | signal | reference | delta | detect |\r\n");
	print_dbg("|----------------------------------------------|\r\n");
#if BOARD == EVK1105
	print_key_data(0, "left", touch_data);
	print_key_data(1, "down", touch_data);
	print_key_data(2, "right", touch_data);
	print_key_data(3, "up", touch_data);
	print_key_data(4, "middle", touch_data);
	print_key_data(5, "home", touch_data);
#elif BOARD == UC3C_EK
	print_key_data(0, "up", touch_data);
	print_key_data(1, "down", touch_data);
	print_key_data(2, "right", touch_data);
	print_key_data(3, "left", touch_data);
	print_key_data(4, "middle", touch_data);
	print_key_data(5, "home", touch_data);
#endif
}

/*! \brief Callback function for a detect event of the touch sensor device.
 */
void touch_detect_callback(void)
{
  touch_detect = true;
  gpio_tgl_gpio_pin(LED3_GPIO);
}

// main function
int main(void) {

  struct at42qt1060_data touch_data;
  t_cpu_time timeout;

  init_sys_clocks();
  init_dbg_rs232(FPBA_HZ);
  print_dbg("AT42QT1060 Touch Example\r\n");

   // Initialize and enable interrupt
  irq_initialize_vectors();
  cpu_irq_enable();

  // Activate LED0 & LED1 & LED2 & LED3 pins in GPIO output mode and switch them off.
  gpio_set_gpio_pin(LED0_GPIO);
  gpio_set_gpio_pin(LED1_GPIO);
  gpio_set_gpio_pin(LED2_GPIO);
  gpio_set_gpio_pin(LED3_GPIO);

  twi_init();

  /* Power up delay: This device needs a maximum of 230ms before it
   * can be accessed. Another method would be to wait until the the
   * detect line goes low but this won't work during a debugging session
   * since the device is not reset.*/
  cpu_delay_ms(230, FCPU_HZ);

  at42qt1060_init(FCPU_HZ);

#if BOARD == EVK1105
  /* Register an interrupt for the detect signal line either as gpio interrupt
   * or as EIC interrupt (if pin is multiplexed on a EIC channel). */
  //at42qt1060_register_int(&touch_detect_callback);
  at42qt1060_register_eic_int(&touch_detect_callback);
#else
  at42qt1060_register_int(&touch_detect_callback);
#endif

  cpu_set_timeout( cpu_ms_2_cy(1000, FCPU_HZ), &timeout);

  while(true)
  {
    // if a touch is detected we read the status
    if(touch_detect)
    {
      touch_data.detect_status =
      	at42qt1060_read_reg(AT42QT1060_DETECTION_STATUS);
      // need to read input port status too to reset CHG line
      at42qt1060_read_reg(AT42QT1060_INPUT_PORT_STATUS);
      touch_detect = false;
    }
    if(cpu_is_timeout(&timeout))
    {
      get_key_signal_values(&touch_data);
      get_key_ref_values(&touch_data);
      print_touch_data(&touch_data);

      cpu_set_timeout(cpu_ms_2_cy(500, FCPU_HZ), &timeout);
    }
  }

  return 0;
}
