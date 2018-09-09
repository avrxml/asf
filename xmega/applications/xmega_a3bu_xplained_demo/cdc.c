/**
 * \file
 *
 * \brief Application CDC Implementation
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
#include "nvm.h"
#include "keyboard.h"
#include "cdc.h"

//! USBID position in user signature row
#define USER_SIGNATURE_USBID_POS   8
//! USBID size in user signature row
#define USER_SIGNATURE_USBID_SIZE 12

uint8_t cdc_serial_number[USB_DEVICE_GET_SERIAL_NAME_LENGTH + 1];

PROGMEM_DECLARE(char const, cdc_help[]) =
	"\r\n"
	"Welcome to the XMEGA-A3BU Xplained Demo CDC interface!\r\n"
	"\r\n"
	"Key bindings for LCD menu control:\r\n"
	"  Enter      : Enter\r\n"
	"  Backspace  : Back\r\n"
	"  Arrow Up   : Up\r\n"
	"  Arrow Down : Down\r\n";

bool cdc_opened = false;

static void cdc_put_help(void)
{
	const char PROGMEM_PTR_T help = cdc_help;
	char val;

	while ((val = PROGMEM_READ_BYTE(help))) {
		udi_cdc_putc(val);
		help++;
	}
}

void cdc_start(void)
{
	uint8_t i;
	uint8_t temp;
	uint8_t nibble_to_ascii[16] = "0123456789ABCDEF";

	/* Copy serial number from user signature row and convert to ASCII
	 * The 6 byte id results in a 12 byte string (1 character per nibble)
	 */
	for (i = 0; i < USER_SIGNATURE_USBID_SIZE / 2; i++) {
		temp = nvm_read_user_signature_row(
				i + USER_SIGNATURE_USBID_POS);
		// Upper nibble
		cdc_serial_number[i * 2] = nibble_to_ascii[(temp & 0xF0) >> 4];
		// Lower nibble
		cdc_serial_number[(i * 2) + 1] = nibble_to_ascii[temp & 0x0F];
	}

	udc_start();
}

void cdc_set_dtr(bool enable)
{
	cdc_opened = enable;
}

keycode_t cdc_getkey(void)
{
	char key;
	static uint8_t esc_state = 0;

	// If the cdc was opened, print out the help
	if (cdc_opened) {
		cdc_opened = false;
		cdc_put_help();
	}

	if (!udi_cdc_is_rx_ready()) {
		return KEYBOARD_NO_KEY;
	}

	key = udi_cdc_getc();

	if (esc_state == 1) {
		if (key == 0x5b) {
			esc_state = 2;
		} else {
			esc_state = 0;
		}
	} else if (esc_state == 2) {
		esc_state = 0;
		// Translate up/down arrow keys to keycodes
		switch (key) {
		case 'A':
			key = KEYBOARD_UP;
			break;
		case 'B':
			key = KEYBOARD_DOWN;
			break;
		}
	}

	// Check for escape sequence
	if (key == 0x1b) {
		esc_state = 1;
		return KEYBOARD_NO_KEY;
	}

	// Just return any other key directly
	return key;
}

