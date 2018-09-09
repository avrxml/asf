/**
 * @file network_winc1500tls_wrapper.c
 *
 * @brief winc1500 TLS wrapper.
 *
 *
 */
/*
 * Copyright 2010-2015 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *  http://aws.amazon.com/apache2.0
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

#include <stdbool.h>
#include <string.h>

#include "AWS_SDK/aws_iot_src/utils/aws_iot_error.h"
#include "AWS_SDK/aws_iot_src/utils/aws_iot_log.h"
#include "AWS_SDK/aws_iot_src/protocol/mqtt/aws_iot_embedded_client_wrapper/network_interface.h"
#include "driver/include/m2m_wifi.h"
//#include "m2m_wifi.h"
#include "driver/source/nmasic.h"
#include "socket/include/socket.h"

/** Using IP address. */
#define IPV4_BYTE(val, index)           ((val >> (index * 8)) & 0xFF)
#define DNS_REQUEST   25
//#define ENABLE_LOGS	  

static int8_t state =  -1,err = 0;
/** Get host IP status variable. */
static bool gbHostIpByName = false;
/** IP address of host. */
uint32_t gu32HostIp = 0;
uint32_t sent_bytes = 0,received_bytes = 0;
static uint8_t gau8SocketTestBuffer[1460];
static uint8_t* buf_ptr = &gau8SocketTestBuffer;


/**
 * \brief Callback function of IP address.
 *
 * \param[in] hostName Domain name.
 * \param[in] hostIp Server IP.
 *
 * \return None.
 */
static void resolve_cb(uint8_t *hostName, uint32_t hostIp)
{
	state = DNS_REQUEST;
	gu32HostIp = hostIp;
	gbHostIpByName = true;
	printf("Host IP is %d.%d.%d.%d\r\n", (int)IPV4_BYTE(hostIp, 0), (int)IPV4_BYTE(hostIp, 1),
			(int)IPV4_BYTE(hostIp, 2), (int)IPV4_BYTE(hostIp, 3));
	printf("Host Name is %s\r\n", hostName);
}

/**
 * \brief Callback function of TCP client socket.
 *
 * \param[in] sock socket handler.
 * \param[in] u8Msg Type of Socket notification
 * \param[in] pvMsg A structure contains notification informations.
 *
 * \return None.
 */
static void socket_cb(SOCKET sock, uint8_t u8Msg, void *pvMsg)
{
	switch (u8Msg) {
		case SOCKET_MSG_CONNECT:
		{
			tstrSocketConnectMsg *pstrConnect = (tstrSocketConnectMsg *)pvMsg;
			if (pstrConnect && pstrConnect->s8Error >= SOCK_ERR_NO_ERROR) {
				state = u8Msg;
				err = 0;
				//recv(sock, gau8SocketTestBuffer, sizeof(gau8SocketTestBuffer),0);	
				printf("Successfully connected.\r\n");
				} else {
				state = u8Msg;
				err = -1;
				printf("Connect error! code(%d)\r\n", pstrConnect->s8Error);
			}
		}
		break;
		
		case SOCKET_MSG_SEND:
		{
			sint16 s16SentBytes = *((sint16*)pvMsg);
			state = u8Msg;
			if(s16SentBytes < 0)
			{
				err = -1;
				printf("Socket send error:%d\r\n",s16SentBytes);
			}
			else
			{
				err = 0;
				sent_bytes = s16SentBytes;
			}
		}
		break;
		
		case SOCKET_MSG_RECV:
		{
			tstrSocketRecvMsg *pstrRecv = (tstrSocketRecvMsg *)pvMsg;
			state = u8Msg;
			if (pstrRecv && pstrRecv->s16BufferSize > 0) {
				received_bytes = pstrRecv->s16BufferSize;
				err = 0;
			}
			else
			{
				err = -1;
			}
			
		}
		
		default:
		break;
	}
}
/**
 * \brief Event handling function for WIFI.
 *
 * \param[in] pvMsg A structure contains notification informations.
 *
 * \return None.
 */
static void wait_for_event(uint8_t msg)
{
	state = -1;
	while(1)
	{
		m2m_wifi_handle_events(NULL);
		if(state == msg)
		{
			return;
		}
	}
}

/**
 * \brief Intialization of network interface for TLS function.
 *
 * \param[in] pNetwork Network interface.
 *
 * \return Error status.
 */
int iot_tls_init(Network *pNetwork) {
	
	if(pNetwork == NULL) {
		return NULL_VALUE_ERROR;
	}
	
	pNetwork->my_socket = -1;
	pNetwork->connect = iot_tls_connect;
	pNetwork->mqttread = iot_tls_read;
	pNetwork->mqttwrite = iot_tls_write;
	pNetwork->disconnect = iot_tls_disconnect;
	pNetwork->isConnected = iot_tls_is_connected;
	pNetwork->destroy = iot_tls_destroy;
	
	/* Initialize Socket module */
	socketInit();
	registerSocketCallback(socket_cb, resolve_cb);

	
	return NONE_ERROR;
}

/**
 * \brief TLS connection status call.
 *
 * \param[in] pNetwork Network interface.
 *
 * \return Status.
 */
int iot_tls_is_connected(Network *pNetwork) {
	/* Use this to add implementation which can check for physical layer disconnect */
	return 1;
}

/**
 * \brief TLS connection functionality.
 *
 * \param[in] pNetwork Network interface.
 * \param[in] params connection parameters for TLS.
 *
 * \return Status.
 */
