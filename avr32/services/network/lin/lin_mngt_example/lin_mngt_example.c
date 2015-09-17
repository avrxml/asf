/*****************************************************************************
 *
 * \file
 *
 * \brief LIN example application for AVR32 USART driver.
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
 ******************************************************************************/

/*! \mainpage
 * \section intro Introduction
 * This is the documentation for the data structures, functions, variables,
 * defines, enums, and typedefs for the USART in LIN modes software driver.\n It also comes
 * bundled with an example. This example is a basic example supporting Master and Slave Mode example.\n
 * It is possible to choose between the two modes. \n
 * <b>Example's operating mode: </b>
 * - Connect the RX/TX lines of LIN to the transceiver.
 * - Open CANVector Tools and select  in LINANALYSER/ folder the project link to you configuration \n
 *        <b>--></b> Lin Version 1.3 or 2.0 \n
 *        <b>--></b>  Node configuration   Master or Slave \n
 * - Run the source code.
 * - On the bus you will see \n
 *        <b>--></b> In case Of Master Configuration: The message with the ID 12 sent by UC3 and the response sent by CANVector Tools.\n
 *        The Message with the ID 0x13 is sent with the response by the UC3. \n
 *        <b>--></b>  In case of Slave Configuration: The message with the ID 12 sent by CANVector Tools and the response sent by UC3.\n
 *        The Message with the ID 0x13 is sent with the response by the CANVector Tools. \n
 *
 * \section files Main Files
 * - lin.c: Service for LIN support mode;
 * - lin.h: Header file of service for LIN support mode;
 * - lin_mngt_example.c: LIN example application in slave mode.
 * - conf_lin.h: LIN example configuration file for the driver.
 *
 * \section compilinfo Compilation Information
 * This software is written for GNU GCC for AVR32 and for IAR Embedded Workbench
 * for Atmel AVR32. Other compilers may or may not work.
 *
 * \section deviceinfo Device Information
 * All AVR32 devices with USART revision 3 and above revision of the IP can be used.
 *
 * \section configinfo Configuration Information
 * This example has been tested with the following configuration:
 * - STK600 starter kit and AVR32UC3B device;
 * - CPU clock: 12 MHz;
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/products/AVR32/">Atmel AVR32</A>.\n
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
#include "lin.h"

#if BOARD == UC3C_EK
#   define TC_INSTANCE               &AVR32_TC0
#   define TC_INSTANCE_IRQ           AVR32_TC0_IRQ0
#   define TC_INSTANCE_IRQ_GROUP     AVR32_TC0_IRQ_GROUP
#elif BOARD == EVK1101
#   define TC_INSTANCE               &AVR32_TC
#   define TC_INSTANCE_IRQ           AVR32_TC_IRQ0
#   define TC_INSTANCE_IRQ_GROUP     AVR32_TC_IRQ_GROUP
#else
#error Wrong board
#endif

//#define MASTER_MODE
//! PBA Clock Reference
#define FPBA                                FOSC0

//! Timer Counter Channel Used
#define TC_CHANNEL                          0

//! Lin Master Task linked to the transmission of the message with the ID 0x12
extern void lin_master_task_ID12(void);

//! Lin Master Task linked to the transmission of the message with the ID 0x15
extern void lin_master_task_ID15(void);

//! Lin Slave Task linked to the reception of the message with the ID 0x12
extern void lin_slave_task_ID12(U8* d_buf);

//! Lin Slave Task linked to the reception of the message with the ID 0x15
extern void lin_slave_task_ID15(U8* d_buf);

//! Local Buffer for transmission
U8  lin_data_out_node0[8];

//! Local Buffer for emission
U8  lin_data_in_node0[8];

//! Current Task Id used for the scheduled table
volatile unsigned char task_id = 0;

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

  // Send message on LIN ...
  switch(task_id)
  {
    case 0 :
      lin_master_task_ID12();
      task_id=1;
      break;

     case 1 :
      lin_master_task_ID15();
      task_id=0;
      break;

    default :
      break;
  }
}



/*! \brief Start Scheduler: Configuration of dedicated timer for periodic LIN transmission.
 */
