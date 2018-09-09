/*****************************************************************************
 *
 * \file
 *
 * \brief 16-bit and 32-bit fixed-point cosine kernel functions or the AVR32 UC3
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
 ******************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
