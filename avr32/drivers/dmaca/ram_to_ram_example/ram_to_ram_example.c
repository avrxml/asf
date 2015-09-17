/*****************************************************************************
 *
 * \file
 *
 * \brief DMACA RAM -> RAM evaluation for AVR32 UC3.
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
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
 *****************************************************************************/

 /*! \mainpage
 * \section intro Introduction
 * This is the documentation for the data structures, functions, variables, defines, enums, and
 * typedefs for the DMACA driver. <BR>It also gives an example of the usage of the
 * DMACA module, eg: <BR>
 * - [on EVK1104S
 *
 * \section files Main Files
 * - dmaca.h : DMACA header file
 * - ram_to_ram_example.c : DMACA code example which benchmark transfer rates of the following path:
 *                              - CPUSRAM to CPUSRAM
 *                              - HSBSRAM0 to HSBSRAM1
 *                              - CPUSRAM to HSBSRAM1
 *                              - HSBSRAM0 to CPUSRAM
 *
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC for AVR32 and IAR Systems compiler
 * for AVR UC3. Other compilers may or may not work.
 *
 * \section deviceinfo Device Info
 * All AVR UC3 devices with a ADC module can be used. This example has been tested
 * with the following setup:<BR>
 * - EVK1104 evaluation kit with AT32UC3A3256.
 * - UC3 A3 Xplained evaluation kit with AT32UC3A3256.
 *
 * \section setupinfo Setup Information
 * - [on EVK1104 only] Connect a PC USB cable to the USB VCP plug (the USB plug
 * on the right) of the EVK1104. The PC is used as a power source. The UC3A3256
 * USART1 is connected to the UC3B USART1. The UC3B holds a firmware that acts as
 * a USART to USB gateway. On the USB side, the UC3B firmware implements a USB
 * CDC class: when connected to a PC, it will enumerate as a Virtual Com Port.
 * Once the UC3B USB is correctly installed on Windows, to communicate on this
 * port, open a HyperTerminal configured with the following settings: 57600 bps,
 * 8 data bits, no parity bit, 1 stop bit, no flow control.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/products/AVR32/">Atmel AVR UC3</A>.\n
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <string.h>
#include "compiler.h"
#include "board.h"
#include "print_funcs.h"
#include "gpio.h"
#include "pm.h"
#include "intc.h"
#include "usart.h"


/*! \name Board-Related Example Settings
 */
//! @{
#if BOARD == EVK1104
#  define DMACA_RAM2RAM_EVAL_USART               (&AVR32_USART1)
#  define DMACA_RAM2RAM_EVAL_USART_RX_PIN        AVR32_USART1_RXD_0_0_PIN
#  define DMACA_RAM2RAM_EVAL_USART_RX_FUNCTION   AVR32_USART1_RXD_0_0_FUNCTION
#  define DMACA_RAM2RAM_EVAL_USART_TX_PIN        AVR32_USART1_TXD_0_0_PIN
#  define DMACA_RAM2RAM_EVAL_USART_TX_FUNCTION   AVR32_USART1_TXD_0_0_FUNCTION
#  define DMACA_RAM2RAM_EVAL_USART_BAUDRATE      57600
#  define DMACA_RAM2RAM_EVAL_LED1                LED0_GPIO
#  define DMACA_RAM2RAM_EVAL_LED2                LED1_GPIO
#  define DMACA_RAM2RAM_EVAL_LED3                LED2_GPIO
#  define DMACA_RAM2RAM_EVAL_LED4                LED3_GPIO
#elif BOARD == UC3_A3_XPLAINED
#  define DMACA_RAM2RAM_EVAL_USART               (&AVR32_USART1)
#  define DMACA_RAM2RAM_EVAL_USART_RX_PIN        AVR32_USART1_RXD_0_0_PIN
#  define DMACA_RAM2RAM_EVAL_USART_RX_FUNCTION   AVR32_USART1_RXD_0_0_FUNCTION
#  define DMACA_RAM2RAM_EVAL_USART_TX_PIN        AVR32_USART1_TXD_0_0_PIN
#  define DMACA_RAM2RAM_EVAL_USART_TX_FUNCTION   AVR32_USART1_TXD_0_0_FUNCTION
#  define DMACA_RAM2RAM_EVAL_USART_BAUDRATE      57600
#  define DMACA_RAM2RAM_EVAL_LED1                LED0_GPIO
#  define DMACA_RAM2RAM_EVAL_LED2                LED1_GPIO
#  define DMACA_RAM2RAM_EVAL_LED3                LED2_GPIO
#  define DMACA_RAM2RAM_EVAL_LED4                LED3_GPIO
#endif

