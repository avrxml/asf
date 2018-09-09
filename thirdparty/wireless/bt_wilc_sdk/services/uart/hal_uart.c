#include "asf.h"
#include "hal_uart_dma.h"
#include "conf_board.h"
#include "stdio_serial.h" 
#include "ioport.h"
#include "hal_tick.h"
#include "btstack_debug.h"
#include "btstack_config.h"

/** All interrupt mask. */
#define ALL_INTERRUPT_MASK   0xffffffff

static void dummy_handler(void){}
	
// RX state
static volatile uint16_t  bytes_to_read = 0;
static volatile uint8_t * rx_buffer_ptr = 0;

// TX state
static volatile uint16_t  bytes_to_write = 0;
static volatile uint8_t * tx_buffer_ptr = 0;

static volatile int       rx_notify;
static volatile int       tx_notify;

static int simulate_flowcontrol;

// handlers
static void (*rx_done_handler)(void) = dummy_handler;
static void (*tx_done_handler)(void) = dummy_handler;
static void (*cts_irq_handler)(void) = dummy_handler;

// J505:6
#define DEBUG_PIN_1 PIO_PD16_IDX
// J505:5
#define DEBUG_PIN_2 PIO_PD15_IDX

static inline void hal_uart_rts_high(void){
	if (!simulate_flowcontrol) return;
	BOARD_USART->US_CR = US_CR_RTSEN;
}
static inline void hal_uart_rts_low(void){
	if (!simulate_flowcontrol) return;
	BOARD_USART->US_CR = US_CR_RTSDIS;
}

void delay_check(uint32_t value)
{
	vTaskDelay(value);
}

