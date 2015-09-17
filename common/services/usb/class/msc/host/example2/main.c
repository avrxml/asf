/**
 * \file
 *
 * \brief Main functions for USB host mass storage example
 *
 * Copyright (C) 2012-2015 Atmel Corporation. All rights reserved.
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
#include "conf_usb_host.h"
#include "ui.h"
#include "main.h"
#include "string.h"

#define MAX_DRIVE      _VOLUMES
#define TEST_FILE_NAME "0:uhi_msc_test.txt"
#define MSG_TEST "Test UHI MSC\n"

typedef enum test_state {
	TEST_NULL,
	TEST_OK,
	TEST_NO_PRESENT,
	TEST_ERROR
} test_state_t;

static volatile uint16_t main_usb_sof_counter = 0;

static test_state_t lun_states[MAX_DRIVE];

static FATFS fs; // Re-use fs for LUNs to reduce memory footprint
static FIL file_object;

static char test_file_name[] = {
	TEST_FILE_NAME
};

static void main_reset_states(void);
static int main_count_states(test_state_t state);

/*! \brief Main function. Execution starts here.
 */
int main(void)
{
#if SAMD21 || SAML21
	system_init();
#else
	sysclk_init();
	board_init();
#endif
	irq_initialize_vectors();
	cpu_irq_enable();

	// Initialize the sleep manager
	sleepmgr_init();

	ui_init();

	// Start USB host stack
	uhc_start();

	// The USB management is entirely managed by interrupts.
	// As a consequence, the user application does only have :
	// - to play with the power modes
	// - to create a file on each new LUN connected
	while (true) {
		//sleepmgr_enter_sleep();
		if (main_usb_sof_counter > 2000) {
			main_usb_sof_counter = 0;
			volatile uint8_t lun;
			FRESULT res;

			for (lun = LUN_ID_USB; (lun < LUN_ID_USB +
					uhi_msc_mem_get_lun()) &&
					(lun < MAX_DRIVE); lun++) {
				// Check if LUN has been already tested
				if (TEST_OK == lun_states[lun] ||
						TEST_ERROR == lun_states[lun]) {
					continue;
				}
				// Mount drive
				memset(&fs, 0, sizeof(FATFS));
				res = f_mount(lun, &fs);
				if (FR_INVALID_DRIVE == res) {
					// LUN is not present
					lun_states[lun] = TEST_NO_PRESENT;
					continue;
				}
				// Create a test file on the disk
				test_file_name[0] = lun + '0';
				res = f_open(&file_object,
						(char const *)test_file_name,
						FA_CREATE_ALWAYS | FA_WRITE);
				if (res == FR_NOT_READY) {
					// LUN not ready
					lun_states[lun] = TEST_NO_PRESENT;
					f_close(&file_object);
					continue;
				}
				if (res != FR_OK) {
					// LUN test error
					lun_states[lun] = TEST_ERROR;
					f_close(&file_object);
					continue;
				}
				// Write to test file
				f_puts(MSG_TEST, &file_object);
				// LUN test OK
				lun_states[lun] = TEST_OK;
				f_close(&file_object);
			}
			if (main_count_states(TEST_NO_PRESENT) == MAX_DRIVE) {
				ui_test_finish(false); // Test fail
			} else if (MAX_DRIVE != main_count_states(TEST_NULL)) {
				if (main_count_states(TEST_ERROR)) {
					ui_test_finish(false); // Test fail
				} else if (main_count_states(TEST_OK)) {
					ui_test_flag_reset();
					ui_test_finish(true); // Test OK
				}
			} else {
				ui_test_flag_reset();
			}
		}
	}
}

void main_usb_sof_event(void)
{
	main_usb_sof_counter++;
	ui_usb_sof_event();
}

void main_usb_connection_event(uhc_device_t * dev, bool b_present)
{
	if (!b_present) {
		main_reset_states(); // LUN is unplugged, reset flags
	}
	ui_usb_connection_event(dev, b_present);
}

static void main_reset_states(void)
{
	int i;
	for (i = 0; i < MAX_DRIVE; i ++) {
		lun_states[i] = TEST_NULL;
	}
}

static int main_count_states(test_state_t state)
{
	int i, count = 0;
	for (i = 0; i < MAX_DRIVE; i ++) {
		if (lun_states[i] == state) {
			count ++;
		}
	}
	return count;
}

/**
 * \mainpage ASF USB host mass storage
 *
 * \section intro Introduction
 * This example shows how to implement a USB host mass storage
 * on Atmel MCU with USB module.
 *
 * \section startup Startup
 * After loading firmware, connect the board (EVKxx,XPlain,...) to a U-Disk
 * (FAT/FAT32 are supported). This example creates a file "uhi_msc_test.txt"
 * on all present U-disks.
 *
 * \copydoc UI
 *
 * \section example About example
 *
 * The example uses the following module groups:
 * - Basic modules:
 *   Startup, board, clock, interrupt, power management
 * - USB host stack and MSC modules:
 *   <br>services/usb/
 *   <br>services/usb/uhc/
 *   <br>services/usb/class/msc/host/
 * - Thirdparty modules:
 *   <br>thirdparty/fatfs
 * - Specific implementation:
 *    - main.c,
 *      <br>initializes clock
 *      <br>initializes interrupt
 *      <br>manages UI
 *    - specific implementation for each target "./examples/product_board/":
 *       - conf_foo.h   configuration of each module
 *       - ui.c        implement of user's interface (buttons, leds)
 */
