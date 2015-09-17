



#ifndef OTA_PARSER_H
#define OTA_PARSER_H
#include "compiler.h"
#include "ota.h"

/**
 * A UART state that expects a \ref SOF to be received as the next character.
 */
#define UART_RX_STATE_SOF               (1)

/**
 * A UART state that expects a \ref FCF to be received as the next character.
 */
#define UART_RX_STATE_FCF               (2)

/**
 * A UART state that expects the length to be received as the next character.
 */
#define UART_RX_STATE_LENGTH_1          (3)

/**
 * A UART state that expects the length to be received as the next character.
 */
#define UART_RX_STATE_LENGTH_2          (4)

/**
 * A UART state that expects the next byte as Sequence number to be received.
 */
#define UART_RX_STATE_SEQ               (5)

/**
 * A UART state that expects the next byte as Stack identifier to be received.
 */
#define UART_RX_STATE_STACK_ID          (6)

/**
 * A UART state that expects the next byte as Stack identifier to be received.
 */
#define UART_RX_STATE_DOMAIN            (7)

/**
 * A UART state that expects the next data character to be received.
 */
#define UART_RX_STATE_PAYLOAD           (8)

/**
 * A UART state that expects a FCS to be received as the next character.
 */
#define UART_RX_STATE_FCS               (9)


/** The start of transmission delimiter. */
#define SOF                             (0x2A)

#define SIO_RX_BUF_SIZE                 (156)
#define SIO_TX_BUF_SIZE                 SIO_RX_BUF_SIZE

#define STACK_ID						(0x00)

#define SIO_FRAME_TYPE_MASK             (0xC0)

#define SIO_FRAME_TYPE_ACK              (0 << 6)

#define SIO_FRAME_TYPE_SREQ             (1 << 6)

#define SIO_FRAME_TYPE_AREQ_ARSP        (2 << 6)

#define SIO_FRAME_TYPE_SRSP             (3 << 6)

#define SIO_FRAME_CTRL_OVERFLOW         (1 << 1)

#define SIO_FRAME_CTRL_INVALID_FCS      (1 << 0)

#define SIO_FRAME_CTRL_VALID_FCS        (0 << 0)

void ota_parser_init(void);
void serial_data_handler(void);
void send_pc_data(ota_domain_t domain_id, uint8_t msg_id, uint8_t *msg, uint16_t len);
void send_server_data(ota_domain_t domain_id, addr_mode_t addr_mode, uint8_t *addr, uint8_t msg_id, uint8_t *msg, uint16_t len);
#endif /* OTA_PARSER_H */
