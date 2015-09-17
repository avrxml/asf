/**
 * \file
 *
 * \brief Main file of the USB mass-storage example.
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
 *
 */

/*! \mainpage AVR UC3 USB Software Framework for Mass Storage
 *
 * \section install Description
 * This embedded application source code illustrates how to implement a USB mass-storage application
 * on the AVR UC3 A3/A4 microcontroller.
 *
 * As the AVR UC3 implements a device/host USB controller, the embedded application can operate
 * in one of the following USB operating modes:
 *   - USB device;
 *   - USB reduced-host controller;
 *   - USB dual-role device (depending on the ID pin). This is the default configuration used in this example.
 *
 * The next table shows the customization to do (into the conf_usb.h and conf_access.h configuration files)
 * in order to support one of the USB operating modes:
 * <table border="1">
 * <tr>
 * <th></th>
 * <th>USB device</th>
 * <th>USB reduced-host</th>
 * <th>USB dual-role</th>
 * </tr>
 * <tr>
 * <th>conf_usb.h</th>
 * </tr>
 * <tr>
 * <th>USB_DEVICE_FEATURE</th>
 * <td>true</td>
 * <td>false</td>
 * <td>true</td>
 * </tr>
 * <tr>
 * <th>USB_HOST_FEATURE</th>
 * <td>false</td>
 * <td>true</td>
 * <td>true</td>
 * </tr>
 * <tr>
 * <th>conf_access.h</th>
 * </tr>
 * <tr>
 * <th>LUN_USB</th>
 * <td>false</td>
 * <td>true</td>
 * <td>true</td>
 * </tr>
 * <tr>
 * <th>ACCESS_USB</th>
 * <td>true</td>
 * <td>false</td>
 * <td>true</td>
 * </tr>
 * </table>
 *
 * To optimize embedded code/RAM size and reduce the number of source modules, the application can be
 * configured to use one and only one of these operating modes.
 *
 * \section sample About the Sample Application
 * By default the sample code is delivered with a simple preconfigured dual-role USB application.
 * It means that the code generated allows to operate as a device or a host depending on the USB ID pin:
 *
 *   - Attached to a mini-B plug (ID pin unconnected) the application will be used in the device operating mode.
 * Thus, the application can be connected to a system host (e.g. a PC) to operate as a USB mass-storage device (removable drive):
 * *
 *   - Attached to a mini-A plug (ID pin tied to ground) the application will operate in reduced-host mode.
 * This mode allows to connect a USB mass-storage device:
 *
 *
 * \section device_use Using the USB Device Mode
 * Connect the application to a USB mass-storage host (e.g. a PC) with a mini-B (embedded side) to A (PC host side) cable.
 * The application will behave as a USB key. It will allow to access files on the on-board virtual, data flash and SD/MMC memories.
 *
 * \section arch Architecture
 * As illustrated in the figure below, the application entry point is located in the mass_storage_example.c file.
 * The main function first performs the initialization of services and tasks and then runs them in an infinite loop.
 *
 * The sample mass-storage application is based on four different tasks:
 *   - The USB task (usb_task.c associated source file) is the task performing the USB low-level
 * enumeration process in device or host mode. Once this task has detected that the USB connection is fully
 * operational, it updates various status flags that can be checked within the high-level applicative tasks.
 *   - The device mass-storage task (device_mass_storage_task.c associated source file) performs
 * SCSI bulk-only protocol decoding and flash memory accesses.
 *   - The host mass-storage task (host_mass_storage_task.c associated file) manages the connected
 * device mass-storage interface by performing SCSI bulk-only protocol encoding and flash memory accesses.
 *
 * \section config Configuration
 * The sample application is configured to implement both host and device functionalities.
 * Of course it can also be configured to be used only in device or reduced-host mode (see the conf_usb.h file).
 * Depending on the USB operating mode selected, the USB task will call either the USB host task (usb_host_task.c),
 * or the USB device task (usb_device_task.c) to manage USB specification chapter 9 requests.
 *
 * \section limitations Limitations
 * Some mass-storage devices do not present directly a mass-storage-class
 * interface, which may e.g. be hidden behind a hub-class device. These devices
 * are not supported by this example and the current mass-storage software
 * framework.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/products/AVR32/">Atmel AVR UC3</A>.\n
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

//_____  I N C L U D E S ___________________________________________________

#ifndef FREERTOS_USED
#if __GNUC__
# include "nlao_cpu.h"
# include "nlao_usart.h"
#endif
#endif
#include <stdio.h>
#include "compiler.h"
#include "preprocessor.h"
#include "board.h"
#include "print_funcs.h"
#include "intc.h"
#include "pm.h"
#include "gpio.h"
#include "usart.h"
#include "print_funcs.h"
#include "ctrl_access.h"
#include "main.h"
#if (AT45DBX_MEM == ENABLE)
# include "spi.h"
# include "conf_at45dbx.h"
# endif
#if (SD_MMC_SPI_MEM == ENABLE)
# include "spi.h"
# include "conf_sd_mmc_spi.h"
#endif
#if (SD_MMC_MCI_0_MEM == ENABLE) || (SD_MMC_MCI_1_MEM == ENABLE)
# include "mci.h"
# include "conf_sd_mmc_mci.h"
#endif
#ifdef FREERTOS_USED
# include "FreeRTOS.h"
# include "task.h"
#endif
#include "conf_usb.h"
#include "usb_task.h"
#if (USB_DEVICE_FEATURE == true)
# include "device_mass_storage_task.h"
#endif
#if (USB_HOST_FEATURE == true)
# include "host_mass_storage_task.h"
#endif
#include "mmi_task.h"


//_____ D E F I N I T I O N S ______________________________________________

/*! \name System Clock Frequencies
 */
