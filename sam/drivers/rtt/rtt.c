/**
 * \file
 *
 * \brief Real-time Timer (RTT) driver for SAM.
 *
 * Copyright (c) 2011-2012 Atmel Corporation. All rights reserved.
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

#include "rtt.h"

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/// @endcond

/**
 * \defgroup sam_drivers_rtt_group Real-time Timer (RTT)
 *
 * The Real-time Timer is built around a 32-bit counter used to count 
 * roll-over events of the programmable 16-bit prescaler, which enables 
 * counting elapsed seconds from a 32 kHz slow clock source. 
 * This is a driver for configuration and use of the RTT peripheral.
 *
 * @{
 */

/**
 * \brief Initialize the given RTT.
 *
 * \note This function restarts the real-time timer. If w_prescaler is equal 
 *  to zero, the prescaler period is equal to 2^16 * SCLK period. If not, 
 *  the prescaler period is equal to w_prescaler * SCLK period.
 *
 * \param p_rtt Pointer to an RTT instance.
 * \param w_prescaler Prescaler value for the RTT.
 *
 * \return 0 if successful.
 */
uint32_t rtt_init(Rtt *p_rtt, uint16_t us_prescaler)
{
	p_rtt->RTT_MR = (us_prescaler | RTT_MR_RTTRST);
	return 0;
}

/**
 * \brief Enable RTT interrupts.
 *
 * \param p_rtt Pointer to an RTT instance.
 * \param ul_sources Interrupts to be enabled.
 */
void rtt_enable_interrupt(Rtt *p_rtt, uint32_t ul_sources)
{
	p_rtt->RTT_MR |= ul_sources;
}

/**
 * \brief Disable RTT interrupts.
 *
 * \param p_rtt Pointer to an RTT instance.
 * \param ul_sources Interrupts to be disabled.
 */
void rtt_disable_interrupt(Rtt *p_rtt, uint32_t ul_sources)
{
	p_rtt->RTT_MR &= (~ul_sources);
}

/**
 * \brief Read the current value of the RTT timer value.
 *
 * \param p_rtt Pointer to an RTT instance.
 *
 * \return The current Real-time Timer value.
 */
uint32_t rtt_read_timer_value(Rtt *p_rtt)
{
	return p_rtt->RTT_VR;
}

/**
 * \brief Get the status register value of the given RTT.
 *
 * \param p_rtt Pointer to an RTT instance.
 *
 * \return The Real-time Timer status.
 */
uint32_t rtt_get_status(Rtt *p_rtt)
{
	return p_rtt->RTT_SR;
}

/**
 * \brief Configure the RTT to generate an alarm at the given time.
 *
 * \param p_rtt Pointer to an RTT instance.
 * \param ul_alarm_time Alarm time.
 *
 * \retval 0 Configuration is done.
 * \retval 1 Parameter error.
 */
uint32_t rtt_write_alarm_time(Rtt *p_rtt, uint32_t ul_alarm_time)
{
	if (ul_alarm_time == 0) {
		return 1;
	}

	/* Alarm time = ALMV + 1 */
	p_rtt->RTT_AR = ul_alarm_time - 1;
	return 0;
}

//@}

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/// @endcond