#if !defined(DMACA_RAM2RAM_EVAL_USART)             || \
    !defined(DMACA_RAM2RAM_EVAL_USART_RX_PIN)      || \
    !defined(DMACA_RAM2RAM_EVAL_USART_RX_FUNCTION) || \
    !defined(DMACA_RAM2RAM_EVAL_USART_TX_PIN)      || \
    !defined(DMACA_RAM2RAM_EVAL_USART_TX_FUNCTION) || \
    !defined(DMACA_RAM2RAM_EVAL_USART_BAUDRATE)    || \
    !defined(DMACA_RAM2RAM_EVAL_LED1)              || \
    !defined(DMACA_RAM2RAM_EVAL_LED2)              || \
    !defined(DMACA_RAM2RAM_EVAL_LED3)              || \
    !defined(DMACA_RAM2RAM_EVAL_LED4)
#  error The USART and LEDs configuration to use for debug on your board is missing
#endif
//! @}

#define DMACA_RAM2RAM_EVAL_BUF_SIZE             256

volatile uint32_t source_data_cpu_sram[DMACA_RAM2RAM_EVAL_BUF_SIZE]; // Src data array in CPUSRAM
volatile uint32_t dest_data_cpu_sram[DMACA_RAM2RAM_EVAL_BUF_SIZE];   // Dst data array in CPUSRAM
volatile uint32_t *source_data_hsb_sram_ptr;                         // Source data array in HSBSRAM
volatile uint32_t *dest_data_hsb_sram_ptr;                           // Destination data array in HSBSRAM
volatile uint32_t start_cycle_count, end_cycle_count;                // Performance evaluation variables


static void test_ram_to_ram(uint16_t u16BufferSize, volatile uint32_t *pSrcBuf, volatile uint32_t *pDstBuf);
static void init_hmatrix(void);


/*! \brief The DMACA Transfer Complete interrupt handler.
 *
 * \note The `__attribute__((__interrupt__))' (under GNU GCC for AVR32) and
 *       `__interrupt' (under IAR Embedded Workbench for Atmel AVR32) C function
 *       attributes are used to manage the `rete' instruction.
 */
#if __GNUC__
__attribute__((__interrupt__))
#elif __ICCAVR32__
__interrupt
#endif
static void dmaca_transfer_int_handler(void)
{
	uint32_t dma_status;

	gpio_clr_gpio_pin(DMACA_RAM2RAM_EVAL_LED1);

	// Clear the DMA Transfer Complete interrupt for channel 2.
	AVR32_DMACA.cleartfr = (1 << AVR32_DMACA_CLEARTFR_CLEAR2_OFFSET);

	// We can use the StatusInt register because we're using only one DMACA channel.
	dma_status = AVR32_DMACA.statusint;
	if(dma_status & (AVR32_DMACA_STATUSINT_DSTT_MASK | AVR32_DMACA_STATUSINT_SRCT_MASK | AVR32_DMACA_STATUSINT_BLOCK_MASK))
		gpio_clr_gpio_pin(DMACA_RAM2RAM_EVAL_LED3);
}


/*! \brief The DMACA Error interrupt handler.
 *
 * \note The `__attribute__((__interrupt__))' (under GNU GCC for AVR32) and
 *       `__interrupt' (under IAR Embedded Workbench for Atmel AVR32) C function
 *       attributes are used to manage the `rete' instruction.
 */
