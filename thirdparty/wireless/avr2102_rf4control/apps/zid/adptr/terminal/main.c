/**
 * @file main.c
 *
 * @brief Terminal Adaptor application
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
 * @author    Atmel Corporation: http://www.atmel.com
 * @author    Support: http://www.atmel.com/design-support/
 */

/*
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

/**
 * \mainpage
 * \section preface Preface
 * This is the reference manual for ZID Terminal adaptor application.
 * \section main_files Application Files
 * - main.c                      Application main file.
 * - vendor_data.c               Vendor Specific API functions
 * \section intro Application Introduction
 * Terminal Adaptor Example Application will act as adaptor for the RF4CE ZID
 * Class device application. Terminal adaptor stack will respond for the user
 * input via serial interface.
 * The user options will be printed on the serial console. Where as the user
 * will choose the options like cold start, warm start, reset(NIB will be reset
 * to default values and stored in EEPROM),
 * start network and push button pairing, print the pairing table,
 *
 *
 * options are user selectable via serial console, response will be printed on
 * the serial console after processing the requests.
 *
 * Push button pairing procedure can be triggered at target side either by using
 *"All-in-one start" option or "Reset"->"Start"->"ZID Connecting" sequence and
 * make
 * sure that the PBP is triggered at the ZID class device side also.
 * The status of the push button pairing procedure will be displayed on the
 * terminal.Then it displays ZID reports  received from the ZID Class device.
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

/* === Includes ============================================================ */
#include <ctype.h>
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
	WARM_STARTING,
	PRINTING_PAIRING_TABLE,
	UNPAIRING
} SHORTENUM node_status_t;

/* === Globals ============================================================= */

static zid_indication_callback_t zid_ind;
static nwk_indication_callback_t nwk_ind;
static uint8_t number_of_paired_dev = 0;
static node_status_t node_status;
static node_status_t previous_node_status;

/* This is used to find out the duplicate entry
 * on receiving the pairing confirmation */
static bool duplicate_pair_entry = false;
/* Write application specific values into flash memory */
FLASH_DECLARE(uint16_t VendorIdentifier) = (uint16_t)NWKC_VENDOR_IDENTIFIER;
FLASH_DECLARE(uint8_t vendor_string[7]) = NWKC_VENDOR_STRING;
FLASH_DECLARE(uint8_t app_user_string[15]) = APP_USER_STRING;
static uint8_t APP_TIMER;

/* === Prototypes ========================================================== */
static void zid_indication_callback_init(void);
static void nlme_unpair_indication(uint8_t PairingRef);
static void zid_heartbeat_indication(uint8_t PairingRef);
static void zid_connect_confirm(nwk_enum_t Status, uint8_t PairingRef);
static void zid_report_data_indication(uint8_t PairingRef,
		uint8_t num_report_records,
		zid_report_data_record_t *zid_report_data_record_ptr,
		uint8_t RxLinkQuality, uint8_t RxFlags);

static void nlme_rx_enable_confirm(nwk_enum_t Status);
static void nlme_reset_confirm(nwk_enum_t Status);
static void nlme_start_confirm(nwk_enum_t Status);
static void nlme_get_confirm(nwk_enum_t Status, nib_attribute_t NIBAttribute,
		uint8_t NIBAttributeIndex, void *NIBAttributeValue);

static void led_handling(void *callback_parameter);
static void app_task(void);
static void print_main_menu(void);
static void handle_input(uint8_t input_char);
static void print_node_status(void);
static void print_app_header(void);
static void print_pairing_table(bool start_from_scratch, uint8_t *table_entry,
		uint8_t index);
static void app_alert(void);

/* === Implementation ====================================================== */

/**
 * Main function, initialization and main message loop
 *
 *
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

	zid_indication_callback_init();

	/* Initialize LEDs. */
	LED_On(LED_START);     /* indicating application is started */
	LED_Off(LED_NWK_SETUP); /* indicating network is started */
	LED_Off(LED_DATA);     /* indicating data reception */

	/*
	 * The stack is initialized above, hence the global interrupts are
	 * enabled
	 * here.
	 */
	cpu_irq_enable();

#ifdef SIO_HUB

	/* Initialize the serial interface used for communication with terminal
	 * program. */

	sio2host_init();
#endif

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
	int8_t input_char;

	/* Check for incoming characters from terminal program. */
	input_char = sio2host_getchar_nowait();
	if (input_char != -1) {
		if (nwk_stack_idle()) {
			handle_input((uint8_t)toupper(input_char));
		} else {
			printf("Network stack is busy \r\n");
		}
	}
}

