/**
 * \file
 *
 * \brief SD/MMC card access through FatFs
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
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

#include <asf.h>
#include <string.h>
#include "conf_app_display.h"
#include "app_microsd.h"

/**
 * \ingroup microsd_task_group
 * \defgroup microsd_task_group_implementation microSD task implementation
 * @{
 */

/* ! Handle on the File System */
static FATFS app_microsd_fs;
/* ! Handle on the open file */
static FIL app_microsd_file;
/* ! String which contains the inf file required to install the USB CDC driver */
PROGMEM_DECLARE(uint8_t, app_microsd_inf_file[]) =
#    include "atmel_devices_cdc_inf.h"
;

#if defined(__ICCAVR__)
/* ! String which contains the inf file required to install the USB CDC driver
 * Note: It is not possible with AVR GCC compiler to add this flash array,
 * because this increases CODE size above 64KB.
 * With AVR GCC compiler, any functions stored above the 64KB can not be
 * accessible using a pointer.
 */
PROGMEM_DECLARE(uint8_t, app_microsd_readme_file[]) =
#    include "readme_mht.h"
;
#endif

/* ! State of the microSD task (disabled when USB is enabled) */
static bool app_microsd_task_running = true;
/* ! Signal that a microSD initialization has been done */
static bool app_microsd_init_done = false;
/* ! Signal that the microSD is ready to save data */
static bool app_microsd_ready = false;

static void app_microsd_display_on(void);
static void app_microsd_display_off(void);
static void app_microsd_display_failed(void);
static bool app_microsd_install(void);
static void app_microsd_f_puts_code(uint8_t PROGMEM_PTR_T str, FIL* fil);

void app_microsd_display_access(bool on_going)
{
	if (!app_microsd_task_running) {
		return;
	}

	gfx_mono_draw_rect(DISPLAY_MICROSD_STA_POS_X,
			DISPLAY_MICROSD_STA_POS_Y,
			DISPLAY_MICROSD_STA_SIZE_X,
			DISPLAY_MICROSD_STA_SIZE_Y,
			on_going ? GFX_PIXEL_SET : GFX_PIXEL_CLR);
}

/**
 * \brief Displays on the microSD icon
 */
static void app_microsd_display_on(void)
{
	gfx_mono_draw_string(DISPLAY_MICROSD_STA_TEXT,
			DISPLAY_MICROSD_STA_TEXT_POS_X,
			DISPLAY_MICROSD_STA_TEXT_POS_Y,
			&sysfont);
}

/**
 * \brief Displays off the microSD icon
 */
static void app_microsd_display_off(void)
{
	gfx_mono_draw_filled_rect(DISPLAY_MICROSD_STA_POS_X,
			DISPLAY_MICROSD_STA_POS_Y,
			DISPLAY_MICROSD_STA_SIZE_X,
			DISPLAY_MICROSD_STA_SIZE_Y,
			GFX_PIXEL_CLR);
}

/**
 * \brief Displays on the microSD icon with a cross
 */
static void app_microsd_display_failed(void)
{
	gfx_mono_draw_line(DISPLAY_MICROSD_STA_POS_X + 1,
			DISPLAY_MICROSD_STA_POS_Y + 1,
			DISPLAY_MICROSD_STA_POS_X + DISPLAY_MICROSD_STA_SIZE_X - 1,
			DISPLAY_MICROSD_STA_POS_Y + DISPLAY_MICROSD_STA_SIZE_Y - 1,
			GFX_PIXEL_SET);
	gfx_mono_draw_line(DISPLAY_MICROSD_STA_POS_X + 1,
			DISPLAY_MICROSD_STA_POS_Y + DISPLAY_MICROSD_STA_SIZE_Y - 1,
			DISPLAY_MICROSD_STA_POS_X + DISPLAY_MICROSD_STA_SIZE_X - 1,
			DISPLAY_MICROSD_STA_POS_Y + 1,
			GFX_PIXEL_SET);
}

void app_microsd_start(void)
{
	app_microsd_task_running = true;
}

void app_microsd_stop(void)
{
	app_microsd_display_off();
	app_microsd_task_running = false;
	app_microsd_init_done = false;
	if (!app_microsd_ready) {
		return;
	}

	app_microsd_ready = false;
	/* Stop record on MicroSD */
	app_microsd_write_file("End of log\r\n");
	f_close(&app_microsd_file);
}

