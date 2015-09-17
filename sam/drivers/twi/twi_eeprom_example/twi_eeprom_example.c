/**
 * \file
 *
 * \brief TWI EEPROM Example for SAM.
 *
 * Copyright (c) 2011-2015 Atmel Corporation. All rights reserved.
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
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
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
 * \mainpage TWI EEPROM Example
 *
 * \section intro Introduction
 *
 * The application demonstrates how to use the SAM TWI driver to access an
 * external serial EEPROM chip.
 *
 * \section Requirements
 *
 * This package can be used with the following setup:
 *  - SAM3X evaluation kit
 *  - SAMG53 Xplained Pro kit
 *  - SAM4N Xplained Pro kit
 *  - SAMG55 Xplained Pro kit
 *
 * \section files Main files:
 *  - twi.c SAM Two-Wire Interface driver implementation.
 *  - twi.h SAM Two-Wire Interface driver definitions.
 *  - twi_eeprom_example.c Example application.
 *
 * \section exampledescription Description of the Example
 * Upon startup, the program configures PIOs for console UART, LEDs and TWI
 * connected to EEPROM on board. Then it configures the TWI driver and data
 * package. The clock of I2C bus is set as 400kHz.
 * After initializing the master mode, the example sends test pattern to the
 * EEPROM. When sending is complete, TWI driver reads the memory and saves the
 * content in the reception buffer. Then the program compares the content
 * received with the test pattern sent before and prints the comparison result.
 * The corresponding LED is turned on.
 *
 * On SAM Xplained Pro, the EEPROM is simulated though the twi_slave_example,
 * which means 2 SAM Xplained Pro boards are required to run this example.
 * One board executes this example as the twi master and another runs
 * twi_salve_example as the simulated EEPROM.
 * The TWI PINs and GND PIN should be connected between these 2 boards.
 * More information of the simulated EEPROM implementation can be referred in
 * the description of twi_slave_example.
 *
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC and IAR EWARM.
 * Other compilers may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/">Atmel</A>.\n
 * Support and FAQ: http://www.atmel.com/design-support/
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include "asf.h"
#include "stdio_serial.h"
#include "conf_board.h"
#include "led.h"

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/// @endcond

/** EEPROM Wait Time */
#define WAIT_TIME   10
/** TWI Bus Clock 400kHz */
#define TWI_CLK     400000
/** Address of AT24C chips */
#define AT24C_ADDRESS           0x50
#define EEPROM_MEM_ADDR         0
#define EEPROM_MEM_ADDR_LENGTH  2

/** Data to be sent */
#define TEST_DATA_LENGTH  (sizeof(test_data_tx)/sizeof(uint8_t))

#define STRING_EOL    "\r"
#define STRING_HEADER "--TWI EEPROM Example --\r\n" \
		"-- "BOARD_NAME" --\r\n" \
		"-- Compiled: "__DATE__" "__TIME__" --"STRING_EOL

#if SAM4N
/** TWI ID for simulated EEPROM application to use */
#define BOARD_ID_TWI_EEPROM    ID_TWI0
/** TWI Base for simulated TWI EEPROM application to use */
#define BOARD_BASE_TWI_EEPROM  TWI0
/** The address for simulated TWI EEPROM application */
#undef  AT24C_ADDRESS
#define AT24C_ADDRESS          0x40
#endif

#if SAMG53
/** TWI ID for simulated EEPROM application to use */
#define BOARD_ID_TWI_EEPROM         ID_TWI2
/** TWI Base for simulated TWI EEPROM application to use */
#define BOARD_BASE_TWI_EEPROM       TWI2
/** The address for simulated TWI EEPROM application */
#undef  AT24C_ADDRESS
#define AT24C_ADDRESS        0x40
#endif

#if SAMG55
/** TWI ID for simulated EEPROM application to use */
#define BOARD_ID_TWI_EEPROM         ID_TWI4
/** TWI Base for simulated TWI EEPROM application to use */
#define BOARD_BASE_TWI_EEPROM       TWI4
/** The address for simulated TWI EEPROM application */
#undef  AT24C_ADDRESS
#define AT24C_ADDRESS        0x40
#endif

static const uint8_t test_data_tx[] = {
	/** SAM TWI EEPROM EXAMPLE */
	'S', 'A', 'M', ' ', 'T', 'W', 'I', ' ',
	'E', 'E', 'P', 'R', 'O', 'M', ' ',
	'E', 'X', 'A', 'M', 'P', 'L', 'E'
};

/** Reception buffer */
static uint8_t gs_uc_test_data_rx[TEST_DATA_LENGTH] = { 0 };

/** Global timestamp in milliseconds since start of application */
volatile uint32_t g_ul_ms_ticks = 0;

/**
 *  \brief Handler for System Tick interrupt.
 *
 *  Process System Tick Event
 *  increments the timestamp counter.
 */
void SysTick_Handler(void)
{
	g_ul_ms_ticks++;
}

/**
 *  \brief Configure the Console UART.
 */
