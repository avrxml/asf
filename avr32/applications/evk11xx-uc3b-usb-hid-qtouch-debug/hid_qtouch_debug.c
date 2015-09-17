/**
 * \file
 *
 * \brief  Main file of the USB HID QTouch Debug.
 *
 * Copyright (c) 2009-2015 Atmel Corporation. All rights reserved.
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
 */

/**
 * \mainpage
 *
 * \par USB HID stub for embedded QTouch Library to QTouch Studio communication
 *
 * This application is a USART<->USB gateway dedicated for trace messages
 * between the embedded QTouch library and QTouch Studio. It is meant to run
 * on a UC3B device connected through a UART to another device running the
 * QTouch Library and connected through USB to a computer running QTouch Studio.
 *
 * \par Usage
 *
 * -# Set-up of the USB stack
 * -# Set-up of the HID generic USB class
 * -# Set-up of the UART connection
 * -# All messages received from USB (i.e. from QTouch Studio) are sent over the
 * UART to the QTouch Library.
 * -# All messages received from the QTouch Library (i.e. over the UART) are
 * sent through USB to QTouch Studio.
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

//_____  I N C L U D E S ___________________________________________________

#include "compiler.h"
#include "board.h"
#include "intc.h"
#include "power_clocks_lib.h"
#include "conf_usb.h"
#include "usb_task.h"
#if USB_DEVICE_FEATURE == true
#include "device_hid_qtouch_debug_task.h"
#endif
#include "hid_qtouch_debug.h"


//_____ M A C R O S ________________________________________________________


//_____ D E F I N I T I O N S ______________________________________________

/*! \name System Clock Frequencies
 */
//! @{
pcl_freq_param_t pcl_freq_param =
{
	.cpu_f        = FCPU_HZ,
	.pba_f        = FPBA_HZ,
	.osc0_f       = FOSC0,
	.osc0_startup = OSC0_STARTUP
};
//! @}


/*! \brief Main function. Execution starts here.
 *
 * \retval No return value.
 */
int main(void)
{
#ifdef __GNUC__
	// Initialize the INTC sw driver.
	INTC_init_interrupts();
#endif

	// Configure system clocks.
	if (pcl_configure_clocks(&pcl_freq_param) != PASS)
		while (true);

	// Initialize USB clock (on PLL1)
	pcl_configure_usb_clock();

	// Initialize USB task
	usb_task_init();

#if USB_DEVICE_FEATURE == true
	// Initialize device generic USB task
	device_generic_hid_task_init();
#endif

	// No OS here. Need to call each task in round-robin mode.
	while (true) {
		usb_task();
	#if USB_DEVICE_FEATURE == true
		device_generic_hid_task();
	#endif
	}
}