void app_microsd_task(void)
{
	if (!app_microsd_task_running) {
		return;
	}

	/* Check card status */
	switch (sd_mmc_test_unit_ready(0)) {
	case CTRL_NO_PRESENT:
	case CTRL_BUSY:
		if (!app_microsd_init_done) {
			return; /* Status and display already reseted */
		}

		app_microsd_init_done = false;
		app_microsd_ready = false;
		app_microsd_display_off();
		return;

	case CTRL_GOOD:
		if (!app_microsd_init_done) {
			/* Start install */
			app_microsd_display_on();
			app_microsd_init_done = true;
			if (!app_microsd_install()) {
				app_microsd_display_failed();
				printf("MicroSD - FS failure\r\n");
				return;
			}
		}

		break;

	default:
		if (app_microsd_init_done) {
			return; /* Initialization already done */
		}

		app_microsd_display_on();
		app_microsd_display_failed();
		printf("MicroSD - SD Hardware failure\r\n");
		app_microsd_init_done = true;
		return;
	}

	/* Initialization done */
	if (!app_microsd_ready) {
		return; /* Install failed */
	}

	/* Check if the file record stop is requested through SW1 */
	if (!ioport_pin_is_low(GPIO_PUSH_BUTTON_1)) {
		return; /* No stop requested */
	}

	/* Stop record on MicroSD */
	app_microsd_write_file("End of log\r\n");
	f_close(&app_microsd_file);
	app_microsd_ready = false;
	printf("MicroSD can be removed\r\n");
	app_microsd_display_off();
}

/**
 * \brief Install the microSD memory and prepare it
 * The function:
 * - mount file system
 * - copy the inf file on memory
 * - create and open a log file
 */
static bool app_microsd_install(void)
{
	FRESULT res;

	printf("Micro SD initialization...\r\n");

	/* Mount disk */
	memset(&app_microsd_fs, 0, sizeof(FATFS));
	res = f_mount(LUN_ID_SD_MMC_0_MEM, &app_microsd_fs);
	if (FR_INVALID_DRIVE == res) {
		return false;
	}

	/* Record inf file */
	char test_file_inf_name[] = "0:atmel_devices_cdc.inf";
	test_file_inf_name[0] = LUN_ID_SD_MMC_0_MEM + '0';
	res = f_open(&app_microsd_file,
			test_file_inf_name,
			FA_CREATE_NEW | FA_WRITE);
	if (res == FR_OK) {
		/* Fill file */
		app_microsd_f_puts_code(app_microsd_inf_file, &app_microsd_file);
		f_close(&app_microsd_file);
	}

#if defined(__ICCAVR__)
	/* Record readme file */
	char test_file_readme_name[] = "0:readme.mht";
	test_file_readme_name[0] = LUN_ID_SD_MMC_0_MEM + '0';
	res = f_open(&app_microsd_file,
			test_file_readme_name,
			FA_CREATE_NEW | FA_WRITE);
	if (res == FR_OK) {
		/* Fill file */
		app_microsd_f_puts_code(app_microsd_readme_file, &app_microsd_file);
		f_close(&app_microsd_file);
	}
#endif

	/* Create & Open file */
	char test_file_name[] = "0:dat_log_c3_xplained_00.txt";
	test_file_name[0] = LUN_ID_SD_MMC_0_MEM + '0';

	while (1) {
		res = f_open(&app_microsd_file,
				(char const *)test_file_name,
				FA_CREATE_NEW | FA_WRITE);
		if (res != FR_EXIST) {
			break;
		}

		if (test_file_name[23] == '9') {
			if (test_file_name[22] == '9') {
				return false; /* Too many file on disk */
			}

			test_file_name[23] = '0';
			test_file_name[22]++;
		} else {
			test_file_name[23]++;
		}
	}
	if (res != FR_OK) {
		return false;
	}

	/* Fill header file */
	app_microsd_write_file("--Data logs--\r\n");
	app_microsd_write_file("timestamp - seconds - °C\r\n");

	/* Notify install successfull */
	app_microsd_ready = true;
	printf("MicroSD [OK]\r\n");
	return true;
}

/**
 * \brief Put a string storage in FLASH to the file  
 *
 * \param str  Pointer to the string to be output
 * \param fil  Pointer to the file object
 */
static void app_microsd_f_puts_code(uint8_t PROGMEM_PTR_T str, FIL* fil)
{
#define BUF_TMP 512
	uint8_t buf_tmp[BUF_TMP];
	UINT btw;		/* Number of bytes to write */
	UINT bw;			/* Pointer to number of bytes written */

	while(PROGMEM_READ_BYTE(str)) {
		/* Fill internal buffer FLASH to RAM */
		btw = 0;
		while(PROGMEM_READ_BYTE(str) && (btw != BUF_TMP)) {
			buf_tmp[btw++] = PROGMEM_READ_BYTE(str);
			str++;
		}
		/* Write internal buffer FLASH to file */
		if (FR_OK != f_write(fil, buf_tmp, btw, &bw)) {
			return;
		}
	}
}

bool app_microsd_is_ready(void)
{
	return app_microsd_ready;
}

bool app_microsd_write_file(const TCHAR *str)
{
	if (0 == f_puts(str, &app_microsd_file)) {
		f_close(&app_microsd_file);
		return false;
	}

	return true;
}

/* ! @} */
