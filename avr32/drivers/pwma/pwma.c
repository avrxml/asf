/**
 * \file
 *
 * \brief Basic Pulse Width Modulation Controller (PWMA) driver.
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
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


#include "pwma.h"
#include "compiler.h"

void pwma_config_and_enable(volatile avr32_pwma_t *pwma,
							unsigned long long int channel_mask,
							int period_cycles, int duty_cycles)
{
	pwma->cr =
		(period_cycles << AVR32_PWMA_CR_TOP_OFFSET) // Set the TOP and SPREAD values of the timebase counter
		| AVR32_PWMA_CR_TCLR_MASK                   // Clear the timebase counter
		| AVR32_PWMA_CR_EN_MASK;                    // Enable the PWMA

	pwma_set_channels_value(pwma, channel_mask, duty_cycles);
}

bool pwma_config_enable(volatile avr32_pwma_t *pwma, uint32_t pwma_freq_hz, 
						uint32_t gclk_freq_hz, uint16_t spread)
{
	uint16_t top_value;

	/* Calculate the Effective Top Value (ETV)
	 * where ETV = (Tpwma/Tgclk)-1
	 */
	top_value = (div_ceil(gclk_freq_hz,pwma_freq_hz))-1; 

	/* Check for the maximum limit of TOP 
	 * and SPREAD value that can be loaded
	 */
	if (spread > PWMA_MAXIMUM_SPREAD)
		return FAIL;
	if (top_value > PWMA_MAXIMUM_TOP)
		return FAIL;

	/* Disable all PWMA interrupts(if any) before enabling PWMA */
	if(pwma->imr & AVR32_PWMA_IMR_MASK)
		pwma->idr=AVR32_PWMA_IDR_MASK;

	pwma->cr = 
			AVR32_PWMA_EN_MASK                      //Enable the PWMA module
			|(spread <<AVR32_PWMA_CR_SPREAD_OFFSET) //Load the SPREAD value 
			| AVR32_PWMA_CR_TCLR_MASK ;             //Clear the timebase counter

	/* Wait for the PWMA to be ready for writing. */
	pwma_wait_for_ready(pwma);

	/* Load the TOP value. */
	#if (defined(_AVR32_PWMA_201_H_INCLUDED) ||\
		defined(AVR32_PWMA_200_H_INCLUDED))
		pwma->tvr = top_value & AVR32_PWMA_TVR_TOP_MASK;
	#else
		pwma->cr |= (top_value <<AVR32_PWMA_CR_TOP_OFFSET);
	#endif

	return PASS;
}

bool pwma_set_channels_value(volatile avr32_pwma_t *pwma,
							uint64_t channel_mask,
							uint16_t duty_cycle)
{
	uint32_t channel_mask_low = channel_mask & 0xffffffff;
	uint32_t channel_mask_high = (channel_mask >> 32) & 0xffffffff;
	uint16_t read_top;
	uint16_t read_spread;

	/* Read the spread and top value */
	read_spread = pwma->CR.spread;
	read_top = pwma->CR.top;

	/*
	 * check if the duty cycle exceeds TOP (in normal mode)
	 * else check if the duty cycle exceed TOP-SPREAD (if spread is 
	 * enabled). if yes, return FAIL.
	 */
	if (duty_cycle > (read_top-read_spread))
		return FAIL;

	/* Wait for the PWMA to be ready for writing.*/
	pwma_wait_for_ready(pwma);

	/* Set the common duty cycle value. */
	pwma->isduty = duty_cycle & AVR32_PWMA_ISDUTY_DUTY_MASK;

	/* 
	 * Define the interlinked channels. ISCHSET0 register 
	 * controls first 32 channels (0 to 31)
	 */
	#if (defined(_AVR32_PWMA_201_H_INCLUDED) ||\
		defined(AVR32_PWMA_200_H_INCLUDED))
		pwma->chgroup[0].ischset = channel_mask_low;
	#else
		pwma->ischset0 = channel_mask_low;
	#endif

	/* Wait for the PWMA to be ready for writing.*/
	pwma_wait_for_ready(pwma);

	/* 
	 * Define the interlinked channels. ISCHSET1 register 
	 * controls next 32 channels (32 to 63)
	 */
	#if (defined(_AVR32_PWMA_201_H_INCLUDED) ||\
		defined(AVR32_PWMA_200_H_INCLUDED))
	pwma->chgroup[1].ischset = channel_mask_high;
	#else
	pwma->ischset1 = channel_mask_high;
	#endif

	/* Wait for the PWMA to be ready for writing. */
	pwma_wait_for_ready(pwma);

	return PASS;
}

