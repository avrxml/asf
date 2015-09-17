/**************************************************************************
 *
 * \file
 *
 * \brief Controller for AT42QT1060 touch sensor.
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
 ***************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include "board.h"
#include "gpio.h"
#include "controller.h"
#include "twim.h"
#include "at42qt1060.h"
#include "conf_at42qt1060.h"

#include "cycle_counter.h"


#define JOYSTICK_KEY_DEBOUNCE_MS    200

volatile bool touch_detect = false;
static U32 static_fcpu_hz;
t_cpu_time joystick_key_sensibility_timer;
struct at42qt1060_data {
  uint8_t detect_status;
  uint16_t key_signal[6];
  uint8_t key_threshold[6];
  uint16_t key_ref_value[6];
};

#include <includes.h>
extern CPU_INT32U  BSP_INTC_IntReg (CPU_FNCT_VOID handler, CPU_INT32U irq, CPU_INT32U int_level);

static void twi_init(U32 fpba_hz)
{
  const gpio_map_t AT42QT1060_TWI_GPIO_MAP =
  {
  {AT42QT1060_TWI_SCL_PIN, AT42QT1060_TWI_SCL_FUNCTION},
  {AT42QT1060_TWI_SDA_PIN, AT42QT1060_TWI_SDA_FUNCTION}
  };

  const twi_options_t AT42QT1060_TWI_OPTIONS =
  {
    .pba_hz = 24000000,
    .speed = AT42QT1060_TWI_MASTER_SPEED,
    .chip = AT42QT1060_TWI_ADDRESS
  };

  // Assign I/Os to SPI.
  gpio_enable_module(AT42QT1060_TWI_GPIO_MAP,
    sizeof(AT42QT1060_TWI_GPIO_MAP) / sizeof(AT42QT1060_TWI_GPIO_MAP[0]));
  // Initialize as master.
  twi_master_init(AT42QT1060_TWI, &AT42QT1060_TWI_OPTIONS);

}

/*! \brief Callback function for a detect event of the touch sensor device.
 */
void touch_detect_callback(void)
{
  touch_detect = true;
}

struct at42qt1060_data touch_data;

void controller_task(void)
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
}

static void controller_detect_int_handler(void)
{
  if(gpio_get_pin_interrupt_flag(AT42QT1060_DETECT_PIN))
  {
    gpio_clear_pin_interrupt_flag(AT42QT1060_DETECT_PIN);
    touch_detect_callback();
  }
}

void controller_init(U32 fcpu_hz, U32 fhsb_hz, U32 fpbb_hz, U32 fpba_hz)
{
  // Disable all interrupts
  Disable_global_interrupt();

  twi_init(fpba_hz);
  // wait until the device settles its CHG line
  cpu_delay_ms(230, fcpu_hz);
  at42qt1060_init(fcpu_hz);
  BSP_INTC_IntReg(&controller_detect_int_handler, AVR32_GPIO_IRQ_0 + AT42QT1060_DETECT_PIN/8, AVR32_INTC_INT1);
  // For now we only react on falling edge
  // Actually this is a level interrupt (low active)
  gpio_enable_pin_interrupt(AT42QT1060_DETECT_PIN, GPIO_FALLING_EDGE);
  gpio_clear_pin_interrupt_flag(AT42QT1060_DETECT_PIN);
  //static_fcpu_hz = fcpu_hz;
  cpu_set_timeout(cpu_ms_2_cy(JOYSTICK_KEY_DEBOUNCE_MS, static_fcpu_hz),
  	&joystick_key_sensibility_timer);

  // Enable global interrupts
  Enable_global_interrupt();

}

// FCT3/ Up
bool controller_inquiry(void)
{
  if (touch_data.detect_status & (1 << 0))
  {
    return true;
  }
  return false;
}

// FCT1/ Right button
bool controller_select(void)
{
  if (touch_data.detect_status & (1 << 2))
  {
    return true;
  }
  return false;
}

// FCT2/ Left
bool controller_connect(void)
{
  if (touch_data.detect_status & (1 << 3))
  {
    return true;
  }
  return false;
}

// FCT5/ Middle
bool controller_send(void)
{
  if (touch_data.detect_status & (1 << 4))
  {
    return true;
  }
  return false;
}
