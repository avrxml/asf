/*****************************************************************************
 *
 * \file
 *
 * \brief Standalone lwIP example for AVR UC3.
 *
 * Copyright (c) 2010 Atmel Corporation. All rights reserved.
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


#include "board.h"
#include "power_clocks_lib.h"
#include "display.h"
#include "ethernet.h"

/*! \mainpage
 * \section intro Introduction
 * This documents data structures, functions, variables, defines, enums, and
 * typedefs in the software for the lwIP standalone example.
 *
 * The given example is a lwIP example using the current lwIP stack and MACB driver.
 *
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC for AVR32 and IAR Systems compiler
 * for AVR32. Other compilers may or may not work.
 *
 * \section files Main Files
 * - macb.c: ethernet low level driver.
 * - macb.h: ethernet low level header file.
 * - conf_eth.h: Configuration file for the MACB module software driver.
 * - standalone_lwip_demo.c: lwIP standalone example (no operating system).
 * - lwipopts.h: Configuration file of the lwIP stack for this example.
 *
 * \section deviceinfo Device Info
 * This example has been tested with the following configuration:
 * - EVK1105 evaluation kit.
 * - CPU clock: 66 MHz.
 *
 * \section configinfo Configuration Information
 * This example features simple lwIP web server.
 * - Plug the Ethernet cable directly into the evaluation kit to connect to the PC.
 * - Configuring the PC network port to local mode to setup a 'point to point' network.
 * - Start the example.
 * - Launch your favorite web browser.
 * - Type the WEB server example IP address in your browser's address bar.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/products/AVR32/">Atmel AVR32</A>.\n
 */

//_____ D E F I N I T I O N S ______________________________________________

/*! \name System Clock Frequencies
 */
//! @{
#define FMCK_HZ                       60000000
#define FCPU_HZ                       FMCK_HZ
#define FHSB_HZ                       FCPU_HZ
#define FPBB_HZ                       FMCK_HZ
#define FPBA_HZ                       FMCK_HZ
//! @}

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


/*! \brief Main function: Responsible for configuring the hardware,
 * ethernet, and other services. It also maintains the lwIP timers,
 * and processes any packets that are received.
 */
int main( void )
{
	/* Configure system clocks */
	pcl_configure_clocks(&pcl_freq_param);

	/* Initialize the Display */
	display_init(pcl_freq_param.cpu_f);

	/* Bring up the ethernet interface & initializes timer0, channel0 */
	init_ethernet(&pcl_freq_param);

	/* This is the main polling loop. */
	while(1)
	{
		/* Check if any packets are available and process if they are
		 * ready. That function also manages the lwIP timers */
		ethernet_task();
	}

	// Never reached.
	return 0;
}


