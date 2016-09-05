/**
 *
 * \file
 *
 * \brief Image Downloader Example.
 *
 * Copyright (c) 2016 Atmel Corporation. All rights reserved.
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

/** \mainpage
 * \section intro Introduction
 * This example demonstrates how to program the WINC1500 internal flash in
 * order to update the firmware and root certificates.<br>
 * It uses the following hardware:
 * - the SAM Xplained Pro.
 * - the WINC1500 on EXT1.
 *
 * \section files Main Files
 * - main.c : Initialize the WINC1500 and perform flash operation.
 *
 * -# Build the program and download it into the board.
 * -# On the computer, open and configure a terminal application as following.
 * \code
 *    Baud Rate : 115200
 *    Data : 8bit
 *    Parity bit : none
 *    Stop bit : 1bit
 *    Flow control : none
 * \endcode
 *
 * -# Start the application.
 * -# In the terminal window, the following text should appear:<br>
 *
 * \code
 *    -- Image downloader example --
 *    -- SAMXXX_XPLAINED_PRO --
 *    -- Compiled: xxx xx xxxx xx:xx:xx --
 *    
 *    (APP)(INFO)Chip ID xxxxxx
 *    Flash ID xxxxxxxx
 *    (APP)(INFO)Flash Size x Mb
 *    
 *    >Start erasing...
 *    Done
 *    main: programming firmware...
 *    main: verifying firmware image from flash...
 *    main: done.
 *    main: programming certificates...
 *    
 *    ...
 *    
 *    >Root certificate verified OK.
 *    
 *    >Start erasing...
 *    Done
 *    >Writing the certificate to SPI flash...
 *    Done
 *    main: reading certificates from flash...
 *    main: done.
 *    
 *    
 *    All task completed successfully.
 * \endcode
 *
 * \section compinfo Compilation Information
 * This software was written for the GNU GCC compiler using Atmel Studio 6.2
 * Other compilers may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com">Atmel</A>.\n
 */

#include <string.h>
#include "common/include/nm_common.h"
#include "driver/include/m2m_wifi.h"
#include "driver/include/m2m_periph.h"
#include "driver/include/m2m_ota.h"
#include "programmer/programmer.h"
#include "root_cert/root_setup.h"
#include "stdio_serial.h"
#include "binaries/DigiCert_Root.h"
#include "binaries/DigiCertSHA2_Root.h"
#include "binaries/GeoTrustGlobalCA_Root.h"
#include "binaries/Radius_Root.h"
#include "binaries/VeriSign_Root.h"

/** Select one of below firmware image (2b0 is Rev A, 3a0 is Rev B). */
#include "binaries/m2m_aio_3a0.h"
//#include "binaries/m2m_aio_2b0.bin"

#define STRING_EOL    "\r\n"
#define STRING_HEADER "-- Image downloader example --"STRING_EOL \
	"-- "BOARD_NAME " --"STRING_EOL	\
	"-- Compiled: "__DATE__ " "__TIME__ " --"STRING_EOL

/**
 * \brief Configure UART console.
 */
static void configure_console(void)
{
	const usart_serial_options_t uart_serial_options = {
		.baudrate =		CONF_UART_BAUDRATE,
		.charlength =	CONF_UART_CHAR_LENGTH,
		.paritytype =	CONF_UART_PARITY,
		.stopbits =		CONF_UART_STOP_BITS,
	};

	/* Configure UART console. */
	sysclk_enable_peripheral_clock(CONSOLE_UART_ID);
	stdio_serial_init(CONF_UART, &uart_serial_options);
}

/**
 * \brief Program root certificates to WINC1500 memory.
 *
 * \param certFiles String array containing file names of root
 * certificates to program.
 * \param numOfCert Number of root certificates to program.
 *
 * \return M2M_SUCCESS on success, error code otherwise.
 */
static sint8 burn_certificates(void)
{
	sint8 ret = 0;

	/* Write certificate file to WINC1500 memory. */
	ret += WriteRootCertificate((char *)"DigiCert_Root", (char *)DigiCert_Root, DigiCert_Root_size); //eg: facebook.com
	ret += WriteRootCertificate((char *)"DigiCertSHA2_Root", (char *)DigiCertSHA2_Root, DigiCertSHA2_Root_size); //eg: facebook.com SHA256
	ret += WriteRootCertificate((char *)"GeoTrustGlobalCA_Root", (char *)GeoTrustGlobalCA_Root, GeoTrustGlobalCA_Root_size); //eg:google.com
	ret += WriteRootCertificate((char *)"Radius_Root", (char *)Radius_Root, Radius_Root_size);
	ret += WriteRootCertificate((char *)"VeriSign_Root", (char *)VeriSign_Root, VeriSign_Root_size); //eg:amazon.com

	return ret;
}

