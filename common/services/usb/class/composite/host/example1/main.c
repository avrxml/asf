/**
 * \file
 *
 * \brief Main functions for USB host mass storage and mouse composite example
 *
 * Copyright (C) 2011-2015 Atmel Corporation. All rights reserved.
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

static volatile uint16_t main_usb_sof_counter = 0;
static volatile uint8_t lun_state = 0;

#define MSG_TEST "Test UHI MSC\n"

/*! \brief Main function. Execution starts here.
 */
int main(void)
{
	sysclk_init();
	irq_initialize_vectors();
	cpu_irq_enable();

	// Initialize the sleep manager
	sleepmgr_init();

	board_init();
	ui_init();

	// Reset File System
	nav_reset();

	// Start USB host stack
	uhc_start();

	// The USB management is entirely managed by interrupts.
	// As a consequence, the user application does only have :
	// - to play with the power modes
	// - to create a file on each new LUN connected
	while (true) {
		sleepmgr_enter_sleep();
		if (main_usb_sof_counter > 2000) {
			main_usb_sof_counter = 0;
			uint8_t lun;

			for (lun = 0; (lun < uhi_msc_mem_get_lun()) && (lun < 8); lun++) {
				// Mount drive
				nav_drive_set(lun);
				if (!nav_partition_mount()) {
					if (fs_g_status == FS_ERR_HW_NO_PRESENT) {
						// The test can not be done, if LUN is not present
						lun_state &= ~(1 << lun); // LUN test reseted
						continue;
					}
					lun_state |= (1 << lun); // LUN test is done.
					ui_test_finish(false); // Test fail
					continue;
				}

				// Check if LUN has been already tested
				if (lun_state & (1 << lun)) {
					continue;
				}

				// Create a test file on the disk
				if (!nav_file_create((FS_STRING) "uhi_msc_test.txt")) {
					if (fs_g_status != FS_ERR_FILE_EXIST) {
						if (fs_g_status == FS_LUN_WP) {
							// Test can be done only on no write protected device
							continue;
						}
						lun_state |= (1 << lun); // LUN test is done.
						ui_test_finish(false); // Test fail
						continue;
					}
				}
				if (!file_open(FOPEN_MODE_APPEND)) {
					if (fs_g_status == FS_LUN_WP) {
						// Test can be done only on no write protected device
						continue;
					}
					lun_state |= (1 << lun); // LUN test is done.
					ui_test_finish(false); // Test fail
					continue;
				}
				if (!file_write_buf((uint8_t*)MSG_TEST, sizeof(MSG_TEST))) {
					lun_state |= (1 << lun); // LUN test is done.
					ui_test_finish(false); // Test fail
					continue;
				}
				file_close();
				lun_state |= (1 << lun); // LUN test is done.
				ui_test_finish(true); // Test pass
			}
			if ((lun == 0) || (lun_state == 0)) {
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
		lun_state = 0; // LUN is unplugged, reset flag
	}
	ui_usb_connection_event(dev, b_present);
}

/**
 * \mainpage ASF USB host MSC and HID mouse example
 *
 * \section intro Introduction
 * This example shows how to implement a USB host mass storage
 * and host mouse on Atmel MCU with USB module.
 *
 * \section startup Startup
 * After loading firmware, connect the board (EVKxx,Xplain,...) to a USB device
 * mouse or a U-disk (FAT/FAT32 are supported). This example creates a file
 * "uhi_msc_test.txt" on all present U-disks.
 *
 * \copydoc UI
 *
 * \section example About example
 *
 * The example uses the following module groups:
 * - Basic modules:
 *   Startup, board, clock, interrupt, power management
 * - USB host stack, MSC and HID modules:
 *   <br>services/usb/
 *   <br>services/usb/uhc/
 *   <br>services/usb/class/msc/host/
 *   <br>services/usb/class/hid/host/mouse/
 * - Specific implementation:
 *    - main.c,
 *      <br>initializes clock
 *      <br>initializes interrupt
 *      <br>manages UI
 *    - specific implementation for each target "./examples/product_board/":
 *       - conf_foo.h   configuration of each module
 *       - ui.c        implement of user's interface (buttons, leds)
 */
