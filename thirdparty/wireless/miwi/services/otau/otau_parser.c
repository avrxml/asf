/**
* \file otau_parser.c
*
* \brief OTAU Parser implementation
*
* Copyright (c) 2018 Microchip Technology Inc. and its subsidiaries.
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
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES,
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
/*
* Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
*/

#if defined(OTAU_ENABLED) && defined(OTAU_SERVER)

#include "string.h"
#include "compiler.h"
#include "otau_parser.h"
#include "sio2host.h"
#include "otau.h"

#include "mimem.h"
#include "miqueue.h"

static void processIncomingSerialData(void);
static void sioSendAck(uint8_t seqNum, uint8_t state);

/**
 * This is the receive buffer of the UART.
 */
static uint8_t *sioRxBuffer = NULL;

/**
 * This pointer points to the next free element inside the
 * receive buffer of the UART.
 */
static uint8_t *sioRxPtr;

/**
 * This is the receiver state of the UART. (\ref UART_RX_STATE_SOF,
 *   \ref UART_RX_STATE_LENGTH, \ref UART_RX_STATE_DATA, or
 *   \ref UART_RX_STATE_FCS)
 */
static volatile uint8_t sioRxState = UART_RX_STATE_SOF;

/**
 * This is the length of the message received.
 */
static uint16_t sioRxLength;

static uint8_t sioSeqNum;

static uint8_t sioTxSeqNum = 0;

static uint8_t sioFCS;

/**
 * This is the length of the message should be transmitted.
 */
static uint8_t sioData[SIO_RX_BUF_SIZE];
static uint8_t data_length = 0;
static uint8_t rx_index = 0;

static uint8_t ack_frame = 0;


MiQueue_t dataToTool;
MiQueue_t dataFromTool;


void otauParserInit(void)
{
	sio2host_init();
	miQueueInit(&dataToTool);
	miQueueInit(&dataFromTool);
	sioRxState = UART_RX_STATE_SOF;
}


/**
 * @brief Process data received from SIO
 */
static void processIncomingSerialData(void)
{
	switch (sioRxState) {
	case UART_RX_STATE_SOF:
		sioRxLength = 0;
		sioFCS = 0;
		ack_frame = 0;
		if (SOF == sioData[rx_index]) {
			sioRxState = UART_RX_STATE_FCF;
		}
		break;

	case UART_RX_STATE_FCF:
		if (SIO_FRAME_TYPE_ACK == (sioData[rx_index] & SIO_FRAME_TYPE_MASK)) {
			ack_frame = 1;
		}
		sioRxState = UART_RX_STATE_LENGTH_1;
		break;

	case UART_RX_STATE_LENGTH_1:
		sioRxLength = sioData[rx_index];
		sioRxState = UART_RX_STATE_LENGTH_2;
		break;

	case UART_RX_STATE_LENGTH_2:
		sioRxLength = sioRxLength | (sioData[rx_index] << 8);
		if (sioRxLength) {
			sioRxState = UART_RX_STATE_SEQ;
		} else {
			sioRxState = UART_RX_STATE_SOF;
		}
		break;

	case UART_RX_STATE_SEQ:
		sioSeqNum = sioData[rx_index];
		sioRxState = UART_RX_STATE_STACK_ID;
		sioRxLength--;
		sioFCS = sioData[rx_index];
		break;
	
	case UART_RX_STATE_STACK_ID:
		if(STACK_ID == sioData[rx_index]){
			sioRxLength--;
			sioFCS ^= sioData[rx_index];
			if (ack_frame){
				sioRxState = UART_RX_STATE_FCS;
			} 
			else{
				sioRxState = UART_RX_STATE_DOMAIN;
			}
		}
		else{
			sioRxState = UART_RX_STATE_SOF;
		}
		break;

	case UART_RX_STATE_DOMAIN:
		if (DOMAIN_OTAU_NOTIFY == sioData[rx_index] || DOMAIN_OTAU_UPGRADE == sioData[rx_index])
		{
			sioRxState = UART_RX_STATE_PAYLOAD;
			/* + 4  to store the next pointer in Queue */
			/* + 2  to store the length in payload */
			sioRxBuffer = MiMem_Alloc((uint8_t)sioRxLength + 2 + 4);
			if (NULL != sioRxBuffer)
			{
				sioRxPtr = sioRxBuffer + 4;
				memcpy(sioRxPtr, (uint8_t *)&sioRxLength, sizeof(uint16_t));
				sioRxPtr += sizeof(uint16_t);
				*sioRxPtr = sioData[rx_index];
				sioRxPtr++;
				sioRxLength--;
				sioFCS ^= sioData[rx_index];
			}
			else
			{
				sioRxState = UART_RX_STATE_SOF;
			}
		}
		else
		{
			sioRxState = UART_RX_STATE_SOF;
		}
		break;

	case UART_RX_STATE_PAYLOAD:
		*sioRxPtr = sioData[rx_index];
		sioRxPtr++;
		--sioRxLength;
		sioFCS ^= sioData[rx_index];
		if (!sioRxLength) {
			sioRxState = UART_RX_STATE_FCS;
		}
		break;

	case UART_RX_STATE_FCS:
		if (ack_frame)
		{
			sioRxState = UART_RX_STATE_SOF;
			break;
		}
		if (sioFCS == sioData[rx_index]) {
			sioSendAck(sioSeqNum, SIO_FRAME_CTRL_VALID_FCS);
			/* Message posted to Queue successfully */
			miQueueAppend(&dataFromTool, (miQueueBuffer_t *)sioRxBuffer);
			sioRxBuffer = NULL;
		}
		else{
			MiMem_Free(sioRxBuffer);
			sioSendAck(sioSeqNum, SIO_FRAME_CTRL_INVALID_FCS);
		}
		
		/* Make rx buffer ready for next reception before handling
		 * received data. */
		sioRxState = UART_RX_STATE_SOF;
		break;

	default:
		sioRxState = UART_RX_STATE_SOF;
		break;
	}
}


