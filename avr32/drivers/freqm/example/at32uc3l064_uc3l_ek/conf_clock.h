/**
 * \file
 *
 * \brief Clock configuration
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
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
