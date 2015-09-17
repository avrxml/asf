/**
 * @file main.c
 *
 * @brief ZID USB HID Adaptor Application
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

/**
 * \mainpage
 * \section preface Preface
 * This is the reference manual for ZID USB HID adaptor application.
 * \section main_files Application Files
 * - main.c                      Application main file.
 * - vendor_data.c               Vendor Specific API functions
 * \section intro Application Introduction
 * When ZID PC adaptor is plugged into a PC,it enumerates as a HID compliant
 * composite device(HID Compliant keyboard,HID compliant Mouse and HID compliant
 * consumer control device)and
 * a green LED on the dongle will blink. It indicates that after initialization
 * pairing procedure is in progress.
 * If it finds a remote and pairing is successful the green led will become
 * stable.
 * Once the pairing is successful the adaptor will receive ZID reports from
 * class device/ZID Remote
 * and redirect the HID reports to PC. ZID reports triggered by media player
 * keys( volume up/down/mute keys,
 * play / pause / stop /next/previous keys are handled by supporting
 * applications (for example, Windows master volume control, media player,
 * etc...) Along with media player remote, the ZID Remote application also
 * demonstrates Power point(PPT) remote
 * and Pointing and clicking functionality as mouse.
 * Warm reset,Multiple devices
 *
 * ZID PC adaptor can be used with the ZID device application.
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
 * 2)  <a href="http://www.atmel.com/design-support/">Atmel Design
 *Support</a>.\n
 *
 * \section contactinfo Contact Information
 * For further information,visit
 * <A href="http://www.atmel.com/avr">www.atmel.com</A>.\n
 */

/*
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

/* === Includes ============================================================ */

#include <stdio.h>
#include "conf_usb.h"
#include "conf_board.h"
#include <asf.h>
#include "app_config.h"
#include "led.h"
#include "delay.h"
#include "vendor_data.h"
#include "pb_pairing.h"
#include "common_sw_timer.h"
#include "zid.h"

/* === Macros ============================================================== */
#define PAIR_WAIT_PERIOD    500000
#if (LED_COUNT >= 3)
#define LED_START                       (LED0)
#define LED_NWK_SETUP                   (LED1)
#define LED_DATA                        (LED2)
#elif (LED_COUNT == 2)
#define LED_START                       (LED0)
#define LED_NWK_SETUP                   (LED0)
#define LED_DATA                        (LED1)
#else
#define LED_START                       (LED0)
#define LED_NWK_SETUP                   (LED0)
#define LED_DATA                        (LED0)
#define LED_DATA                        (LED0)
#endif
/* === TYPES =============================================================== */
typedef enum node_status_tag {
	IDLE = 0,
	RESETTING,
	POWER_SAVE,
	STARTING,
	ZID_CONNECTING,
	ALL_IN_ONE_START,
	WARM_STARTING
} SHORTENUM node_status_t;

/* === Globals ============================================================= */
static zid_indication_callback_t zid_ind;
static nwk_indication_callback_t nwk_ind;
static uint8_t number_of_paired_dev = 0;
static node_status_t node_status;

/* This is used to find out the duplicate entry
 * on receiving the pairing confirmation */
static bool duplicate_pair_entry = false;
static uint8_t target_auto_start = true;
static volatile bool main_b_kbd_enable = false;
static bool main_b_mouse_enable = false;
static uint8_t APP_TIMER;
/* Write application specific values into flash memory */
FLASH_DECLARE(uint16_t VendorIdentifier) = (uint16_t)NWKC_VENDOR_IDENTIFIER;
FLASH_DECLARE(uint8_t vendor_string[7]) = NWKC_VENDOR_STRING;
FLASH_DECLARE(uint8_t app_user_string[15]) = APP_USER_STRING;

/* === Prototypes ========================================================== */
static void zid_indication_callback_init(void);
static void nlme_unpair_indication(uint8_t PairingRef);
static void zid_heartbeat_indication(uint8_t PairingRef);
static void zid_connect_confirm(nwk_enum_t Status, uint8_t PairingRef);
static void zid_report_data_indication(uint8_t PairingRef,
		uint8_t num_report_records,
		zid_report_data_record_t *zid_report_data_record_ptr,
		uint8_t RxLinkQuality, uint8_t RxFlags);

void zid_standby_leave_indication(void);
static void nlme_reset_confirm(nwk_enum_t Status);
static void nlme_start_confirm(nwk_enum_t Status);
static void app_task(void);
static void led_handling(void *callback_parameter);
static void app_alert(void);

/* === Implementation ====================================================== */

