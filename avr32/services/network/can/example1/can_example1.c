/*****************************************************************************
 *
 * \file
 *
 * \brief CAN Software Stack Example 1.
 *
 * Copyright (c) 2006-2015 Atmel Corporation. All rights reserved.
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
 ******************************************************************************/

/*! \mainpage
 * \section intro Introduction
 * This documents data structures, functions, variables, defines, enums, and
 * typedefs in the software for the CAN driver.
 *
 * The given example is a CAN driver example using two CAN channels available.
 * A mailbox is defined by the user and all messages are sent easily.
 *
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC for AVR32 and IAR Systems compiler
 * for AVR32. Other compilers may or may not work.
 *
 * \section files Main Files
 * - canif.c: CAN low level driver.
 * - canif.h: CAN low level header file.
 * - can.c: CAN High level driver.
 * - can.h: CAN High level header file.
 * - conf_can.h: Configuration file for the CAN Lib.
 * - can_example1.c: CAN Driver example.
 * - conf_can_example.h: Configuration file for the CAN example.
 *
 * \section configinfo Configuration Information
 * This example has been tested with the following configuration:
 * - UC3C_EK evaluation kit;
 * - CPU clock: 16 MHz;
 * This example demonstrates that the application can be waked-up from the CAN.
 * - Plug a DB9 cable on CAN1 DB9 connector and an external CAN node to setup a 
 * 'point to point' network.
 * - Configure the baudrate to 1Mbps.
 * - Setup the external CAN node to transmit message.
 * - Start the example.
 * - The CPU is in sleep mode (STATIC) at the beginning of the application and 
 * then is waked up after the reception
 *   of the message from the external CAN node
 * - Led0 is ON once a wakeup frame is received and Led1 is ON once the message 
 * is acknowledged.
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/products/AVR32/">Atmel AVR32</A>.\n
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include "asf.h"
#include "compiler.h"
#include "board.h"
#include "power_clocks_lib.h"
#include "gpio.h"
#include "pm_uc3c.h"
#include "scif_uc3c.h"
#include "intc.h"
#include "can.h"
#include "canif.h"
#include "usart.h"
#include "print_funcs.h"
#include "conf_can_example.h"

/** CAN Wakeup Mask example */
#define CAN_WAKEUP_MASK_EXAMPLE         AVR32_PM_AWEN_CANIF0WEN_MASK

/**  CAN Example Channel Used: 0 means CAN Channel 0 */
#define CAN_CHANNEL_EXAMPLE             0

/** Local allocation for MOB buffer */
volatile can_msg_t mob_ram_ch0[NB_MOB_CHANNEL];

/** Define the number of message received on CAN Channel 0 */
volatile U8 nb_message_received_on_channel0 = 0;

/** Local function to prepare RX and TX buffers */
void can_example_prepare_data_to_send(U8 mode);
/** Call Back prototype */
void can_out_callback_channel0(U8 handle, U8 event);

//! Call Back called by can_drv
void can_out_callback_channel0(U8 handle, U8 event)
{
	/* Reception of Wakeup frame */
	if (handle == NO_MOB) {
		/* Disable Wake-Up Mode */
		CANIF_disable_wakeup(CAN_CHANNEL_EXAMPLE);
		/* Clear Interrupt Flag */
		CANIF_clr_interrupt_status(CAN_CHANNEL_EXAMPLE);
		gpio_clr_gpio_pin(LED0_GPIO);
	}
	else {
		/* Reception of Data frame */
		appli_rx_msg.can_msg->data.u64 = 
			can_get_mob_data(CAN_CHANNEL_EXAMPLE,handle).u64;
		appli_rx_msg.can_msg->id = can_get_mob_id(CAN_CHANNEL_EXAMPLE,handle);
		appli_rx_msg.dlc = can_get_mob_dlc(CAN_CHANNEL_EXAMPLE,handle);
		appli_rx_msg.status = event;
		can_mob_free(CAN_CHANNEL_EXAMPLE,handle);
		nb_message_received_on_channel0 = 1;
		gpio_clr_gpio_pin(LED1_GPIO);
	}
}

