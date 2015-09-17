/*****************************************************************************
 *
 * \file
 *
 * \brief 16-bit and 32-bit fixed-point sine kernel functions or the AVR32 UC3
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
dsp16_t dsp16_op_kernel_sinfix(dsp16_t angle)
{
  // Coefficients of the 5th order Tchebychev polynomial to calculate a sine.
  const dsp16_t s1 = DSP16_Q(0.785369873046875);
  const dsp16_t s3 = DSP16_Q(0.322784423828125);
  const dsp16_t s5 = DSP16_Q(0.03875732421875);
  S32 z, suma;

  // Computation of the polynomial
  z = (angle*angle) >> (DSP16_QB - 3);
  suma = s3 - ((z*s5) >> (DSP16_QB + 1));
  suma = s1 - ((z*suma) >> (DSP16_QB + 1));

  return (angle*suma) >> (DSP16_QB - 2);
}

// Input angle from -pi/4 to pi/4
dsp32_t dsp32_op_kernel_sinfix(dsp32_t angle)
{
  // Coefficients of the 13th order Tchebychev polynomial to calculate a sinus.
  const dsp32_t S0 = DSP32_Q(0.78539816340);    //pi/4;
  const dsp32_t S1 = DSP32_Q(-0.64596411675);   //-1.6666667163e-01*pi^3/8;
  const dsp32_t S2 = DSP32_Q(0.31877052162);    //8.3333337680e-03*pi^5/8;
  const dsp32_t S3 = DSP32_Q(-0.07490806720);   //-1.9841270114e-04*pi^7/8;
  const dsp32_t S4 = DSP32_Q(0.01026823400);    //2.7557314297e-06*pi^9/8;
  const dsp32_t S5 = DSP32_Q(-0.00092125426);   //-2.5050759689e-08*pi^11/8;
  const dsp32_t S6 = DSP32_Q(0.00005769937);    //1.5896910177e-10*pi^13/8;
  long long z, v, suma, produ;

  // Computation of the polynomial
  z = (long long) angle;
  z = (z*z) >> DSP32_QB;
  v = (long long) angle;
  v = (z*v) >> DSP32_QB;

  produ = (z*S6) >> DSP32_QB;
  suma = S5 + produ;
  produ = (z*suma) >> DSP32_QB;
  suma = S4 + produ;
  produ = (z*suma) >> DSP32_QB;
  suma = S3 + produ;
  produ = (z*suma) >> DSP32_QB;
  suma = S2 + produ;

  produ = (z*suma) >> DSP32_QB;
  suma = S1 + produ;
  produ = (v*suma) >> (DSP32_QB - 3);

  return ((S0*((long long) angle)) >> (DSP32_QB - 2)) + produ;
}
