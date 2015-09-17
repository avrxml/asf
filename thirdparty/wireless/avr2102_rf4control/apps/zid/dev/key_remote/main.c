/**
 * @file main.c
 *
 * @brief HID Key Remote Application
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
 * This is the reference manual for ZID application.
 * \section main_files Application Files
 * - main.c                      Application main file.
 * - vendor_data.c               Vendor Specific API functions
 * \section intro Application Introduction
 * ZID HID Class Device Application will act as HID Device for the RF4CE ZID
 * Class adaptor application. ZID HID Class Device stack will respond for the
 *user
 * input via button press events in key remote control board.
 * Corresponds to button press event LED will toggle on the key remote
 *controller board
 * once the device is paired with adaptor. Where as the user
 * will choose the options like cold start, warm start, reset(NIB will be reset
 * to default values and stored in EEPROM),
 *
 *
 * options are user selectable via serial console, response will be printed on
 * the serial console after processing the requests.
 *
 *
 * Terminal adaptor can be used with the ZID device application.
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

#define INTER_FRAME_DURATION_US        (200000) /* 200ms */
#define INTER_FRAME_DURATION_MOUSE_US  (5000)   /* 5ms */
#define APPX_5_DEGREE_VALUE    10

/* ZID Tx Options */
#define TX_OPTIONS  (TXO_UNICAST | TXO_DST_ADDR_NET | \
	TXO_ACK_REQ | TXO_SEC_REQ | TXO_MULTI_CH | \
	TXO_CH_NOT_SPEC | TXO_VEND_NOT_SPEC)

/* Key Remote controller board key count */
#define TOTAL_NO_OF_ZID_KEY_RC (27)

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
#define  BUTTON_REPEAT     (0x0200)

/* Key Remote Controller Modes */
#define  BUTTON_MOUSE_MODE (0xFB)
#define  BUTTON_PPT_MODE   (0xFC)
#define  BUTTON_GAME_MODE  (0xFD)
#define  BUTTON_MEDIA_MODE (0xFE)

/* Keyboard control definition */
#define  BUTTON_ESC         (41)
#define  BUTTON_TAB         (43)
#define  BUTTON_UP_E        (82)
#define  BUTTON_DOWN_E      (81)
#define  BUTTON_LEFT_E      (80)
#define  BUTTON_RIGHT_E     (79)
#define  BUTTON_DELETE      (76)
#define  BUTTON_PAGE_UP     (75)
#define  BUTTON_PAGE_DOWN   (78)
#define  BUTTON_FUNCTION_F5 (62)

/* mouse control definition */
#define  BUTTON_LEFT_SINGLE_CLK  (90)
#define  BUTTON_RIGHT_SINGLE_CLK (91)
#define  BUTTON_SCROLL_UP        (92)
#define  BUTTON_SCROLL_DOWN      (93)
#define  BUTTON_MIDDLE_CLK       (94)
#define  BUTTON_1  0x10
#define  BUTTON_2  0x20
#define  BUTTON_3  0x40
#define  BUTTON_4  0x80
#define BUTTON_THROTTLE_UP 0x11
#define BUTTON_THROTTLE_DOWN 0x14

#define ZID_COLD_START           (0)

/* Invalid button definition */
#define  BUTTON_INVALID        (0xFF)

/* === GLOBALS ============================================================= */
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
uint16_t x_val;
uint16_t y_val;
uint16_t z_val;
uint16_t ADC_val;
uint8_t status;
/* Offset values for the axis */
uint16_t x_offset;
uint16_t y_offset;
uint16_t z_offset;
/* Function pointers for offset correction function assignment */
void (*Correct_x_offset)(uint16_t *temp_val, uint16_t temp_offset);
void (*Correct_y_offset)(uint16_t *temp_val, uint16_t temp_offset);
void (*Correct_z_offset)(uint16_t *temp_val, uint16_t temp_offset);
/* ZID application status indication LED */
#define ZID_APP_LED (LED0)
#define ACCELEROMETER_SAMPLE_TIME 10000
static uint8_t APP_TIMER_ACC_READ;
static void acc_read_cb(void *parameter);

