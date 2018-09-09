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

/**
 * \page mqtt_quickstart Quick start guide for MQTT service
 *
 * This is the quick start guide for the how to configure and use the MQTT service.
 *
 *
 * \section mqtt_operation Basic operations of MQTT
 * MQTT is formed by the following operations.
 * -# \ref mqtt_opearion_connect
 * -# \ref mqtt_opearion_publish
 * -# \ref mqtt_opearion_subscribe
 * -# \ref mqtt_opearion_unsubscribe
 * -# \ref mqtt_opearion_disconnect
 *
 * Before performing this operations, must call \ref mqtt_init function.
 *
 * \subsection mqtt_opearion_connect Connect
 * -# Connect operation is required for the other operations.
 * -# Connect operation is consist of following 2 functions.
 * \code
 * mqtt_connect : This function preforms the connect to MQTT broker server.
 * mqtt_connect_broker : This function preforms the authenticate and configure options to the MQTT broker server.
 * \endcode
 * -# After \ref mqtt_connect operation has completed \ref MQTT_CALLBACK_SOCK_CONNECTED event will produced. It means server was found and connected.
 * -# After that you should call \ref mqtt_connect_broker function and configure this MQTT connection. After this operation finished, \ref MQTT_CALLBACK_CONNECTED event will produced.
 *
 * \subsection mqtt_opearion_publish Publish
 * -# Publish operation perform the send message to all of subscribers that share the same topic.
 * -# Publish operation is consist of following function.
 * \code
 * mqtt_publish : This function preforms the publish message to the subscribers
 * \endcode
 *
 * \subsection mqtt_opearion_subscribe Subscribe
 * -# Subscribe operation is used to get the message on a particular topic when it is generated.
 * -# Subscribe operation is consist of following function.
 * \code
 * mqtt_subscribe : This function preforms the subscription of the topic.
 * \endcode
 *
 * \subsection mqtt_opearion_disconnect Disconnect
 * -# Disconnect operation is used to disconnect from the MQTT broker server.
 * -# Disconnect operation is consist of following function.
 * \code
 * mqtt_unsubscribe : This function preforms the unsubscribe topic.
 * \endcode
 *
 * \subsection mqtt_opearion_unsubscribe Unsubscribe
 * -# Unsubscribe operation is used to cancel a subscription of the specific topic.
 * -# Unsubscribe operation is consist of following function.
 * \code
 * mqtt_disconnect : This function preforms the disconnection from the broker server.
 * \endcode
 *
 */

/**
 * \defgroup sam0_mqtt_group MQTT service
 *
 * This module provides implementation of MQTT wrapper for Paho MQTT.
 *
 * MQ Telemetry Transport (MQTT) is a lightweight broker-based publish/subscribe
 * messaging protocol designed to be open, simple, lightweight and easy to implement.
 *
 * Revision history
 * 2014/10/07 : Initial draft. (v1.0)
 *
 * @{
 */

#ifndef MQTT_H_INCLUDED
#define MQTT_H_INCLUDED

#include <stdint.h>
#include "socket/include/socket.h"
#include "common/include/nm_common.h"
#include "MQTTClient/Platforms/mqtt_platform.h"
#include "MQTTClient/MQTTClient.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief A type of MQTT callback.
 */
enum mqtt_callback_type {
	/**
	 * Socket was connected.
	 * After received this event, try connect to broker server using mqtt_connect_broker.
	 */
	MQTT_CALLBACK_SOCK_CONNECTED,
	/** MQTT connection was connected to broker server. */
	MQTT_CALLBACK_CONNECTED,
	/**  The publish operation is completed. */
	MQTT_CALLBACK_PUBLISHED,
	/**  The subscribe operation is completed. */
	MQTT_CALLBACK_SUBSCRIBED,
	/**  The unsubscribe operation is completed. */
	MQTT_CALLBACK_UNSUBSCRIBED,
	/**  The disconnection operation is completed. */
	MQTT_CALLBACK_DISCONNECTED,
	/**  The PING operation is completed. */
	MQTT_CALLBACK_SENT_PING,
};

