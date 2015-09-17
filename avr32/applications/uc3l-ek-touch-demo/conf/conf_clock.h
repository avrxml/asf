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
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#ifndef CONF_CLOCK_H_INCLUDED
#define CONF_CLOCK_H_INCLUDED

/**
 * @name System Clock Configuration
 *
 * @brief
 * The QDebug protocol USART clock settings are available in QDebug/Serial.h.
 * When modifying the EXAMPLE_CPUCLK_HZ clock, care must be taken to modify
 * the TARGET_PBA_FREQ_HZ and TARGET_CPU_FREQ_HZ defines in the Serial.h file
 * for the QDebug protocol to work correctly.
 *
 * @def EXAMPLE_CPUCLK_HZ
 * @brief MCU Clock Configuration (Hertz)
 *
 * @def EXAMPLE_TARGET_DFLL_FREQ_HZ
 * @brief DFLL target frequency (Hertz)
 *
 * @def EXAMPLE_TARGET_MCUCLK_FREQ_HZ
 * @brief MCU clock target frequency (Hertz)
 *
 * @def EXAMPLE_TARGET_PBACLK_FREQ_HZ
 * @brief PBA clock target frequency (Hertz)
 *
 * @{
 */
#define EXAMPLE_CPUCLK_HZ               (48000000UL)
#define EXAMPLE_TARGET_DFLL_FREQ_HZ     (96000000UL)
#define EXAMPLE_TARGET_MCUCLK_FREQ_HZ   (48000000UL)
#define EXAMPLE_TARGET_PBACLK_FREQ_HZ   (48000000UL)
//! @}

#define F_CPU                           (EXAMPLE_CPUCLK_HZ)

#endif /* CONF_CLOCK_H_INCLUDED */
