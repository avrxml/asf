/**
 * @file main.c
 *
 * @brief ZID Device application
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
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

/* === INCLUDES ============================================================ */
#include <asf.h>
#include "conf_board.h"
#include "led.h"
#include "common_sw_timer.h"
#include "keyboard.h"
#include "app_config.h"
#include "rf4ce.h"
#include "vendor_data.h"
#include "pb_pairing.h"
#include "zid.h"
#include "zid_device.h"

/* === TYPES =============================================================== */

typedef enum node_status_tag {
	IDLE,
	WARM_START,
	COLD_START,
	CONFIGURING_ATTRIBUTES,
	CONNECTING,
	TRANSMITTING
} SHORTENUM node_status_t;

/* === MACROS ============================================================== */

#define INTER_FRAME_DURATION_US     200000 /* 200 ms */
#define TX_OPTIONS  (TXO_UNICAST | TXO_DST_ADDR_NET | \
	TXO_ACK_REQ | TXO_SEC_REQ | TXO_MULTI_CH | \
	TXO_CH_NOT_SPEC | TXO_VEND_NOT_SPEC)

/* === GLOBALS ============================================================= */

FLASH_DECLARE(uint16_t VendorIdentifier) = (uint16_t)NWKC_VENDOR_IDENTIFIER;
FLASH_DECLARE(uint8_t vendor_string[7]) = NWKC_VENDOR_STRING;
FLASH_DECLARE(uint8_t app_user_string[15]) = APP_USER_STRING;

static node_status_t node_status;
static uint8_t pairing_ref = 0xFF;
#if (defined  RF4CE_CALLBACK_PARAM)
static zid_indication_callback_t zid_ind;

#endif
static uint8_t report_id = 0;
static bool loop_end = false;
static uint8_t set_attribute_index = 0;
static void app_alert(void);

/* === PROTOTYPES ========================================================== */

static void app_task(void);
static void extended_delay_ms(uint16_t delay_ms);
static void indicate_fault_behavior(void);
static key_state_t key_state_read(key_id_t key_no);

#ifdef RF4CE_CALLBACK_PARAM
static void nlme_reset_confirm(nwk_enum_t Status);
static void nlme_start_confirm(nwk_enum_t Status);
static void app_nlme_rx_enable_confirm(nwk_enum_t Status);
static void zid_connect_confirm(nwk_enum_t Status, uint8_t PairingRef);
static void zid_report_data_confirm(nwk_enum_t Status, uint8_t PairingRef);
static void zid_data_confirm(nwk_enum_t Status, uint8_t PairingRef);
static void zid_report_data_indication(uint8_t PairingRef,
		uint8_t num_report_records,
		zid_report_data_record_t *zid_report_data_record_ptr,
		uint8_t RxLinkQuality, uint8_t RxFlags);
static void zid_get_report_indication(uint8_t PairingRef,
		zid_report_types_t zid_report_type,
		zid_report_desc_t zid_report_desc,
		uint8_t RxLinkQuality, uint8_t RxFlags);

static void zid_set_attribute_confirm(nwk_enum_t status, uint8_t PairingRef,
		zid_attribute_t ZIDAttribute,
		uint8_t ZIDAttributeIndex);

#endif

/* === IMPLEMENTATION ====================================================== */

/**
 * \mainpage
 * \section preface Preface
 * This is the reference manual for ZID device demo
 * application.
 * \section main_files Application Files
 * - main.c                     Application main file.
 * - vendor_data.c               Vendor Specific API functions
 * \section intro Application Introduction
 *  ZID Class device is the RF4CE demo application which can be used in
 * the ZID adaptor - Class device setup
 *  This will support push button pairing procedure and zid reports i.e Sending
 * the Standard HID reports(Keyboard,Mouse,Touch sensor etc...)  to the remote
 * terminal adaptor over the air.
 *
 *	Application supports cold reset and warm reset. While powering on the
 * device, if the Select key is  pressed then it does cold reset.
 *  Otherwise it does warm reset i.e retrieving the network information base
 * from NVM.
 *
 *  If the Select key is pressed on power on, it does the push pairing procedure
 * following the cold reset.
 *
 *  The Application will use the ZID reports to send hid reports to
 * paired device.
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

/**
 * @brief Main function of the ZID Device application
 */
