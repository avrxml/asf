/*****************************************************************************
 *
 * \file
 *
 * \brief CAN Software Stack Example 2.
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
 * - can_example2.c: CAN Driver example.
 * - conf_can_example.h: Configuration file for the CAN example.
 *
 * \section configinfo Configuration Information
 * This example has been tested with the following configuration:
 * - UC3C_EK evaluation kit;
 * - CPU clock: 16 MHz;
 * This example demonstrates that how to send/receive messages from the CAN.
 * - Plug a DB9 cable on CAN1 DB9 connector and an external CAN node to setup a 
 * 'point to point' network.
 * - Configure the baudrate to 1Mbps.
 * - Start the example.
 * - A menu is displayed
 *      -- 1) If choose 1), the UC3C_EK will send a message on CAN node. 
 *      In case the message is correctly sent, a message is displayed.
 *      -- 2) If choose 2), the UC3C_EK will receive a message on CAN node. 
 *      In case the message is correctly received, a message is displayed with 
 *      the content of the FRAME.
 *      -- 3) If choose 3), the UC3C_EK will send a remote message on CAN node. 
 *      In case the message is correctly sent, a message is displayed.
 * - The CPU is in sleep mode (STATIC) at the beginning of the application and 
 * then is waked up after the reception of the message from the external CAN 
 * node
 *
 *
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

/* Local allocation for MOB buffer in HSB_RAM */
#if defined (__GNUC__) && defined (__AVR32__)
volatile can_msg_t mob_ram_ch0[NB_MOB_CHANNEL] __attribute__ ((section (".hsb_ram_loc")));
#elif defined (__ICCAVR32__)
volatile __no_init can_msg_t mob_ram_ch0[NB_MOB_CHANNEL] @0xA0000000;
#endif

/** Local function to prepare RX and TX buffers */ 
void can_example_prepare_data(U8 mode);

volatile U16 state_machine = 0;


/** USART interrupt handler, fired each time a character is received. */
#if defined (__GNUC__)
__attribute__((__interrupt__))
#elif defined (__ICCAVR32__)
__interrupt
#endif
static void usart_int_handler(void)
{
	int32_t c;
	usart_read_char(UART_USART, &c);
	switch( c ) {
		case '1':
			if( state_machine == 1)  state_machine = 10;
		break;
		case '2':
			if( state_machine == 1) state_machine = 20;
		break;
		case '3':
			if( state_machine == 1) state_machine = 30;
		break;
		case 'Y':
		case 'y':
			if( state_machine == 11) state_machine = 12;
			if( state_machine == 21) state_machine = 22;
			if( state_machine == 31) state_machine = 32;
		break;
		case 'N':
		case 'n':
			if( state_machine == 11 ) state_machine = 0;
			if( state_machine == 21) state_machine  = 0;
			if( state_machine == 31) state_machine  = 0;
		break;
		case 'Q':
		case 'q':
			if( state_machine == 121) state_machine = 0;
			if( state_machine == 131) state_machine = 0;
			if( state_machine == 221) state_machine = 0;
			if( state_machine == 220 ) state_machine = 0;
			if( state_machine == 331) state_machine = 0;
		break;
	}
}
/** Define the number of message received on CAN Channel 0 */
volatile U8 nb_message_received_on_channel0 = 0;
/** Call Back prototype */
void can_out_callback_channel0(U8 handle, U8 event);
/** Call Back called by can_drv */
void can_out_callback_channel0(U8 handle, U8 event)
{
	gpio_tgl_gpio_pin(LED0_GPIO);
	switch(state_machine)
	{
		/* Transmission Only */
		case 120:
			appli_tx_msg.status = event;
			can_mob_free(0,handle);
			state_machine = 121;
		break;
		/* Reception Only */
		case 220:
			appli_rx_msg.can_msg->data.u64 = can_get_mob_data(0,handle).u64;
			appli_rx_msg.can_msg->id = can_get_mob_id(0,handle);
			appli_rx_msg.dlc = can_get_mob_dlc(0,handle);
			appli_rx_msg.status = event;
			can_mob_free(0,handle);
			nb_message_received_on_channel0 = 1;
		break;
		/* Remote Reception Only */
		case 320:
			appli_remote_rx_msg.status = event;
			can_mob_free(0,handle);
			state_machine = 321;
		break;
	}
}


