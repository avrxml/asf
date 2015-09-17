/**
 * \file
 *
 * \brief PRIME Header
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

#ifndef PRIME_H_INCLUDE
#define PRIME_H_INCLUDE

#include "compiler.h"

/* @cond 0 */
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/* @endcond */

/**
 * \defgroup prime_group PRIME
 *
 * This module provides configuration and utils for PRIME.
 */

/**
 * \ingroup prime_group
 * \defgroup mac_prime_group PRIME MAC Layer
 *
 * This module provides configuration and utils for the
 * MAC layer in PRIME.
 *
 * @{
 */

enum MacLnidValues {
	PRIME_BROADCAST_LNID = 0x3FFF,
	PRIME_MULTICAST_LNID = 0x3FFE,
};

/* \name Length in bytes of EUI48 identifier of a Node */
/* @{ */
#define PRIME_MAC_LENGTH                         6
/* @} */

/* \name Length in bytes of max pdu */
/* @{ */
#define MAX_PDU_SIZE                             364
/* @} */

/* \name Time interval [msec] to check unexpected PLC chip resets */
/* @{ */
#define PRIME_CHECK_PLC_CFG_TIMER                5000
/* @} */

/* \name PRIME main control interface */
/* @{ */
void prime_init(void);
void prime_start(void);
void prime_process(void);
void prime_upd1ms(void);

/* @} */

/* \name PLME Result values */
/* @{ */
#define PRIME_PLME_RESULT_DONE                   0
#define PRIME_PLME_RESULT_SUCCESS                0
#define PRIME_PLME_RESULT_FAILED                 1
#define PRIME_PLME_RESULT_REJECTED               2
#define PRIME_PLME_RESULT_BADATTR                3
#define PRIME_PLME_RESULT_OUTOFRANGE             4
#define PRIME_PLME_RESULT_READONLY               5
/* @} */

/* \name PLME Commands for callback messages */
/* @{ */
#define PRIME_PLME_RESET_CONFIRM                 1
#define PRIME_PLME_SLEEP_CONFIRM                 2
#define PRIME_PLME_RESUME_CONFIRM                3
#define PRIME_PLME_TESTMODE_CONFIRM              4
#define PRIME_PLME_GET_CONFIRM                   5
#define PRIME_PLME_SET_CONFIRM                   6
/* @} */

/* \name PLME Commands for request messages */
/* @{ */
#define PRIME_PLME_RESET_REQUEST                 20
#define PRIME_PLME_SLEEP_REQUEST                 21
#define PRIME_PLME_RESUME_REQUEST                22
#define PRIME_PLME_TESTMODE_REQUEST              23
#define PRIME_PLME_GET_REQUEST                   24
#define PRIME_PLME_SET_REQUEST                   25
/* @} */

/* Types for PLME information access */

/* Extract PLME command */
typedef struct {
	uint8_t cmd;
} PlmeCommand;

/* Extract PLME result of operation */
typedef struct {
	uint8_t cmd;
	uint8_t result;
} PlmeResult;

/* Extract PLME numeric value */
typedef struct {
	uint8_t cmd;
	uint8_t result;
	uint16_t pibAttrib;
	uint32_t pibValue;
} PlmeGetValue;

/* Extract PLME array value */
typedef struct {
	uint8_t cmd;
	uint8_t result;
	uint16_t pibAttrib;
	uint8_t buf[1];
} PlmeGetArray;

/* Extract PLME SET result */
typedef struct {
	uint8_t cmd;
	uint8_t result;
	uint16_t pibAttrib;
} PlmeSetResult;

/* \name Buffer size needed to receive PLME data */
/* @{ */
#define PRIME_PLME_DATA_SIZE                     364
/* @} */