/**
 * Main function, initialization and main message loop
 *
 */
int main(void)
{
	irq_initialize_vectors();

	/* Initialize the board.
	 * The board-specific conf_board.h file contains the configuration of
	 * the board initialization.
	 */
	sysclk_init();
	board_init();

	sw_timer_init();

	if (nwk_init() != NWK_SUCCESS) {
		app_alert();
	}

	zid_indication_callback_init();

	/*
	 * The stack is initialized above, hence the global interrupts are
	 * enabled
	 * here.
	 */
	cpu_irq_enable();
	/* Initializing udc stack as HID composite device*/
	udc_start();

	sw_timer_get_id(&APP_TIMER);

	/* Endless while loop */
	while (1) {
		app_task(); /* Application task */
		nwk_task(); /* RF4CE network layer task */
	}
}

/**
 * @brief Application task
 */
static void app_task(void)
{
	if (target_auto_start == true) {
		target_auto_start = false;
		node_status = ALL_IN_ONE_START;
		nlme_reset_request(true,
				(FUNC_PTR)nlme_reset_confirm
				);
		return;
	}
}

/**
 * @brief Notify the application of the status of its request to reset the NWK
 *        layer.
 *
 * @param Status  nwk status
 */

static void nlme_reset_confirm(nwk_enum_t Status)
{
	if (Status == NWK_SUCCESS) {
		nlme_start_request(
				(FUNC_PTR)nlme_start_confirm
				);
	} else {
		/*Something Went wrong sending nlme_reset_request again*/
		nlme_reset_request(true,
				(FUNC_PTR)nlme_reset_confirm
				);
	}
}

/**
 * @brief Notify the application of the status of its request to start the NWK.
 *
 *
 * @param Status  nwk status
 */
static void nlme_start_confirm(nwk_enum_t Status)
{
	if (Status == NWK_SUCCESS) {
		sw_timer_start(APP_TIMER,
				PAIR_WAIT_PERIOD,
				SW_TIMEOUT_RELATIVE,
				(FUNC_PTR)led_handling,
				NULL);

		dev_type_t RecDevTypeList[DEVICE_TYPE_LIST_SIZE];
		profile_id_t RecProfileIdList[PROFILE_ID_LIST_SIZE];

		RecDevTypeList[0] = (dev_type_t)SUPPORTED_DEV_TYPE_0;
		RecProfileIdList[0] = SUPPORTED_PROFILE_ID_0;

		node_status = ZID_CONNECTING;

		zid_rec_connect_request(APP_CAPABILITIES, RecDevTypeList,
				RecProfileIdList,
				(FUNC_PTR)zid_connect_confirm
				);
	}
}

/**
 * @brief Function to handle the LED States based on application state.
 *
 *
 * @param callback_parameter  callback parameter if any.
 */
static void led_handling(void *callback_parameter)
{
	switch (node_status) {
	case ZID_CONNECTING:
	case ALL_IN_ONE_START:
		sw_timer_start(APP_TIMER,
				PAIR_WAIT_PERIOD,
				SW_TIMEOUT_RELATIVE,
				(FUNC_PTR)led_handling,
				NULL);
		LED_Toggle(LED_NWK_SETUP);
		break;

	default:
		sw_timer_stop(APP_TIMER);
		break;
	}

	/* Keep compiler happy */
	callback_parameter = callback_parameter;
}

/**
 * @brief This function decides whether push button pairing request should be
 *        allowed.
 *
 * Decision could be based on one of the parameter.
 *
 * @param Status              nwk status
 * @param SrcIEEEAddr         IEEE Address of the source requesting the pair.
 * @param OrgVendorId         Vendor Id of the source requesting the pair.
 * @param OrgVendorString     Vendor string of the source requesting the pair.
 * @param OrgUserString       User string of the source requesting the pair.
 * @param KeyExTransferCount  Number of key seeds to establish key.
 *
 * @return true if pairing is granted; else false
 */
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

/**
 * @brief This function registers the callback function for indications from the
 * stack.
 *
 */
static void zid_indication_callback_init(void)
{
	zid_ind.zid_heartbeat_indication_cb = zid_heartbeat_indication;

	zid_ind.zid_report_data_indication_cb = zid_report_data_indication;
	register_zid_indication_callback(&zid_ind);
	nwk_ind.nlme_unpair_indication_cb = nlme_unpair_indication;
	zid_ind.zid_standby_leave_indication_cb = zid_standby_leave_indication;
	register_nwk_indication_callback(&nwk_ind);
}

