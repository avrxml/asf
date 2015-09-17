/*****************************************************************************
 *
 * \file
 *
 * \brief LCD ET024006dhu display
 *
 * Copyright (c) 2009-2015 Atmel Corporation. All rights reserved.
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

#include "board.h"
#include "et024006dhu.h"
#include "pwm.h"
#include "display.h"

#include "FreeRTOS.h"
#include "task.h"

avr32_pwm_channel_t pwm_channel6 = {
/*
  .cmr = ((PWM_MODE_LEFT_ALIGNED << AVR32_PWM_CMR_CALG_OFFSET)
    | (PWM_POLARITY_HIGH << AVR32_PWM_CMR_CPOL_OFFSET)
    | (PWM_UPDATE_DUTY << AVR32_PWM_CMR_CPD_OFFSET)
    | AVR32_PWM_CMR_CPRE_MCK_DIV_2),
    */
  //.cdty = 0,
  .cdty = 0,
  .cprd = 100
};

static void tft_bl_init(void)
{

  pwm_opt_t opt = {
    .diva = 0,
    .divb = 0,
    .prea = 0,
    .preb = 0
  };
  /* MCK = OSC0 = 12MHz
   * Desired output 60kHz
   * Choosen MCK_DIV_2
   * CPRD = 12MHz / (60kHz * 2) = 100
   *
   * The duty cycle is 100% (CPRD = CDTY)
   * */
  pwm_init(&opt);
  pwm_channel6.CMR.calg = PWM_MODE_LEFT_ALIGNED;
  pwm_channel6.CMR.cpol = PWM_POLARITY_HIGH; //PWM_POLARITY_LOW;//PWM_POLARITY_HIGH;
  pwm_channel6.CMR.cpd = PWM_UPDATE_DUTY;
  pwm_channel6.CMR.cpre = AVR32_PWM_CMR_CPRE_MCK_DIV_2;

  pwm_channel_init(6, &pwm_channel6);
  pwm_start_channels(AVR32_PWM_ENA_CHID6_MASK);

}

/*!
 * \brief main function : do init and loop (poll if configured so)
 */
void display_init(U32 fcpu_hz)
{

  et024006_Init( fcpu_hz, fcpu_hz );

  /* PWM is fed by PBA bus clock which is by default the same
   * as the CPU speed. We set a 0 duty cycle and thus keep the
   * display black*/
  tft_bl_init();

  // Clear the display i.e. make it black
  et024006_DrawFilledRect(0 , 0, ET024006_WIDTH, ET024006_HEIGHT, BLUE );


  /* Lets do a nice fade in by increasing the duty cycle */
  while(pwm_channel6.cdty < pwm_channel6.cprd)
  {
    pwm_channel6.cdty++;
    pwm_channel6.cupd = pwm_channel6.cdty;
    //pwm_channel6.cdty--;
    pwm_async_update_channel(AVR32_PWM_ENA_CHID6, &pwm_channel6);
    vTaskDelay(10);
  }

  // Display default message.

  display_print("ATMEL");
  display_print("lwIP with DHCP demo");
}

void display_print(const char *string)
{
    static U8 current_line = 0;

    et024006_PrintString(( char *)string, (const unsigned char*) &FONT8x16,
                         16, 16 * current_line, 0XFFE0, BLUE);

    current_line++;
    current_line = current_line % 14;
    et024006_DrawFilledRect(16, (current_line) * 16, 320, 16, BLUE);
}
