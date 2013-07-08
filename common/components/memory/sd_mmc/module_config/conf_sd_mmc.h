/**
 * \file
 *
 * \brief SD/MMC stack configuration file.
 *
 * Copyright (c) 2012 Atmel Corporation. All rights reserved.
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

#ifndef CONF_SD_MMC_H_INCLUDED
#define CONF_SD_MMC_H_INCLUDED

// Define to enable the SPI mode instead of Multimedia Card interface mode
//#define SD_MMC_SPI_MODE

// Define to enable the SDIO support
#define SDIO_SUPPORT_ENABLE

// Define it to enable the debug trace to the current standard output (stdio)
//#define SD_MMC_DEBUG

/*! \name board MCI SD/MMC slot template definition
 *
 * The GPIO and MCI/HSMCI connections of the SD/MMC Connector must be added
 * in board.h file.
 * Also do not forget to add the initialization of the GPIO in the board init.c
 * See below an example:
 */
//! @{
#if UC3
#  ifndef SD_MMC_MCI_MEM_CNT
#    warning The GPIO and MCI connections of the SD/MMC connector must\
     be added in the board.h file.
	// Example from EVK1104
#    define SD_MMC_MCI_MEM_CNT            1
/* Optional card detect pin and write protection pin
#    define SD_MMC_0_CD_GPIO              AVR32_PIN_PB08
#    define SD_MMC_0_CD_DETECT_VALUE      0
#    define SD_MMC_0_WP_GPIO              AVR32_PIN_PB06
#    define SD_MMC_0_WP_DETECT_VALUE      1
*/
#    define SD_MMC_MCI_SLOT_0_SIZE        4 // 4-bits connector pin
#    define SD_MMC_MCI_SLOT_0_DATA0_PIN   SD_SLOT_4BITS_DATA0_PIN
#    define SD_SLOT_4BITS                 1
#    define SD_SLOT_4BITS_CLK_PIN         AVR32_MCI_CLK_0_PIN
#    define SD_SLOT_4BITS_CLK_FUNCTION    AVR32_MCI_CLK_0_FUNCTION
#    define SD_SLOT_4BITS_CMD_PIN         AVR32_MCI_CMD_1_0_PIN
#    define SD_SLOT_4BITS_CMD_FUNCTION    AVR32_MCI_CMD_1_0_FUNCTION
#    define SD_SLOT_4BITS_DATA0_PIN       AVR32_MCI_DATA_8_0_PIN
#    define SD_SLOT_4BITS_DATA0_FUNCTION  AVR32_MCI_DATA_8_0_FUNCTION
#    define SD_SLOT_4BITS_DATA1_PIN       AVR32_MCI_DATA_9_0_PIN
#    define SD_SLOT_4BITS_DATA1_FUNCTION  AVR32_MCI_DATA_9_0_FUNCTION
#    define SD_SLOT_4BITS_DATA2_PIN       AVR32_MCI_DATA_10_0_PIN
#    define SD_SLOT_4BITS_DATA2_FUNCTION  AVR32_MCI_DATA_10_0_FUNCTION
#    define SD_SLOT_4BITS_DATA3_PIN       AVR32_MCI_DATA_11_0_PIN
#    define SD_SLOT_4BITS_DATA3_FUNCTION  AVR32_MCI_DATA_11_0_FUNCTION
#  endif
#endif
#if SAM
#  ifndef SD_MMC_HSMCI_MEM_CNT
#    warning The GPIO and HSMCI connections of the SD/MMC connector must\
     be added in the board.h file.
	// Example from SAM3S-EK
#    define SD_MMC_HSMCI_MEM_CNT          1
/* Optional card detect pin and write protection pin
#    define SD_MMC_0_CD_GPIO              (PIO_PA6_IDX)
#    define SD_MMC_0_CD_PIO_ID            ID_PIOA
#    define SD_MMC_0_CD_FLAGS             (PIO_INPUT | PIO_PULLUP)
#    define SD_MMC_0_CD_DETECT_VALUE      0
*/
#    define SD_MMC_HSMCI_SLOT_0_SIZE      4 // 4-bits connector pin
#    define PIN_HSMCI_MCCDA_GPIO          (PIO_PA28_IDX)
#    define PIN_HSMCI_MCCDA_FLAGS         (PIO_PERIPH_C | PIO_DEFAULT)
#    define PIN_HSMCI_MCCK_GPIO           (PIO_PA29_IDX)
#    define PIN_HSMCI_MCCK_FLAGS          (PIO_PERIPH_C | PIO_DEFAULT)
#    define PIN_HSMCI_MCDA0_GPIO          (PIO_PA30_IDX)
#    define PIN_HSMCI_MCDA0_FLAGS         (PIO_PERIPH_C | PIO_DEFAULT)
#    define PIN_HSMCI_MCDA1_GPIO          (PIO_PA31_IDX)
#    define PIN_HSMCI_MCDA1_FLAGS         (PIO_PERIPH_C | PIO_DEFAULT)
#    define PIN_HSMCI_MCDA2_GPIO          (PIO_PA26_IDX)
#    define PIN_HSMCI_MCDA2_FLAGS         (PIO_PERIPH_C | PIO_DEFAULT)
#    define PIN_HSMCI_MCDA3_GPIO          (PIO_PA27_IDX)
#    define PIN_HSMCI_MCDA3_FLAGS         (PIO_PERIPH_C | PIO_DEFAULT)
#  endif
#endif
//! @}

#endif /* CONF_SD_MMC_H_INCLUDED */

