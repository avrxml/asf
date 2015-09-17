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

#ifndef _SAM4CM_AES_INSTANCE_
#define _SAM4CM_AES_INSTANCE_

/* ========== Register definition for AES peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
  #define REG_AES_CR                       (0x40000000U) /**< \brief (AES) Control Register */
  #define REG_AES_MR                       (0x40000004U) /**< \brief (AES) Mode Register */
  #define REG_AES_IER                      (0x40000010U) /**< \brief (AES) Interrupt Enable Register */
  #define REG_AES_IDR                      (0x40000014U) /**< \brief (AES) Interrupt Disable Register */
  #define REG_AES_IMR                      (0x40000018U) /**< \brief (AES) Interrupt Mask Register */
  #define REG_AES_ISR                      (0x4000001CU) /**< \brief (AES) Interrupt Status Register */
  #define REG_AES_KEYWR                    (0x40000020U) /**< \brief (AES) Key Word Register */
  #define REG_AES_IDATAR                   (0x40000040U) /**< \brief (AES) Input Data Register */
  #define REG_AES_ODATAR                   (0x40000050U) /**< \brief (AES) Output Data Register */
  #define REG_AES_IVR                      (0x40000060U) /**< \brief (AES) Initialization Vector Register */
  #define REG_AES_AADLENR                  (0x40000070U) /**< \brief (AES) Additional Authenticated Data Length Register */
  #define REG_AES_CLENR                    (0x40000074U) /**< \brief (AES) Plaintext/Ciphertext Length Register */
  #define REG_AES_GHASHR                   (0x40000078U) /**< \brief (AES) GCM Intermediate Hash Word Register */
  #define REG_AES_TAGR                     (0x40000088U) /**< \brief (AES) GCM Authentication Tag Word Register */
  #define REG_AES_CTRR                     (0x40000098U) /**< \brief (AES) GCM Encryption Counter Value Register */
  #define REG_AES_GCMHR                    (0x4000009CU) /**< \brief (AES) GCM H World Register */
  #define REG_AES_RPR                      (0x40000100U) /**< \brief (AES) Receive Pointer Register */
  #define REG_AES_RCR                      (0x40000104U) /**< \brief (AES) Receive Counter Register */
  #define REG_AES_TPR                      (0x40000108U) /**< \brief (AES) Transmit Pointer Register */
  #define REG_AES_TCR                      (0x4000010CU) /**< \brief (AES) Transmit Counter Register */
  #define REG_AES_RNPR                     (0x40000110U) /**< \brief (AES) Receive Next Pointer Register */
  #define REG_AES_RNCR                     (0x40000114U) /**< \brief (AES) Receive Next Counter Register */
  #define REG_AES_TNPR                     (0x40000118U) /**< \brief (AES) Transmit Next Pointer Register */
  #define REG_AES_TNCR                     (0x4000011CU) /**< \brief (AES) Transmit Next Counter Register */
  #define REG_AES_PTCR                     (0x40000120U) /**< \brief (AES) Transfer Control Register */
  #define REG_AES_PTSR                     (0x40000124U) /**< \brief (AES) Transfer Status Register */
#else
  #define REG_AES_CR      (*(__O  uint32_t*)0x40000000U) /**< \brief (AES) Control Register */
  #define REG_AES_MR      (*(__IO uint32_t*)0x40000004U) /**< \brief (AES) Mode Register */
  #define REG_AES_IER     (*(__O  uint32_t*)0x40000010U) /**< \brief (AES) Interrupt Enable Register */
  #define REG_AES_IDR     (*(__O  uint32_t*)0x40000014U) /**< \brief (AES) Interrupt Disable Register */
  #define REG_AES_IMR     (*(__I  uint32_t*)0x40000018U) /**< \brief (AES) Interrupt Mask Register */
  #define REG_AES_ISR     (*(__I  uint32_t*)0x4000001CU) /**< \brief (AES) Interrupt Status Register */
  #define REG_AES_KEYWR   (*(__O  uint32_t*)0x40000020U) /**< \brief (AES) Key Word Register */
  #define REG_AES_IDATAR  (*(__O  uint32_t*)0x40000040U) /**< \brief (AES) Input Data Register */
  #define REG_AES_ODATAR  (*(__I  uint32_t*)0x40000050U) /**< \brief (AES) Output Data Register */
  #define REG_AES_IVR     (*(__O  uint32_t*)0x40000060U) /**< \brief (AES) Initialization Vector Register */
  #define REG_AES_AADLENR (*(__IO uint32_t*)0x40000070U) /**< \brief (AES) Additional Authenticated Data Length Register */
  #define REG_AES_CLENR   (*(__IO uint32_t*)0x40000074U) /**< \brief (AES) Plaintext/Ciphertext Length Register */
  #define REG_AES_GHASHR  (*(__IO uint32_t*)0x40000078U) /**< \brief (AES) GCM Intermediate Hash Word Register */
  #define REG_AES_TAGR    (*(__I  uint32_t*)0x40000088U) /**< \brief (AES) GCM Authentication Tag Word Register */
  #define REG_AES_CTRR    (*(__I  uint32_t*)0x40000098U) /**< \brief (AES) GCM Encryption Counter Value Register */
  #define REG_AES_GCMHR   (*(__IO uint32_t*)0x4000009CU) /**< \brief (AES) GCM H World Register */
  #define REG_AES_RPR     (*(__IO uint32_t*)0x40000100U) /**< \brief (AES) Receive Pointer Register */
  #define REG_AES_RCR     (*(__IO uint32_t*)0x40000104U) /**< \brief (AES) Receive Counter Register */
  #define REG_AES_TPR     (*(__IO uint32_t*)0x40000108U) /**< \brief (AES) Transmit Pointer Register */
  #define REG_AES_TCR     (*(__IO uint32_t*)0x4000010CU) /**< \brief (AES) Transmit Counter Register */
  #define REG_AES_RNPR    (*(__IO uint32_t*)0x40000110U) /**< \brief (AES) Receive Next Pointer Register */
  #define REG_AES_RNCR    (*(__IO uint32_t*)0x40000114U) /**< \brief (AES) Receive Next Counter Register */
  #define REG_AES_TNPR    (*(__IO uint32_t*)0x40000118U) /**< \brief (AES) Transmit Next Pointer Register */
  #define REG_AES_TNCR    (*(__IO uint32_t*)0x4000011CU) /**< \brief (AES) Transmit Next Counter Register */
  #define REG_AES_PTCR    (*(__O  uint32_t*)0x40000120U) /**< \brief (AES) Transfer Control Register */
  #define REG_AES_PTSR    (*(__I  uint32_t*)0x40000124U) /**< \brief (AES) Transfer Status Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#endif /* _SAM4CM_AES_INSTANCE_ */
