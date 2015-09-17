/**
 * @file main.c
 *
 * @brief ZID HID Qtouch Remote Application
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

/**
 * \mainpage
 * \section preface Preface
 * This application demonstrates the use of  AVR477 remote control as ZID class
 * device,
 * Which pairs with the ZID HID PC adaptor and controls the PC applications like
 * Windows Media player.
 * \section main_files Application Files
 * - main.c                     Application main file.
 * - vendor_data.c              Vendor Specific API functions
 * \section intro Application Introduction
 *  HID QTOUCH Remote is the demo application which can act as a media player
 * remote when paired up with ZID USB HID PC adaptor.
 *  On Power up,Remote will initiate push button pairing procedure and pairs
 * with an adaptor if it is found.After pairing it can be
 *  used as a media player remote,The demo application
 *  shows the following Commands,
 *  1. Opening the media player
 *  2. Play
 *	3. Pause
 *	4. Volume up
 *	5. Volume down
 *	6. Mute
 *	7. Stop
 *  Qtouch button movements in the remote will send Corresponding HID commands.
 *  the Application uses the ZID reports to send hid reports to
 *  paired device.
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

/* === INCLUDES ============================================================ */
#include <asf.h>
#include "conf_board.h"
#include "led.h"
#include "common_sw_timer.h"
#include "app_config.h"
#include "rf4ce.h"
#include "vendor_data.h"
#include "pb_pairing.h"
#include "zid.h"
#include "zid_device.h"
#include "Avr477QTouch.h"

#define BUTTON_0 0
#define BUTTON_1 1
#define BUTTON_2 2

/* === TYPES =============================================================== */
/* ZID States for Tracking */
typedef enum node_status_tag {
	IDLE,
	WARM_START,
	COLD_START,
	CONFIGURING_ATTRIBUTES,
	CONNECTING,
	TRANSMITTING
} SHORTENUM node_status_t;

/* === MACROS ============================================================== */

/* media player control definition */
#define  BUTTON_PLAY       (0x0080)
#define  BUTTON_PAUSE      (0x0100)
#define  BUTTON_MPLAYER    (0x0008)
#define  BUTTON_STOP       (0x0010)
#define  BUTTON_NEXT       (0x0040)
#define  BUTTON_PREVIOUS   (0x0020)
#define  BUTTON_VOLUME_P   (0x0002)
#define  BUTTON_VOLUME_N   (0x0004)
#define  BUTTON_MUTE       (0x0001)
#define  BUTTON_MODE       (0x00FE)
#define  BUTTON_REPEAT     (0x0200)

/* Remote Controller Modes */
#define  BUTTON_PPT_MODE   (0x01)
#define  BUTTON_MEDIA_MODE (0x02)

/* Keyboard control definition */
#define  BUTTON_ESC        (41)
#define  BUTTON_TAB        (43)
#define  BUTTON_UP         (82)
#define  BUTTON_DOWN       (81)
#define  BUTTON_LEFT       (80)
#define  BUTTON_RIGHT      (79)
#define  BUTTON_DELETE     (76)

#define  BUTTON_PAGE_UP     (75)
#define  BUTTON_PAGE_DOWN   (78)
#define  BUTTON_FUNCTION_F5 (62)

#define  LAST_BUTTON_INDEX (0xFF)

/* Key mapping for media control */
uint16_t key_mapping_media[]
	= {BUTTON_STOP, BUTTON_PREVIOUS, BUTTON_NEXT, BUTTON_MPLAYER, \
	   BUTTON_VOLUME_P, BUTTON_PLAY, BUTTON_PAUSE,
	   BUTTON_VOLUME_N, \
	   BUTTON_MUTE, LAST_BUTTON_INDEX,
	   LAST_BUTTON_INDEX, LAST_BUTTON_INDEX};

/* Default mode for the key remote controller board is media player control */
uint8_t button_mode = BUTTON_MEDIA_MODE;

