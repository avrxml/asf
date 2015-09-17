/**
 * \file
 *
 * \brief DLMS Application Emulator for ATMEL PRIME v.1.3 Service Node
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

#include <string.h>
#include <stdio.h>

#include "asf.h"
#include "compiler.h"
#include "dlms_emu.h"

/**
 * \defgroup thirdparty_prime_group Short Cycles Application
 *
 * \par Purpose
 *
 * This application shows how PRIME interfaces should be used.
 *
 */

#define STATUS_432_CONNECTION    PIB_432_CONNECTION_STATE
#define MAX_TIME_BETWEEN_MSGS    10000    /*miliseconds */

/* Serial Number buffer */
static uint8_t puc_serial_number[13 + 1];

/* Counter to check connection status */
static uint8_t uc_counter_chk_conn;

enum {
	CH_SEND_CHECK,
	CH_WAIT_ANSWER
}
stCheckNode;

static uint8_t uc_mlme_cmd[MLME_DATA_SIZE];
static uint16_t us_base_node_address;

static dl432Buffer_t x_cmd_tx_432;
static dl432Buffer_t x_cmd_rx_432;
static dl432_LlcCallback_t x_rcv_432;

/* variables needed to generate serial number from mac address */
typedef struct {
	uint16_t cfgMagic;
	uint8_t _mac[6];
} MacCfg;