/* Key mapping for media control */
static uint16_t key_mapping_media[TOTAL_NO_OF_ZID_KEY_RC] =
{  BUTTON_REPEAT, /* BUTTON_REPEAT    */
   BUTTON_MOUSE_MODE, /* BUTTON_MOUSE_MODE  */
   BUTTON_PPT_MODE, /* BUTTON_PPT_MODE */
   BUTTON_GAME_MODE, /* BUTTON_GAME_MODE   */
   BUTTON_MEDIA_MODE, /* BUTTON_MEDIA_MODE */
   BUTTON_INVALID, /* BUTTON_INVALID */
   BUTTON_INVALID, /* BUTTON_INVALID */
   BUTTON_INVALID, /* BUTTON_INVALID */
   BUTTON_INVALID, /* BUTTON_INVALID */
   BUTTON_INVALID, /* BUTTON_NUMBER_1 */
   BUTTON_INVALID, /* BUTTON_NUMBER_2 */
   BUTTON_INVALID, /* BUTTON_NUMBER_3 */
   BUTTON_INVALID, /* BUTTON_NUMBER_4 */
   BUTTON_INVALID, /* BUTTON_NUMBER_5 */
   BUTTON_INVALID, /* BUTTON_NUMBER_6 */
   BUTTON_INVALID, /* BUTTON_NUMBER_7 */
   BUTTON_INVALID, /* BUTTON_NUMBER_8 */
   BUTTON_INVALID, /* BUTTON_NUMBER_9 */
   BUTTON_PLAY,    /* BUTTON_UP       */
   BUTTON_PREVIOUS, /* BUTTON_LEFT     */
   BUTTON_MPLAYER, /* BUTTON_ENTER    */
   BUTTON_NEXT,    /* BUTTON_RIGHT    */
   BUTTON_PAUSE,   /* BUTTON_DOWN     */
   BUTTON_VOLUME_P, /* BUTTON_VOLUME_P  */
   BUTTON_VOLUME_N, /* BUTTON_VOLUME_N */
   BUTTON_MUTE,    /* BUTTON_MUTE */
   BUTTON_STOP     /* BUTTON_STOP */
};