#define INTER_FRAME_DURATION_US     200000 /* 200 ms */
#define TX_OPTIONS  (TXO_UNICAST | TXO_DST_ADDR_NET | \
	TXO_ACK_REQ | TXO_SEC_REQ | TXO_MULTI_CH | \
	TXO_CH_NOT_SPEC | TXO_VEND_NOT_SPEC)

/* === GLOBALS ============================================================= */
uint8_t b_event = 0xFF;
uint8_t b_state = 0xFF;

/* ZID Attribute Request Size */
#define ZID_ATTRIBUTE_REQ_SIZE (10)

/* Macros and Flash declaration for vendor and application string */
#define VENDOR_STRING_LEN_SIZE (7)
#define APP_USER_STRING_SIZE   (15)
FLASH_DECLARE(uint16_t VendorIdentifier) = (uint16_t)NWKC_VENDOR_IDENTIFIER;
FLASH_DECLARE(uint8_t vendor_string[VENDOR_STRING_LEN_SIZE])
	= NWKC_VENDOR_STRING;
FLASH_DECLARE(uint8_t app_user_string[APP_USER_STRING_SIZE]) = APP_USER_STRING;

/* ZID Node Status */
static node_status_t node_status;

/* default pairing reference value */
static uint8_t pairing_ref = 0xFF;
#if (defined  RF4CE_CALLBACK_PARAM)
static zid_indication_callback_t zid_ind;

#endif
/* Initial value for ZID Report and attribute index */
static uint8_t report_id = 0;
static uint8_t set_attribute_index = 0;
static void app_alert(void);

/* === PROTOTYPES ========================================================== */
static void app_task(void);
static void indicate_fault_behavior(void);

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
 * Main function, initialization and main message loop
 *
 */