/**
 * \brief Connect return code.
 * The connect return code is sent in the variable header of a CONNACK message.
 */
enum mqtt_conn_result {
	/** Connection Accepted. */
	MQTT_CONN_RESULT_ACCEPT = 0,
	/** Connection Refused: unacceptable protocol version. */
	MQTT_CONN_RESULT_UNACCEPTABLE_VERSION,
	/** Connection Refused: identifier rejected. */
	MQTT_CONN_RESULT_ID_REJECTED,
	/** Connection Refused: server unavailable. */
	MQTT_CONN_RESULT_SERVER_UNAVAILABLE,
	/** Connection Refused: bad user name or password. */
	MQTT_CONN_RESULT_BAD_USER_NAME,
	/** Connection Refused: not authorized. */
	MQTT_CONN_RESULT_NOT_AUTHORIZED,
};

/**
 * \brief Structure of the MQTT_CALLBACK_SOCK_CONNECTED callback.
 */
struct mqtt_data_sock_connected {
	/**
	 * Result of operation.
	 *
	 * \return     -ENOENT         No such address.
	 * \return     -EINVAL         Invalid argument.
	 * \return     -ENOSPC         No space left on device.
	 * \return     -EIO            Device was occurred error due to unknown exception.
	 * \return     -EDESTADDRREQ   Destination address required.
	 * \return     -ECONNRESET     Connection reset by peer.
	 * \return     -EAGAIN         Try again.
	 * \return     -EBUSY          Device or resource busy.
	 * \return     -EADDRINUSE     Address already in use.
	 * \return     -EALREADY       Socket already connected.
	 * \return     -ENOTCONN       Service in bad state.
	 * \return     -ECONNREFUSED   Connection refused.
	 * \return     -EOVERFLOW      Value too large for defined data type.
	 * \return     -EBADMSG        Not a data message.
	 */
	int result;
};

/**
 * \brief Structure of the MQTT_CALLBACK_CONNECTED callback.
 */
struct mqtt_data_connected {
	/** Result of operation. */
	enum mqtt_conn_result result;
};

/**
 * \brief Structure of the MQTT_CALLBACK_PUBLISHED callback.
 */
struct mqtt_data_published {
	int dummy;
};

/**
 * \brief Structure of the MQTT_CALLBACK_SUBSCRIBED callback.
 */
struct mqtt_data_subscribed {
	int dummy;
};

/**
 * \brief Structure of the MQTT_CALLBACK_UNSUBSCRIBED callback.
 */
struct mqtt_data_unsubscribed {
	int dummy;
};

/**
 * \brief Structure of the MQTT_CALLBACK_DISCONNECTED callback.
 */
struct mqtt_data_disconnected {
	/** Error code of operation. \ref iot_error . */
	int reason;
};

/**
 * \brief Structure of the MQTT_CALLBACK_SENT_PING callback.
 */
struct mqtt_data_ping {
	int dummy;
};

/**
 * \brief Structure of the MQTT callback.
 */
union mqtt_data {
	struct mqtt_data_sock_connected sock_connected;
	struct mqtt_data_connected connected;
	struct mqtt_data_published published;
	struct mqtt_data_subscribed subscribed;
	struct mqtt_data_unsubscribed unsubscribed;
	struct mqtt_data_disconnected disconnected;
	struct mqtt_data_ping ping;
};

/* Before declaring for the callback type. */
struct mqtt_module;
/**
 * \brief Callback interface of MQTT service.
 */
typedef void (*mqtt_callback_t)(struct mqtt_module *module_inst, int type, union mqtt_data* data);

/**
 * \brief MQTT configuration structure
 *
 * Configuration struct for a MQTT instance. This structure should be
 * initialized by the \ref mqtt_get_config_defaults function before being
 * modified by the user application.
 */
