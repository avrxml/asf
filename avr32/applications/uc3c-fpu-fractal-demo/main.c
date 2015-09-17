/*****************************************************************************
 *
 * \file
 *
 * \brief UC3C-EK FPU demo
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
 * This is the documentation for FPU  demo application running on the UC3C_EK
 * development kit.
 * <b>Operating mode: </b>
 * - The demo displays two fractals on LCD screen: one using FPU acceleration
 * and an other
 * one without FPU acceleration.
 * \section files Main Files
 * - main.c: Main File;
 *
 * \section compilinfo Compilation Information
 * This software is written for IAR Embedded Workbench
 * for Atmel AVR32. Other compilers may or may not work.
 *
 * \section deviceinfo Device Information
 * All AVR32 AT32UC3 devices with FPU can be used.
 *
 * \section configinfo Configuration Information
 * This example has been tested with the following configuration:
 * - UC3C_EK evaluation kit;
 * - CPU clock: 60 MHz;
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/products/AVR32/">Atmel AVR32</A>.\n
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include "board.h"
#include "gpio.h"
#include "power_clocks_lib.h"
#include "et024006dhu.h"
#include "cycle_counter.h"
#include "fpu_operators.h"
#include "conf_demo.h"
#include <math.h>

pcl_freq_param_t   pcl_freq_param=
{
   .cpu_f  =       FCPU_HZ         // Put here the wanted CPU freq
,  .pba_f    =     FPBA_HZ         // Put here the wanted PBA freq
,  .osc0_f     =   FOSC0           // Oscillator 0 frequency
,  .osc0_startup = OSC0_STARTUP    // Oscillator 0 startup time
};


//! Local variables for Draw Mandel with FPU Optimization
static int iter_wfpu =40;
static float xstart_wfpu = -2.0;
static float xend_wfpu    = 1.0;
static float ystart_wfpu = -1.35;
static float yend_wfpu   = 1.35;
static float xstep_wfpu;
static float ystep_wfpu;
static float x_wfpu,y_wfpu;
static float z_wfpu,zi_wfpu,newz_wfpu,newzi_wfpu;
static int colour_wfpu;
static int i_wfpu=0,j_wfpu=0,k_wfpu=0;
static int inset_wfpu;
static int draw_mandel_with_fpu(void);


//! Local variables for Draw Mandel without FPU Optimization
static int iter_wofpu =40;
static float xstart_wofpu = -2.0;
static float xend_wofpu    = 1.0;
static float ystart_wofpu = -1.35;
static float yend_wofpu   = 1.35;
static float xstep_wofpu;
static float ystep_wofpu;
static float x_wofpu,y_wofpu;
static float z_wofpu,zi_wofpu,newz_wofpu,newzi_wofpu;
static int colour_wofpu;
static int i_wofpu=0,j_wofpu=0,k_wofpu=0;
static int inset_wofpu;
static int draw_mandel_without_fpu(void);

/**
 * \brief Main function.
 */
