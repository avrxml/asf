/*****************************************************************************
 *
 * \file
 *
 * \brief PEVC example 3 application for AVR32 using PEVC IO as generator
 *      and PDCA as event user.
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
 *
 * The given example covers a use of PEVC controller and demonstrates how to
 *  use it.
 * In the present example a PEVC Input generate event. This one is transmitted
 * to the PDCA. Each time a new event is coming, a character is sent to the
 * USART without use of the CPU. The main loop of the function is a 500ms delay and
 * toggle LED 1 continuously to show CPU activity.
 * Press one of the touch button present on the UC3C_EK to generate an event.
 *
 * \section files Main Files
 * - pevc.c: PEVC driver;
 * - pevc.h: PEVC driver header file;
 * - pevc_example3.c: PEVC example application.
 *
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC for AVR32 and IAR Systems compiler
 * for AVR32. Other compilers may or may not work.
 *
 * \section deviceinfo Device Info
 * All AVR32 devices with a PEVC module can be used. This example has been tested
 * with the following setup:
 *   - UC3C_EK evaluation kit;
 *
 * \section setupinfo Setup Information
 * - CPU speed for UC3C : <i> 60 MHz from 12-MHz Osc0 crystal </i>
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/products/AVR32/">Atmel AVR32</A>.\n
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


// Include Files
#include <avr32/io.h>
#include <stdio.h>
#include "compiler.h"
#include "print_funcs.h"
#include "board.h"
#include "power_clocks_lib.h"
#include "twim.h"
#include "gpio.h"
#include "pdca.h"
#include "cycle_counter.h"
#include "flashc.h"
#include "at42qt1060.h"
#include "conf_at42qt1060.h"
#include "pevc.h"

#define FCPU_HZ		60000000
#define FPBA_HZ		60000000

//##
//## Size of the string to transfer to USART0 through the PDCA
//##
#define STRING_TRANSFER_SIZE  36

//! The PDCA channel instance for the USART0 Tx
// NOTE: the PEVC-PDCA links are only available for PDCA channel 0 & 1.
// => Test with both channels.
#define PDCA_CHANNEL_USART      0
#define PDCA_CHANNEL_IRQ        AVR32_PDCA_IRQ_0
#define PEVC_PDCA_SOT_USER      AVR32_PEVC_ID_USER_PDCA_0

volatile bool touch_detect = false;
volatile avr32_pevc_t* ppevc = &AVR32_PEVC;
volatile avr32_pdca_channel_t *pdca_channel;

static volatile U32 u32PdcaIsr;

/*! \brief Initializes the MCU system clocks.
*/
static void init_sys_clocks(void)
{
	scif_osc32_opt_t opt =
	{
		.mode = SCIF_OSC_MODE_2PIN_CRYSTAL,
		.startup = AVR32_SCIF_OSCCTRL32_STARTUP_0_RCOSC
	};

	/*! \name System Clock Frequencies
	*/
	//! @{
	static pcl_freq_param_t pcl_freq_param =
	{
		.cpu_f        = FCPU_HZ,
		.pba_f        = FPBA_HZ,
		.osc0_f       = FOSC0,
		.osc0_startup = OSC0_STARTUP
	};
	//! @}

	// Configure system clocks.
	if (pcl_configure_clocks(&pcl_freq_param) != PASS) {
		while(1);
	}

	scif_start_osc32(&opt,true);
}

// String to transfer to USART0 through the PDCA.
unsigned char aDataTransfered[STRING_TRANSFER_SIZE];
/**
 ** PDCA Interrupts handler.
 **/
ISR(pdca_int_handler,AVR32_PDCA_IRQ_GROUP0,0)
{
	u32PdcaIsr = pdca_channel->isr;
	if( u32PdcaIsr & (1<<AVR32_PDCA_ISR0_TRC_OFFSET) )
	{
		// Count the number of Transfer Complete interrupts.
		pdca_reload_channel(PDCA_CHANNEL_USART, (void *)aDataTransfered,
			sizeof( aDataTransfered ));
		print_dbg("\r\n");
	}
}

/*! \brief Initializes the TWI for AT42QT1060.
*/
static void twi_init(void)
{
	const gpio_map_t AT42QT1060_TWI_GPIO_MAP = {
		{AT42QT1060_TWI_SCL_PIN, AT42QT1060_TWI_SCL_FUNCTION},
		{AT42QT1060_TWI_SDA_PIN, AT42QT1060_TWI_SDA_FUNCTION}
	};

	const twi_options_t AT42QT1060_TWI_OPTIONS = {
		.pba_hz = FPBA_HZ,
		.speed = AT42QT1060_TWI_MASTER_SPEED,
		.chip = AT42QT1060_TWI_ADDRESS
	};

	// Assign I/Os to SPI.
	gpio_enable_module(AT42QT1060_TWI_GPIO_MAP,
	sizeof(AT42QT1060_TWI_GPIO_MAP) / sizeof(AT42QT1060_TWI_GPIO_MAP[0]));
	// Initialize as master.
	twi_master_init(AT42QT1060_TWI, &AT42QT1060_TWI_OPTIONS);
}

