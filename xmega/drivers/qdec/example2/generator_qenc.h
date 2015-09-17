/**
 * \file
 *
 * \brief XMEGA E Quadrature decoder test signal generation header file. It
 *        generates a quadrature encoded signal.
 *        This file contains the function prototypes and Macros.
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

#ifndef GENERATOR_QENC_H
#define GENERATOR_QENC_H

/**
 * \brief Ggenerates a quadrature encoder signals on port pins
 *
 * \param port       The port used to generate signals
 * \param pins_base  First pin number to be used for QDPH0 input from port
 *                   pins_base+1 for QDPH90 and pins_base+2 for QDINDX
 * \param revolution Number of position for a revolution
 * \param freq       The frequency of generated signal (mHz)
 * \param dir        The direction of generated signal
 */
void generator_qenc_enable(PORT_t *port, uint8_t pins_base,
		volatile void *timer, uint8_t revolution, uint32_t freq, bool dir);

/**
 * \brief Changes the speed of quadrature encoder generator
 *
 * \param freq       The frequency of generated signal (mHz)
 */
void generator_qenc_set_freq(uint32_t freq);

/**
 * \brief Changes the direction of quadrature encoder generator
 *
 * \param dir        The direction of generated signal
 */
void generator_qenc_set_direction(bool dir);

#endif /* GENERATOR_QENC_H */
