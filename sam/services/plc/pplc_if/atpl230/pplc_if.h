/**
 * \file
 *
 * \brief Proxy PLC Controller interface layer implementation.
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

#ifndef CHIP_PPLC_IF_H_INCLUDED
#define CHIP_PPLC_IF_H_INCLUDED

#include "compiler.h"

/* @cond 0 */
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/* @endcond */

/**
 * \ingroup plc_group
 * \defgroup pplc_plc_group Proxy PLC Controller
 *
 * This module provides configuration and utils to control the PLC interface
 * with the ATPL230 PHY layer.
 *
 * @{
 */

/* \name Proxy PLC Controller commands (PPLC_CMD) */
/* @{ */
#define PPLC_CMD_READ       0x63 /* < Read operation */
#define PPLC_CMD_WRITE      0x2a /* < Write operation */
#define PPLC_CMD_WRITE_REP  0x1e /* < Repetitive write operation */
#define PPLC_CMD_AND        0x4c /* < AND operation */
#define PPLC_CMD_OR         0x71 /* < OR operation */
#define PPLC_CMD_XOR        0x6d /* < XOR operation */
/* @} */

/* \name Programmable Clock Settings (Hz) */
/* @{ */
#define PPLC_CLOCK          9000000 /* < PPLC clock setting */
/* @} */

/* \name PPLC interruption priority */

/* \note In case of use of FreeRTOS, GROUP_PRIO is greater value than
 * configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY */
/* @{ */
#define PPLC_PRIO          11 /* < PPLC interruption group priority */
/* @} */

/* \name PPLC communication parameters */
/* @{ */
#define PPLC_DLYBS         10 /* < Delay before SPCK */
#define PPLC_DLYBCT        0  /* < Delay between consecutive transfers */
/* @} */

/* \name Proxy PLC Controller interface */
/* @{ */
void pplc_if_init(void);
uint8_t pplc_if_read8(uint16_t us_addr);
uint8_t pplc_if_write8(uint16_t us_addr, uint8_t uc_dat);
uint16_t pplc_if_read16(uint16_t us_addr);
uint8_t pplc_if_write16(uint16_t us_addr, uint16_t us_dat);
uint32_t pplc_if_read32(uint16_t us_addr);
uint8_t pplc_if_write32(uint16_t us_addr, uint32_t ul_dat);
uint8_t pplc_if_read_buf(uint16_t us_addr, uint8_t *buf, uint16_t us_len);
uint8_t pplc_if_write_buf(uint16_t us_addr, uint8_t *buf, uint16_t us_len);
uint8_t pplc_if_write_rep(uint16_t us_addr, uint8_t uc_bytes_rep,
		uint8_t *ptr_buf, uint16_t us_len);
void pplc_if_and8(uint16_t us_addr, uint8_t uc_mask);
void pplc_if_or8(uint16_t us_addr, uint8_t uc_mask);
void pplc_if_xor8(uint16_t us_addr, uint8_t uc_mask);
void pplc_set_handler(void (*p_handler)(void));

/* @} */
/* @} */

/* @cond 0 */
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/* @endcond */

#endif /* CHIP_PPLC_IF_H_INCLUDED */
