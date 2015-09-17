/**
 * \file
 *
 * \brief PLC PRIME CRC calculation service
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

#ifndef PCRC_H_INCLUDED
#define PCRC_H_INCLUDED

/* @cond 0 */
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/* @endcond */

/**
 * \ingroup plc_group
 * \defgroup pcrc_plc_group PLC PRIME CRC Calculation
 *
 * This module provides configuration and utils to calculate the CRC
 * for PRIME messages in a PLC network.
 *
 * @{
 */

/* \name CRC types */

/*** WARNING: duplicated definitions ***
 *   Keep numerically consistent with those in atpl230.h
 */
enum PCRC_CRC_types {
	PCRC_CRC_TYPE_8 = 0,
	PCRC_CRC_TYPE_16 = 1,
	PCRC_CRC_TYPE_24 = 2,
	PCRC_CRC_TYPE_32 = 3,
};

/* \name Header type */

/*** WARNING: duplicated definitions ***
 *   Keep numerically consistent with those in atpl230.h
 */
/* @{ */
/* Header type: GENERIC PACKET */
#define PCRC_HT_GENERIC               0
/* Header type: PROMOTION PACKET */
#define PCRC_HT_PROMOTION             1
/* Header type: BEACON PACKET */
#define PCRC_HT_BEACON                2
/* Header type: USI message */
#define PCRC_HT_USI                   3
/* @} */

/* Invalid CRC */
#define PCRC_CRC_INVALID 0xFFFFFFFF

/* CRC length for the promotion needed PDUs */
#define PCRC_PROMOTION_CRC_LENGTH 13

/* \name PLC PRIME CRC Calculation interface */
/* @{ */
uint32_t pcrc_calculate_prime_crc(uint8_t *puc_buf, uint32_t ul_len,
		uint8_t uc_header_type, uint8_t uc_crc_type);
void pcrc_configure_sna(uint8_t *sna);

/* @} */

/* @} */

/* @cond 0 */
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/* @endcond */

#endif /* PCRC_H_INCLUDED */
