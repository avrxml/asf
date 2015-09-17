/**
 * \file
 *
 * \brief ATPL230 Serial Interface for Physical layer
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

#ifndef SERIAL_IF_H_INCLUDED
#define SERIAL_IF_H_INCLUDED

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
 * \defgroup serial_plc_group PLC PHY Serial Interface
 *
 * This module provides configuration and utils to serialize the PLC PHY layer.
 *
 * @{
 */

/* \name Masks to enable the serial interface and/or the sniffer */
/* @{ */
#define IFACE_SERIAL_ENABLE  0x1
#define IFACE_SNIFFER_ENABLE 0x2
/* @} */

/* \name Task priorities and stack definitions */
/* @{ */
#define TASK_SERIAL_IF_GET_TX_RESULT_PRIO    (tskIDLE_PRIORITY + 1)
#define TASK_SERIAL_IF_GET_RX_PRIO           (tskIDLE_PRIORITY + 1)
#define TASK_SERIAL_IF_GET_TX_RESULT_STACK   (configMINIMAL_STACK_SIZE * 1)
#define TASK_SERIAL_IF_GET_RX_STACK          (configMINIMAL_STACK_SIZE * 1)
/* @} */

/* \name Serial interface commands identifiers */
/* @{ */
#define SERIAL_IF_PHY_COMMAND_GET_CFG               0   /* Get reception configuration query */
#define SERIAL_IF_PHY_COMMAND_GET_CFG_RSP           1   /* Get reception configuration response */
#define SERIAL_IF_PHY_COMMAND_SET_CFG               2   /* Set reception configuration command */
#define SERIAL_IF_PHY_COMMAND_SET_CFG_RSP           3   /* Set reception configuration response */
#define SERIAL_IF_PHY_COMMAND_CMD_CFG               4   /* Get reception configuration query */
#define SERIAL_IF_PHY_COMMAND_CMD_CFG_RSP           5   /* Get reception configuration response */
#define SERIAL_IF_PHY_COMMAND_SEND_MSG              6   /* Send message data */
#define SERIAL_IF_PHY_COMMAND_SEND_MSG_RSP          7   /* Send message data response */
#define SERIAL_IF_PHY_COMMAND_RECEIVE_MSG           8   /* Receive message data */
#define SERIAL_IF_PHY_COMMAND_NOISE_REQ             9   /* Set noise capture mode */
#define SERIAL_IF_PHY_COMMAND_NOISE_RSP             10  /* Get noise capture */
#define SERIAL_IF_PHY_COMMAND_GET_CFG_LIST          11  /* Get parameter list */
#define SERIAL_IF_PHY_COMMAND_GET_CFG_LIST_RSP      12  /* Parameter list response */
#define SERIAL_IF_PHY_COMMAND_RESET_PHY_LAYER       13  /* Reset phy layer */
/* @} */

/* \name Message Structure to communicate with USI layer */
/* @{ */
typedef struct {
	uint8_t uc_protocol_type; /* Protocol Type */
	uint8_t *ptr_buf;         /* Pointer to data buffer */
	uint16_t us_len;          /* Length of data */
} x_usi_serial_cmd_params;
/* @} */

/* \name PHY serial interface */
/* @{ */
void serial_if_init(void);
uint8_t serial_if_api_parser(uint8_t *puc_rx_msg, uint16_t us_len);
void serial_if_check_tx_result(void);
void serial_if_check_rx_msg(void);

/* @} */

/* @} */

/* @cond 0 */
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/* @endcond */

#endif   /* SERIAL_IF_H_INCLUDED */