/* \name PLME Functions interface */
/* @{ */
int prime_PLME_RESET_request(void);
int prime_PLME_SLEEP_request(void);
int prime_PLME_RESUME_request(void);
int prime_PLME_TESTMODE_request(uint8_t enable, uint8_t mode, uint8_t modulation, uint8_t pwr_level);
int prime_PLME_GET_request(uint16_t pibAttribute);
int prime_PLME_SET_request(uint16_t pibAttribute, uint32_t pibAttributeValue);
int prime_PLME_SET_BUF_request(uint16_t pibAttribute, uint8_t *pibBuf, uint16_t pibLen);
int prime_PLME_callback(uint8_t *buf);

/* @} */

/* \name MLME Result values */
/* @{ */
#define PRIME_MLME_RESULT_DONE                   0
#define PRIME_MLME_RESULT_SUCCESS                0
#define PRIME_MLME_RESULT_FAILED                 1
#define PRIME_MLME_RESULT_REJECTED               1
#define PRIME_MLME_RESULT_TIMEOUT                2
#define PRIME_MLME_RESULT_NOSNA                  8
#define PRIME_MLME_RESULT_NOSWITCH               9
#define PRIME_MLME_RESULT_REDUNDANT              10
#define PRIME_MLME_RESULT_BADATTR                11
#define PRIME_MLME_RESULT_OUTOFRANGE             12
#define PRIME_MLME_RESULT_READONLY               13
/* @} */

/* \name MLME Commands for callback messages */
/* @{ */
#define PRIME_MLME_REGISTER_CONFIRM              1
#define PRIME_MLME_REGISTER_INDICATION           2
#define PRIME_MLME_UNREGISTER_CONFIRM            3
#define PRIME_MLME_UNREGISTER_INDICATION         4
#define PRIME_MLME_PROMOTE_CONFIRM               5
#define PRIME_MLME_PROMOTE_INDICATION            6
#define PRIME_MLME_DEMOTE_CONFIRM                7
#define PRIME_MLME_DEMOTE_INDICATION             8
#define PRIME_MLME_RESET_CONFIRM                 9
#define PRIME_MLME_GET_CONFIRM                   10
#define PRIME_MLME_LIST_GET_CONFIRM              11
#define PRIME_MLME_SET_CONFIRM                   12
/* @} */

/* \name MLME Commands for request messages */
/* @{ */
#define PRIME_MLME_REGISTER_REQUEST              20
#define PRIME_MLME_UNREGISTER_REQUEST_SERVICE    21
#define PRIME_MLME_PROMOTE_REQUEST_SERVICE       22
#define PRIME_MLME_DEMOTE_REQUEST_SERVICE        23
#define PRIME_MLME_RESET_REQUEST                 24
#define PRIME_MLME_GET_REQUEST                   25
#define PRIME_MLME_LIST_GET_REQUEST              26
#define PRIME_MLME_SET_REQUEST                   27
#define PRIME_MLME_UNREGISTER_REQUEST_BASE       29
#define PRIME_MLME_PROMOTE_REQUEST_BASE          30
#define PRIME_MLME_DEMOTE_REQUEST_BASE           31
/* @} */

/* \name MLME definitions */
/* @{ */
/* Buffer size needed to receive MLME data */
#define MLME_DATA_SIZE                           364

/* Number of registered devices per message */
#define MLME_LIST_REGDEVNUM                      15

/* Number of active connections per message */
#define MLME_LIST_ACTCONNUM                      22

/* Number of multicast entries per message */
#define MLME_LIST_MCASTNUM                       83

/* Number of Switch Nodes entries per message */
#define MLME_LIST_SWNODENUM                      64

/* Number of direct connections per message */
#define MLME_LIST_DIRCONNUM                      8

/* Number of direct connections per message */
#define MLME_LIST_DIRSWNODENUM                   15

/* Number of Available Switch Nodes entries per message */
#define MLME_LIST_AVAILSWNODENUM                 22

/* Number of Switch Nodes entries per message */
#define MLME_LIST_PHYCOMMNUM                     17

/* Number of active connections per message */
#define MLME_LIST_ACTCONNUM_EX                   20

/* Number of 432 Nodes per message */
#define MLME_LIST_432_NODES                      7

/* Maximum number of extended SNR elements per message */
#define MLME_LIST_EXSNR_DEVNUM                   80