/*
 * @brief Function to handle the state machine serial data exchange.
 */
void serialDataHandler(void)
{
	/* Rx processing */
	if (data_length == 0) {
		/* No data to process, read the stream IO */
		rx_index = 0;
		data_length = sio2host_rx(&sioData[0], SIO_RX_BUF_SIZE); 
	} else { 
		/* Data has been received, process the data */
		/* Process each single byte */
		processIncomingSerialData();
		data_length--;
		rx_index++;
	}
	if (0 != dataToTool.size)
	{
		uint8_t *event = NULL;
		event = (uint8_t *)miQueueRemove(&dataToTool, NULL);
		if (NULL != event)
		{
			/* + 4  to store the next pointer in Queue */
			uint8_t *buff = event + 4;
			uint16_t buff_len;
			memcpy(&buff_len, buff, 2);
			buff += 2;
			sio2host_tx(buff, buff_len);
			MiMem_Free(event);
		}
	}
}

#define ADDITIONAL_SERIAL_BYTES    (10)
#define MESSAGE_HEADER             (5)
#define ADDR_MODE_SIZE             (1)
#define SERIAL_ACK_LEN             (7)

void send_server_data(otau_domain_t domainId, addr_mode_t addr_mode, uint8_t *addr, uint8_t msgId, uint8_t *msg, uint16_t len)
{
	uint8_t addr_size;
	uint16_t totalLen;
	uint8_t *bufferHead;
	if(NATIVE_ADDR_MODE == addr_mode)
	{
		addr_size = NATIVE_ADDR_SIZE;
	}
	else
	{
		addr_size = EXTENDED_ADDR_SIZE;
	}
	totalLen = len + ADDITIONAL_SERIAL_BYTES + ADDR_MODE_SIZE + addr_size;
	/* + 4  to store the next pointer in Queue */
	/* + 2  to store the length in payload */
	bufferHead = MiMem_Alloc(totalLen + 2 + 4);
	if (NULL != bufferHead)
	{
		uint8_t fcs = 0;
		uint8_t *buffer_ptr = bufferHead + 4;

		memcpy(buffer_ptr, (uint8_t *)&totalLen, 2);
		buffer_ptr += 2;
		totalLen -= MESSAGE_HEADER;
		*buffer_ptr++ = SOF;
		*buffer_ptr++ = 0x80; //AREQ/ARSP
		*buffer_ptr++ = (uint8_t)totalLen;
		*buffer_ptr++ = (uint8_t)totalLen >> 8;
		*buffer_ptr++ = sioTxSeqNum;
		*buffer_ptr++ = STACK_ID;
		*buffer_ptr++ = domainId;
		*buffer_ptr++ = 0x00;
		*buffer_ptr++ = msgId;
		*buffer_ptr++ = addr_mode;
		memcpy(buffer_ptr, addr, addr_size);
		buffer_ptr += addr_size;
		fcs = sioTxSeqNum ^ STACK_ID ^ domainId ^ msgId ^ addr_mode;
		for(uint8_t index = 0; index < addr_size; index++)
		{
			fcs ^= *(addr + index);
		}
		if(len)
		{
			memcpy(buffer_ptr, msg, len);
			buffer_ptr += len;
			for(uint8_t index = 0; index < len; index++)
			{
				fcs ^= *(msg + index);
			}
		}
		*buffer_ptr = fcs;
		miQueueAppend(&dataToTool, (miQueueBuffer_t *)bufferHead);
		sioTxSeqNum++;
	}
}