//! @{
pm_freq_param_t   pm_freq_param=
{
	.cpu_f  =       APPLI_CPU_SPEED,
	.pba_f    =     APPLI_PBA_SPEED,
	.osc0_f     =   FOSC0,
	.osc0_startup = OSC0_STARTUP
};
//! @}


#ifndef FREERTOS_USED

/*! \brief Initializes STDIO.
 */
static void init_stdio(void)
{
#if __GNUC__ && __AVR32__

	static const gpio_map_t STDIO_USART_GPIO_MAP =
	{
		{STDIO_USART_RX_PIN, STDIO_USART_RX_FUNCTION},
		{STDIO_USART_TX_PIN, STDIO_USART_TX_FUNCTION}
	};

	// Initialize the USART used for STDIO.
	set_usart_base((void *)STDIO_USART);
	gpio_enable_module(STDIO_USART_GPIO_MAP,
			sizeof(STDIO_USART_GPIO_MAP) / sizeof(STDIO_USART_GPIO_MAP[0]));
	usart_init(STDIO_USART_BAUDRATE);

#elif __ICCAVR32__

	static const gpio_map_t STDIO_USART_GPIO_MAP =
	{
		{STDIO_USART_RX_PIN, STDIO_USART_RX_FUNCTION},
		{STDIO_USART_TX_PIN, STDIO_USART_TX_FUNCTION}
	};

	static const usart_options_t STDIO_USART_OPTIONS =
	{
		.baudrate     = STDIO_USART_BAUDRATE,
		.charlength   = 8,
		.paritytype   = USART_NO_PARITY,
		.stopbits     = USART_1_STOPBIT,
		.channelmode  = USART_NORMAL_CHMODE
	};

	// Initialize the USART used for STDIO.
	extern volatile avr32_usart_t *volatile stdio_base;
	stdio_base = STDIO_USART;
	gpio_enable_module(STDIO_USART_GPIO_MAP,
			sizeof(STDIO_USART_GPIO_MAP) / sizeof(STDIO_USART_GPIO_MAP[0]));
	usart_init_rs232(STDIO_USART, &STDIO_USART_OPTIONS, APPLI_PBA_SPEED);

#endif
}

  #if __GNUC__

/*! \brief Low-level initialization routine called during startup, before the
 *         main function.
 *
 * This version comes in replacement to the default one provided by the Newlib
 * add-ons library.
 * Newlib add-ons' _init_startup only calls init_exceptions, but Newlib add-ons'
 * exception and interrupt vectors are defined in the same section and Newlib
 * add-ons' interrupt vectors are not compatible with the interrupt management
 * of the INTC module.
 * More low-level initializations are besides added here.
 */
int _init_startup(void)
{
	// Import the Exception Vector Base Address.
	extern void _evba;

	// Load the Exception Vector Base Address in the corresponding system register.
	Set_system_register(AVR32_EVBA, (int)&_evba);

	// Enable exceptions.
	Enable_global_exception();

	// Initialize interrupt handling.
	INTC_init_interrupts();

	// Don't-care value for GCC.
	return 1;
}

  #elif __ICCAVR32__

/*! \brief Low-level initialization routine called during startup, before the
 *         main function.
 */
int __low_level_init(void)
{
	// Enable exceptions.
	Enable_global_exception();

	// Initialize interrupt handling.
	INTC_init_interrupts();

	// Request initialization of data segments.
	return 1;
}

	#endif  // Compiler

#endif  // FREERTOS_USED


