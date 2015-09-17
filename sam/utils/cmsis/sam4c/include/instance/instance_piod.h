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
#ifndef _SAM4C_PIOD_INSTANCE_
#define _SAM4C_PIOD_INSTANCE_

/* ========== Register definition for PIOD peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
  #define REG_PIOD_PER                      (0x400E1200U) /**< \brief (PIOD) PIO Enable Register */
  #define REG_PIOD_PDR                      (0x400E1204U) /**< \brief (PIOD) PIO Disable Register */
  #define REG_PIOD_PSR                      (0x400E1208U) /**< \brief (PIOD) PIO Status Register */
  #define REG_PIOD_OER                      (0x400E1210U) /**< \brief (PIOD) Output Enable Register */
  #define REG_PIOD_ODR                      (0x400E1214U) /**< \brief (PIOD) Output Disable Register */
  #define REG_PIOD_OSR                      (0x400E1218U) /**< \brief (PIOD) Output Status Register */
  #define REG_PIOD_IFER                     (0x400E1220U) /**< \brief (PIOD) Glitch Input Filter Enable Register */
  #define REG_PIOD_IFDR                     (0x400E1224U) /**< \brief (PIOD) Glitch Input Filter Disable Register */
  #define REG_PIOD_IFSR                     (0x400E1228U) /**< \brief (PIOD) Glitch Input Filter Status Register */
  #define REG_PIOD_SODR                     (0x400E1230U) /**< \brief (PIOD) Set Output Data Register */
  #define REG_PIOD_CODR                     (0x400E1234U) /**< \brief (PIOD) Clear Output Data Register */
  #define REG_PIOD_ODSR                     (0x400E1238U) /**< \brief (PIOD) Output Data Status Register */
  #define REG_PIOD_PDSR                     (0x400E123CU) /**< \brief (PIOD) Pin Data Status Register */
  #define REG_PIOD_IER                      (0x400E1240U) /**< \brief (PIOD) Interrupt Enable Register */
  #define REG_PIOD_IDR                      (0x400E1244U) /**< \brief (PIOD) Interrupt Disable Register */
  #define REG_PIOD_IMR                      (0x400E1248U) /**< \brief (PIOD) Interrupt Mask Register */
  #define REG_PIOD_ISR                      (0x400E124CU) /**< \brief (PIOD) Interrupt Status Register */
  #define REG_PIOD_MDER                     (0x400E1250U) /**< \brief (PIOD) Multi-driver Enable Register */
  #define REG_PIOD_MDDR                     (0x400E1254U) /**< \brief (PIOD) Multi-driver Disable Register */
  #define REG_PIOD_MDSR                     (0x400E1258U) /**< \brief (PIOD) Multi-driver Status Register */
  #define REG_PIOD_PUDR                     (0x400E1260U) /**< \brief (PIOD) Pull-up Disable Register */
  #define REG_PIOD_PUER                     (0x400E1264U) /**< \brief (PIOD) Pull-up Enable Register */
  #define REG_PIOD_PUSR                     (0x400E1268U) /**< \brief (PIOD) Pad Pull-up Status Register */
  #define REG_PIOD_ABCDSR                   (0x400E1270U) /**< \brief (PIOD) Peripheral Select Register */
  #define REG_PIOD_IFSCDR                   (0x400E1280U) /**< \brief (PIOD) Input Filter Slow Clock Disable Register */
  #define REG_PIOD_IFSCER                   (0x400E1284U) /**< \brief (PIOD) Input Filter Slow Clock Enable Register */
  #define REG_PIOD_IFSCSR                   (0x400E1288U) /**< \brief (PIOD) Input Filter Slow Clock Status Register */
  #define REG_PIOD_SCDR                     (0x400E128CU) /**< \brief (PIOD) Slow Clock Divider Debouncing Register */
  #define REG_PIOD_PPDDR                    (0x400E1290U) /**< \brief (PIOD) Pad Pull-down Disable Register */
  #define REG_PIOD_PPDER                    (0x400E1294U) /**< \brief (PIOD) Pad Pull-down Enable Register */
  #define REG_PIOD_PPDSR                    (0x400E1298U) /**< \brief (PIOD) Pad Pull-down Status Register */
  #define REG_PIOD_OWER                     (0x400E12A0U) /**< \brief (PIOD) Output Write Enable */
  #define REG_PIOD_OWDR                     (0x400E12A4U) /**< \brief (PIOD) Output Write Disable */
  #define REG_PIOD_OWSR                     (0x400E12A8U) /**< \brief (PIOD) Output Write Status Register */
  #define REG_PIOD_AIMER                    (0x400E12B0U) /**< \brief (PIOD) Additional Interrupt Modes Enable Register */
  #define REG_PIOD_AIMDR                    (0x400E12B4U) /**< \brief (PIOD) Additional Interrupt Modes Disable Register */
  #define REG_PIOD_AIMMR                    (0x400E12B8U) /**< \brief (PIOD) Additional Interrupt Modes Mask Register */
  #define REG_PIOD_ESR                      (0x400E12C0U) /**< \brief (PIOD) Edge Select Register */
  #define REG_PIOD_LSR                      (0x400E12C4U) /**< \brief (PIOD) Level Select Register */
  #define REG_PIOD_ELSR                     (0x400E12C8U) /**< \brief (PIOD) Edge/Level Status Register */
  #define REG_PIOD_FELLSR                   (0x400E12D0U) /**< \brief (PIOD) Falling Edge/Low-Level Select Register */
  #define REG_PIOD_REHLSR                   (0x400E12D4U) /**< \brief (PIOD) Rising Edge/ High-Level Select Register */
  #define REG_PIOD_FRLHSR                   (0x400E12D8U) /**< \brief (PIOD) Fall/Rise - Low/High Status Register */
  #define REG_PIOD_WPMR                     (0x400E12E4U) /**< \brief (PIOD) Write Protection Mode Register */
  #define REG_PIOD_WPSR                     (0x400E12E8U) /**< \brief (PIOD) Write Protection Status Register */
  #define REG_PIOD_SCHMITT                  (0x400E1300U) /**< \brief (PIOD) Schmitt Trigger Register */
  #define REG_PIOD_DRIVER1                  (0x400E1318U) /**< \brief (PIOD) I/O Drive Register 1 */
  #define REG_PIOD_DRIVER2                  (0x400E131CU) /**< \brief (PIOD) I/O Drive Register 2 */