#if __GNUC__
__attribute__((__interrupt__))
#elif __ICCAVR32__
__interrupt
#endif
static void dmaca_error_int_handler(void)
{
	uint32_t dma_status;

	// This interrupt should ideally not happen. If it does, the DMACA transfer is
	// cancelled and the channel is disabled.

	gpio_clr_gpio_pin(DMACA_RAM2RAM_EVAL_LED2);

	// Clear the DMACA Error interrupt for channel 2.
	AVR32_DMACA.clearerr = (1 << AVR32_DMACA_CLEARERR_CLEAR2_OFFSET);

	// We can use the StatusInt register because we're using only one DMACA channel.
	dma_status = AVR32_DMACA.statusint;
	if(dma_status & (AVR32_DMACA_STATUSINT_DSTT_MASK | AVR32_DMACA_STATUSINT_SRCT_MASK | AVR32_DMACA_STATUSINT_BLOCK_MASK))
		gpio_clr_gpio_pin(DMACA_RAM2RAM_EVAL_LED4);
}


/*! \brief The main function.
 *
 * It sets up the USART module on EXAMPLE_USART. The terminal settings are 57600
 * 8N1.
 * Then it sets up the interrupt handler and waits for a USART interrupt to
 * trigger.
 */
int main(void)
{
#if (BOARD == EVK1104) || (BOARD == UC3_A3_XPLAINED)
	pm_freq_param_t pm_freq_param =
	{
		.cpu_f        = DMACA_RAM2RAM_EVAL_CPU_FREQ,
		.pba_f        = DMACA_RAM2RAM_EVAL_CPU_FREQ,
		.osc0_f       = FOSC0,
		.osc0_startup = OSC0_STARTUP
	};

	// Start external oscillator as the main system clock
	if (pm_configure_clocks(&pm_freq_param) == PM_FREQ_STATUS_FAIL)
		while (1);
#endif

	init_hmatrix();

	// USART GPIO map
	static const gpio_map_t USART_GPIO_MAP =
	{
		{DMACA_RAM2RAM_EVAL_USART_RX_PIN, DMACA_RAM2RAM_EVAL_USART_RX_FUNCTION},
		{DMACA_RAM2RAM_EVAL_USART_TX_PIN, DMACA_RAM2RAM_EVAL_USART_TX_FUNCTION}
	};

	// USART options.
	static const usart_options_t USART_OPTIONS =
	{
		.baudrate     = DMACA_RAM2RAM_EVAL_USART_BAUDRATE,
		.charlength   = 8,
		.paritytype   = USART_NO_PARITY,
		.stopbits     = USART_1_STOPBIT,
		.channelmode  = USART_NORMAL_CHMODE
	};

	// Assign GPIO to USART.
	gpio_enable_module(USART_GPIO_MAP,
		sizeof(USART_GPIO_MAP) / sizeof(USART_GPIO_MAP[0]));

	// Initialize USART in RS232 mode.
	usart_init_rs232(DMACA_RAM2RAM_EVAL_USART, &USART_OPTIONS, DMACA_RAM2RAM_EVAL_CPU_FREQ);
	print(DMACA_RAM2RAM_EVAL_USART, "\x1B[2J\x1B[H.: RAM to RAM transfers with the DMACA at ");
	print_ulong(DMACA_RAM2RAM_EVAL_USART, DMACA_RAM2RAM_EVAL_CPU_FREQ);
	print(DMACA_RAM2RAM_EVAL_USART, "Hz :.\r\n\r\n");

	// Disable all interrupts.
	Disable_global_interrupt();

	// Initialize interrupt vectors.
	INTC_init_interrupts();

	// Register to two DMACA interrupts to the interrupt controller.
	// dmaca_tfrint_handler() is the interrupt handler for the Transfer Complete interrupt.
	// dmaca_errint_handler() is the interrupt handler for the Error interrupt.
	INTC_register_interrupt(&dmaca_transfer_int_handler, AVR32_DMACA_TFR_IRQ, AVR32_INTC_INT0);
	INTC_register_interrupt(&dmaca_error_int_handler, AVR32_DMACA_ERR_IRQ, AVR32_INTC_INT0);

	// Enable all interrupts.
	Enable_global_interrupt();

	// Fill the input arrays with test data
	for(uint32_t i = 0; i < DMACA_RAM2RAM_EVAL_BUF_SIZE; i++) {
		source_data_cpu_sram[i]     = i;
		source_data_hsb_sram_ptr[i] = i;
	}

	// Set the source and destination array addresses to respectively HSBSRAM0 & HSBRAM1.
	source_data_hsb_sram_ptr = (void *)AVR32_INTRAM0_ADDRESS;
	dest_data_hsb_sram_ptr   = (void *)AVR32_INTRAM1_ADDRESS;

	//****************************************************************************
	// DMACA transfer : CPUSRAM -> CPUSRAM
	//  - input of 256 32bit words in CPUSRAM
	//  - output of 256 32bit words in CPUSRAM
	//****************************************************************************
	print(DMACA_RAM2RAM_EVAL_USART, "\r\n\r\n---------------------------------------------------\r\n");
	print(DMACA_RAM2RAM_EVAL_USART, "------ DMACA transfer: CPUSRAM -> CPUSRAM ------\r\n");
	print(DMACA_RAM2RAM_EVAL_USART, "       - input of 256 32bit words in CPUSRAM\r\n");
	print(DMACA_RAM2RAM_EVAL_USART, "       - output of 256 32bit words in CPUSRAM\r\n");
	print(DMACA_RAM2RAM_EVAL_USART, "---------------------------------------------------\r\n");

	// Perform the RAM to RAM transfer
	test_ram_to_ram(DMACA_RAM2RAM_EVAL_BUF_SIZE, source_data_cpu_sram, dest_data_cpu_sram);


	//****************************************************************************
	// DMACA transfer : HSBSRAM0 -> HSBSRAM1
	//  - input of 256 32bit words in HSBSRAM0
	//  - output of 256 32bit words in HSBSRAM1
	//****************************************************************************
	print(DMACA_RAM2RAM_EVAL_USART, "\r\n\r\n---------------------------------------------------\r\n");
	print(DMACA_RAM2RAM_EVAL_USART, "------ DMACA transfer: HSBSRAM0 -> HSBSRAM1 ------\r\n");
	print(DMACA_RAM2RAM_EVAL_USART, "       - input of 256 32bit words in HSBSRAM0\r\n");
	print(DMACA_RAM2RAM_EVAL_USART, "       - output of 256 32bit words in HSBSRAM1\r\n");
	print(DMACA_RAM2RAM_EVAL_USART, "---------------------------------------------------\r\n");

	// Perform the RAM to RAM transfer
	test_ram_to_ram(DMACA_RAM2RAM_EVAL_BUF_SIZE, source_data_hsb_sram_ptr, dest_data_hsb_sram_ptr);


	//****************************************************************************
	// DMACA transfer : CPUSRAM -> HSBSRAM1
	//  - input of 256 32bit words in CPUSRAM
	//  - output of 256 32bit words in HSBSRAM1
	//****************************************************************************
	print(DMACA_RAM2RAM_EVAL_USART, "\r\n\r\n---------------------------------------------------\r\n");
	print(DMACA_RAM2RAM_EVAL_USART, "------ DMACA transfer: CPUSRAM -> HSBSRAM1 ------\r\n");
	print(DMACA_RAM2RAM_EVAL_USART, "       - input of 256 32bit words in CPUSRAM\r\n");
	print(DMACA_RAM2RAM_EVAL_USART, "       - output of 256 32bit words in HSBSRAM1\r\n");
	print(DMACA_RAM2RAM_EVAL_USART, "---------------------------------------------------\r\n");

	// Perform the RAM to RAM transfer
	test_ram_to_ram(DMACA_RAM2RAM_EVAL_BUF_SIZE, source_data_cpu_sram, dest_data_hsb_sram_ptr);


	//****************************************************************************
	// DMACA transfer : HSBSRAM0 -> CPUSRAM
	//  - input of 256 32bit words in HSBSRAM0
	//  - output of 256 32bit words in CPUSRAM
	//****************************************************************************
	print(DMACA_RAM2RAM_EVAL_USART, "\r\n\r\n---------------------------------------------------\r\n");
	print(DMACA_RAM2RAM_EVAL_USART, "------ DMACA transfer: HSBSRAM0 -> CPUSRAM ------\r\n");
	print(DMACA_RAM2RAM_EVAL_USART, "       - input of 256 32bit words in HSBSRAM0\r\n");
	print(DMACA_RAM2RAM_EVAL_USART, "       - output of 256 32bit words in CPUSRAM\r\n");
	print(DMACA_RAM2RAM_EVAL_USART, "---------------------------------------------------\r\n");

	// Perform the RAM to RAM transfer
	test_ram_to_ram(DMACA_RAM2RAM_EVAL_BUF_SIZE, source_data_hsb_sram_ptr, dest_data_cpu_sram);

	// End of test: go to sleep.
	SLEEP(AVR32_PM_SMODE_STATIC);
	while (true);
}