#if AT45DBX_MEM == ENABLE

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

	// If the SPI used by the AT45DBX is not enabled.
	if (!spi_is_enabled(AT45DBX_SPI))
	{
		// Initialize as master.
		spi_initMaster(AT45DBX_SPI, &spiOptions);

		// Set selection mode: variable_ps, pcs_decode, delay.
		spi_selectionMode(AT45DBX_SPI, 0, 0, 0);

		// Enable SPI.
		spi_enable(AT45DBX_SPI);
	}

	// Initialize data flash with SPI PB clock.
	at45dbx_init(spiOptions, pm_freq_param.pba_f);
}

#endif  // AT45DBX_MEM == ENABLE


#if SD_MMC_SPI_MEM == ENABLE

/*! \brief Initializes SD/MMC resources: GPIO, SPI and SD/MMC.
 */
static void sd_mmc_spi_resources_init(void)
{
	static const gpio_map_t SD_MMC_SPI_GPIO_MAP =
	{
		{SD_MMC_SPI_SCK_PIN,  SD_MMC_SPI_SCK_FUNCTION },  // SPI Clock.
		{SD_MMC_SPI_MISO_PIN, SD_MMC_SPI_MISO_FUNCTION},  // MISO.
		{SD_MMC_SPI_MOSI_PIN, SD_MMC_SPI_MOSI_FUNCTION},  // MOSI.
		{SD_MMC_SPI_NPCS_PIN, SD_MMC_SPI_NPCS_FUNCTION}   // Chip Select NPCS.
	};

	// SPI options.
	spi_options_t spiOptions =
	{
		.reg          = SD_MMC_SPI_NPCS,
		.baudrate     = SD_MMC_SPI_MASTER_SPEED,  // Defined in conf_sd_mmc_spi.h.
		.bits         = SD_MMC_SPI_BITS,          // Defined in conf_sd_mmc_spi.h.
		.spck_delay   = 0,
		.trans_delay  = 0,
		.stay_act     = 1,
		.spi_mode     = 0,
		.modfdis      = 1
	};

	// Assign I/Os to SPI.
	gpio_enable_module(SD_MMC_SPI_GPIO_MAP,
						sizeof(SD_MMC_SPI_GPIO_MAP) / sizeof(SD_MMC_SPI_GPIO_MAP[0]));

	// If the SPI used by the SD/MMC is not enabled.
	if (!spi_is_enabled(SD_MMC_SPI)) {
		// Initialize as master.
		spi_initMaster(SD_MMC_SPI, &spiOptions);

		// Set selection mode: variable_ps, pcs_decode, delay.
		spi_selectionMode(SD_MMC_SPI, 0, 0, 0);

		// Enable SPI.
		spi_enable(SD_MMC_SPI);
	}

	// Initialize SD/MMC with SPI PB clock.
	sd_mmc_spi_init(spiOptions, pm_freq_param.pba_f);
}

#endif  // SD_MMC_SPI_MEM == ENABLE


#if SD_MMC_MCI_0_MEM == ENABLE || SD_MMC_MCI_1_MEM == ENABLE

/*! \brief Initializes SD/MMC resources: GPIO, MCI and SD/MMC.
 */
static void sd_mmc_mci_resources_init(void)
{
	static const gpio_map_t SD_MMC_MCI_GPIO_MAP =
	{
		{SD_SLOT_8BITS_CLK_PIN,   SD_SLOT_8BITS_CLK_FUNCTION  },  // SD CLK.
		{SD_SLOT_8BITS_CMD_PIN,   SD_SLOT_8BITS_CMD_FUNCTION  },  // SD CMD.
		{SD_SLOT_8BITS_DATA0_PIN, SD_SLOT_8BITS_DATA0_FUNCTION},  // SD DAT[0].
		{SD_SLOT_8BITS_DATA1_PIN, SD_SLOT_8BITS_DATA1_FUNCTION},  // DATA Pin.
		{SD_SLOT_8BITS_DATA2_PIN, SD_SLOT_8BITS_DATA2_FUNCTION},  // DATA Pin.
		{SD_SLOT_8BITS_DATA3_PIN, SD_SLOT_8BITS_DATA3_FUNCTION},  // DATA Pin.
		{SD_SLOT_8BITS_DATA4_PIN, SD_SLOT_8BITS_DATA4_FUNCTION},  // DATA Pin.
		{SD_SLOT_8BITS_DATA5_PIN, SD_SLOT_8BITS_DATA5_FUNCTION},  // DATA Pin.
		{SD_SLOT_8BITS_DATA6_PIN, SD_SLOT_8BITS_DATA6_FUNCTION},  // DATA Pin.
		{SD_SLOT_8BITS_DATA7_PIN, SD_SLOT_8BITS_DATA7_FUNCTION}   // DATA Pin.
	};

	// Assign I/Os to MCI.
	gpio_enable_module(SD_MMC_MCI_GPIO_MAP,
			sizeof(SD_MMC_MCI_GPIO_MAP) / sizeof(SD_MMC_MCI_GPIO_MAP[0]));

	// Enable pull-up for Card Detect.
	gpio_enable_pin_pull_up(SD_SLOT_8BITS_CARD_DETECT);

	// Enable pull-up for Write Protect.
	gpio_enable_pin_pull_up(SD_SLOT_8BITS_WRITE_PROTECT);

	// Initialize SD/MMC with MCI PB clock.
	sd_mmc_mci_init(SD_SLOT_8BITS, pm_freq_param.pba_f, pm_freq_param.cpu_f);
}