/* Number of FU Nodes per message */
#define MLME_LIST_FU_NODES                       20
/* @} */

/* PLC states */
enum MacPlcStateValues {
	MAC_PLC_STATE_DISCONNECTED           = 0,
	MAC_PLC_STATE_DETECTION              = 1,
	MAC_PLC_STATE_REGISTERING            = 2,
	MAC_PLC_STATE_OPERATIVE              = 3,
	MAC_PLC_STATE_BASE                   = 4,
};

/* Types for MLME information access */

/* Extract MLME command */
typedef struct {
	uint8_t cmd;
} MlmeCommand;

/* Extract MLME result of operation */
typedef struct {
	uint8_t cmd;
	uint8_t result;
} MlmeResult;

/* Extract MLME numeric value */
typedef struct {
	uint8_t cmd;
	uint8_t result;
	uint16_t pibAttrib;
	uint32_t pibValue;
} MlmeGetValue;

/* Extract MLME array value */
typedef struct {
	uint8_t cmd;
	uint8_t result;
	uint16_t pibAttrib;
	uint8_t buf[1];
} MlmeGetArray;

/* Extract MLME SET result */
typedef struct {
	uint8_t cmd;
	uint8_t result;
	uint16_t pibAttrib;
} MlmeSetResult;

/* Extract MLME results */
typedef struct {
	uint8_t headerType;
	uint8_t ctrlPktFlag;
	uint16_t lcid;
	uint8_t type;
} MlmeParam;

/* Value for MLME Register Confirm command */
typedef struct {
	uint8_t cmd;
	uint8_t result;
	uint8_t sna[PRIME_MAC_LENGTH];
	uint8_t mac[PRIME_MAC_LENGTH];
} MlmeRegisterConfirm;

/* Value for MLME Register Indication command */
typedef struct {
	uint8_t cmd;
	uint8_t sna[PRIME_MAC_LENGTH];
	uint8_t mac[PRIME_MAC_LENGTH];
} MlmeRegisterIndication;

/* Header for MLME ListGet commands */
typedef struct {
	uint8_t cmd;
	uint8_t result;
	uint16_t pibAttrib;
	uint8_t isLast;
	uint8_t numItems;
} MlmeListGetHeader;

/* Information of Registered Device */
typedef struct {
	uint8_t mac[PRIME_MAC_LENGTH];
	uint8_t state;
	uint8_t level;
	uint8_t tCap;
	uint8_t swCap;
	uint16_t lnid;
	uint16_t lsid;
	uint16_t sid;
} MlmeRegDevice;

/* ListGet for Registered	Devices */
typedef struct {
	uint8_t cmd;
	uint8_t result;
	uint16_t pibAttrib;
	uint8_t isLast;
	uint8_t numItems;
	MlmeRegDevice dev[MLME_LIST_REGDEVNUM];
} MlmeListGetRegDevices;

/* Information of Active Connections */
typedef struct {
	uint8_t mac[PRIME_MAC_LENGTH];
	uint16_t sid;
	uint16_t lnid;
	uint8_t lcid;
} MlmeActiveConn;

/* ListGet for Active Connections */
typedef struct {
	uint8_t cmd;
	uint8_t result;
	uint16_t pibAttrib;
	uint8_t isLast;
	uint8_t numItems;
	MlmeActiveConn conn[MLME_LIST_ACTCONNUM];
} MlmeListGetActiveConn;

/* Information of Active Connections (Extended List) */
typedef struct {
	uint8_t mac[PRIME_MAC_LENGTH];
	uint16_t sid;
	uint16_t lnid;
	uint16_t lcid;
	uint8_t type;
} MlmeActiveConnEx;

/* ListGet for Active Connections  (Extended List) */
typedef struct {
	uint8_t cmd;
	uint8_t result;
	uint16_t pibAttrib;
	uint8_t isLast;
	uint8_t numItems;
	MlmeActiveConnEx conn[MLME_LIST_ACTCONNUM_EX];
} MlmeListGetActiveConnEx;