/** Prepare Data on CAN Bus. */
void can_example_prepare_data(U8 mode)
{
	/* Prepare Transmission Mode */
	if (mode ==0) {
		/* Initialize channel 0 */
		can_init(0, ((uint32_t)&mob_ram_ch0[0]), CANIF_CHANNEL_MODE_NORMAL,
			can_out_callback_channel0);
		/* Allocate one mob for TX */
		appli_tx_msg.handle = can_mob_alloc(0);

		/* Check return if no mob are available */
		if (appli_tx_msg.handle==CAN_CMD_REFUSED) {
			while(1);
		}

		can_tx(0, appli_tx_msg.handle, appli_tx_msg.dlc, appli_tx_msg.req_type,
			appli_tx_msg.can_msg);
	}
	/* Prepare Reception Mode */
	else if (mode == 1) {
		can_init(0, ((uint32_t)&mob_ram_ch0[0]), CANIF_CHANNEL_MODE_NORMAL,
			can_out_callback_channel0);

		/* Allocate one mob for RX */
		appli_rx_msg.handle = can_mob_alloc(0);

		can_rx(0, appli_rx_msg.handle, appli_rx_msg.req_type, 
			appli_rx_msg.can_msg);
	}
	/* Prepare Remote Reception Mode */
	else if (mode == 2) {
		can_init(0, ((uint32_t)&mob_ram_ch0[0]), CANIF_CHANNEL_MODE_NORMAL,
			can_out_callback_channel0);

		/* Allocate one mob for RX */
		appli_remote_rx_msg.handle = can_mob_alloc(0);

		can_rx(0, appli_remote_rx_msg.handle, appli_remote_rx_msg.req_type,
			appli_remote_rx_msg.can_msg);
	}
}

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

	static const gpio_map_t CAN_GPIO_MAP = {
		{AVR32_CANIF_RXLINE_0_0_PIN, AVR32_CANIF_RXLINE_0_0_FUNCTION},
		{AVR32_CANIF_TXLINE_0_0_PIN, AVR32_CANIF_TXLINE_0_0_FUNCTION}
	};
	/* Assign GPIO to CAN. */
	gpio_enable_module(CAN_GPIO_MAP,
		sizeof(CAN_GPIO_MAP) / sizeof(CAN_GPIO_MAP[0]));

	init_dbg_rs232(FPBA_HZ);

	/* Disable all interrupts. */
	Disable_global_interrupt();

	/* Initialize interrupt vectors. */
	INTC_init_interrupts();

	/* Register the USART interrupt handler to the interrupt controller.
	* usart_int_handler is the interrupt handler to register.
	* EXAMPLE_USART_IRQ is the IRQ of the interrupt handler to register.
	* AVR32_INTC_INT0 is the interrupt priority level to assign to the group of
	* this IRQ.
	* void INTC_register_interrupt(__int_handler handler, unsigned int irq, 
	* unsigned int int_level);
	*/
	INTC_register_interrupt(&usart_int_handler, UART_USART_IRQ, 
		UART_USART_IRQ_LEVEL);

	/*  Enable USART Rx interrupt. */
	UART_USART->ier = AVR32_USART_IER_RXRDY_MASK;

	/* Initialize channel 0 */
	can_init(0, ((uint32_t)&mob_ram_ch0[0]), CANIF_CHANNEL_MODE_LISTENING,
		can_out_callback_channel0);

	/* Enable all interrupts. */
	Enable_global_interrupt();

	while(1) {

 /*---                 C O R E   O F   C A N - DEMO                        ---*/

		switch( state_machine )
		{
			case 0:
				state_machine = 1;
				print_dbg(txt_logo_atmel);
			break;
			case 1:
				/* wait input (1,2,3 choice) */
			break;

/*---                 Case 1 : Transmission                  ---*/
			case 10:
				/*  "1" be pressed */
				state_machine = 11;
				print_dbg( Demo_Sent_Data );
				print_dbg( Data_Sent_Start_or_Not );
			break;
			case 11:
				/* wait input (y,n choice) */
			break;
			case 12:
				/* "y or Y" be pressed, */
				state_machine = 120;
				can_example_prepare_data(0);
			break;
			case 120:
				/* Idle state */
			break;
			case 121:
				/* Print Status */
				if (appli_tx_msg.status == CAN_STATUS_COMPLETED) {
					print_dbg(CAN_Success);
				}
				else if (appli_tx_msg.status == CAN_STATUS_ERROR) {
					print_dbg(CAN_Error);
				}
				else if(appli_tx_msg.status == CAN_STATUS_BUSOFF) {
					print_dbg(CAN_BusOFFIRQ);
				}
				state_machine = 131;
			break;
			case 131:
				/* wait input */
			break;


/*---                 Case 2 : Reception                     --- */
			case 20:
				/* "2" be pressed */
				state_machine = 21;
				print_dbg( Data_Sent_Start_or_Not );
			break;

			case 21:
				/* wait input (y,n choice) */
			break;

			case 22:
				/* "y or Y" be pressed, */
				state_machine = 220;
				can_example_prepare_data(1);
			break;
			case 220:
				/* Idle state */
				if ((nb_message_received_on_channel0 == 1)) {
					nb_message_received_on_channel0 = 0;
					if (appli_rx_msg.status == CAN_STATUS_COMPLETED) {
						print_dbg ("-1- RxCAN @");
						print_dbg_ulong(appli_rx_msg.can_msg->id);
						print_dbg (":");
						print_dbg_char_hex(appli_rx_msg.can_msg->id);
						print_dbg (",");
						print_dbg_char_hex(appli_rx_msg.dlc);
						print_dbg (",");
						uint8_t i;
						for (i=0;i<appli_rx_msg.dlc;i++) {
							print_dbg_char_hex(appli_rx_msg.can_msg->data.u8[i]);
							if (i<appli_rx_msg.dlc-1)
								print_dbg ("-");
						}
						print_dbg("\r\n");
						/* Allocate one mob for RX */
						appli_rx_msg.handle = can_mob_alloc(0);
						can_rx(0, appli_rx_msg.handle, appli_rx_msg.req_type,
							appli_rx_msg.can_msg);
					}
					else if (appli_rx_msg.status == CAN_STATUS_ERROR) {
						state_machine = 221;
						print_dbg(CAN_Error);
					}
					else if(appli_rx_msg.status == CAN_STATUS_BUSOFF) {
						state_machine = 221;
						print_dbg(CAN_BusOFFIRQ);
					}
				}
			break;

			case 221:
				/* wait input */
			break;


/*---                 Case 3 : Remote Reception              ---*/
			case 30:
				/* "3" be pressed */
				state_machine = 31;
				print_dbg( Data_Sent_Start_or_Not );
			break;

			case 31:
				/* wait input (y,n choice) */
			break;

			case 32:
				/* "y or Y" be pressed,*/
				state_machine = 320;
				can_example_prepare_data(2);
			break;

			case 320:
				/* Idle state */
			break;

			case 321:
				/* Print Status */
				if (appli_remote_rx_msg.status == CAN_STATUS_COMPLETED) {
					print_dbg(CAN_Success);
				}
				else if (appli_remote_rx_msg.status == CAN_STATUS_ERROR){
					print_dbg(CAN_Error);
				}
				else if(appli_remote_rx_msg.status == CAN_STATUS_BUSOFF) {
					print_dbg(CAN_BusOFFIRQ);
				}
				state_machine = 331;
			break;

			case 331:
				/* wait input */
			break;

/*---                 Default                                ---*/
			default:
			break;
		}
	}
}
