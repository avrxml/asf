/**
 * \file
 *
 * \brief Atmel Development Board Clock Configuration (ASF)
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
 */
#ifndef CONF_CLOCK_H_INCLUDED
#define CONF_CLOCK_H_INCLUDED

#include <board.h>

#if (BOARD == EVK1100)
#   define USE_PLL
#   define OSC      (0)
#   define PLL_MUL  (10)
#elif (BOARD == EVK1101)
#   define USE_PLL
#   define OSC      (0)
#   define PLL_MUL  (9)
#elif (BOARD == EVK1104)
#   define USE_PLL
#   define OSC      (0)
#   define PLL_MUL  (9)     /* for some reason we can't use 66 Mhz */
#elif (BOARD == EVK1105)
#   define USE_PLL
#   define OSC      (1)
#   define PLL_MUL  (10)
#endif

/**
 * @name System Clock Frequencies
 * @{
 */
#if (OSC == 0)
#   define FOSC     (FOSC0) /* 12 MHz */
#else
#   define FOSC     (FOSC1) /* 11.2896 MHz */
#endif

#ifdef USE_PLL
#   define FMCK_HZ  ((FOSC * (PLL_MUL + 1)) / 2)
#else
#   define FMCK_HZ  (FOSC)
#endif

#define FCPU_HZ     (FMCK_HZ)
#define FHSB_HZ     (FCPU_HZ)
#define FPBB_HZ     (FMCK_HZ)
#define FPBA_HZ     (FMCK_HZ)
//! @}

#define F_CPU       (FPBA_HZ)

#endif /* CONF_CLOCK_H_INCLUDED */