/**
 * @brief Notify the application of the removal of link by another device.
 *
 * The NLME-UNPAIR.indication primitive allows the NLME to notify the
 * application
 * of the removal of a pairing link by another device.
 *
 * @param PairingRef       Pairing Ref for which entry is removed from pairing
 * table.
 */
static void nlme_unpair_indication(uint8_t PairingRef)
{
	number_of_paired_dev--;
}

/**
 * @brief Notify the application of the status of its heartbeat request.
 *
 *
 * @param PairingRef  Pairing reference.
 */

static void zid_heartbeat_indication(uint8_t PairingRef)
{
	PairingRef = PairingRef;
}

/**
 * @brief Notify the application of the status of its connect request.
 *
 * @param Status  nwk status.
 * @param PairingRef  Pairing reference.
 */

static void zid_connect_confirm(nwk_enum_t Status, uint8_t PairingRef)
{
	node_status = IDLE;
	if (Status == NWK_SUCCESS) {
		if (duplicate_pair_entry == false) {
			number_of_paired_dev++;
		}

		LED_On(LED0);
	}
}

/**
 * @brief Notify the application of the status of its standby request.
 *
 */
void zid_standby_leave_indication(void)
{
	LED_Off(LED0);
}

/**
 * @brief Notify the application when ZID report data is received from the
 * paired device.
 *
 * @param PairingRef Pairing reference.
 * @param num_report_records number of Report records.
 * @param *zid_report_data_record_ptr pointer to the report data received.
 * @param  RxLinkQuality    LQI value of the report data frame.
 * @param  RxFlags          Receive flags.
 */
static void zid_report_data_indication(uint8_t PairingRef,
		uint8_t num_report_records,
		zid_report_data_record_t *zid_report_data_record_ptr,
		uint8_t RxLinkQuality, uint8_t RxFlags)
{
	for (uint8_t i = 0; i < num_report_records; i++) {
		switch (zid_report_data_record_ptr->report_desc_identifier) {
		case MOUSE:
		{
			mouse_desc_t *mouse_desc;
			mouse_desc
				= (mouse_desc_t *)zid_report_data_record_ptr->
					report_data;

			udi_hid_mouse_btnleft(mouse_desc->button0);
			udi_hid_mouse_btnright(mouse_desc->button1);

			if ((0x80 == (mouse_desc->button2))) {
				udi_hid_mouse_moveScroll((mouse_desc->
						y_coordinate));
				mouse_desc->y_coordinate = 0;
			} else if (0x01 == (mouse_desc->button2)) {
				udi_hid_mouse_btnmiddle(0x01);
			}

			udi_hid_mouse_moveX((mouse_desc->x_coordinate));
			udi_hid_mouse_moveY((mouse_desc->y_coordinate));

			break;
		}

		case KEYBOARD:
		{
			if (zid_report_data_record_ptr->report_type == INPUT) {
				keyboard_input_desc_t *keyboard_input_desc;
				keyboard_input_desc
					= (keyboard_input_desc_t *)
						zid_report_data_record_ptr
						->report_data;
				if (main_b_kbd_enable) {
					if (keyboard_input_desc->modifier_keys)
					{
						udi_hid_kbd_modifier_down(
								keyboard_input_desc->modifier_keys);
						udi_hid_kbd_modifier_up(
								keyboard_input_desc->modifier_keys);
					}

					for (uint8_t j = 0; j < 4; j++) {
						if (keyboard_input_desc->
								key_code
								[j]) {
							udi_hid_kbd_down(
									keyboard_input_desc->key_code[
										j]);
							udi_hid_kbd_up(
									keyboard_input_desc->key_code[
										j]);
						}
					}
					uint16_t u_value;
					u_value = convert_byte_array_to_16_bit(
							&(
								keyboard_input_desc
								->key_code[4]));
					if (u_value) {
						udi_hid_mkbd_modifier_down(
								u_value);
						udi_hid_mkbd_modifier_up(u_value);
					}
				}
			} else {
				/* Application can implement for other report
				 * types.*/
			}

			break;
		}

		default:
			break;
		}
		zid_report_data_record_ptr++;
	}

	RxLinkQuality = RxLinkQuality;
	RxFlags = RxFlags;
}

bool main_mouse_enable(void)
{
	main_b_mouse_enable = true;
	return true;
}

void main_mouse_disable(void)
{
	main_b_mouse_enable = false;
}

bool main_kbd_enable(void)
{
	main_b_kbd_enable = true;
	return true;
}

void main_kbd_disable(void)
{
	main_b_kbd_enable = false;
}

/* Alert to indicate something has gone wrong in the application */
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
