/**
 * Copyright (C) 2017 Microchip Technology Inc. and its subsidiaries (Microchip).  All rights reserved.

 * You are permitted to use the software and its derivatives with Microchip products.

 * SOFTWARE AND DOCUMENTATION ARE PROVIDED “AS IS” WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, 
 * ANY WARRANTY OF MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS 
 * BE LIABLE OR OBLIGATED UNDER CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER LEGAL OR EQUITABLE THEORY FOR 
 * ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES, OR OTHER 
 * SIMILAR COSTS. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP AND ITS LICENSORS LIABILITY WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY, THAT 
 * YOU PAID DIRECTLY TO MICROCHIP TO USE THIS SOFTWARE.  MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE TERMS.

 * THIRD PARTY SOFTWARE:  Notwithstanding anything to the contrary, any third party software accompanying this software is subject to the terms and 
 * conditions of the third party’s license agreement or open source software license.  To the extent required by third party or open source licenses 
 * covering such third party software, the terms of such license will apply in lieu of the terms provided herein.  To the extent the terms of such third 
 * party licenses prohibit any of the restrictions described herein, such restrictions will not apply to such third party software.  
 * THIRD PARTY SOFTWARE IS SUBJECT TO THE FOREGOING WARRANTY DISCLAIMER AND LIMIT ON LIABILITY PROVIDED IN THE PARAGRAPH ABOVE.
 */

#include "MQTTClient/Wrapper/mqtt.h"
#include "driver/include/m2m_wifi.h"
#include "socket/include/socket.h"
#include "string.h"

#define IPV4_BYTE(val,index) 	((val >> (index * 8)) & 0xFF)
#define MQTT_RX_POOL_SIZE		256

static unsigned long MilliTimer=0;
static int32_t gi32MQTTBrokerIp=0;
static int32_t gi32MQTTBrokerRxLen=0;
static bool gbMQTTBrokerIpresolved=false;
static bool gbMQTTBrokerConnected=false;
static bool gbMQTTBrokerSendDone=false;
static bool gbMQTTBrokerRecvDone=false;
static unsigned char gcMQTTRxFIFO[MQTT_RX_POOL_SIZE];
static uint32_t gu32MQTTRxFIFOPtr=0;
static uint32_t gu32MQTTRxFIFOLen=0;
static char *gpcHostAddr;

static bool isMQTTSocket(SOCKET sock)
{
	unsigned int cIdx;
	struct mqtt_module *mqttInstance;
	
	for(cIdx = 0; cIdx < MQTT_MAX_CLIENTS; cIdx++)
	{
		if(mqttClientPool[cIdx].mqtt_instance != NULL)
		{
			mqttInstance = mqttClientPool[cIdx].mqtt_instance;
			if(mqttInstance->network.socket == sock)
				return true;
		}
	}
	return false;
}

void dnsResolveCallback(uint8_t *hostName, uint32_t hostIp)
{
	if((gbMQTTBrokerIpresolved == false) && (!strcmp((const char *)gpcHostAddr, (const char *)hostName)))
	{
		gi32MQTTBrokerIp = hostIp;
		gbMQTTBrokerIpresolved = true;
		#ifdef MQTT_PLATFORM_DBG
		printf("INFO >> Host IP of %s is %d.%d.%d.%d\r\n", hostName, (int)IPV4_BYTE(hostIp, 0), (int)IPV4_BYTE(hostIp, 1),
		(int)IPV4_BYTE(hostIp, 2), (int)IPV4_BYTE(hostIp, 3));
		#endif
	}
}