/* implemented obis object */
static const uint8_t md[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

const uint8_t puc_headerdlms[]  =  {0xC4, 0x01, 0x00, 0x00};
const uint8_t puc_headerdlms2[] =  {0xC4, 0x02, 0x00, 0x00};

const uint8_t puc_obis0000010000FF[] = {0x00, 0x00, 0x01, 0x00, 0x00, 0xff};
const uint8_t puc_msg_obis0000010000FF[] = {0xC5, 0x01, 0xC1, 0x00};
uint8_t puc_msg_obis0000010000ff[] = {0x09, 0x0C, 0x07, 0xDE, 0x08, 0x06, 0x02, 0x12,
	   0x01, 0x1B, 0xFF, 0x80, 0x00, 0x80};

const uint8_t puc_obis0100000200FF[] = {0x01, 0x00, 0x00, 0x02, 0x00, 0xFF};
const uint8_t puc_msg_obis0100000200FF[] = {0x09, 0x05, 0x56, 0x39, 0x33, 0x30, 0x30};

const uint8_t puc_obis00001C0700FF[] = {0x00, 0x00, 0x1C, 0x07, 0x00, 0xFF};
const uint8_t puc_msg_obis00001C0700FF[] = {0x09, 0x10, 0x30, 0x31, 0x30, 0x33, 0x30, 0x38,
	   0x30, 0x31, 0x44, 0x42, 0x55, 0x00, 0x00, 0x00, 0x00, 0x00};

const uint8_t puc_obis0000150005FF[] = {0x00, 0x00, 0x15, 0x00, 0x05, 0xff};
const uint8_t puc_msg_obis0000150005FF[] = {0x01, 0x01, 0x02, 0x0D, 0x09, 0x0C, 0x07,
	   0xDE, 0x04, 0x08, 0x02, 0x11, 0x0D, 0x05, 0xFF, 0x00, 0x78, 0x80, 0x12, 0x00,
	   0xFD, 0x12, 0x00, 0x00, 0x12, 0x00, 0x00, 0x12, 0x00, 0x00, 0x12, 0x00, 0x00,
	   0x12, 0x00, 0x00, 0x12, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00,
	   0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00,
	   0x12, 0x00, 0x00};
const uint8_t puc_obis0000150006FF[] = {0X00, 0X00, 0X15, 0X00, 0X06, 0XFF};
const uint8_t puc_msg_obis0000150006FF[] = {0x01, 0x01, 0x02, 0x07, 0x09, 0x0C, 0x07,
	   0xDE, 0x04, 0x08, 0x02, 0x10, 0x0A, 0x32, 0xFF, 0x00, 0x78, 0x80, 0x06, 0x00,
	   0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00,
	   0x06, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00,
	   0x00, 0x00};

const uint8_t puc_obis0000600100FF[] = {0X00, 0X00, 0X60, 0X01, 0X00, 0XFF};
const uint8_t puc_msg_obis0000600100FF[] = {0x09, 0x0A};

const uint8_t puc_obis0000600101FF[] = {0X00, 0X00, 0X60, 0X01, 0X01, 0XFF};
const uint8_t puc_msg_obis0000600101FF[] = {0x09, 0x06, 0x20, 0x48, 0x41, 0x42, 0x31, 0x32};

const uint8_t puc_obis0000600102FF[] = {0X00, 0X00, 0X60, 0X01, 0X02, 0XFF};
const uint8_t puc_msg_obis0000600102FF[] = {0x09, 0x12, 0x63, 0x6F, 0x6E, 0x74, 0x61,
	   0x64,0x6F, 0x72, 0x20, 0x20, 0x44, 0x4C, 0x4D, 0x53, 0x30, 0x31, 0x30, 0x35};

const uint8_t puc_obis010001080AFF[] = {0x01, 0x00, 0x01, 0x08, 0x0A, 0xFF};
const uint8_t puc_msg_obis010001080AFF[]   = {0x06, 0x00, 0x00, 0x00, 0x00};

const uint8_t puc_obis01015e2264ff[] = {0x01, 0x01, 0x5e, 0x22, 0x64, 0xff};
const uint8_t puc_msg_obis01015e2264ff[]   = {0x11, 0x00};

const uint8_t puc_obis01015e2268ff[] = {0x01, 0x01, 0x5e, 0x22, 0x68, 0xff};
const uint8_t puc_msg_obis01015e2268ff[]   = {0x11, 0x01};

/*	date obis objects */

const uint8_t puc_obis00015e2214ff[] = {0x00, 0x01, 0x5e, 0x22, 0x14, 0xff};
const uint8_t puc_msg_obis00015e2214ff[] = {0x16, 0x00};

const uint8_t puc_obis0000600E01FF[] = {0x00, 0x00, 0x60, 0x0E, 0x01, 0xFF};
const uint8_t puc_msg_obis0000600E01FF [] = {0x11, 0x01};

const uint8_t puc_obis000060030AFF[] = {0x00, 0x00, 0x60, 0x03, 0x0A, 0xFF};
const uint8_t puc_msg_obis000060030AFF [] = {0x16, 0x01};
/* disconector */
const uint8_t puc_obis00000F0001FF[]  = {0x00, 0x00, 0x0F, 0x00, 0x01, 0xFF};
const uint8_t puc_msg_obis00000F0001FF_02[] = {0x02, 0x02, 0x09, 0x06, 0x00, 0x00, 0x0A, 0x00,
	   0x6A, 0xFF, 0x12, 0x00, 0x00};
const uint8_t puc_msg_obis00000F0001FF_04[] = {0x01, 0x01, 0x02, 0x02, 0x09, 0x04, 0xFF, 0xFF,
	   0xFF, 0xFF, 0x09, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

/* puc_obis request and answers */

const uint8_t puc_obis0100630100FF[] = {0x01, 0x00, 0x63, 0x01, 0x00, 0xFF};
const uint8_t puc_msg_obis0100630100FF[] = {0x00, 0x00, 0x00, 0x01, 0x00, 0x81, 0xF2};
uint8_t puc_msg_obis0100630100FF_data[] = {0x02, 0x08, 0x09, 0x0C, 0x07, 0xDE, 0x05, 0x0C,
	   0x01, 0x10, 0x00, 0x00, 0xFF, 0x80, 0x00, 0x80, 0x11, 0x00, 0x06, 0x00, 0x00, 0x00,
	   0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00,
	   0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00};

/* current control threshold */
const uint8_t puc_obis00015E221FFF[] = {0x00, 0x01, 0x5E, 0x22, 0x1F, 0xFF};
const uint8_t puc_msg_obis00015E221FFF[] = {0x06, 0x00, 0x00, 0x3A, 0x98};
/* % of demand to close */
const uint8_t puc_obi00005E2246FF[] = { 0x00, 0x00, 0x5E, 0x22, 0x46, 0xFF};
const uint8_t puc_msg_obis00005E2246FF[] = {0x12, 0x25, 0x1C};

/* power meter configuration */
const uint8_t puc_obis00015E220XFF[] = {0x00, 0x01, 0x5E, 0x22, 0x00, 0xFF};
const uint8_t puc_msg_obis00015E220XFF[] = {0x06, 0x00, 0x00, 0x3A, 0x98};

#define LEN_ASSOCIATION_RSP    43
static const uint8_t puc_association_rsp[LEN_ASSOCIATION_RSP] = {0x61, 0x29, 0xA1, 0x09,
	   0x06, 0x07, 0x60, 0x85, 0x74, 0x05, 0x08, 0x01, 0x01, 0xA2, 0x03, 0x02, 0x01, 0x00,
	   0xA3, 0x05, 0xA1, 0x03, 0x02, 0x01, 0x00, 0xBE, 0x10, 0x04, 0x0E, 0x08, 0x00, 0x06,
	   0x5F, 0x1F, 0x04, 0x00, 0x00, 0x10, 0x14, 0x00, 0x00, 0x00, 0x07};

#define LEN_RELEASE_RSP        5
static const uint8_t puc_release_rsp[LEN_RELEASE_RSP] = { 0x63, 0x03, 0x80, 0x01, 0x00};

uint8_t S02_block_counter;
uint8_t S02_block_send;

uint8_t x_ini_S02[12];
uint8_t x_end_S02[12];
uint16_t max_fields_s02;

/**
* Convert functions
**/
static uint8_t _convertHEX2STR(uint8_t num)
{
	if (num > 9) {
		return (num + 0x37);
	} else {
		return (num + 0x30);
	}
}

static void _generate_serial(uint8_t *stringSerialBoard)
{
	uint8_t i;
	uint8_t num, num1;
	MacCfg mac;

	hal_read_mac_data_cfg(&mac, sizeof(mac));

	*stringSerialBoard = 'A';
	*(stringSerialBoard + 1) = 'T';
	*(stringSerialBoard + 2) = 'M';
	/* convert hex to ascii */
	for (i = 1; i < 6; i++) {
		num  = ((mac._mac[i] & 0xf0) >> 4);
		num1 = (mac._mac[i] & 0x0f);
		stringSerialBoard[2 * (i) + 1] = _convertHEX2STR(num);
		stringSerialBoard[2 * (i) + 2] = _convertHEX2STR(num1);
	}
	*(stringSerialBoard + 13) = 0x00;
}

/**
* Read date in the reception buffer and get how many fields have to be send in
* the S02 profile
**/
static uint16_t getNumberoffield_S02(void)
{
	uint32_t iniday, endday;

	memcpy(x_ini_S02, x_cmd_rx_432.dl.buff + 36, 12);
	memcpy(x_end_S02, x_cmd_rx_432.dl.buff + 50, 12);

	/* calculation have know accuracy errors ... assume all months
	 * are 30 days and leap year is not corrected*/
	endday = (x_end_S02[0] * 256 + x_end_S02[1]) * 365 * 24 + x_end_S02[2] * 30 * 24 +
			x_end_S02[3] * 24 + x_end_S02[5];
	iniday = (x_ini_S02[0] * 256 + x_ini_S02[1]) * 365 * 24 + x_ini_S02[2] * 30 * 24 +
			x_ini_S02[3] * 24 + x_ini_S02[5];
	/* update msg puc_obis */
	puc_msg_obis0100630100FF_data [10] = 0;
	/* init hour register  */
	puc_msg_obis0100630100FF_data [9] = x_ini_S02[5];
	/* week day */
	puc_msg_obis0100630100FF_data [8] = x_ini_S02[4];
	/* init register day */
	puc_msg_obis0100630100FF_data [7] = x_ini_S02[3];
	/* init register month convert to hex from bcd */
	puc_msg_obis0100630100FF_data [6] = x_ini_S02[2];
	/* init register year */
	puc_msg_obis0100630100FF_data [5] = x_ini_S02[1];
	puc_msg_obis0100630100FF_data [4] = x_ini_S02[0];

	return (uint16_t)(endday - iniday);
}

/**
* timestamp field increment 1 hour
* a single calendar implementation..
**/

static void updateDate(void)
{
	/* increment one hour	*/
	puc_msg_obis0100630100FF_data[9]++;
	if (puc_msg_obis0100630100FF_data[9] > 23) {
		/* start new day at 00H */
		puc_msg_obis0100630100FF_data[9] = 0;
		/* next day of week	*/
		puc_msg_obis0100630100FF_data[8]++;
		if (puc_msg_obis0100630100FF_data[8] > 7) {
			/* sunday to monday */
			puc_msg_obis0100630100FF_data[8] = 1;
		}

		/* new day */
		puc_msg_obis0100630100FF_data[7]++;
		/* change month	*/
		if (puc_msg_obis0100630100FF_data [7] > md[puc_msg_obis0100630100FF_data[6] - 1]) {
			puc_msg_obis0100630100FF_data [7] = 1;
			puc_msg_obis0100630100FF_data [6]++;
			/* change year */
			if (puc_msg_obis0100630100FF_data [6] > 12) {
				puc_msg_obis0100630100FF_data [6] = 1;
				puc_msg_obis0100630100FF_data [5]++;
			}
		}
	}
}

/**
**  build associate resquest with low security response/
*      associate response
* ack ="00" association accepted
* ack ="01" association not accepted.
* conformance = 0x01 must handshake security protocol.
* ACSEserviceUserValue =0
* pduVaule
**/
static uint16_t MSG_AssociationResponse( uint8_t *msg, uint8_t ack,
		uint8_t ACSEserviceUserValue, uint8_t NegotiateConformance, uint16_t maxPDU)
{
	uint8_t *puc_msg_aux = msg;

	memcpy( puc_msg_aux, puc_association_rsp, LEN_ASSOCIATION_RSP);

	*(puc_msg_aux + 17) = ack;
	*(puc_msg_aux + 24) = ACSEserviceUserValue;
	*(puc_msg_aux + 39) = (uint8_t)((maxPDU & 0xff00) >> 8);
	*(puc_msg_aux + 40) = (uint8_t)((maxPDU & 0x00FF));

	if ((NegotiateConformance == 1) && (ACSEserviceUserValue == 1)) {
		/* answer to request 601D */
		*(puc_msg_aux + 37) = 0x00;
		*(puc_msg_aux + 38) = 0x10;
	} else {
		if (NegotiateConformance == 1) {
			/* answer to 6021 request */
			*(puc_msg_aux + 17) = 0x01;  /*association result */
			*(puc_msg_aux + 24) = 0x0D;  /* Authentication not
			                              * accepted  */
			*(puc_msg_aux + 38) = 0x14;
		} else {
			/* answer to 6036 */
			*(puc_msg_aux + 17) = 00;  /* association result */
			*(puc_msg_aux + 24) = 00;  /* Authentication accepted */
			*(puc_msg_aux + 38) = 0x1D;
		}
	}

#ifdef DLMS_EMU_DEBUG_ENABLE
	printf("MSG_AssociationResponse<< 6129\r\n");
#endif

	return (LEN_ASSOCIATION_RSP);
}

/**
*  build DLMS answer using parameters
*  InvokeIDandPriority   message number in dlms dialog
*  puc_obis   obis string
*  classid    object class  of the obis object
*  dlmsATTR   object attribute of the object.
*
*  function return lenght of the dlms answer
*  if 0... object not found
**/

static uint16_t MSG_GetResponse( uint8_t *msg, int8_t InvokeIDandPriority,
		uint8_t *puc_obis, uint16_t classID, uint8_t dlmsATTR)
{
	uint16_t blocklenght = 0;

	memcpy(msg, puc_headerdlms, 4);
	/* puc_obis 0100000200 version dlms */
	if (memcmp(puc_obis, puc_obis0100000200FF, 6) == 0) {
		blocklenght = 4 + sizeof(puc_msg_obis0100000200FF);
		memcpy(msg + 4, puc_msg_obis0100000200FF, sizeof(puc_msg_obis0100000200FF));
		*(msg + 2) = InvokeIDandPriority;
		return (blocklenght);
	}

	/* puc_obis 00001C0700 version prime */
	if (memcmp(puc_obis, puc_obis00001C0700FF, 6) == 0) {
		blocklenght = 4 + sizeof(puc_msg_obis00001C0700FF);
		memcpy(msg + 4, puc_msg_obis00001C0700FF, sizeof(puc_msg_obis00001C0700FF));
		*(msg + 2) = InvokeIDandPriority;
		return (blocklenght);
	}

	/* puc_obis 0000150005FF meter details */
	if (memcmp(puc_obis, puc_obis0000150005FF, 6) == 0) {
		blocklenght = 4 + sizeof(puc_msg_obis0000150005FF);
		memcpy(msg + 4, puc_msg_obis0000150005FF, sizeof(puc_msg_obis0000150005FF));
		*(msg + 2) = InvokeIDandPriority;
		return (blocklenght);
	}

	/* puc_obis 0000150006FF meter details */
	if (memcmp(puc_obis, puc_obis0000150006FF, 6) == 0) {
		blocklenght = 4 + sizeof(puc_msg_obis0000150006FF);
		memcpy(msg + 4, puc_msg_obis0000150006FF, sizeof(puc_msg_obis0000150006FF));
		*(msg + 2) = InvokeIDandPriority;
		return (blocklenght);
	}

	/* METER SERIAL NUMBER */
	if (memcmp(puc_obis, puc_obis0000600100FF, 6) == 0) {
		blocklenght     = 4 + sizeof(puc_msg_obis0000600100FF) + 10;
		memcpy(msg + 4, puc_msg_obis0000600100FF, sizeof(puc_msg_obis0000600100FF));
		memcpy(msg + 4 + sizeof(puc_msg_obis0000600100FF), puc_serial_number + 3, 10);
		*(msg + 2) = InvokeIDandPriority;
		return (blocklenght);
	}

	if (memcmp(puc_obis, puc_obis0000600101FF, 6) == 0) {
		blocklenght = 4 + sizeof(puc_msg_obis0000600101FF);
		memcpy(msg + 4, puc_msg_obis0000600101FF, sizeof(puc_msg_obis0000600101FF));
		*(msg + 2) = InvokeIDandPriority;
		return (blocklenght);
	}

	if (memcmp(puc_obis, puc_obis0000600102FF, 6) == 0) {
		/* puc_obis 0000600102FF */
		blocklenght = 4 + sizeof(puc_msg_obis0000600102FF);
		memcpy(msg + 4, puc_msg_obis0000600102FF, sizeof(puc_msg_obis0000600102FF));
		*(msg + 2) = InvokeIDandPriority;
		return (blocklenght);
	}

	/* puc_obis 010001080A */
	if (memcmp(puc_obis, puc_obis010001080AFF, 6) == 0) {
		blocklenght = 4 + sizeof(puc_msg_obis010001080AFF);
		memcpy(msg + 4, puc_msg_obis010001080AFF, sizeof(puc_msg_obis010001080AFF));
		*(msg + 2) = InvokeIDandPriority;
		return (blocklenght);
	}

	/* puc_obis 0100630100 s02 profile request */

	if (memcmp(puc_obis, puc_obis0100630100FF, 6) == 0) {
		if (dlmsATTR == 0x02) {
			memcpy(msg, puc_headerdlms2, 4);
			/* number of hour to ask cycles... load curve lenght */
			max_fields_s02 = getNumberoffield_S02();
			memcpy(msg + 4, puc_msg_obis0100630100FF, sizeof(puc_msg_obis0100630100FF));
			*(msg + 2) = InvokeIDandPriority;
			/* number of block 1 */
			*(msg + 7) = 0x01;
			/* number of data fields in the load curve */
			*(msg + 11) = 0x01;
			*(msg + 12) = max_fields_s02;
			/* number of data block to send */
			S02_block_counter = max_fields_s02 / 5;
			S02_block_send = 0;
			if ((max_fields_s02 % 5) != 0) {
				S02_block_counter++;
			}

			uint8_t position = 13;
			/* number of data fields in the load curve */
			uint8_t i = 0;
			while ((i < 5) && (S02_block_send <= max_fields_s02)) {
				memcpy(msg + position, puc_msg_obis0100630100FF_data, 48);
				updateDate();
				position += 48;
				i++;
				S02_block_send++;
			}
			*(msg + 9) = 0x81;
			*(msg + 10) = 48 * i + 2; /* data lenght */
			return (position);
		}
	}

	/* puc_obis 0000010000FF invoke c1 classid 08 */
	if (memcmp(puc_obis, puc_obis0000010000FF, 6) == 0) {
		if (classID == 0x08) {
			blocklenght = 4 + sizeof(puc_msg_obis0000010000ff);
			memcpy(msg + 4, puc_msg_obis0000010000ff, sizeof(puc_msg_obis0000010000ff));
			*(msg + 2) = InvokeIDandPriority;
			return (blocklenght);
		} else {
			return 0;
		}
	}

	/* instant power */
	if (memcmp(puc_obis, puc_obis01015e2264ff, 6) == 0) {
		blocklenght = 4 + sizeof(puc_msg_obis01015e2264ff);
		memcpy(msg + 4, puc_msg_obis01015e2264ff, sizeof(puc_msg_obis01015e2264ff));
		*(msg + 2) = InvokeIDandPriority;
		return (blocklenght);
	}

	if (memcmp(puc_obis, puc_obis01015e2268ff, 6) == 0) {
		blocklenght = 4 + sizeof(puc_msg_obis01015e2268ff);
		memcpy(msg + 4, puc_msg_obis01015e2268ff, sizeof(puc_msg_obis01015e2268ff));
		*(msg + 2) = InvokeIDandPriority;
		return (blocklenght);
	}

	if (memcmp(puc_obis, puc_obis00015e2214ff, 6) == 0) {
		blocklenght = 4 + sizeof(puc_msg_obis00015e2214ff);
		memcpy(msg + 4, puc_msg_obis00015e2214ff, sizeof(puc_msg_obis00015e2214ff));
		*(msg + 2) = InvokeIDandPriority;
		return (blocklenght);
	}

	/* demand threshold */
	if (memcmp(puc_obis, puc_obis00015E220XFF, 4) == 0) {
		blocklenght = 4 + sizeof(puc_msg_obis00015E220XFF);
		memcpy(msg + 4, puc_msg_obis00015E220XFF, sizeof(puc_msg_obis00015E220XFF));
		*(msg + 2) = InvokeIDandPriority;
		return (blocklenght);
	}

	if (memcmp(puc_obis, puc_obis00015e2214ff, 4) == 0) {
		blocklenght = 4 + sizeof(puc_msg_obis00015e2214ff);
		memcpy(msg + 4, puc_msg_obis00015e2214ff, blocklenght);
		*(msg + 2) = InvokeIDandPriority;
		return (blocklenght);
	}

	if (memcmp(puc_obis, puc_obis0000600E01FF, 4) == 0) {
		blocklenght = 4 + sizeof(puc_msg_obis0000600E01FF);
		memcpy(msg + 4, puc_msg_obis0000600E01FF, blocklenght);
		*(msg + 2) = InvokeIDandPriority;
		return (blocklenght);
	}

	if (memcmp(puc_obis, puc_obis000060030AFF, 4) == 0) {
		blocklenght = 4 + sizeof(puc_msg_obis000060030AFF);
		memcpy(msg + 4, puc_msg_obis000060030AFF, blocklenght);
		*(msg + 2) = InvokeIDandPriority;
		return (blocklenght);
	}

	/* disconector operation */
	if (memcmp(puc_obis, puc_obis00000F0001FF, 4) == 0) {
		switch (dlmsATTR) {
		case 0x02:
			blocklenght = 4 + sizeof(puc_msg_obis00000F0001FF_02);
			memcpy(msg + 4, puc_msg_obis00000F0001FF_02, blocklenght);
			*(msg + 2) = InvokeIDandPriority;
			return (blocklenght);

		case 0x04:
			blocklenght = 4 + sizeof(puc_msg_obis00000F0001FF_04);
			memcpy(msg + 4, puc_msg_obis00000F0001FF_04, blocklenght);
			*(msg + 2) = InvokeIDandPriority;
			return (blocklenght);

		default:
			return 0;
		}
	}

	if (memcmp(puc_obis, puc_obis00015E221FFF, 4) == 0) {
		blocklenght = 4 + sizeof(puc_msg_obis00015E221FFF);
		memcpy(msg + 4, puc_msg_obis00015E221FFF, blocklenght);
		*(msg + 2) = InvokeIDandPriority;
		return (blocklenght);
	}

	if (memcmp(puc_obis, puc_obi00005E2246FF, 4) == 0) {
		blocklenght = 4 + sizeof(puc_msg_obis00005E2246FF);
		memcpy(msg + 4, puc_msg_obis00005E2246FF, blocklenght);
		*(msg + 2) = InvokeIDandPriority;
		return (blocklenght);
	}

	/* obis not found.  return dlms message of ObjectUndefined */
	blocklenght = 5;
	*(msg + 4) = 0x04;
	*(msg + 2) = InvokeIDandPriority;
	*(msg + 3) = 0x01;
	return (blocklenght);
}

/**
**  build release response
**/
static uint16_t MSG_ReleaseResponse(uint8_t *msg, uint8_t value)
{
	memcpy(msg, puc_release_rsp, LEN_RELEASE_RSP);
	msg[4] = value;

#ifdef DLMS_EMU_DEBUG_ENABLE
	printf("MSG_ReleaseResponse<< 6308\r\n");
#endif

	return LEN_RELEASE_RSP;
}

/**
**  send DLMS answer
**/
static void DLMSMessageSend( uint16_t lengthmsg)
{
	if (dl432_dl_data_request(x_cmd_rx_432.dl.lsap, x_cmd_rx_432.dl.dsap, us_base_node_address, &x_cmd_tx_432, lengthmsg, 0) == -1) {
#ifdef DLMS_EMU_DEBUG_ENABLE
		printf("Error. Indication Not response\r\n");
	} else {
		printf("Indication Response OK. Length: %d\r\n", lengthmsg);
#endif
	}
}

static uint16_t _checkReceivedData(uint8_t *data)
{
	uint16_t DLMSrequest = 0x0000;
	uint16_t classID;
	uint8_t InvokeAndPriority;
	uint16_t blocklenght;
	uint8_t byteCode1, byteCode2;
	uint8_t *pinstanceID;
	uint8_t uc_nextblock;
	uint16_t obisattribute;
	uint16_t position = 11;
	uint8_t i = 0;

	byteCode1 = *(data);
	byteCode2 = *(data + 1);
	DLMSrequest = byteCode1 * 256 + byteCode2;
#ifdef DLMS_EMU_DEBUG_ENABLE
	printf("received  %X", DLMSrequest);
#endif
	switch (DLMSrequest) {
	case 0x6021:
		/*Association request */
#ifdef DLMS_EMU_DEBUG_ENABLE
		printf("association request received>> %X", DLMSrequest);
#endif
		blocklenght = MSG_AssociationResponse( x_cmd_tx_432.dl.buff, 0, 0, 1, 0x0100);
		DLMSMessageSend(blocklenght);
		break;

	case 0x601D:
#ifdef DLMS_EMU_DEBUG_ENABLE
		printf("registration association request>>  %X", DLMSrequest);
#endif
		blocklenght = MSG_AssociationResponse( x_cmd_tx_432.dl.buff, 0, 1, 1, 0x01F4);
		DLMSMessageSend(blocklenght);
		break;

	case 0x6036:
#ifdef DLMS_EMU_DEBUG_ENABLE
		printf("registration association request>>  %X", DLMSrequest);
#endif
		blocklenght = MSG_AssociationResponse( x_cmd_tx_432.dl.buff, 0, 1, 0, 0x0100);
		DLMSMessageSend(blocklenght);
		break;

	case 0x6200:
		/* return release response. */
#ifdef DLMS_EMU_DEBUG_ENABLE
		printf("release request received>>  %X", DLMSrequest);
#endif
		/* return normal release */
		blocklenght = MSG_ReleaseResponse((uint8_t *)x_cmd_tx_432.dl.buff, 0x00);
		DLMSMessageSend(blocklenght);
		break;

	case 0xC001:
		/* GET request message */
#ifdef DLMS_EMU_DEBUG_ENABLE
		printf("get request >> %X", DLMSrequest);
#endif
		/* dlms protocol answer with same invoke number */
		InvokeAndPriority = *(data + 2);
		/* obis class id identifier */
		classID = (*(data + 3) << 0xf) + *(data + 4);
		/* pointer to obis object identifier */
		pinstanceID = (data + 5);
		/* obis object attribute */
		obisattribute = *(data + 11);
		blocklenght = MSG_GetResponse(x_cmd_tx_432.dl.buff, InvokeAndPriority,
										pinstanceID, classID, obisattribute);
		if (blocklenght) {
			DLMSMessageSend(blocklenght);
		} else {
#ifdef DLMS_EMU_DEBUG_ENABLE
			printf("unknown puc_obis :%x %x%x%x%x%x%x\n",
					DLMSrequest, *pinstanceID,
					*(pinstanceID + 1), *(pinstanceID + 2),
					*(pinstanceID + 3), *(pinstanceID + 4),
					*(pinstanceID + 5));
#endif
		}

		break;

	case 0xC002:
		uc_nextblock = *(data + 6) + 1;    /* next block number */
#ifdef DLMS_EMU_DEBUG_ENABLE
		printf("get request >> %X", DLMSrequest);
		printf("DLMS block number: %u", uc_nextblock);
#endif
		/*dlms protocol answer with same invoke number */
		InvokeAndPriority = *(data + 2);
		memcpy(x_cmd_tx_432.dl.buff, puc_headerdlms2, 4);
		memcpy(x_cmd_tx_432.dl.buff + 4, puc_msg_obis0100630100FF, sizeof(puc_msg_obis0100630100FF));
		*(x_cmd_tx_432.dl.buff + 2) = InvokeAndPriority;
		*(x_cmd_tx_432.dl.buff + 7) = uc_nextblock;
		if (uc_nextblock >= S02_block_counter) {
			/* this is the last data block */
			*(x_cmd_tx_432.dl.buff + 3) = 1;
		} else {
			*(x_cmd_tx_432.dl.buff + 3) = 0;
		}

		/* number of data fields in the load curve */
		while ((i < 5) && (S02_block_send <= max_fields_s02)) {
			memcpy(x_cmd_tx_432.dl.buff + position, puc_msg_obis0100630100FF_data,
					sizeof(puc_msg_obis0100630100FF_data));
			updateDate();
			position += 48;
			i++;
			S02_block_send++;
		}
		*(x_cmd_tx_432.dl.buff + 9) = 0x81;
		*(x_cmd_tx_432.dl.buff + 10) = 48 * i;     /* data lenght */
		blocklenght = position;
		DLMSMessageSend(position);
		break;

	case 0xC101:
		/*dlms protocol answer with same invoke number */
		InvokeAndPriority = *(data + 2);
		/* obis class id identifier */
		classID = (*(data + 3) << 0xf) + *(data + 4);
		/* obis */
		pinstanceID = (data + 5);
		if (memcmp(pinstanceID, puc_obis0000010000FF, 6) == 0) {
#ifdef DLMS_EMU_DEBUG_ENABLE
			printf("update datetime>>");
#endif
			/* Sync CCT date */
			memcpy(&puc_msg_obis0000010000ff[2], data + 15, 12);
			blocklenght = sizeof(puc_msg_obis0000010000FF);
			memcpy(x_cmd_tx_432.dl.buff, puc_msg_obis0000010000FF, blocklenght);
			DLMSMessageSend(blocklenght);
		}

		break;

	default:
		/*  unexpected bytecode  */
#ifdef DLMS_EMU_DEBUG_ENABLE
		printf(" not ready to this bytecode :%x\n", DLMSrequest );
#endif
		/* return abnormal release */
		blocklenght = MSG_ReleaseResponse((uint8_t *)x_cmd_tx_432.dl.buff, 0x01);
		DLMSMessageSend(blocklenght);
		break;
	}

	return DLMSrequest;
}

static Bool _checkNodestatus(uint32_t *status)
{
	uint16_t pibAttrib;
	Bool end = false;

	switch (stCheckNode) {
	case CH_SEND_CHECK:
		prime_MLME_GET_request(STATUS_432_CONNECTION);
		stCheckNode = CH_WAIT_ANSWER;
		break;

	case CH_WAIT_ANSWER:
		if (prime_MLME_callback(uc_mlme_cmd)) {
			pibAttrib
				= (uint16_t)(uc_mlme_cmd[2] << 8) + (uint16_t)uc_mlme_cmd[3];
			if ((uc_mlme_cmd[0] == PRIME_MLME_GET_CONFIRM) && (pibAttrib == STATUS_432_CONNECTION)) {
				if (uc_mlme_cmd[1] == PRIME_MLME_RESULT_SUCCESS) {
					/* Get pibValue*/
					*status = (uint16_t)(uc_mlme_cmd[4] << 24) + (uint16_t)(uc_mlme_cmd[5] << 16) +
							(uint16_t)(uc_mlme_cmd[6] << 8) + (uint16_t)uc_mlme_cmd[7];
				} else {
					*status = 0;         /* node disconnected */
				}

				stCheckNode = CH_SEND_CHECK;
				end = true;
			}
		}

		break;

	default:
		stCheckNode = CH_SEND_CHECK;
		break;
	}
	return end;
}

static void _checkReceivedMessages(void)
{
	sscs432_NotService_t newNot;

	if (sscs432_poll_establish_release(&newNot)) {
		if (newNot.cmd == CL_432_RELEASE_CONFIRM) {
#ifdef DLMS_EMU_DEBUG_ENABLE
			printf("4-32 Connection CLOSED %s \n", str_Serial_Number);
#endif
		} else {
			us_base_node_address = newNot.baseAddress;
#ifdef DLMS_EMU_DEBUG_ENABLE
			printf("4-32 Connection Opened %s\n", str_Serial_Number);
#endif
		}
	}

	dl432_dl_callback(&x_rcv_432, &x_cmd_rx_432, MAX_LENGTH_432_DATA);
	if (x_rcv_432.cmd == DL432_DATA_INDICATION) {
		_checkReceivedData(x_cmd_rx_432.lsdu);
	}
}

/**
 * \brief Create main Cycles Application task and create timer to update
 * internal counters.
 *
 */
void dlms_eum_init(void)
{
	_generate_serial(puc_serial_number);

	/* Init variables and status */
	stCheckNode = CH_SEND_CHECK;
	uc_counter_chk_conn = PRIME_CHECK_CONNECTION_RATE;

#ifdef DLMS_EMU_DEBUG_ENABLE
	printf("DLMS EMU Application: Service Node...\r\n");
#endif
}

/**
 * \brief Periodic task to process Cycles App. Initialize and start Cycles
 * Application and launch timer
 * to update internal counters.
 *
 */
void dlms_eum_process(void)
{
	uint32_t nodeConnected;

	_checkReceivedMessages();
	if (!uc_counter_chk_conn--) {
		uc_counter_chk_conn = PRIME_CHECK_CONNECTION_RATE;
		if (_checkNodestatus(&nodeConnected)) {
			if (!nodeConnected) {
				/* launch connection */
				sscs432_establish_request((uint8_t *)puc_serial_number,
							strlen((const char *)puc_serial_number));
			}
		}
	}
}
