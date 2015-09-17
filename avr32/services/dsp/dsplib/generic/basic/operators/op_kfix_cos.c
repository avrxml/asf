/*****************************************************************************
 *
 * \file
 *
 * \brief 16-bit and 32-bit fixed-point cosine kernel functions or the AVR32 UC3
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
 ******************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


#include "dsp.h"

// Input angle from -pi/4 to pi/4
dsp16_t dsp16_op_kernel_cosfix(dsp16_t angle)
{
  // Coefficients of the 4th order Tchebychev polynomial to calculate a cosine.
  const dsp16_t c0 = DSP16_Q(1.);
  const dsp16_t c2 = DSP16_Q(0.61651611328125);
  const dsp16_t c4 = DSP16_Q(0.1231689453125);
  S32 z, suma;

  // Computation of the polynomial
  z = (angle*angle) >> (DSP16_QB - 3);
  suma = c2 - ((z*c4) >> (DSP16_QB + 1));

  return (c0 - ((z*suma) >> DSP16_QB));
}

// Input angle from -pi/4 to pi/4
dsp32_t dsp32_op_kernel_cosfix(dsp32_t angle)
{
  // Coefficients of the 12th order Tchebychev polynomial to calculate a cosine.
  const dsp32_t one = DSP32_Q(1.);
  const dsp32_t half = DSP32_Q(0.61685027507);    //0.5*(pi^2/8);
  const dsp32_t C1 = DSP32_Q(0.50733903092);      //(4.1666667908e-02*pi^2)*(pi^2/8);
  const dsp32_t C2 = DSP32_Q(-0.16690785015);     //(-1.3888889225e-03*pi^4)*(pi^2/8);
  const dsp32_t C3 = DSP32_Q(0.02941632920);      //(2.4801587642e-05*pi^6)*(pi^2/8);
  const dsp32_t C4 = DSP32_Q(-0.00322586085);     //(-2.7557314297e-07*pi^8)*(pi^2/8);
  const dsp32_t C5 = DSP32_Q(0.00024118485);      //(2.0875723372e-09*pi^10)*(pi^2/8);
  const dsp32_t C6 = DSP32_Q(-0.00001295308);     //(-1.1359647598e-11*pi^12)*(pi^2/8);
  long long z, suma, produ;

  // Computation of the polynomial
  z = (long long) angle;
  z = (z*z) >> DSP32_QB;

  produ = (z*C6) >> DSP32_QB;

  suma = C5 + produ;
  produ = (z*suma) >> DSP32_QB;
  suma = C4 + produ;
  produ = (z*suma) >> DSP32_QB;
  suma = C3 + produ;
  produ = (z*suma) >> DSP32_QB;
  suma = C2 + produ;
  produ = (z*suma) >> DSP32_QB;
  suma = C1 + produ;
  produ = (z*suma) >> DSP32_QB;
  produ = z*produ;

  return (one - (((half*z - produ)) >> (DSP32_QB - 3)));
}