int main(void)
{
	irq_initialize_vectors();

	/* Initialize the board.*/
	board_init();

	/* Initialize the Software timer */
	sw_timer_init();

	/* Initialize the RF4CE Network */
	if (nwk_init() != NWK_SUCCESS) {
		app_alert();
	}

#ifdef RF4CE_CALLBACK_PARAM
	/* Register the ZID Callback indication */
	zid_ind.zid_report_data_indication_cb = zid_report_data_indication;
	zid_ind.zid_get_report_indication_cb = zid_get_report_indication;
	register_zid_indication_callback(&zid_ind);
#endif

	/*
	 * The stack is initialized above,
	 * hence the global interrupts are enabled here.
	 */

	BSP_InitQTouch(appButtonsInd);

#ifdef QDEBUG
	while (1) {
		/* If Touch Detected return will be true else false */
		if (Touch_measurement(&b_event, &b_state)) {
			LED_Toggle(LED0);
		}
	}
#endif
	cpu_irq_enable();

	if (b_event) {
		/* Force push button pairing */
		/* Cold start */
		LED_On(ZID_APP_LED);
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

/**
 * @brief nlme_reset_confirm The NLME-RESET.confirm primitive allows the NLME
 * to notify the application of the status of its request to reset the NWK
 * layer.
 *
 * @param Status  status from the network layer corresponds to the
 * nlme_reset_req
 */
#ifdef RF4CE_CALLBACK_PARAM
static
#endif
void nlme_reset_confirm(nwk_enum_t Status)
{
	if (Status != NWK_SUCCESS) {
		if (node_status == COLD_START) {
			while (1) {
				indicate_fault_behavior();
			}
		} else {
			indicate_fault_behavior();
			/* Reset the network and start again */
			/* Warm start */
			node_status = WARM_START;
			nlme_reset_request(false
#ifdef RF4CE_CALLBACK_PARAM
					, (FUNC_PTR)nlme_reset_confirm
#endif
					);
		}
	}

	if (node_status == COLD_START) {
		/* cold start procedure starts here */
		pairing_ref = 0xFF;
		nlme_start_request(
#ifdef RF4CE_CALLBACK_PARAM
				(FUNC_PTR)nlme_start_confirm
#endif

				);
	} else { /* warm start procedure starts here */
		pairing_ref = 0;
		/* Set power save mode: sleep */
		nlme_rx_enable_request(0 /*nwkcMinActivePeriod*/
#ifdef RF4CE_CALLBACK_PARAM
				, (FUNC_PTR)app_nlme_rx_enable_confirm
#endif

				);
	}
}

/**
 * @brief nlme_start_confirm The NLME-START.confirm primitive allows the NLME
 * to notify the application of the status of its request to start a network.
 *
 * @param Status  status from the network layer corresponds to the
 * nlme_start_req
 */
#ifdef RF4CE_CALLBACK_PARAM
static
#endif
void nlme_start_confirm(nwk_enum_t Status)
{
	if (Status != NWK_SUCCESS) {
		if (node_status == COLD_START) {
			app_alert();
		} else {
			indicate_fault_behavior();
			/* Reset the network and start again */
			/* Warm start */
			node_status = WARM_START;
			nlme_reset_request(false
#ifdef RF4CE_CALLBACK_PARAM
					, (FUNC_PTR)nlme_reset_confirm
#endif
					);
		}
	}

	if (node_status == COLD_START) {
		uint8_t value = ZID_ATTRIBUTE_REQ_SIZE;
		zid_set_attribute_request(0xFF, aplHIDNumStdDescComps, 0, &value
#ifdef RF4CE_CALLBACK_PARAM
				, (FUNC_PTR)zid_set_attribute_confirm
#endif
				);
		node_status = CONFIGURING_ATTRIBUTES;
	}
}

/**
 * @brief zid_set_attribute_confirm ZID Set attribute confirm callback allows to
 *        check whether attributes are sent to the ZID Adaptor.
 *
 * @param Status  status from the network layer corresponds to the
 * zid_set_attribute_request
 * @param PairingRef current pairing ref
 * @param ZIDAttribute attribute name
 * @param ZIDAttributeIndex attribute index
 */
static void zid_set_attribute_confirm(nwk_enum_t status, uint8_t PairingRef,
		zid_attribute_t ZIDAttribute,
		uint8_t ZIDAttributeIndex)
{
	if (status == NWK_SUCCESS) {
		if (ZIDAttribute == aplHIDStdDescCompsList) {
			if (ZIDAttributeIndex >= (ZID_ATTRIBUTE_REQ_SIZE - 1)) {
				set_attribute_index = 0;
				LED_Off(ZID_APP_LED);

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
		}
	} else {
		if (node_status == COLD_START) {
			app_alert();
		} else {
			indicate_fault_behavior();
			/* Reset the network and start again */
			/* Warm start */
			node_status = WARM_START;
			nlme_reset_request(false
#ifdef RF4CE_CALLBACK_PARAM
					, (FUNC_PTR)nlme_reset_confirm
#endif
					);
		}
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
		if (node_status == COLD_START) {
			app_alert();
		} else {
			indicate_fault_behavior();
			/* Reset the network and start again */
			/* Warm start */
			node_status = WARM_START;
			nlme_reset_request(false
#ifdef RF4CE_CALLBACK_PARAM
					, (FUNC_PTR)nlme_reset_confirm
#endif
					);
		}
	}

	pairing_ref = PairingRef;

	/* Set power save mode */

	if (node_status == CONNECTING) {
		node_status = IDLE;
		zid_state = ZID_STATE_IDLE;

		/* LED handling */
		LED_On(ZID_APP_LED);
		delay_ms(1000);
		LED_Off(ZID_APP_LED);
	}
}

/**
 * @brief heart beat confirm; target and controller use
 *
 * The heart beat confirm is a callback that provides the status of the
 * ZID Adaptor. HID class device can periodically check in with the HID
 * adaptor to give it the opportunity to send a message back
 *
 * @param Status        Status of the heartbeat command
 * @param PairingRef    PairingRef contains assigned pairing reference.
 */
void zid_heartbeat_confirm(nwk_enum_t Status, uint8_t PairingRef)
{
	/* Keep compiler happy. */
	Status = Status;
	PairingRef = PairingRef;
}

/**
 * @brief ZID Report Data indication used to know if the adaptor sends
 *        any output report to the ZID Device.
 *
 * @param PairingRef    PairingRef contains assigned pairing reference.
 * @param num_report_records total no of reports in the report data
 * @param zid_report_data_record_ptr pointer to the report data
 * @param RxLinkQuality Link Quality of the received report
 * @param RxFlags Flags for the Received frame
 */
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

/**
 * @brief The NLME-RX-ENABLE.confirm primitive reports the results of
 *        the attempt to enable or disable the receiver.
 *
 * @param Status Status of the nlme_rx_enable_req
 */
#ifdef RF4CE_CALLBACK_PARAM
static
#endif
void app_nlme_rx_enable_confirm(nwk_enum_t Status)
{
	if (Status != NWK_SUCCESS) {
		if (node_status == COLD_START) {
			while (1) {
				indicate_fault_behavior();
			}
		} else {
			indicate_fault_behavior();
			/* Reset the network and start again */
			/* Warm start */
			node_status = WARM_START;
			nlme_reset_request(false
#ifdef RF4CE_CALLBACK_PARAM
					, (FUNC_PTR)nlme_reset_confirm
#endif
					);
		}
	}

	if (node_status == COLD_START) {
		node_status = IDLE;

		/* LED handling */
		LED_On(ZID_APP_LED);
		delay_ms(1000);
		LED_Off(ZID_APP_LED);
	} else if (node_status == WARM_START) {
		node_status = IDLE;
		LED_On(ZID_APP_LED);
		delay_ms(250);
		LED_Off(ZID_APP_LED);
	}
}

/**
 * @brief Application task and state machine
 */
static void app_task(void)
{
	switch (node_status) {
	case IDLE:
	{
		static uint32_t current_time;
		static uint32_t previous_button_time;
		uint8_t num_records = 1;

		if (Touch_measurement(&b_event, &b_state)) {
			current_time = sw_timer_get_time();
			if ((current_time - previous_button_time) <
					INTER_FRAME_DURATION_US) {
				return;
			} else {
				/* Store current time */
				previous_button_time = current_time;
			}

			if (key_mapping_media[b_state] != LAST_BUTTON_INDEX) {
				zid_report_data_record_t zid_report_data[2];
				uint8_t report_data_buffer[80];
				uint8_t *msg_ptr = &report_data_buffer[0];

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
				keyboard_input_desc->key_code[4]
					= (uint8_t)key_mapping_media
						[b_state];
				keyboard_input_desc->key_code[5]
					= (uint8_t)(key_mapping_media
						[b_state] >> 8);
				num_records = 1;

				if (zid_report_data_request(pairing_ref,
						num_records,
						zid_report_data,
						TX_OPTIONS
  #ifdef RF4CE_CALLBACK_PARAM
						, (FUNC_PTR)
						zid_report_data_confirm
  #endif
						)) {
					node_status = TRANSMITTING;
				}
			}
		} else {
			if (nwk_stack_idle()) {
				/* Set MCU to sleep */
				/* Configure the wakeup source and sleep */
				/* MCU is awake again */
			}
		}
	}
	break;

	case CONFIGURING_ATTRIBUTES:
	{
		/* Configure the attributes */
		uint8_t value[ZID_ATTRIBUTE_REQ_SIZE]
			= {MOUSE, KEYBOARD, CONTACT_DATA,
			   TAP_GESTURE, SCROLL_GESTURE,
			   PINCH_GESTURE, ROTATE_GESTURE,
			   SYNC, TOUCH_SENSOR_PROPERTIES,
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
 * @brief zid_report_data_confirm is call back of zid_report_data_request
 *        which gives the status of the report data request
 *
 * @param Status Status of the zid_report_data_request
 * @param PairingRef pairing reference for current transaction
 */
#ifdef RF4CE_CALLBACK_PARAM
static
#endif
void zid_report_data_confirm(nwk_enum_t Status, uint8_t PairingRef)
{
	node_status = IDLE;

	if (report_id < 2) {
		report_id++;
	} else {
		report_id = 0;
	}

	if (Status == NWK_SUCCESS) {
		LED_Off(ZID_APP_LED);
	} else {
		indicate_fault_behavior();
	}

	/* Keep compiler happy. */
	PairingRef = PairingRef;
}

/**
 * @brief zid_data_confirm is call back of zid_data_request
 *        which gives the status of the data request
 *
 * @param Status Status of the zid_data_request
 * @param PairingRef pairing reference for current transaction
 */
#ifdef RF4CE_CALLBACK_PARAM
static
#endif
void zid_data_confirm(nwk_enum_t Status, uint8_t PairingRef)
{
	node_status = IDLE;

	if (Status == NWK_SUCCESS) {
		LED_Off(ZID_APP_LED);
	} else {
		indicate_fault_behavior();
	}

	/* Keep compiler happy. */
	PairingRef = PairingRef;
}

/**
 * @brief zid_data_confirm is call back to the application layer.
 *        HID adaptor shall create a get report command frame and transmit
 *        it to the appropriate HID class device. On receipt of the
 *        corresponding report data  command frame, the HID adaptor shall pass
 * the
 *        report to the HID class driver.
 *
 * @param zid_report_type Report type
 * @param PairingRef pairing reference for current transaction
 * @param zid_report_desc zid report descriptor type
 * @param RxLinkQuality Received frame link quality
 * @param RxFlags Received frame RxFlags
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

/**
 * @brief nlde_data_confirm The NLDE-DATA.confirm primitive is generated by the
 *        NWK layer entity in response to an NLDE-DATA.request primitive.
 *
 * @param Status Status of the NLDE-DATA.request primitive
 * @param PairingRef PairingRef for the current transaction
 * @param ProfileId ProfileId used for the current transaction
 */
void nlde_data_confirm(nwk_enum_t Status, uint8_t PairingRef,
		profile_id_t ProfileId)
{
	node_status = IDLE;

	if (Status == NWK_SUCCESS) {
		LED_Off(ZID_APP_LED);
	} else {
		indicate_fault_behavior();
	}

	/* Keep compiler happy. */
	PairingRef = PairingRef;
	ProfileId = ProfileId;
}

/**
 * @brief nlde_data_confirm The NLDE-DATA.confirm primitive is generated by the
 *        NWK layer entity in response to an NLDE-DATA.request primitive.
 *
 * @param PairingRef PairingRef for the current transaction
 * @param payload_length payload length
 * @param payload payload to check the adaptor compatibility
 * @return bool true or false
 */
bool check_zid_adaptor_compatibility(uint8_t PairingRef, uint8_t payload_length,
		uint8_t *payload)
{
	/*
	 * Application need to find out the compatibility with the adaptor
	 * It needs to extract the adaptor attributes from the following payloas
	 * Payload format is as per the GET_ATTRIBUTES_RESPONSE packet format
	 * excluding the header
	 * payload[0] = attr_id...........
	 *
	 */
	PairingRef = PairingRef;
	payload_length = payload_length;
	payload = payload;
	return true;
}

/* --- Helper functions ---------------------------------------------------- */

/**
 * @brief Indicating malfunction
 */
static void indicate_fault_behavior(void)
{
	uint8_t i;

	for (i = 0; i < 10; i++) {
		LED_On(ZID_APP_LED);
		delay_ms(250);
		LED_Off(ZID_APP_LED);
		delay_ms(250);
	}
}

/**
 * @brief Vendor-specific callback; handles reaction to incoming alive request
 */
void vendor_app_alive_req(void)
{
	/* Variant to demonstrate FOTA featue */
	LED_On(ZID_APP_LED);
	delay_ms(500);
	LED_Off(ZID_APP_LED);
	delay_ms(500);
}

/**
 * @brief application failure indication
 */
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
		delay_ms(300);
	}
}

/* EOF */