/* Key mapping for ZID Keyboard Control */
static uint8_t key_mapping_ppt[TOTAL_NO_OF_ZID_KEY_RC] =
{  BUTTON_ESC, /* BUTTON_ESC    */
   BUTTON_MOUSE_MODE, /* BUTTON_MOUSE_MODE  */
   BUTTON_PPT_MODE, /* BUTTON_PPT_MODE */
   BUTTON_GAME_MODE, /* BUTTON_GAME_MODE   */
   BUTTON_MEDIA_MODE, /* BUTTON_MEDIA_MODE */
   BUTTON_INVALID, /* BUTTON_INVALID */
   BUTTON_INVALID, /* BUTTON_INVALID */
   BUTTON_INVALID, /* BUTTON_INVALID */
   BUTTON_INVALID, /* BUTTON_INVALID */
   BUTTON_INVALID, /* BUTTON_NUMBER_1 */
   BUTTON_INVALID, /* BUTTON_NUMBER_2 */
   BUTTON_INVALID, /* BUTTON_NUMBER_3 */
   BUTTON_INVALID, /* BUTTON_NUMBER_4 */
   BUTTON_INVALID, /* BUTTON_NUMBER_5 */
   BUTTON_INVALID, /* BUTTON_NUMBER_6 */
   BUTTON_INVALID, /* BUTTON_NUMBER_7 */
   BUTTON_INVALID, /* BUTTON_NUMBER_8 */
   BUTTON_INVALID, /* BUTTON_NUMBER_9 */
   BUTTON_UP_E, /* BUTTON_UP_E       */
   BUTTON_LEFT_E, /* BUTTON_LEFT_E     */
   BUTTON_FUNCTION_F5, /* BUTTON_FUNCTION_F5    */
   BUTTON_RIGHT_E, /* BUTTON_RIGHT_E    */
   BUTTON_DOWN_E, /* BUTTON_DOWN_E     */
   BUTTON_TAB, /* BUTTON_TAB  */
   BUTTON_DELETE, /* BUTTON_DELETE */
   BUTTON_PAGE_UP, /* BUTTON_PAGE_UP */
   BUTTON_PAGE_DOWN /* BUTTON_PAGE_DOWN */
};
/* Key mapping for ZID mouse Control */
static uint8_t key_mapping_gamepad[TOTAL_NO_OF_ZID_KEY_RC] =
{   BUTTON_INVALID,  /* BUTTON_INVALID    */
    BUTTON_MOUSE_MODE,      /* BUTTON_MOUSE_MODE  */
    BUTTON_PPT_MODE,      /* BUTTON_PPT_MODE */
    BUTTON_GAME_MODE,      /* BUTTON_GAME_MODE   */
    BUTTON_MEDIA_MODE,      /* BUTTON_MEDIA_MODE */
    BUTTON_INVALID,      /* BUTTON_INVALID */
    BUTTON_INVALID,      /* BUTTON_INVALID */
    BUTTON_INVALID,      /* BUTTON_INVALID */
    BUTTON_INVALID,      /* BUTTON_INVALID */
    BUTTON_INVALID,      /* BUTTON_NUMBER_1 */
    BUTTON_INVALID,      /* BUTTON_NUMBER_2 */
    BUTTON_INVALID,      /* BUTTON_NUMBER_3 */
    BUTTON_INVALID,      /* BUTTON_NUMBER_4 */
    BUTTON_INVALID,      /* BUTTON_NUMBER_5 */
    BUTTON_INVALID,      /* BUTTON_NUMBER_6 */
    BUTTON_INVALID,      /* BUTTON_NUMBER_7 */
    BUTTON_INVALID,      /* BUTTON_NUMBER_8 */
    BUTTON_INVALID,      /* BUTTON_NUMBER_9 */
    BUTTON_1,      /* BUTTON_LEFT_SINGLE_CLK  */
    BUTTON_2,      /* BUTTON_RIGHT_SINGLE_CLK */
    BUTTON_INVALID,      /* BUTTON_SCROLL_UP */
    BUTTON_3,      /* BUTTON_SCROLL_DOWN */
    BUTTON_4,      /* BUTTON_RIGHT_E    */
    BUTTON_THROTTLE_UP,      /* BUTTON_LEFT_SINGLE_CLK  */
    BUTTON_THROTTLE_DOWN,      /* BUTTON_RIGHT_SINGLE_CLK */
    BUTTON_INVALID,      /* BUTTON_SCROLL_UP */
    BUTTON_INVALID      /* BUTTON_SCROLL_DOWN */
};

/* Key mapping for ZID mouse Control */
static uint8_t key_mapping_mouse[TOTAL_NO_OF_ZID_KEY_RC] =
{   BUTTON_INVALID, /* BUTTON_INVALID    */
    BUTTON_MOUSE_MODE,     /* BUTTON_MOUSE_MODE  */
    BUTTON_PPT_MODE,     /* BUTTON_PPT_MODE */
    BUTTON_GAME_MODE,     /* BUTTON_GAME_MODE   */
    BUTTON_MEDIA_MODE,     /* BUTTON_MEDIA_MODE */
    BUTTON_INVALID,     /* BUTTON_INVALID */
    BUTTON_INVALID,     /* BUTTON_INVALID */
    BUTTON_INVALID,     /* BUTTON_INVALID */
    BUTTON_INVALID,     /* BUTTON_INVALID */
    BUTTON_INVALID,     /* BUTTON_NUMBER_1 */
    BUTTON_INVALID,     /* BUTTON_NUMBER_2 */
    BUTTON_INVALID,     /* BUTTON_NUMBER_3 */
    BUTTON_INVALID,     /* BUTTON_NUMBER_4 */
    BUTTON_INVALID,     /* BUTTON_NUMBER_5 */
    BUTTON_INVALID,     /* BUTTON_NUMBER_6 */
    BUTTON_INVALID,     /* BUTTON_NUMBER_7 */
    BUTTON_INVALID,     /* BUTTON_NUMBER_8 */
    BUTTON_INVALID,     /* BUTTON_NUMBER_9 */
    BUTTON_UP_E,     /* BUTTON_UP_E       */
    BUTTON_LEFT_E,     /* BUTTON_LEFT_E     */
    BUTTON_MIDDLE_CLK,     /* BUTTON_MIDDLE_CLK    */
    BUTTON_RIGHT_E,     /* BUTTON_RIGHT_E    */
    BUTTON_DOWN_E,     /* BUTTON_DOWN_E     */
    BUTTON_LEFT_SINGLE_CLK,     /* BUTTON_LEFT_SINGLE_CLK  */
    BUTTON_RIGHT_SINGLE_CLK,     /* BUTTON_RIGHT_SINGLE_CLK */
    BUTTON_SCROLL_UP,     /* BUTTON_SCROLL_UP */
    BUTTON_SCROLL_DOWN     /* BUTTON_SCROLL_DOWN */
};

