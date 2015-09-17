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

#include "contiki.h"
#include "dev/leds.h"
#include "platform-conf.h"

#include "board.h"
#include "conf_board.h"
#include "port.h"
//#include "samr21_xplained_pro.h"
/*---------------------------------------------------------------------------*/
void
leds_arch_init(void)
{
  /* Configure pins as output, turn LED off */
  struct port_config pin_conf;
  port_get_config_defaults(&pin_conf);
  pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
  port_pin_set_config(LED_0_PIN, &pin_conf);
  port_pin_set_output_level(LED_0_PIN, LED_0_INACTIVE);
}
/*---------------------------------------------------------------------------*/
unsigned char
leds_arch_get(void)
{
  if(port_pin_get_output_level(LED_0_PIN) == 0) {
    /* The LED is active low. */
    return (unsigned char) LEDS_YELLOW;
  }
  return 0;
}
/*---------------------------------------------------------------------------*/
void
leds_arch_set(unsigned char leds)
{
  if((leds & (LEDS_ALL)) != 0) {
    /* Turn on (LED on board is active low) */
    port_pin_set_output_level(LED_0_PIN, 0);
  } else {
    /* Turn off */
    port_pin_set_output_level(LED_0_PIN, 1);
  }
}
/*---------------------------------------------------------------------------*/
