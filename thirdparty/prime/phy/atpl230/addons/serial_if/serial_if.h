/**
 * \file
 *
 * \brief ATPL230 Serial Interface for Physical layer
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
