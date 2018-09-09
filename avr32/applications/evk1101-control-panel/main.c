/**
 * \file
 *
 * \brief Main file of the USB Full custom example.
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
 *
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */


//_____  I N C L U D E S ___________________________________________________

#include <stdio.h>
#include "conf_usb.h"
#include "usb_descriptors.h"
#include "compiler.h"
#include "preprocessor.h"
#include "board.h"
#include "print_funcs.h"
#include "intc.h"
#include "pm.h"
#include "gpio.h"
#if( EVK1101_CTRL_PANEL_PID==EVK1101_CTRL_PANEL_DEMO_HID_MS_PID )
# include "ctrl_access.h"
#endif
#if (AT45DBX_MEM==ENABLE) &&( EVK1101_CTRL_PANEL_PID==EVK1101_CTRL_PANEL_DEMO_HID_MS_PID )
#  include "conf_at45dbx.h"
#endif
#include "usb_task.h"
#if USB_DEVICE_FEATURE == true
#  include "device_ctrl_panel_task.h"
#  if( EVK1101_CTRL_PANEL_PID==EVK1101_CTRL_PANEL_DEMO_HID_MS_PID )
#    include "device_mass_storage_task.h"
#  endif
#endif
#include "FreeRTOS.h"
#include "task.h"


//_____ M A C R O S ________________________________________________________


//_____ D E F I N I T I O N S ______________________________________________

#define CRLF                  "\r\n"

#if __GNUC__
const char *const pcCtrlPanelVersion = "GCC "__VERSION__" "__DATE__" "__TIME__" ATMEL_CONTROLPANEL_AVR32_UC3_10000"CRLF; // 1.00.00
#elif __ICCAVR32__
const char *const pcCtrlPanelVersion = "IAR "ASTRINGZ(__VER__)"A "__DATE__" "__TIME__" ATMEL_CONTROLPANEL_AVR32_UC3_10000"CRLF; // 1.00.00
#endif
/* xyyzz
   x = major version number (0-9)
   yy = minor version number (00-99)
   zz = minor release number (00-99)
*/


#if (AT45DBX_MEM==ENABLE) &&( EVK1101_CTRL_PANEL_PID==EVK1101_CTRL_PANEL_DEMO_HID_MS_PID )

/*! \brief Initializes AT45DBX resources: GPIO, SPI and AT45DBX.
 */
static void at45dbx_resources_init(void)
{
	static const gpio_map_t AT45DBX_SPI_GPIO_MAP =
	{
		{AT45DBX_SPI_SCK_PIN,          AT45DBX_SPI_SCK_FUNCTION         },  // SPI Clock.
		{AT45DBX_SPI_MISO_PIN,         AT45DBX_SPI_MISO_FUNCTION        },  // MISO.
		{AT45DBX_SPI_MOSI_PIN,         AT45DBX_SPI_MOSI_FUNCTION        },  // MOSI.
#define AT45DBX_ENABLE_NPCS_PIN(NPCS, unused) \
		{AT45DBX_SPI_NPCS##NPCS##_PIN, AT45DBX_SPI_NPCS##NPCS##_FUNCTION},  // Chip Select NPCS.
		MREPEAT(AT45DBX_MEM_CNT, AT45DBX_ENABLE_NPCS_PIN, ~)
#undef AT45DBX_ENABLE_NPCS_PIN
	};

	// SPI options.
	spi_options_t spiOptions =
	{
		.reg          = AT45DBX_SPI_FIRST_NPCS,   // Defined in conf_at45dbx.h.
		.baudrate     = AT45DBX_SPI_MASTER_SPEED, // Defined in conf_at45dbx.h.
		.bits         = AT45DBX_SPI_BITS,         // Defined in conf_at45dbx.h.
		.spck_delay   = 0,
		.trans_delay  = 0,
		.stay_act     = 1,
		.spi_mode     = 0,
		.modfdis      = 1
	};

	// Assign I/Os to SPI.
	gpio_enable_module(AT45DBX_SPI_GPIO_MAP,
			sizeof(AT45DBX_SPI_GPIO_MAP) / sizeof(AT45DBX_SPI_GPIO_MAP[0]));

	// Initialize as master.
	spi_initMaster(AT45DBX_SPI, &spiOptions);

	// Set selection mode: variable_ps, pcs_decode, delay.
	spi_selectionMode(AT45DBX_SPI, 0, 0, 0);

	// Enable SPI.
	spi_enable(AT45DBX_SPI);

	// Initialize data flash with SPI clock Osc0.
	at45dbx_init(spiOptions, FOSC0);
}

#endif  // AT45DBX_MEM == ENABLE


/*! \brief Starts the generation of a 48-MHz clock for the USB.
 */
/*! \brief Main function. Execution starts here.
 *
 *
 * \retval No return from main function
 */
int main(void)
{
	// Switch main clock from internal RC to external Oscillator 0
	pm_switch_to_osc0(&AVR32_PM, FOSC0, OSC0_STARTUP);

	// Start Debug RS232 on USART_1
	init_dbg_rs232(FOSC0);

#if (AT45DBX_MEM==ENABLE) &&( EVK1101_CTRL_PANEL_PID==EVK1101_CTRL_PANEL_DEMO_HID_MS_PID )
	// This resource is used by the mass storage device task
	at45dbx_resources_init();
#endif

#if( EVK1101_CTRL_PANEL_PID==EVK1101_CTRL_PANEL_DEMO_HID_MS_PID )
	if (!ctrl_access_init()) {
		portDBG_TRACE("The module CTRL_ACCESS could not be initialized.");
		while (true);
	}
#endif

	pm_configure_usb_clock();
	usb_task_init();

#if USB_DEVICE_FEATURE == true
	device_full_custom_task_init();
	#if( EVK1101_CTRL_PANEL_PID==EVK1101_CTRL_PANEL_DEMO_HID_MS_PID )
	device_mass_storage_task_init();
	#endif
#endif

	portDBG_TRACE("AVR UC3B Control Panel Firmware.");
	portDBG_TRACE(pcCtrlPanelVersion);
	vTaskStartScheduler();
	portDBG_TRACE("FreeRTOS returned.");
	while (true);
}
