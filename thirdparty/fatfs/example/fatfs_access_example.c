/**
 * \file
 *
 * \brief FatFS example.
 *
 * Copyright (c) 2012-2015 Atmel Corporation. All rights reserved.
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

/**
 *  \mainpage FatFS Example
 *
 *  \section Purpose
 *
 * The FatFS example will help you to get familiar with filesystem and the ctrl
 * access modules provided by the asf. It provides sample code of common
 * operations of a filesystem through the abstraction memory based on ctrl
 * access services. It can also help you to configure the filesystem according
 * to your own needs, which can be used for fast implementation of your own
 * filesystem and other applications related.
 * FatFs is a generic file system module to implement the FAT file system to
 * small embedded systems.
 *
 *  \section Requirements
 *
 * This package can be used with all MCU boards with
 * \ref group_common_services_storage_ctrl_access support. The conf_access.h
 * used in the ctrl access module will determine the actually used memory
 * device, e.g., the file system can be created in the internal RAM, NAND Flash 
 * and DataFlash (an AT45DBX needs to be connected) on SAM3X-EK/SAM3N-EK/
 * SAM3S-EK/SAM3U-EK/SAM4E-EK/SAM4S-EK/SAM4S-Xplained/ATxmega128A1-Xplained/
 * ATxmegaA3BU-Xplained/SAM4N-Xplained-Pro/SAM4CMP16C-SAM4CMP-DB/
 * SAM4CMS16C-SAM4CMS-DB/SAMV71-Xplained-Ultra/SAME70-Xplained. The file system 
 * is created in the Micro SD/MMC on IO1 extension board connected to SAM D/R/L 
 * Xplained Pro.
 *
 *  \section Description
 *
 * When launched, this program will check the existing file system on the
 * abstraction layer of memory through the ctrl access service. The abstraction
 * layer of memory can be configured by the conf_access.h. There are 2 cases:
 *   - The memory is not yet formatted. The program formats it with FAT file
 *     system.
 *   - The memory is already formatted. The program will read the content of
 *     the memory and re-format the memory with FAT file system.
 *
 * Then a Basic.bin file will be created, written with a special pattern and
 * closed.
 * Finally the file will be reopened and its data will be verified.
 *
 *  \section Usage
 *
 *  -# Build the program and download it into the evaluation board.
 *  -# On the computer, open and configure a terminal application
 *     (e.g., HyperTerminal on Microsoft Windows) with these settings:
 *    - 115200 bauds (or 38400 bauds on SAM D/R/L Xplained Pro)
 *    - 8 bits of data
 *    - No parity
 *    - 1 stop bit
 *    - No flow control
 *  -# In the terminal window, the
 *     following text should appear (values depend on the board and the chip
 * used):
 *     \code
	-- FatFS Example --
	-- xxxxxx-xx
	-- Compiled: xxx xx xxxx xx:xx:xx --
	-I- Mount disk 0
	-I- xxxxxx
\endcode
 *  -# The application will output the converted value to hyperterminal and
 * display a menu for users to set different modes.
 *
 */

#include <asf.h>
#include "memories_initialization.h"

/** Size of the file to write/read. */
#define DATA_SIZE 2048

/** Test settings: Number of bytes to test */
#define TEST_SIZE   (4 * 1024)

/** Example header */
#define STRING_EOL    "\r"
#ifndef BOARD_NAME
# define BOARD_NAME "Undefined"
#endif
#define STRING_HEADER "-- FatFS Example --\r\n" \
	"-- "BOARD_NAME " --\r\n" \
	"-- Compiled: "__DATE__ " "__TIME__ " --"STRING_EOL
#define MENU_HEADER "\n\r" \
	"---------------------------------------------------------\n\r"

#if SAM0
/* Structure for UART module connected to EDBG (used for unit test output) */
struct usart_module cdc_uart_module;
#endif

/* Read/write buffer */
static uint8_t data_buffer[DATA_SIZE];

/**
 * \brief Scan files under a certain path.
 *
 * \param path Folder path.
 *
 * \return Scan result, 1: success.
 */