/* Information of Multicast Entries */
typedef struct {
	uint16_t member;
	uint8_t lcid;
} MlmeMcastEntrie;

/* ListGet for Multicast Entries */
typedef struct {
	uint8_t cmd;
	uint8_t result;
	uint16_t pibAttrib;
	uint8_t isLast;
	uint8_t numItems;
	MlmeMcastEntrie ent[MLME_LIST_MCASTNUM];
} MlmeListGetMcastEntries;

/* Information of Switch Table */
typedef struct {
	uint16_t sid;
} MlmeSwitchNode;

/* ListGet for Switch Table */
typedef struct {
	uint8_t cmd;
	uint8_t result;
	uint16_t pibAttrib;
	uint8_t isLast;
	uint8_t numItems;
	MlmeSwitchNode nod[MLME_LIST_SWNODENUM];
} MlmeListGetSwitchTable;

/* Information of Direct Connections */
typedef struct {
	uint8_t srcmac[PRIME_MAC_LENGTH];
	uint16_t srcsid;
	uint16_t srclnid;
	uint8_t srclcid;
	uint8_t dstmac[PRIME_MAC_LENGTH];
	uint16_t dstsid;
	uint16_t dstlnid;
	uint8_t dstlcid;
	uint8_t dmac[PRIME_MAC_LENGTH];
	uint16_t dsid;
} MlmeDirectConn;

/* ListGet for Direct Connections */
typedef struct {
	uint8_t cmd;
	uint8_t result;
	uint16_t pibAttrib;
	uint8_t isLast;
	uint8_t numItems;
	MlmeDirectConn conn[MLME_LIST_DIRCONNUM];
} MlmeListGetDirectConn;

/* Information of Direct Switch Table */
typedef struct {
	uint8_t dsmac[PRIME_MAC_LENGTH];
	uint16_t srcsid;
	uint16_t srclnid;
	uint8_t srclcid;
	uint16_t dstsid;
	uint16_t dstlnid;
	uint8_t dstlcid;
} MlmeDirectSwitchNode;

/* ListGet for Direct Switch Table */
typedef struct {
	uint8_t cmd;
	uint8_t result;
	uint16_t pibAttrib;
	uint8_t isLast;
	uint8_t numItems;
	MlmeDirectSwitchNode nod[MLME_LIST_DIRSWNODENUM];
} MlmeListGetDirectSwitchTable;

/* Information of Available Switch Table */
typedef struct {
	uint8_t sna[PRIME_MAC_LENGTH];
	uint16_t sid;
	uint8_t level;
	uint8_t rxLevel;
	uint8_t rxSNR;
} MlmeAvailableSwitchNode;

/* ListGet for Available Switch Table */
typedef struct {
	uint8_t cmd;
	uint8_t result;
	uint16_t pibAttrib;
	uint8_t isLast;
	uint8_t numItems;
	MlmeAvailableSwitchNode nod[MLME_LIST_AVAILSWNODENUM];
} MlmeListGetAvailableSwitchTable;

/* Information of Phy Communication Parameters Table */
typedef struct {
	uint8_t eui48[PRIME_MAC_LENGTH];
	uint8_t txPower;
	uint8_t txCoding;
	uint8_t rxCoding;
	uint8_t rxLevel;
	uint8_t snr;
	uint8_t txPowerMod;
	uint8_t txCodingMod;
	uint8_t rxCodingMod;
} MlmePhyCommDevice;

/* ListGet for Phy Communication Parameters Table */
typedef struct {
	uint8_t cmd;
	uint8_t result;
	uint16_t pibAttrib;
	uint8_t isLast;
	uint8_t numItems;
	MlmePhyCommDevice dev[MLME_LIST_PHYCOMMNUM];
} MlmeListGetPhyCommTable;

/* List of extended SNR (PIB 0x8150) */
typedef struct {
	uint8_t sid;
	uint16_t lnid;
	uint8_t exSnr;
} ExSNRElementType;