#define MOUSE_POSITIVE_DISPLACEMENT  (2)
#define MOUSE_NEGATIVE_DISPLACEMENT  (-2)

/* Default mode for the key remote controller board is media player control */
uint16_t button_mode = BUTTON_MEDIA_MODE;

/* inter frame duration */
uint32_t zid_interframe_duration = INTER_FRAME_DURATION_US;

#if (defined  RF4CE_CALLBACK_PARAM)
static zid_indication_callback_t zid_ind;
#endif

/* Initial value for ZID Report and attribute index */
static uint8_t report_id = 0;
static uint8_t set_attribute_index = 0;

/* Store the current button val & set the initial button state */
button_id_t button_val;
uint8_t b_state = 0xFF;

/* === PROTOTYPES ========================================================== */
static void app_alert(void);
uint8_t get_zid_keyrc_button(button_id_t button_id);
static void app_task(void);
static void indicate_fault_behavior(void);
static void app_calculate_offset();
static void correct_negative_offset(uint16_t *temp_val, uint16_t temp_offset);
static void correct_positive_offset(uint16_t *temp_val, uint16_t temp_offset);

#ifdef RF4CE_CALLBACK_PARAM
static void nlme_reset_confirm(nwk_enum_t Status);
static void nlme_start_confirm(nwk_enum_t Status);
static void app_nlme_rx_enable_confirm(nwk_enum_t Status);
static void zid_connect_confirm(nwk_enum_t Status, uint8_t PairingRef);
static void zid_report_data_confirm(nwk_enum_t Status, uint8_t PairingRef);
static void zid_heartbeat_confirm(nwk_enum_t Status, uint8_t PairingRef);
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

	/* Init the RF4CE Network layer */
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
	cpu_irq_enable();
	sw_timer_get_id(&APP_TIMER_ACC_READ);
	if (get_zid_keyrc_button(button_scan()) == ZID_COLD_START) {
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
 *layer.
 *
 * @param Status  status from the network layer corresponds to the
 *nlme_reset_req
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
 *nlme_start_req
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
 *zid_set_attribute_request
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
		LED_On(LED_1);
		LED_Off(LED_2);
		LED_Off(LED_3);
		LED_Off(LED_4);
		LED_On(LED_5);
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
		button_id_t button;
		static uint32_t current_time;
		static uint32_t previous_button_time;
		uint8_t num_records = 1;
		static button_id_t previous_button;
		keyboard_input_desc_t *joystick_desc;
		zid_report_data_record_t zid_report_data_joystick[2];
		uint8_t report_data_buffer_joystick[80];
		uint8_t *msg_ptr = &report_data_buffer_joystick[0];
		if (button_mode == BUTTON_GAME_MODE) {
			/* Create the input report for the ppt control */
			zid_report_data_joystick[0].report_type = INPUT;
			zid_report_data_joystick[0].report_desc_identifier
				= KEYBOARD;
			zid_report_data_joystick[0].report_data
				= (void *)msg_ptr;
			joystick_desc = (keyboard_input_desc_t *)msg_ptr;
			joystick_desc->modifier_keys = 0x00;
			joystick_desc->key_code[0] = 0x00;
			joystick_desc->key_code[1] = 0x00;
			joystick_desc->key_code[2] = 0x00;
			joystick_desc->key_code[3] = 0x00;
			joystick_desc->key_code[4] = 0x00;
			joystick_desc->key_code[5] = 0x00;
			uint16_t x_temp = x_val;
			uint16_t y_temp = y_val;

			if (y_temp > (ADC_val + APPX_5_DEGREE_VALUE)) {
				joystick_desc->key_code[2] = 1;                   /* down
					                                           * y-axis
					                                           * negative */
			} else if (y_temp < (ADC_val - APPX_5_DEGREE_VALUE)) {
				joystick_desc->key_code[2] = 2;                   /* up
					                                           * y-axis
					                                           * positive */
			} else if (x_temp > (ADC_val + APPX_5_DEGREE_VALUE)) {
				joystick_desc->key_code[2] = 3;                   /* left
					                                           * x-axis
					                                           * negative */
			} else if (x_temp < (ADC_val - APPX_5_DEGREE_VALUE)) {
				joystick_desc->key_code[2] = 4;                   /* right
					                                           * x-axis
					                                           * positive */
			} else {
				/*  */
			}
		}

		/* Scan the button events */
		button = button_scan();
		/* Check if any valid vents occurred */
		if ((button != BUTTON_UNKNOWN) && (button == previous_button)) {
			/* Check time to previous transmission. */

			current_time = sw_timer_get_time();
			if ((current_time - previous_button_time) <
					zid_interframe_duration) {
				return;
			} else {
				/* Check if the key press mode is mouse mode */
				if (BUTTON_MOUSE_MODE == button_mode) {
					if (!((key_mapping_mouse[b_state] <=
							BUTTON_UP_E) &&
							(BUTTON_RIGHT_E >=
							key_mapping_mouse
							[b_state]))) {
						/* Inter-frame duration for
						 *mouse will be less than the
						 *actual key events */
						if ((current_time -
								previous_button_time)
								<
								INTER_FRAME_DURATION_MOUSE_US)
						{
							return;
						}
					}
				}

				/* Store current time */
				previous_button_time = current_time;
			}

			/* Get the key index value for the actual event */
			b_state = get_zid_keyrc_button(button);

			/* Check any mode change event occurred */
			switch (key_mapping_media[b_state]) {
			case BUTTON_MOUSE_MODE:
				/* Configure for the mouse mode */
				button_mode = BUTTON_MOUSE_MODE;
				LED_On(LED_2);
				LED_Off(LED_3);
				LED_Off(LED_4);
				LED_Off(LED_5);
				zid_interframe_duration
					= INTER_FRAME_DURATION_MOUSE_US;
				return;

				break;

			case BUTTON_PPT_MODE:
				/* Configure for the ppt mode */
				button_mode = BUTTON_PPT_MODE;
				LED_Off(LED_2);
				LED_On(LED_3);
				LED_Off(LED_4);
				LED_Off(LED_5);
				zid_interframe_duration
					= INTER_FRAME_DURATION_US;
				return;

				break;

			case BUTTON_GAME_MODE:
				/* Configure for the game controller mode. */
				button_mode = BUTTON_GAME_MODE;
				LED_Off(LED_2);
				LED_Off(LED_3);
				LED_On(LED_4);
				LED_Off(LED_5);
				read_acc(&x_val, &y_val, &z_val, &ADC_val);
				app_calculate_offset();
				Correct_x_offset(&x_val, x_offset);
				Correct_y_offset(&y_val, y_offset);
				sw_timer_start(APP_TIMER_ACC_READ,
						ACCELEROMETER_SAMPLE_TIME,
						SW_TIMEOUT_RELATIVE,
						(FUNC_PTR)acc_read_cb, NULL);
				zid_interframe_duration
					= INTER_FRAME_DURATION_US;
				return;

				break;

			case BUTTON_MEDIA_MODE:
				/* Configure for the media player control mode
				 **/
				button_mode = BUTTON_MEDIA_MODE;
				LED_Off(LED_2);
				LED_Off(LED_3);
				LED_Off(LED_4);
				LED_On(LED_5);
				zid_interframe_duration
					= INTER_FRAME_DURATION_US;
				return;

				break;

			default:
				LED_On(LED_1);
				break;
			}

			if (button_mode == BUTTON_MEDIA_MODE) {
				/* get the valid key inputs for the media player
				 *control */
				zid_report_data_record_t zid_report_data[2];
				uint8_t report_data_buffer[80];
				uint8_t *msg_ptr = &report_data_buffer[0];

				if (key_mapping_media[b_state] ==
						BUTTON_INVALID) {
					return;
				}

				/* Create the input report for the media player
				 *control */
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
			} else if (button_mode == BUTTON_PPT_MODE) {
				/* get the valid key inputs for the ppt mode */
				zid_report_data_record_t zid_report_data[2];
				uint8_t report_data_buffer[80];
				uint8_t *msg_ptr = &report_data_buffer[0];

				if (key_mapping_ppt[b_state] ==
						BUTTON_INVALID) {
					return;
				}

				/* Create the input report for the ppt control
				 **/
				zid_report_data[0].report_type = INPUT;
				zid_report_data[0].report_desc_identifier
					= KEYBOARD;
				zid_report_data[0].report_data
					= (void *)msg_ptr;

				keyboard_input_desc_t *keyboard_input_desc;
				keyboard_input_desc
					= (keyboard_input_desc_t *)msg_ptr;

				keyboard_input_desc->modifier_keys = 0x00;
				keyboard_input_desc->key_code[0]
					= key_mapping_ppt[
					b_state];
				keyboard_input_desc->key_code[1] = 0x00;
				keyboard_input_desc->key_code[2] = 0x00;
				keyboard_input_desc->key_code[3] = 0x00;
				keyboard_input_desc->key_code[4] = 0x00;
				keyboard_input_desc->key_code[5] = 0x00;
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
					b_state = BUTTON_INVALID;
				}
			} else if (button_mode == BUTTON_MOUSE_MODE) {
				/* get the valid key inputs for the mouse mode
				 **/
				zid_report_data_record_t zid_report_data[2];
				uint8_t report_data_buffer[80];
				uint8_t *msg_ptr = &report_data_buffer[0];
				mouse_desc_t *mouse_desc;

				if (key_mapping_mouse[b_state] ==
						BUTTON_INVALID) {
					return;
				}

				/* Create the input report for the mouse control
				 **/
				zid_report_data[0].report_type = INPUT;
				zid_report_data[0].report_desc_identifier
					= MOUSE;
				zid_report_data[0].report_data
					= (void *)msg_ptr;
				mouse_desc = (mouse_desc_t *)msg_ptr;

				mouse_desc->button0 = 0x00;
				mouse_desc->button1 = 0x00;
				mouse_desc->button2 = 0x00;
				mouse_desc->x_coordinate = 0x00;
				mouse_desc->y_coordinate = 0x00;

				switch (key_mapping_mouse[b_state]) {
				case BUTTON_UP_E:
					mouse_desc->y_coordinate
						= MOUSE_NEGATIVE_DISPLACEMENT;
					break;

				case BUTTON_LEFT_E:
					mouse_desc->x_coordinate
						= MOUSE_NEGATIVE_DISPLACEMENT;
					break;

				case BUTTON_RIGHT_E:
					mouse_desc->x_coordinate
						= MOUSE_POSITIVE_DISPLACEMENT;
					break;

				case BUTTON_DOWN_E:
					mouse_desc->y_coordinate
						= MOUSE_POSITIVE_DISPLACEMENT;
					break;

				case BUTTON_LEFT_SINGLE_CLK:
					mouse_desc->button0 = 0x01;
					break;

				case BUTTON_RIGHT_SINGLE_CLK:
					mouse_desc->button1 = 0x01;
					break;

				case BUTTON_MIDDLE_CLK:
					mouse_desc->button2 = 0x01;
					break;

				case BUTTON_SCROLL_UP:
					mouse_desc->y_coordinate = 1;
					mouse_desc->button2 = 0x80;
					break;

				case BUTTON_SCROLL_DOWN:
					mouse_desc->x_coordinate = -1;
					mouse_desc->button2 = 0x80;
					break;

				default:
					break;
				}

				msg_ptr += sizeof(mouse_desc_t);
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
					b_state = BUTTON_INVALID;
				}
			} else if (button_mode == BUTTON_GAME_MODE) {
				if (key_mapping_gamepad[b_state] ==
						BUTTON_INVALID) {
					return;
				}

				switch (key_mapping_gamepad[b_state]) {
				case BUTTON_1:
				case BUTTON_2:
				case BUTTON_3:
				case BUTTON_4:
					joystick_desc->key_code[0]
						= key_mapping_gamepad
							[b_state];
					break;

				case BUTTON_THROTTLE_UP:
					joystick_desc->key_code[1] = 0x01;
					break;

				case BUTTON_THROTTLE_DOWN:
					joystick_desc->key_code[1] = 0x02;

				default:
					break;
				}

				num_records = 1;
			}
		} else {
			previous_button = button;
			previous_button_time = sw_timer_get_time();
			if (nwk_stack_idle()) {
				/* Set MCU to sleep */
				/* Configure the wakeup source and sleep */
				/* MCU is awake again */
			}
		}

		if (button_mode == BUTTON_GAME_MODE) {
			if (zid_report_data_request(pairing_ref, num_records,
					zid_report_data_joystick,
					TX_OPTIONS
					 #ifdef RF4CE_CALLBACK_PARAM
					, (FUNC_PTR)zid_report_data_confirm
					 #endif
					)) {
				node_status = TRANSMITTING;
				b_state = BUTTON_INVALID;
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
 *the
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
	 *excluding the header
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

/**
 * @brief get_zid_keyrc_button This function will get the index for each key
 *
 * @param button_id pressed button id will be given as input
 * @return uint8_t button index
 */
uint8_t get_zid_keyrc_button(button_id_t button_id)
{
	uint8_t i;
	for (i = 0; i < TOTAL_NO_OF_ZID_KEY_RC; i++) {
		if (button_id & ((button_id_t)1 << i)) {
			return i;
		}
	}
	return (BUTTON_INVALID);
}

/**
 * @brief Offset calculation function.
 *
 * @ingroup apiPalAppDemo
 */
void app_calculate_offset()
{
	uint16_t temp;

	if (x_val >= ADC_val) {
		/* Positive or No offset on the axis */
		x_offset = x_val - ADC_val;

		/* Initalise the Positive offset correction function to the
		 *function pointer */
		Correct_x_offset = &correct_positive_offset;
	} else {
		/* Negative Offset */
		x_offset = ADC_val - x_val;

		/* Initalise the Negative offset correction function to the
		 *function pointer */
		Correct_x_offset = &correct_negative_offset;
	}

	if (y_val >= ADC_val) {
		/* Positive or No offset on the axis */
		y_offset = y_val - ADC_val;

		/* Initalise the Positive offset correction function to the
		 *function pointer */
		Correct_y_offset = &correct_positive_offset;
	} else {
		/* Negative Offset */
		y_offset = ADC_val - y_val;

		/* Initalise the Negative offset correction function to the
		 *function pointer */
		Correct_y_offset = &correct_negative_offset;
	}

	/* On Z-axis a voltage of 1g is expected */
	temp = (ADC_val + (ADC_val * 2) / 10);

	if (z_val >= temp) {
		/* Positive or No offset on the axis */
		z_offset = z_val - temp;

		/* Initalise the Positive offset correction function to the
		 *function pointer */
		Correct_z_offset = &correct_positive_offset;
	} else {
		/* Negative Offset */
		z_offset = temp - z_val;

		/* Initalise the Negative offset correction function to the
		 *function pointer */
		Correct_z_offset = &correct_negative_offset;
	}
}

/**
 * @brief Positive offset correction function.
 *
 * @param temp_val value to be corrected
 * @param temp_offset Offset value
 *
 * @ingroup apiPalAppDemo
 */
static void correct_positive_offset(uint16_t *temp_val, uint16_t temp_offset)
{
	/* Subtract the offset form the sampled value */
	*(temp_val) -= temp_offset;
}

/**
 * @brief Negative offset correction function.
 *
 * @param temp_val value to be corrected
 * @param temp_offset Offset value
 *
 * @ingroup apiPalAppDemo
 */
static void correct_negative_offset(uint16_t *temp_val, uint16_t temp_offset)
{
	/* Add the offset form the sampled value */
	*(temp_val) += temp_offset;
}

static void acc_read_cb(void *parameter)
{
	read_acc(&x_val, &y_val, &z_val, &ADC_val);
	Correct_x_offset(&x_val, x_offset);
	Correct_y_offset(&y_val, y_offset);
	sw_timer_start(APP_TIMER_ACC_READ, ACCELEROMETER_SAMPLE_TIME,
			SW_TIMEOUT_RELATIVE,
			(FUNC_PTR)acc_read_cb, NULL);

	parameter = parameter;
}

/* EOF */
