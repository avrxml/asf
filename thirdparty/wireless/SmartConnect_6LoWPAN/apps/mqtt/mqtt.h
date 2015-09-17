
#ifndef MQTT_H_
#define MQTT_H_



#include "thsq.h"


#include "lib/assert.h"
#include "lib/list.h"
#include "ip64-addr.h"
#include "tcp-socket.h"

#include "string.h"

#include "sys/pt.h"

/* Protocol constants */
#define MQTT_CLIENT_ID_MAX_LEN 23



/* Size of the underlying TCP buffers */
#define MQTT_TCP_INPUT_BUFF_SIZE 256
#define MQTT_TCP_OUTPUT_BUFF_SIZE 256

/* The
 */
#define MQTT_INPUT_BUFF_SIZE 256
#define MQTT_MAX_TOPIC_LENGTH 64
#define MQTT_MAX_TOPICS_PER_SUBSCRIBE 1

#define MQTT_FHDR_SIZE 1
#define MQTT_MAX_REMAINING_LENGTH_BYTES 4
#define MQTT_PROTOCOL_VERSION 3
#define MQTT_PROTOCOL_NAME "MQIsdp"
#define MQTT_TOPIC_MAX_LENGTH 128

/* Debug configuration, this is similar but not exactly like the Debugging
 * System discussion at https://github.com/contiki-os/contiki/wiki.
 */
#define DEBUG_MQTT 0

#if DEBUG_MQTT == 1
#define DBG(...) printf(__VA_ARGS__)
#else
#define DBG(...)
#endif /* _DEBUG_ */

extern process_event_t mqtt_update_event;

/* Forward declaration */
struct mqtt_connection;

typedef enum {
  MQTT_RETAIN_OFF,
  MQTT_RETAIN_ON,
} mqtt_retain_t;

typedef enum {
  MQTT_EVENT_CONNECTED,
  MQTT_EVENT_DISCONNECTED,

  MQTT_EVENT_SUBACK,
  MQTT_EVENT_UNSUBACK,
  MQTT_EVENT_PUBLISH,
  MQTT_EVENT_PUBACK,

  /* Errors */
  MQTT_EVENT_ERROR        = 0x80,
  MQTT_EVENT_PROTOCOL_ERROR,
  MQTT_EVENT_CONNECTION_REFUSED_ERROR,
  MQTT_EVENT_DNS_ERROR,
  MQTT_EVENT_NOT_IMPLEMENTED_ERROR,
  /* Add more */
} mqtt_event_t;

typedef enum {
  MQTT_STATUS_OK,

  MQTT_STATUS_OUT_QUEUE_FULL,

  /* Errors */
  MQTT_STATUS_ERROR       = 0x80,
  MQTT_STATUS_NOT_CONNECTED_ERROR,
  MQTT_STATUS_INVALID_ARGS_ERROR,
  MQTT_STATUS_DNS_ERROR,
} mqtt_status_t;

typedef enum {
  MQTT_QOS_LEVEL_0,
  MQTT_QOS_LEVEL_1,
  MQTT_QOS_LEVEL_2,
} mqtt_qos_level_t;

typedef enum {
  MQTT_QOS_STATE_NO_ACK,
  MQTT_QOS_STATE_GOT_ACK,

  /* Expand for QoS 2 */
} mqtt_qos_state_t;

/* This is the state of the connection itself.
 *
 * N.B. The order is important because of runtime checks on how far the
 *      connection has proceeded.  */
typedef enum {
  MQTT_CONN_STATE_ERROR,
  MQTT_CONN_STATE_DNS_ERROR,
  MQTT_CONN_STATE_DISCONNECTING,

  MQTT_CONN_STATE_NOT_CONNECTED,
  MQTT_CONN_STATE_DNS_LOOKUP,
  MQTT_CONN_STATE_TCP_CONNECTING,
  MQTT_CONN_STATE_TCP_CONNECTED,
  MQTT_CONN_STATE_CONNECTING_TO_BROKER,
  MQTT_CONN_STATE_CONNECTED_TO_BROKER,
} mqtt_conn_state_t;


struct mqtt_string {
  uint8_t* string;
  uint16_t length;
};

/* Note that the pairing mid <-> QoS level only applies one-to-one if we only
 * allow the subscription of one topic at a time. Otherwise we will have an
 * ordered list of QoS levels corresponding to the order of topics.
 *
 * This could be part of a union of event data structures.
 */
struct mqtt_suback_event {
  uint16_t mid;
  mqtt_qos_level_t qos_level;
};

/**
 * TEMPORARY NOTE: This is the MQTT message that is exposed to the end user.
 */
struct mqtt_message {
  uint32_t mid;
  char topic[MQTT_MAX_TOPIC_LENGTH +1]; /* +1 for string termination */

  uint8_t* payload_chunk;
  uint16_t payload_chunk_length;

  uint8_t first_chunk;
  uint16_t payload_length;
  uint16_t payload_left;
};

/**
 *  This struct represents a packet received from the MQTT server.
 *
 *  Note that many variable
 */
struct mqtt_in_packet {
  /* Used by the list interface, must be first in the struct. */
  struct mqtt_connection* next;

  /* Total bytes read so far. Compared to the remaining length to to decide when
   * we've read the payload. */
  uint32_t byte_counter;
  uint8_t packet_received;

