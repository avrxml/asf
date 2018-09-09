/**
 * @file timer.c
 *
 * @brief D21 implementation of the timer interface.
 *
 * Copyright (c) 2017-2018 Microchip Technology Inc. and its subsidiaries.
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
#include "AWS_SDK/aws_iot_src/protocol/mqtt/aws_iot_embedded_client_wrapper/timer_interface.h"
extern volatile uint32_t ms_ticks;
/**
 * \brief Get time in milliseconds.
 *
 * \return milli second ticks count.
 */
static uint32_t getTimeInMillis(void)
{
	return ms_ticks;
}
/**
 * \brief Timer expiry check function.
 *
 * \param[in] timer Timer to be checked.
 *
 * \return time to expiry.
 */
char expired(Timer *timer)
{
	return ((timer->end_time > 0)
	&& ((getTimeInMillis() + timer->offset) > timer->end_time));
}
/**
 * \brief count down timer function.
 *
 * \param[in] timer Timer to be started.
 * \param[in] timeout timeout in milliseconds.
 *
 * \return None.
 */
void countdown_ms(Timer *timer, unsigned int timeout)
{
	uint32_t timems = getTimeInMillis();

	timer->end_time = timems + timeout;

	if (timer->end_time < timems) {
		timer->offset = ~0 - timems + 1;
		timer->end_time += timer->offset;
	}
	else {
		timer->offset = 0;
	}
}
/**
 * \brief count down timer function.
 *
 * \param[in] timer Timer to be started.
 * \param[in] timeout timeout in seconds.
 *
 * \return None.
 */
void countdown(Timer *timer, unsigned int timeout)
{
	uint32_t timems = getTimeInMillis();

	timer->end_time = timems + (timeout * 1000);

	if (timer->end_time < timems) {
		timer->offset = ~0 - timems + 1;
		timer->end_time += timer->offset;
	}
	else {
		timer->offset = 0;
	}
}
/**
 * \brief Time avaible for timer expiry function.
 *
 * \param[in] timer Timer to be started.
 *
 * \return Time for expiry in milliseconds.
 */
int left_ms(Timer *timer)
{
	int diff = timer->end_time - (getTimeInMillis() + timer->offset);
	return (diff > 0 ? diff : 0);
}
/**
 * \brief Timer intialization function.
 *
 * \param[in] timer Timer to be intialized.
 *
 * \return None.
 */
void InitTimer(Timer *timer)
{
	timer->end_time = 0;
	timer->offset = 0;
}