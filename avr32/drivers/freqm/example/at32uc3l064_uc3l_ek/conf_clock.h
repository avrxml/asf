/**
 * \file
 *
 * \brief Clock configuration
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
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#ifndef CONF_CLOCK_H
#define CONF_CLOCK_H

// DFLL target frequency, in Hz
#define EXAMPLE_TARGET_DFLL_FREQ_HZ   96000000
// MCU clock target frequency, in Hz
#define EXAMPLE_TARGET_MCUCLK_FREQ_HZ 12000000
// PBA clock target frequency, in Hz
#define EXAMPLE_TARGET_PBACLK_FREQ_HZ 12000000
#define FPBA                          EXAMPLE_TARGET_PBACLK_FREQ_HZ

// System clock source
#define CONFIG_SYSCLK_SOURCE          SYSCLK_SRC_DFLL
// CPU, PBA, PBB equals main clock divided by 8
#define CONFIG_SYSCLK_CPU_DIV         3
#define CONFIG_SYSCLK_PBA_DIV         3
#define CONFIG_SYSCLK_PBB_DIV         3

// GCLK source clock frequency
#define GC_SRC_FREQ_HZ                AVR32_SCIF_RCOSC_FREQUENCY
// GCLK source clock
#define CONFIG_DFLL0_SOURCE         GENCLK_SRC_RCSYS
// GCLK division factor
#define CONFIG_DFLL0_DIV              0

// DFLL0 multiply factor
#define CONFIG_DFLL0_MUL            (EXAMPLE_TARGET_DFLL_FREQ_HZ / GC_SRC_FREQ_HZ)


#endif /* CONF_CLOCK_H */