/**
 * @brief Print main menu to terminal program
 */
static void print_main_menu(void)
{
	printf("\n");
	print_app_header();
	printf("\r\n");
	print_node_status();
	printf("\r\n");
	printf("(R) : Reset (step 1) - Cold start\r\n");
	printf("(S) : Start (step 2)\r\n");
	printf("(Z) : ZID connecting (step 3)\r\n");
	printf("(A) : All-in-one start (Reset, start, pairing)\r\n");
	printf("(W) : Warm start - reset(0)\r\n");
	printf("(P) : Print pairing table\r\n");
	printf(">\r\n");
}

/**
 * @brief Print the head lines of the application to the terminal program
 */
static void print_app_header(void)
{
	printf("******************************************************\r\n");
	printf("Atmel RF4Control ZID Terminal Adaptor Application V0.1\r\n");
	printf("Configuration: ");
	printf("Adaptor, ");
}

/**
 * @brief Print the node status to the terminal program
 */
static void print_node_status(void)
{
	printf("Node status: ");

	switch (node_status) {
	case IDLE:
		printf("IDLE");
		break;

	case ZID_CONNECTING:
		printf("ZID - connecting");
		break;

	case ALL_IN_ONE_START:
		printf("All-in-one start");
		break;

	case RESETTING:
		printf("resetting");
		break;

	case STARTING:
		printf("starting");
		break;

	case PRINTING_PAIRING_TABLE:
		printf("printing paring table");
		break;

	default:
		printf("unknown status");
		break;
	}
	printf("  No.of paired devices: %d", number_of_paired_dev);
	printf("\r\n");

	nlme_get_request(nwkBaseChannel, 0, (FUNC_PTR)nlme_get_confirm);
}

/**
 * @brief Handle keyboard input
 *
 * @param input_char       character typed from input device.
 */
static void handle_input(uint8_t input_char)
{
	switch (input_char) {
	case 'R':
		printf("Reset node - \r\n");
		node_status = RESETTING;
		nlme_reset_request(true,
				(FUNC_PTR)nlme_reset_confirm

				);
		break;

	case 'S':
		printf("Start node - \r\n");
		node_status = STARTING;
		nlme_start_request(
				(FUNC_PTR)nlme_start_confirm
				);
		break;

	case 'Z':
	{
		dev_type_t RecDevTypeList[DEVICE_TYPE_LIST_SIZE];
		profile_id_t RecProfileIdList[PROFILE_ID_LIST_SIZE];

		RecDevTypeList[0] = (dev_type_t)SUPPORTED_DEV_TYPE_0;
		RecProfileIdList[0] = SUPPORTED_PROFILE_ID_0;
		printf("ZID-connecting - \r\n");
		node_status = ZID_CONNECTING;

		zid_rec_connect_request(APP_CAPABILITIES, RecDevTypeList,
				RecProfileIdList,
				(FUNC_PTR)zid_connect_confirm
				);
	}
	break;

	case 'A':
		printf("All-in-one start; wait until done.\r\n");
		node_status = ALL_IN_ONE_START;
		printf("Reset node - \r\n");
		nlme_reset_request(true,
				(FUNC_PTR)nlme_reset_confirm
				);
		break;

	case 'W':
		previous_node_status = node_status;
		printf("Warm start - \r\n");
		node_status = WARM_STARTING;
		nlme_reset_request(false,
				(FUNC_PTR)nlme_reset_confirm
				);
		break;

	case 'P':
		previous_node_status = node_status;
		node_status = PRINTING_PAIRING_TABLE;
		print_pairing_table(true, NULL, 0);
		break;

	default:
		print_main_menu();
		break;
	}
}

/**
 * @brief Notify the application of the status of its request for the value of a
 *        NIB attribute
 *
 * @param Status              nwk status
 * @param NIBAttribute        NIBAttribute
 * @param NIBAttributeIndex   NIBAttributeIndex
 * @param NIBAttributeValue   Value of the NIB attribute.
 */