int iot_tls_connect(Network *pNetwork, TLSConnectParams params) {
	IoT_Error_t ret = TCP_CONNECT_ERROR;
	struct sockaddr_in addr;
	SOCKET tls_socket = -1;
	
	gethostbyname((uint8_t*)params.pDestinationURL);
	wait_for_event(DNS_REQUEST);
	if(gu32HostIp != 0)
	{
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = gu32HostIp;
		addr.sin_port = _htons(params.DestinationPort);	
		//sslSetActiveCipherSuites(SSL_ENABLE_ALL_SUITES & ~(SSL_ENABLE_RSA_GCM_SUITES|SSL_ENABLE_DHE_GCM_SUITES
		//							|SSL_ENABLE_ECDHE_GCM_SUITES | SSL_ENABLE_ECDHE_SHA256_SUITES | SSL_ENABLE_ECDHE_SHA_SUITES));
		if ((tls_socket = socket(AF_INET, SOCK_STREAM,SOCKET_FLAGS_SSL)) < 0) 
		{
			return SSL_CONNECT_ERROR;
		}
		/* Connect server */
		if(connect(tls_socket, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) < 0)
		{
			pNetwork->my_socket = -1;
			close(tls_socket);
			return SSL_CONNECT_ERROR;
		}
		wait_for_event(SOCKET_MSG_CONNECT);
		if(err == 0)
		{
			pNetwork->my_socket = tls_socket;
			ret = NONE_ERROR;
		}
		else
		{
			return SSL_CONNECT_ERROR;
		}
	}
	else
	{
		ret = SSL_CONNECT_ERROR;
	}
	return ret;
}

/**
 * \brief TLS write functionality.
 *
 * \param[in] pNetwork Network interface.
 * \param[in] pMsg data to write
 * \param[in] len length of data.
 * \param[in] timeout_ms timeout for write function.
 * \return Status.
 */
int iot_tls_write(Network *pNetwork, unsigned char *pMsg, int len, int timeout_ms) {
	SOCKET tls_sock = -1;
	int ret = 0, i = 0;

	if (pNetwork == NULL || pMsg == NULL || pNetwork->my_socket < 0
	|| timeout_ms == 0) {
		return (NULL_VALUE_ERROR);
	}

	tls_sock = (SOCKET)pNetwork->my_socket;
#ifdef ENABLE_LOGS	
	printf("write:");
	for(i=0;i<len;i++)
	{
		printf("%02x:",pMsg[i]);
	}
	printf("\n");
#endif
   
	ret = send(tls_sock, pMsg, len, 0);
	if( ret < 0)
	{
		return SSL_WRITE_ERROR;
	}
	wait_for_event(SOCKET_MSG_SEND);
	if (err == 0) {
		return (sent_bytes);
	}
	return (SSL_WRITE_ERROR);
}

/**
 * \brief TLS read functionality.
 *
 * \param[in] pNetwork Network interface.
 * \param[in] pMsg store the data read
 * \param[in] len length of data.
 * \param[in] timeout_ms timeout for write function.
 * \return Status.
 */
extern volatile uint32_t ms_ticks;
int iot_tls_read(Network *pNetwork, unsigned char *pMsg, int len, int timeout_ms) {
	 int ret = 0,i = 0;
	 SOCKET tls_socket = NULL;
	 if (pNetwork == NULL || pMsg == NULL || pNetwork->my_socket < 0
		 	 || timeout_ms == 0) {
		if(timeout_ms == 0 && received_bytes != 0){
			
		}
		else{
			return (NULL_VALUE_ERROR);
		}
	 }
	 tls_socket = (SOCKET)pNetwork->my_socket;
	 if(received_bytes == 0)
	 {
		buf_ptr = &gau8SocketTestBuffer;
		received_bytes = 0;
		ret = recv(tls_socket, gau8SocketTestBuffer,sizeof(gau8SocketTestBuffer), timeout_ms);	
		if( ret < 0)
		{
			return SSL_READ_ERROR;
		}
		wait_for_event(SOCKET_MSG_RECV);
		 if (err == -1) {
			 return SSL_READ_ERROR;
		 }
		 else{
			 memcpy(pMsg,buf_ptr,len);
			 received_bytes -= len;
			 buf_ptr += len;

#ifdef ENABLE_LOGS
			  printf("readi->%d:",received_bytes);
			  for(i=0;i<len;i++)
			  {
				  printf("%02x:",pMsg[i]);
			  }
			  printf("\n");
#endif
			 
			 return len;
		 }
	 }
	 else{
		 memcpy(pMsg,buf_ptr,len);
		 received_bytes -= len;
		 buf_ptr += len;

#ifdef ENABLE_LOGS		 
		  printf("read->%d:",received_bytes);
		  for(i=0;i<len;i++)
		  {
			  printf("%02x:",pMsg[i]);
		  }
		  printf("\n");
#endif
		 
		 return len;		 
	 }
	 return (SSL_READ_ERROR);
}

/**
 * \brief TLS disconnect function.
 *
 * \param[in] pNetwork Network interface.
 * 
 * \return None.
 */
void iot_tls_disconnect(Network *pNetwork) {
	if (pNetwork == NULL || pNetwork->my_socket < 0) {
		return;
	}
	close(pNetwork->my_socket);
}
/**
 * \brief TLS interface de-init function.
 *
 * \param[in] pNetwork Network interface.
 * 
 * \return Status.
 */
int iot_tls_destroy(Network *pNetwork) {
	
	 if (pNetwork == NULL) {
		 return (NULL_VALUE_ERROR);
	 }

	 pNetwork->my_socket = 0;
	 pNetwork->mqttread = NULL;
	 pNetwork->mqttwrite = NULL;
	 pNetwork->disconnect = NULL;

	 return (NONE_ERROR);
}
