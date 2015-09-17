/**
 * \file
 *
 * \brief PRIME Management Layer
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

#ifndef MNGLAYER_H_INCLUDED
#define MNGLAYER_H_INCLUDED

#include "compiler.h"

/* @cond 0 */
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/* @endcond */

/**
 * \ingroup mac_prime_group
 * \defgroup mngl_prime_group PRIME Management Plane
 *
 * This module provides configuration and utils for the
 * Management Plane in PRIME.
 *
 * @{
 */

/* \name Communication profiles */
/* @{ */
#define MNGP_SERIAL_PROFILE                     0x00
#define MNGP_PRIME_PROFILE                      0x01
#define MNGP_IFACE_PROFILE                      0x02
/* @} */

/* \name Commands */
/* @{ */
#define MNGP_PRIME                              0x00
#define MNGP_PRIME_GETQRY                       0x00
#define MNGP_PRIME_GETRSP                       0x01
#define MNGP_PRIME_SET                          0x02
#define MNGP_PRIME_RESET                        0x03
#define MNGP_PRIME_REBOOT                       0x04
#define MNGP_PRIME_FU                           0x05
#define MNGP_PRIME_EN_PIBQRY                    0x06
#define MNGP_PRIME_EN_PIBRSP                    0x07
/* @} */

/* \name Enhanced PIB query types */
/* @{ */
#define MNGP_PRIME_LISTQRY                      0x0E
#define MNGP_PRIME_LISTRSP                      0x0F
/* @} */

/* Length to be used */
#define MAX_LEN_ANSWER                          400

