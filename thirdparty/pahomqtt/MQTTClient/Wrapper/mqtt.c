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
#include "string.h"

struct mqtt_client_pool mqttClientPool[MQTT_MAX_CLIENTS] = {0,};

static void allocateClient(struct mqtt_module *module);
static void deAllocateClient(struct mqtt_module *module);

static void allocateClient(struct mqtt_module *module)
{
	unsigned int cIdx;
	
	if(!module)
		return;
		
	for(cIdx = 0; cIdx < MQTT_MAX_CLIENTS; cIdx++)
	{
		if(mqttClientPool[cIdx].mqtt_instance == NULL)
		{
			mqttClientPool[cIdx].mqtt_instance = module;
			module->client = &(mqttClientPool[cIdx].client);
			return;
		}
	}	
}

static void deAllocateClient(struct mqtt_module *module)
{
	unsigned int cIdx;
	
	if(!module)
		return;
	
	for(cIdx = 0; cIdx < MQTT_MAX_CLIENTS; cIdx++)
	{
		if(mqttClientPool[cIdx].mqtt_instance == module)
		{
			mqttClientPool[cIdx].mqtt_instance = NULL;
			module->client = NULL;
			return;
		}
	}
}

int mqtt_init(struct mqtt_module *module, struct mqtt_config *config)
{
	unsigned int timeout_ms;
	
	if(NULL == module || NULL == config || NULL == config->send_buffer || NULL == config->read_buffer)
		return FAILURE;
		
	timeout_ms = config->keep_alive * 1000;
	NetworkInit(&(module->network));
	memcpy((void *)&(module->config), config, sizeof(struct mqtt_config));
	allocateClient(module);
	
	if(module->client)
	{
		MQTTClientInit(module->client, &(module->network), timeout_ms, config->send_buffer, config->send_buffer_size, config->read_buffer, config->read_buffer_size);
		return SUCCESS;
	}
	else
		return FAILURE;
}

int mqtt_deinit(struct mqtt_module *module)
{
	if(module)
	{
		if(module->isConnected)
			mqtt_disconnect(module, 0);
			
		deAllocateClient(module);
		return SUCCESS;
	}
	else
		return FAILURE;
}

void mqtt_get_config_defaults(struct mqtt_config *const config)
{
	config->port = 1883;
	config->tls = 0;
	config->keep_alive = 60;
	/* Below configuration must be initialized by Application */
	config->read_buffer = NULL;
	config->send_buffer = NULL;
	config->read_buffer_size = 0;
	config->send_buffer_size = 0;
}

int mqtt_register_callback(struct mqtt_module *module, mqtt_callback_t callback)
{
	if(module)
	{
		module->callback = callback;
		return SUCCESS;
	}
	else
		return FAILURE;
}
	
int mqtt_unregister_callback(struct mqtt_module *module)
{
	if(!module)
		return FAILURE;
		
	module->callback = NULL;
	return SUCCESS;
}

void mqtt_socket_event_handler(SOCKET sock, uint8_t msg_type, void *msg_data)
{
	tcpClientSocketEventHandler(sock, msg_type, msg_data);
}

void mqtt_socket_resolve_handler(uint8_t *domain_name, uint32_t server_ip)
{
	dnsResolveCallback(domain_name, server_ip);
}

int mqtt_connect(struct mqtt_module *module, const char *host)
{
	union mqtt_data connResult;
	connResult.sock_connected.result = ConnectNetwork(&(module->network), (char *)host, module->config.port, module->config.tls);
	if(module->callback)
		module->callback(module, MQTT_CALLBACK_SOCK_CONNECTED, &connResult);
	return connResult.sock_connected.result;
}

int mqtt_connect_broker(struct mqtt_module *const module, uint8_t clean_session, const char *id, const char *password, const char *client_id, const char *will_topic, const char *will_msg, uint32_t will_msg_len, uint8_t will_qos, uint8_t will_retain)
{
	// Will Message length is not used by Paho MQTT. 
	int rc;
	union mqtt_data connBrokerResult;
	MQTTPacket_connectData connectData = MQTTPacket_connectData_initializer;
		
	connectData.MQTTVersion = 4; //use protocol version 3.1.1
	connectData.clientID.cstring = (char *)client_id;
	connectData.username.cstring = (char *)id;
	connectData.password.cstring = (char *)password;
	connectData.cleansession = clean_session;
	connectData.will.topicName.cstring = (char *)will_topic;
	connectData.will.message.cstring = (char *)will_msg;
	connectData.will.retained = will_retain;
	connectData.will.qos = will_qos;
	
	if(will_topic && will_msg)
		connectData.willFlag = 1;
		
	rc = MQTTConnect(module->client, &connectData);
	
	connBrokerResult.connected.result = rc;
	if(module->callback)
		module->callback(module, MQTT_CALLBACK_CONNECTED, &connBrokerResult);
	
	module->isConnected = true;
	return rc;
}

int mqtt_disconnect(struct mqtt_module *const module, int force_close)
{
	//force_close is not used by Paho MQTT
	int rc;
	union mqtt_data disconnectResult;
	
	rc = MQTTDisconnect(module->client);
	
	disconnectResult.disconnected.reason = rc;
	
	if(module->callback)
		module->callback(module, MQTT_CALLBACK_DISCONNECTED, &disconnectResult);
	
	module->isConnected = false;
	return rc;
}

int mqtt_publish(struct mqtt_module *const module, const char *topic, const char *msg, uint32_t msg_len, uint8_t qos, uint8_t retain)
{
	int rc;
	MQTTMessage mqttMsg;	
	
	mqttMsg.qos = qos;
	mqttMsg.payload = (char *)msg;
	mqttMsg.payloadlen = (size_t)msg_len;
	mqttMsg.retained = retain;
	
	rc = MQTTPublish(module->client, topic, &mqttMsg);
	
	if(module->callback)
		module->callback(module, MQTT_CALLBACK_PUBLISHED, NULL);
	
	return rc;
}

int mqtt_subscribe(struct mqtt_module *module, const char *topic, uint8_t qos, messageHandler msgHandler)
{
	int rc;
	
	rc = MQTTSubscribe(module->client, topic, qos, msgHandler);
	
	if(module->callback)
		module->callback(module, MQTT_CALLBACK_SUBSCRIBED, NULL);	
	
	return rc;
}

int mqtt_unsubscribe(struct mqtt_module *module, const char *topic)
{
	int rc;
	
	rc = MQTTUnsubscribe(module->client, topic);

	if(module->callback)
		module->callback(module, MQTT_CALLBACK_SUBSCRIBED, NULL);	
	
	return rc;
}

int mqtt_yield(struct mqtt_module *module, int timeout_ms)
{
	return MQTTYield(module->client, timeout_ms);
}