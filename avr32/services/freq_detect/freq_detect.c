/**
 * \file
 *
 * \brief Oscillator Frequency detection
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#include "compiler.h"
#include "conf_freq_detect.h"
#include "freq_detect.h"

#if (defined __GNUC__)
#define USE_BUILT_IN_64_BITS_OPERATORS   false
#elif (defined __ICCAVR32__)
#define USE_BUILT_IN_64_BITS_OPERATORS   true
// Note: with IAR, use the 64bit built-in operators (saving 8Bytes of code)
#endif


uint32_t freq_detect_start(void)
{
	int delay_osc_ref, delay_osc_target;
	int counter_ref, counter_target;
	int estimated_freq_hz;
	int previous_estimated_freq_hz;
	bool waiting;

	const uint32_t freq_avalaible[] = FREQ_DETECT_FREQ_AVALAIBLE;
	int i, current_candidate = -1;
	unsigned int current_excursion = (unsigned int) -1;

	// Compute approximate delays to read counter values
	freq_detect_iface_ref_cnt_reset();
	delay_osc_ref = freq_detect_iface_ref_cnt_value();
	freq_detect_iface_target_cnt_reset();
	delay_osc_target = freq_detetc_iface_target_cnt_value();

	freq_detect_iface_ref_cnt_reset();
	freq_detect_iface_target_cnt_reset();

	estimated_freq_hz = 0;
	counter_ref = 0;
	counter_target = 0;
	do {
		int previous_counter_ref, previous_counter_target;

		waiting = true;
		// Assigns previous values
		previous_estimated_freq_hz = estimated_freq_hz;
		previous_counter_ref = counter_ref;
		previous_counter_target = counter_target;
		// Reads the timer counters
		counter_ref = freq_detect_iface_ref_cnt_value();
		counter_target = freq_detetc_iface_target_cnt_value();
		// Makes sure the counter are not wrapped
		if (previous_counter_ref > counter_ref ||
				previous_counter_target > counter_target) {
			break;
		}
		if (counter_target <= delay_osc_target || counter_ref <= delay_osc_ref) {
			continue;
		}
		// Subtracts the delay due to the functions' call
		counter_ref -= delay_osc_ref;
		counter_target -= delay_osc_target;
		// Estimates the frequency
#if defined(USE_BUILT_IN_64_BITS_OPERATORS) \
		&& (USE_BUILT_IN_64_BITS_OPERATORS == true)
		estimated_freq_hz = ((unsigned long long) counter_target
				* FREQ_DETECT_REF_HZ) / counter_ref;
#else
		estimated_freq_hz = counter_target / counter_ref;
		estimated_freq_hz *= FREQ_DETECT_REF_HZ;
#endif
		if (!estimated_freq_hz) {
			continue;
		}
		waiting = false;

		// Stay in the loop until the estimated frequency stabilizes
	} while (waiting || abs(previous_estimated_freq_hz - estimated_freq_hz)
			> FREQ_DETECT_CONV_RATE);

	if (0==estimated_freq_hz) {
		return 0; // No frequency find
	}

	// Matches a reference frequency amongst a list of known

	for (i=0; i<sizeof(freq_avalaible)/sizeof(freq_avalaible[0]); i++) {
		if (current_excursion > abs(estimated_freq_hz - freq_avalaible[i])) {
			current_excursion = (unsigned int)
					abs(estimated_freq_hz - freq_avalaible[i]);
			current_candidate = i;
		}
	}
	if (current_candidate == -1) {
		return estimated_freq_hz;
	}
	return freq_avalaible[current_candidate];
}