void start_scheduler(void)
{
  volatile avr32_tc_t *tc = TC_INSTANCE;

  // Options for waveform generation.
  static const tc_waveform_opt_t WAVEFORM_OPT =
  {
    .channel  = TC_CHANNEL,                        // Channel selection.

    .bswtrg   = TC_EVT_EFFECT_NOOP,                // Software trigger effect on TIOB.
    .beevt    = TC_EVT_EFFECT_NOOP,                // External event effect on TIOB.
    .bcpc     = TC_EVT_EFFECT_NOOP,                // RC compare effect on TIOB.
    .bcpb     = TC_EVT_EFFECT_NOOP,                // RB compare effect on TIOB.

    .aswtrg   = TC_EVT_EFFECT_NOOP,                // Software trigger effect on TIOA.
    .aeevt    = TC_EVT_EFFECT_NOOP,                // External event effect on TIOA.
    .acpc     = TC_EVT_EFFECT_NOOP,                // RC compare effect on TIOA: toggle.
    .acpa     = TC_EVT_EFFECT_NOOP,                // RA compare effect on TIOA: toggle (other possibilities are none, set and clear).

    .wavsel   = TC_WAVEFORM_SEL_UP_MODE_RC_TRIGGER,// Waveform selection: Up mode with automatic trigger(reset) on RC compare.
    .enetrg   = false,                             // External event trigger enable.
    .eevt     = 0,                                 // External event selection.
    .eevtedg  = TC_SEL_NO_EDGE,                    // External event edge selection.
    .cpcdis   = false,                             // Counter disable when RC compare.
    .cpcstop  = false,                             // Counter clock stopped with RC compare.

    .burst    = false,                             // Burst signal selection.
    .clki     = false,                             // Clock inversion.
    .tcclks   = TC_CLOCK_SOURCE_TC5                // Internal source clock 3, connected to fPBA / 128.
  };

  static const tc_interrupt_t TC_INTERRUPT =
  {
    .etrgs = 0,
    .ldrbs = 0,
    .ldras = 0,
    .cpcs  = 1,
    .cpbs  = 0,
    .cpas  = 0,
    .lovrs = 0,
    .covfs = 0
  };

  // Register the RTC interrupt handler to the interrupt controller.
  INTC_register_interrupt(&tc_irq, TC_INSTANCE_IRQ, AVR32_INTC_INT0);

  // Initialize the timer/counter.
  tc_init_waveform(tc, &WAVEFORM_OPT);         // Initialize the timer/counter waveform.

  // Set the compare triggers.
  // Remember TC counter is 16-bits, so counting second is not possible with fPBA = 16 MHz.
  // We configure it to count 0.5 s.
  // We want: (1/(fPBA/8)) * RC = 0.5 s, hence RC = (fPBA/128)/2 = 62500 to get an interrupt every 0.5 s.
  tc_write_rc(tc, TC_CHANNEL, (FPBA / 128)/2); // Set RC value.

  tc_configure_interrupts(tc, TC_CHANNEL, &TC_INTERRUPT);

  // Start the timer/counter.
  tc_start(tc, TC_CHANNEL);                    // And start the timer/counter.

}

/*! \brief This is an example demonstrating the LIN mode of USART IP
 *         functionalities using a dedicated USART LIN driver.
 */
int main(void)
{
   // Switch main clock to external oscillator 0 (crystal).
  pcl_switch_to_osc(PCL_OSC0, FOSC0, OSC0_STARTUP);

#if (defined AVR32_HMATRIX)
  AVR32_HMATRIX.mcfg[AVR32_HMATRIX_MASTER_CPU_INSN] = 0x1;
#endif
#if (defined AVR32_HMATRIXB)
  AVR32_HMATRIXB.mcfg[AVR32_HMATRIXB_MASTER_CPU_INSN] = 0x1;
#endif

  // The INTC driver has to be used only for GNU GCC for AVR32.
  // Initialize interrupt vectors.
  INTC_init_interrupts();

  // USART LIN options.
#ifdef MASTER_MODE
  // Node 0:  LIN_MASTER_MODE
  lin_init( true, 0, 9600, FOSC0);
#else
  // Node 0:  LIN_SLAVE_MODE
  lin_init( false, 0, 9600, FOSC0);
#endif

  // Configure lin_descriptor
  //- Init LIN data Node 0
  // Object 0
  lin_descript_list_node0[0].l_id = 0x12;
  lin_descript_list_node0[0].l_dlc = 2;
  lin_descript_list_node0[0].l_cmd = PUBLISH;
  lin_descript_list_node0[0].l_status = 0;
  lin_descript_list_node0[0].l_pt_data = lin_data_out_node0;
  lin_descript_list_node0[0].l_pt_function = lin_slave_task_ID12;

  // Configure lin_descriptor
  //- Init LIN data Node 0
  // Object 1
  lin_descript_list_node0[1].l_id = 0x15;
  lin_descript_list_node0[1].l_dlc = 2;
  lin_descript_list_node0[1].l_cmd = SUBSCRIBE;
  lin_descript_list_node0[1].l_status = 0;
  lin_descript_list_node0[1].l_pt_data = lin_data_in_node0;
  lin_descript_list_node0[1].l_pt_function = lin_slave_task_ID15;

#ifdef MASTER_MODE
  // In case of Master Mode, the timing transmission starts...
  start_scheduler();
#else
  // In case of Slave Mode, only wait until ID reception...
#endif

  Enable_global_interrupt();

  while (true);
}

//_____ M A S T E R    F U N C T I O N S _______________________________________

/*! \brief lin_master_task_ID12
 *
 */
void  lin_master_task_ID12(void)
{
  lin_send_cmd (0,lin_descript_list_node0[0].l_id, lin_descript_list_node0[0].l_dlc );
}

/*! \brief lin_master_task_ID15
 *
 */
void  lin_master_task_ID15(void)
{
  lin_send_cmd (0,lin_descript_list_node0[1].l_id, lin_descript_list_node0[1].l_dlc );
}

//_____ S L A V E   F U N C T I O N S __________________________________________

/*! \brief lin_slave_task_ID12
 *
 */
void  lin_slave_task_ID12(U8* d_buf)
{
  gpio_tgl_gpio_pin(LED0_GPIO);
  d_buf[0] = 0x77;
  d_buf[1] = 0x88;
}

/*! \brief lin_slave_task_ID15
 *
 */
void  lin_slave_task_ID15(U8* d_buf)
{
  gpio_tgl_gpio_pin(LED1_GPIO);
}