static FRESULT scan_files(char *path)
{
	FRESULT res;
	FILINFO fno;
	DIR dir;
	int32_t i;
	char *pc_fn;
#if _USE_LFN
	char c_lfn[_MAX_LFN + 1];
	fno.lfname = c_lfn;
	fno.lfsize = sizeof(c_lfn);
#endif

	/* Open the directory */
	res = f_opendir(&dir, path);
	if (res == FR_OK) {
		i = strlen(path);
		for (;;) {
			res = f_readdir(&dir, &fno);
			if (res != FR_OK || fno.fname[0] == 0) {
				break;
			}

#if _USE_LFN
			pc_fn = *fno.lfname ? fno.lfname : fno.fname;
#else
			pc_fn = fno.fname;
#endif
			if (*pc_fn == '.') {
				continue;
			}

			/* Check if it is a directory type */
			if (fno.fattrib & AM_DIR) {
				sprintf(&path[i], "/%s", pc_fn);
				res = scan_files(path);
				if (res != FR_OK) {
					break;
				}

				path[i] = 0;
			} else {
				printf("%s/%s\n\r", path, pc_fn);
			}
		}
	}

	return res;
}

/**
 * \brief Do file system tests.
 *
 * \param disk_dev_num disk number
 *
 * \return Test result, 1: success.
 */
static uint8_t run_fatfs_test(uint32_t disk_dev_num)
{
	uint32_t i;
	UINT byte_to_read;
	UINT byte_read;
#if _FS_READONLY == 0
	UINT byte_written;
#endif

	FRESULT res;
	DIR dirs;
	TCHAR root_directory[3] = "0:";
	/* File name to be validated */
	TCHAR file_name[12] = "0:Basic.bin";

	/* Declare these as static to avoid stack usage.
	 * They each contain an array of maximum sector size.
	 */
	static FATFS fs;
	static FIL file_object;

	root_directory[0] = '0' + disk_dev_num;
	file_name[0] = '0' + disk_dev_num;

	/* Mount disk*/
	printf("-I- Mount disk %d\n\r", (int)disk_dev_num);
	/* Clear file system object */
	memset(&fs, 0, sizeof(FATFS));
	res = f_mount(disk_dev_num, &fs);
	if (res != FR_OK) {
		printf("-E- f_mount pb: 0x%X\n\r", res);
		return 0;
	}

	/* Test if the disk is formatted */
	res = f_opendir(&dirs, root_directory);
	if (res == FR_OK) {
		/* Erase sd card to reformat it ? */
		puts("-I- The disk is already formatted.\r");

		/* Display the file tree */
		puts("-I- Display files contained in the memory :\r");
		strcpy((char *)data_buffer, root_directory);
		scan_files((char *)data_buffer);

#if _FS_TINY == 0
		puts("-I- The disk will be re-formatted.\r");

		res = FR_NO_FILESYSTEM;
#endif
	}

	if (res == FR_NO_FILESYSTEM) {
#if _FS_READONLY == 0
		/* Format disk */
		printf("-I- Format disk %d\n\r", (int)disk_dev_num);
		puts("-I- Please wait a moment during formatting...\r");
		res = f_mkfs(disk_dev_num, /* Drv */
				0, /* FDISK partition */
				512); /* AllocSize */
		puts("-I- Disk format finished !\r");
		if (res != FR_OK) {
			printf("-E- f_mkfs pb: 0x%X\n\r", res);
			return 0;
		}

#else
		puts("-I- Please run Full version FAT FS test first\r");
		return 0;
#endif
	} else if (FR_OK != res) {
		printf("-E- f_opendir pb: 0x%X\n\r", res);
		return 0;
	}

#if _FS_READONLY == 0
	/* Create a new file */
	printf("-I- Create a file : \"%s\"\n\r", file_name);
	res = f_open(&file_object, (char const *)file_name,
			FA_CREATE_ALWAYS | FA_WRITE);
	if (res != FR_OK) {
		printf("-E- f_open create pb: 0x%X\n\r", res);
		return 0;
	}

	/* Write a checkerboard pattern in the buffer */
	for (i = 0; i < sizeof(data_buffer); i++) {
		if ((i & 1) == 0) {
			data_buffer[i] = (i & 0x55);
		} else {
			data_buffer[i] = (i & 0xAA);
		}
	}
	puts("-I- Write file\r");
	for (i = 0; i < TEST_SIZE; i += DATA_SIZE) {
		res = f_write(&file_object, data_buffer, DATA_SIZE,
				&byte_written);

		if (res != FR_OK) {
			printf("-E- f_write pb: 0x%X\n\r", res);
			return 0;
		}
	}

	/* Close the file */
	puts("-I- Close file\r");
	res = f_close(&file_object);
	if (res != FR_OK) {
		printf("-E- f_close pb: 0x%X\n\r", res);
		return 0;
	}
#endif

	/* Open the file */
	printf("-I- Open the same file : \"%s\"\n\r", file_name);
	res = f_open(&file_object, (char const *)file_name,
			FA_OPEN_EXISTING | FA_READ);
	if (res != FR_OK) {
		printf("-E- f_open read pb: 0x%X\n\r", res);
		return 0;
	}

	/* Read file */
	puts("-I- Read file\r");
	memset(data_buffer, 0, DATA_SIZE);
	byte_to_read = file_object.fsize;

	for (i = 0; i < byte_to_read; i += DATA_SIZE) {
		res = f_read(&file_object, data_buffer, DATA_SIZE, &byte_read);
		if (res != FR_OK) {
			printf("-E- f_read pb: 0x%X\n\r", res);
			return 0;
		}
	}

	/* Close the file*/
	puts("-I- Close file\r");
	res = f_close(&file_object);
	if (res != FR_OK) {
		printf("-E- f_close pb: 0x%X\n\r", res);
		return 0;
	}

	/* Compare the read data with the expected data */
	for (i = 0; i < sizeof(data_buffer); i++) {
		if (!((((i & 1) == 0) && (data_buffer[i] == (i & 0x55))) ||
				(data_buffer[i] == (i & 0xAA)))) {
			printf(
					"Invalid data at data[%u] (expected 0x%02X, read 0x%02X)\n\r",
					(unsigned int)i,
					(unsigned int)(((i & 1) == 0) ? (i & 0x55) : (i & 0xAA)),
					(unsigned int)data_buffer[i]);
		}
	}
	puts("-I- File data Ok !\r");

	return 1;
}

