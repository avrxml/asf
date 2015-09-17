

#include "stddef.h"
#include "stdint.h"
#include "ota_parser.h"
//#include "bmm.h"
//#include "qmm.h"
#include "sio2host.h"
#include "ota.h"
#include "ota_mgr.h"
//#include "halUart.h"


static void process_incoming_sio_data(void);
static void send_ack(uint8_t seq_no, uint8_t state);

/**
 * This is the receive buffer of the UART.
 */
uint8_t sio_rx_buf[SIO_RX_BUF_SIZE];

/**
 * This pointer points to the next free element inside the
 * receive buffer of the UART.
 */
static uint8_t *sio_rx_ptr;

/**
 * This is the receiver state of the UART. (\ref UART_RX_STATE_SOT,
 *   \ref UART_RX_STATE_LENGTH, \ref UART_RX_STATE_DATA, or
 *   \ref UART_RX_STATE_EOT)
 */
static volatile uint8_t sio_rx_state = UART_RX_STATE_SOF;

/**
 * This is the length of the message received.
 */
static uint16_t sio_rx_length;

static uint16_t frame_length;

static uint8_t sio_seq_no;

static uint8_t tx_seq_no = 0;

static uint8_t sio_fcs;

/**
 * This is the length of the message should be transmitted.
 */
static uint8_t data[SIO_RX_BUF_SIZE];
static uint8_t data_length = 0;
static uint8_t rx_index = 0;

//queue_t msg_from_tool_q;
//queue_t msg_to_tool_q;

static uint8_t ack_frame = 0;

void ota_parser_init(void)
{
	//sio2host_init();
	sio_rx_state = UART_RX_STATE_SOF;
	//qmm_queue_init(&msg_from_tool_q);
	//qmm_queue_init(&msg_to_tool_q);
}


/**
 * @brief Process data received from SIO
 */
static void process_incoming_sio_data(void)
{
	switch (sio_rx_state) {
	case UART_RX_STATE_SOF:
		sio_rx_ptr = sio_rx_buf;
		sio_rx_length = 0;
		sio_fcs = 0;
		ack_frame = 0;
		if (SOF == data[rx_index]) {
			sio_rx_state = UART_RX_STATE_FCF;
		}
		break;

	case UART_RX_STATE_FCF:
		if (SIO_FRAME_TYPE_ACK == (data[rx_index] & SIO_FRAME_TYPE_MASK)) {
			ack_frame = 1;
		}
		sio_rx_state = UART_RX_STATE_LENGTH_1;
		break;

	case UART_RX_STATE_LENGTH_1:
		sio_rx_length = data[rx_index];
		sio_rx_state = UART_RX_STATE_LENGTH_2;
		break;

	case UART_RX_STATE_LENGTH_2:
		sio_rx_length = sio_rx_length | (data[rx_index] << 8);
		if (sio_rx_length) {
			sio_rx_state = UART_RX_STATE_SEQ;
		} else {
			sio_rx_state = UART_RX_STATE_SOF;
		}
		break;

	case UART_RX_STATE_SEQ:
		sio_seq_no = data[rx_index];
		sio_rx_state = UART_RX_STATE_STACK_ID;
		sio_rx_length--;
		sio_fcs = data[rx_index];
		break;
	
	case UART_RX_STATE_STACK_ID:
		if(STACK_ID == data[rx_index]){
			sio_rx_length--;
			sio_fcs ^= data[rx_index];
			if (ack_frame)
			{
				sio_rx_state = UART_RX_STATE_FCS;
			} 
			else
			{
				sio_rx_state = UART_RX_STATE_DOMAIN;
			}
		}
		else{
			sio_rx_state = UART_RX_STATE_SOF;
		}
		break;

	case UART_RX_STATE_DOMAIN:
		if (DOMAIN_OTA_COMMON == data[rx_index] || DOMAIN_OTA_UPGRADE == data[rx_index])
		{
			sio_rx_state = UART_RX_STATE_PAYLOAD;
			*sio_rx_ptr = data[rx_index];
			sio_rx_ptr++;
			frame_length = sio_rx_length;
			sio_rx_length--;
			sio_fcs ^= data[rx_index];
		}
		else
		{
			sio_rx_state = UART_RX_STATE_SOF;
		}
		break;

	case UART_RX_STATE_PAYLOAD:
		*sio_rx_ptr = data[rx_index];
		sio_rx_ptr++;
		--sio_rx_length;
		sio_fcs ^= data[rx_index];
		if (!sio_rx_length) {
			sio_rx_state = UART_RX_STATE_FCS;
		}
		break;

	case UART_RX_STATE_FCS:
		if (ack_frame)
		{
			sio_rx_state = UART_RX_STATE_SOF;
			break;
		}
		if (sio_fcs == data[rx_index]) {
			send_ack(sio_seq_no, SIO_FRAME_CTRL_VALID_FCS);
			/* Message received successfully */
			handle_ota_msg(frame_length, sio_rx_buf);
		}
		else
		{
			send_ack(sio_seq_no, SIO_FRAME_CTRL_INVALID_FCS);
		}
		
		/* Make rx buffer ready for next reception before handling
		 * received data. */
//		sio_rx_ptr = sio_rx_buf;
		sio_rx_state = UART_RX_STATE_SOF;
		break;

	default:
		sio_rx_state = UART_RX_STATE_SOF;
		break;
	}
}


