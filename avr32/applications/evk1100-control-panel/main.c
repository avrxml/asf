/**
 * \file
 *
 * \brief Control Panel main file.
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
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


//!
//! Init the microcontroller & the board shared resources. Launch the Control
//! Panel main modules. Start FreeRTOS.
//!


//_____  I N C L U D E S ___________________________________________________


#include <string.h>

#include "pm.h"
#include "flashc.h"
#include "gpio.h"
#include "spi.h"
// #include "wdt.h"

#include "ctrl_access.h"
#include "fsaccess.h"

//! Scheduler include files.
#include "FreeRTOS.h"
#include "task.h"

//! Demo file headers.
#include "serial.h"
#include "navigation.h"
#include "supervisor.h"
#include "shell.h"
#include "cptime.h"

#include "conf_ctrlpanel.h"
#include "conf_at45dbx.h"

#if SD_MMC_SPI_MEM == ENABLE
#include "conf_sd_mmc_spi.h"
#endif


//_____ M A C R O S ________________________________________________________

//! Priority definitions for the supervisor task.
#define mainSUPERVISOR_TASK_PRIORITY   ( tskIDLE_PRIORITY + 1 )


//_____ D E F I N I T I O N S ______________________________________________

static void prv_at45dbx_resources_init( void ); // FORWARD
#if SD_MMC_SPI_MEM == ENABLE
static void prv_sd_mmc_resources_init(void); // FORWARD
#endif
static void prv_clk_gen_start(void);        // FORWARD

void vApplicationStackOverflowHook( xTaskHandle *pxTask, signed portCHAR *pcTaskName );

#if defined(__GNUC__)
const char *const pcCtrlPanelVersion = "GCC "__VERSION__" "__DATE__" "__TIME__" ATMEL_CONTROLPANEL_AVR32_UC3_10400"CRLF; // 1.04.00
#elif defined(__ICCAVR32__)
const char *const pcCtrlPanelVersion = "IAR "ASTRINGZ(__VER__)"G "__DATE__" "__TIME__" ATMEL_CONTROLPANEL_AVR32_UC3_10400"CRLF; // 1.04.00
#endif
/* xyyzz
   x = major version number (0-9)
   yy = minor version number (00-99)
   zz = minor release number (00-99)
*/


//_____ D E C L A R A T I O N S ____________________________________________


//!
//! \fn     main
//! \brief  1) Initialize the microcontroller and the shared hardware resources
//!         of the board.
//!         2) Launch the Ctrl Panel modules.
//!         3) Start FreeRTOS.
//! \return Should never occur.
//! \note
//!
int main(void)
{
	// Disable the WDT.
	// wdt_disable();

	//**
	//** 1) Initialize the microcontroller and the shared hardware resources of the board.
	//**

	// switch to external oscillator 0
	pm_switch_to_osc0(&AVR32_PM, FOSC0, OSC0_STARTUP);

	// Init USB & MACB clock.
	prv_clk_gen_start();

	// initialize AT45DBX resources: GPIO, SPI and AT45DBX
	prv_at45dbx_resources_init();

#if SD_MMC_SPI_MEM == ENABLE
	prv_sd_mmc_resources_init();
#endif

	// Setup the LED's for output.
	LED_Off( LED0 ); LED_Off( LED1 ); LED_Off( LED2 ); LED_Off( LED3 );
	LED_Off( LED4 ); LED_Off( LED5 ); LED_Off( LED6 ); LED_Off( LED7 );
	// vParTestInitialise();

	// Init the memory module.
	if (false == ctrl_access_init()) {
		// TODO: Add msg on LCD.
		// gpio_clr_gpio_pin( 60 );
	while (1);
	}

	/* check if the AT45DBX mem is OK */
	while (CTRL_GOOD != mem_test_unit_ready( LUN_ID_AT45DBX_MEM )) {
		// TODO: Add msg on LCD.
		// gpio_clr_gpio_pin( 61 );
	}

	// Init the FAT navigation module.
	if (false == b_fsaccess_init()) {
		// TODO: Add msg on LCD.
		// gpio_clr_gpio_pin( 62 );
	while (true);
	}

	// Init the time module.
	v_cptime_Init();

	//**
	//** 2) Launch the Control Panel supervisor task that will in turn create all
	//** the necessary tasks.
	//**
	vSupervisor_Start( mainSUPERVISOR_TASK_PRIORITY );

	//**
	//** 3) Start FreeRTOS.
	//**
	// Use preemptive scheduler define configUSE_PREEMPTION as 1 in portmacro.h
	vTaskStartScheduler();

	/* Should never reach this point. */
	while (true);
}
/*-----------------------------------------------------------*/


/*! \brief Initialize AT45DBX resources: GPIO, SPI and AT45DBX
 *
 */