struct mqtt_config {
	/**
	 * TCP port of MQTT.
	 * Default value is 1883.
	 */
	uint16_t port;
	/**
	 * A flag for the whether using the TLS socket or not.
	 * Default value is 0.
	 */
	uint8_t tls;
	/**
	 * Time value for the keep alive time out.
	 * Unit is seconds.
	 * Default value is 600. (10minutes)
	 */
	uint16_t keep_alive;
	/**
	 * Rx buffer.
	 * Default value is NULL.
	 */
	unsigned char *read_buffer;
	/**
	 * Maximum size of the receive buffer.
	 * If receiving data over this value, Connection is to be disconnected.
	 * Default value is 0.
	 */
	uint32_t read_buffer_size;
	/**
	 * Rx buffer.
	 * Default value is NULL.
	 */
	unsigned char *send_buffer;
	/**
	 * Send buffer size in the MQTT service.
	 * This buffer is located in the stack.
	 * Therefore, the size of the buffer increases the speed will increase, but it may cause a stack overflow.
	 * It MUST bigger than 5.
	 * Default value is 32.
	 */
	uint32_t send_buffer_size;
 };

/**
 * \brief Structure of MQTT connection instance.
 */
struct mqtt_module {
	/** Callback interface entry. */
	mqtt_callback_t callback;
	/** Configuration instance of MQTT module. That was registered from the \ref mqtt_init*/
	struct mqtt_config config;
	/** Paho Client pointer associtated with instance */
	MQTTClient *client;
	/** Network structure used to store the mqtt socket information for an instance of mqtt module */
	Network network;	
	/** Connection Status of the module instance */
	bool isConnected;
};

struct mqtt_client_pool{
	/** Client allocated for an instance */
	MQTTClient client;
	/** MQTT instance associated with the client */
	struct mqtt_module *mqtt_instance;
};

/** Static Client Pool Allocation */
extern struct mqtt_client_pool mqttClientPool[MQTT_MAX_CLIENTS];
	
/**
 * \brief Get default configuration of SW timer.
 *
 * \param[in]  config          Pointer of configuration structure which will be used in the module.
 */
void mqtt_get_config_defaults(struct mqtt_config *const config);

/**
 * \brief Initialize MQTT service.
 *
 * \param[in]  module          Module instance of MQTT.
 * \param[in]  config          Pointer of configuration structure which will be used in the module.
 *
 * \return     0               Function succeeded
 * \return     -ENOENT         No such address.
 * \return     -EINVAL         Invalid argument.
 * \return     -ENOSPC         No space left on device.
 * \return     -EIO            Device was occurred error due to unknown exception.
 * \return     -EDESTADDRREQ   Destination address required.
 * \return     -ECONNRESET     Connection reset by peer.
 * \return     -EAGAIN         Try again.
 * \return     -EBUSY          Device or resource busy.
 * \return     -EADDRINUSE     Address already in use.
 * \return     -EALREADY       Socket already connected.
 * \return     -ENOTCONN       Service in bad state.
 * \return     -ECONNREFUSED   Connection refused.
 * \return     -EOVERFLOW      Value too large for defined data type.
 * \return     -EBADMSG        Not a data message.
 * \return     -ENOMEM         Out of memory.
 */
int mqtt_init(struct mqtt_module *module, struct mqtt_config *config);

/**
 * \brief Termivate MQTT service.
 *
 * \param[in]  module          Module instance of MQTT.
 *
 * \return     0               Function succeeded
 * \return     -ENOENT         No such address.
 * \return     -EINVAL         Invalid argument.
 * \return     -ENOSPC         No space left on device.
 * \return     -EIO            Device was occurred error due to unknown exception.
 * \return     -EDESTADDRREQ   Destination address required.
 * \return     -ECONNRESET     Connection reset by peer.
 * \return     -EAGAIN         Try again.
 * \return     -EBUSY          Device or resource busy.
 * \return     -EADDRINUSE     Address already in use.
 * \return     -EALREADY       Socket already connected.
 * \return     -ENOTCONN       Service in bad state.
 * \return     -ECONNREFUSED   Connection refused.
 * \return     -EOVERFLOW      Value too large for defined data type.
 * \return     -EBADMSG        Not a data message.
 * \return     -ENOMEM         Out of memory.
 */
int mqtt_deinit(struct mqtt_module *const module);

