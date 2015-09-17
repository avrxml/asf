/**
 * \file
 *
 * \brief Application Lin Task Management.
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
#include <avr32/io.h>
#include "compiler.h"
#include "board.h"
#include "power_clocks_lib.h"
#include "gpio.h"
#include "tc.h"
#include "intc.h"
#include "usart.h"
#include "can.h"
#include "lin.h"
#include "dsp.h"
#include "lin_task.h"
#include "conf_demo.h"

// Local definition of start scheduler function
void start_scheduler(void);

// Lin Master Task linked to the transmission of the message with the ID 0x12
extern void lin_master_task_ID12(void);
// Lin Slave Task linked to the reception of the message with the ID 0x12
extern void  lin_slave_task_ID12(U8* d_buf);
// Lin Master Task Callback linked with the ID 0x12
extern void lin_master_cb_task_ID12(U8* d_buf);
// Local Buffer for transmission
uint8_t  lin_data_out_node0[8];
// Local Buffer for emission
volatile U8  lin_data_in_node1[8];
// Current Task Id used for the scheduled table
volatile uint8_t task_id = 0;

extern can_mob_t pCANMOB_message2[1];
A_ALIGNED dsp16_t signal2_buf[BUFFER_LENGTH];

/*! \brief TC interrupt.
 */
#if defined (__GNUC__)
__attribute__((__interrupt__))
#elif defined (__ICCAVR32__)
#pragma handler = TC_INSTANCE_IRQ_GROUP, 1
__interrupt
#endif
static void tc_irq(void)
{
	// Clear the interrupt flag. This is a side effect of reading the TC SR.
	tc_read_sr(TC_INSTANCE, TC_CHANNEL);
	lin_master_task_ID12();
}

volatile avr32_tc_t *tc = TC_INSTANCE;

/*! \brief Start LIN scheduler for periodic transmission of LIN message.
 */
void start_scheduler(void)
{
	// Options for waveform generation.
	static const tc_waveform_opt_t WAVEFORM_OPT = {
		.channel  = TC_CHANNEL,         // Channel selection.
		.bswtrg   = TC_EVT_EFFECT_NOOP, // Software trigger effect on TIOB.
		.beevt    = TC_EVT_EFFECT_NOOP, // External event effect on TIOB.
		.bcpc     = TC_EVT_EFFECT_NOOP, // RC compare effect on TIOB.
		.bcpb     = TC_EVT_EFFECT_NOOP, // RB compare effect on TIOB.
		.aswtrg   = TC_EVT_EFFECT_NOOP, // Software trigger effect on TIOA.
		.aeevt    = TC_EVT_EFFECT_NOOP, // External event effect on TIOA.
		.acpc     = TC_EVT_EFFECT_NOOP, // RC compare effect on TIOA: toggle.
		.acpa     = TC_EVT_EFFECT_NOOP, // RA compare effect on TIOA: toggle.
// Waveform selection: Up mode with automatic trigger(reset) on RC compare.
		.wavsel   = TC_WAVEFORM_SEL_UP_MODE_RC_TRIGGER,
// External event trigger enable.
		.enetrg   = false,
// External event selection.
		.eevt     = 0,
		.eevtedg  = TC_SEL_NO_EDGE,    // External event edge selection.
		.cpcdis   = false,             // Counter disable when RC compare.
		.cpcstop  = false,             // Counter clock stopped with RC compare.
		.burst    = false,             // Burst signal selection.
		.clki     = false,             // Clock inversion.
		.tcclks   = TC_CLOCK_SOURCE_TC5 // Internal source clock 3.
	};

	static const tc_interrupt_t TC_INTERRUPT = {
		.etrgs = 0,
		.ldrbs = 0,
		.ldras = 0,
		.cpcs  = 1,
		.cpbs  = 0,
		.cpas  = 0,
		.lovrs = 0,
		.covfs = 0
	};
	// Register the TC interrupt handler to the interrupt controller.
	INTC_register_interrupt(&tc_irq, TC_INSTANCE_IRQ, AVR32_INTC_INT0);

	// Initialize the timer/counter.
	tc_init_waveform(tc, &WAVEFORM_OPT);

	// Set the compare triggers.
	// Remember TC counter is 16-bits, so counting second is not possible
	// with fPBA = 16 MHz. We configure it to count 0.5 s.
	// We want: (1/(fPBA/8)) * RC = 0.5 s, hence RC = (fPBA/128)/2 = 62500 to
	// get an interrupt every 0.5 s.
	tc_write_rc(tc, TC_CHANNEL, (FPBA_HZ / 128)/2); // Set RC value.

	tc_configure_interrupts(tc, TC_CHANNEL, &TC_INTERRUPT);

	// Start the timer/counter.
	tc_start(tc, TC_CHANNEL);
}

