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

#ifndef _SAM4E_AFEC0_INSTANCE_
#define _SAM4E_AFEC0_INSTANCE_

/* ========== Register definition for AFEC0 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
  #define REG_AFEC0_CR                       (0x400B0000U) /**< \brief (AFEC0) AFEC Control Register */
  #define REG_AFEC0_MR                       (0x400B0004U) /**< \brief (AFEC0) AFEC Mode Register */
  #define REG_AFEC0_EMR                      (0x400B0008U) /**< \brief (AFEC0) AFEC Extended Mode Register */
  #define REG_AFEC0_SEQ1R                    (0x400B000CU) /**< \brief (AFEC0) AFEC Channel Sequence 1 Register */
  #define REG_AFEC0_SEQ2R                    (0x400B0010U) /**< \brief (AFEC0) AFEC Channel Sequence 2 Register */
  #define REG_AFEC0_CHER                     (0x400B0014U) /**< \brief (AFEC0) AFEC Channel Enable Register */
  #define REG_AFEC0_CHDR                     (0x400B0018U) /**< \brief (AFEC0) AFEC Channel Disable Register */
  #define REG_AFEC0_CHSR                     (0x400B001CU) /**< \brief (AFEC0) AFEC Channel Status Register */
  #define REG_AFEC0_LCDR                     (0x400B0020U) /**< \brief (AFEC0) AFEC Last Converted Data Register */
  #define REG_AFEC0_IER                      (0x400B0024U) /**< \brief (AFEC0) AFEC Interrupt Enable Register */
  #define REG_AFEC0_IDR                      (0x400B0028U) /**< \brief (AFEC0) AFEC Interrupt Disable Register */
  #define REG_AFEC0_IMR                      (0x400B002CU) /**< \brief (AFEC0) AFEC Interrupt Mask Register */
  #define REG_AFEC0_ISR                      (0x400B0030U) /**< \brief (AFEC0) AFEC Interrupt Status Register */
  #define REG_AFEC0_OVER                     (0x400B004CU) /**< \brief (AFEC0) AFEC Overrun Status Register */
  #define REG_AFEC0_CWR                      (0x400B0050U) /**< \brief (AFEC0) AFEC Compare Window Register */
  #define REG_AFEC0_CGR                      (0x400B0054U) /**< \brief (AFEC0) AFEC Channel Gain Register */
  #define REG_AFEC0_CDOR                     (0x400B005CU) /**< \brief (AFEC0) AFEC Channel Calibration DC Offset Register */
  #define REG_AFEC0_DIFFR                    (0x400B0060U) /**< \brief (AFEC0) AFEC Channel Differential Register */
  #define REG_AFEC0_CSELR                    (0x400B0064U) /**< \brief (AFEC0) AFEC Channel Register Selection */
  #define REG_AFEC0_CDR                      (0x400B0068U) /**< \brief (AFEC0) AFEC Channel Data Register */
  #define REG_AFEC0_COCR                     (0x400B006CU) /**< \brief (AFEC0) AFEC Channel Offset Compensation Register */
  #define REG_AFEC0_TEMPMR                   (0x400B0070U) /**< \brief (AFEC0) AFEC Temperature Sensor Mode Register */
  #define REG_AFEC0_TEMPCWR                  (0x400B0074U) /**< \brief (AFEC0) AFEC Temperature Compare Window Register */
  #define REG_AFEC0_ACR                      (0x400B0094U) /**< \brief (AFEC0) AFEC Analog Control Register */
  #define REG_AFEC0_COSR                     (0x400B00D0U) /**< \brief (AFEC0) AFEC Correction Select Register */
  #define REG_AFEC0_CVR                      (0x400B00D4U) /**< \brief (AFEC0) AFEC Correction Values Register */
  #define REG_AFEC0_CECR                     (0x400B00D8U) /**< \brief (AFEC0) AFEC Channel Error Correction Register */
  #define REG_AFEC0_WPMR                     (0x400B00E4U) /**< \brief (AFEC0) AFEC Write Protection Mode Register */
  #define REG_AFEC0_WPSR                     (0x400B00E8U) /**< \brief (AFEC0) AFEC Write Protection Status Register */
  #define REG_AFEC0_RPR                      (0x400B0100U) /**< \brief (AFEC0) Receive Pointer Register */
  #define REG_AFEC0_RCR                      (0x400B0104U) /**< \brief (AFEC0) Receive Counter Register */
  #define REG_AFEC0_RNPR                     (0x400B0110U) /**< \brief (AFEC0) Receive Next Pointer Register */
  #define REG_AFEC0_RNCR                     (0x400B0114U) /**< \brief (AFEC0) Receive Next Counter Register */
  #define REG_AFEC0_PTCR                     (0x400B0120U) /**< \brief (AFEC0) Transfer Control Register */
  #define REG_AFEC0_PTSR                     (0x400B0124U) /**< \brief (AFEC0) Transfer Status Register */
