/**
 * \file
 *
 * \brief AT30TS Temperature Sensor configuration file.
 *
 * Copyright (c) 2015 Atmel Corporation. All rights reserved.
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
#ifndef _CONF_AT30TS_H_
#define _CONF_AT30TS_H_

/** \brief ACK_POLLING_INTERATION
 *
 * With an oscilloscope or logic analyzer, determine the number of NACK
 * iteration required for ts_ack_polling() to span 5ms.  5ms is the
 * maximum time required for a nonvolatile operation to complete.
 *
 * For example: With the F_CPU running at 16MHz and an optimization of -O2,
 * it takes 165 iterations of ts_ack_polling to span 5ms.
 */

/* FOR ACK POLLING */
#define ACK_POLLING_ITERATION  1000

#define TWI_MODULE        &TWIC

/* Device type must be defined in the project options*/
#if    (!_AT30TS75_   && \
	!_AT30TS750_  && \
	!_AT30TSE752_ && \
	!_AT30TSE754_ && \
	!_AT30TSE758_ && \
	!_AT30TSE00_  && \
	!_AT30TSE002B_)

	 #error No Digital Temperature Sensors device defined
#define _AT30TSE758_
#endif

#endif // _CONF_AT30TS_H_