/*
 * @brief Function to handle the state machine serial data exchange.
 */
void serial_data_handler(void)
{
	/* Rx processing */
	if (data_length == 0) {
		/* No data to process, read the stream IO */
		rx_index = 0;
		data_length = sio2host_rx(&data[0], SIO_RX_BUF_SIZE); /* @ToDo 20 ?,
		                                                   * different
		                                                   * values for
		                                                   * USB and
		                                                   * UART ? */
	} else { /* Data has been received, process the data */
		/* Process each single byte */
		process_incoming_sio_data();
		data_length--;
		rx_index++;
	}
}

void send_server_data(ota_domain_t domain_id, addr_mode_t addr_mode, uint8_t *addr, uint8_t msg_id, uint8_t *msg, uint16_t len)
{
	uint8_t fcs = 0;
	uint8_t total_len;
	uint8_t addr_size;
	if(NATIVE_ADDR_MODE == addr_mode)
	{
		addr_size = NATIVE_ADDR_SIZE;
	}
	else
	{
		addr_size = EXTENDED_ADDR_SIZE;
	}
	total_len = len + 5 + 1 + addr_size; //Seq no + stack id + Domain + command id + msg_id
	sio2host_putchar(SOF);
	sio2host_putchar(0x80); //AREQ/ARSP
	sio2host_putchar((uint8_t)total_len);
	sio2host_putchar((uint8_t)total_len >> 8);
	sio2host_putchar(tx_seq_no);
	sio2host_putchar(STACK_ID);
	sio2host_putchar(domain_id);
	sio2host_putchar(0x00);
	sio2host_putchar(msg_id);
	sio2host_putchar(addr_mode);
	sio2host_tx(addr, addr_size);
	fcs = tx_seq_no ^ STACK_ID ^ domain_id ^ 0x00 ^ msg_id ^ addr_mode;
	for(uint8_t index = 0; index < addr_size; index++)
	{
		fcs ^= *(addr + index);
	}
	if(len)
	{
		sio2host_tx(msg, len);
		for(uint8_t index = 0; index < len; index++)
		{
			fcs ^= *(msg + index);
		}
	}
	sio2host_putchar(fcs);
	tx_seq_no++;
}

void send_pc_data(ota_domain_t domain_id, uint8_t msg_id, uint8_t *msg, uint16_t len)
{
	uint8_t fcs = 0;
	uint8_t total_len = len + 5; //Seq no + stack id + Domain + command id + msg_id
	sio2host_putchar(SOF);
	sio2host_putchar(0x80); //AREQ/ARSP
	sio2host_putchar((uint8_t)total_len);
	sio2host_putchar((uint8_t)total_len >> 8);
	sio2host_putchar(tx_seq_no); //constant sequence number as of now...
	sio2host_putchar(STACK_ID);
	sio2host_putchar(domain_id);
	sio2host_putchar(COMMON);
	sio2host_putchar(msg_id);
	fcs = tx_seq_no ^ STACK_ID ^ domain_id ^ COMMON ^ msg_id;
	sio2host_tx(msg, len);
	for(uint8_t index = 0; index < len; index++)
	{
		fcs ^= *(msg + index);
	}
	sio2host_putchar(fcs);
	tx_seq_no++;
}

static void send_ack(uint8_t seq_no, uint8_t state)
{
	sio2host_putchar(SOF);
	sio2host_putchar(state & 0x3F);
	sio2host_putchar(0x02);
	sio2host_putchar(0x00);
	sio2host_putchar(seq_no);
	sio2host_putchar(STACK_ID);
	sio2host_putchar(seq_no ^ STACK_ID); //for FCS
}