int main(void)
{
	irq_initialize_vectors();

	/* Initialize the board.
	 * The board-specific conf_board.h file contains the configuration of
	 * the board initialization.
	 */
	board_init();
	sysclk_init();

	sw_timer_init();

	if (nwk_init() != NWK_SUCCESS) {
		app_alert();
	}

#ifdef RF4CE_CALLBACK_PARAM
	zid_ind.zid_report_data_indication_cb = zid_report_data_indication;
	zid_ind.zid_get_report_indication_cb = zid_get_report_indication;
	register_zid_indication_callback(&zid_ind);
#endif

	/*
	 * The stack is initialized above,
	 * hence the global interrupts are enabled here.
	 */
	cpu_irq_enable();

	key_state_t key_state = key_state_read(SELECT_KEY);
	/* For debugging: Force button press */
	/* button = BUTTON_PRESSED; */
	if (key_state == KEY_PRESSED) {
		/* Force push button pairing */
		/* Cold start */
		LED_On(LED0);
		node_status = COLD_START;
		nlme_reset_request(true
#ifdef RF4CE_CALLBACK_PARAM
				, (FUNC_PTR)nlme_reset_confirm
#endif
				);
	} else {
		/* Warm start */
		node_status = WARM_START;
		nlme_reset_request(false
#ifdef RF4CE_CALLBACK_PARAM
				, (FUNC_PTR)nlme_reset_confirm
#endif
				);
	}

	/* Endless while loop */
	while (1) {
		app_task(); /* Application task */
		nwk_task(); /* RF4CE network layer task */
	}
}

/*
 * The NLME-RESET.confirm primitive allows the NLME to notify the application of
 * the status of its request to reset the NWK layer.
 */
#ifdef RF4CE_CALLBACK_PARAM
static
#endif
void nlme_reset_confirm(nwk_enum_t Status)
{
	if (Status != NWK_SUCCESS) {
		while (1) {
			/* endless while loop! */
			indicate_fault_behavior();
		}
	}

	if (node_status == COLD_START) {
		pairing_ref = 0xFF;
		nlme_start_request(
#ifdef RF4CE_CALLBACK_PARAM
				(FUNC_PTR)nlme_start_confirm
#endif

				);
	} else { /* warm start */
		pairing_ref = 0;
		/* Set power save mode: sleep */
		nlme_rx_enable_request(0 /*nwkcMinActivePeriod*/
#ifdef RF4CE_CALLBACK_PARAM
				, (FUNC_PTR)app_nlme_rx_enable_confirm
#endif

				);
	}
}

/*
 * The NLME-START.confirm primitive allows the NLME to notify the application of
 * the status of its request to start a network.
 */
#ifdef RF4CE_CALLBACK_PARAM
static
#endif
void nlme_start_confirm(nwk_enum_t Status)
{
	if (Status != NWK_SUCCESS) {
		app_alert();
	}

	if (node_status == COLD_START) {
		uint8_t value = 10;
		zid_set_attribute_request(0xFF, aplHIDNumStdDescComps, 0, &value
#ifdef RF4CE_CALLBACK_PARAM
				, (FUNC_PTR)zid_set_attribute_confirm
#endif
				);
		node_status = CONFIGURING_ATTRIBUTES;
	}
}

static void zid_set_attribute_confirm(nwk_enum_t status, uint8_t PairingRef,
		zid_attribute_t ZIDAttribute,
		uint8_t ZIDAttributeIndex)
{
	if (status == NWK_SUCCESS) {
		if (ZIDAttribute == aplHIDStdDescCompsList) {
			if (ZIDAttributeIndex >= 9) {
				set_attribute_index = 0;
				LED_Off(LED0);

				node_status = CONNECTING;

				dev_type_t OrgDevTypeList[1];
				profile_id_t OrgProfileIdList[1];
				profile_id_t DiscProfileIdList[1];

				OrgDevTypeList[0] = DEV_TYPE_REMOTE_CONTROL;
				OrgProfileIdList[0] = PROFILE_ID_ZID;
				DiscProfileIdList[0] = PROFILE_ID_ZID;

				zid_org_connect_request(APP_CAPABILITIES,
						OrgDevTypeList,
						OrgProfileIdList,
						DEV_TYPE_WILDCARD,
						NUM_SUPPORTED_PROFILES, DiscProfileIdList
#ifdef RF4CE_CALLBACK_PARAM
						, (FUNC_PTR)zid_connect_confirm
#endif
						);
			} else {
				set_attribute_index++;
			}
		} else {
			/* node_status = IDLE; */
		}
	} else {
		app_alert();
	}

	PairingRef = PairingRef;
	ZIDAttribute = ZIDAttribute;
	ZIDAttributeIndex = ZIDAttributeIndex;
}

