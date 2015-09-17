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

#ifndef _SAM4N_UART3_INSTANCE_
#define _SAM4N_UART3_INSTANCE_

/* ========== Register definition for UART3 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_UART3_CR            (0x40048000U) /**< \brief (UART3) Control Register */
#define REG_UART3_MR            (0x40048004U) /**< \brief (UART3) Mode Register */
#define REG_UART3_IER           (0x40048008U) /**< \brief (UART3) Interrupt Enable Register */
#define REG_UART3_IDR           (0x4004800CU) /**< \brief (UART3) Interrupt Disable Register */
#define REG_UART3_IMR           (0x40048010U) /**< \brief (UART3) Interrupt Mask Register */
#define REG_UART3_SR            (0x40048014U) /**< \brief (UART3) Status Register */
#define REG_UART3_RHR           (0x40048018U) /**< \brief (UART3) Receive Holding Register */
#define REG_UART3_THR           (0x4004801CU) /**< \brief (UART3) Transmit Holding Register */
#define REG_UART3_BRGR          (0x40048020U) /**< \brief (UART3) Baud Rate Generator Register */
#else
#define REG_UART3_CR   (*(WoReg*)0x40048000U) /**< \brief (UART3) Control Register */
#define REG_UART3_MR   (*(RwReg*)0x40048004U) /**< \brief (UART3) Mode Register */
#define REG_UART3_IER  (*(WoReg*)0x40048008U) /**< \brief (UART3) Interrupt Enable Register */
#define REG_UART3_IDR  (*(WoReg*)0x4004800CU) /**< \brief (UART3) Interrupt Disable Register */
#define REG_UART3_IMR  (*(RoReg*)0x40048010U) /**< \brief (UART3) Interrupt Mask Register */
#define REG_UART3_SR   (*(RoReg*)0x40048014U) /**< \brief (UART3) Status Register */
#define REG_UART3_RHR  (*(RoReg*)0x40048018U) /**< \brief (UART3) Receive Holding Register */
#define REG_UART3_THR  (*(WoReg*)0x4004801CU) /**< \brief (UART3) Transmit Holding Register */
#define REG_UART3_BRGR (*(RwReg*)0x40048020U) /**< \brief (UART3) Baud Rate Generator Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#endif /* _SAM4N_UART3_INSTANCE_ */