static void test_ram_to_ram(uint16_t buffer_length, volatile uint32_t *source_data, volatile uint32_t *dest_data)
{
	// Clear the destination buffer before transferring
	memset((void *)dest_data, 0, DMACA_RAM2RAM_EVAL_BUF_SIZE * sizeof(uint32_t));

	// Enable the DMACA
	AVR32_DMACA.dmacfgreg = (1 << AVR32_DMACA_DMACFGREG_DMA_EN_OFFSET);

	//*
	//* Configure the DMA RAM -> RAM channel.
	//*
	// ------------+--------+--------+------------+--------+-------+----------------
	//  Transfer   | Source | Dest   | Flow       | Width  | Chunk | Buffer
	//  type       |        |        | controller | (bits) | size  | Size
	// ------------+--------+--------+------------+--------+-------+----------------
	//             |        |        |            |        |       |
	//  Mem-to-Mem | RamMem | RamMem |   DMACA    |   32   |   4   | buffer_length
	//             |        |        |            |        |       |
	// ------------+--------+--------+------------+--------+-------+----------------
	// NOTE: We'll use the DMACA channel 2.

	// Src Address: the source_data address
	AVR32_DMACA.sar2 = (uint32_t)source_data;

	// Dst Address: the dest_data address
	AVR32_DMACA.dar2 = (uint32_t)dest_data;

	// Linked list ptrs: not used.
	AVR32_DMACA.llp2 = 0x00000000;

	// Channel 2 Ctrl register low
	AVR32_DMACA.ctl2l =
		(1 << AVR32_DMACA_CTL2L_INT_EN_OFFSET)       |  // Enable interrupts
		(2 << AVR32_DMACA_CTL2L_DST_TR_WIDTH_OFFSET) |  // Dst transfer width: 32bit
		(2 << AVR32_DMACA_CTL2L_SRC_TR_WIDTH_OFFSET) |  // Src transfer width: 32bit
		(0 << AVR32_DMACA_CTL2L_DINC_OFFSET)         |  // Dst address increment: increment
		(0 << AVR32_DMACA_CTL2L_SINC_OFFSET)         |  // Src address increment: increment
		(1 << AVR32_DMACA_CTL2L_DST_MSIZE_OFFSET)    |  // Dst burst transaction len: 4 data items (each of size DST_TR_WIDTH)
		(1 << AVR32_DMACA_CTL2L_SRC_MSIZE_OFFSET)    |  // Src burst transaction len: 4 data items (each of size DST_TR_WIDTH)
		(0 << AVR32_DMACA_CTL2L_TT_FC_OFFSET)        |  // transfer type:M2M, flow controller: DMACA
		(1 << AVR32_DMACA_CTL2L_DMS_OFFSET)          |  // Destination master: HSB master 2
		(0 << AVR32_DMACA_CTL2L_SMS_OFFSET)          |  // Source master: HSB master 1
		(0 << AVR32_DMACA_CTL2L_LLP_D_EN_OFFSET)     |  // Not used
		(0 << AVR32_DMACA_CTL2L_LLP_S_EN_OFFSET);       // Not used

	// Channel 2 Ctrl register high
	AVR32_DMACA.ctl2h =
		(buffer_length << AVR32_DMACA_CTL2H_BLOCK_TS_OFFSET) | // Block transfer size
		(0             << AVR32_DMACA_CTL2H_DONE_OFFSET);      // Not done

	// Channel 2 Config register low
	AVR32_DMACA.cfg2l =
		(0 << AVR32_DMACA_CFG2L_HS_SEL_DST_OFFSET) | // Destination handshaking: ignored because the dst is memory
		(0 << AVR32_DMACA_CFG2L_HS_SEL_SRC_OFFSET);  // Source handshaking: ignored because the src is memory.

	// Channel 2 Config register high
	AVR32_DMACA.cfg2h =
		(0 << AVR32_DMACA_CFG2H_DEST_PER_OFFSET) | // Dest hw handshaking itf: ignored because the dst is memory.
		(0 << AVR32_DMACA_CFG2H_SRC_PER_OFFSET);   // Source hw handshaking itf: ignored because the src is memory.

	// Unmask the DMA Transfer Complete interrupt and the DMA Error for channel 2.
#if DMACA_RAM2RAM_EVAL_TEST_INTERRUPTS
	AVR32_DMACA.masktfr =
		(1 << AVR32_DMACA_MASKTFR_INT_M_WE2_OFFSET) | // Set the Write-enable bit
		(1 << AVR32_DMACA_MASKTFR_INT_MASK2_OFFSET);   // Unmask the Transfer Complete interrupt.
	AVR32_DMACA.maskerr =
		(1 << AVR32_DMACA_MASKERR_INT_M_WE2_OFFSET) | // Set the Write-enable bit
		(1 << AVR32_DMACA_MASKERR_INT_MASK2_OFFSET);      // Unmask the Error interrupt.
#endif

	// Fetch the starting cycle count
	start_cycle_count = Get_system_register(AVR32_COUNT);

	// Enable Channel 2 : start the process.
	AVR32_DMACA.chenreg = ((4 << AVR32_DMACA_CHENREG_CH_EN_OFFSET) | (4 << AVR32_DMACA_CHENREG_CH_EN_WE_OFFSET));

	// Wait for the end of the RAM->RAM transfer (channel 2).
	while (AVR32_DMACA.chenreg & (4 << AVR32_DMACA_CHENREG_CH_EN_OFFSET));

	// Fetch the ending cycle count
	end_cycle_count = Get_system_register(AVR32_COUNT);

	// Compare the source and destination memory buffers for equality
	if (memcmp((void *)source_data, (void *)dest_data, DMACA_RAM2RAM_EVAL_BUF_SIZE * sizeof(uint32_t)) != 0) {
		print(DMACA_RAM2RAM_EVAL_USART, "Fail!\r\n");
	}
	else {
		print(DMACA_RAM2RAM_EVAL_USART, "OK! Elapsed cycles: ");
		print_ulong(DMACA_RAM2RAM_EVAL_USART, end_cycle_count - start_cycle_count);
	}
}