int main(void)
{

	/*
	 * Initialize basic features for the AVR UC3 family.
	 *  - Sysclk init for configuring clock speed.
	 *  - Configure and enable LCD Display.
	 */
	pcl_configure_clocks(&pcl_freq_param);
	et024006_Init( pcl_freq_param.cpu_f, pcl_freq_param.cpu_f );
	gpio_set_gpio_pin(ET024006DHU_BL_PIN);
	et024006_DrawFilledRect(0 , 0, ET024006_WIDTH, ET024006_HEIGHT, BLACK );
        et024006_PrintString("With FPU ", (const unsigned char *)&FONT8x16, 40, 0, WHITE, BLACK);
        et024006_PrintString("Without FPU ", (const unsigned char *)&FONT8x16, 200, 0, WHITE, BLACK);

	/*
	 * Initialize local variables for fractal algorithm with FPU optimization.
	 */
	xstep_wfpu = (xend_wfpu-xstart_wfpu)/WIDTH;
	ystep_wfpu = (yend_wfpu-ystart_wfpu)/HEIGHT;
	x_wfpu = xstart_wfpu;
	y_wfpu = ystart_wfpu;


	/*
	 * Initialize local variables for fractal algorithm without FPU optimization.
	 */
	xstep_wofpu = (xend_wofpu-xstart_wofpu)/WIDTH;
	ystep_wofpu = (yend_wofpu-ystart_wofpu)/HEIGHT;
	x_wofpu = xstart_wofpu;
	y_wofpu = ystart_wofpu;

	/*the main loop */
	do
	{
	draw_mandel_with_fpu();
	draw_mandel_without_fpu();
        if (i_wofpu == 121){
          et024006_DrawFilledRect(0 , 0, ET024006_WIDTH, ET024006_HEIGHT, BLACK );
          /*
           * Initialize local variables for fractal algorithm with FPU optimization.
           */
          xstep_wfpu = (xend_wfpu-xstart_wfpu)/WIDTH;
          ystep_wfpu = (yend_wfpu-ystart_wfpu)/HEIGHT;
          x_wfpu = xstart_wfpu;
          y_wfpu = ystart_wfpu;


          /*
           * Initialize local variables for fractal algorithm without FPU optimization.
           */
          xstep_wofpu = (xend_wofpu-xstart_wofpu)/WIDTH;
          ystep_wofpu = (yend_wofpu-ystart_wofpu)/HEIGHT;
          x_wofpu = xstart_wofpu;
          y_wofpu = ystart_wofpu;
          iter_wfpu =40;
          xstart_wfpu = -2.0;
          xend_wfpu    = 1.0;
          ystart_wfpu = -1.35;
          yend_wfpu   = 1.35;
          i_wfpu=0,j_wfpu=0,k_wfpu=0;
          iter_wofpu =40;
          xstart_wofpu = -2.0;
          xend_wofpu    = 1.0;
          ystart_wofpu = -1.35;
          yend_wofpu   = 1.35;
          i_wofpu=0,j_wofpu=0,k_wofpu=0;
          et024006_PrintString("With FPU ", (const unsigned char *)&FONT8x16, 40, 0, WHITE, BLACK);
          et024006_PrintString("Without FPU ", (const unsigned char *)&FONT8x16, 200, 0, WHITE, BLACK);
        }

	}while(true);
}

/**
 * \brief Fractal Algorithm with FPU optimization.
 */
static int draw_mandel_with_fpu(void)
{
  t_cpu_time timer;
  while (i_wfpu<HEIGHT/2+1)
  {
    cpu_set_timeout( cpu_us_2_cy(DELAY_US,pcl_freq_param.cpu_f), &timer );
    while(j_wfpu<WIDTH)
    {
      z_wfpu = 0;
      zi_wfpu = 0;
      inset_wfpu = 1;
      while (k_wfpu<iter_wfpu)
      {
        /* z^2 = (a+bi)(a+bi) = a^2 + 2abi - b^2 */
		newz_wfpu = (z_wfpu*z_wfpu)-(zi_wfpu*zi_wfpu) + x_wfpu;
		newzi_wfpu = 2*z_wfpu*zi_wfpu + y_wfpu;
        z_wfpu = newz_wfpu;
        zi_wfpu = newzi_wfpu;
		if(((z_wfpu*z_wfpu)+(zi_wfpu*zi_wfpu)) > 4)
		{
		  inset_wfpu = 0;
		  colour_wfpu = k_wfpu;
		  k_wfpu = iter_wfpu;
		}
        k_wfpu++;
      };
      k_wfpu = 0;
      // Draw Mandelbrot set
      if (inset_wfpu)
      {
       et024006_DrawPixel(j_wfpu,i_wfpu+OFFSET_DISPLAY,BLACK);
       et024006_DrawPixel(j_wfpu,HEIGHT-i_wfpu+OFFSET_DISPLAY,BLACK);
      }
      else
      {
        et024006_DrawPixel(j_wfpu,
		i_wfpu+OFFSET_DISPLAY,
		BLUE_LEV((colour_wfpu*255) / iter_wfpu )+
		GREEN_LEV((colour_wfpu*127) / iter_wfpu )+
		RED_LEV((colour_wfpu*127) / iter_wfpu ));
        et024006_DrawPixel(j_wfpu,
		HEIGHT-i_wfpu+OFFSET_DISPLAY,
		BLUE_LEV((colour_wfpu*255) / iter_wfpu )+
		GREEN_LEV((colour_wfpu*127) / iter_wfpu )+
		RED_LEV((colour_wfpu*127) / iter_wfpu ));
      }
      x_wfpu += xstep_wfpu;
      j_wfpu++;
    };
    j_wfpu = 0;
    y_wfpu += ystep_wfpu;
    x_wfpu = xstart_wfpu;
    i_wfpu++;
    if( cpu_is_timeout(&timer) )
    {
      return 0;
    }
  };
  return 1;
}

