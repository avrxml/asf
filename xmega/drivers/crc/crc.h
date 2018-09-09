/**
 * \file
 *
 * \brief Cyclic Redundancy Check module driver
 *
 * Copyright (c) 2011-2018 Microchip Technology Inc. and its subsidiaries.
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
#ifndef CRC_H_INCLUDED
#define CRC_H_INCLUDED

#include <compiler.h>
#include <stdint.h>
#include <stdbool.h>
#include <nvm.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup crc_group CRC module driver
 *
 * See \ref xmega_crc_quickstart.
 *
 * \brief Low-level driver implementation for the AVR XMEGA AU/B/C Cyclic
 * Redundancy Check (CRC) module.
 *
 * A cyclic redundancy check (CRC) is an error detection technique test
 * algorithm used to find accidental errors in data, and it is commonly used to
 * determine the correctness of a data transmission, and data present in the
 * data and program memories. The CRC module takes a data stream or a block of
 * data as input and generates a 16- or 32-bit output that can be appended to
 * the data and used as a checksum. When the same data are later received or
 * read, the device or application can either repeat the calculation for the
 * data with the appended checksum, which should result in the checksum zero
 * flag being set. The other option is to calculate the checksum and compare it
 * to the one calculated earlier.
 * If the new CRC result does not match the one calculated earlier or zero, the
 * block contains a data error. The application is then able to the detect this
 * and may take a corrective action, such as requesting the data to be sent
 * again or simply not using the incorrect data.
 * The CRC module in XMEGA devices supports two commonly used CRC polynomials;
 * CRC-16 (CRC-CCITT) and CRC-32 (IEEE 802.3).
 *
 */
//! @{

//! Perform a CRC calculation on an address range in flash
#define CRC_FLASH_RANGE    NVM_CMD_FLASH_RANGE_CRC_gc
//! Perform a CRC calculation on the application section
#define CRC_BOOT           NVM_CMD_BOOT_CRC_gc
//! Perform a CRC calculation on the boot loader section
#define CRC_APP            NVM_CMD_APP_CRC_gc

//! Flag to indicate whether DMA is supported
#if !(XMEGA_C3 || XMEGA_C4)
# define DMA_SUPPORTED
#endif


//! Enum to indicate whether to use CRC-16 or CRC-32
enum crc_16_32_t {
	CRC_16BIT,
	CRC_32BIT,
};

void crc_set_initial_value(uint32_t value);

void crc32_append_value(uint32_t value, void *ptr);
void crc16_append_value(uint16_t value, void *ptr);

uint32_t crc_flash_checksum(NVM_CMD_t crc_type, flash_addr_t
		flash_addr, uint32_t len);

uint32_t crc_io_checksum(void *data, uint16_t len,
		enum crc_16_32_t crc_16_32);

void crc_io_checksum_byte_start(enum crc_16_32_t crc_16_32);
void crc_io_checksum_byte_add(uint8_t data);
uint32_t crc_io_checksum_byte_stop(void);

#ifdef DMA_SUPPORTED
void crc_dma_checksum_start(uint8_t dma_channel, enum crc_16_32_t crc_16_32);
uint32_t crc_dma_checksum_stop(void);
#endif
//! @}

#ifdef __cplusplus
}
#endif