  uint8_t fhdr;
  uint16_t remaining_length;
  uint16_t mid;

  /* Helper variables needed to decode the remaining_length */
  uint8_t remaining_multiplier;
  uint8_t has_remaining_length;
  uint8_t remaining_length_bytes;

  /* Not the same as payload in the MQTT sense, it also contains the variable
   * header.
   */
  uint8_t payload_pos;
  uint8_t payload[MQTT_INPUT_BUFF_SIZE];

  /* Message specific data */
  uint16_t topic_len;
  uint16_t topic_pos;
  uint8_t topic_len_received;
  uint8_t topic_received;
};

/**
 *  This struct represents a packet sent to the MQTT server.
 */
struct mqtt_out_packet {
  uint8_t fhdr;
  uint32_t remaining_length;
  uint8_t remaining_length_enc[MQTT_MAX_REMAINING_LENGTH_BYTES];
  uint8_t remaining_length_enc_bytes;
  uint16_t mid;
  const char* topic;
  uint16_t topic_length;
  uint8_t* payload;
  uint32_t payload_size;
  mqtt_qos_level_t qos;
  mqtt_qos_state_t qos_state;
  mqtt_retain_t retain;
};
/**
 * \brief           MQTT event callback function
 * \param m         A pointer to a MQTT connection
 * \param user_ptr  A user-defined pointer
 * \param event     The event number
 *
 *             The MQTT socket event callback function gets
 *             called whenever there is an event on a MQTT connection, such as
 *             the connection getting connected or closed.
 */
typedef void (* mqtt_event_callback_t)(struct mqtt_connection *m,
                                       mqtt_event_t event,
                                       void* data);

typedef void (* mqtt_topic_callback_t)(struct mqtt_connection *m,
                                       struct mqtt_message* msg);

struct mqtt_will {
  struct mqtt_string topic;
  struct mqtt_string message;
  mqtt_qos_level_t qos;
};

struct mqtt_credentials {
  struct mqtt_string username;
  struct mqtt_string password;
};

struct mqtt_connection {
  /* Used by the list interface, must be first in the struct */
  struct mqtt_connection* next;

  struct mqtt_string client_id;

  uint8_t connect_vhdr_flags;
  uint8_t auto_reconnect;

  uint16_t keep_alive;
  struct ctimer keep_alive_timer;
  uint8_t waiting_for_pingresp;

  struct mqtt_will will;
  struct mqtt_credentials credentials;

  mqtt_conn_state_t state;
  mqtt_event_callback_t event_callback;

  /* Internal data */
  uint16_t mid_counter;

  /* Used for communication between MQTT API and APP */
  uint8_t out_queue_full;
  struct process* app_process;

  /* Outgoing data related */
  uint8_t* out_buffer_ptr;
  uint8_t out_buffer[MQTT_TCP_OUTPUT_BUFF_SIZE];
  uint8_t out_buffer_sent;
  struct mqtt_out_packet out_packet;
  struct pt out_proto_thread;
  uint32_t out_write_pos;

  /* Incoming data related */
  uint8_t in_buffer[MQTT_TCP_INPUT_BUFF_SIZE];
  struct mqtt_in_packet in_packet;
  struct mqtt_message in_publish_msg;


  /* TCP related information */
  const char* server_host;
  uip_ipaddr_t server_ip;
  uint16_t server_port;
  struct tcp_socket socket;
};


/* API */

/**
 * Initializes the MQTT application.
 *
 * This method shall be called before any other MQTT function.
 */
mqtt_status_t mqtt_register(struct mqtt_connection* conn,
                            struct process* app_process,
                            const char* client_id,
                            mqtt_event_callback_t event_callback);

mqtt_status_t mqtt_connect(struct mqtt_connection* conn,
                           const char* host,
                           uint16_t port,
                           uint16_t keep_alive);

void mqtt_disconnect(struct mqtt_connection* conn);

mqtt_status_t mqtt_subscribe(struct mqtt_connection* conn,
                             uint16_t* mid,
                             const char* topic,
                             mqtt_qos_level_t qos_level);

mqtt_status_t mqtt_unsubscribe(struct mqtt_connection* conn,
                               uint16_t* mid,
                               const char* topic);

mqtt_status_t mqtt_publish(struct mqtt_connection* conn,
                           uint16_t* mid,
                           const char* topic,
                           uint8_t* payload,
                           uint32_t payload_size,
                           mqtt_qos_level_t qos_level,
                           mqtt_retain_t retain);

void mqtt_set_username_password(struct mqtt_connection* conn,
                                const char* username,
                                const char* password);

void mqtt_set_last_will(struct mqtt_connection* conn,
                        const char* topic,
                        const char* message,
                        mqtt_qos_level_t qos);

#define mqtt_connected(conn) \
  ((conn)->state == MQTT_CONN_STATE_CONNECTED_TO_BROKER ? 1 : 0)

#define mqtt_ready(conn) \
  (!(conn)->out_queue_full && mqtt_connected((conn)))

/* Not implemented yet */

/* The last argument is a topic(s) specific callback. Need to handle DUP in
 * some way also, would be best if this was done automatically behind the
 * scenes. */


#endif /* MQTT_H_ */