/**
 *  \brief Application entry point for FatFS example.
 */
int main(void)
{
	uint32_t disk_dev_num;
#if SAM0
	system_init();
	struct usart_config usart_conf;
	usart_get_config_defaults(&usart_conf);
	usart_conf.mux_setting = CONF_STDIO_MUX_SETTING;
	usart_conf.pinmux_pad0 = CONF_STDIO_PINMUX_PAD0;
	usart_conf.pinmux_pad1 = CONF_STDIO_PINMUX_PAD1;
	usart_conf.pinmux_pad2 = CONF_STDIO_PINMUX_PAD2;
	usart_conf.pinmux_pad3 = CONF_STDIO_PINMUX_PAD3;
	usart_conf.baudrate    = CONF_STDIO_BAUDRATE;
	stdio_serial_init(&cdc_uart_module, CONF_STDIO_USART, &usart_conf);
	usart_enable(&cdc_uart_module);
#else
	const usart_serial_options_t usart_serial_options = {
#  if !SAM
		.baudrate   = CONF_TEST_BAUDRATE,
		.paritytype = CONF_TEST_PARITY,
		.charlength = CONF_TEST_CHARLENGTH,
		.stopbits   = CONF_TEST_STOPBITS,
#else
		.baudrate = CONF_TEST_BAUDRATE,
#ifdef CONF_TEST_CHARLENGTH
		.charlength = CONF_TEST_CHARLENGTH,
#endif
		.paritytype = CONF_TEST_PARITY,
#ifdef CONF_TEST_STOPBITS
		.stopbits = CONF_TEST_STOPBITS,
#endif
#  endif
	};
	/* Initialize the system */
	sysclk_init();
	board_init();

#  if XMEGA
	rtc_init();
#  endif

#  if SAM
	sysclk_enable_peripheral_clock(CONSOLE_UART_ID);
#  endif
	stdio_serial_init(CONF_TEST_USART, &usart_serial_options);
#endif

	/* Intialize the memory device */
	memories_initialization();

	/* Output example information */
	puts(STRING_HEADER);

	for (disk_dev_num = 0; disk_dev_num < get_nb_lun(); disk_dev_num++) {
		if (run_fatfs_test(disk_dev_num)) {
			printf("-I- DISK %d Test passed !\n\r\n\r", (int)disk_dev_num);
		} else {
			printf("-F- DISK %d Test Failed !\n\r\n\r", (int)disk_dev_num);
		}
	}

	while (1) {
		/* Do nothing */
	}
}
