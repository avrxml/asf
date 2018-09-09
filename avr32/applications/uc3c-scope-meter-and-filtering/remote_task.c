/**
 * \file
 *
 * \brief Remote Task Control.
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */
//_____  I N C L U D E S ___________________________________________________
#include "compiler.h"
#include "dsp.h"
#include "pdca.h"
#include "gpio.h"
#include "usart.h"
#include "print_funcs.h"
#include "remote_task.h"
//_____ M A C R O S ________________________________________________________

//_____ D E F I N I T I O N S ______________________________________________
#define FRAME_PRIVILEGIED_CMD                   0x00
#define FRAME_SEND_SIGNALIN_CMD                 0x10
#define FRAME_SEND_NOISE_CMD                    0x11
#define FRAME_SEND_SIGNALIN_NOISE_CMD           0x12
#define FRAME_SEND_SIGNAL_FILTER_DSP_CMD        0x13
#define FRAME_SEND_SIGNAL_FILTER_FPU_CMD        0x14
//_____ D E C L A R A T I O N S ____________________________________________
//! Reference to Signal In + Noise Signal Buff in floating point Format
//! scaled for the remote task
extern volatile float signalin_noise_remote[GUI_BUFFER_LENGTH];
//! Reference to Filtered Signal through 16-bit fixed point Format
//! scaled for the remote task
extern volatile float signalout_fp_remote[GUI_BUFFER_LENGTH];
//! Reference to Filtered Signal through floating point Format
//! scaled for the remote task.
extern volatile float signalout_fpu_remote[GUI_BUFFER_LENGTH];
// Local Buffer for transmission
volatile uint8_t txbuf[REMOTE_TXBUF_SIZE];
// Local End Of Transfer Flag
volatile bool flag_eot = false;
//! Local definition for Set IRQ function of USART through PDCA
static void pdca_set_irq_usart(void);
//! Local definition to start PDCA transfer function
static void remote_task_pdca_start(void);

/*! \brief The PDCA interrupt handler.
 *
 */
ISR(pdca_int_handler_usart, AVR32_PDCA_IRQ_GROUP2, 1)
{
	flag_eot = true;
	pdca_disable_interrupt_transfer_complete(REMOTE_PDCA_CHANNEL_USART);
}

/*! \brief Init interrupt controller and register pdca_int_handler interrupt.
 */
static void pdca_set_irq_usart(void)
{
	/* register Register pdca_int_handler_usart interrupt on level 1 */
	irqflags_t flags = cpu_irq_save();
	irq_register_handler(pdca_int_handler_usart,
			AVR32_PDCA_IRQ_8, 1);
	cpu_irq_restore(flags);
}

static void remote_task_pdca_start(void)
{
	// PDCA channel options
	static const pdca_channel_options_t REMOTE_PDCA_OPTIONS_USART =
	{
		.addr = (void *)txbuf,                    // memory address
		.pid = AVR32_USART2_PDCA_ID_TX,           // select peripheral
		.size = REMOTE_TXBUF_SIZE,                // transfer counter
		.r_addr = NULL,                           // next memory address
		.r_size = 0,                              // next transfer counter
		.transfer_size = PDCA_TRANSFER_SIZE_BYTE  // select size of the transfer
	};

	// Init PDCA channel with the pdca_options.
	pdca_init_channel(REMOTE_PDCA_CHANNEL_USART, &REMOTE_PDCA_OPTIONS_USART);

	// Register PDCA IRQ interrupt.
	pdca_set_irq_usart();

	// Enable pdca interrupt each time the reload counter reaches zero.
	pdca_enable_interrupt_transfer_complete(REMOTE_PDCA_CHANNEL_USART);

	// Enable now the transfer.
	pdca_enable(REMOTE_PDCA_CHANNEL_USART);

}


/*! \brief Remote Task Init:
 *        - Initialize Usart
 *        - Start PDCA Transfer
 */
void remote_task_init(void)
{
	// Initialize USART
	static const gpio_map_t REMOTE_USART_GPIO_MAP = {
		{REMOTE_USART_RX_PIN, REMOTE_USART_RX_FUNCTION},
		{REMOTE_USART_TX_PIN, REMOTE_USART_TX_FUNCTION}
	};

	// USART options.
	static const usart_options_t REMOTE_USART_OPTIONS = {
		.baudrate     = 115200,
		.charlength   = 8,
		.paritytype   = USART_NO_PARITY,
		.stopbits     = USART_1_STOPBIT,
		.channelmode  = USART_NORMAL_CHMODE
	};
	// Assign GPIO to USART.
	gpio_enable_module(REMOTE_USART_GPIO_MAP,
			sizeof(REMOTE_USART_GPIO_MAP) / sizeof(REMOTE_USART_GPIO_MAP[0]));

	// Initialize USART in RS232 mode.
	usart_init_rs232(REMOTE_USART, &REMOTE_USART_OPTIONS, sysclk_get_pba_hz());

	// Start PDCA Transfer
	remote_task_pdca_start();
}

/*! \brief Remote Task:
 *        - Prepare PDCA Buffer
 *        - Re-Start PDCA Transfer
 */
void remote_task(void)
{
	uint32_t i=0,j=0;
	uint32_t tmp;

	if (flag_eot == true) {
		flag_eot = false;
		// Send Msg Signal In Noise
		txbuf[i++] = FRAME_SEND_SIGNALIN_NOISE_CMD;
		txbuf[i++] = 0x00;
		txbuf[i++] = GUI_BUFFER_LENGTH;
		txbuf[i++] = 0x00;
		for (j=0;j<GUI_BUFFER_LENGTH;j++) {
			// Convert float to uint32_t
			tmp = *((uint32_t *) ((void *) (&signalin_noise_remote[j])));
			txbuf[i++] = (tmp&0x000000FF);
			txbuf[i++] = (tmp&0x0000FF00)>>8;
			txbuf[i++] = (tmp&0x00FF0000)>>16;
			txbuf[i++] = (tmp&0xFF000000)>>24;
		}
		// Send Msg Signal Filtered using 16-bit fixed point format
		txbuf[i++] = FRAME_SEND_SIGNAL_FILTER_DSP_CMD;
		txbuf[i++] = 0x00;
		txbuf[i++] = GUI_BUFFER_LENGTH;
		txbuf[i++] = 0x00;
		for (j=0;j<GUI_BUFFER_LENGTH;j++) {
			// Convert float to uint32_t
			tmp = *((uint32_t *) ((void *) (&signalout_fp_remote[j])));
			txbuf[i++] = (tmp&0x000000FF);
			txbuf[i++] = (tmp&0x0000FF00)>>8;
			txbuf[i++] = (tmp&0x00FF0000)>>16;
			txbuf[i++] = (tmp&0xFF000000)>>24;
		}
		// Send Msg Signal Filtered using floating point format
		txbuf[i++] = FRAME_SEND_SIGNAL_FILTER_FPU_CMD;
		txbuf[i++] = 0x00;
		txbuf[i++] = GUI_BUFFER_LENGTH;
		txbuf[i++] = 0x00;
		for (j=0;j<GUI_BUFFER_LENGTH;j++) {
			// Convert float to uint32_t
			tmp = *((uint32_t *) ((void *) (&signalout_fpu_remote[j])));
			txbuf[i++] = (tmp&0x000000FF);
			txbuf[i++] = (tmp&0x0000FF00)>>8;
			txbuf[i++] = (tmp&0x00FF0000)>>16;
			txbuf[i++] = (tmp&0xFF000000)>>24;
		}
		// Transfer Buffer through PDCA
		remote_task_pdca_start();
	}
}
