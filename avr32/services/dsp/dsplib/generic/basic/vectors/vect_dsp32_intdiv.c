/*****************************************************************************
 *
 * \file
 *
 * \brief generic 32-bit vector division with an integer function
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
#include "preprocessor.h"

#if defined(FORCE_ALL_GENERICS) || \
    defined(FORCE_GENERIC_VECT32_INTDIV) || \
    !defined(TARGET_SPECIFIC_VECT32_INTDIV)

#define DSP32_INTDIVISION(x_num, data) \
  vect1[x_num] = vect2[x_num] / (dsp32_t) integer;

/*********************************************************************************************
 * This function is the ending function of the division with an integer. It is used to divide the last items of a vector.
 *********************************************************************************************/
#define DSP32_INTDIV_KERNEL_X_FCT(x_num, data)    \
static void TPASTE2(dsp32_vect_intdiv_kernel_x, x_num)(dsp32_t *vect1, dsp32_t *vect2, int integer) \
{ \
  MREPEAT(x_num, DSP32_INTDIVISION, ""); \
}
/*********************************************************************************************/

DSP32_INTDIV_KERNEL_X_FCT(0, "")
DSP32_INTDIV_KERNEL_X_FCT(1, "")
DSP32_INTDIV_KERNEL_X_FCT(2, "")
DSP32_INTDIV_KERNEL_X_FCT(3, "")

void dsp32_vect_intdiv(dsp32_t *vect1, dsp32_t *vect2, int size, int integer)
{
  typedef void (*intdiv_kernel_opti_t)(dsp32_t *, dsp32_t *, int);
  static const intdiv_kernel_opti_t intdiv_end_kernel_opti[4] = {
    dsp32_vect_intdiv_kernel_x0,
    dsp32_vect_intdiv_kernel_x1,
    dsp32_vect_intdiv_kernel_x2,
    dsp32_vect_intdiv_kernel_x3
  };
  int n;

  for(n=0; n<size-3; n+=4)
  {
    DSP32_INTDIVISION(n, "");
    DSP32_INTDIVISION(n+1, "");
    DSP32_INTDIVISION(n+2, "");
    DSP32_INTDIVISION(n+3, "");
  }

  // Jump on different functions depending on the length of the vectors to compute
  intdiv_end_kernel_opti[size&0x3](&vect1[n], &vect2[n], integer);
}

#endif
