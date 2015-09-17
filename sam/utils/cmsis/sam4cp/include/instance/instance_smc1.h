/**
 * \file
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

#ifndef _SAM4CP_SMC1_INSTANCE_
#define _SAM4CP_SMC1_INSTANCE_

/* ========== Register definition for SMC1 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
  #define REG_SMC1_SETUP0                  (0x4801C000U) /**< \brief (SMC1) SMC Setup Register (CS_number = 0) */
  #define REG_SMC1_PULSE0                  (0x4801C004U) /**< \brief (SMC1) SMC Pulse Register (CS_number = 0) */
  #define REG_SMC1_CYCLE0                  (0x4801C008U) /**< \brief (SMC1) SMC Cycle Register (CS_number = 0) */
  #define REG_SMC1_MODE0                   (0x4801C00CU) /**< \brief (SMC1) SMC Mode Register (CS_number = 0) */
  #define REG_SMC1_SETUP1                  (0x4801C010U) /**< \brief (SMC1) SMC Setup Register (CS_number = 1) */
  #define REG_SMC1_PULSE1                  (0x4801C014U) /**< \brief (SMC1) SMC Pulse Register (CS_number = 1) */
  #define REG_SMC1_CYCLE1                  (0x4801C018U) /**< \brief (SMC1) SMC Cycle Register (CS_number = 1) */
  #define REG_SMC1_MODE1                   (0x4801C01CU) /**< \brief (SMC1) SMC Mode Register (CS_number = 1) */
  #define REG_SMC1_SETUP2                  (0x4801C020U) /**< \brief (SMC1) SMC Setup Register (CS_number = 2) */
  #define REG_SMC1_PULSE2                  (0x4801C024U) /**< \brief (SMC1) SMC Pulse Register (CS_number = 2) */
  #define REG_SMC1_CYCLE2                  (0x4801C028U) /**< \brief (SMC1) SMC Cycle Register (CS_number = 2) */
  #define REG_SMC1_MODE2                   (0x4801C02CU) /**< \brief (SMC1) SMC Mode Register (CS_number = 2) */
  #define REG_SMC1_SETUP3                  (0x4801C030U) /**< \brief (SMC1) SMC Setup Register (CS_number = 3) */
  #define REG_SMC1_PULSE3                  (0x4801C034U) /**< \brief (SMC1) SMC Pulse Register (CS_number = 3) */
  #define REG_SMC1_CYCLE3                  (0x4801C038U) /**< \brief (SMC1) SMC Cycle Register (CS_number = 3) */
  #define REG_SMC1_MODE3                   (0x4801C03CU) /**< \brief (SMC1) SMC Mode Register (CS_number = 3) */
  #define REG_SMC1_OCMS                    (0x4801C080U) /**< \brief (SMC1) SMC OCMS MODE Register */
  #define REG_SMC1_KEY1                    (0x4801C084U) /**< \brief (SMC1) SMC OCMS KEY1 Register */
  #define REG_SMC1_KEY2                    (0x4801C088U) /**< \brief (SMC1) SMC OCMS KEY2 Register */
  #define REG_SMC1_WPMR                    (0x4801C0E4U) /**< \brief (SMC1) SMC Write Protect Mode Register */
  #define REG_SMC1_WPSR                    (0x4801C0E8U) /**< \brief (SMC1) SMC Write Protect Status Register */
#else
  #define REG_SMC1_SETUP0 (*(__IO uint32_t*)0x4801C000U) /**< \brief (SMC1) SMC Setup Register (CS_number = 0) */
  #define REG_SMC1_PULSE0 (*(__IO uint32_t*)0x4801C004U) /**< \brief (SMC1) SMC Pulse Register (CS_number = 0) */
  #define REG_SMC1_CYCLE0 (*(__IO uint32_t*)0x4801C008U) /**< \brief (SMC1) SMC Cycle Register (CS_number = 0) */
  #define REG_SMC1_MODE0  (*(__IO uint32_t*)0x4801C00CU) /**< \brief (SMC1) SMC Mode Register (CS_number = 0) */
  #define REG_SMC1_SETUP1 (*(__IO uint32_t*)0x4801C010U) /**< \brief (SMC1) SMC Setup Register (CS_number = 1) */
  #define REG_SMC1_PULSE1 (*(__IO uint32_t*)0x4801C014U) /**< \brief (SMC1) SMC Pulse Register (CS_number = 1) */
  #define REG_SMC1_CYCLE1 (*(__IO uint32_t*)0x4801C018U) /**< \brief (SMC1) SMC Cycle Register (CS_number = 1) */
  #define REG_SMC1_MODE1  (*(__IO uint32_t*)0x4801C01CU) /**< \brief (SMC1) SMC Mode Register (CS_number = 1) */
  #define REG_SMC1_SETUP2 (*(__IO uint32_t*)0x4801C020U) /**< \brief (SMC1) SMC Setup Register (CS_number = 2) */
  #define REG_SMC1_PULSE2 (*(__IO uint32_t*)0x4801C024U) /**< \brief (SMC1) SMC Pulse Register (CS_number = 2) */
  #define REG_SMC1_CYCLE2 (*(__IO uint32_t*)0x4801C028U) /**< \brief (SMC1) SMC Cycle Register (CS_number = 2) */
  #define REG_SMC1_MODE2  (*(__IO uint32_t*)0x4801C02CU) /**< \brief (SMC1) SMC Mode Register (CS_number = 2) */
  #define REG_SMC1_SETUP3 (*(__IO uint32_t*)0x4801C030U) /**< \brief (SMC1) SMC Setup Register (CS_number = 3) */
  #define REG_SMC1_PULSE3 (*(__IO uint32_t*)0x4801C034U) /**< \brief (SMC1) SMC Pulse Register (CS_number = 3) */
  #define REG_SMC1_CYCLE3 (*(__IO uint32_t*)0x4801C038U) /**< \brief (SMC1) SMC Cycle Register (CS_number = 3) */
  #define REG_SMC1_MODE3  (*(__IO uint32_t*)0x4801C03CU) /**< \brief (SMC1) SMC Mode Register (CS_number = 3) */
  #define REG_SMC1_OCMS   (*(__IO uint32_t*)0x4801C080U) /**< \brief (SMC1) SMC OCMS MODE Register */
  #define REG_SMC1_KEY1   (*(__O  uint32_t*)0x4801C084U) /**< \brief (SMC1) SMC OCMS KEY1 Register */
  #define REG_SMC1_KEY2   (*(__O  uint32_t*)0x4801C088U) /**< \brief (SMC1) SMC OCMS KEY2 Register */
  #define REG_SMC1_WPMR   (*(__IO uint32_t*)0x4801C0E4U) /**< \brief (SMC1) SMC Write Protect Mode Register */
  #define REG_SMC1_WPSR   (*(__I  uint32_t*)0x4801C0E8U) /**< \brief (SMC1) SMC Write Protect Status Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#endif /* _SAM4CP_SMC1_INSTANCE_ */
