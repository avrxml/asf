/**
 * \file
 *
 * \brief MQTT service.
 *
 * Copyright (c) 2016 Atmel Corporation. All rights reserved.
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
 * This module provides implementation of MQTT 3.1 for WINC1500 board.
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
#include "iot/sw_timer.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Protocol version ID of MQTT. ID of MQTT v3.1 is 3. */
#define MQTT_PROTO_VERSION             3

/** Protocol version string of MQTT. String of MQTT v3.1 is MQIsdp. */
#define MQTT_PROTO_NAME               "MQIsdp"


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
	/** MQTT connection received publish message from the MQTT broker server. */
	MQTT_CALLBACK_RECV_PUBLISH,
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
 * \brief Structure of the MQTT_CALLBACK_RECV_PUBLISH callback.
 */
struct mqtt_data_recv_publish {
	/** Topic data of received packet. */
	char *topic;
	/** Topic data size. */
	int topic_size;
	/** Message data of received packet. */
	char *msg;
	/** Message data size. */
	int msg_size;
	/** QOS of received packet. */
	uint8_t qos	        : 2;
	/** If this value is set to 1, This server is failed to send publish message at least once. */
	uint8_t duplicate   : 1;
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
	struct mqtt_data_recv_publish recv_publish;
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
	 * Timer module for the MQTT Ping timer out.
	 * Default value is NULL.
	 */
	struct sw_timer_module *timer_inst;
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
	char *recv_buffer;
	/**
	 * Maximum size of the receive buffer.
	 * If receiving data over this value, Connection is to be disconnected.
	 * Default value is 0.
	 */
	uint32_t recv_buffer_size;
	/**
	 * Send buffer size in the MQTT service.
	 * This buffer is located in the stack.
	 * Therefore, the size of the buffer increases the speed will increase, but it may cause a stack overflow.
	 * It MUST bigger than 5.
	 * Default value is 32.
	 */
	uint32_t send_buffer_size;
	/**
	 * A flag for for the processing in a device busy status
	 * If this flag set to 0, Wait until the device is idle.
	 * Otherwise, If device is is busy when calling the function, function will be returned -EAGAIN.
	 * This flag affect to following operations.
	 * CONNECT_TO_BROKER, SUBSCRIBE, UNSUBSCRIBE, PUBLISH, DISCONNET
	 * Default value is 0.
	 */
	uint8_t nonblocking;
 };

/**
 * \brief Structure of MQTT connection instance.
 */
struct mqtt_module {
	/** A ID of last message sent. */
	uint16_t last_msg_id;
	/** Socket instance of MQTT connection. */
	SOCKET sock;
	/** Host address of MQTT broker. */
	char host[HOSTNAME_MAX_SIZE];

	/* status */
	/** A flag for the socket is connected. */
	uint8_t sock_connected  : 1;
	/** A flag for the MQTT is connected. */
	uint8_t mqtt_connected  : 1;
	/** A flag for the socket is sending. */
	uint8_t sending         : 1;
	/** A flag for the service is busy. */
	uint8_t busy            : 1;
	/** A flag for the socket is terminating. */
	uint8_t terminating     : 1;
	/** A flag for the receive buffer located in the heap. */
	uint8_t alloc_buffer    : 1;

	/** Size that received. */
	uint32_t recved_size;

	/* pending ack */
	/** If any acknowledge is fail to send, This variables are set. */
	uint8_t pend_type;
	/** Message ID of pended acknowledge. */
	uint16_t pend_msg_id;

	/** Last error code of MQTT service. */
	int last_err;

	/** SW Timer ID. */
	int timer_id;
	/** Callback interface entry. */
	mqtt_callback_t cb;
	/** Configuration instance of MQTT module. That was registered from the \ref mqtt_init*/
	struct mqtt_config config;
};

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
int mqtt_init(struct mqtt_module *const module, struct mqtt_config *config);

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
int mqtt_subscribe(struct mqtt_module *const module, const char *topic, uint8_t qos);

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

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* MQTT_H_INCLUDED */
