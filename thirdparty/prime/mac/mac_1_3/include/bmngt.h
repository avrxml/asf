/**
 * \file
 *
 * \brief PRIME Base Management
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
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

#ifndef BaseMngH
#define BaseMngH

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
 * \defgroup bmngt_prime_group PRIME Base Management Protocol
 *
 * This module provides configuration and utils for the
 * Base Managament Protocol in PRIME.
 *
 * @{
 */

#define MAC_SIZE                   6
#define FW_VERSION_SIZE            32
#define MAX_BUFFER_RCV             500

/* FUP request messages */
enum FupReqValues {
	FUP_CLEARTARGETLIST_REQUEST             = 0x14,
	FUP_ADDTARGET_REQUEST                   = 0x15,
	FUP_SETFWDATA_REQUEST                   = 0x16,
	FUP_SETUPGOPTIONS_REQUEST               = 0x17,
	FUP_INITFILETX_REQUEST                  = 0x18,
	FUP_DATAFRAME_REQUEST                   = 0x19,
	FUP_CRC_REQUEST                         = 0x1A,
	FUP_ABORT_FUP_REQUEST                   = 0x1B,
	FUP_START_FU_REQUEST                    = 0x1C,
	FUP_SETMATCHRULES_REQUEST               = 0x1D,
	FUP_VERSION_REQUEST                     = 0x1E,
	FUP_STATE_REQUEST                       = 0x1F,
};

/* Base Management banned MAC list request messages */
enum BaseMngBannedMacValues {
	BASEMNG_ADDBANNEDMACLIST_REQUEST        = 0x28,
	BASEMNG_REMOVEBANNEDMACLIST_REQUEST     = 0x29,
	BASEMNG_CLEARBANNEDMACLIST_REQUEST      = 0x2A,
	BASEMNG_ENABLEBANNEDMACLIST_REQUEST     = 0x2B,
	BASEMNG_DISABLEBANNEDMACLIST_REQUEST    = 0x2C,
	BASEMNG_MODIFYBANNEDMACLIST_REQUEST     = 0x2D,
	BASEMNG_GETBANNEDMACLIST_REQUEST        = 0x2E,
};

/* PRIME Profile request messages */
enum PrimeProfReqValues {
	PPROF_GETPIB_REQUEST                    = 0x30,
	PPROF_SETPIB_REQUEST                    = 0x31,
	PPROF_RESETSTAT_REQUEST                 = 0x32,
	PPROF_REBOOTDEV_REQUEST                 = 0x33,
};

/* \name Base Management response */
/* @{ */
#define BASEMNG_ACK                         1
/* @} */

/* FUP indication messages */
enum FupValues {
	FUP_ACK                                 = 1,
	FUP_STATUS_INDICATION                   = 3,
	FUP_ERROR_INDICATION                    = 4,
	FUP_VERSION_INDICATION                  = 5,
	FUP_KILL_INDICATION                     = 6,
};

/* \name Base Mangement banned MAC list response */
/* @{ */
#define BASEMNG_GETBANNEDMACLIST_RESPONSE   0x10
/* @} */

/* \name PRIME Profile response */
/* @{ */
#define PPROF_GETPIB_RESPONSE               0x20
/* @} */

/* \name Base Management event network */
/* @{ */
#define BASEMNG_EVENT_NETWORK               0x28
#define BASEMNG_EVENT_ALIVE_NETWORK         0x29
/* @} */

/* \name Base Management ACK response codes */
/* @{ */
#define BASEMNG_ACK_OK                      0
#define BASEMNG_ACK_ERROR                   1
/* @} */

/* FUP ACK response codes */
enum FupAckValues {
	FUP_ACK_OK                              = 0,
	FUP_ACK_ERROR                           = 1,
	FUP_ACK_ERROR_MACADD                    = 2,
	FUP_ACK_ERROR_VENDOR                    = 3,
	FUP_ACK_ERROR_MODEL                     = 4,
	FUP_ACK_ERROR_CRC                       = 5,
	FUP_ACK_ERROR_DATA                      = 6,
	FUP_ACK_ERROR_CRCFILE                   = 7,
	FUP_ACK_CRC_INPROCESS                   = 8,
	FUP_ACK_FU_INPROCESS                    = 9,
};

