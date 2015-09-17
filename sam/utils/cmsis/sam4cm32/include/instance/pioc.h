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

#ifndef _SAM4CM_PIOC_INSTANCE_
#define _SAM4CM_PIOC_INSTANCE_

/* ========== Register definition for PIOC peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
  #define REG_PIOC_PER                      (0x4800C000U) /**< \brief (PIOC) PIO Enable Register */
  #define REG_PIOC_PDR                      (0x4800C004U) /**< \brief (PIOC) PIO Disable Register */
  #define REG_PIOC_PSR                      (0x4800C008U) /**< \brief (PIOC) PIO Status Register */
  #define REG_PIOC_OER                      (0x4800C010U) /**< \brief (PIOC) Output Enable Register */
  #define REG_PIOC_ODR                      (0x4800C014U) /**< \brief (PIOC) Output Disable Register */
  #define REG_PIOC_OSR                      (0x4800C018U) /**< \brief (PIOC) Output Status Register */
  #define REG_PIOC_IFER                     (0x4800C020U) /**< \brief (PIOC) Glitch Input Filter Enable Register */
  #define REG_PIOC_IFDR                     (0x4800C024U) /**< \brief (PIOC) Glitch Input Filter Disable Register */
  #define REG_PIOC_IFSR                     (0x4800C028U) /**< \brief (PIOC) Glitch Input Filter Status Register */
  #define REG_PIOC_SODR                     (0x4800C030U) /**< \brief (PIOC) Set Output Data Register */
  #define REG_PIOC_CODR                     (0x4800C034U) /**< \brief (PIOC) Clear Output Data Register */
  #define REG_PIOC_ODSR                     (0x4800C038U) /**< \brief (PIOC) Output Data Status Register */
  #define REG_PIOC_PDSR                     (0x4800C03CU) /**< \brief (PIOC) Pin Data Status Register */
  #define REG_PIOC_IER                      (0x4800C040U) /**< \brief (PIOC) Interrupt Enable Register */
  #define REG_PIOC_IDR                      (0x4800C044U) /**< \brief (PIOC) Interrupt Disable Register */
  #define REG_PIOC_IMR                      (0x4800C048U) /**< \brief (PIOC) Interrupt Mask Register */
  #define REG_PIOC_ISR                      (0x4800C04CU) /**< \brief (PIOC) Interrupt Status Register */
  #define REG_PIOC_MDER                     (0x4800C050U) /**< \brief (PIOC) Multi-driver Enable Register */
  #define REG_PIOC_MDDR                     (0x4800C054U) /**< \brief (PIOC) Multi-driver Disable Register */
  #define REG_PIOC_MDSR                     (0x4800C058U) /**< \brief (PIOC) Multi-driver Status Register */
  #define REG_PIOC_PUDR                     (0x4800C060U) /**< \brief (PIOC) Pull-up Disable Register */
  #define REG_PIOC_PUER                     (0x4800C064U) /**< \brief (PIOC) Pull-up Enable Register */
  #define REG_PIOC_PUSR                     (0x4800C068U) /**< \brief (PIOC) Pad Pull-up Status Register */
  #define REG_PIOC_ABCDSR                   (0x4800C070U) /**< \brief (PIOC) Peripheral Select Register */
  #define REG_PIOC_IFSCDR                   (0x4800C080U) /**< \brief (PIOC) Input Filter Slow Clock Disable Register */
  #define REG_PIOC_IFSCER                   (0x4800C084U) /**< \brief (PIOC) Input Filter Slow Clock Enable Register */
  #define REG_PIOC_IFSCSR                   (0x4800C088U) /**< \brief (PIOC) Input Filter Slow Clock Status Register */
  #define REG_PIOC_SCDR                     (0x4800C08CU) /**< \brief (PIOC) Slow Clock Divider Debouncing Register */
  #define REG_PIOC_PPDDR                    (0x4800C090U) /**< \brief (PIOC) Pad Pull-down Disable Register */
  #define REG_PIOC_PPDER                    (0x4800C094U) /**< \brief (PIOC) Pad Pull-down Enable Register */
  #define REG_PIOC_PPDSR                    (0x4800C098U) /**< \brief (PIOC) Pad Pull-down Status Register */
  #define REG_PIOC_OWER                     (0x4800C0A0U) /**< \brief (PIOC) Output Write Enable */
  #define REG_PIOC_OWDR                     (0x4800C0A4U) /**< \brief (PIOC) Output Write Disable */
  #define REG_PIOC_OWSR                     (0x4800C0A8U) /**< \brief (PIOC) Output Write Status Register */
  #define REG_PIOC_AIMER                    (0x4800C0B0U) /**< \brief (PIOC) Additional Interrupt Modes Enable Register */
  #define REG_PIOC_AIMDR                    (0x4800C0B4U) /**< \brief (PIOC) Additional Interrupt Modes Disable Register */
  #define REG_PIOC_AIMMR                    (0x4800C0B8U) /**< \brief (PIOC) Additional Interrupt Modes Mask Register */
  #define REG_PIOC_ESR                      (0x4800C0C0U) /**< \brief (PIOC) Edge Select Register */
  #define REG_PIOC_LSR                      (0x4800C0C4U) /**< \brief (PIOC) Level Select Register */
  #define REG_PIOC_ELSR                     (0x4800C0C8U) /**< \brief (PIOC) Edge/Level Status Register */
  #define REG_PIOC_FELLSR                   (0x4800C0D0U) /**< \brief (PIOC) Falling Edge/Low-Level Select Register */
  #define REG_PIOC_REHLSR                   (0x4800C0D4U) /**< \brief (PIOC) Rising Edge/ High-Level Select Register */
  #define REG_PIOC_FRLHSR                   (0x4800C0D8U) /**< \brief (PIOC) Fall/Rise - Low/High Status Register */
  #define REG_PIOC_WPMR                     (0x4800C0E4U) /**< \brief (PIOC) Write Protection Mode Register */
  #define REG_PIOC_WPSR                     (0x4800C0E8U) /**< \brief (PIOC) Write Protection Status Register */
  #define REG_PIOC_SCHMITT                  (0x4800C100U) /**< \brief (PIOC) Schmitt Trigger Register */
  #define REG_PIOC_DRIVER1                  (0x4800C118U) /**< \brief (PIOC) I/O Drive Register 1 */
  #define REG_PIOC_DRIVER2                  (0x4800C11CU) /**< \brief (PIOC) I/O Drive Register 2 */
