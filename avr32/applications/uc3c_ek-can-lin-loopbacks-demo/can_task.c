/**
 * \file
 *
 * \brief Application CAN Task Management.
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
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include <stddef.h>
#include <stdio.h>
#include <avr32/io.h>
#include "compiler.h"
#include "board.h"
#include "power_clocks_lib.h"
#include "gpio.h"
#include "pm_uc3c.h"
#include "scif_uc3c.h"
#include "print_funcs.h"
#include "can_task.h"
#include "conf_can_task.h"
#include "dsp.h"
#include "conf_demo.h"

//! Local Mob Declaration
can_msg_t mob_ram_ch0[NB_MOB_CHANNEL];
can_msg_t mob_ram_ch1[NB_MOB_CHANNEL];

//! Local function to prepare RX and TX buffers
void can_example_prepare_data_to_send(void);
void can_example_prepare_data_to_receive(void);
void can_out_callback_channel0(U8 handle, U8 event);
void can_out_callback_channel1(U8 handle, U8 event);
//! External reference on ADC Current Conversion
extern volatile uint16_t adc_current_conversion;

//! Buffer to store ADC Conversions
A_ALIGNED dsp16_t signal3_buf[BUFFER_LENGTH];

//! Boolean for message transmitted on CAN channel 1
volatile bool message_transmitted_on_channel1 = false;
/*! \brief CAN Call Back when message is transmitted
 *
 */
void can_out_callback_channel1(U8 handle, U8 event)
{
	gpio_tgl_gpio_pin(LED2_GPIO);
	// Transmission Only
	can_mob_free(1,handle);
	message_transmitted_on_channel1 = true;
}

//! Boolean for message transmitted on CAN channel 0
volatile bool message_received_on_channel0 = false;
/*! \brief CAN Call Back when message is received
 */
void can_out_callback_channel0(U8 handle, U8 event)
{
	gpio_tgl_gpio_pin(LED3_GPIO);
	// Reception Only
	pCANMOB_message2[0].can_msg->data.u64 = can_get_mob_data(0,handle).u64;
	pCANMOB_message2[0].can_msg->id = can_get_mob_id(0,handle);
	pCANMOB_message2[0].dlc = can_get_mob_dlc(0,handle);
	pCANMOB_message2[0].status = event;
	can_mob_free(0,handle);
	message_received_on_channel0 = true;
}

void can_task_init(void)
{
	// Setup the generic clock for CAN
	scif_gc_setup(AVR32_SCIF_GCLK_CANIF,
		SCIF_GCCTRL_OSC0,
		AVR32_SCIF_GC_NO_DIV_CLOCK,
		0);
	// Now enable the generic clock
	scif_gc_enable(AVR32_SCIF_GCLK_CANIF);
	static const gpio_map_t CAN_GPIO_MAP = {
		{AVR32_CANIF_RXLINE_0_0_PIN, AVR32_CANIF_RXLINE_0_0_FUNCTION},
		{AVR32_CANIF_TXLINE_0_0_PIN, AVR32_CANIF_TXLINE_0_0_FUNCTION},
		{AVR32_CANIF_RXLINE_1_2_PIN, AVR32_CANIF_RXLINE_1_2_FUNCTION},
		{AVR32_CANIF_TXLINE_1_2_PIN, AVR32_CANIF_TXLINE_1_2_FUNCTION}
	};
	// Assign GPIO to CAN.
	gpio_enable_module(CAN_GPIO_MAP,
		sizeof(CAN_GPIO_MAP) / sizeof(CAN_GPIO_MAP[0]));
	can_example_prepare_data_to_receive();
	can_example_prepare_data_to_send();
}

/*! \brief CAN Task:
 *        - Check if messages are correctly transmitted/received
 *        - Update message for transmission/reception
 */
void can_task(void)
{
int16_t i;
int16_t value;
	if (message_transmitted_on_channel1 == true) {
		message_transmitted_on_channel1 = false;
		if (message_received_on_channel0 == true) {
			message_received_on_channel0 = false;
			for (i=BUFFER_LENGTH-1;i>=1;i--) {
				signal3_buf[i] = signal3_buf[i-1];
			}
			value = (pCANMOB_message2[0].can_msg->data.u8[0]<<8)| \
			(pCANMOB_message2[0].can_msg->data.u8[1]);
			signal3_buf[0] = ( value*0x20) - 0x8000;
			can_example_prepare_data_to_receive();
			can_example_prepare_data_to_send();
		}
	}
}

/*! \brief CAN Prepare Data to Send
 *        - Allocate one MOB in transmission
 *        - Fill the MOB with the correct DATA
 *        - Start the transmission
 */
void can_example_prepare_data_to_send(void)
{
	// Initialize channel 1
	can_init(1,
		((U32)&mob_ram_ch1[0]),
		CANIF_CHANNEL_MODE_NORMAL,
		can_out_callback_channel1);
	// Allocate one mob for TX
	pCANMOB_message0[0].handle = can_mob_alloc(1);

	// Check return if no mob are available
	if (pCANMOB_message0[0].handle==CAN_CMD_REFUSED) {
		while(true);
	}
	pCANMOB_message0[0].can_msg->data.u8[0] =
		((adc_current_conversion&0xFF00)>>8);
	pCANMOB_message0[0].can_msg->data.u8[1] =
		(adc_current_conversion&0xFF);
	can_tx(1,
		pCANMOB_message0[0].handle,
		pCANMOB_message0[0].dlc,
		pCANMOB_message0[0].req_type,
		pCANMOB_message0[0].can_msg);
}

/*! \brief CAN Prepare Data to Receive
 *        - Allocate one MOB in reception
 *        - Start the reception
 */
void can_example_prepare_data_to_receive(void)
{
	// Initialize channel 0
	can_init(0,
		((U32)&mob_ram_ch0[0]),
		CANIF_CHANNEL_MODE_NORMAL,
		can_out_callback_channel0);

	// Allocate one mob for RX
	pCANMOB_message2[0].handle = can_mob_alloc(0);

	can_rx(0,
		pCANMOB_message2[0].handle,
		pCANMOB_message2[0].req_type,
		pCANMOB_message2[0].can_msg);
}