void send_pc_data(otau_domain_t domainId, uint8_t msgId, uint8_t *msg, uint16_t len)
{
	uint16_t total_len = len + ADDITIONAL_SERIAL_BYTES;
	/* + 4  to store the next pointer in Queue */
	/* + 2  to store the length in payload */
	uint8_t *bufferHead = MiMem_Alloc(total_len + 2 + 4);
	if (NULL != bufferHead)
	{
		uint8_t fcs = 0;
		uint8_t *bufferPtr = bufferHead + 4;
		memcpy(bufferPtr, (uint8_t *)&total_len, 2);
		bufferPtr += 2;
		total_len -= MESSAGE_HEADER;
		*bufferPtr++ = SOF;
		*bufferPtr++ = 0x80; //AREQ/ARSP
		*bufferPtr++ = (uint8_t)total_len;
		*bufferPtr++ = (uint8_t)total_len >> 8;
		*bufferPtr++ = sioTxSeqNum;
		*bufferPtr++ = STACK_ID;
		*bufferPtr++ = domainId;
		*bufferPtr++ = 0x00;
		*bufferPtr++ = msgId;
		fcs = sioTxSeqNum ^ STACK_ID ^ domainId ^ msgId;
		memcpy(bufferPtr, msg, len);
		bufferPtr += len;
		for(uint8_t index = 0; index < len; index++)
		{
			fcs ^= *(msg + index);
		}
		*bufferPtr = fcs;
		miQueueAppend(&dataToTool, (miQueueBuffer_t *)bufferHead);
		sioTxSeqNum++;
	}
}

static void sioSendAck(uint8_t seqNum, uint8_t state)
{
	uint16_t total_len = SERIAL_ACK_LEN;
	/* + 4  to store the next pointer in Queue */
	/* + 2  to store the length in payload */
	uint8_t *buffer_header = MiMem_Alloc(total_len + 2 + 4);
	if (NULL != buffer_header)
	{
		uint8_t *buffer_ptr = buffer_header + 4;
		memcpy(buffer_ptr, (uint8_t *)&total_len, 2);
		buffer_ptr += 2;
		*buffer_ptr++ = SOF;
		*buffer_ptr++ = state & 0x3F;
		*buffer_ptr++ = 0x02;
		*buffer_ptr++ = 0x00;
		*buffer_ptr++ = seqNum;
		*buffer_ptr++ = STACK_ID;
		*buffer_ptr = seqNum ^ STACK_ID; //for FCS
		miQueueAppend(&dataToTool, (miQueueBuffer_t *)buffer_header);
	}
}

#endif //#if defined(OTAU_ENABLED) && defined(OTAU_SERVER)