/**
 * \brief Register and enable the callback.
 *
 * \param[in]  module_inst     Instance of MQTT module.
 * \param[in]  callback        Callback entry for the MQTT module.
 *
 * \return     0               Function succeeded
 * \return     -ENOENT         No such address.
 * \return     -EINVAL         Invalid argument.
 * \return     -ENOSPC         No space left on device.
 * \return     -EIO            Device was occurred error due to unknown exception.
 * \return     -EDESTADDRREQ   Destination address required.
 * \return     -ECONNRESET     Connection reset by peer.
 * \return     -EAGAIN         Try again.
 * \return     -EBUSY          Device or resource busy.
 * \return     -EADDRINUSE     Address already in use.
 * \return     -EALREADY       Socket already connected.
 * \return     -ENOTCONN       Service in bad state.
 * \return     -ECONNREFUSED   Connection refused.
 * \return     -EOVERFLOW      Value too large for defined data type.
 * \return     -EBADMSG        Not a data message.
 */
int mqtt_register_callback(struct mqtt_module *const module, mqtt_callback_t callback);

/**
 * \brief Unregister callback.
 *
 * \param[in]  module_inst     Instance of MQTT module.
 *
 * \return     0               Function succeeded
 * \return     -ENOENT         No such address.
 * \return     -EINVAL         Invalid argument.
 * \return     -ENOSPC         No space left on device.
 * \return     -EIO            Device was occurred error due to unknown exception.
 * \return     -EDESTADDRREQ   Destination address required.
 * \return     -ECONNRESET     Connection reset by peer.
 * \return     -EAGAIN         Try again.
 * \return     -EBUSY          Device or resource busy.
 * \return     -EADDRINUSE     Address already in use.
 * \return     -EALREADY       Socket already connected.
 * \return     -ENOTCONN       Service in bad state.
 * \return     -ECONNREFUSED   Connection refused.
 * \return     -EOVERFLOW      Value too large for defined data type.
 * \return     -EBADMSG        Not a data message.
 */
int mqtt_unregister_callback(struct mqtt_module *const module);

/**
 * \brief Event handler of socket event.
 *
 * \param[in]  sock            Socket descriptor.
 * \param[in]  msg_type        Event type.
 * \param[in]  msg_data        Structure of socket event.
 */
void mqtt_socket_event_handler(SOCKET sock, uint8_t msg_type, void *msg_data);

/**
 * \brief Event handler of gethostbyname.
 *
 * \param[in]  doamin_name     Domain name.
 * \param[in]  server_ip       Server IP.
 */
void mqtt_socket_resolve_handler(uint8_t *doamin_name, uint32_t server_ip);

/**
 * \brief Connect to specific MQTT broker server.
 * This function is responsible only connect the socket.
 * If operation of this function is complete, MQTT_CALLBACK_SOCK_CONNECTED event will be sent through MQTT callback.
 *
 * \param[in]  module_inst     Instance of MQTT module.
 * \param[in]  host            URL of MQTT broker server.
 *
 * \return     0               Function succeeded
 * \return     -ENOENT         No such address.
 * \return     -EINVAL         Invalid argument.
 * \return     -ENOSPC         No space left on device.
 * \return     -EIO            Device was occurred error due to unknown exception.
 * \return     -EDESTADDRREQ   Destination address required.
 * \return     -ECONNRESET     Connection reset by peer.
 * \return     -EAGAIN         Try again.
 * \return     -EBUSY          Device or resource busy.
 * \return     -EADDRINUSE     Address already in use.
 * \return     -EALREADY       Socket already connected.
 * \return     -ENOTCONN       Service in bad state.
 * \return     -ECONNREFUSED   Connection refused.
 * \return     -EOVERFLOW      Value too large for defined data type.
 * \return     -EBADMSG        Not a data message.
 */
int mqtt_connect(struct mqtt_module *const module, const char *host);