static void prv_at45dbx_resources_init(void)
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
	if (!spi_is_enabled(AT45DBX_SPI)) {
		// Initialize as master.
		spi_initMaster(AT45DBX_SPI, &spiOptions);

		// Set selection mode: variable_ps, pcs_decode, delay.
		spi_selectionMode(AT45DBX_SPI, 0, 0, 0);

		// Enable SPI.
		spi_enable(AT45DBX_SPI);
	}

	// Initialize data flash with SPI clock Osc0.
	at45dbx_init(spiOptions, CP_PBA_SPEED);
}


#if SD_MMC_SPI_MEM == ENABLE

/*! \brief Initializes SD/MMC resources: GPIO, SPI and SD/MMC.
 */
static void prv_sd_mmc_resources_init(void)
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

	// Initialize SD/MMC with SPI clock Osc0.
	sd_mmc_spi_init(spiOptions, CP_PBA_SPEED);
}

#endif  // SD_MMC_SPI_MEM == ENABLE


/*!
 *  Start the generation of a 48-MHz clock for the USB
 */
static void prv_clk_gen_start(void)
{
	volatile avr32_pm_t *pm = &AVR32_PM;

	/** USB **/
#ifdef USB_ENABLE
	// Set PLL1 @ 96 MHz from Osc0: 12MHz*(7+1)/1 = 96MHz
	// In order to work, we need to go above 80MHz, then divide.
	pm_pll_setup(pm, 1,   // pll
			7,            // mul
			1,            // div
			0,            // osc
			16);          // lockcount

	pm_pll_set_option( pm, 1,   // pll1
			1,                  // Choose the range 80-180MHz.
			1,                  // div2
			0 );                // wbwdisable

	// Enable PLL1
	pm_pll_enable(pm, 1);

	// Wait for PLL1 locked
	pm_wait_for_pll1_locked(pm);

	// Setup USB GCLK
	pm_gc_setup(pm, AVR32_PM_GCLK_USBB, // gc
			1,                          // osc_or_pll: use Osc (if 0) or PLL (if 1)
			1,                          // pll_osc: select Osc0/PLL0 or Osc1/PLL1
			0,                          // diven
			0);                         // div

	// Enable USB GCLK
	pm_gc_enable(pm, AVR32_PM_GCLK_USBB);
#endif


	/** MACB **/
#if CP_CPU_SPEED == 60000000

	/* Setup PLL0 on OSC0, mul+1=10 ,divisor by 1, lockcount=16, ie. 12Mhzx10/1 = 120MHz output.
		Extra div by 2 => 60MHz */
	pm_pll_setup(pm,  // volatile avr32_pm_t* pm
			0,        // unsigned int pll
			9,        // unsigned int mul
			1,        // unsigned int div: Sel Osc0/PLL0 or Osc1/Pll1
			0,        // unsigned int osc
			16);      // unsigned int lockcount

	pm_pll_set_option(pm, 0,    // pll0
			1,                  // Choose the range 80-180MHz.
			1,                  // div2
			0 );                // wbwdisable

	/* Enable PLL0 */
	pm_pll_enable(pm,0);

	/* Wait for PLL0 locked. */
	pm_wait_for_pll0_locked(pm) ;
#endif

#if (CP_PBA_SPEED == 27000000) && (CP_CPU_SPEED == 54000000)
	/** Reduce speed Bus. **/
	// Set divider to 4 for PBA bus (so that PBA_SPEED == 108/2pow(sel+1) == 27),
	// to 2 for PBB and HSB (so that HSB_SPEED == 108/2pow(sel+1) == 54.
	// Note that HSB_SPEED == CPU_SPEED always.
	pm_cksel(pm,1,1,1,0,1,0);
#elif (CP_PBA_SPEED == 15000000) && (CP_CPU_SPEED == 60000000)
	/** Reduce speed Bus. **/
	// Set divider to 4 for PBA bus (so that PBA_SPEED == 60/2pow(sel+1) == 15).
	// Note that HSB_SPEED == CPU_SPEED always.
	pm_cksel(pm,1,1,0,0,0,0);
#endif

#if CP_CPU_SPEED > 33000000
	/* Add one wait state for flash management. */
	flashc_set_wait_state(1);

	/* switch to clock. */
	pm_switch_to_clock(pm, AVR32_PM_MCCTRL_MCSEL_PLL0);
#endif
}


void vApplicationStackOverflowHook( xTaskHandle *pxTask, signed portCHAR *pcTaskName )
{
	volatile int i = 10000;
	LED_On(LED0 | LED1 | LED2 | LED3 | LED4 | LED5 | LED6 | LED7);
	while (1) {
		while (i--);
		LED_Toggle(LED0 | LED1 | LED2 | LED3 | LED4 | LED5 | LED6 | LED7);
		i = 10000;
	}
}
