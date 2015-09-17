/*****************************************************************************
 *
 * \file
 *
 * \brief LodePNG decoder example
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

/*! \mainpage
 * \section intro Introduction
 *
 * This example shows how to use the LodePNG decoder library
 *
 * \section files Main Files
 * - lodepng_example.c: LodePNG example application.
 *
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC for AVR32 .
 * Other compilers may or may not work.
 *
 * \section deviceinfo Device Info
 * All AVR UC3 devices with a EBI module can be used. This example has
 * been tested with the following setup:<BR>
 * <ul>
 *  <li>EVK1104 evaluation kit
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
#include "pngfile.h"
#include "lodepng.h"
#include "conf_clock.h"

#define PIXEL uint16_t

#define AVERAGE(a, b)   (PIXEL)( (a) == (b) ? (a) \
			  : (((a) & 0xf7dfU) + ((b) & 0xf7dfU)) >> 1 )


/*! \name External Board Mappings
 */
//! @{
#if BOARD==EVK1104
#  define EXAMPLE_TC_CHANNEL_PIN        AVR32_TC0_A0_0_0_PIN
#  define EXAMPLE_TC_CHANNEL_FUNCTION   AVR32_TC0_A0_0_0_FUNCTION
#elif BOARD==EVK1105
#endif
//! @}

pcl_freq_param_t 	pcl_freq_param = {
							.cpu_f	= FCPU_HZ,	// Put here the wanted CPU freq
							.pba_f	= FPBA_HZ,	// Put here the wanted PBA freq
							.osc0_f	= FOSC0,	// Oscillator 0 frequency
							.osc0_startup	= OSC0_STARTUP	// Osc_0 startup time
						};

// Call to the decoder display function (from lodepng_app.c)
void lodepng_decode_display(unsigned char* table,size_t buffersize,int scale);

/* supports scaling by 2,4,8 in denominator to 1 */
void scale_by_denom(PIXEL *Target, PIXEL *Source, int SrcWidth,
												int SrcHeight,int denom);