/**
 * \brief Send MQTT connect message to broker server with MQTT parameter.
 * If operation of this function is complete, MQTT_CALLBACK_CONNECTED event will be sent through MQTT callback.
 *
 * \param[in]  module_inst     Instance of MQTT module.
 * \param[in]  clean_session   If this value set to 1, Broker server store the previous subscribed informations after disconnected.
 * \param[in]  id              ID of user.
 * \param[in]  password        Password of user.
 * \param[in]  client_id       Client ID of this connection.
 * \param[in]  will_topic      A MQTT topic that will be sent to subscribers when this connection is disconnected.
 * \param[in]  will_msg        A MQTT payload that will be sent to subscribers when this connection is disconnected.
 * \param[in]  will_qos        QOS level of will message.
 * \param[in]  will_retain     Whether broker server will be store MQTT will message or not.
 *
 * \return     0               Function succeeded
 * \return     -ENOENT         No such address.
 * \return     -EINVAL         Invalid argument.
 * \return     -ENOSPC         No space left on device.
 * \return     -EIO            Device was occurred error due to unknown exception.
 * \return     -EDESTADDRREQ   Destination address required.
 * \return     -ECONNRESET     Connection reset by peer.
 * \return     -EAGAIN         Try again.
 * \return     -EBUSY          Device or resource busy.
 * \return     -EADDRINUSE     Address already in use.
 * \return     -EALREADY       Socket already connected.
 * \return     -ENOTCONN       Service in bad state.
 * \return     -ECONNREFUSED   Connection refused.
 * \return     -EOVERFLOW      Value too large for defined data type.
 * \return     -EBADMSG        Not a data message.
 */
int mqtt_connect_broker(struct mqtt_module *const module, uint8_t clean_session,
	const char *id, const char *password, const char *client_id, const char *will_topic, const char *will_msg,
	uint32_t will_msg_len, uint8_t will_qos, uint8_t will_retain);

/**
 * \brief Send disconnect message to MQTT broker server and closing socket.
 * If operation of this function is complete, MQTT_CALLBACK_DISCONNECTED event will be sent through MQTT callback.
 *
 * \param[in]  module_inst     Instance of MQTT module.
 * \param[in]  force_close     If this flag is set, MQTT service will be close the socket immediately.
 *
 * \return     0               Function succeeded
 * \return     -ENOENT         No such address.
 * \return     -EINVAL         Invalid argument.
 * \return     -ENOSPC         No space left on device.
 * \return     -EIO            Device was occurred error due to unknown exception.
 * \return     -EDESTADDRREQ   Destination address required.
 * \return     -ECONNRESET     Connection reset by peer.
 * \return     -EAGAIN         Try again.
 * \return     -EBUSY          Device or resource busy.
 * \return     -EADDRINUSE     Address already in use.
 * \return     -EALREADY       Socket already connected.
 * \return     -ENOTCONN       Service in bad state.
 * \return     -ECONNREFUSED   Connection refused.
 * \return     -EOVERFLOW      Value too large for defined data type.
 * \return     -EBADMSG        Not a data message.
 */
int mqtt_disconnect(struct mqtt_module *const module, int force_close);

/**
 * \brief Send publish message to MQTT broker server.
 * If operation of this function is complete, MQTT_CALLBACK_PUBLISHED event will be sent through MQTT callback.
 *
 * \param[in]  module_inst     Instance of MQTT module.
 * \param[in]  topic           Topic of this MQTT message.
 * \param[in]  msg             Payload of this MQTT message.
 * \param[in]  msg_len         Payload size of this MQTT message.
 * \param[in]  qos             QOS level of this MQTT message. (0 <= qos <= 2)
 * \param[in]  retain          Whether broker server will be store this MQTT message or not.
 *
 * \return     0               Function succeeded
 * \return     -ENOENT         No such address.
 * \return     -EINVAL         Invalid argument.
 * \return     -ENOSPC         No space left on device.
 * \return     -EIO            Device was occurred error due to unknown exception.
 * \return     -EDESTADDRREQ   Destination address required.
 * \return     -ECONNRESET     Connection reset by peer.
 * \return     -EAGAIN         Try again.
 * \return     -EBUSY          Device or resource busy.
 * \return     -EADDRINUSE     Address already in use.
 * \return     -EALREADY       Socket already connected.
 * \return     -ENOTCONN       Service in bad state.
 * \return     -ECONNREFUSED   Connection refused.
 * \return     -EOVERFLOW      Value too large for defined data type.
 * \return     -EBADMSG        Not a data message.
 */