/* * \name PHY PIB attributes */
/* @{ */
#define PIB_PHY_STATS_CRC_INCORRECT             0x00A0
#define PIB_PHY_STATS_CRC_FAIL_COUNT            0x00A1
#define PIB_PHY_STATS_TX_DROP_COUNT             0x00A2
#define PIB_PHY_STATS_RX_DROP_COUNT             0x00A3
#define PIB_PHY_STATS_RX_TOTAL_COUNT            0x00A4
#define PIB_PHY_STATS_BLK_AVG_EVM               0x00A5
#define PIB_PHY_EMA_SMOOTHING                   0x00A8
#define PIB_PHY_TX_QUEUE_LEN                    0x00B0
#define PIB_PHY_RX_QUEUE_LEN                    0x00B1
#define PIB_PHY_TX_PROCESSING_DELAY             0x00B2
#define PIB_PHY_RX_PROCESSING_DELAY             0x00B3
#define PIB_PHY_AGC_MIN_GAIN                    0x00B4
#define PIB_PHY_AGC_STEP_VALUE                  0x00B5
#define PIB_PHY_AGC_STEP_NUMBER                 0x00B6
/* @} */
/* \name MAC variable PIB attributes */
/* @{ */
#define PIB_MAC_MIN_SWITCH_SEARCH_TIME          0x0010
#define PIB_MAC_MAX_PROMOTION_PDU               0x0011
#define PIB_MAC_PROMOTION_PDU_TX_PERIOD         0x0012
#define PIB_MAC_BEACONS_PER_FRAME               0x0013
#define PIB_MAC_SCP_MAX_TX_ATTEMPTS             0x0014
#define PIB_MAC_CTL_RE_TX_TIMER                 0x0015
#define PIB_MAC_SCP_RBO                         0x0016
#define PIB_MAC_SCP_CH_SENCSE_COUNT             0x0017
#define PIB_MAC_MAX_CTL_RE_TX                   0x0018
#define PIB_MAC_EMA_SMOOTHING                   0x0019
/* @} */
/* \name MAC functional PIB attributes */
/* @{ */
#define PIB_MAC_LNID                            0x0020
#define PIB_MAC_LSID                            0x0021
#define PIB_MAC_SID                             0x0022
#define PIB_MAC_SNA                             0x0023
#define PIB_MAC_STATE                           0x0024
#define PIB_MAC_SCP_LENGTH                      0x0025
#define PIB_MAC_NODE_HIERARCHY_LEVEL            0x0026
#define PIB_MAC_BEACON_SLOT_COUNT               0x0027
#define PIB_MAC_BEACON_RX_SLOT                  0x0028
#define PIB_MAC_BEACON_TX_SLOT                  0x0029
#define PIB_MAC_BEACON_RX_FRECUENCY             0x002A
#define PIB_MAC_BEACON_TX_FRECUENCY             0x002B
#define PIB_MAC_MAC_CAPABILITES                 0x002C
/* @} */
/* \name MAC statistical PIB attributes */
/* @{ */
#define PIB_MAC_TX_DATAPKT_COUNT                0x0040
#define PIB_MAC_RX_DATAPKT_COUNT                0x0041
#define PIB_MAC_TX_CTRLPKT_COUNT                0x0042
#define PIB_MAC_RX_CTRLPKT_COUNT                0x0043
#define PIB_MAC_CSMA_FAIL_COUNT                 0x0044
#define PIB_MAC_CSMA_CH_BUSY_COUNT              0x0045
/* @} */
/* \name MAC list PIB attributes */
/* @{ */
#define PIB_MAC_LIST_REGISTER_DEVICES           0x0050
#define PIB_MAC_LIST_ACTIVE_CONN                0x0051
#define PIB_MAC_LIST_MCAST_ENTRIES              0x0052
#define PIB_MAC_LIST_SWITCH_TABLE               0x0053
#define PIB_MAC_LIST_DIRECT_CONN                0x0054
#define PIB_MAC_LIST_DIRECT_TABLE               0x0055
#define PIB_MAC_LIST_AVAILABLE_SWITCHES         0x0056
#define PIB_MAC_LIST_PHY_COMM                   0x0057
#define PIB_MAC_LIST_ACTIVE_CONN_EX             0x0058
/* @} */
/* \name MAC action PIB attributes */
/* @{ */
#define PIB_MAC_ACTION_TX_DATA                  0x0060
#define PIB_MAC_ACTION_CONN_CLOSE               0x0061
#define PIB_MAC_ACTION_REG_REJECT               0x0062
#define PIB_MAC_ACTION_PRO_REJECT               0x0063
#define PIB_MAC_ACTION_UNREGISTER               0x0064
/* @} */
/* \name Base Node certification PIB attributes */
/* @{ */
#define PIB_MAC_ACTION_PROMOTE                  0x0065
#define PIB_MAC_ACTION_DEMOTE                   0x0066
#define PIB_MAC_ACTION_REJECT                   0x0067
#define PIB_MAC_ACTION_CURRENT_ALIVE_TIME       0x0068
#define PIB_MAC_ACTION_PRM                      0x0069
#define PIB_MAC_ACTION_BROADCAST_BURST          0x006A
#define PIB_MAC_ACTION_MGMT_CON                 0x006B
#define PIB_MAC_ACTION_MGMT_MUL                 0x006C
#define PIB_MAC_ACTION_CONN_CLOSE_BN            0x006D
#define PIB_MAC_ACTION_UNREGISTER_BN            0x006E
#define PIB_MAC_ACTION_SEGMENTED_432            0x006F
#define PIB_MAC_ACTION_APPEMU_DATA_BURST        0x0080
/* @} */
/* \name MAC application PIB attributes */
/* @{ */
#define PIB_MAC_APP_FWDL_RUNNING                0x0070
#define PIB_MAC_APP_FWDL_RX_PHT_COUNT           0x0071
#define PIB_MAC_APP_FW_VERSION                  0x0075
#define PIB_MAC_APP_VENDOR_ID                   0x0076
#define PIB_MAC_APP_PRODUCT_ID                  0x0077
/* @} */
/* \name ATMEL PHY PIB attributes */
/* @{ */
#define PIB_PHY_SW_VERSION                      0x8080
#define PIB_PHY_ZCT                             0x8081
#define PIB_PHY_CD                              0x8082
#define PIB_PHY_AGC                             0x8083
#define PIB_PHY_TIMER                           0x8085
#define PIB_PHY_RMS_LAST_TX                     0x8086
#define PIB_PHY_EXECUTE_CALIBRATION             0x8087
#define PIB_PHY_RX_PARAMS                       0x8088
#define PIB_PHY_TX_PARAM                        0x8089
#define PIB_PHY_SEED_GENERATION                 0x808A
#define PIB_PHY_NOISE_LEVEL                     0x808B
#define PIB_PHY_SNR                             0x808C
#define PIB_PHY_BOARD_SERIAL_NUMBER             0x808D
#define PIB_PHY_EOL                             0x808E
#define PIB_PHY_TX_CHANNEL                      0x8090
#define PIB_PHY_RX_PDU                          0xC08F
/* @} */
/* \name ATPL230 PIB attributes */
/* @{ */
#define PIB_PHY_DRV_AUTO                        0x8301
#define PIB_PHY_DRV_IMPEDANCE                   0x8302
#define PIB_PHY_PIOC_CTRL                       0x8303
/* @} */
/* \name ATMEL MAC PIB attributes */
/* @{ */
#define PIB_MAC_EUI48                           0x8100
#define PIB_MAC_PLC_STATE                       0x8101
#define PIB_MAC_SW_VERSION                      0x8102
#define PIB_MAC_PRM_ACTIVATION                  0x8103
#define PIB_MAC_TX_PARAMS                       0x8104
#define PIB_MAC_RX_PARAMS                       0x8105
#define PIB_MAC_SNIFFER_ACTIVATION              0x8106
#define PIB_MAC_NETWORK_EVENTS                  0x8107
#define PIB_MAC_ALV_MODE                        0x8108
#define PIB_MAC_ALV_CHECK_PERIOD                0x8109
#define PIB_MAC_ALV_TIME                        0x810A
#define PIB_MAC_MAX_ALV_INPERIOD                0x810B
#define PIB_MAC_CFP_LENGTH                      0x810D
#define PIB_MAC_BCN_SLOT_COUNT                  0x810E
#define PIB_MAC_BOARD_SERIAL_NUMBER             0x810F
#define PIB_CERTIFICATION_MODE                  0x8120
#define PIB_CERTIFICATION_SEND_MSG              0x8121
#define PIB_MAC_SNA_RW                          0x8123
#define PIB_MAC_ARQ_WINDOW_SIZE                 0x8124
#define PIB_MAC_ENABLE_SAR_CRITICAL             0x8125
#define PIB_MAC_INTERNAL_SW_VERSION             0x8126
#define PIB_MAC_LIST_EX_SNR                     0x8150
#define PIB_432_CONNECTION_STATE                0x8200
#define PIB_432_INTERNAL_SW_VERSION             0x8201
#define PIB_432_LIST_NODES                      0x8250
#define PIB_FU_LIST                             0x8350
/* @} */
/* \name BOOT PIB attributes */
/* @{ */
#define PIB_BOOT_VERSION                        0x8400
/* @} */

/* \name Management Plane interface */
/* @{ */
void mngl_init(void);
void mngl_start(void);
void mngl_process(void);
void mngl_extract_pkt(uint8_t commProf, uint8_t *rxBufPtr, uint16_t length);
uint8_t mngl_rcv_cmd(uint8_t *rxBuff, uint16_t length);

/* @} */

/* @} */

/* @cond 0 */
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/* @endcond */

#endif /* MNGLAYER_H_INCLUDED */
