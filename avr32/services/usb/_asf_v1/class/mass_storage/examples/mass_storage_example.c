/**
 * \file
 *
 * \mainpage
 * \section title Main file of the USB mass-storage example.
 *
 * \section file File(s)
 * - \ref mass_storage_example.c
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
 ******************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


//_____  I N C L U D E S ___________________________________________________

#ifndef FREERTOS_USED
  #if (defined __GNUC__)
    #include "nlao_cpu.h"
  #endif
#else
  #include <stdio.h>
#endif
#include "compiler.h"
#include "preprocessor.h"
#include "mass_storage_example.h"
#include "board.h"
#include "print_funcs.h"
#include "intc.h"
#include "power_clocks_lib.h"
#include "gpio.h"
#include "ctrl_access.h"
#if (defined AT45DBX_MEM) && (AT45DBX_MEM == ENABLE)
  #include "spi.h"
  #include "conf_at45dbx.h"
#endif
#if (defined SD_MMC_SPI_MEM) && (SD_MMC_SPI_MEM == ENABLE)
  #include "spi.h"
  #include "conf_sd_mmc_spi.h"
#endif
#if ((defined SD_MMC_MCI_0_MEM) && (SD_MMC_MCI_0_MEM == ENABLE)) || ((defined SD_MMC_MCI_1_MEM) && (SD_MMC_MCI_1_MEM == ENABLE))
  #include "mci.h"
  #include "conf_sd_mmc_mci.h"
#endif
#ifdef FREERTOS_USED
  #include "FreeRTOS.h"
  #include "task.h"
#endif
#include "conf_usb.h"
#include "usb_task.h"
#if USB_DEVICE_FEATURE == true
  #include "device_mass_storage_task.h"
#endif
#if USB_HOST_FEATURE == true
  #include "host_mass_storage_task.h"
#endif
#include "ushell_task.h"


//_____ D E F I N I T I O N S ______________________________________________

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

/*! \name Hmatrix bus configuration
 */
void init_hmatrix(void)
{
  union
  {
    unsigned long                 scfg;
    avr32_hmatrix_scfg_t          SCFG;
  } u_avr32_hmatrix_scfg;

  // For the internal-flash HMATRIX slave, use last master as default.
  u_avr32_hmatrix_scfg.scfg = AVR32_HMATRIX.scfg[AVR32_HMATRIX_SLAVE_FLASH];
  u_avr32_hmatrix_scfg.SCFG.defmstr_type = AVR32_HMATRIX_DEFMSTR_TYPE_LAST_DEFAULT;
  AVR32_HMATRIX.scfg[AVR32_HMATRIX_SLAVE_FLASH] = u_avr32_hmatrix_scfg.scfg;
}


#ifndef FREERTOS_USED

  #if (defined __GNUC__)

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

  // Give the used CPU clock frequency to Newlib, so it can work properly.
  set_cpu_hz(pcl_freq_param.pba_f);

  // Don't-care value for GCC.
  return 1;
}

  #elif __ICCAVR32__

/*! \brief Low-level initialization routine called during startup, before the
 *         main function.
 */
int __low_level_init(void)
{

#if BOARD == UC3C_EK
	// Disable WDT At Startup
	AVR32_WDT.ctrl = 0x55000000;
	AVR32_WDT.ctrl = 0xAA000000;
#endif

  // Enable exceptions.
  Enable_global_exception();

  // Initialize interrupt handling.
  INTC_init_interrupts();

  // Request initialization of data segments.
  return 1;
}

  #endif  // Compiler

#endif  // FREERTOS_USED


#if (defined AT45DBX_MEM) && (AT45DBX_MEM == ENABLE)

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
  at45dbx_init(spiOptions, pcl_freq_param.pba_f);
}

#endif  // AT45DBX_MEM == ENABLE


#if (defined SD_MMC_SPI_MEM) && (SD_MMC_SPI_MEM == ENABLE)

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
  if (!spi_is_enabled(SD_MMC_SPI))
  {
    // Initialize as master.
    spi_initMaster(SD_MMC_SPI, &spiOptions);

    // Set selection mode: variable_ps, pcs_decode, delay.
    spi_selectionMode(SD_MMC_SPI, 0, 0, 0);

    // Enable SPI.
    spi_enable(SD_MMC_SPI);
  }

  // Initialize SD/MMC with SPI PB clock.
  sd_mmc_spi_init(spiOptions, pcl_freq_param.pba_f);
}

#endif  // SD_MMC_SPI_MEM == ENABLE


#if ((defined SD_MMC_MCI_0_MEM) && (SD_MMC_MCI_0_MEM == ENABLE)) || ((defined SD_MMC_MCI_1_MEM) && (SD_MMC_MCI_1_MEM == ENABLE))

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
  sd_mmc_mci_init(SD_SLOT_8BITS, pcl_freq_param.pba_f, pcl_freq_param.cpu_f);
}

#endif  // SD_MMC_MCI_0_MEM == ENABLE || SD_MMC_MCI_1_MEM == ENABLE


/*! \brief Main function. Execution starts here.
 *
 * \retval 42 Fatal error.
 */
int main(void)
{
  // Configure system clocks.
  if (pcl_configure_clocks(&pcl_freq_param) != PASS)
    return 42;

  // Initialize USART link.
  init_dbg_rs232(pcl_freq_param.pba_f);

#if (defined AT45DBX_MEM) && (AT45DBX_MEM == ENABLE)
  at45dbx_resources_init();
#endif
#if SD_MMC_SPI_MEM == ENABLE
  sd_mmc_spi_resources_init();
#endif
#if ((defined SD_MMC_MCI_0_MEM) && (SD_MMC_MCI_0_MEM == ENABLE)) || ((defined SD_MMC_MCI_1_MEM) && (SD_MMC_MCI_1_MEM == ENABLE))
  sd_mmc_mci_resources_init();
#endif
#ifdef FREERTOS_USED
  if (!ctrl_access_init())
  {
    portDBG_TRACE("The module CTRL_ACCESS could not be initialized.");
    return 42;
  }
#endif  // FREERTOS_USED

  // Init Hmatrix bus
  init_hmatrix();

  // Initialize USB clock.
  pcl_configure_usb_clock();

  // Initialize USB tasks.
  usb_task_init();
#if USB_DEVICE_FEATURE == true
  device_mass_storage_task_init();
#endif
#if USB_HOST_FEATURE == true
  host_mass_storage_task_init();
#endif
  ushell_task_init(pcl_freq_param.pba_f);

#ifdef FREERTOS_USED
  vTaskStartScheduler();
  portDBG_TRACE("FreeRTOS returned.");
  return 42;
#else
  while (true)
  {
    usb_task();
  #if USB_DEVICE_FEATURE == true
    device_mass_storage_task();
  #endif
  #if USB_HOST_FEATURE == true
    host_mass_storage_task();
  #endif
    ushell_task();
  }
#endif  // FREERTOS_USED
}