#if BOARD == EVK1105
#include "pwm.h"
avr32_pwm_channel_t 	pwm_channel6 = {
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

	pwm_opt_t 	opt = {
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
	 */
	pwm_init(&opt);
	pwm_channel6.CMR.calg = PWM_MODE_LEFT_ALIGNED;
	pwm_channel6.CMR.cpol = PWM_POLARITY_HIGH; //PWM_POLARITY_LOW;
	pwm_channel6.CMR.cpd = PWM_UPDATE_DUTY;
	pwm_channel6.CMR.cpre = AVR32_PWM_CMR_CPRE_MCK_DIV_2;

	pwm_channel_init(6, &pwm_channel6);
	pwm_start_channels(AVR32_PWM_ENA_CHID6_MASK);

}
#endif // #if BOARD == EVK1105

/*
 * \brief main function : Example for the usage of PNG decoder
 * and display an image
 */
int main(void)
{
	// Set CPU and PBA clock
	pcl_configure_clocks(&pcl_freq_param);

	// Initialize usart communication
	init_dbg_rs232(pcl_freq_param.pba_f);

	// Initialize TFT display
	et024006_Init( pcl_freq_param.cpu_f, pcl_freq_param.cpu_f );

	// intialise the SDRAM
	sdramc_init(pcl_freq_param.cpu_f);

	// Enable back-light
#if BOARD == EVK1105
	/* PWM is fed by PBA bus clock which is by default the same
	 * as the CPU speed. We set a 0 duty cycle and thus keep the
	 * display black
	 */
	tft_bl_init();

	// Lets do a nice fade in by increasing the duty cycle
	while(pwm_channel6.cdty < pwm_channel6.cprd) {

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

	while(1) {
		// Decode and Display png file for each scaling - 1
		lodepng_decode_display((unsigned char *)table1,sizeof(table1),1);
		delay_ms(2000);

		// Decode and Display png file for each scaling - 2
		lodepng_decode_display((unsigned char *)table1,sizeof(table1),2);
		delay_ms(2000);

		// Decode and Display png file for each scaling - 4
		lodepng_decode_display((unsigned char *)table1,sizeof(table1),4);
		delay_ms(2000);

		// Decode and Display png file for each scaling - 8
		lodepng_decode_display((unsigned char *)table1,sizeof(table1),8);
		delay_ms(2000);
	}
}


/*
 * \brief lodepng_decode_display function : This function initialise the
 * PNG decoder, decodes the .PNG image and display it on ET024006 display device
 * with various scaling factors.
 */

void lodepng_decode_display(unsigned char* table,size_t buffersize,int scale)
{
	unsigned char* image;
	uint8_t r,g,b;
	size_t  imagesize;
	LodePNG_Decoder decoder;
	unsigned int i,j;
	uint16_t color;
	uint16_t* temp16;

	LodePNG_Decoder_init(&decoder);
	//decode the png
	LodePNG_decode(&decoder, &image, &imagesize, table, buffersize);
	print_dbg("Decoding completed \r\n");

	/* if there's an error, display it, otherwise display
	 * information about the image
	 */
	if(decoder.error) {
		print_dbg("error: Decoding\r\n");
		return;
	}
	else {
#if (defined __GNUC__)
		print_dbg("Decoding Successful\r\n");
#elif (defined __ICCAVR32__)
		printf("w: %d\n", decoder.infoPng.width);
		printf("h: %d\n", decoder.infoPng.height);
		printf("bitDepth: %d\n", decoder.infoPng.color.bitDepth);
		printf("bpp: %d\n", LodePNG_InfoColor_getBpp(&decoder.infoPng.color));
		printf("colorChannels: %d\n", LodePNG_InfoColor_getChannels(
			&decoder.infoPng.color));
		printf("paletteSize: %d\n", decoder.infoPng.color.palettesize);
		printf("colorType: %d\n", decoder.infoPng.color.colorType);
		printf("compressionMethod: %d\n", decoder.infoPng.compressionMethod);
		printf("filterMethod: %d\n", decoder.infoPng.filterMethod);
		printf("interlaceMethod: %d\n", decoder.infoPng.interlaceMethod);
		for(i = 0; i < decoder.infoPng.text.num; i++)
		printf("%s: %s\n",  decoder.infoPng.text.keys[i],
			decoder.infoPng.text.strings[i]);
		for(i = 0; i < decoder.infoPng.itext.num; i++)
		printf("%s (%s %s) : %s\n",  decoder.infoPng.itext.keys[i],
			decoder.infoPng.itext.langtags[i], decoder.infoPng.itext.transkeys[i],
			decoder.infoPng.itext.strings[i]);

		if(decoder.infoPng.time_defined) {
			printf("modification time: %d-%d-%d %d:%d:%d\n",
				decoder.infoPng.time.year, decoder.infoPng.time.month,
				decoder.infoPng.time.day, decoder.infoPng.time.hour,
				decoder.infoPng.time.minute, decoder.infoPng.time.second);
		}

		if(decoder.infoPng.phys_defined) {
			printf("physical size: %d %d %d\n", decoder.infoPng.phys_x,
				decoder.infoPng.phys_y, (int)decoder.infoPng.phys_unit);
		}
#endif

	}
   print_dbg("sending to display\r\n");

	// changing to 16-bit 565
	temp16 = (uint16_t *) image;
	for(i=0,j=0;i<(decoder.infoPng.width*decoder.infoPng.height*4);i=i+4,j++) {
		r= (uint8_t)*(image+i);
		g = (uint8_t)*(image+i+1);
		b = (uint8_t)*(image+i+2);
		color = et024006_Color(r,g,b);
		temp16[j] = color;
	}

	//call scaling if required -supports only 1/2,1/4,1/8
	if(scale>1) {
		scale_by_denom(temp16,temp16,320,240,scale);
	}

	et024006_DrawFilledRect(0,0,ET024006_WIDTH,ET024006_HEIGHT,0x2458 );

	print_dbg("sending to display\r\n");
	et024006_PutPixmap(	(uint16_t *)temp16, 320/scale, 0, 0, 0, 0, 320/scale,
		240/scale );
	free(image);

	// cleanup decoder
	LodePNG_Decoder_cleanup(&decoder);
	return;
}



// supports scaling by 2,4,8 in denominator to 1
void scale_by_denom(PIXEL *Target, PIXEL *Source, int SrcWidth,
	int SrcHeight,int denom)
{
	int x, y, x2, y2;
	int TgtWidth, TgtHeight;
	PIXEL p,p1,q,q1,r,r1,s,s1;

	TgtWidth = SrcWidth / denom;
	TgtHeight = SrcHeight / denom;

	// Box filter method
	for (y = 0; y < TgtHeight; y++) {
		y2 = denom * y;
		for (x = 0; x < TgtWidth; x++) {
			x2 = denom * x;
			p = AVERAGE(Source[y2*SrcWidth + x2], Source[y2*SrcWidth +
				x2 + 1]);
			q = AVERAGE(Source[(y2+1)*SrcWidth + x2], Source[(y2+1)*SrcWidth +
				x2 + 1]);

			if (denom==4) {
				r = AVERAGE(Source[(y2+2)*SrcWidth + x2],
					Source[(y2+2)*SrcWidth + x2 + 1]);
				s = AVERAGE(Source[(y2+3)*SrcWidth + x2],
					Source[(y2+3)*SrcWidth + x2 + 1]);
				p = AVERAGE(p,r);
				q = AVERAGE(q,s);
			}
			else if(denom==8) {
				r = AVERAGE(Source[(y2+2)*SrcWidth + x2],
					Source[(y2+2)*SrcWidth + x2 + 1]);
				s = AVERAGE(Source[(y2+3)*SrcWidth + x2],
					Source[(y2+3)*SrcWidth + x2 + 1]);
				p = AVERAGE(p,r);
				q = AVERAGE(q,s);
				p1 = AVERAGE(Source[(y2+4)*SrcWidth + x2],
					Source[(y2+4)*SrcWidth + x2 + 1]);
				q1 = AVERAGE(Source[(y2+5)*SrcWidth + x2],
					Source[(y2+5)*SrcWidth + x2 + 1]);
				r1 = AVERAGE(Source[(y2+6)*SrcWidth + x2],
					Source[(y2+6)*SrcWidth + x2 + 1]);
				s1 = AVERAGE(Source[(y2+7)*SrcWidth + x2],
					Source[(y2+7)*SrcWidth + x2 + 1]);
				p1 = AVERAGE(p1,r1);
				q1 = AVERAGE(q1,s1);
				p =  AVERAGE(p,p1);
				q = AVERAGE(q,q1);
			}
			Target[y*TgtWidth + x] = AVERAGE(p, q);
		} // for
	} // for
}