/**
 ** PEVC overrun and trigger Interrupt handler.
 **/
ISR(pevc_int_handler,AVR32_PEVC_IRQ_GROUP,0)
{
int32_t i=0;
	for(i=0;i<PEVC_NUMBER_OF_EVENT_USERS;i++) {
		if(true == pevc_channel_is_trigger_interrupt_raised(ppevc, i)) {
		// An overrun on the channel Id=i occurred.
		// Clear the interrupt
		pevc_channel_clear_trigger_interrupt(ppevc, i);
		touch_detect=true;
		gpio_tgl_gpio_pin(LED0_GPIO);
		}
		if(true == pevc_channel_is_overrun_interrupt_raised(ppevc, i)) {
			// An overrun on the channel Id=i occurred.
			// Clear the interrupt
			pevc_channel_clear_overrun_interrupt(ppevc, i);
			touch_detect=true;
			gpio_tgl_gpio_pin(LED1_GPIO);
		}
	}
}

/*! \brief Init PEVC
 * Toggle LED when the Busy pin change
*/
void init_pevc(void)
{

	// PEVC Event Shaper options.
	static const pevc_evs_opt_t PEVC_EVS_OPTIONS =
	{
		.igfdr = 0x0A,            // Set the IGF clock to be clk_slow*2pow11
		.igf = PEVC_EVS_IGF_OFF,  // Input Glitch Filter off
		.evf = PEVC_EVS_EVF_ON,   // Enable Event on falling edge
		.evr = PEVC_EVS_EVR_ON    // Enable Event on rising edge
	};

	// Need to Setup GPIO ahs PEVC PAD EVT
	static const gpio_map_t PEVC_GPIO_MAP =
	{
	{AVR32_PEVC_PAD_EVT_0_0_PIN , AVR32_PEVC_PAD_EVT_0_0_FUNCTION},
	{AVR32_PEVC_PAD_EVT_1_0_PIN , AVR32_PEVC_PAD_EVT_1_0_FUNCTION},
	{AVR32_PEVC_PAD_EVT_2_0_PIN , AVR32_PEVC_PAD_EVT_2_0_FUNCTION},
	{AVR32_PEVC_PAD_EVT_3_0_PIN , AVR32_PEVC_PAD_EVT_3_0_FUNCTION},
	{AVR32_PEVC_PAD_EVT_4_0_PIN , AVR32_PEVC_PAD_EVT_4_0_FUNCTION},
	{AVR32_PEVC_PAD_EVT_5_0_PIN , AVR32_PEVC_PAD_EVT_5_0_FUNCTION},
	{AVR32_PEVC_PAD_EVT_6_0_PIN , AVR32_PEVC_PAD_EVT_6_0_FUNCTION},
	{AVR32_PEVC_PAD_EVT_7_0_PIN , AVR32_PEVC_PAD_EVT_7_0_FUNCTION},
	{AVR32_PEVC_PAD_EVT_8_0_PIN , AVR32_PEVC_PAD_EVT_8_0_FUNCTION},
	{AVR32_PEVC_PAD_EVT_9_0_PIN , AVR32_PEVC_PAD_EVT_9_0_FUNCTION},
	{AVR32_PEVC_PAD_EVT_10_0_PIN, AVR32_PEVC_PAD_EVT_10_0_FUNCTION},
	{AVR32_PEVC_PAD_EVT_11_0_PIN, AVR32_PEVC_PAD_EVT_11_0_FUNCTION},
	{AVR32_PEVC_PAD_EVT_12_0_PIN, AVR32_PEVC_PAD_EVT_12_0_FUNCTION},
	{AVR32_PEVC_PAD_EVT_13_0_PIN, AVR32_PEVC_PAD_EVT_13_0_FUNCTION},
	{AVR32_PEVC_PAD_EVT_14_0_PIN, AVR32_PEVC_PAD_EVT_14_0_FUNCTION},
	{AVR32_PEVC_PAD_EVT_15_0_PIN, AVR32_PEVC_PAD_EVT_15_0_FUNCTION}
	};

	// Assign GPIO pins to USART0.
	gpio_enable_module(PEVC_GPIO_MAP,
		sizeof(PEVC_GPIO_MAP) / sizeof(PEVC_GPIO_MAP[0]));

	// register Register interrupts
	irqflags_t flags = cpu_irq_save();
	irq_register_handler(pevc_int_handler,
			AVR32_PEVC_TRIGGER_IRQ, 0);
	irq_register_handler(pevc_int_handler,
			AVR32_PEVC_OVERRUN_IRQ, 0);
	cpu_irq_restore(flags);

  // Assign PEVC PAD 2 to PDCA
	if(FAIL == pevc_channel_configure(ppevc, PEVC_PDCA_SOT_USER,
			AVR32_PEVC_ID_GEN_PAD_2,
			&PEVC_EVS_OPTIONS))
	{
	  print_dbg("PEVC channel config failed!!!\r\n");
	  gpio_clr_gpio_pin(LED1_GPIO);
	  while(1);
	}

	// Enable the PEVC trigger interrupt.
	pevc_channels_enable_trigger_interrupt(ppevc, 1<<AVR32_PEVC_ID_USER_PDCA_0);
	pevc_channels_enable_overrun_interrupt(ppevc, 1<<AVR32_PEVC_ID_USER_PDCA_0);

	// Enable the PEVC channel "PDCA CHANNEL 0/1 ONE-ITEM-TRANSFER"
	PEVC_CHANNELS_ENABLE(ppevc, 1<<PEVC_PDCA_SOT_USER);
}

