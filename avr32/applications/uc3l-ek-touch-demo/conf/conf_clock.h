/**
 * \file
 *
 * \brief Atmel Development Board Clock Configuration (ASF)
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