extern volatile avr32_usart_t* usart_lin_node1;

/*! \brief LIN Task Init
 *         - Initialize Node 0 and Node 1
 *         - Initialize lin messages (lin_descriptor)
 *         - Start Sequencer
 */
void lin_task_init(void)
{
	// USART LIN options.
	// Node 0:  LIN_MASTER_MODE
	lin_init (true,0,9600,FOSC0);

	// Node 1:  LIN_SLAVE_MODE
	lin_init (false,1,9600,FOSC0);

	// Configure lin_descriptor
	//- Init LIN data Node 0
	// Object 0
	lin_descript_list_node0[0].l_id = 0x22;
	lin_descript_list_node0[0].l_dlc = 8;
	lin_descript_list_node0[0].l_cmd = PUBLISH;
	lin_descript_list_node0[0].l_status = 0;
	lin_descript_list_node0[0].l_pt_data = lin_data_out_node0;
	lin_descript_list_node0[0].l_pt_function = lin_master_cb_task_ID12;

	// Configure lin_descriptor
	//- Init LIN data Node 1
	// Object 0
	lin_descript_list_node1[0].l_id = 0x22;
	lin_descript_list_node1[0].l_dlc = 8;
	lin_descript_list_node1[0].l_cmd = SUBSCRIBE;
	lin_descript_list_node1[0].l_status = 0;
	lin_descript_list_node1[0].l_pt_data = (U8 *)lin_data_in_node1;
	lin_descript_list_node1[0].l_pt_function = lin_slave_task_ID12;

	// In case of Master Mode, the timing transmission starts...
	start_scheduler();
}

/*! \brief LIN Task
 *         - Update LIN Message content (message to be sent)
 */
void lin_task()
{
	int32_t i;
	int32_t value;
	for (i=BUFFER_LENGTH-1;i>=1;i--) {
		signal2_buf[i] = signal2_buf[i-1];
	}
	value = (lin_descript_list_node1[0].l_pt_data[0]<<8)| \
		(lin_descript_list_node1[1].l_pt_data[1]);
	signal2_buf[0] = ( value*0x20) - 0x8000;
}
/*! \brief LIN Task Stop
 */
void lin_task_stop()
{
}
/*! \brief LIN Task Restart
 */
void lin_task_restart()
{
	Disable_global_interrupt();
	lin_init (false,1,9600,FOSC0);
	Enable_global_interrupt();
}

//_____ M A S T E R    F U N C T I O N S _______________________________________

/*! \brief Lin Command When Id is transmitted
 *
 */
void  lin_master_task_ID12(void)
{
	lin_send_cmd (0,lin_descript_list_node0[0].l_id, \
		lin_descript_list_node0[0].l_dlc );
}
/*! \brief Lin Callback when message is transmitted
 *
 */
void  lin_master_cb_task_ID12(U8* d_buf)
{
	gpio_tgl_gpio_pin(LED1_GPIO);

	d_buf[0] = pCANMOB_message2[0].can_msg->data.u8[0];
	d_buf[1] = pCANMOB_message2[0].can_msg->data.u8[1];

}
//_____ S L A V E   F U N C T I O N S __________________________________________

/*! \brief Lin Callback when message is received
 */
void  lin_slave_task_ID12(U8* d_buf)
{
	gpio_tgl_gpio_pin(LED0_GPIO);
}