/**
 * \brief Fractal Algorithm without FPU optimization.
 */
static int draw_mandel_without_fpu(void)
{
  t_cpu_time timer;
  while (i_wofpu<HEIGHT/2+1)
  {
    cpu_set_timeout( cpu_us_2_cy(DELAY_US,pcl_freq_param.cpu_f), &timer );
    while(j_wofpu<WIDTH)
    {
      z_wofpu = 0;
      zi_wofpu = 0;
      inset_wofpu = 1;
      while (k_wofpu<iter_wofpu)
      {
        /* z^2 = (a+bi)(a+bi) = a^2 + 2abi - b^2 */
		newz_wofpu = float_add(
								float_sub(
										float_mul(z_wofpu,z_wofpu),
										float_mul(zi_wofpu,zi_wofpu)),
							    x_wofpu
					);
		newzi_wofpu = float_add(
								2*float_mul(
											z_wofpu,
											zi_wofpu),
								y_wofpu
					);
        z_wofpu = newz_wofpu;
        zi_wofpu = newzi_wofpu;
		if(float_add(
						float_mul(z_wofpu,z_wofpu),
						float_mul(zi_wofpu,zi_wofpu)
			) > 4)
		{
		  inset_wofpu = 0;
		  colour_wofpu = k_wofpu;
		  k_wofpu = iter_wofpu;
		}
        k_wofpu++;
      };
      k_wofpu = 0;
      // Draw Mandelbrot set
      if (inset_wofpu)
      {
       et024006_DrawPixel(j_wofpu+WIDTH,i_wofpu+OFFSET_DISPLAY,BLACK);
       et024006_DrawPixel(j_wofpu+WIDTH,HEIGHT-i_wofpu+OFFSET_DISPLAY,BLACK);
      }
      else
      {
        et024006_DrawPixel(j_wofpu+WIDTH,
			i_wofpu+OFFSET_DISPLAY,
			BLUE_LEV((colour_wofpu*255) / iter_wofpu )+
			GREEN_LEV((colour_wofpu*127) / iter_wofpu )+
			RED_LEV((colour_wofpu*127) / iter_wofpu ));
        et024006_DrawPixel(j_wofpu+WIDTH,
			HEIGHT-i_wofpu+OFFSET_DISPLAY,
			BLUE_LEV((colour_wofpu*255) / iter_wofpu )+
			GREEN_LEV((colour_wofpu*127) / iter_wofpu )+
			RED_LEV((colour_wofpu*127) / iter_wofpu ));
      }
      x_wofpu += xstep_wofpu;
      j_wofpu++;
    };
    j_wofpu = 0;
    y_wofpu += ystep_wofpu;
    x_wofpu = xstart_wofpu;
    i_wofpu++;
    if( cpu_is_timeout(&timer) )
    {
      return 0;
    }
  };
  return 1;
}
