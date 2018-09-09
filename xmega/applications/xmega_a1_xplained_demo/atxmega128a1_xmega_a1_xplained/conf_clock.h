/**
 * \file
 *
 * \brief Chip-specific clock configuration
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

//! Use the 32 MHz RC oscillator as PLL reference
#define CONFIG_PLL0_SOURCE      PLL_SRC_RC32MHZ
//! Divide by 4 (and multiply by 8) to produce a 64 MHz clock
#define CONFIG_PLL0_DIV         4
//! Multiply by 8 (and divide by 4) to produce a 64 MHz clock
#define CONFIG_PLL0_MUL         8

//! Use the PLL as system clock source
#define CONFIG_SYSCLK_SOURCE    SYSCLK_SRC_PLL

//! Run the CLKper4 on 64 MHz
#define CONFIG_SYSCLK_PSADIV    SYSCLK_PSADIV_1
//! Run the CLKper2 on 64 MHz and CLKper on 32 MHz (CPU speed)
#define CONFIG_SYSCLK_PSBCDIV   SYSCLK_PSBCDIV_1_2

#endif /* CONF_CLOCK_H */