static void configure_console(void)
{
	const usart_serial_options_t uart_serial_options = {
		.baudrate = CONF_UART_BAUDRATE,
#ifdef CONF_UART_CHAR_LENGTH
		.charlength = CONF_UART_CHAR_LENGTH,
#endif
		.paritytype = CONF_UART_PARITY,
#ifdef CONF_UART_STOP_BITS
		.stopbits = CONF_UART_STOP_BITS,
#endif
	};

	/* Configure console UART. */
	sysclk_enable_peripheral_clock(CONSOLE_UART_ID);
	stdio_serial_init(CONF_UART, &uart_serial_options);
}

/**
 *  \brief Wait for the given number of milliseconds (using the dwTimeStamp
 *         generated by the SAM microcontrollers' system tick).
 *  \param ul_dly_ticks  Delay to wait for, in milliseconds.
 */
static void mdelay(uint32_t ul_dly_ticks)
{
	uint32_t ul_cur_ticks;

	ul_cur_ticks = g_ul_ms_ticks;
	while ((g_ul_ms_ticks - ul_cur_ticks) < ul_dly_ticks);
}

/**
 * \brief Application entry point for TWI EEPROM example.
 *
 * \return Unused (ANSI-C compatibility).
 */
int main(void)
{
	uint32_t i;
	twi_options_t opt;
	twi_packet_t packet_tx, packet_rx;

	/* Initialize the SAM system */
	sysclk_init();

	/* Initialize the board */
	board_init();

	/* Turn off LEDs */
#if SAM3XA
	LED_Off(LED0_GPIO);
	LED_Off(LED1_GPIO);
#else
	LED_Off(LED0);
#endif
	/* Initialize the console UART */
	configure_console();

	/* Output example information */
	puts(STRING_HEADER);

	/* Configure systick for 1 ms */
	puts("Configure system tick to get 1ms tick period.\r");
	if (SysTick_Config(sysclk_get_cpu_hz() / 1000)) {
		puts("-E- Systick configuration error\r");
		while (1) {
			/* Capture error */
		}
	}

#if (SAMG55)
	/* Enable the peripheral and set TWI mode. */
	flexcom_enable(BOARD_FLEXCOM_TWI);
	flexcom_set_opmode(BOARD_FLEXCOM_TWI, FLEXCOM_TWI);
#else
	/* Enable the peripheral clock for TWI */
	pmc_enable_periph_clk(BOARD_ID_TWI_EEPROM);
#endif

	/* Configure the options of TWI driver */
	opt.master_clk = sysclk_get_cpu_hz();
	opt.speed      = TWI_CLK;

	/* Configure the data packet to be transmitted */
	packet_tx.chip        = AT24C_ADDRESS;
	packet_tx.addr[0]     = EEPROM_MEM_ADDR >> 8;
	packet_tx.addr[1]     = EEPROM_MEM_ADDR;
	packet_tx.addr_length = EEPROM_MEM_ADDR_LENGTH;
	packet_tx.buffer      = (uint8_t *) test_data_tx;
	packet_tx.length      = TEST_DATA_LENGTH;

	/* Configure the data packet to be received */
	packet_rx.chip        = packet_tx.chip;
	packet_rx.addr[0]     = packet_tx.addr[0];
	packet_rx.addr[1]     = packet_tx.addr[1];
	packet_rx.addr_length = packet_tx.addr_length;
	packet_rx.buffer      = gs_uc_test_data_rx;
	packet_rx.length      = packet_tx.length;

	if (twi_master_init(BOARD_BASE_TWI_EEPROM, &opt) != TWI_SUCCESS) {
		puts("-E-\tTWI master initialization failed.\r");
#if SAM3XA
		LED_On(LED0_GPIO);
		LED_On(LED1_GPIO);
#endif
		while (1) {
			/* Capture error */
		}
	}

	/* Send test pattern to EEPROM */
	if (twi_master_write(BOARD_BASE_TWI_EEPROM, &packet_tx) != TWI_SUCCESS) {
		puts("-E-\tTWI master write packet failed.\r");
#if SAM3XA
		LED_On(LED0_GPIO);
		LED_On(LED1_GPIO);
#endif
		while (1) {
			/* Capture error */
		}
	}
	printf("Write:\tOK!\n\r");

	/* Wait at least 10 ms */
	mdelay(WAIT_TIME);

	/* Get memory from EEPROM */
	if (twi_master_read(BOARD_BASE_TWI_EEPROM, &packet_rx) != TWI_SUCCESS) {
		puts("-E-\tTWI master read packet failed.\r");
#if SAM3XA
		LED_On(LED0_GPIO);
		LED_On(LED1_GPIO);
#endif
		while (1) {
			/* Capture error */
		}
	}
	puts("Read:\tOK!\r");

	/* Compare the sent and the received */
	for (i = 0; i < TEST_DATA_LENGTH; i++) {
		if (test_data_tx[i] != gs_uc_test_data_rx[i]) {
			/* No match */
			puts("Data comparison:\tUnmatched!\r");
#if SAM3XA
			LED_On(LED0_GPIO);
#endif
			while (1) {
				/* Capture error */
			}
		}
	}
	/* Match */
	puts("Data comparison:\tMatched!\r");
#if SAM3XA
	LED_On(LED1_GPIO);
#else
	LED_On(LED0);
#endif
	while (1) {
	}
}

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/// @endcond