/**
 * \page xmega_crc_quickstart Quick start guide for CRC driver
 *
 * This is the quick start guide for the \ref crc_group "CRC driver", with
 * step-by-step instructions on how to configure and use the drivers in a
 * selection of use cases.
 *
 * The use cases contain several code fragments. The code fragments in the
 * steps for setup can be copied into a custom initialization function, while
 * the steps for usage can be copied into, e.g., the main application function.
 *
 * \section crc_basic_use_case Basic use case
 * In this use case, the CRC module is configured for:
 * - Performing CRC on I/O interface
 * - Using CRC-32
 *
 * \section crc_basic_use_case_setup Setup steps
 *
 * \subsection crc_basic_use_case_setup_prereq Prerequisites
 * For the setup code of this use case to work, the following must
 * be added to the project:
 * -# The common Clock service.
 * -# A uint8_t array of data with at least 5 entries to perform CRC on.
 *
 * \subsection crc_basic_use_case_setup_code Example code
 * Add to application initialization:
 * \code
	sysclk_init();
\endcode
 *
 * \subsection crc_basic_use_case_setup_flow Workflow
 * -# Initialize system clock:
 *   - \code sysclk_init(); \endcode
 *
 * \section crc_basic_use_case_usage Usage steps
 *
 * \subsection crc_basic_use_case_usage_code Example code
 * Add to application C-file:
 * \code
	uint32_t checksum;
	checksum = crc_io_checksum((void*)data, 5, CRC_32BIT);
\endcode
 *
 * \subsection crc_basic_use_case_usage_flow Workflow
 * -# Calculate the checksum of the data array and store it in the checksum
 * variable:
 *   - \code checksum = crc_io_checksum((void*)data, 5, CRC_32BIT) \endcode
 *
 * \section crc_use_cases Advanced use cases
 * For more advanced use of the CRC driver, see the following use cases:
 * - \subpage crc_use_case_1 : 
 * - \subpage crc_use_case_2 :
 */