int mqtt_publish(struct mqtt_module *const module, const char *topic, const char *msg, uint32_t msg_len, uint8_t qos, uint8_t retain);

/**
 * \brief Send subscribe message to MQTT broker server.
 * If operation of this function is complete, MQTT_CALLBACK_SUBSCRIBED event will be sent through MQTT callback.
 *
 * \param[in]  module_inst     Instance of MQTT module.
 * \param[in]  topic           A topic which will be received.
 * \param[in]  qos             QOS level of received publish message.
 *
 * \return     0               Function succeeded
 * \return     -ENOENT         No such address.
 * \return     -EINVAL         Invalid argument.
 * \return     -ENOSPC         No space left on device.
 * \return     -EIO            Device was occurred error due to unknown exception.
 * \return     -EDESTADDRREQ   Destination address required.
 * \return     -ECONNRESET     Connection reset by peer.
 * \return     -EAGAIN         Try again.
 * \return     -EBUSY          Device or resource busy.
 * \return     -EADDRINUSE     Address already in use.
 * \return     -EALREADY       Socket already connected.
 * \return     -ENOTCONN       Service in bad state.
 * \return     -ECONNREFUSED   Connection refused.
 * \return     -EOVERFLOW      Value too large for defined data type.
 * \return     -EBADMSG        Not a data message.
 */
int mqtt_subscribe(struct mqtt_module *const module, const char *topic, uint8_t qos, messageHandler msgHandler);

/**
 * \brief Send unsubscribe message to MQTT broker server.
 * If operation of this function is complete, MQTT_CALLBACK_UNSUBSCRIBED event will be sent through MQTT callback.
 *
 * \param[in]  module_inst     Instance of MQTT module.
 * \param[in]  topic           A topic that want to be removed.
 *
 * \return     0               Function succeeded
 * \return     -ENOENT         No such address.
 * \return     -EINVAL         Invalid argument.
 * \return     -ENOSPC         No space left on device.
 * \return     -EIO            Device was occurred error due to unknown exception.
 * \return     -EDESTADDRREQ   Destination address required.
 * \return     -ECONNRESET     Connection reset by peer.
 * \return     -EAGAIN         Try again.
 * \return     -EBUSY          Device or resource busy.
 * \return     -EADDRINUSE     Address already in use.
 * \return     -EALREADY       Socket already connected.
 * \return     -ENOTCONN       Service in bad state.
 * \return     -ECONNREFUSED   Connection refused.
 * \return     -EOVERFLOW      Value too large for defined data type.
 * \return     -EBADMSG        Not a data message.
 */
int mqtt_unsubscribe(struct mqtt_module *const module, const char *topic);

/**
 * \brief Poll for published frames.
 *
 * \param[in]  module_inst     Instance of MQTT module.
 * \param[in]  timeout_ms      time limit for polling.
 *
 * \return     0               Function succeeded
 * \return     -ENOENT         No such address.
 * \return     -EINVAL         Invalid argument.
 * \return     -ENOSPC         No space left on device.
 * \return     -EIO            Device was occurred error due to unknown exception.
 * \return     -EDESTADDRREQ   Destination address required.
 * \return     -ECONNRESET     Connection reset by peer.
 * \return     -EAGAIN         Try again.
 * \return     -EBUSY          Device or resource busy.
 * \return     -EADDRINUSE     Address already in use.
 * \return     -EALREADY       Socket already connected.
 * \return     -ENOTCONN       Service in bad state.
 * \return     -ECONNREFUSED   Connection refused.
 * \return     -EOVERFLOW      Value too large for defined data type.
 * \return     -EBADMSG        Not a data message.
 */
int mqtt_yield(struct mqtt_module *module, int timeout_ms);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* MQTT_H_INCLUDED */
