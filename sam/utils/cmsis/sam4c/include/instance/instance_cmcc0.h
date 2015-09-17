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
#ifndef _SAM4C_CMCC0_INSTANCE_
#define _SAM4C_CMCC0_INSTANCE_

/* ========== Register definition for CMCC0 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
  #define REG_CMCC0_TYPE                    (0x4007C000U) /**< \brief (CMCC0) Cache Type Register */
  #define REG_CMCC0_CTRL                    (0x4007C008U) /**< \brief (CMCC0) Cache Control Register */
  #define REG_CMCC0_SR                      (0x4007C00CU) /**< \brief (CMCC0) Cache Status Register */
  #define REG_CMCC0_MAINT0                  (0x4007C020U) /**< \brief (CMCC0) Cache Maintenance Register 0 */
  #define REG_CMCC0_MAINT1                  (0x4007C024U) /**< \brief (CMCC0) Cache Maintenance Register 1 */
  #define REG_CMCC0_MCFG                    (0x4007C028U) /**< \brief (CMCC0) Cache Monitor Configuration Register */
  #define REG_CMCC0_MEN                     (0x4007C02CU) /**< \brief (CMCC0) Cache Monitor Enable Register */
  #define REG_CMCC0_MCTRL                   (0x4007C030U) /**< \brief (CMCC0) Cache Monitor Control Register */
  #define REG_CMCC0_MSR                     (0x4007C034U) /**< \brief (CMCC0) Cache Monitor Status Register */
#else
  #define REG_CMCC0_TYPE   (*(__I  uint32_t*)0x4007C000U) /**< \brief (CMCC0) Cache Type Register */
  #define REG_CMCC0_CTRL   (*(__O  uint32_t*)0x4007C008U) /**< \brief (CMCC0) Cache Control Register */
  #define REG_CMCC0_SR     (*(__I  uint32_t*)0x4007C00CU) /**< \brief (CMCC0) Cache Status Register */
  #define REG_CMCC0_MAINT0 (*(__O  uint32_t*)0x4007C020U) /**< \brief (CMCC0) Cache Maintenance Register 0 */
  #define REG_CMCC0_MAINT1 (*(__O  uint32_t*)0x4007C024U) /**< \brief (CMCC0) Cache Maintenance Register 1 */
  #define REG_CMCC0_MCFG   (*(__IO uint32_t*)0x4007C028U) /**< \brief (CMCC0) Cache Monitor Configuration Register */
  #define REG_CMCC0_MEN    (*(__IO uint32_t*)0x4007C02CU) /**< \brief (CMCC0) Cache Monitor Enable Register */
  #define REG_CMCC0_MCTRL  (*(__O  uint32_t*)0x4007C030U) /**< \brief (CMCC0) Cache Monitor Control Register */
  #define REG_CMCC0_MSR    (*(__I  uint32_t*)0x4007C034U) /**< \brief (CMCC0) Cache Monitor Status Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#endif /* _SAM4C_CMCC0_INSTANCE_ */