/**
 * @brief Push button pairing confirm; target and controller use
 *
 * The push button pairing confirm is a callback that provides the status of the
 * push pairing request.
 *
 * @param Status        Status of the last pairing transaction
 * @param PairingRef    If pairing was successful, PairingRef contains assigned
 *                      pairing reference.
 */
#ifdef RF4CE_CALLBACK_PARAM
static
#endif
void zid_connect_confirm(nwk_enum_t Status, uint8_t PairingRef)
{
	if (Status != NWK_SUCCESS) {
		app_alert();
	}

	pairing_ref = PairingRef;

	/* Set power save mode */
	/* nlme_rx_enable_request(0x00/ *nwkcMinActivePeriod* /); */

	if (Status != NWK_SUCCESS) {
		while (1) {
			indicate_fault_behavior();
		}
	}

	if (node_status == CONNECTING) {
		node_status = IDLE;

		/* LED handling */
		LED_On(LED0);
		extended_delay_ms(1000);
		LED_Off(LED0);
	}
}

#ifdef RF4CE_CALLBACK_PARAM
static
#endif
void zid_report_data_indication(uint8_t PairingRef, uint8_t num_report_records,
		zid_report_data_record_t *zid_report_data_record_ptr,
		uint8_t RxLinkQuality, uint8_t RxFlags)
{
	PairingRef = PairingRef;
	num_report_records = num_report_records;
	zid_report_data_record_ptr = zid_report_data_record_ptr;
	RxLinkQuality = RxLinkQuality;
	RxFlags = RxFlags;
}

#if 1

/*
 * The NLME-RX-ENABLE.confirm primitive reports the results of the attempt to
 * enable or disable the receiver.
 */
#ifdef RF4CE_CALLBACK_PARAM
static
#endif
void app_nlme_rx_enable_confirm(nwk_enum_t Status)
{
	if (Status != NWK_SUCCESS) {
		while (1) {
			indicate_fault_behavior();
		}
	}

	if (node_status == COLD_START) {
		node_status = IDLE;

		/* LED handling */
		LED_On(LED0);
		extended_delay_ms(1000);
		LED_Off(LED0);
	} else if (node_status == WARM_START) {
		node_status = IDLE;

		LED_On(LED0);
		extended_delay_ms(250);
		LED_Off(LED0);
	}
}

#endif

key_state_t key_state = KEY_RELEASED;

/**
 * @brief Application task and state machine
 */