#else
  #define REG_PIOC_PER     (*(__O  uint32_t*)0x4800C000U) /**< \brief (PIOC) PIO Enable Register */
  #define REG_PIOC_PDR     (*(__O  uint32_t*)0x4800C004U) /**< \brief (PIOC) PIO Disable Register */
  #define REG_PIOC_PSR     (*(__I  uint32_t*)0x4800C008U) /**< \brief (PIOC) PIO Status Register */
  #define REG_PIOC_OER     (*(__O  uint32_t*)0x4800C010U) /**< \brief (PIOC) Output Enable Register */
  #define REG_PIOC_ODR     (*(__O  uint32_t*)0x4800C014U) /**< \brief (PIOC) Output Disable Register */
  #define REG_PIOC_OSR     (*(__I  uint32_t*)0x4800C018U) /**< \brief (PIOC) Output Status Register */
  #define REG_PIOC_IFER    (*(__O  uint32_t*)0x4800C020U) /**< \brief (PIOC) Glitch Input Filter Enable Register */
  #define REG_PIOC_IFDR    (*(__O  uint32_t*)0x4800C024U) /**< \brief (PIOC) Glitch Input Filter Disable Register */
  #define REG_PIOC_IFSR    (*(__I  uint32_t*)0x4800C028U) /**< \brief (PIOC) Glitch Input Filter Status Register */
  #define REG_PIOC_SODR    (*(__O  uint32_t*)0x4800C030U) /**< \brief (PIOC) Set Output Data Register */
  #define REG_PIOC_CODR    (*(__O  uint32_t*)0x4800C034U) /**< \brief (PIOC) Clear Output Data Register */
  #define REG_PIOC_ODSR    (*(__IO uint32_t*)0x4800C038U) /**< \brief (PIOC) Output Data Status Register */
  #define REG_PIOC_PDSR    (*(__I  uint32_t*)0x4800C03CU) /**< \brief (PIOC) Pin Data Status Register */
  #define REG_PIOC_IER     (*(__O  uint32_t*)0x4800C040U) /**< \brief (PIOC) Interrupt Enable Register */
  #define REG_PIOC_IDR     (*(__O  uint32_t*)0x4800C044U) /**< \brief (PIOC) Interrupt Disable Register */
  #define REG_PIOC_IMR     (*(__I  uint32_t*)0x4800C048U) /**< \brief (PIOC) Interrupt Mask Register */
  #define REG_PIOC_ISR     (*(__I  uint32_t*)0x4800C04CU) /**< \brief (PIOC) Interrupt Status Register */
  #define REG_PIOC_MDER    (*(__O  uint32_t*)0x4800C050U) /**< \brief (PIOC) Multi-driver Enable Register */
  #define REG_PIOC_MDDR    (*(__O  uint32_t*)0x4800C054U) /**< \brief (PIOC) Multi-driver Disable Register */
  #define REG_PIOC_MDSR    (*(__I  uint32_t*)0x4800C058U) /**< \brief (PIOC) Multi-driver Status Register */
  #define REG_PIOC_PUDR    (*(__O  uint32_t*)0x4800C060U) /**< \brief (PIOC) Pull-up Disable Register */
  #define REG_PIOC_PUER    (*(__O  uint32_t*)0x4800C064U) /**< \brief (PIOC) Pull-up Enable Register */
  #define REG_PIOC_PUSR    (*(__I  uint32_t*)0x4800C068U) /**< \brief (PIOC) Pad Pull-up Status Register */
  #define REG_PIOC_ABCDSR  (*(__IO uint32_t*)0x4800C070U) /**< \brief (PIOC) Peripheral Select Register */
  #define REG_PIOC_IFSCDR  (*(__O  uint32_t*)0x4800C080U) /**< \brief (PIOC) Input Filter Slow Clock Disable Register */
  #define REG_PIOC_IFSCER  (*(__O  uint32_t*)0x4800C084U) /**< \brief (PIOC) Input Filter Slow Clock Enable Register */
  #define REG_PIOC_IFSCSR  (*(__I  uint32_t*)0x4800C088U) /**< \brief (PIOC) Input Filter Slow Clock Status Register */
  #define REG_PIOC_SCDR    (*(__IO uint32_t*)0x4800C08CU) /**< \brief (PIOC) Slow Clock Divider Debouncing Register */
  #define REG_PIOC_PPDDR   (*(__O  uint32_t*)0x4800C090U) /**< \brief (PIOC) Pad Pull-down Disable Register */
  #define REG_PIOC_PPDER   (*(__O  uint32_t*)0x4800C094U) /**< \brief (PIOC) Pad Pull-down Enable Register */
  #define REG_PIOC_PPDSR   (*(__I  uint32_t*)0x4800C098U) /**< \brief (PIOC) Pad Pull-down Status Register */
  #define REG_PIOC_OWER    (*(__O  uint32_t*)0x4800C0A0U) /**< \brief (PIOC) Output Write Enable */
  #define REG_PIOC_OWDR    (*(__O  uint32_t*)0x4800C0A4U) /**< \brief (PIOC) Output Write Disable */
  #define REG_PIOC_OWSR    (*(__I  uint32_t*)0x4800C0A8U) /**< \brief (PIOC) Output Write Status Register */
  #define REG_PIOC_AIMER   (*(__O  uint32_t*)0x4800C0B0U) /**< \brief (PIOC) Additional Interrupt Modes Enable Register */
  #define REG_PIOC_AIMDR   (*(__O  uint32_t*)0x4800C0B4U) /**< \brief (PIOC) Additional Interrupt Modes Disable Register */
  #define REG_PIOC_AIMMR   (*(__I  uint32_t*)0x4800C0B8U) /**< \brief (PIOC) Additional Interrupt Modes Mask Register */
  #define REG_PIOC_ESR     (*(__O  uint32_t*)0x4800C0C0U) /**< \brief (PIOC) Edge Select Register */
  #define REG_PIOC_LSR     (*(__O  uint32_t*)0x4800C0C4U) /**< \brief (PIOC) Level Select Register */
  #define REG_PIOC_ELSR    (*(__I  uint32_t*)0x4800C0C8U) /**< \brief (PIOC) Edge/Level Status Register */
  #define REG_PIOC_FELLSR  (*(__O  uint32_t*)0x4800C0D0U) /**< \brief (PIOC) Falling Edge/Low-Level Select Register */
  #define REG_PIOC_REHLSR  (*(__O  uint32_t*)0x4800C0D4U) /**< \brief (PIOC) Rising Edge/ High-Level Select Register */
  #define REG_PIOC_FRLHSR  (*(__I  uint32_t*)0x4800C0D8U) /**< \brief (PIOC) Fall/Rise - Low/High Status Register */
  #define REG_PIOC_WPMR    (*(__IO uint32_t*)0x4800C0E4U) /**< \brief (PIOC) Write Protection Mode Register */
  #define REG_PIOC_WPSR    (*(__I  uint32_t*)0x4800C0E8U) /**< \brief (PIOC) Write Protection Status Register */
  #define REG_PIOC_SCHMITT (*(__IO uint32_t*)0x4800C100U) /**< \brief (PIOC) Schmitt Trigger Register */
  #define REG_PIOC_DRIVER1 (*(__IO uint32_t*)0x4800C118U) /**< \brief (PIOC) I/O Drive Register 1 */
  #define REG_PIOC_DRIVER2 (*(__IO uint32_t*)0x4800C11CU) /**< \brief (PIOC) I/O Drive Register 2 */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#endif /* _SAM4CM_PIOC_INSTANCE_ */