void tcpClientSocketEventHandler(SOCKET sock, uint8_t u8Msg, void *pvMsg)
{
	if(isMQTTSocket(sock)) { 
		switch (u8Msg) {
			case SOCKET_MSG_CONNECT:
			{
				gbMQTTBrokerConnected=true;
				#ifdef MQTT_PLATFORM_DBG
				printf("INFO >> Successfully connected Broker Socket.\r\n");
				#endif
			}
			break;
			case SOCKET_MSG_SEND:
			{
				gbMQTTBrokerSendDone=true;
				#ifdef MQTT_PLATFORM_DBG
				printf("INFO >> Successfully sent message via Broker Socket.\r\n");
				#endif
			}
			break;
			case SOCKET_MSG_RECV:
			{
				tstrSocketRecvMsg* pstrRx = (tstrSocketRecvMsg*)pvMsg;
				gi32MQTTBrokerRxLen = pstrRx->s16BufferSize;
				if((gi32MQTTBrokerRxLen<0) && (gi32MQTTBrokerRxLen!=SOCK_ERR_TIMEOUT)) {
					#ifdef MQTT_PLATFORM_DBG
					printf("ERROR >> Receive error for broker socket (Err=%ld).\r\n",gi32MQTTBrokerRxLen);
					#endif
				}
				#ifdef MQTT_PLATFORM_DBG
				printf("DEBUG >> Remaining data in Rx buffer of broker socket: %d\r\n",pstrRx->u16RemainingSize);
				#endif
				gbMQTTBrokerRecvDone=true;
			}
			break;
			default: break;
		}
	}
}

void SysTick_Handler(void){
	MilliTimer++;
}

char TimerIsExpired(Timer* timer) {
	long left = timer->end_time - MilliTimer;
	return (left < 0);
}


void TimerCountdownMS(Timer* timer, unsigned int timeout) {
	timer->end_time = MilliTimer + timeout;
}


void TimerCountdown(Timer* timer, unsigned int timeout) {
	timer->end_time = MilliTimer + (timeout * 1000);
}


int TimerLeftMS(Timer* timer) {
	long left = timer->end_time - MilliTimer;
	return (left < 0) ? 0 : left;
}


void TimerInit(Timer* timer) {
	timer->end_time = 0;
}

static int WINC1500_read(Network* n, unsigned char* buffer, int len, int timeout_ms) { 
  //at times the upper layer requests for single byte of data, but network has more in rx buffer.
  //this results in callback being invoked multiple times with length 1 before returning. To prevent loss of
  //data in this process, pool data internally and give it to upper layer on request. 
  
  //temporary workaround for timer overrun 
  if(0==timeout_ms) timeout_ms=10;
  
  if(0==gu32MQTTRxFIFOLen){ //no data in internal FIFO
	  #ifdef MQTT_PLATFORM_DBG
	  printf("DEBUG >> Requesting data from network\r\n");
	  #endif
	  gbMQTTBrokerRecvDone=false;
	  if (SOCK_ERR_NO_ERROR!=recv(n->socket,gcMQTTRxFIFO,MQTT_RX_POOL_SIZE,timeout_ms)){
		  #ifdef MQTT_PLATFORM_DBG
		  printf("ERROR >> recv failed\r\n");
		  #endif
		  return -1;
	  }
	  //call handle_events until we get rx callback 
	  while (false==gbMQTTBrokerRecvDone){
		  m2m_wifi_handle_events(NULL);
	  }
	  
	  //update current FIFO length
	  if(gi32MQTTBrokerRxLen>0){ //data recieved form network
		gu32MQTTRxFIFOLen=gi32MQTTBrokerRxLen;
		gu32MQTTRxFIFOPtr=0;
		#ifdef MQTT_PLATFORM_DBG
		printf("DEBUG >> filled FiFo from network. length=%d\r\n",gu32MQTTRxFIFOLen);
		#endif
	  
	  #ifdef MQTT_PLATFORM_DBG
    	printf("DEBUG >> rx data through socket: \r\n");
    	int i=0;
    	for(i=0;i<gu32MQTTRxFIFOLen;i++)
    	printf("0x%x, ",gcMQTTRxFIFO[i]);
    	printf("\r\n");
	  #endif
	  }
	  else{//mostly in case of timeout
		  gu32MQTTRxFIFOLen=0;
		  gu32MQTTRxFIFOPtr=0;
		  #ifdef MQTT_PLATFORM_DBG
		  printf("DEBUG >> no data to send. returning error code (%d)\r\n",gi32MQTTBrokerRxLen);
		  #endif
		  return gi32MQTTBrokerRxLen; //this corresponds to the error code.
	  }
  }
  //return data to client from data present in the FIFO from previous recv()
  if(len>(int)gu32MQTTRxFIFOLen){
	  #ifdef MQTT_PLATFORM_DBG
	  printf("ERROR >> unable to process read request since FIFO contains less data than requested\r\n");
	  #endif
	  return -1;
  }
 
  memcpy((void*)buffer, (const void*)&gcMQTTRxFIFO[gu32MQTTRxFIFOPtr],len);
  gu32MQTTRxFIFOLen-=len;
  gu32MQTTRxFIFOPtr+=len;
  
  return len;
}