void init_pdca(void)
{
	// PDCA channel 0/1 options
	static const pdca_channel_options_t PDCA_CH_OPTIONS =
	{
		.addr = (void *)aDataTransfered,          // memory address
		.pid = AVR32_PDCA_PID_USART2_TX,          // select peripheral
		.size = 0,                                // transfer counter
		.r_addr = (void *)aDataTransfered,        // next memory address
		.r_size = sizeof(aDataTransfered),        // next transfer counter
		.transfer_size = PDCA_TRANSFER_SIZE_BYTE, // select size of one data packet
		.etrig = true                             // Trigger transfer on event.
	};
	// register Register interrupts
	irqflags_t flags = cpu_irq_save();
	irq_register_handler(pdca_int_handler,
			PDCA_CHANNEL_IRQ, 0);
	cpu_irq_restore(flags);

	//##
	//## PDCA Init.
	//##
	// Init PDCA channel with the pdca_options.
	pdca_init_channel(PDCA_CHANNEL_USART, &PDCA_CH_OPTIONS);
	pdca_channel = pdca_get_handler(PDCA_CHANNEL_USART); // For use in the pdca interrupt handler.

	// Enable pdca transfer error interrupt & transfer complete interrupt.
	pdca_enable_interrupt_transfer_error(PDCA_CHANNEL_USART);
	pdca_enable_interrupt_transfer_complete(PDCA_CHANNEL_USART);

	// Enable the PDCA.
	pdca_enable(PDCA_CHANNEL_USART);
}

// main function
int main(void)
{
int i;
t_cpu_time timeout;

	init_sys_clocks();
	init_dbg_rs232(FPBA_HZ);
	print_dbg("\x0CPEVC Driver - EXAMPLE 3\r\n");
	print_dbg("AVR32 UC3 - PEVC Software event example\r\n");

	// Initialize and enable interrupt
	irq_initialize_vectors();
	cpu_irq_enable();

	// Init the string with a simple recognizable pattern.
	for(i=0;i<sizeof(aDataTransfered);i++)
		aDataTransfered[i] = '0' + (i%36);

	// Activate LED0 & LED1 & LED2 & LED3 pins in GPIO output mode and switch them off.
	gpio_set_gpio_pin(LED0_GPIO);
	gpio_set_gpio_pin(LED1_GPIO);
	gpio_set_gpio_pin(LED2_GPIO);
	gpio_set_gpio_pin(LED3_GPIO);
	twi_init();


	/* Power up delay: This device needs a maximum of 230ms before it
	* can be accessed. Another method would be to wait until the the
	* detect line goes low but this won't work during a debugging session
	* since the device is not reset.*/
	cpu_delay_ms(230, FCPU_HZ);

	at42qt1060_init(FCPU_HZ);

	cpu_set_timeout( cpu_ms_2_cy(1000, FCPU_HZ), &timeout);

	init_pevc();

	init_pdca();

	while(true)
	{
		// if a touch is detected we read the status
		if(touch_detect)
		{
			touch_detect = false;
			at42qt1060_read_reg(AT42QT1060_DETECTION_STATUS);
			at42qt1060_read_reg(AT42QT1060_INPUT_PORT_STATUS);
		}
		cpu_delay_ms(500, FCPU_HZ);
		gpio_tgl_gpio_pin(LED3_GPIO);
	}

	return 0;
}