#else
  #define REG_PIOD_PER     (*(__O  uint32_t*)0x400E1200U) /**< \brief (PIOD) PIO Enable Register */
  #define REG_PIOD_PDR     (*(__O  uint32_t*)0x400E1204U) /**< \brief (PIOD) PIO Disable Register */
  #define REG_PIOD_PSR     (*(__I  uint32_t*)0x400E1208U) /**< \brief (PIOD) PIO Status Register */
  #define REG_PIOD_OER     (*(__O  uint32_t*)0x400E1210U) /**< \brief (PIOD) Output Enable Register */
  #define REG_PIOD_ODR     (*(__O  uint32_t*)0x400E1214U) /**< \brief (PIOD) Output Disable Register */
  #define REG_PIOD_OSR     (*(__I  uint32_t*)0x400E1218U) /**< \brief (PIOD) Output Status Register */
  #define REG_PIOD_IFER    (*(__O  uint32_t*)0x400E1220U) /**< \brief (PIOD) Glitch Input Filter Enable Register */
  #define REG_PIOD_IFDR    (*(__O  uint32_t*)0x400E1224U) /**< \brief (PIOD) Glitch Input Filter Disable Register */
  #define REG_PIOD_IFSR    (*(__I  uint32_t*)0x400E1228U) /**< \brief (PIOD) Glitch Input Filter Status Register */
  #define REG_PIOD_SODR    (*(__O  uint32_t*)0x400E1230U) /**< \brief (PIOD) Set Output Data Register */
  #define REG_PIOD_CODR    (*(__O  uint32_t*)0x400E1234U) /**< \brief (PIOD) Clear Output Data Register */
  #define REG_PIOD_ODSR    (*(__IO uint32_t*)0x400E1238U) /**< \brief (PIOD) Output Data Status Register */
  #define REG_PIOD_PDSR    (*(__I  uint32_t*)0x400E123CU) /**< \brief (PIOD) Pin Data Status Register */
  #define REG_PIOD_IER     (*(__O  uint32_t*)0x400E1240U) /**< \brief (PIOD) Interrupt Enable Register */
  #define REG_PIOD_IDR     (*(__O  uint32_t*)0x400E1244U) /**< \brief (PIOD) Interrupt Disable Register */
  #define REG_PIOD_IMR     (*(__I  uint32_t*)0x400E1248U) /**< \brief (PIOD) Interrupt Mask Register */
  #define REG_PIOD_ISR     (*(__I  uint32_t*)0x400E124CU) /**< \brief (PIOD) Interrupt Status Register */
  #define REG_PIOD_MDER    (*(__O  uint32_t*)0x400E1250U) /**< \brief (PIOD) Multi-driver Enable Register */
  #define REG_PIOD_MDDR    (*(__O  uint32_t*)0x400E1254U) /**< \brief (PIOD) Multi-driver Disable Register */
  #define REG_PIOD_MDSR    (*(__I  uint32_t*)0x400E1258U) /**< \brief (PIOD) Multi-driver Status Register */
  #define REG_PIOD_PUDR    (*(__O  uint32_t*)0x400E1260U) /**< \brief (PIOD) Pull-up Disable Register */
  #define REG_PIOD_PUER    (*(__O  uint32_t*)0x400E1264U) /**< \brief (PIOD) Pull-up Enable Register */
  #define REG_PIOD_PUSR    (*(__I  uint32_t*)0x400E1268U) /**< \brief (PIOD) Pad Pull-up Status Register */
  #define REG_PIOD_ABCDSR  (*(__IO uint32_t*)0x400E1270U) /**< \brief (PIOD) Peripheral Select Register */
  #define REG_PIOD_IFSCDR  (*(__O  uint32_t*)0x400E1280U) /**< \brief (PIOD) Input Filter Slow Clock Disable Register */
  #define REG_PIOD_IFSCER  (*(__O  uint32_t*)0x400E1284U) /**< \brief (PIOD) Input Filter Slow Clock Enable Register */
  #define REG_PIOD_IFSCSR  (*(__I  uint32_t*)0x400E1288U) /**< \brief (PIOD) Input Filter Slow Clock Status Register */
  #define REG_PIOD_SCDR    (*(__IO uint32_t*)0x400E128CU) /**< \brief (PIOD) Slow Clock Divider Debouncing Register */
  #define REG_PIOD_PPDDR   (*(__O  uint32_t*)0x400E1290U) /**< \brief (PIOD) Pad Pull-down Disable Register */
  #define REG_PIOD_PPDER   (*(__O  uint32_t*)0x400E1294U) /**< \brief (PIOD) Pad Pull-down Enable Register */
  #define REG_PIOD_PPDSR   (*(__I  uint32_t*)0x400E1298U) /**< \brief (PIOD) Pad Pull-down Status Register */
  #define REG_PIOD_OWER    (*(__O  uint32_t*)0x400E12A0U) /**< \brief (PIOD) Output Write Enable */
  #define REG_PIOD_OWDR    (*(__O  uint32_t*)0x400E12A4U) /**< \brief (PIOD) Output Write Disable */
  #define REG_PIOD_OWSR    (*(__I  uint32_t*)0x400E12A8U) /**< \brief (PIOD) Output Write Status Register */
  #define REG_PIOD_AIMER   (*(__O  uint32_t*)0x400E12B0U) /**< \brief (PIOD) Additional Interrupt Modes Enable Register */
  #define REG_PIOD_AIMDR   (*(__O  uint32_t*)0x400E12B4U) /**< \brief (PIOD) Additional Interrupt Modes Disable Register */
  #define REG_PIOD_AIMMR   (*(__I  uint32_t*)0x400E12B8U) /**< \brief (PIOD) Additional Interrupt Modes Mask Register */
  #define REG_PIOD_ESR     (*(__O  uint32_t*)0x400E12C0U) /**< \brief (PIOD) Edge Select Register */
  #define REG_PIOD_LSR     (*(__O  uint32_t*)0x400E12C4U) /**< \brief (PIOD) Level Select Register */
  #define REG_PIOD_ELSR    (*(__I  uint32_t*)0x400E12C8U) /**< \brief (PIOD) Edge/Level Status Register */
  #define REG_PIOD_FELLSR  (*(__O  uint32_t*)0x400E12D0U) /**< \brief (PIOD) Falling Edge/Low-Level Select Register */
  #define REG_PIOD_REHLSR  (*(__O  uint32_t*)0x400E12D4U) /**< \brief (PIOD) Rising Edge/ High-Level Select Register */
  #define REG_PIOD_FRLHSR  (*(__I  uint32_t*)0x400E12D8U) /**< \brief (PIOD) Fall/Rise - Low/High Status Register */
  #define REG_PIOD_WPMR    (*(__IO uint32_t*)0x400E12E4U) /**< \brief (PIOD) Write Protection Mode Register */
  #define REG_PIOD_WPSR    (*(__I  uint32_t*)0x400E12E8U) /**< \brief (PIOD) Write Protection Status Register */
  #define REG_PIOD_SCHMITT (*(__IO uint32_t*)0x400E1300U) /**< \brief (PIOD) Schmitt Trigger Register */
  #define REG_PIOD_DRIVER1 (*(__IO uint32_t*)0x400E1318U) /**< \brief (PIOD) I/O Drive Register 1 */
  #define REG_PIOD_DRIVER2 (*(__IO uint32_t*)0x400E131CU) /**< \brief (PIOD) I/O Drive Register 2 */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#endif /* _SAM4C_PIOD_INSTANCE_ */
