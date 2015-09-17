/*****************************************************************************
 *
 * \file
 *
 * \brief ET024006DHU TFT display driver example 2.
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
 * This is the documentation for the ET024006DHU TFT display example 2.
 *
 * \section files Main Files
 * - et024006dhu.c , .h: the ET024006DHU driver;
 * - et024006dhu_example2.c: the example 2
 * - smc_et024006dhu.h: SMC configuration
 *
 * \section compilinfo Compilation Information
 * This software is written for GNU GCC for AVR32 and for IAR Embedded Workbench
 * for Atmel AVR32. Other compilers may or may not work.
 *
 * \section deviceinfo Device Information
 * All AVR32 devices with an EBI and SMC module can be used.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/uc3">Atmel AVR UC3</A>.\n
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include "board.h"
#include "gpio.h"
#include "power_clocks_lib.h"
#include "conf_clock.h"
#include "et024006dhu.h"
#include "delay.h"
#include <math.h>

#define WIDTH 320
#define HEIGHT 240

#if BOARD == EVK1105
#include "pwm.h"
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
   * Chosen MCK_DIV_2
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
#endif

pcl_freq_param_t   pcl_freq_param=
{
   .cpu_f  =       FCPU_HZ          // Put here the wanted CPU freq
,  .pba_f    =     FPBA_HZ          // Put here the wanted PBA freq
,  .osc0_f     =   FOSC0            // Oscillator 0 frequency
,  .osc0_startup = OSC0_STARTUP     // Oscillator 0 startup time
};

static void draw_mandel(void);
static void draw_plasma(void);

// Main Function
int main(void)
{
  // Set CPU and PBA clock
  pcl_configure_clocks(&pcl_freq_param);

  gpio_enable_gpio_pin(LED0_GPIO);
  gpio_enable_gpio_pin(LED1_GPIO);
  gpio_enable_gpio_pin(LED2_GPIO);
  gpio_enable_gpio_pin(LED3_GPIO);

  et024006_Init( pcl_freq_param.cpu_f, pcl_freq_param.cpu_f );

#if BOARD == EVK1105
  /* PWM is fed by PBA bus clock which is by default the same
   * as the CPU speed. We set a 0 duty cycle and thus keep the
   * display black*/
  tft_bl_init();
#elif BOARD == EVK1104 || BOARD == UC3C_EK
  gpio_set_gpio_pin(ET024006DHU_BL_PIN);
#endif

  // Clear the display i.e. make it black
  et024006_DrawFilledRect(0 , 0, ET024006_WIDTH, ET024006_HEIGHT, BLACK );

#if BOARD == EVK1105
  /* Lets do a nice fade in by increasing the duty cycle */
  while(pwm_channel6.cdty < pwm_channel6.cprd)
  {
    pwm_channel6.cdty++;
    pwm_channel6.cupd = pwm_channel6.cdty;
    //pwm_channel6.cdty--;
    pwm_async_update_channel(AVR32_PWM_ENA_CHID6, &pwm_channel6);
    delay_ms(10);
  }
#endif
  // Draw Mandelbrot set
  draw_mandel();
  //draw_plasma();
  while(true);

}

static void draw_mandel(void)
{

  int iter =40;
  float xstart = -2.0;
  float xend    = 1.0;

  float ystart = -1.35;
  float yend   = -ystart;

  float xstep;
  float ystep;


  float x,y;

  float z,zi,newz,newzi;

  int colour;

  int i,j,k;
  int inset;

  /* these are used for calculating the points corresponding to the pixels */
  xstep = (xend-xstart)/WIDTH;
  ystep = (yend-ystart)/HEIGHT;

  /*the main loop */
  x = xstart;
  y = ystart;
  for (i=0; i<HEIGHT/2+1; i++)
  {
    for (j=0; j<WIDTH; j++)
    {
      z = 0;
      zi = 0;
      inset = 1;
      for (k=0; k<iter; k++)
      {
        /* z^2 = (a+bi)(a+bi) = a^2 + 2abi - b^2 */
	newz = (z*z)-(zi*zi) + x;
	newzi = 2*z*zi + y;
        z = newz;
        zi = newzi;
	if(((z*z)+(zi*zi)) > 4)
	{
	  inset = 0;
	  colour = k;
	  k = iter;
	}
      }
      if (inset)
      {
       et024006_DrawPixel(j,i,BLACK);
       et024006_DrawPixel(j,HEIGHT-i,BLACK);
      }
      else
      {
        et024006_DrawPixel(j,i,BLUE_LEV((colour*255) / iter )+GREEN_LEV((colour*127) / iter )+RED_LEV((colour*127) / iter ));
        et024006_DrawPixel(j,HEIGHT-i,BLUE_LEV((colour*255) / iter )+GREEN_LEV((colour*127) / iter )+RED_LEV((colour*127) / iter ));
      }
      x += xstep;
    }
    y += ystep;
    x = xstart;
  }

}

unsigned short int plasma[HEIGHT*WIDTH];

static void draw_plasma(void)
{
 unsigned int pos1 = 0, pos2 = 0, pos3 = 0, pos4 = 0, tpos1, tpos2, tpos3, tpos4;
 int aSin[512];
 int colors[256];
 float rad;

  unsigned int i,j;
  unsigned int index;
  int x;

  /*create sin lookup table */
  for (i = 0; i < 512; i++) {
      rad =  ((float)i * 0.703125) * 0.0174532; /* 360 / 512 * degree to rad, 360 degrees spread over 512 values to be able to use AND 512-1 instead of using modulo 360*/
      aSin[i] = sin(rad) * 1024; /*using fixed point math with 1024 as base*/
  }

    /* create palette */
  for (i = 0; i < 64; ++i) {
      colors[i]     = RED_LEV(i >> 1)             +GREEN_LEV(31 - ((i >> 1) + 1));
      colors[i+64]  = RED_LEV(31)                 +GREEN_LEV((i >> 1) + 1);
      colors[i+128] = RED_LEV(31 - ((i >> 1) + 1))+GREEN_LEV(31 - ((i >> 1) + 1));
      colors[i+192] = RED_LEV(0)                  +GREEN_LEV((i >> 1) + 1);
    }

  while( 1 ) {
    /* draw plasma */
    tpos4 = pos4;
    tpos3 = pos3;

    for (i = 0; i < HEIGHT;i++) {
      tpos1 = pos1 + 5;
      tpos2 = pos2 + 3;

      tpos3 &= 511;
      tpos4 &= 511;

      for (j = 0; j < WIDTH;j++) {
        tpos1 &= 511;
        tpos2 &= 511;

        x = aSin[tpos1] + aSin[tpos2] + aSin[tpos3] + aSin[tpos4]; /*actual plasma calculation*/

        index = 128 + (x >> 4); /*fixed point multiplication but optimized so basically it says (x * (64 * 1024) / (1024 * 1024)), x is already multiplied by 1024*/

           et024006_DrawPixel(j,i,colors[index]);
           tpos1 += 5;
           tpos2 += 3;
      }
      tpos4 += 3;
      tpos3 += 1;
    }

    /* move plasma */
    pos1 +=9;
    pos3 +=8;
  }
}
