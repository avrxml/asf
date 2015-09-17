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

#ifndef _SAM4CP_SLCDC_INSTANCE_
#define _SAM4CP_SLCDC_INSTANCE_

/* ========== Register definition for SLCDC peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
  #define REG_SLCDC_CR                      (0x4003C000U) /**< \brief (SLCDC) SLCDC Control Register */
  #define REG_SLCDC_MR                      (0x4003C004U) /**< \brief (SLCDC) SLCDC Mode Register */
  #define REG_SLCDC_FRR                     (0x4003C008U) /**< \brief (SLCDC) SLCDC Frame Rate Register */
  #define REG_SLCDC_DR                      (0x4003C00CU) /**< \brief (SLCDC) SLCDC Display Register */
  #define REG_SLCDC_SR                      (0x4003C010U) /**< \brief (SLCDC) SLCDC Status Register */
  #define REG_SLCDC_IER                     (0x4003C020U) /**< \brief (SLCDC) SLCDC Interrupt Enable Register */
  #define REG_SLCDC_IDR                     (0x4003C024U) /**< \brief (SLCDC) SLCDC Interrupt Disable Register */
  #define REG_SLCDC_IMR                     (0x4003C028U) /**< \brief (SLCDC) SLCDC Interrupt Mask Register */
  #define REG_SLCDC_ISR                     (0x4003C02CU) /**< \brief (SLCDC) SLCDC Interrupt Status Register */
  #define REG_SLCDC_SMR0                    (0x4003C030U) /**< \brief (SLCDC) SLCDC Segment Map Register 0 */
  #define REG_SLCDC_SMR1                    (0x4003C034U) /**< \brief (SLCDC) SLCDC Segment Map Register 1 */
  #define REG_SLCDC_WPMR                    (0x4003C0E4U) /**< \brief (SLCDC) SLCDC Write Protect Mode Register */
  #define REG_SLCDC_WPSR                    (0x4003C0E8U) /**< \brief (SLCDC) SLCDC Write Protect Status Register */
  #define REG_SLCDC_LMEMR0                  (0x4003C200U) /**< \brief (SLCDC) SLCDC LSB Memory Register (com = 0) */
  #define REG_SLCDC_MMEMR0                  (0x4003C204U) /**< \brief (SLCDC) SLCDC MSB Memory Register (com = 0) */
  #define REG_SLCDC_LMEMR1                  (0x4003C208U) /**< \brief (SLCDC) SLCDC LSB Memory Register (com = 1) */
  #define REG_SLCDC_MMEMR1                  (0x4003C20CU) /**< \brief (SLCDC) SLCDC MSB Memory Register (com = 1) */
  #define REG_SLCDC_LMEMR2                  (0x4003C210U) /**< \brief (SLCDC) SLCDC LSB Memory Register (com = 2) */
  #define REG_SLCDC_MMEMR2                  (0x4003C214U) /**< \brief (SLCDC) SLCDC MSB Memory Register (com = 2) */
  #define REG_SLCDC_LMEMR3                  (0x4003C218U) /**< \brief (SLCDC) SLCDC LSB Memory Register (com = 3) */
  #define REG_SLCDC_MMEMR3                  (0x4003C21CU) /**< \brief (SLCDC) SLCDC MSB Memory Register (com = 3) */
  #define REG_SLCDC_LMEMR4                  (0x4003C220U) /**< \brief (SLCDC) SLCDC LSB Memory Register (com = 4) */
  #define REG_SLCDC_MMEMR4                  (0x4003C224U) /**< \brief (SLCDC) SLCDC MSB Memory Register (com = 4) */
  #define REG_SLCDC_LMEMR5                  (0x4003C228U) /**< \brief (SLCDC) SLCDC LSB Memory Register (com = 5) */
  #define REG_SLCDC_MMEMR5                  (0x4003C22CU) /**< \brief (SLCDC) SLCDC MSB Memory Register (com = 5) */