/* ListGet for Phy Communication Parameters Table */
typedef struct {
	uint8_t cmd;
	uint8_t result;
	uint16_t pibAttrib;
	uint8_t isLast;
	uint8_t numItems;
	ExSNRElementType exSnrDev[MLME_LIST_EXSNR_DEVNUM];
} MlmeListGetExSnrTable;

/* Equal to LEN_432_SERIAL_NUMBER defined in 432 layer */
#define SERIAL_NUMBER_432_MAC                    16

/* List of 432 Information Nodes if exists 432 (PIB 0x8250) */
typedef struct {
	uint16_t address;
	int8_t serialNumber[SERIAL_NUMBER_432_MAC];
	uint8_t lenSerial;
	uint8_t mac[PRIME_MAC_LENGTH];
	uint8_t arq;
} Node432Type;

/* ListGet for Phy Communication Parameters Table */
typedef struct {
	uint8_t cmd;
	uint8_t result;
	uint16_t pibAttrib;
	uint8_t isLast;
	uint8_t numItems;
	Node432Type dev[MLME_LIST_432_NODES];
} MlmeListGetNode432Table;

/* List of Nodes in the FU target list (PIB 0x8350) */
typedef struct {
	uint8_t fuState;
	uint32_t pagesReceived;
	uint8_t mac[PRIME_MAC_LENGTH];
} FuNodeTargetType;

/* ListGet for FU target list */
typedef struct {
	uint8_t cmd;
	uint8_t result;
	uint16_t pibAttrib;
	uint8_t isLast;
	uint8_t numItems;
	FuNodeTargetType dev[MLME_LIST_FU_NODES];
} MlmeListFUTable;

/* \name MLME Functions interface */
/* @{ */
int prime_MLME_REGISTER_request(uint8_t *sna, uint8_t *mac);
int prime_MLME_UNREGISTER_request_base(uint8_t *mac);
int prime_MLME_UNREGISTER_request_service(void);
int prime_MLME_PROMOTE_request_base(uint8_t *mac);
int prime_MLME_PROMOTE_request_service(void);
int prime_MLME_DEMOTE_request_base(uint8_t *mac);
int prime_MLME_DEMOTE_request_service(void);
int prime_MLME_RESET_request(void);
int prime_MLME_GET_request(uint16_t pibAttribute);
int prime_MLME_LIST_GET_request(uint16_t pibListAttribute);
int prime_MLME_SET_request(uint16_t pibAttribute, uint32_t pibAttributeValue);
int prime_MLME_SET_BUF_request(uint16_t pibAttribute, uint8_t *pibBuf, uint8_t pibLen);
int prime_MLME_callback(uint8_t *buf);

/* @} */

/* \name MAC_SAP Result values */
/* @{ */
#define PRIME_MACSAP_RESULT_SUCCESS              0
#define PRIME_MACSAP_RESULT_ACCEPT               0
#define PRIME_MACSAP_RESULT_ERROR                1
#define PRIME_MACSAP_RESULT_REJECT               1
#define PRIME_MACSAP_RESULT_TIMEOUT              2
#define PRIME_MACSAP_RESULT_NO_BANDWITH          3
#define PRIME_MACSAP_RESULT_NO_SUCH_DEVICE       4
#define PRIME_MACSAP_RESULT_REDIRECT_FAILED      5
#define PRIME_MACSAP_RESULT_NOT_REGISTERED       6
#define PRIME_MACSAP_RESULT_NO_MORE_LCIDS        7
/* @} */

/* \name MAC_SAP Error Type values */
/* @{ */
#define PRIME_MACSAP_ERROR_NO_ERROR              0
#define PRIME_MACSAP_ERROR_REJECT                1
#define PRIME_MACSAP_ERROR_TIMEOUT               2
#define PRIME_MACSAP_ERROR_NO_SUCH_DEVICE        4
#define PRIME_MACSAP_ERROR_NOT_REGISTERED        6
#define PRIME_MACSAP_ERROR_INVALID_HANDLER       8
#define PRIME_MACSAP_ERROR_NO_RESOURCES          9
#define PRIME_MACSAP_ERROR_PROCESS_RUNNING       10
#define PRIME_MACSAP_ERROR_TOO_LONG              11
#define PRIME_MACSAP_ERROR_CON_NOT_OPEN          12
#define PRIME_MACSAP_ERROR_NO_ACKED              13
#define PRIME_MACSAP_ERROR_TX_BUSY               14
#define PRIME_MACSAP_ERROR_INVALID_BCAST         15
#define PRIME_MACSAP_ERROR_INVALID_DIRCON        16
#define PRIME_MACSAP_ERROR_INVALID_EUI48         17
/* @} */

