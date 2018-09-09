/*****************************************************************************
 *
 * \file
 *
 * \brief CONF_EBI EBI/SMC driver for AVR32 UC3.
 *
 * \note The values defined in this file are device-specific. See the device
 *       datasheet for further information.
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


#ifndef _CONF_EBI_H_
#define _CONF_EBI_H_

#include "compiler.h"
#include "board.h"

  #define  SMC_USE_NCS1
  #define  SMC_COMPONENT_CS1          "smc_peripheral.h"

  // ! Pin Mapping
  #define EBI_ADDR_0                  AVR32_EBI_ADDR_0
  #define EBI_ADDR_1                  AVR32_EBI_ADDR_1
  #define EBI_ADDR_2                  AVR32_EBI_ADDR_2
  #define EBI_ADDR_3                  AVR32_EBI_ADDR_3
  #define EBI_ADDR_4                  AVR32_EBI_ADDR_4
  #define EBI_ADDR_5                  AVR32_EBI_ADDR_5
  #define EBI_ADDR_6                  AVR32_EBI_ADDR_6
  #define EBI_ADDR_7                  AVR32_EBI_ADDR_7
  #define EBI_ADDR_8                  AVR32_EBI_ADDR_8
  #define EBI_ADDR_9                  AVR32_EBI_ADDR_9
  #define EBI_ADDR_10                 AVR32_EBI_ADDR_10
  #define EBI_ADDR_11                 AVR32_EBI_ADDR_11
  #define EBI_ADDR_12                 AVR32_EBI_ADDR_12
  #define EBI_ADDR_13                 AVR32_EBI_ADDR_13
  #define EBI_ADDR_14                 AVR32_EBI_ADDR_14
  #define EBI_ADDR_15                 AVR32_EBI_ADDR_15
  #define EBI_ADDR_16                 AVR32_EBI_ADDR_16
  #define EBI_ADDR_17                 AVR32_EBI_ADDR_17
  #define EBI_ADDR_18                 AVR32_EBI_ADDR_18
  #define EBI_ADDR_19                 AVR32_EBI_ADDR_19
#if   BOARD == EVK1100
  // Nothing to do
#elif BOARD == EVK1104
  #define EBI_ADDR_20                 AVR32_EBI_ADDR_20
  #define EBI_ADDR_21                 AVR32_EBI_ADDR_21
  #define EBI_ADDR_22                 AVR32_EBI_ADDR_22
#elif BOARD == EVK1105
  #error BOARD == EVK1105
  #define EBI_ADDR_20                 AVR32_EBI_ADDR_20_0
  #define EBI_ADDR_21                 AVR32_EBI_ADDR_21_0
  #define EBI_ADDR_22                 AVR32_EBI_ADDR_22_0
#else
  #error No known AVR32 board defined
#endif
  #define EBI_ADDR_23                 AVR32_EBI_ADDR_23


  #define EBI_DATA_0                  AVR32_EBI_DATA_0
  #define EBI_DATA_1                  AVR32_EBI_DATA_1
  #define EBI_DATA_2                  AVR32_EBI_DATA_2
  #define EBI_DATA_3                  AVR32_EBI_DATA_3
  #define EBI_DATA_4                  AVR32_EBI_DATA_4
  #define EBI_DATA_5                  AVR32_EBI_DATA_5
  #define EBI_DATA_6                  AVR32_EBI_DATA_6
  #define EBI_DATA_7                  AVR32_EBI_DATA_7
  #define EBI_DATA_8                  AVR32_EBI_DATA_8
  #define EBI_DATA_9                  AVR32_EBI_DATA_9
  #define EBI_DATA_10                 AVR32_EBI_DATA_10
  #define EBI_DATA_11                 AVR32_EBI_DATA_11
  #define EBI_DATA_12                 AVR32_EBI_DATA_12
  #define EBI_DATA_13                 AVR32_EBI_DATA_13
  #define EBI_DATA_14                 AVR32_EBI_DATA_14
  #define EBI_DATA_15                 AVR32_EBI_DATA_15


  #define EBI_NWE0                    AVR32_EBI_NWE0_0
  #define EBI_NWE1                    AVR32_EBI_NWE1_0
  #define EBI_NWE3                    AVR32_EBI_NWE3_0
  #define EBI_NRD                     AVR32_EBI_NRD_0
  #define EBI_NWAIT                   AVR32_EBI_NWAIT_0
  #define EBI_NCS_0                   AVR32_EBI_NCS_0_0
  #define EBI_NCS_1                   AVR32_EBI_NCS_1
  #define EBI_NCS_2                   AVR32_EBI_NCS_2
  #define EBI_NCS_3                   AVR32_EBI_NCS_3

#endif  // _CONF_EBI_H_