static void app_task(void)
{
	switch (node_status) {
	case IDLE:
	{
		/* button_state_t button = BUTTON_OFF; */
		static uint32_t current_time;
		static uint32_t previous_button_time;
		uint8_t num_records = 1;

		key_state = key_state_read(SELECT_KEY);
		if ((key_state == KEY_PRESSED) || (loop_end == false)) {
			loop_end = false;
			/* Check time to previous transmission. */

			current_time = sw_timer_get_time();
			if ((current_time - previous_button_time) <
					INTER_FRAME_DURATION_US) {
				return;
			} else {
				/* Store current time */
				previous_button_time = current_time;
			}

			zid_report_data_record_t zid_report_data[2];
			uint8_t report_data_buffer[80];
			uint8_t *msg_ptr = &report_data_buffer[0];
			if (report_id == 0) {
				/* Sending Media player Open Command */
				zid_report_data[0].report_type = INPUT;
				zid_report_data[0].report_desc_identifier
					= KEYBOARD;
				zid_report_data[0].report_data
					= (void *)msg_ptr;

				keyboard_input_desc_t *keyboard_input_desc;
				keyboard_input_desc
					= (keyboard_input_desc_t *)msg_ptr;

				keyboard_input_desc->modifier_keys = 0x00;
				keyboard_input_desc->key_code[0] = 0x00;
				keyboard_input_desc->key_code[1] = 0x00;
				keyboard_input_desc->key_code[2] = 0x00;
				keyboard_input_desc->key_code[3] = 0x00;
				keyboard_input_desc->key_code[4] = 0x08;
				keyboard_input_desc->key_code[5] = 0x00;
				num_records = 1;
			} else if (report_id == 1) {
				/* sending Media player Play command. */
				zid_report_data[0].report_type = INPUT;
				zid_report_data[0].report_desc_identifier
					= KEYBOARD;
				zid_report_data[0].report_data
					= (void *)msg_ptr;

				keyboard_input_desc_t *keyboard_input_desc;
				keyboard_input_desc
					= (keyboard_input_desc_t *)msg_ptr;

				keyboard_input_desc->modifier_keys = 0x00;
				keyboard_input_desc->key_code[0] = 0x00;
				keyboard_input_desc->key_code[1] = 0x00;
				keyboard_input_desc->key_code[2] = 0x00;
				keyboard_input_desc->key_code[3] = 0x00;
				keyboard_input_desc->key_code[4] = 0x80;
				keyboard_input_desc->key_code[5] = 0x00;
				num_records = 1;
			} else if (report_id == 2) {
				/* Sending Media Player Stop command. */
				zid_report_data[0].report_type = INPUT;
				zid_report_data[0].report_desc_identifier
					= KEYBOARD;
				zid_report_data[0].report_data
					= (void *)msg_ptr;

				keyboard_input_desc_t *keyboard_input_desc;
				keyboard_input_desc
					= (keyboard_input_desc_t *)msg_ptr;

				keyboard_input_desc->modifier_keys = 0x00;
				keyboard_input_desc->key_code[0] = 0x00;
				keyboard_input_desc->key_code[1] = 0x00;
				keyboard_input_desc->key_code[2] = 0x00;
				keyboard_input_desc->key_code[3] = 0x00;
				keyboard_input_desc->key_code[4] = 0x10;
				keyboard_input_desc->key_code[5] = 0x00;
				num_records = 1;
			} else if (report_id == 3) {
				/* sending pinch_gesture & scroll_gesture */
				zid_report_data[0].report_type = INPUT;
				zid_report_data[0].report_desc_identifier
					= SCROLL_GESTURE;
				zid_report_data[0].report_data
					= (void *)msg_ptr;

				scroll_gesture_report_t *scroll_gesture_report;
				scroll_gesture_report
					= (scroll_gesture_report_t *)
						msg_ptr;

				scroll_gesture_report->type = 0x1D;
				scroll_gesture_report->finger_count = 0x6;
				scroll_gesture_report->direction = 0x7;
				scroll_gesture_report->distance = 0x1D1;

				msg_ptr += sizeof(scroll_gesture_report_t);

				zid_report_data[1].report_type = INPUT;
				zid_report_data[1].report_desc_identifier
					= PINCH_GESTURE;
				zid_report_data[1].report_data
					= (void *)msg_ptr;

				pinch_gesture_report_t *pinch_gesture_report;
				pinch_gesture_report
					= (pinch_gesture_report_t *)
						msg_ptr;

				pinch_gesture_report->finger_present = 0x1;
				pinch_gesture_report->direction = 0x1;
				pinch_gesture_report->distance = 0x2D2;
				pinch_gesture_report->center_x = 0x3D3;
				pinch_gesture_report->center_y = 0x4D4;

				num_records = 2;
			} else if (report_id == 4) {
				/* sending rotate_gesture & sync */
				zid_report_data[0].report_type = INPUT;
				zid_report_data[0].report_desc_identifier
					= ROTATE_GESTURE;
				zid_report_data[0].report_data
					= (void *)msg_ptr;

				rotation_gesture_report_t *
				rotation_gesture_report;
				rotation_gesture_report
					= (rotation_gesture_report_t
						*)msg_ptr;

				rotation_gesture_report->finger_present = 0x1;
				rotation_gesture_report->direction = 0x1;
				rotation_gesture_report->magnitude = 0xE1;

				msg_ptr += sizeof(rotation_gesture_report_t);

				zid_report_data[1].report_type = INPUT;
				zid_report_data[1].report_desc_identifier
					= SYNC;
				zid_report_data[1].report_data
					= (void *)msg_ptr;

				sync_report_t *sync_report;
				sync_report = (sync_report_t *)msg_ptr;

				sync_report->gesture = 0x1;
				sync_report->contact_count = 0xE;

				num_records = 2;
			} else if (report_id == 5) {
				/* sending touch & tap properties */

				zid_report_data[0].report_type = INPUT;
				zid_report_data[0].report_desc_identifier
					= TOUCH_SENSOR_PROPERTIES;
				zid_report_data[0].report_data
					= (void *)msg_ptr;

				touch_sensor_properties_t *
				touch_sensor_properties;
				touch_sensor_properties
					= (touch_sensor_properties_t
						*)msg_ptr;

				touch_sensor_properties->
				no_of_additional_contacts
					= 0xF;
				touch_sensor_properties->origin = 0x2;
				touch_sensor_properties->reliable_index = 0x1;
				touch_sensor_properties->gestures = 0x1;
				touch_sensor_properties->resolution_x = 0xF1;
				touch_sensor_properties->resolution_y = 0xF2;
				touch_sensor_properties->max_coordinate_x
					= 0x3F3;
				touch_sensor_properties->max_coordinate_y
					= 0x4F4;
				touch_sensor_properties->shape = 0x5;

				msg_ptr += sizeof(touch_sensor_properties_t);

				zid_report_data[1].report_type = INPUT;
				zid_report_data[1].report_desc_identifier
					= TAP_SUPPORT_PROPERTIES;
				zid_report_data[1].report_data
					= (void *)msg_ptr;

				tap_support_properties_t *tap_support_properties;
				tap_support_properties
					= (tap_support_properties_t
						*)msg_ptr;

				tap_support_properties->single_tap = 0x1;
				tap_support_properties->tap_and_a_half = 0x1;
				tap_support_properties->double_tap = 0x1;
				tap_support_properties->long_tap = 0x1;

				num_records = 2;
				loop_end = true;
			}

			if (zid_report_data_request(pairing_ref, num_records,
					zid_report_data, TX_OPTIONS
  #ifdef RF4CE_CALLBACK_PARAM
					, (FUNC_PTR)zid_report_data_confirm
  #endif
					)) {
				node_status = TRANSMITTING;
			}
		} else { /* (button == BUTTON_OFF) */
			if (nwk_stack_idle()) {
				/* Set MCU to sleep */
				/* pal_sleep_mode(SLEEP_MODE_PWR_SAVE); */
				/* MCU is awake again */
			}
		}
	}
	break;

	case CONFIGURING_ATTRIBUTES:
	{
		uint8_t value[10]
			= {MOUSE, KEYBOARD, CONTACT_DATA, TAP_GESTURE,
			   SCROLL_GESTURE, PINCH_GESTURE,
			   ROTATE_GESTURE, SYNC,
			   TOUCH_SENSOR_PROPERTIES,
			   TAP_SUPPORT_PROPERTIES};
		zid_set_attribute_request(0xFF, aplHIDStdDescCompsList,
				set_attribute_index,
				&value[set_attribute_index]
#ifdef RF4CE_CALLBACK_PARAM
				, (FUNC_PTR)zid_set_attribute_confirm
#endif
				);
	}

	default:
		break;
	}
}