/* \name MAC_SAP Commands for callback messages */
/* @{ */
#define PRIME_MACSAP_ESTABLISH_CONFIRM           1
#define PRIME_MACSAP_ESTABLISH_INDICATION        2
#define PRIME_MACSAP_RELEASE_CONFIRM             3
#define PRIME_MACSAP_RELEASE_INDICATION          4
#define PRIME_MACSAP_DATA_CONFIRM                5
#define PRIME_MACSAP_DATA_INDICATION             6
#define PRIME_MACSAP_JOIN_CONFIRM                8
#define PRIME_MACSAP_LEAVE_CONFIRM               9
#define PRIME_MACSAP_JOIN_INDICATION_BASE        10
#define PRIME_MACSAP_LEAVE_INDICATION_BASE       11
#define PRIME_MACSAP_JOIN_INDICATION_SERVICE     12
#define PRIME_MACSAP_LEAVE_INDICATION_SERVICE    13
/* @} */

/* \name MAC_SAP Commands for callback messages (Request & Response) */
/* @{ */
#define PRIME_MACSAP_ESTABLISH_REQUEST           20
#define PRIME_MACSAP_ESTABLISH_RESPONSE          21
#define PRIME_MACSAP_REDIRECT_RESPONSE           22
#define PRIME_MACSAP_RELEASE_REQUEST             23
#define PRIME_MACSAP_RELEASE_RESPONSE            24
#define PRIME_MACSAP_SEND_REQUEST                25
#define PRIME_MACSAP_DATA_REQUEST                26
#define PRIME_MACSAP_JOIN_REQUEST_BASE           27
#define PRIME_MACSAP_LEAVE_REQUEST_BASE          28
#define PRIME_MACSAP_JOIN_RESPONSE_BASE          29
#define PRIME_MACSAP_JOIN_REQUEST_SERVICE        30
#define PRIME_MACSAP_LEAVE_REQUEST_SERVICE       31
#define PRIME_MACSAP_JOIN_RESPONSE_SERVICE       32
/* @} */

/* \name MAX Buffer size to transmit or receive MAC data */
/* @{ */
#define PRIME_MACSAP_DATA_SIZE                   1024
/* @} */

/* \name Message Priorities */
/* @{ */
#define MAX_PRIO                                 0
#define PRIO_1                                   1
#define PRIO_2                                   2
#define PRIO_3                                   3
/* @} */

/* \name Default Priority for Data Messages */
/* @{ */
#define DATA_PRIO                                2
/* @} */

/* \name Type values defined for different CS defined in PRIME Spec v1.3 */
/* @{ */
#define TYPE_CL_IPv4_AR                          1
#define TYPE_CL_IPv4_UNICAST                     2
#define TYPE_CL_432                              3
#define TYPE_CL_MGMT                             4
#define TYPE_CL_APPEMU                           250
/* @} */

/* \name LCI values for broadcast connections defined in PRIME Spec v1.3 */
/* @{ */
#define LCI_CL_IPv4_BROADCAST                    1
#define LCI_CL_432_BROADCAST                     2
/* @} */

/* \name Generic Handler and Type values to use on MAC SAP Callback */
/* @{ */
#define GENERIC_CALLBACK_TYPE                    0
#define GENERIC_CALLBACK_HANDLER                 0xFFFF
/* @} */