/** Main function to configure the CAN, and begin transfers. */
int main(void)
{
	/* Initialize the system clocks */
	sysclk_init();

	/* Setup the generic clock for CAN */
	scif_gc_setup(AVR32_SCIF_GCLK_CANIF,
				SCIF_GCCTRL_OSC0,
				AVR32_SCIF_GC_NO_DIV_CLOCK,
				0);
	/* Now enable the generic clock */
	scif_gc_enable(AVR32_SCIF_GCLK_CANIF);

	init_dbg_rs232(FPBA_HZ);

	/* Disable all interrupts. */
	Disable_global_interrupt();

	/* Initialize interrupt vectors. */
	INTC_init_interrupts();

	static const gpio_map_t CAN_GPIO_MAP = {
		{AVR32_CANIF_RXLINE_0_0_PIN, AVR32_CANIF_RXLINE_0_0_FUNCTION},
		{AVR32_CANIF_TXLINE_0_0_PIN, AVR32_CANIF_TXLINE_0_0_FUNCTION}
	};
	/* Assign GPIO to CAN. */
	gpio_enable_module(CAN_GPIO_MAP,
		sizeof(CAN_GPIO_MAP) / sizeof(CAN_GPIO_MAP[0]));

	/* Initialize channel 0 */
	can_init(CAN_CHANNEL_EXAMPLE, ((uint32_t)&mob_ram_ch0[0]), 
		CANIF_CHANNEL_MODE_LISTENING, can_out_callback_channel0);

	/* Enable all interrupts. */
	Enable_global_interrupt();

	print_dbg("\r\nUC3C CAN Examples 1\r\n");

	print_dbg(CAN_Wakeup);

	/* Initialize CAN Channel 0 */
	can_init(CAN_CHANNEL_EXAMPLE, ((uint32_t)&mob_ram_ch0[0]), 
		CANIF_CHANNEL_MODE_NORMAL, can_out_callback_channel0);

	/* Allocate one mob for RX */
	appli_rx_msg.handle = can_mob_alloc(CAN_CHANNEL_EXAMPLE);

	/* Initialize RX message */
	can_rx(CAN_CHANNEL_EXAMPLE, appli_rx_msg.handle, appli_rx_msg.req_type,
		appli_rx_msg.can_msg);

	/* Enable Async Wake Up Mode */
	pm_asyn_wake_up_enable(CAN_WAKEUP_MASK_EXAMPLE);

	/* ---------SLEEP MODE PROCEDURE------------- */
	/* Disable CAN Channel 0 */
	CANIF_disable(CAN_CHANNEL_EXAMPLE);
	/* Wait CAN Channel 0 is disabled */
	while(!CANIF_channel_enable_status(CAN_CHANNEL_EXAMPLE));
	/* Enable Wake-Up Mode */
	CANIF_enable_wakeup(CAN_CHANNEL_EXAMPLE);
	/* Go to sleep mode. */
	SLEEP(AVR32_PM_SMODE_STATIC);
	/* ---------SLEEP MODE PROCEDURE------------- */
	print_dbg(CAN_WakeupD);
	/* Initialize again CAN Channel 0 */
	can_init(CAN_CHANNEL_EXAMPLE, ((uint32_t)&mob_ram_ch0[0]),
		CANIF_CHANNEL_MODE_NORMAL, can_out_callback_channel0);

	/* Allocate one mob for RX */
	appli_rx_msg.handle = can_mob_alloc(CAN_CHANNEL_EXAMPLE);

	/* Initialize RX message */
	can_rx(CAN_CHANNEL_EXAMPLE, appli_rx_msg.handle, appli_rx_msg.req_type,
		appli_rx_msg.can_msg);

	for (;;) {
		/* Do nothing; interrupts handle the DAC conversions */
	}

}