#ifdef RF4CE_CALLBACK_PARAM
static
#endif
void nlme_get_confirm(nwk_enum_t Status, nib_attribute_t NIBAttribute,
		uint8_t NIBAttributeIndex, void *NIBAttributeValue)
{
	uint8_t channel;
	if (Status == NWK_SUCCESS) {
		switch (NIBAttribute) {
		case nwkPairingTable:
			if (node_status == WARM_STARTING) {
				pairing_table_t *table;
				table = (pairing_table_t *)NIBAttributeValue;
				if (table->DestinationLogicalChannel != 0) {
					number_of_paired_dev++;
				}
			} else {
				print_pairing_table(false, NIBAttributeValue,
						NIBAttributeIndex);
			}

#if (MAX_PAIRED_DEVICES > 1)
			if (NIBAttributeIndex < (MAX_PAIRED_DEVICES - 1)) {
				nlme_get_request(nwkPairingTable,
						NIBAttributeIndex + 1,
						(FUNC_PTR)nlme_get_confirm
						);
			} else {
				printf(
						"\n\r> Press Enter to return to main menu:\r\n ");
				node_status = previous_node_status;
			}

#else
			printf("> Press Enter to return to main menu:\r\n");
			node_status = previous_node_status;
#endif
			break;

		case nwkBaseChannel:

			channel = *((uint8_t *)NIBAttributeValue);
			printf("channel %uh\r\n", channel);
			break;

		default:
			break;
		}
	}
}

/**
 * @brief Print the pairing table to the terminal program
 *
 * @param start_from_scratch    Whether pairing table has to printed from
 * scratch.
 * @param table_entry           Table entry
 * @param index                 Index of the table entry to be printed.
 */
static void print_pairing_table(bool start_from_scratch, uint8_t *table_entry,
		uint8_t index)
{
	if (start_from_scratch) {
		printf("\n");
		printf("Pairing table:\r\n");

		nlme_get_request(nwkPairingTable, 0,
				(FUNC_PTR)nlme_get_confirm
				);
	} else {
		uint8_t i;
		pairing_table_t *table;
		uint8_t addr[8];

		table = (pairing_table_t *)table_entry;

		printf("Entry #%d\r\n", index);

		printf("Source network address: 0x%.2X%.2X",
				(uint8_t)((table->SourceNetworkAddress) >> 8),
				(uint8_t)(table->SourceNetworkAddress));

		printf(", destination logical channel: %d\r\n",
				table->DestinationLogicalChannel);
		printf("Destination IEEE address: 0x");
		memcpy(addr, &table->DestinationIEEEaddress, 8);
		for (i = 0; i < 8; i++) {
			printf("%.2X", addr[7 - i]);
		}
		printf("\r\n");

		printf("Destination PAN Id: 0x%.2X%.2X",
				(uint8_t)((table->DestinationPANidentifier) >> 8),
				(uint8_t)(table->DestinationPANidentifier));

		printf(", destination network address: 0x%.2X%.2X\r\n",
				(uint8_t)((table->DestinationNetworkAddress) >>
				8),
				(uint8_t)(table->DestinationNetworkAddress));
		printf("Recipient capabilities: 0x%.2X",
				table->RecipientCapabilities);

		printf(", Recipient frame counter %lu\r\n",
				table->RecipientFrameCounter);

		printf("Security link key: 0x");
		for (i = 0; i < 16; i++) {
			printf("%.2X", table->SecurityLinkKey[15 - i]);
		}
		printf("\r\n");
	}
}

/**
 * @brief Notify the application of the status of its request to reset the NWK
 *        layer.
 *
 * @param Status              nwk status
 */
