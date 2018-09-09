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

#ifndef MCHP_ATWX_H_
#define MCHP_ATWX_H_

#include "socket/include/socket.h"

/* As WINC15x0 supports only 7 TCP sockets, maximum of 7 MQTT clients can be supported */
#define MQTT_MAX_CLIENTS  TCP_SOCK_MAX

typedef struct Timer Timer;

struct Timer {
	unsigned long systick_period;
	unsigned long end_time;
};

typedef struct Network_t Network;

struct Network_t
{
	int socket;
	int hostIP;
	int (*mqttread) (Network*, unsigned char*, int, int);
	int (*mqttwrite) (Network*, unsigned char*, int, int);
	void (*disconnect) (Network*);
}; 

int winc1500_read(Network*, unsigned char*, unsigned int, int);
int winc1500_write(Network*, unsigned char*, unsigned int, int);
void winc1500_disconnect(Network*);
void NetworkInit(Network* n);

int ConnectNetwork(Network*, char*, int, int);

void tcpClientSocketEventHandler(SOCKET, uint8_t, void*);
void dnsResolveCallback(uint8_t*, uint32_t);

#endif /* MCHP_ATWX_H_ */