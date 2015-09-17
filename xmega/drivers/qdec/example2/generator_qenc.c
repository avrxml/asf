/**
 * \file
 *
 * \brief XMEGA E Quadrature decoder test signal generation main file. It
 *        generates a quadrature encoded signal used for decoder testing.
 *
 * Copyright (C) 2014-2015 Atmel Corporation. All rights reserved.
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

#include <asf.h>
#include <generator_qenc.h>

#define QENC_PH0_PIN             (0x01)
#define QENC_PH90_PIN            (0x02)
#define QENC_PH0_PH90_PINS       (0x03)
#define QENC_INDEX_PIN           (0x04)
#define QENC_PH0_PH90_INDEX_PINS (0x07)

/* ! \brief Static variable to store port used for signal generation */
static PORT_t *generator_qenc_port;

/* ! \brief Static variable to store the first pin position
 * of port used for signal generation */
static uint8_t generator_qenc_pins_base;

/* ! \brief Static variable to store revolution value in generated signal */
static uint8_t generator_qenc_revolution;

/* ! \brief Static variable to store direction */
static bool generator_qenc_dir;

/* ! \brief Static variable to store timer handle in generated signal */
static volatile void *generator_qenc_timer;

#if XMEGA_E
#define tc_enable                          tc45_enable
#define tc_set_resolution                  tc45_set_resolution
#define tc_get_resolution                  tc45_get_resolution
#define tc_set_wgm                         tc45_set_wgm
#define TC_WG_NORMAL                       TC45_WG_NORMAL
#define tc_write_count                     tc45_write_count
#define tc_write_period                    tc45_write_period
#define tc_clear_overflow                  tc45_clear_overflow
#define tc_set_overflow_interrupt_level    tc45_set_overflow_interrupt_level
#define tc_set_overflow_interrupt_callback tc45_set_overflow_interrupt_callback
#define TC_INT_LVL_LO                      TC45_INT_LVL_LO
#endif

/**
 * \brief The call back function for CCA interrupt
 *  - This function sets phase0 and clears phase90
 *  - If round passed, sets index signal also
 */
void generator_qenc_timer_ccaint_handler(void)
{
	/* Static variable to store pulse count */
	static uint16_t pulse_count = 0;
	static uint8_t quadrature_steps[4] = {
		0,
		QENC_PH90_PIN,
		QENC_PH0_PIN | QENC_PH90_PIN,
		QENC_PH0_PIN,
	};

	tc_clear_overflow(generator_qenc_timer);

	/* Increment pulse count */
	if (generator_qenc_dir) {
		pulse_count++;
		if (pulse_count == generator_qenc_revolution) {
			pulse_count = 0;
		}
	} else {
		if (pulse_count == 0) {
			pulse_count = generator_qenc_revolution - 1;
		} else {
			pulse_count--;
		}
	}

	generator_qenc_port->OUT
			= (generator_qenc_port->OUT & ~(QENC_PH0_PH90_INDEX_PINS
			<< generator_qenc_pins_base))
			| (quadrature_steps[pulse_count % 4]
			 << generator_qenc_pins_base)
			| ((pulse_count == 0) ?
			 (QENC_INDEX_PIN << generator_qenc_pins_base) : 0);
}

void generator_qenc_enable(PORT_t *port, uint8_t pins_base,
		volatile void *timer, uint8_t revolution, uint32_t freq, bool dir )
{
#if XMEGA_E
	Assert((TC4_t *)timer == &TCC4);
#endif

	/* Store parameter in static global variable */
	generator_qenc_port = port;
	generator_qenc_pins_base = pins_base;
	generator_qenc_revolution = revolution;
	generator_qenc_timer = timer;

	/* Clear all pins on test port */
	port->DIRSET = QENC_PH0_PH90_INDEX_PINS << generator_qenc_pins_base;
	port->OUTCLR = QENC_PH0_PH90_INDEX_PINS << generator_qenc_pins_base;

	tc_enable(timer);
	tc_set_wgm(timer, TC_WG_NORMAL);

	generator_qenc_set_freq(freq);
	generator_qenc_set_direction(dir);

	/* Enable low level interrupt on CCA */
	tc_set_overflow_interrupt_level(timer, TC_INT_LVL_LO);

	/* Set interrupt callback function for CCA */
	tc_set_overflow_interrupt_callback(timer,
			generator_qenc_timer_ccaint_handler);
}

void generator_qenc_set_freq(uint32_t freq)
{
	/* Number of ticks per step */
	uint16_t ticks = 0;

	/* The following code calculates the upper boundary of the timer and the
	 * interrupt positions to get a correct Quadrature signal of the given
	 * frequency.
	 * The different compare interrupts sets the phase0 and phase90 signals.
	 * Phase-0 uses Pin1, Phase90-Pin2 and Index-Pin3 from selected port
	 *
	 * Compare A interrupt clear phase0 and sets phase90
	 * Compare B interrupt sets phase0 and phase90
	 * Compare C interrupt sets phase0 and clear phase90
	 * Compare D interrupt clears phase0 and phase90.
	 *
	 * Compare A interrupt also sets the index signal when one round has
	 * passed.
	 */

	/* Calculates upper boundary of timer to get desired frequency */
	tc_set_resolution(generator_qenc_timer,
			(freq * generator_qenc_revolution) / 1000);
	ticks = ((uint64_t)tc_get_resolution(generator_qenc_timer) *
			1000) / (freq * generator_qenc_revolution);

	/* Write timer period register */
	tc_write_count(generator_qenc_timer, 0);
	tc_write_period(generator_qenc_timer, ticks);
}

void generator_qenc_set_direction(bool dir)
{
	generator_qenc_dir = dir;
}