/**
 * @brief Confirmation status to the application for its report data request.
 * @param Status nwk status
 * @param PairingRef Pairing reference.
 */
#ifdef RF4CE_CALLBACK_PARAM
static
#endif
void zid_report_data_confirm(nwk_enum_t Status, uint8_t PairingRef)
{
	node_status = IDLE;

	if (report_id < 5) {
		report_id++;
	} else {
		report_id = 0;
	}

	if (Status == NWK_SUCCESS) {
		LED_Off(LED0);
	} else {
		indicate_fault_behavior();
	}

	/* Keep compiler happy. */
	PairingRef = PairingRef;
}

/**
 * @brief Confirmation status to the application for its ZID data request.
 * @param Status nwk status
 * @param PairingRef Pairing reference.
 */
#ifdef RF4CE_CALLBACK_PARAM
static
#endif
void zid_data_confirm(nwk_enum_t Status, uint8_t PairingRef)
{
	node_status = IDLE;

	if (Status == NWK_SUCCESS) {
		LED_Off(LED0);
	} else {
		indicate_fault_behavior();
	}

	/* Keep compiler happy. */
	PairingRef = PairingRef;
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
static void zid_get_report_indication(uint8_t PairingRef,
		zid_report_types_t zid_report_type,
		zid_report_desc_t zid_report_desc,
		uint8_t RxLinkQuality, uint8_t RxFlags)
{
	PairingRef = PairingRef;
	RxLinkQuality = RxLinkQuality;
	zid_report_type = zid_report_type;
	zid_report_desc = zid_report_desc;
	RxFlags = RxFlags;

	/* zid_report_data_request should be used to respond to this indication
	**/
	/* num_report_data_records will be one */
	mouse_desc_t mouse_desc;
	zid_report_data_record_t zid_report_data;
	/* zid_report_data_record_t *zid_report_data_ptr[1]; */
	zid_report_data.report_type = INPUT;
	zid_report_data.report_desc_identifier = MOUSE;
	zid_report_data.report_data = (void *)&mouse_desc;
	mouse_desc.button0 = true;
	mouse_desc.button1 = true;
	mouse_desc.button2 = true;
	mouse_desc.x_coordinate = 0x11;
	mouse_desc.y_coordinate = 0x22;
	if (node_status == IDLE) {
		if (zid_report_data_request(pairing_ref, 1, &zid_report_data,
				TX_OPTIONS
#ifdef RF4CE_CALLBACK_PARAM
				, (FUNC_PTR)zid_data_confirm
#endif
				)) {
			node_status = TRANSMITTING;
		}
	}
}

/*
 * @brief The NLDE-DATA.confirm primitive is generated by the NWK layer entity
 * in
 * response to an NLDE-DATA.request primitive.
 * @param Status nwk status
 * @param PairimgRef Pairing reference.
 * @param ProfileId Profile Identifier.
 */
void nlde_data_confirm(nwk_enum_t Status, uint8_t PairingRef,
		profile_id_t ProfileId)
{
	node_status = IDLE;

	if (Status == NWK_SUCCESS) {
		LED_Off(LED0);
	} else {
		indicate_fault_behavior();
	}

	/* Keep compiler happy. */
	PairingRef = PairingRef;
	ProfileId = ProfileId;
}

/**
 * @brief This function checks the Adaptor compatibility while pairing decides
 * whether
 * push button pairing request should be allowed.
 *
 * Decision could be based on one of the parameter.
 *
 * @param PairingRef Pairing reference.
 * @param payload_length Length of the Payload
 * @param *payload Pointer to the Payload Adaptor attributes can be extracted
 * from the payload.
 *
 * @return true if pairing is granted; else false
 */
bool check_zid_adaptor_compatibility(uint8_t PairingRef, uint8_t payload_length,
		uint8_t *payload)
{
	/*Application need to find out the compatibility with the adaptor
	 * It needs to extract the adaptor attributes from the following payload
	 * Payload format is as per the GET_ATTRIBUTES_RESPONSE packet format
	 * excluding the header
	 * payload[0] = attr_id...........*/
	PairingRef = PairingRef;
	payload_length = payload_length;
	payload = payload;
	return true;
}

/* --- Helper functions ---------------------------------------------------- */

/**
 * @brief Extended blocking delay
 *
 * @param delay_ms Delay value in ms
 */
static void extended_delay_ms(uint16_t delay_ms)
{
	uint16_t i;
	uint16_t timer_delay;

	timer_delay = delay_ms / 50;
	for (i = 0; i < timer_delay; i++) {
		delay_ms(5);
	}
}

/**
 * @brief Indicating malfunction
 */
static void indicate_fault_behavior(void)
{
	uint8_t i;

	for (i = 0; i < 10; i++) {
		LED_On(LED0);
		extended_delay_ms(200);
		LED_Off(LED0);
	}
}

/**
 * @brief Vendor-spefic callback; handles reaction to incoming alive request
 */
void vendor_app_alive_req(void)
{
	/* Variant to demonstrate FOTA feature */

	LED_On(LED0);
	delay_ms(500);
	LED_Off(LED0);
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

/**
 * @brief Read key_state
 *
 * @param key_no Keyid to be read.
 */
static key_state_t key_state_read(key_id_t key_no)
{
	key_state_t key_val = KEY_RELEASED;

	if (SELECT_KEY == key_no) {
		if (!ioport_get_pin_level(GPIO_PUSH_BUTTON_0)) {
			key_val = KEY_PRESSED;
		}
	}

	return key_val;
}

/* EOF */