/**
 * \page crc_use_case_1 Use case #1
 *
 * In this use case, the CRC module is configured for:
 * - Performing CRC on an address range in flash
 * - Using CRC-32
 *
 * This use case shows how to calculate the checksum for an address range in 
 * flash.
 *
 * \section crc_use_case_1_setup Setup steps
 *
 * \subsection crc_use_case_1_setup_code Example code
 * An address in flash must be defined:
 * \code
	#define FLASHADDR      0x0300
\endcode
 * Add to application initialization:
 * \code
	sysclk_init();
\endcode
 *
 * \subsection crc_use_case_1_setup_flow Workflow
 * -# Initialize system clock:
 *   - \code sysclk_init(); \endcode
 *
 * \section crc_use_case_1_usage Usage steps
 *
 * \subsection crc_use_case_1_usage_code Example code
 * \code
	uint32_t checksum;
	checksum = crc_flash_checksum(CRC_FLASH_RANGE, FLASHADDR, 4);
\endcode
 *
 * \subsection crc_use_case_1_usage_flow Workflow
 * -# Calculate the checksum of 4 bytes in flash from FLASHADDR
 *   - \code
	checksum = crc_flash_checksum(CRC_FLASH_RANGE, FLASHADDR, 4);
\endcode
 */

 /**
 * \page crc_use_case_2 Use case #2
 *
 * In this use case, the CRC module is configured for:
 * - Performing CRC on data passing through a DMA channel
 * - Using CRC-16
 *
 * This use case shows how to calculate the checksum for data passing through
 * a DMA channel.
 *
 * \section crc_use_case_2_setup Setup steps
 *
 * \subsection crc_use_case_2_setup_prereq Prerequisites
 * For the setup code of this use case to work, the following must
 * be added to the project:
 * -# PMIC driver for interrupt handling
 * -# DMA driver for enabling DMA transaction
 * -# The common Clock service.
 * -# A callback function for DMA transfer complete must be provided by the
 * user:
 * \code
	static void callback_dma_transfer_done(enum dma_channel_status status)
	{
	    uint32_t checksum;
	    checksum = crc_dma_checksum_stop();
	}
\endcode
 * -# A static uint8_t array of data with at least DMA_BUFFER_SIZE entries to
 * perform CRC on.
 *
 * \subsection crc_use_case_2_setup_code Example code
 * Some defines and data buffers must be set up:
 * \code
	//! DMA channel to use
	#define DMA_CHANNEL 0
	//! Size of data buffer to transfer
	#define DMA_BUFFER_SIZE 8
	//! Source buffer to use for DMA transfer
	static uint8_t          source[DMA_BUFFER_SIZE];
	//! Destination buffer to use for DMA transfer
	static uint8_t          destination[DMA_BUFFER_SIZE];
\endcode
 * Add to application initialization:
 * \code
	sysclk_init();
\endcode
 *
 * \subsection crc_use_case_1_setup_flow Workflow
 * -# Initialize system clock:
 *   - \code sysclk_init(); \endcode
 * 
 * \section crc_use_case_1_usage Usage steps
 *
 * \subsection crc_use_case_1_usage_code Example code
 * \code
	struct dma_channel_config config;
	uint32_t                  checksum;
	crc_dma_checksum_start(DMA_CHANNEL, CRC_16BIT);
	dma_enable();
	dma_set_callback(DMA_CHANNEL, callback_dma_transfer_done);
	memset(&config, 0, sizeof(config));
	dma_channel_set_interrupt_level(&config, PMIC_LVL_LOW);
	dma_channel_set_burst_length(&config, DMA_CH_BURSTLEN_1BYTE_gc);
	dma_channel_set_transfer_count(&config, DMA_BUFFER_SIZE);
	dma_channel_set_src_reload_mode(&config,
	        DMA_CH_SRCRELOAD_TRANSACTION_gc);
	dma_channel_set_dest_reload_mode(&config,
	        DMA_CH_DESTRELOAD_TRANSACTION_gc);
	dma_channel_set_src_dir_mode(&config, DMA_CH_SRCDIR_INC_gc);
	dma_channel_set_dest_dir_mode(&config, DMA_CH_DESTDIR_INC_gc);
	dma_channel_set_source_address(&config, (uint16_t)(uintptr_t)source);
	dma_channel_set_destination_address(&config,
	        (uint16_t)(uintptr_t)destination);
	dma_channel_write_config(DMA_CHANNEL, &config);
	dma_channel_enable(DMA_CHANNEL);
	cpu_irq_enable();
	dma_channel_trigger_block_transfer(DMA_CHANNEL);
	while (true) {
	}
\endcode
 *
 * \subsection crc_use_case_1_usage_flow Workflow
 * -# Enable the CRC module for DMA
 *   - \code
	crc_dma_checksum_start(DMA_CHANNEL, CRC_16BIT);
\endcode
 * -# Enable DMA
 *   - \code
	dma_enable();
\endcode
 * -# Set callback function for DMA completion
 *   - \code
	dma_set_callback(DMA_CHANNEL, callback_dma_transfer_done);
\endcode
 * -# Make sure config is all zeroed out so we don't get any stray bits
 *   - \code
	memset(&config, 0, sizeof(config));
\endcode
 * -# Configure the DMA channel with the following
 * settings:
 *  - Low interrupt priority
 *  - 1 byte burst length
 *  - DMA_BUFFER_SIZE bytes for each transfer
 *  - Reload source and destination address at end of each transfer
 *  - Increment source and destination address during transfer
 *  - Source address is set to source buffer
 *  - Destination address is set to destination buffer
 *   - \code
	dma_channel_set_interrupt_level(&config, PMIC_LVL_LOW);
	dma_channel_set_burst_length(&config, DMA_CH_BURSTLEN_1BYTE_gc);
	dma_channel_set_transfer_count(&config, DMA_BUFFER_SIZE);
	dma_channel_set_src_reload_mode(&config,
	        DMA_CH_SRCRELOAD_TRANSACTION_gc);
	dma_channel_set_dest_reload_mode(&config,
	        DMA_CH_DESTRELOAD_TRANSACTION_gc);
	dma_channel_set_src_dir_mode(&config, DMA_CH_SRCDIR_INC_gc);
	dma_channel_set_dest_dir_mode(&config, DMA_CH_DESTDIR_INC_gc);
	dma_channel_set_source_address(&config, (uint16_t)(uintptr_t)source);
	dma_channel_set_destination_address(&config,
	        (uint16_t)(uintptr_t)destination);
	dma_channel_write_config(DMA_CHANNEL, &config);
\endcode
 * -# Use the configuration above by enabling the DMA channel in use.
 *   - \code
	dma_channel_enable(DMA_CHANNEL);
\endcode
 * -# Enable interrupts
 *   - \code
	cpu_irq_enable();
\endcode
 * -# Trigger the DMA transfer
 *   - \code
	dma_channel_trigger_block_transfer(DMA_CHANNEL);
\endcode
 * -# Do nothing while waiting for DMA transfer to complete
 *   - \code
	while (true) {
	}
\endcode
 */

#endif /* CRC_H_INCLUDED */
