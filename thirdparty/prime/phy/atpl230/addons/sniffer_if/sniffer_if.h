/**
 * \file
 *
 * \brief ATPL230 Sniffer Interface for Physical layer
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

#ifndef SNIFFER_IF_H_INCLUDED
#define SNIFFER_IF_H_INCLUDED

#include "conf_atpl230.h"
#include "conf_phy.h"

/* @cond 0 */
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/* @endcond */

/**
 * \ingroup phy_plc_group
 * \defgroup sniffer_plc_group PLC PHY Sniffer
 *
 * This module provides configuration and utils for the sniffer in the PLC PHY
 *layer.
 *
 * @{
 */

/* \name Task priorities and stack definitions */
/* @{ */
#define TASK_SNIFFER_IF_GET_RX_PRIO           (tskIDLE_PRIORITY + 1)
#define TASK_SNIFFER_IF_GET_RX_STACK          (configMINIMAL_STACK_SIZE * 1)
/* @} */

/* \name SNIFFER version */
/* @{ */
#define SNIFFER_ATPL230 0x01
#define SNIFFER_VERSION 0x14
/* @} */

/* \name PRIME Time definitions */
/* @{ */
#define TIME_PRIME_1_3_PREAMBLE_US         2048L
#define TIME_PRIME_1_3_HEADER_US           4480L
#define TIME_OFDM_SYMBOL_US                2240L
#define TIME_PRIME_PLUS_PREAMBLE_US        (TIME_PRIME_1_3_PREAMBLE_US * 4)
#define TIME_PRIME_PLUS_HEADER_US          (TIME_OFDM_SYMBOL_US * 4)

#define TIME_IN_TICS(x)                 (x / 10)
/* @} */

/* \name Sniffer interface commands identifiers */
/* @{ */
#define SNIFFER_IF_PHY_COMMAND_MAC_CRC                   1     /* ENABLE MAC on the ATPL230 */
#define SNIFFER_IF_PHY_COMMAND_SET_CHANNEL               2     /* SET PLC channel (1 = CENELEC- A) */
#define SNIFFER_IF_PHY_COMMAND_ENABLE_PRIME_PLUS_ROBUST  3     /* Enable robust modes of PRIME */
#define SNIFFER_IF_PHY_COMMAND_MESSAGE                   4     /* Inject message in PLC */
#define SNIFFER_IF_PHY_MESSAGE_PRIME_1_3                 0x20  /* PRIME_1_3 pdu received */
#define SNIFFER_IF_PHY_MESSAGE_PRIME_PLUS                0x21  /* PRIME_PLUS pdu received */
/* @} */

/* \name Message Structure to communicate with USI layer */
/* @{ */
typedef struct {
	uint8_t uc_protocol_type; /* Protocol Type */
	uint8_t *ptr_buf;         /* Pointer to data buffer */
	uint16_t us_len;          /* Length of data */
} x_usi_sniffer_cmd_params;
/* @} */

/* \name PHY sniffer interface */
/* @{ */
void sniffer_if_init(uint8_t uc_enable_led);
uint8_t serial_if_sniffer_api_parser(uint8_t *puc_rx_msg, uint16_t us_len);
void sniffer_if_process(void);

/* @} */

/* @} */

/* @cond 0 */
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/* @endcond */

#endif /* SNIFFER_IF_H_INCLUDED */
