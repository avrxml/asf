/*****************************************************************************
 *
 * \file
 *
 * \brief 16-bit and 32-bit convolution functions for the AVR32 UC3
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

#if defined(FORCE_ALL_GENERICS) || \
    defined(FORCE_GENERIC_VECT16_CONV) || \
    !defined(TARGET_SPECIFIC_VECT16_CONV)

void dsp16_vect_conv(dsp16_t *vect1, dsp16_t *vect2, int vect2_size, dsp16_t *vect3, int vect3_size)
{
  if (vect2_size >= vect3_size)
    {
        // Zero-pads the beginning of the vector
      dsp16_vect_zeropad(vect1, vect3_size-1, vect3_size-1);
        // Copy the rest of the vector
      dsp16_vect_copy(&vect1[vect3_size-1], vect2, vect2_size);
        // Zero-pads the tail of the vector
      dsp16_vect_zeropad(&vect1[vect2_size+vect3_size-1], vect3_size-1, vect3_size-1);

      dsp16_vect_convpart(vect1, vect1, vect2_size + 2*(vect3_size - 1), vect3, vect3_size);
    }
  else
    {
        // Zero-pads the beginning of the vector
      dsp16_vect_zeropad(vect1, vect2_size-1, vect2_size-1);
        // Copy the rest of the vector
      dsp16_vect_copy(&vect1[vect2_size-1], vect3, vect3_size);
        // Zero-pads the tail of the vector
      dsp16_vect_zeropad(&vect1[vect3_size+vect2_size-1], vect2_size-1, vect2_size-1);

      dsp16_vect_convpart(vect1, vect1, vect3_size + 2*(vect2_size - 1), vect2, vect2_size);
    }
}

#endif

#if defined(FORCE_ALL_GENERICS) || \
    defined(FORCE_GENERIC_VECT32_CONV) || \
    !defined(TARGET_SPECIFIC_VECT32_CONV)

void dsp32_vect_conv(dsp32_t *vect1, dsp32_t *vect2, int vect2_size, dsp32_t *vect3, int vect3_size)
{
  if (vect2_size >= vect3_size)
    {
        // Zero-pads the beginning of the vector
      dsp32_vect_zeropad(vect1, vect3_size-1, vect3_size-1);
        // Copy the rest of the vector
      dsp32_vect_copy(&vect1[vect3_size-1], vect2, vect2_size);
        // Zero-pads the tail of the vector
      dsp32_vect_zeropad(&vect1[vect2_size+vect3_size-1], vect3_size-1, vect3_size-1);

      dsp32_vect_convpart(vect1, vect1, vect2_size + 2*(vect3_size - 1), vect3, vect3_size);
    }
  else
    {
        // Zero-pads the beginning of the vector
      dsp32_vect_zeropad(vect1, vect2_size-1, vect2_size-1);
        // Copy the rest of the vector
      dsp32_vect_copy(&vect1[vect2_size-1], vect3, vect3_size);
        // Zero-pads the tail of the vector
      dsp32_vect_zeropad(&vect1[vect3_size+vect2_size-1], vect2_size-1, vect2_size-1);

      dsp32_vect_convpart(vect1, vect1, vect3_size + 2*(vect2_size - 1), vect2, vect2_size);
    }
}

#endif
