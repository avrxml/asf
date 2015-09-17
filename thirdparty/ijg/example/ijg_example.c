/*****************************************************************************
 *
 * \file
 *
 * \brief IJG JPEG decoder example
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
 *
 * This example shows how to use the JPEG IJG decoder library
 *
 * \section files Main Files
 * - ijg_example.c: Jpeg IJG example application.
 *
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC for AVR32 and IAR Systems compiler
 * for AVR32. Other compilers may or may not work.
 *
 * \section deviceinfo Device Info
 * All AVR32 devices with a EBI module can be used. This example has been tested
 * with the following setup:<BR>
 * <ul>
 *  <li>EVK1104 evaluation kit
 *  <li>EVK1105 evaluation kit
 *  <li>UC3C_EK evaluation kit
 *  </ul>
 *
 * \section setupinfo Setup Information
 * CPU speed: <i> Switch to oscillator external OSC0 = 12 Mhz. </i>
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/products/AVR32/">Atmel AVR32</A>.\n
 */

#include "string.h"
#include "board.h"
#include "sdramc.h"
#include "intc.h"
#include "gpio.h"
#include "print_funcs.h"
#include "usart.h"
#include "et024006dhu.h"
#include "power_clocks_lib.h"
#include "delay.h"
#include "jpeg_decoder.h"
#include "jpegfile.h"


/*! \name External Board Mappings
 */
//! @{
#if BOARD==EVK1104
#  define EXAMPLE_TC_CHANNEL_PIN        AVR32_TC0_A0_0_0_PIN
#  define EXAMPLE_TC_CHANNEL_FUNCTION   AVR32_TC0_A0_0_0_FUNCTION
#elif BOARD==EVK1105
#endif
//! @}

const U8 * stream_jpeg_src_ptr;		// JPEG source pointer
U16 stream_src_size;				// JPEG source size

pcl_freq_param_t   pcl_freq_param=
{
   .cpu_f  =       FCPU_HZ          // Put here the wanted CPU freq
,  .pba_f    =     FPBA_HZ          // Put here the wanted PBA freq
,  .osc0_f     =   FOSC0            // Oscillator 0 frequency
,  .osc0_startup = OSC0_STARTUP     // Oscillator 0 startup time
};

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
#endif // #if BOARD == EVK1105

/*
 * \brief main function : do init of the jpeg decoder library and display an image
 */
int main(void)
{
	et024006_color_t const *picture_ptr;

	// Set CPU and PBA clock
	pcl_configure_clocks(&pcl_freq_param);

  // Initialize usart communication
	init_dbg_rs232(pcl_freq_param.pba_f);

  // Initialize TFT display
	et024006_Init( pcl_freq_param.cpu_f, pcl_freq_param.cpu_f );

	sdramc_init(pcl_freq_param.cpu_f);

	// Enable back-light
	#if BOARD == EVK1105
	/* PWM is fed by PBA bus clock which is by default the same
	 * as the CPU speed. We set a 0 duty cycle and thus keep the
	 * display black*/
	tft_bl_init();
        /* Lets do a nice fade in by increasing the duty cycle */
        while(pwm_channel6.cdty < pwm_channel6.cprd)
        {
          pwm_channel6.cdty++;
          pwm_channel6.cupd = pwm_channel6.cdty;
          //pwm_channel6.cdty--;
          pwm_async_update_channel(AVR32_PWM_ENA_CHID6, &pwm_channel6);
          delay_ms(10);
        }
	#elif BOARD == EVK1104 || BOARD == UC3C_EK
	gpio_set_gpio_pin(ET024006DHU_BL_PIN);
	#endif

	// Clear the display: make it blue
	et024006_DrawFilledRect(0,0,ET024006_WIDTH,ET024006_HEIGHT,0x2458 );

	print_dbg("\x1B[2J\x1B[HDecoding JPEG image...\r\n");

	if (!jpeg_lib_init())						// JPEG IJG lib initialization
	{
		print_dbg("\r\n Decoder Initialization failed");
		while (1);
	}
	// JPEG stream definition
	stream_jpeg_src_ptr = jpegdata;
	stream_src_size = sizeof jpegdata/sizeof jpegdata[0];

	// main decoder
	U16 width;
	U16 height;

	while(1)
	{
		width= 320;
		height= 240;
		picture_ptr = (et024006_color_t const *)jpeg_lib_decode_ex(0, &width, &height);
		et024006_PutPixmap(	picture_ptr, 320, 0, 0, 0, 0, 320, 240 );
  	    delay_ms(2000);
		width= 160;
		height= 120;
		picture_ptr = (et024006_color_t const *)jpeg_lib_decode_ex(0, &width, &height);
		et024006_PutPixmap(	picture_ptr, 160, 0, 0, 0, 0, 160,120 );
		et024006_PutPixmap(	picture_ptr, 160, 0, 0, 160, 0, 160, 120 );
		et024006_PutPixmap(	picture_ptr, 160, 0, 0, 0, 120, 160, 120 );
		et024006_PutPixmap(	picture_ptr, 160, 0, 0, 160, 120, 160, 120 );
	    delay_ms(500);
		et024006_PutPixmap(	picture_ptr, 160, 0, 0, 80, 60, 160, 120 );
	    delay_ms(2000);
	}
	jpeg_lib_exit();							// JPEG IJG lib out
}
