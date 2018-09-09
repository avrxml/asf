/**
 * \file
 *
 * \brief ATPL230 Sniffer Interface for Physical layer
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
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
