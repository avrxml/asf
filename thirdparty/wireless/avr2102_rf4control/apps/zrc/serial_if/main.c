/**
 * @file main.c
 *
 * @brief RF4CE Serial Interface Application
 *
 * Copyright (c) 2013-2018 Microchip Technology Inc. and its subsidiaries.
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

/**
 * \mainpage
 * \section preface Preface
 * This is the reference manual for RF4CE ZRC Serial Interface Application
 * \section main_files Application Files
 * - main.c                      Application main file.
 * \section intro Application Introduction
 * RF4Control ZRC Serial Interface Application running on the network
 * co-processor can be driven by the host controller which is running the main
 * application. Both controller use a serial interface to communicate.
 *
 * The host controller can be implemented as a standalone microcontroller, or it
 * can also be a personal computer.
 *
 * The network co-processor receives the commands from the host, such as
 * reset/pairing/data requests. The network co-processor will send the response
 * to the host controller after processing the request and also
 * indicates the data received from the prired RF4Control nodes.
 *
 * Interface between the host controller and the RF4CE client can be any
 * interface. like UART, USB, TWI, Proprietary interface
 *
 * In this application serial interface is used for communication, whereas
 * serial_data_handler api will handle all the incoming bytes from the serial
 * interface.
 * \section api_modules Application Dependent Modules
 * - \ref group_rf4control
 * - \subpage api
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC and IAR .
 * Other compilers may or may not work.
 *
 * \section references References
 * 1)  IEEE Std 802.15.4-2006 Part 15.4: Wireless Medium Access Control (MAC)
 *     and Physical Layer (PHY) Specifications for Low-Rate Wireless Personal
 * Area
 *     Networks (WPANs).\n\n
 * 2)  <a href="https://www.microchip.com/support/">Atmel Design
 *Support</a>.\n
 *
 * \section contactinfo Contact Information
 * For further information,visit
 * <A href="http://www.microchip.com/design-centers/8-bit">www.microchip.com</A>.\n
 */

/* === Includes ============================================================ */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <asf.h>
#include "conf_board.h"
#include "common_sw_timer.h"
#include "serial_interface.h"
#include "app_config.h"
#include "pb_pairing.h"
#include "nwk_config.h"

/* === Macros ============================================================== */

/* === Globals ============================================================= */

/* Write application specific values into flash memory */
FLASH_DECLARE(uint16_t VendorIdentifier) = (uint16_t)NWKC_VENDOR_IDENTIFIER;
FLASH_DECLARE(uint8_t vendor_string[7]) = NWKC_VENDOR_STRING;
FLASH_DECLARE(uint8_t app_user_string[15]) = APP_USER_STRING;
#ifdef ZRC_CMD_DISCOVERY
FLASH_DECLARE(uint8_t supported_cec_cmds[32]) = SUPPORTED_CEC_CMDS;
#endif

/* === EXTERNALS =========================================================== */

extern void stack_indication_callback_init(void);

/* === Prototypes ========================================================== */
static void app_alert(void);

/* === Implementation ====================================================== */

/**
 * Main function, initialization and main message loop
 *
 * @return error code
 */
int main(void)
{
	irq_initialize_vectors();
#if SAMD21 || SAMD20 || SAMR21
	system_init();
	delay_init();
#else
	sysclk_init();

	/* Initialize the board.
	 * The board-specific conf_board.h file contains the configuration of
	 * the board initialization.
	 */
	board_init();
#endif

	sw_timer_init();

	if (nwk_init() != NWK_SUCCESS) {
		app_alert();
	}

	stack_indication_callback_init();

#ifdef FLASH_NVRAM
	pal_ps_set(EE_IEEE_ADDR, 8, &tal_pib.IeeeAddress);
#endif
	/* Initialize LEDs */
	/* pal_led_init(); */
	cpu_irq_enable();

	/*
	 * The global interrupt has to be enabled here as TAL uses the timer
	 * delay which in turn requires interrupt to be enabled
	 */
	/* pal_global_irq_enable(); */

	serial_interface_init();

	/* Loop forever, the interrupts are doing the rest */
	while (1) {
		nwk_task();
		serial_data_handler();
	}
	/* No return statement here, because this code is unreachable */
}

#ifdef PBP_REC
bool pbp_allow_pairing(nwk_enum_t Status, uint64_t SrcIEEEAddr,
		uint16_t OrgVendorId,
		uint8_t OrgVendorString[7], uint8_t OrgUserString[15],
		uint8_t KeyExTransferCount)
{
	/* Keep compiler happy */
	Status = Status;
	SrcIEEEAddr = SrcIEEEAddr;
	OrgVendorId = OrgVendorId;
	OrgVendorString[0] = OrgVendorString[0];
	OrgUserString[0] = OrgUserString[0];
	KeyExTransferCount = KeyExTransferCount;

	return true;
}

#endif
static void app_alert(void)
{
	while (1) {
		#if LED_COUNT > 0
		LED_Toggle(LED0);
		#endif

		#if LED_COUNT > 1
		LED_Toggle(LED1);
		#endif

		#if LED_COUNT > 2
		LED_Toggle(LED2);
		#endif

		#if LED_COUNT > 3
		LED_Toggle(LED3);
		#endif

		#if LED_COUNT > 4
		LED_Toggle(LED4);
		#endif

		#if LED_COUNT > 5
		LED_Toggle(LED5);
		#endif

		#if LED_COUNT > 6
		LED_Toggle(LED6);
		#endif

		#if LED_COUNT > 7
		LED_Toggle(LED7);
		#endif
		delay_us(0xFFFF);
	}
}

/* EOF */