#endif  // SD_MMC_MCI_0_MEM == ENABLE || SD_MMC_MCI_1_MEM == ENABLE


/*! \brief Main function. Execution starts here.
 *
 * \retval 42 Fatal error.
 */
int main(void)
{
	// Set CPU and PBA clock
	if( PM_FREQ_STATUS_FAIL==pm_configure_clocks(&pm_freq_param) )
		while (true);

	// Initialize the USART for debug purpose.
	init_stdio();

	union
	{
		unsigned long                 scfg;
		avr32_hmatrix_scfg_t          SCFG;
	} u_avr32_hmatrix_scfg;

	// For the internal-flash HMATRIX slave, use last master as default.
	u_avr32_hmatrix_scfg.scfg = AVR32_HMATRIX.scfg[AVR32_HMATRIX_SLAVE_FLASH];
	u_avr32_hmatrix_scfg.SCFG.defmstr_type = AVR32_HMATRIX_DEFMSTR_TYPE_LAST_DEFAULT;
	AVR32_HMATRIX.scfg[AVR32_HMATRIX_SLAVE_FLASH] = u_avr32_hmatrix_scfg.scfg;

	// For the internal-SRAM HMATRIX slave, use last master as default.
	u_avr32_hmatrix_scfg.scfg = AVR32_HMATRIX.scfg[AVR32_HMATRIX_SLAVE_SRAM];
	u_avr32_hmatrix_scfg.SCFG.defmstr_type = AVR32_HMATRIX_DEFMSTR_TYPE_LAST_DEFAULT;
	AVR32_HMATRIX.scfg[AVR32_HMATRIX_SLAVE_SRAM] = u_avr32_hmatrix_scfg.scfg;

	// For the EBI HMATRIX slave, use last master as default.
	u_avr32_hmatrix_scfg.scfg = AVR32_HMATRIX.scfg[AVR32_HMATRIX_SLAVE_EBI];
	u_avr32_hmatrix_scfg.SCFG.defmstr_type = AVR32_HMATRIX_DEFMSTR_TYPE_LAST_DEFAULT;
	AVR32_HMATRIX.scfg[AVR32_HMATRIX_SLAVE_EBI] = u_avr32_hmatrix_scfg.scfg;

	// Initialize USART link.
	init_dbg_rs232(pm_freq_param.pba_f);

#if AT45DBX_MEM == ENABLE
	at45dbx_resources_init();
#endif
#if SD_MMC_SPI_MEM == ENABLE
	sd_mmc_spi_resources_init();
#endif
#if SD_MMC_MCI_0_MEM == ENABLE || SD_MMC_MCI_1_MEM == ENABLE
	sd_mmc_mci_resources_init();
#endif
#ifdef FREERTOS_USED
	if (!ctrl_access_init()) {
		portDBG_TRACE("The module CTRL_ACCESS could not be initialized.");
		while (true);
	}
#endif  // FREERTOS_USED

	// Initialize USB clock.
	pm_configure_usb_clock();

	// Initialize USB tasks.
	usb_task_init();
#if USB_DEVICE_FEATURE == true
	device_mass_storage_task_init();
#endif
#if USB_HOST_FEATURE == true
	host_mass_storage_task_init();
#endif

	// Initialize the MMI.
	mmi_task_init(pm_freq_param.cpu_f, pm_freq_param.pba_f);

#ifdef FREERTOS_USED
	vTaskStartScheduler();
	portDBG_TRACE("FreeRTOS returned.");
	while (true);
#else
	while (true) {
		usb_task();
	#if USB_DEVICE_FEATURE == true
		device_mass_storage_task();
	#endif
	#if USB_HOST_FEATURE == true
		host_mass_storage_task();
	#endif
		mmi_task();
	}
#endif  // FREERTOS_USED
}
