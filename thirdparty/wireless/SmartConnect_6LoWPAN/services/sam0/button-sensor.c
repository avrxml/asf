/*
 * Copyright (c) 2013, Thingsquare, http://www.thingsquare.com/.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "contiki.h"
#include "button-sensor.h"
#include "sys/timer.h"
#include "asf.h"
#include "extint.h"
//#include "samr21_xplained_pro.h"
// #include ""

static struct timer debouncetimer;
#define DEBOUNCE_TIME       (CLOCK_SECOND/8)
/*---------------------------------------------------------------------------*/
static void
extint_detection_callback(void)
{
  bool pin_state = port_pin_get_input_level(BUTTON_0_PIN);
  pin_state = pin_state;

  if(!timer_expired(&debouncetimer)) {
    return;
  }

  sensors_changed(&button_sensor);
  timer_set(&debouncetimer, DEBOUNCE_TIME);
}
/*---------------------------------------------------------------------------*/
static int
button_sensor_value(int type)
{
  bool PinLvl = port_pin_get_input_level(BUTTON_0_PIN);
  return PinLvl;
}
/*---------------------------------------------------------------------------*/
static int enabled = 0;
static int
button_sensor_configure(int type, int value)
{
  struct extint_chan_conf config_extint_chan;
  extint_chan_get_config_defaults(&config_extint_chan);
  config_extint_chan.gpio_pin           = BUTTON_0_EIC_PIN;
  config_extint_chan.gpio_pin_mux       = BUTTON_0_EIC_MUX;
  config_extint_chan.gpio_pin_pull      = EXTINT_PULL_UP;

  if(value == 1) {
    /* Enable the sensor */
    config_extint_chan.detection_criteria = EXTINT_DETECT_FALLING;
    //config_extint_chan.detection_criteria = EXTINT_DETECT_BOTH;
    enabled = 1;
  } else {
    /* Disable the sensor */
    config_extint_chan.detection_criteria = EXTINT_DETECT_NONE;
    enabled = 0;
  }
  extint_chan_set_config(BUTTON_0_EIC_LINE, &config_extint_chan);

  return 0;
}
/*---------------------------------------------------------------------------*/
void
button_sensor_init(void)
{
  struct extint_chan_conf config_extint_chan;
  struct port_config pin_conf;
  port_get_config_defaults(&pin_conf);
  pin_conf.direction  = PORT_PIN_DIR_INPUT;
  pin_conf.input_pull = PORT_PIN_PULL_UP;
  port_pin_set_config(BUTTON_0_PIN, &pin_conf);

  //  extint_enable();
  extint_chan_get_config_defaults(&config_extint_chan);
  config_extint_chan.gpio_pin           = BUTTON_0_EIC_PIN;
  config_extint_chan.gpio_pin_mux       = BUTTON_0_EIC_MUX;
  config_extint_chan.gpio_pin_pull      = EXTINT_PULL_UP;
  config_extint_chan.detection_criteria = EXTINT_DETECT_FALLING;
  // config_extint_chan.detection_criteria = EXTINT_DETECT_BOTH;
  extint_chan_set_config(BUTTON_0_EIC_LINE, &config_extint_chan);

  extint_register_callback((extint_callback_t)extint_detection_callback,
                           BUTTON_0_EIC_LINE,
                           EXTINT_CALLBACK_TYPE_DETECT);
  extint_chan_enable_callback(BUTTON_0_EIC_LINE, EXTINT_CALLBACK_TYPE_DETECT);

  timer_set(&debouncetimer, 0);
  system_interrupt_enable_global();

  /* on by default */
  button_sensor_configure(0, 1);

}
/*---------------------------------------------------------------------------*/
static int
button_sensor_status(int type)
{
  return enabled;
}
/*---------------------------------------------------------------------------*/
SENSORS_SENSOR(button_sensor, "Button sensor",
               button_sensor_value, button_sensor_configure, button_sensor_status);
/*---------------------------------------------------------------------------*/
