/**
 * \file
 *
 * \brief  Main file of the USB HID QTouch Debug.
 *
 * Copyright (c) 2009-2018 Microchip Technology Inc. and its subsidiaries.
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
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
