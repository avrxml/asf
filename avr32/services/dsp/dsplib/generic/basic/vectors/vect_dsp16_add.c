/*****************************************************************************
 *
 * \file
 *
 * \brief generic 16-bit addition function
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
    defined(FORCE_GENERIC_VECT16_ADD) || \
    !defined(TARGET_SPECIFIC_VECT16_ADD)

#define DSP16_ADDITION(x_num, data) \
  vect1[x_num] = vect2[x_num] + vect3[x_num];

/*********************************************************************************************
 * This function is the ending function of the addition. It is used to add the last items of a vector.
 *********************************************************************************************/
#define DSP16_ADD_KERNEL_X_FCT(x_num, data)    \
static void TPASTE2(dsp16_vect_add_kernel_x, x_num)(dsp16_t *vect1, dsp16_t *vect2, dsp16_t *vect3) \
{ \
  MREPEAT(x_num, DSP16_ADDITION, ""); \
}
/*********************************************************************************************/

DSP16_ADD_KERNEL_X_FCT(0, "")
DSP16_ADD_KERNEL_X_FCT(1, "")
DSP16_ADD_KERNEL_X_FCT(2, "")
DSP16_ADD_KERNEL_X_FCT(3, "")

void dsp16_vect_add(dsp16_t *vect1, dsp16_t *vect2, dsp16_t *vect3, int size)
{
  typedef void (*add_kernel_opti_t)(dsp16_t *, dsp16_t *, dsp16_t *);
  static const add_kernel_opti_t add_end_kernel_opti[4] = {
    dsp16_vect_add_kernel_x0,
    dsp16_vect_add_kernel_x1,
    dsp16_vect_add_kernel_x2,
    dsp16_vect_add_kernel_x3
  };
  int n;

  for(n=0; n<size-3; n+=4)
  {
    vect1[n] = vect2[n] + vect3[n];
    vect1[n+1] = vect2[n+1] + vect3[n+1];
    vect1[n+2] = vect2[n+2] + vect3[n+2];
    vect1[n+3] = vect2[n+3] + vect3[n+3];
  }

  // Jump on different functions depending on the length of the vectors to compute
  add_end_kernel_opti[size&0x3](&vect1[n], &vect2[n], &vect3[n]);
}

#endif