static int hal_uart_dma_initialized = 0;
void hal_uart_dma_init(void)
{

#ifndef CONF_MODE_WIFI_AND_BLE
	TickType_t xDelay;
#endif
	
	if (hal_uart_dma_initialized){
		log_info("hal_uart_dma_init already initialized");
		return;
	}
	hal_uart_dma_initialized = 1;

	// debug
#ifdef DEBUG_PIN_1
	ioport_set_pin_dir(DEBUG_PIN_1, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(DEBUG_PIN_1, IOPORT_PIN_LEVEL_LOW);
#endif
#ifdef DEBUG_PIN_2
	ioport_set_pin_dir(DEBUG_PIN_2, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(DEBUG_PIN_2, IOPORT_PIN_LEVEL_LOW);
#endif
	// power on
	ioport_set_pin_dir(BLUETOOTH_CHP_EN, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(BLUETOOTH_CHP_EN, IOPORT_PIN_LEVEL_HIGH);

#ifndef CONF_MODE_WIFI_AND_BLE
	// reset
	ioport_set_pin_dir(BLUETOOTH_RESET, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(BLUETOOTH_RESET, IOPORT_PIN_LEVEL_LOW);
	
	xDelay = (250 / portTICK_PERIOD_MS);
	vTaskDelay(xDelay);
	
	ioport_set_pin_level(BLUETOOTH_RESET, IOPORT_PIN_LEVEL_HIGH);
	vTaskDelay(xDelay);
#endif	

	/* Enable the peripheral clock in the PMC. */
	sysclk_enable_peripheral_clock(BOARD_ID_USART);
	
	// configure Bluetooth USART
	const sam_usart_opt_t bluetooth_settings = {
		115200,
		US_MR_CHRL_8_BIT,
		US_MR_PAR_NO,
		US_MR_NBSTOP_1_BIT,
		US_MR_CHMODE_NORMAL,
		/* This field is only used in IrDA mode. */
		0
	};

	/* Configure USART mode. */
	simulate_flowcontrol = 0;
	usart_init_rs232(BOARD_USART, &bluetooth_settings, sysclk_get_peripheral_hz());
	// Set RTS = 0 (normal mode)
	BOARD_USART->US_CR = US_CR_RTSEN;

	/* Disable all the interrupts. */
	usart_disable_interrupt(BOARD_USART, ALL_INTERRUPT_MASK);

	/* Enable TX & RX function. */
	usart_enable_tx(BOARD_USART);
	usart_enable_rx(BOARD_USART);
	
    NVIC_ClearPendingIRQ(USART0_IRQn);
    NVIC_SetPriority( USART0_IRQn ,(configMAX_SYSCALL_INTERRUPT_PRIORITY + 2));
	/* Configure and enable interrupt of USART. */
	NVIC_EnableIRQ(USART_IRQn);
}

void hal_uart_dma_set_sleep(uint8_t sleep){
}

void hal_uart_dma_set_block_received( void (*the_block_handler)(void)){
	rx_done_handler = the_block_handler;
}

void hal_uart_dma_set_block_sent( void (*the_block_handler)(void)){
	tx_done_handler = the_block_handler;
}

void hal_uart_dma_set_csr_irq_handler( void (*the_irq_handler)(void)){
	cts_irq_handler = the_irq_handler;
}

int  hal_uart_dma_set_baud(uint32_t baud){

	/* Disable TX & RX function. */
	usart_disable_tx(BOARD_USART);
	usart_disable_rx(BOARD_USART);
	uint32_t res = usart_set_async_baudrate(BOARD_USART, baud, sysclk_get_peripheral_hz());
	if (res){
		log_error("hal_uart_dma_set_baud library call failed");
	}
	/* Enable TX & RX function. */
	usart_enable_tx(BOARD_USART);
	usart_enable_rx(BOARD_USART);
	NVIC_ClearPendingIRQ(USART0_IRQn);
	NVIC_SetPriority( USART0_IRQn ,(configMAX_SYSCALL_INTERRUPT_PRIORITY + 2));
	/* Configure and enable interrupt of USART. */
	NVIC_EnableIRQ(USART_IRQn);
	return 0;
	
}

#ifdef HAVE_UART_DMA_SET_FLOWCONTROL

int hal_uart_dma_set_flowcontrol(int flowcontrol)
{
	//log_info("hal_uart_dma_set_flowcontrol %u", flowcontrol);
	simulate_flowcontrol = flowcontrol;
	if (flowcontrol){
		/* Set hardware handshaking mode. */
		BOARD_USART->US_MR = (BOARD_USART->US_MR & ~US_MR_USART_MODE_Msk) | US_MR_USART_MODE_HW_HANDSHAKING;
		hal_uart_rts_high();
	} else {
		/* Set nomal mode. */
		BOARD_USART->US_MR = (BOARD_USART->US_MR & ~US_MR_USART_MODE_Msk) | US_MR_USART_MODE_NORMAL;
		// Set RTS = 0 (normal mode)
		BOARD_USART->US_CR = US_CR_RTSEN;
	}
	return 0;
}
#endif

void hal_uart_dma_send_block(const uint8_t *data, uint16_t size){

   	tx_notify = 1;

	if (bytes_to_write){
		log_error("send block, bytes to write %u", bytes_to_write);
		return;
	}
    tx_buffer_ptr = (uint8_t *) data;
    bytes_to_write = size;
	usart_enable_interrupt(BOARD_USART, US_IER_TXRDY);
}

void hal_uart_dma_receive_block(uint8_t *data, uint16_t size)
{
	hal_uart_rts_low();
	rx_notify = 1;
    rx_buffer_ptr = data;
    bytes_to_read = size;
	usart_enable_interrupt(BOARD_USART, US_IER_RXRDY);
}

void USART_Handler(void)
{
	/* Read USART status. */
	uint32_t ul_status = usart_get_status(BOARD_USART);

	// handle ready to send
	if(ul_status & US_IER_TXRDY) {
		if (bytes_to_write){
			// send next byte
			usart_write(BOARD_USART, *tx_buffer_ptr);
			tx_buffer_ptr++;
			bytes_to_write--;
		} else {

			// done. disable tx ready interrupt to avoid starvation here
			usart_disable_interrupt(BOARD_USART, US_IER_TXRDY);
			if (tx_notify){
				tx_notify = 0;
				tx_done_handler();
			}
		}
	}

	// handle byte available for read
	if (ul_status & US_IER_RXRDY) {
			if (bytes_to_read){
			uint32_t ch;
			usart_read(BOARD_USART, (uint32_t *)&ch);
			*rx_buffer_ptr++ = ch;
			bytes_to_read--;
			if (bytes_to_read == 0){

				// done. disable rx ready interrupt, raise RTS
				hal_uart_rts_high();
				usart_disable_interrupt(BOARD_USART, US_IER_RXRDY);
				if (rx_notify){
					rx_notify = 0;
					rx_done_handler();
				}
			}
		} else {
			// shoult not happen, disable irq anyway
			usart_disable_interrupt(BOARD_USART, US_IER_RXRDY);
		}
	}
}