static int WINC1500_write(Network* n, unsigned char* buffer, int len, int timeout_ms) {
  gbMQTTBrokerSendDone=false;
  if (SOCK_ERR_NO_ERROR!=send(n->socket,buffer,len,0)){
	  #ifdef MQTT_PLATFORM_DBG
	  printf("ERROR >> send error");
	  #endif
	  return -1;
  }
  //wait for send callback
  while (false==gbMQTTBrokerSendDone){
	  m2m_wifi_handle_events(NULL);
  }
  
  #ifdef MQTT_PLATFORM_DBG
  printf("DEBUG >> sent data through socket: \r\n");
  int i=0;
  for(i=0;i<len;i++)
	  printf("0x%x ",buffer[i]);
  printf("\r\n");	
  #endif

  //TODO: figure out how to get actual send length from callback
  //this length will be updated in the callback
  //return gu32MQTTBrokerSendLen;
  return len;
}


static void WINC1500_disconnect(Network* n) {
	close(n->socket);
	n->socket=-1;
	gbMQTTBrokerConnected=false;
}


void NetworkInit(Network* n) {
	n->socket = -1;
	n->mqttread = WINC1500_read;
	n->mqttwrite = WINC1500_write;
	n->disconnect = WINC1500_disconnect;
}

int ConnectNetwork(Network* n, char* addr, int port, int TLSFlag){

  //Resolve Server URL.
  gbMQTTBrokerIpresolved = false;
  gpcHostAddr = addr;
  gethostbyname((uint8*)addr);
 
  //wait for resolver callback
  while (false==gbMQTTBrokerIpresolved){
	  m2m_wifi_handle_events(NULL);
  }
  
  n->hostIP = gi32MQTTBrokerIp;
  
  //connect to socket
  struct sockaddr_in addr_in;
  addr_in.sin_family = AF_INET;
  addr_in.sin_port = _htons(port);
  addr_in.sin_addr.s_addr = gi32MQTTBrokerIp;

  /* Create secure socket */ 
  if(n->socket < 0)
	n->socket = socket(AF_INET, SOCK_STREAM, TLSFlag);
  
  /* Check if socket was created successfully */
  if (n->socket == -1) {
   #ifdef MQTT_PLATFORM_DBG
   printf("ERROR >> socket error.\r\n");
   #endif
   close(n->socket);
   return SOCK_ERR_INVALID;
  }
  
  /* If success, connect to socket */
  if (connect(n->socket, (struct sockaddr *)&addr_in, sizeof(struct sockaddr_in)) != SOCK_ERR_NO_ERROR) {
   #ifdef MQTT_PLATFORM_DBG  
   printf("ERROR >> connect error.\r\n");
   #endif
   return SOCK_ERR_INVALID;
  }
  
  gbMQTTBrokerConnected = false;
  
  /*wait for SOCKET_MSG_CONNECT event */
  while(false==gbMQTTBrokerConnected){
    m2m_wifi_handle_events(NULL);
  }
  
  /* Success */
  #ifdef MQTT_PLATFORM_DBG
  printf("INFO >> ConnectNetwork successful\r\n");
  #endif
  return SOCK_ERR_NO_ERROR;
}