bool pwma_set_multiple_values(volatile avr32_pwma_t *pwma,
							 uint32_t channel_mask,
							 uint16_t *channel_duty_cycles)
{
	uint8_t channels[PWMA_CHANNEL_LIMIT];
	uint16_t read_top;
	uint16_t read_spread;
	uint8_t count;

	/* Read the spread and top value */
	read_spread = pwma->CR.spread;
	read_top = pwma->CR.top;

	/*
	 * check if the duty cycle exceeds TOP (in normal mode)
	 * else check if the duty cycle exceed TOP-SPREAD (if spread is 
	 * enabled). if yes, return FAIL.
	 */
	for (count=0; count<PWMA_CHANNEL_LIMIT; count++){
		if (channel_duty_cycles[count] > (read_top-read_spread))
			return FAIL;
	}

	//Load the channel index number to channel buffer
	channels[0]=channel_mask & 0xff;
	channels[1]=(channel_mask >> 8) & 0xff;
	channels[2]=(channel_mask >> 16) & 0xff;
	channels[3]=(channel_mask >> 24) & 0xff;

	// Wait for the PWMA to be ready for writing.
	pwma_wait_for_ready(pwma);

	/* 
	 * For UC3D and UC3L3/L4 devices loading to DUTY registers will 
	 * update the least 8 bits of the IMDUTY registers
	 */
	#if (UC3D || UC3L3_L4)
		pwma->duty0 = channel_duty_cycles[0] & AVR32_PWMA_DUTY0_DUTY_MASK;
		pwma->duty1 = channel_duty_cycles[1] & AVR32_PWMA_DUTY1_DUTY_MASK;
		pwma->duty2 = channel_duty_cycles[2] & AVR32_PWMA_DUTY2_DUTY_MASK;
		pwma->duty3 = channel_duty_cycles[3] & AVR32_PWMA_DUTY3_DUTY_MASK;
	#else
		pwma->imduty =  (channel_duty_cycles[3] << 24)
						| (channel_duty_cycles[2] << 16)
						| (channel_duty_cycles[1] << 8)
						| (channel_duty_cycles[0]);
	#endif

	/* Load the channels values to Channel select register */
	pwma->imchsel = (channels[3] << 24)
					| (channels[2] << 16)
					| (channels[1] << 8)
					| (channels[0]);

	/* Wait for the PWMA to be ready for writing. */
	pwma_wait_for_ready(pwma);

	return PASS;
}

bool pwma_write_top_value(volatile avr32_pwma_t *pwma,uint16_t top_value)
{
	if (top_value > PWMA_MAXIMUM_TOP)
		return FAIL;

	/* Wait for the PWMA to be ready for writing */
	pwma_wait_for_ready(pwma);

	// Load the TOP value. 
	#if (defined(_AVR32_PWMA_201_H_INCLUDED) ||\
		defined(AVR32_PWMA_200_H_INCLUDED))
		pwma->tvr = top_value & AVR32_PWMA_TVR_TOP_MASK;
	#else
		pwma->cr |= (top_value <<AVR32_PWMA_CR_TOP_OFFSET);
	#endif

	return PASS;
}

#if (defined(_AVR32_PWMA_201_H_INCLUDED) || defined(AVR32_PWMA_200_H_INCLUDED))
void pwma_enable_cwg_mode(volatile avr32_pwma_t *pwma,
						 uint32_t channel_mask)
{
	/* Enable the Composite waveform Mode */
	/* CWG0 controls channels first 32 pairs of channels channel (0 to 63) */
	pwma->chgroup[0].cwg = channel_mask;
}
#endif

#if (defined(_AVR32_PWMA_201_H_INCLUDED) || defined(AVR32_PWMA_101_H_INCLUDED))
void pwma_enable_input_peripheral_event(volatile avr32_pwma_t *pwma,
										uint64_t channel_mask)
{
	uint32_t channel_mask_low = channel_mask & 0xffffffff;
	uint32_t channel_mask_high = ((channel_mask >> 32) & 0xffffffff);

	/* Enable the input peripheral event. */
	#if (defined(AVR32_PWMA_101_H_INCLUDED))
		/* CHEER0 controls 0-31 channels */
		pwma->cheer0=channel_mask_low;
		/* CHEER1 controls channels 32 to 63 */
		pwma->cheer1=channel_mask_high;
	#else
		/* CHEER0 controls 0-31 channels */
		pwma->chgroup[0].cheer=channel_mask_low;
		/* CHEER1 controls channels 32 to 63 */
		pwma->chgroup[1].cheer=channel_mask_high;
	#endif
}
#endif

#if (defined(_AVR32_PWMA_201_H_INCLUDED))
void pwma_config_event_response(volatile avr32_pwma_t *pwma,
								uint64_t channel_mask)
{
	/* Enable the input event response. */
	/* CHERR0 controls channels 0 to 31 */
	pwma->chgroup[0].cherr=(channel_mask & 0xffffffff);
	/* CHERR1 controls channels 32 to 64 */
	pwma->chgroup[1].cherr=((channel_mask >> 32) & 0xffffffff);
}
#endif