#else
  #define REG_SLCDC_CR     (*(__O  uint32_t*)0x4003C000U) /**< \brief (SLCDC) SLCDC Control Register */
  #define REG_SLCDC_MR     (*(__IO uint32_t*)0x4003C004U) /**< \brief (SLCDC) SLCDC Mode Register */
  #define REG_SLCDC_FRR    (*(__IO uint32_t*)0x4003C008U) /**< \brief (SLCDC) SLCDC Frame Rate Register */
  #define REG_SLCDC_DR     (*(__IO uint32_t*)0x4003C00CU) /**< \brief (SLCDC) SLCDC Display Register */
  #define REG_SLCDC_SR     (*(__I  uint32_t*)0x4003C010U) /**< \brief (SLCDC) SLCDC Status Register */
  #define REG_SLCDC_IER    (*(__O  uint32_t*)0x4003C020U) /**< \brief (SLCDC) SLCDC Interrupt Enable Register */
  #define REG_SLCDC_IDR    (*(__O  uint32_t*)0x4003C024U) /**< \brief (SLCDC) SLCDC Interrupt Disable Register */
  #define REG_SLCDC_IMR    (*(__O  uint32_t*)0x4003C028U) /**< \brief (SLCDC) SLCDC Interrupt Mask Register */
  #define REG_SLCDC_ISR    (*(__I  uint32_t*)0x4003C02CU) /**< \brief (SLCDC) SLCDC Interrupt Status Register */
  #define REG_SLCDC_SMR0   (*(__IO uint32_t*)0x4003C030U) /**< \brief (SLCDC) SLCDC Segment Map Register 0 */
  #define REG_SLCDC_SMR1   (*(__IO uint32_t*)0x4003C034U) /**< \brief (SLCDC) SLCDC Segment Map Register 1 */
  #define REG_SLCDC_WPMR   (*(__IO uint32_t*)0x4003C0E4U) /**< \brief (SLCDC) SLCDC Write Protect Mode Register */
  #define REG_SLCDC_WPSR   (*(__I  uint32_t*)0x4003C0E8U) /**< \brief (SLCDC) SLCDC Write Protect Status Register */
  #define REG_SLCDC_LMEMR0 (*(__IO uint32_t*)0x4003C200U) /**< \brief (SLCDC) SLCDC LSB Memory Register (com = 0) */
  #define REG_SLCDC_MMEMR0 (*(__IO uint32_t*)0x4003C204U) /**< \brief (SLCDC) SLCDC MSB Memory Register (com = 0) */
  #define REG_SLCDC_LMEMR1 (*(__IO uint32_t*)0x4003C208U) /**< \brief (SLCDC) SLCDC LSB Memory Register (com = 1) */
  #define REG_SLCDC_MMEMR1 (*(__IO uint32_t*)0x4003C20CU) /**< \brief (SLCDC) SLCDC MSB Memory Register (com = 1) */
  #define REG_SLCDC_LMEMR2 (*(__IO uint32_t*)0x4003C210U) /**< \brief (SLCDC) SLCDC LSB Memory Register (com = 2) */
  #define REG_SLCDC_MMEMR2 (*(__IO uint32_t*)0x4003C214U) /**< \brief (SLCDC) SLCDC MSB Memory Register (com = 2) */
  #define REG_SLCDC_LMEMR3 (*(__IO uint32_t*)0x4003C218U) /**< \brief (SLCDC) SLCDC LSB Memory Register (com = 3) */
  #define REG_SLCDC_MMEMR3 (*(__IO uint32_t*)0x4003C21CU) /**< \brief (SLCDC) SLCDC MSB Memory Register (com = 3) */
  #define REG_SLCDC_LMEMR4 (*(__IO uint32_t*)0x4003C220U) /**< \brief (SLCDC) SLCDC LSB Memory Register (com = 4) */
  #define REG_SLCDC_MMEMR4 (*(__IO uint32_t*)0x4003C224U) /**< \brief (SLCDC) SLCDC MSB Memory Register (com = 4) */
  #define REG_SLCDC_LMEMR5 (*(__IO uint32_t*)0x4003C228U) /**< \brief (SLCDC) SLCDC LSB Memory Register (com = 5) */
  #define REG_SLCDC_MMEMR5 (*(__IO uint32_t*)0x4003C22CU) /**< \brief (SLCDC) SLCDC MSB Memory Register (com = 5) */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#endif /* _SAM4CP_SLCDC_INSTANCE_ */