/*! \brief Initializes the HSB bus matrix.
 */
static void init_hmatrix(void)
{
	// Set flashc master type to last default to save one cycle for each branch.
	avr32_hmatrix_scfg_t scfg;

	scfg = AVR32_HMATRIX.SCFG[AVR32_HMATRIX_SLAVE_FLASH];
	scfg.defmstr_type = AVR32_HMATRIX_DEFMSTR_TYPE_LAST_DEFAULT;
	AVR32_HMATRIX.SCFG[AVR32_HMATRIX_SLAVE_FLASH] = scfg;

	scfg = AVR32_HMATRIX.SCFG[AVR32_HMATRIX_SLAVE_SRAM];
	scfg.defmstr_type = AVR32_HMATRIX_DEFMSTR_TYPE_LAST_DEFAULT;
	AVR32_HMATRIX.SCFG[AVR32_HMATRIX_SLAVE_SRAM] = scfg;

	scfg = AVR32_HMATRIX.SCFG[AVR32_HMATRIX_SLAVE_EMBEDDED_SYS_SRAM_0];
	scfg.defmstr_type = AVR32_HMATRIX_DEFMSTR_TYPE_LAST_DEFAULT;
	AVR32_HMATRIX.SCFG[AVR32_HMATRIX_SLAVE_EMBEDDED_SYS_SRAM_0] = scfg;

	scfg = AVR32_HMATRIX.SCFG[AVR32_HMATRIX_SLAVE_EMBEDDED_SYS_SRAM_1];
	scfg.defmstr_type = AVR32_HMATRIX_DEFMSTR_TYPE_LAST_DEFAULT;
	AVR32_HMATRIX.SCFG[AVR32_HMATRIX_SLAVE_EMBEDDED_SYS_SRAM_1] = scfg;
}
