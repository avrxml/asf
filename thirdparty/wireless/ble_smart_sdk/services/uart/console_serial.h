
#ifndef COSOLE_SERIAL_H
#define COSOLE_SERIAL_H

#include <uart.h>

typedef void (*uart_rx_callback_t)(uint8_t input);

void serial_console_init(void);
int getchar_b11(void);
int getchar_b11_timeout(unsigned int sec);
void register_uart_callback(uart_rx_callback_t callback_func);

#endif /* COSOLE_SERIAL_H */