/* FUP states */
enum FupStates {
	FUP_STATE_IDLE                          = 0,
	FUP_STATE_INIT                          = 1,
	FUP_STATE_JOIN                          = 2,
	FUP_STATE_MULT                          = 3,
	FUP_STATE_UNIC                          = 4,
	FUP_STATE_RSTR                          = 5,
	FUP_STATE_CFRM                          = 6,
	FUP_STATE_END                           = 7,
	FUP_STATE_KILL                          = 8,
	FUP_STATE_CNTDWN                        = 9,
};

/* FUP error type */
enum FupErrorValues {
	FU_ERROR_NODE_NOERROR                   = 0,
	FU_ERROR_NODE_WRONGSTATE_EXEC           = 1,
	FU_ERROR_NODE_WRONGSTATE_UPG            = 2,
	FU_ERROR_NODE_WRONGSTATE                = 3,
	FU_ERROR_NODE_WRONGSTATE_RCV            = 4,
	FU_ERROR_NODE_WRONGSTATE_CNTDWN         = 5,
	FU_ERROR_NODE_FW_NOTMATCH               = 6,
	FU_ERROR_NODE_REVERT_ERRORINIT          = 7,
	FU_ERROR_NODE_REVERT_ERR7               = 8,
	FU_ERROR_NODE_RETRY_KILL                = 9,
	FU_ERROR_NODE_UNICAST_TIMEOUT           = 10,
	FU_ERROR_NODE_CONFIRM_IND               = 11,
	FU_ERROR_NODE_REVERT_CRCNOK             = 12,
	FU_ERROR_NODE_RESTART                   = 13,
	FU_ERROR_NODE_WRONG_MISS_BITMAP         = 14,
	FU_ERROR_NODE_WRONG_MISS_LIST           = 15,
	FU_ERROR_NODE_VENDOR_INVALID            = 16,
	FU_ERROR_NODE_VENDOR_NOTMATCH           = 17,
	FU_ERROR_NODE_MODEL_INVALID             = 18,
	FU_ERROR_NODE_MODEL_NOTMATCH            = 19,
};

/* FUP warning type */
enum FupWarningValues {
	FU_WARNING_NODE_ALREADY_UPDATE          = 128,
	FU_WARNING_NODE_ALREADY_EXECUTED        = 129,
	FU_WARNING_NODE_LINK_QUALITY            = 130,
};

#define FRAME_SIZE                 256

#define STATUSIND_LENGTH           12
#define ERRORIND_LENGTH            8
#define VERSIONIND_MAX_LENGTH      103
#define KILLIND_LENGTH             7

/* \name Definitions to serialize Base Management (USI) */
/* @{ */
#define PROTOCOL_FUP               0x1D
#define CMD_FUP_PROTOCOL_MSK       0x3F
#define CMD_FUP_PROTOCOL(A)        ((A)&CMD_FUP_PROTOCOL_MSK)
/* @} */

#define MAC_SIZE                   6

/* \name Firmware Information List */
/* @{ */
#define INFO_LIST_MAX_SIZE         32
#define INFO_LIST_NUMBER_ID        3
/* @} */

typedef struct {
	uint8_t macAddr[6];
	uint8_t *msgdata;
	uint16_t len;
} MsgPProfileParams;

/* Status information */
typedef struct {
	uint8_t state;
	uint32_t pages;
	uint8_t macAddr[MAC_SIZE];
} FupStatusInfo;

/* Frame information */
typedef struct {
	uint8_t *fwdata;
	uint16_t *numfr;
	uint16_t len;
} FupFrameInfo;

/* Error information */
typedef struct {
	uint8_t error;
	uint8_t macAddr[MAC_SIZE];
} FupErrorInfo;

/* Firmware Information List */
typedef struct _FupInfoIdList {
	uint8_t infoId;
	uint8_t idLen;
	char idData[INFO_LIST_MAX_SIZE];
} FupInfoIdList;

/* Version information */
typedef struct {
	uint8_t macAddr[MAC_SIZE];
	FupInfoIdList infoIdList[INFO_LIST_NUMBER_ID];
} FupVersionInfo;