#ifdef RF4CE_CALLBACK_PARAM
static
#endif
void nlme_reset_confirm(nwk_enum_t Status)
{
	printf("Node reset completed - status:%d\r\n", Status);

	if ((node_status == RESETTING) || (Status != NWK_SUCCESS)) {
		printf("> Press Enter to return to main menu: \r\n");
		node_status = IDLE;
		number_of_paired_dev = 0;
		return;
	} else if (node_status == WARM_STARTING) {
		nlme_rx_enable_request(RX_DURATION_INFINITY,
				(FUNC_PTR)nlme_rx_enable_confirm
				);
	} else if (node_status == ALL_IN_ONE_START) {
		printf("Start RF4CE network layer - \r\n");
		nlme_start_request(
				(FUNC_PTR)nlme_start_confirm
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
	printf("Node start completed - status :%0x\r\n", Status);
	if (node_status == STARTING) {
		printf("> Press Enter to return to main menu:\r\n");
		node_status = IDLE;
	}

	if (node_status == ALL_IN_ONE_START) {
		sw_timer_start(APP_TIMER,
				500000,
				SW_TIMEOUT_RELATIVE,
				(FUNC_PTR)led_handling,
				NULL);
		LED_On(LED_NWK_SETUP);

		dev_type_t RecDevTypeList[DEVICE_TYPE_LIST_SIZE];
		profile_id_t RecProfileIdList[PROFILE_ID_LIST_SIZE];

		RecDevTypeList[0] = (dev_type_t)SUPPORTED_DEV_TYPE_0;
		RecProfileIdList[0] = SUPPORTED_PROFILE_ID_0;
		printf("ZID-connecting - \r\n");
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
				500000,
				SW_TIMEOUT_RELATIVE,
				(FUNC_PTR)led_handling,
				NULL);
		LED_Toggle(LED_NWK_SETUP);
		break;

	default:
		sw_timer_stop(APP_TIMER);
		LED_Off(LED_DATA);
		LED_Off(LED_NWK_SETUP);
		break;
	}

	/* Keep compiler happy */
	callback_parameter = callback_parameter;
}

/**
 * @brief Notify the application of the status of its rx enable request.
 *
 *
 * @param Status  nwk status
 */
#ifdef RF4CE_CALLBACK_PARAM
static
#endif
void nlme_rx_enable_confirm(nwk_enum_t Status)
{
	if (node_status == WARM_STARTING) {
		nlme_get_request(nwkPairingTable, 0,
				(FUNC_PTR)nlme_get_confirm
				);
	}

	/* Keep compiler happy */
	Status = Status;
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
void nlme_unpair_indication(uint8_t PairingRef)
{
	printf(" Unpair indication from pairing ref:%d\r\n", PairingRef);
}

/**
 * @brief Notify the application of the status of its heartbeat request.
 *
 *
 * @param PairingRef  Pairing reference.
 */
void zid_heartbeat_indication(uint8_t PairingRef)
{
	printf("ZID-heartbeat from pairing ref:%d\r\n\r\n", PairingRef);
}

/**
 * @brief Notify the application of the status of its connect request.
 *
 * @param Status  nwk status.
 * @param PairingRef  Pairing reference.
 */
void zid_connect_confirm(nwk_enum_t Status, uint8_t PairingRef)
{
	node_status = IDLE;
	if (Status == NWK_SUCCESS) {
		if (duplicate_pair_entry == false) {
			number_of_paired_dev++;
		}
	}

	printf("ZID-connect status code:0x%x pairing ref:%d\r\n\r\n", Status,
			PairingRef);
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
void zid_report_data_indication(uint8_t PairingRef, uint8_t num_report_records,
		zid_report_data_record_t *zid_report_data_record_ptr,
		uint8_t RxLinkQuality, uint8_t RxFlags)
{
	printf("ZID - %d reports from pairing ref:%d\r\n", num_report_records,
			PairingRef);
	for (uint8_t i = 0; i < num_report_records; i++) {
		switch (zid_report_data_record_ptr->report_desc_identifier) {
		case MOUSE:
		{
			printf("Mouse:\r\n");
			mouse_desc_t *mouse_desc;
			mouse_desc
				= (mouse_desc_t *)zid_report_data_record_ptr->
					report_data;
			printf("B0:0x%x B1:0x%x B2:0x%x X:0x%x Y:0x%x\r\n",
					mouse_desc->button0,
					mouse_desc->button1,
					mouse_desc->button2, \
					mouse_desc->x_coordinate,
					mouse_desc->y_coordinate);
			printf("\r\n");
			break;
		}

		case KEYBOARD:
		{
			if (zid_report_data_record_ptr->report_type == INPUT) {
				printf("Keyboard input desc.:\r\n");
				keyboard_input_desc_t *keyboard_input_desc;
				keyboard_input_desc
					= (keyboard_input_desc_t *)
						zid_report_data_record_ptr
						->report_data;
				printf("Mod. key:0x%x\r\n",
						keyboard_input_desc->modifier_keys);

				printf("Key code:\r\n");
				for (uint8_t j = 0; j < 6; j++) {
					printf("0x%x ",
							keyboard_input_desc->key_code[
								j]);
				}
				printf("\r\n\r\n");
			} else {
				printf("Keyboard output desc: not valid\r\n");
			}

			break;
		}

		case CONTACT_DATA:
		{
			contact_data_report_t *contact_data_report;
			contact_data_report
				= (contact_data_report_t *)
					zid_report_data_record_ptr
					->report_data;

			printf("Contact data.:\r\n");
			printf(
					"Type:0x%x Index:0x%x State:0x%x Major_axis:0x%x Pressure:0x%x\r\n", contact_data_report->contact_type, contact_data_report->contact_index, \
					contact_data_report->contact_state, contact_data_report->major_axis_orientation,
					contact_data_report->pressure);
			printf(
					"Loc_x:0x%x Loc_y:0x%x Major_axis:0x%x Minor_axis:0x%x\r\n", contact_data_report->location_x, contact_data_report->location_y, \
					contact_data_report->major_axis_length,
					contact_data_report->minor_axis_length);

			printf("\r\n");

			break;
		}

		case TAP_GESTURE:
		{
			tap_gesture_report_t *tap_gesture_report;
			tap_gesture_report
				= (tap_gesture_report_t *)
					zid_report_data_record_ptr
					->report_data;

			printf("Tap gesture:\r\n");
			printf(
					"Finger_cnt:0x%x Type:0x%x Loc_x:0x%x Loc_y:0x%x\r\n", tap_gesture_report->finger_count, tap_gesture_report->type, \
					tap_gesture_report->location_x,
					tap_gesture_report->location_y);

			printf("\r\n");

			break;
		}

		case SCROLL_GESTURE:
		{
			scroll_gesture_report_t *scroll_gesture_report;
			scroll_gesture_report
				= (scroll_gesture_report_t *)
					zid_report_data_record_ptr
					->report_data;

			printf("Scroll gesture:\r\n");
			printf(
					"Finger_cnt:0x%x Type:0x%x Dir:0x%x Distance:0x%x\r\n", scroll_gesture_report->finger_count, scroll_gesture_report->type, \
					scroll_gesture_report->direction,
					scroll_gesture_report->distance);

			printf("\r\n");

			break;
		}

		case PINCH_GESTURE:
		{
			pinch_gesture_report_t *pinch_gesture_report;
			pinch_gesture_report
				= (pinch_gesture_report_t *)
					zid_report_data_record_ptr
					->report_data;

			printf("Pinch gesture:\r\n");
			printf(
					"Dir:0x%x Finger_pre:0x%x Distance:0x%x Center_x:0x%x Center_y:0x%x\r\n", pinch_gesture_report->direction, pinch_gesture_report->finger_present, \
					pinch_gesture_report->distance, pinch_gesture_report->center_x,
					pinch_gesture_report->center_y);

			printf("\r\n");

			break;
		}

		case ROTATE_GESTURE:
		{
			rotation_gesture_report_t *rotation_gesture_report;
			rotation_gesture_report
				= (rotation_gesture_report_t *)
					zid_report_data_record_ptr
					->report_data;

			printf("Rotate gesture:\r\n");
			printf("Dir:0x%x Finger_pre:0x%x Mag:0x%x\r\n",
					rotation_gesture_report->direction,
					rotation_gesture_report->finger_present, \
					rotation_gesture_report->magnitude);

			printf("\r\n");

			break;
		}

		case SYNC:
		{
			sync_report_t *sync_report;
			sync_report
				= (sync_report_t *)zid_report_data_record_ptr
					->report_data;

			printf("Sync:\r\n");
			printf("Contact_cnt:0x%x Gesture:0x%x\r\n",
					sync_report->contact_count,
					sync_report->gesture);

			printf("\r\n");

			break;
		}

		case TOUCH_SENSOR_PROPERTIES:
		{
			touch_sensor_properties_t *touch_sensor_properties;
			touch_sensor_properties
				= (touch_sensor_properties_t *)
					zid_report_data_record_ptr
					->report_data;

			printf("Touch sensor properties:\r\n");
			printf(
					"Add_contatcs:0x%x Origin:0x%x Rel_index:0x%x Gesture:0x%x Res_x:0x%x Res_y:0x%x\r\n", touch_sensor_properties->no_of_additional_contacts, touch_sensor_properties->origin, \
					touch_sensor_properties->reliable_index, touch_sensor_properties->gestures, touch_sensor_properties->resolution_x,
					touch_sensor_properties->resolution_y);

			printf("Max_coor_x:0x%x Max_coor_x:0x%x Shape:0x%x\r\n",
					touch_sensor_properties->max_coordinate_x,
					touch_sensor_properties->max_coordinate_y, \
					touch_sensor_properties->shape);

			printf("\r\n");

			break;
		}

		case TAP_SUPPORT_PROPERTIES:
		{
			tap_support_properties_t *tap_support_properties;
			tap_support_properties
				= (tap_support_properties_t *)
					zid_report_data_record_ptr
					->report_data;

			printf("Tap support properties:\r\n");
			printf(
					"Single_tap:0x%x Tap_half:0x%x Double_tap:0x%x Long_tap:0x%x\r\n", tap_support_properties->single_tap, tap_support_properties->tap_and_a_half, \
					tap_support_properties->double_tap,
					tap_support_properties->long_tap);

			printf("\r\n");

			break;
		}
		}
		zid_report_data_record_ptr++;
	}

	RxLinkQuality = RxLinkQuality;
	RxFlags = RxFlags;
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