/* Type for callback information */
typedef struct {
	uint8_t command;
	uint8_t type;
	uint16_t handler;
	uint16_t answer;
	uint8_t errorType;
	uint16_t bufLength;
	uint16_t cfpBytes;
	uint16_t lcid;
	uint8_t arq;
	uint8_t priority;
	uint8_t broadcast;
	uint8_t macAddr[PRIME_MAC_LENGTH];
	uint8_t buf[PRIME_MACSAP_DATA_SIZE];
} MacSapCallBack;

/* \name MAC Functions Interface */
/* @{ */
int prime_MAC_ESTABLISH_request(uint8_t *macAddr, uint8_t type, uint8_t *buf, uint16_t dataLength, uint8_t arq, uint16_t cfpBytes);
int prime_MAC_ESTABLISH_response(uint16_t conHandle, uint8_t answer, uint8_t *buf, uint16_t dataLength);
int prime_MAC_RELEASE_request(uint16_t conHandle);
int prime_MAC_RELEASE_response(uint16_t conHandle, uint8_t answer);
int prime_MAC_JOIN_request_base(uint8_t broadcast, uint16_t handler, uint8_t *macAddr, uint8_t type, uint8_t *buf, uint16_t dataLength);
int prime_MAC_JOIN_request_service(uint8_t broadcast, uint8_t type, uint8_t *buf, uint16_t dataLength);
int prime_MAC_JOIN_response_base(uint16_t handler, uint8_t *macAddr, uint8_t answer);
int prime_MAC_JOIN_response_service(uint16_t handler, uint8_t answer);
int prime_MAC_LEAVE_request_base(uint16_t conHandle, uint8_t *macAddr);
int prime_MAC_LEAVE_request_service(uint16_t conHandle);
int prime_MAC_REDIRECT_response(uint16_t conHandle, uint8_t *macAddr, uint8_t *buf, uint16_t dataLength);
int prime_MAC_DATA_request(uint16_t conHandle, uint8_t *buf, uint16_t dataLength, uint8_t priority);
int prime_MAC_callback(MacSapCallBack *info, uint16_t handler, uint8_t type);

/* @} */

/* \name Number of different types */
/* @{ */
#define PRIME_TYPE_NUM                           255
/* @} */

/* \name Prime reserved Interface */
/* @{ */
int8_t mac_sap_reserve_type(uint8_t type);
int8_t mac_sap_free_type(uint8_t type);
int8_t mac_sap_get_reserve_type(uint8_t type);

int8_t mac_sap_reserve_handler(uint16_t handler, uint16_t iNID, uint8_t type);
int8_t mac_sap_free_handler(uint16_t connHandler);
int8_t mac_sap_get_reserve_handler(uint16_t handler);

int prime_MAC_callback_reserved(MacSapCallBack *info, uint16_t handler, uint8_t type);

/* @} */

/* \name Types for 432 information access */
/* @{ */
/* Extract 432 connections table */
typedef struct _Node432 {
	uint16_t handler;
	int8_t serialNumber[SERIAL_NUMBER_432_MAC];
	uint8_t lenSerial;
	uint8_t mac[PRIME_MAC_LENGTH];
	uint8_t arq;
	uint16_t next;
	uint16_t prev;
} Node432;
/* @} */

/* \name Macros */
/* @{ */
#define GET_INDEX_NODE_432(X)                    (X - 2)
#define GET_ADDRESS_NODE_432(X)                  (X + 2)
/* @} */

/* \name Embedded Sniffer Interface */
/* @{ */
#define MASK_SNIFFER_EMBEDDED                    0x80
/* @} */

/* \name Sniffer types */
/* @{ */
#define SNIFFER_ATPL210                          0x00
#define SNIFFER_ATPL240                          0x01
/* @} */

typedef enum {
	SNIFFER_RX = 0,
	SNIFFER_TX = 1,
	SNIFFER_TX_SERIAL = 2,
	SNIFFER_RX_SERIAL = 3
} SnifferType;

typedef struct {
	SnifferType type;
	uint8_t scheme;
	uint8_t level;
	uint8_t exSnr;
	uint32_t time;
} SnifferParam;

/* @} */

/* @cond 0 */
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/* @endcond */
#endif /* PRIME_H_INCLUDE */
