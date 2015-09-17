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
#ifndef _SAM4C_MATRIX0_INSTANCE_
#define _SAM4C_MATRIX0_INSTANCE_

/* ========== Register definition for MATRIX0 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
  #define REG_MATRIX0_MCFG                        (0x400E0200U) /**< \brief (MATRIX0) Master Configuration Register */
  #define REG_MATRIX0_SCFG                        (0x400E0240U) /**< \brief (MATRIX0) Slave Configuration Register */
  #define REG_MATRIX0_PRAS0                       (0x400E0280U) /**< \brief (MATRIX0) Priority Register A for Slave 0 */
  #define REG_MATRIX0_PRAS1                       (0x400E0288U) /**< \brief (MATRIX0) Priority Register A for Slave 1 */
  #define REG_MATRIX0_PRAS2                       (0x400E0290U) /**< \brief (MATRIX0) Priority Register A for Slave 2 */
  #define REG_MATRIX0_PRAS3                       (0x400E0298U) /**< \brief (MATRIX0) Priority Register A for Slave 3 */
  #define REG_MATRIX0_PRAS4                       (0x400E02A0U) /**< \brief (MATRIX0) Priority Register A for Slave 4 */
  #define REG_MATRIX0_PRAS5                       (0x400E02A8U) /**< \brief (MATRIX0) Priority Register A for Slave 5 */
  #define REG_MATRIX0_PRAS6                       (0x400E02B0U) /**< \brief (MATRIX0) Priority Register A for Slave 6 */
  #define REG_MATRIX0_PRAS7                       (0x400E02B8U) /**< \brief (MATRIX0) Priority Register A for Slave 7 */
  #define REG_MATRIX0_PRAS8                       (0x400E02BCU) /**< \brief (MATRIX0) Priority Register A for Slave 8 */
  #define REG_MATRIX0_SYSIO                       (0x400E0314U) /**< \brief (MATRIX0) System I/O Configuration Register */
  #define REG_MATRIX0_SMCNFCS                     (0x400E031CU) /**< \brief (MATRIX0) SMC Nand Flash Chip Select Configuration Register */
  #define REG_MATRIX0_CORE_DEBUG                  (0x400E0328U) /**< \brief (MATRIX0) Core Debug Configuration Register */
  #define REG_MATRIX0_WPMR                        (0x400E03E4U) /**< \brief (MATRIX0) Write Protection Mode Register */
  #define REG_MATRIX0_WPSR                        (0x400E03E8U) /**< \brief (MATRIX0) Write Protection Status Register */
#else
  #define REG_MATRIX0_MCFG       (*(__IO uint32_t*)0x400E0200U) /**< \brief (MATRIX0) Master Configuration Register */
  #define REG_MATRIX0_SCFG       (*(__IO uint32_t*)0x400E0240U) /**< \brief (MATRIX0) Slave Configuration Register */
  #define REG_MATRIX0_PRAS0      (*(__IO uint32_t*)0x400E0280U) /**< \brief (MATRIX0) Priority Register A for Slave 0 */
  #define REG_MATRIX0_PRAS1      (*(__IO uint32_t*)0x400E0288U) /**< \brief (MATRIX0) Priority Register A for Slave 1 */
  #define REG_MATRIX0_PRAS2      (*(__IO uint32_t*)0x400E0290U) /**< \brief (MATRIX0) Priority Register A for Slave 2 */
  #define REG_MATRIX0_PRAS3      (*(__IO uint32_t*)0x400E0298U) /**< \brief (MATRIX0) Priority Register A for Slave 3 */
  #define REG_MATRIX0_PRAS4      (*(__IO uint32_t*)0x400E02A0U) /**< \brief (MATRIX0) Priority Register A for Slave 4 */
  #define REG_MATRIX0_PRAS5      (*(__IO uint32_t*)0x400E02A8U) /**< \brief (MATRIX0) Priority Register A for Slave 5 */
  #define REG_MATRIX0_PRAS6      (*(__IO uint32_t*)0x400E02B0U) /**< \brief (MATRIX0) Priority Register A for Slave 6 */
  #define REG_MATRIX0_PRAS7      (*(__IO uint32_t*)0x400E02B8U) /**< \brief (MATRIX0) Priority Register A for Slave 7 */
  #define REG_MATRIX0_PRAS8      (*(__IO uint32_t*)0x400E02BCU) /**< \brief (MATRIX0) Priority Register A for Slave 8 */
  #define REG_MATRIX0_SYSIO      (*(__IO uint32_t*)0x400E0314U) /**< \brief (MATRIX0) System I/O Configuration Register */
  #define REG_MATRIX0_SMCNFCS    (*(__IO uint32_t*)0x400E031CU) /**< \brief (MATRIX0) SMC Nand Flash Chip Select Configuration Register */
  #define REG_MATRIX0_CORE_DEBUG (*(__IO uint32_t*)0x400E0328U) /**< \brief (MATRIX0) Core Debug Configuration Register */
  #define REG_MATRIX0_WPMR       (*(__IO uint32_t*)0x400E03E4U) /**< \brief (MATRIX0) Write Protection Mode Register */
  #define REG_MATRIX0_WPSR       (*(__I  uint32_t*)0x400E03E8U) /**< \brief (MATRIX0) Write Protection Status Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#endif /* _SAM4C_MATRIX0_INSTANCE_ */
