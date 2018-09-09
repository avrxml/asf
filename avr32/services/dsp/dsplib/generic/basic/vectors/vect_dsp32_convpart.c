/*****************************************************************************
 *
 * \file
 *
 * \brief Generic 32-bit partial convolution function for the AVR32 UC3
 *
 * This file contains the code of the partial convolution.
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
    defined(FORCE_GENERIC_VECT32_CONVPART) || \
    !defined(TARGET_SPECIFIC_VECT32_CONVPART)

#define DSP32_SUM_ITEM(x, line)       sum += ((long long) pvect2[x])*((long long) pvect3[8 - x - 1]);

#define DSP32_SUM_ITEM_INIT(x, line)  sum += ((long long) *pvect2++)*((long long) *--pvect3);

#define DSP32_CONVPART_KERNEL_X_FCT(x_num, data)    \
static void TPASTE2(dsp32_vect_convpart_kernel_x, x_num)(dsp32_t *vect1, dsp32_t *vect2, int vect1_size, dsp32_t *vect3, int vect3_size) \
{ \
  int i, j; \
  long long sum; \
  dsp32_t *pvect3, *pvect2; \
\
  for(j=0; j<vect1_size; j++) \
    { \
      sum = 0; \
      pvect3 = &vect3[vect3_size]; \
      pvect2 = &vect2[j]; \
\
      MREPEAT(x_num, DSP32_SUM_ITEM_INIT, ) \
\
      for(i=x_num; i<vect3_size; i += 8) \
        { \
          pvect3 -= 8; \
          MREPEAT8(DSP32_SUM_ITEM, ) \
          pvect2 += 8; \
        } \
\
      vect1[j] = sum >> DSP32_QB; \
    } \
}

DSP32_CONVPART_KERNEL_X_FCT(0, )
DSP32_CONVPART_KERNEL_X_FCT(1, )
DSP32_CONVPART_KERNEL_X_FCT(2, )
DSP32_CONVPART_KERNEL_X_FCT(3, )
DSP32_CONVPART_KERNEL_X_FCT(4, )
DSP32_CONVPART_KERNEL_X_FCT(5, )
DSP32_CONVPART_KERNEL_X_FCT(6, )
DSP32_CONVPART_KERNEL_X_FCT(7, )

void dsp32_vect_convpart(dsp32_t *vect1, dsp32_t *vect2, int vect2_size, dsp32_t *vect3, int vect3_size)
{
  typedef void (*convpart_kernel_opti_t)(dsp32_t *, dsp32_t *, int, dsp32_t *, int);
  static const convpart_kernel_opti_t convpart_kernel_opti[8] = {
      dsp32_vect_convpart_kernel_x0,
      dsp32_vect_convpart_kernel_x1,
      dsp32_vect_convpart_kernel_x2,
      dsp32_vect_convpart_kernel_x3,
      dsp32_vect_convpart_kernel_x4,
      dsp32_vect_convpart_kernel_x5,
      dsp32_vect_convpart_kernel_x6,
      dsp32_vect_convpart_kernel_x7
    };

  // Jump on different functions depending on the length of the partial convolution to compute
  convpart_kernel_opti[vect3_size&0x7](vect1, vect2, vect2_size - vect3_size + 1, vect3, vect3_size);
}

#endif

