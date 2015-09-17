/*****************************************************************************
 *
 * \file
 *
 * \brief 16-bit and 32-bit pseudo-random functions for the AVR32 UC3
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

static int seed = 123456789;

// Get the reference's value to compute a pseudo-random number
void dsp_op_srand(int new_seed)
{
  if (new_seed == 0)
    new_seed = 1;
  seed = new_seed;
}

#if defined(FORCE_ALL_GENERICS) || \
    defined(FORCE_GENERIC_OP16_RAND) || \
    !defined(TARGET_SPECIFIC_OP16_RAND)

// Generate a 16-bit pseudo-random number from a reference's value
dsp16_t dsp16_op_rand(void)
{
  // Park and Miller Minimal Standard
  seed = 16807*seed;
  return (dsp16_t) (seed >> (DSP16_QA-1)) & (((U16) -1) >> (DSP16_QA-1));
}

#endif

#if defined(FORCE_ALL_GENERICS) || \
    defined(FORCE_GENERIC_OP32_RAND) || \
    !defined(TARGET_SPECIFIC_OP32_RAND)

// Generate a 32-bit pseudo-random number from a reference's value
dsp32_t dsp32_op_rand(void)
{
  // Park and Miller Minimal Standard
  seed = 16807*seed;
  return (dsp32_t) (seed >> (DSP32_QA-1)) & (((U32) -1) >> (DSP16_QA-1));
}

#endif