#else
  #define REG_AFEC0_CR      (*(__O  uint32_t*)0x400B0000U) /**< \brief (AFEC0) AFEC Control Register */
  #define REG_AFEC0_MR      (*(__IO uint32_t*)0x400B0004U) /**< \brief (AFEC0) AFEC Mode Register */
  #define REG_AFEC0_EMR     (*(__IO uint32_t*)0x400B0008U) /**< \brief (AFEC0) AFEC Extended Mode Register */
  #define REG_AFEC0_SEQ1R   (*(__IO uint32_t*)0x400B000CU) /**< \brief (AFEC0) AFEC Channel Sequence 1 Register */
  #define REG_AFEC0_SEQ2R   (*(__IO uint32_t*)0x400B0010U) /**< \brief (AFEC0) AFEC Channel Sequence 2 Register */
  #define REG_AFEC0_CHER    (*(__O  uint32_t*)0x400B0014U) /**< \brief (AFEC0) AFEC Channel Enable Register */
  #define REG_AFEC0_CHDR    (*(__O  uint32_t*)0x400B0018U) /**< \brief (AFEC0) AFEC Channel Disable Register */
  #define REG_AFEC0_CHSR    (*(__I  uint32_t*)0x400B001CU) /**< \brief (AFEC0) AFEC Channel Status Register */
  #define REG_AFEC0_LCDR    (*(__I  uint32_t*)0x400B0020U) /**< \brief (AFEC0) AFEC Last Converted Data Register */
  #define REG_AFEC0_IER     (*(__O  uint32_t*)0x400B0024U) /**< \brief (AFEC0) AFEC Interrupt Enable Register */
  #define REG_AFEC0_IDR     (*(__O  uint32_t*)0x400B0028U) /**< \brief (AFEC0) AFEC Interrupt Disable Register */
  #define REG_AFEC0_IMR     (*(__I  uint32_t*)0x400B002CU) /**< \brief (AFEC0) AFEC Interrupt Mask Register */
  #define REG_AFEC0_ISR     (*(__I  uint32_t*)0x400B0030U) /**< \brief (AFEC0) AFEC Interrupt Status Register */
  #define REG_AFEC0_OVER    (*(__I  uint32_t*)0x400B004CU) /**< \brief (AFEC0) AFEC Overrun Status Register */
  #define REG_AFEC0_CWR     (*(__IO uint32_t*)0x400B0050U) /**< \brief (AFEC0) AFEC Compare Window Register */
  #define REG_AFEC0_CGR     (*(__IO uint32_t*)0x400B0054U) /**< \brief (AFEC0) AFEC Channel Gain Register */
  #define REG_AFEC0_CDOR    (*(__IO uint32_t*)0x400B005CU) /**< \brief (AFEC0) AFEC Channel Calibration DC Offset Register */
  #define REG_AFEC0_DIFFR   (*(__IO uint32_t*)0x400B0060U) /**< \brief (AFEC0) AFEC Channel Differential Register */
  #define REG_AFEC0_CSELR   (*(__IO uint32_t*)0x400B0064U) /**< \brief (AFEC0) AFEC Channel Register Selection */
  #define REG_AFEC0_CDR     (*(__I  uint32_t*)0x400B0068U) /**< \brief (AFEC0) AFEC Channel Data Register */
  #define REG_AFEC0_COCR    (*(__IO uint32_t*)0x400B006CU) /**< \brief (AFEC0) AFEC Channel Offset Compensation Register */
  #define REG_AFEC0_TEMPMR  (*(__IO uint32_t*)0x400B0070U) /**< \brief (AFEC0) AFEC Temperature Sensor Mode Register */
  #define REG_AFEC0_TEMPCWR (*(__IO uint32_t*)0x400B0074U) /**< \brief (AFEC0) AFEC Temperature Compare Window Register */
  #define REG_AFEC0_ACR     (*(__IO uint32_t*)0x400B0094U) /**< \brief (AFEC0) AFEC Analog Control Register */
  #define REG_AFEC0_COSR    (*(__IO uint32_t*)0x400B00D0U) /**< \brief (AFEC0) AFEC Correction Select Register */
  #define REG_AFEC0_CVR     (*(__IO uint32_t*)0x400B00D4U) /**< \brief (AFEC0) AFEC Correction Values Register */
  #define REG_AFEC0_CECR    (*(__IO uint32_t*)0x400B00D8U) /**< \brief (AFEC0) AFEC Channel Error Correction Register */
  #define REG_AFEC0_WPMR    (*(__IO uint32_t*)0x400B00E4U) /**< \brief (AFEC0) AFEC Write Protection Mode Register */
  #define REG_AFEC0_WPSR    (*(__I  uint32_t*)0x400B00E8U) /**< \brief (AFEC0) AFEC Write Protection Status Register */
  #define REG_AFEC0_RPR     (*(__IO uint32_t*)0x400B0100U) /**< \brief (AFEC0) Receive Pointer Register */
  #define REG_AFEC0_RCR     (*(__IO uint32_t*)0x400B0104U) /**< \brief (AFEC0) Receive Counter Register */
  #define REG_AFEC0_RNPR    (*(__IO uint32_t*)0x400B0110U) /**< \brief (AFEC0) Receive Next Pointer Register */
  #define REG_AFEC0_RNCR    (*(__IO uint32_t*)0x400B0114U) /**< \brief (AFEC0) Receive Next Counter Register */
  #define REG_AFEC0_PTCR    (*(__O  uint32_t*)0x400B0120U) /**< \brief (AFEC0) Transfer Control Register */
  #define REG_AFEC0_PTSR    (*(__I  uint32_t*)0x400B0124U) /**< \brief (AFEC0) Transfer Status Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#endif /* _SAM4E_AFEC0_INSTANCE_ */