/**
 * \brief Dump WINC1500 certificates memory section to a file on
 * SD/MMC card.
 *
 * \return M2M_SUCCESS on success, error code otherwise.
 */
static sint8 dump_certificate_section(void)
{
	uint8 flash_content[FLASH_SECTOR_SZ] = {0};

	/* Dump entire root certificate memory region. */
	if (programmer_read_cert_image(flash_content) != M2M_SUCCESS) {
		printf("dump_certificate_section: read access failed on certificate section!\r\n");
		return M2M_ERR_FAIL;
	}

	return M2M_SUCCESS;
}

/**
 * \brief Program firmware to WINC1500 memory.
 *
 * \return M2M_SUCCESS on success, error code otherwise.
 */
static sint8 burn_firmware(void)
{
	uint32 u32FileSize = 0;
	uint32 offset = 0;
	uint32 copy = 0;
	u32FileSize = winc_fw_size;
	uint8	tmp_buffer[FLASH_SECTOR_SZ] = {0};

	while (u32FileSize) {
		/* Check how many data can be copied in this block. */	
		if (u32FileSize > FLASH_SECTOR_SZ) {
			copy = FLASH_SECTOR_SZ;
		}
		else {
			copy = u32FileSize;
		}
		memcpy(tmp_buffer, &winc_fw[offset], copy);
		/* Copy firmware binary to the WINC1500 memory. */
		if (programmer_write_firmware_image(tmp_buffer, offset, copy) != M2M_SUCCESS) {
			printf("burn_firmware: write error!\r\n");
			return M2M_ERR_FAIL;
		}
		u32FileSize -= copy;
		offset += copy;
	}

	return M2M_SUCCESS;
}

static sint8 dump_firmware_section(void)
{
	uint8 flash_content[FLASH_SECTOR_SZ] = {0};
	uint32 offset = 0;

	if (programmer_read_firmware_image(flash_content, offset, FLASH_SECTOR_SZ) != M2M_SUCCESS) {
		printf("dump_firmware_section: read access failed on firmware section!\r\n");
		return M2M_ERR_FAIL;
	}
	for (uint32 i = 0; i < FLASH_SECTOR_SZ; ++i) {
		if (flash_content[i] != winc_fw[i]) {
			printf("dump_firmware_section: verification failed!\r\n");
			return M2M_ERR_FAIL;
		}
	}

	return M2M_SUCCESS;
}

/**
 * \brief Main application function.
 *
 * Application entry point.
 *
 * \return program return value.
 */
int main(void)
{
	/* Initialize the board. */
	sysclk_init();
	board_init();

	/* Initialize the UART console. */
	configure_console();

	/* Output example information */
	puts(STRING_HEADER);

	/* Initialize BSP and Bus Interface */
	nm_bsp_init();

	/* Enter download mode. */
	if (m2m_wifi_download_mode() != M2M_SUCCESS) {
		printf("main: failed to enter download mode!\r\n");
		while (1) {
		}
	}

	/* Erase the WINC1500 flash. */
	if (programmer_erase_all() != M2M_SUCCESS) {
		printf("main: failed to erase entire flash!\r\n");
		while (1) {
		}
	}

	/* Program and verify the firmware on the WINC1500 flash. */
	printf("main: programming firmware...\r\n");
	if (burn_firmware() != M2M_SUCCESS) {
		printf("main: error while writing firmware!\r\n");
		while (1);
	}
	printf("main: verifying firmware image from flash...\r\n");
	if (dump_firmware_section() != M2M_SUCCESS) {
		printf("main: failed to verify firmware section!\r\n");
		while (1);
	}
	printf("main: done.\r\n");
	
	/* Program and read the certificates on the WINC1500 flash. */
	printf("main: programming certificates...\r\n");
	if (burn_certificates() != M2M_SUCCESS) {
		printf("main: error while writing certificates!\r\n");
		while (1);
	}
	printf("main: reading certificates from flash...\r\n");
	if (dump_certificate_section() != M2M_SUCCESS) {
		printf("main: failed to dump certificate section!\r\n");
		while (1);
	}
	printf("main: done.\r\n");

	printf("\r\n\r\nAll task completed successfully.\r\n");

	while (1) {
	}
}
