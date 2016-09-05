#include <asf.h>
#include <conf_console_serial.h>
#include <string.h>
#include <stdarg.h>

#include "console_serial.h"


struct uart_module uart_instance;
struct uart_config config_uart;

unsigned char string_input[1] = {0};
unsigned int tick = 0 ;

uart_rx_callback_t user_callback_func = NULL;

enum uart_read_status {
	UART_READ_NONE,
	UART_READ_WAITING,
	UART_READ_DONE
};

volatile enum uart_read_status read_status = UART_READ_NONE;

void _time_start(unsigned int sec);
unsigned int _time_done(void);

static void uart_read_complete_callback(struct uart_module *const module)
{
	if(read_status == UART_READ_WAITING) {
		read_status = UART_READ_DONE;
		
		if(user_callback_func == NULL)
			uart_disable_callback(&uart_instance, UART_RX_COMPLETE);
	}
	else if(user_callback_func != NULL) {
		user_callback_func(string_input[0]);
		uart_read_buffer_job(&uart_instance, string_input, sizeof(string_input));
	}
	else {
		//Nothing to do.
	}
}

void serial_console_init(void)
{
	system_clock_config(CLOCK_RESOURCE_XO_26_MHZ, CLOCK_FREQ_26_MHZ);

	uart_get_config_defaults(&config_uart);

	config_uart.baud_rate = CONF_STDIO_BAUDRATE;
	config_uart.pin_number_pad[0] = EDBG_CDC_PIN_PAD0;
	config_uart.pin_number_pad[1] = EDBG_CDC_PIN_PAD1;
	config_uart.pin_number_pad[2] = EDBG_CDC_PIN_PAD2;
	config_uart.pin_number_pad[3] = EDBG_CDC_PIN_PAD3;
	
	config_uart.pinmux_sel_pad[0] = EDBG_CDC_MUX_PAD0;
	config_uart.pinmux_sel_pad[1] = EDBG_CDC_MUX_PAD1;
	config_uart.pinmux_sel_pad[2] = EDBG_CDC_MUX_PAD2;
	config_uart.pinmux_sel_pad[3] = EDBG_CDC_MUX_PAD3;

	stdio_serial_init(&uart_instance, CONF_STDIO_USART_MODULE, &config_uart);
	
	uart_register_callback(&uart_instance, uart_read_complete_callback, UART_RX_COMPLETE);
}

void _time_start(unsigned int sec)
{
	system_clock_get_value();
	unsigned int main_clk = system_clock_get_value();

	tick = (((double)(main_clk)) * ((double)0.0000493)) * (sec * 1000);
}

unsigned int _time_done()
{
	return --tick;
}

int getchar_b11_timeout(unsigned int sec)
{
	read_status = UART_READ_WAITING;
	
	if(user_callback_func == NULL)
		uart_enable_callback(&uart_instance, UART_RX_COMPLETE);

	_time_start(sec);
	
	string_input[0] = 0;
	uart_read_buffer_job(&uart_instance, string_input, sizeof(string_input));
	
	while (read_status != UART_READ_DONE && _time_done() > 0 );
	
	read_status = UART_READ_NONE;

	return string_input[0];
}

int getchar_b11(void)
{
	read_status = UART_READ_WAITING;
	
	if(user_callback_func == NULL)
		uart_enable_callback(&uart_instance, UART_RX_COMPLETE);
	
	string_input[0] = 0;
	uart_read_buffer_job(&uart_instance, string_input, sizeof(string_input));
	
	while(read_status != UART_READ_DONE);
	
	read_status = UART_READ_NONE;
	return string_input[0];
}

void register_uart_callback(uart_rx_callback_t callback_func)
{
	user_callback_func = callback_func;
	uart_enable_callback(&uart_instance, UART_RX_COMPLETE);
	uart_read_buffer_job(&uart_instance, string_input, sizeof(string_input));
}