/* Status command */
typedef struct {
	uint8_t cmd;
	FupStatusInfo statusinfo;
} FUStatusCmd;

/* Information associated to message */
typedef union {
	/* Info in crc at first */
	struct {
		uint32_t val;
		uint8_t pad;
	} crcFirst;

	/* Info in crc at last */
	struct {
		uint8_t pad;
		uint32_t val;
	} crcLast;
} FupBufferCrc;

/* Kill information */
typedef struct {
	uint8_t macAddr[MAC_SIZE];
} FupKillInfo;

typedef struct {
	uint8_t command;
	uint8_t cmd_ack;
	uint8_t ack;
	uint16_t datafr;
} FUPAckCallBack;

typedef struct {
	uint8_t command;
	uint8_t macAddr[MAC_SIZE];

	FupInfoIdList infoIdList[INFO_LIST_NUMBER_ID];
} FUPVersionIndCallBack;

typedef struct {
	uint8_t command;
	uint8_t state;
	uint32_t pages;

	uint8_t macAddr[MAC_SIZE];
} FUPStatusIndCallBack;

typedef struct {
	uint8_t command;
	uint8_t error;
	uint8_t macAddr[MAC_SIZE];
} FUPErrorIndCallBack;

typedef struct {
	uint8_t command;
	uint8_t nodeState;
	uint8_t mac[MAC_SIZE];
	uint16_t lnid;
	uint8_t sid;
	uint8_t lsid;
	uint8_t macFather[MAC_SIZE];
	uint16_t lnidFather;
	uint8_t sidFather;
} NetworkEvent;

typedef struct {
	uint8_t command;
	uint8_t nodeState;
	uint8_t mac[MAC_SIZE];
	uint16_t lnid;
	uint8_t sid;
	uint8_t lsid;
	uint8_t alvRxcnt;
	uint8_t alvTxcnt;
	uint8_t alvTime;
} NetworkEventAlive;

/* \name Base Management interface */
/* @{ */
void baseMng_Start(void);
void baseMng_Process(void);
uint8_t baseMng_receivedCmd(uint8_t *buf, uint16_t len);
int8_t baseMng_SendBuf(uint8_t *bufPtr, uint16_t len);
int8_t baseMng_SendPProfMsg(MsgPProfileParams *rsp);
int8_t baseMng_SendEventNetwork(void *event);

/* @} */

/* \name Firmware Upgrade (FUP) interface */
/* @{ */
int8_t fup_clearTargetList(void);
int8_t fup_addNewTarget(uint8_t *macAddr);
int8_t fup_SetFwDataRequest(uint8_t *fwdata);
int8_t fup_SetMatchRulesRequest(uint8_t *fwdata);
int8_t fup_SetUpgOptionsRequest(uint8_t *fwdata);
int8_t fup_InitFileTxRequest(uint8_t *fwdata);
int8_t fup_DataFrameRequest(FupFrameInfo *frameInfo);
int8_t fup_EndFileTxRequest(void);
int8_t fup_checkCrcStatus(void);
int8_t fup_enableFU(uint8_t enable);
int8_t fup_abort(uint8_t *macadd);
int8_t fup_getVersionRequestfromNode(uint8_t *macadd);
int8_t fup_getStateRequestfromNode(uint8_t *macadd);

void fup_SendStatus(FupStatusInfo *statusinfo);
void fup_SendError(FupErrorInfo *errorinfo);
void fup_SendVersion(FupVersionInfo *versioninfo);
void fup_SendKill(FupKillInfo *killinfo);

void fup_Init(void);
void fup_Start(void);
void fup_Process(void);

/* @} */

/* \name Iface FUP interface */
/* @{ */
int8_t ifaceFUP_send_ACK(uint8_t ackcmd, uint8_t ackerrcode);
int8_t ifaceFUP_send_FrameACK(uint8_t ackerrcode, uint16_t numfr);
int ifaceFUP_Unpack_DataFrameRequest(uint8_t *dataPtr, uint16_t len,
		uint16_t *numFrame);

/* @} */

/* @} */

/* @cond 0 */
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/* @endcond */

#endif
