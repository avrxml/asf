/**
 * @file main.c
 *
 * @brief Terminal Target application
 *
 * Copyright (c) 2013-2015 Atmel Corporation. All rights reserved.
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

/**
 * \mainpage
 * \section preface Preface
 * This is the reference manual for ZRC Terminal target application.
 * \section main_files Application Files
 * - main.c                      Application main file.
 * - vendor_data.c               Vendor Specific API functions
 * \section intro Application Introduction
 * Terminal Target Example Application will act as target for the RF4CE ZRC
 * button controller application. Terminal Target stack will respond for the
 * user
 * input via serial interface.
 * The user options will be printed on the serial console. Where as the user
 * will choose the options like cold start, warm start, reset(NIB will be reset
 * to default values and stored in EEPROM),
 * start network and push button pairing, channel agility, vendor data support,
 * unpair, print the pairing table, setting the base channel on the target
 * device, toggle the standby mode on the target device.
 *
 * options are user selectable via serial console, response will be printed on
 * the serial console after processing the requests.
 *
 * Push button pairing procedure can be triggered at target side either by using
 *"All-in-one start" option or "Reset"->"Start"->"Pairing" sequence and make
 * sure that the PBP is triggered at the controller side also.
 * The status of the push button pairing proceure will be displayed on the
 * terminal.Then it displays ZRC commands received from the controller.
 *
 * Terminal target can be used with the single button controller node and button
 * controller nodes.
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

#include <ctype.h>
#include "conf_board.h"
#include <asf.h>
#include "app_config.h"
#if (!defined SAMD20) || (!defined SAMD21) || (!defined SAMR21)
#include "led.h"
#endif
#include "delay.h"
#include "vendor_data.h"
#include "pb_pairing.h"
#include "common_sw_timer.h"
#include "sio2host.h"

/* === TYPES =============================================================== */

typedef enum node_status_tag {
	IDLE = 0,
	RESETTING,
	POWER_SAVE,
	STARTING,
	PUSH_BUTTON_PAIRING,
#ifdef ZRC_CMD_DISCOVERY
	CMD_DISCOVERING,
#endif
	ALL_IN_ONE_START,
	WARM_STARTING,
	PRINTING_PAIRING_TABLE,
	UNPAIRING,
	CH_AGILITY_EXECUTION,
	BASE_CHANNEL_CHANGE,
	GETTING_CH_AG_NIBS
} SHORTENUM node_status_t;

/* === MACROS ============================================================== */

#define MAX_PAIRED_DEVICES  NWKC_MAX_PAIRING_TABLE_ENTRIES

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
#endif

/* === GLOBALS ============================================================= */

FLASH_DECLARE(uint16_t VendorIdentifier) = (uint16_t)NWKC_VENDOR_IDENTIFIER;
FLASH_DECLARE(uint8_t vendor_string[7]) = NWKC_VENDOR_STRING;
FLASH_DECLARE(uint8_t app_user_string[15]) = APP_USER_STRING;
#ifdef ZRC_CMD_DISCOVERY
FLASH_DECLARE(uint8_t supported_cec_cmds[32]) = SUPPORTED_CEC_CMDS;
#endif

static uint8_t number_of_paired_dev = 0;
static node_status_t node_status;
static node_status_t previous_node_status;
static bool ch_ag_enabled = false;
static uint8_t nwk_Private_ChAgEdThreshold;
static uint32_t nwk_Private_ChAgScanInterval;
static uint8_t nwk_ScanDuration;

/* This is used to find out the duplicate entry
 * on receiving the pairing confirmation */
static bool duplicate_pair_entry = false;

static uint8_t led_timer;

/* === EXTERNALS =========================================================== */

/* === PROTOTYPES ========================================================== */

static void app_task(void);
static void print_main_menu(void);
static void handle_input(uint8_t input_char);
static void print_node_status(void);
static void print_app_header(void);
static void print_pairing_table(bool start_from_scratch, uint8_t *table_entry,
		uint8_t index);
static void print_unpair_submenu(void);
static void led_handling(void *callback_parameter);
static void print_ch_change_submenu(void);
static void print_sub_mode_ch_ag_setup(void);
static void print_vendor_data_submenu(vendor_cmd_id_t Vcmd);
static char *get_status_text(nwk_enum_t status);

static void nlme_rx_enable_confirm(nwk_enum_t Status);
static void nlme_reset_confirm(nwk_enum_t Status);
static void nlme_start_confirm(nwk_enum_t Status);
static void nwk_ch_agility_confirm(nwk_enum_t Status, bool ChannelChanged,
		uint8_t LogicalChannel);
static void nlme_get_confirm(nwk_enum_t Status, nib_attribute_t NIBAttribute,
		uint8_t NIBAttributeIndex, void *NIBAttributeValue);
static void pbp_rec_pair_confirm(nwk_enum_t Status, uint8_t PairingRef);
static void nlme_unpair_confirm(uint8_t Status, uint8_t PairingRef);
static void zrc_cmd_disc_confirm(nwk_enum_t Status, uint8_t PairingRef,
		uint8_t *SupportedCmd);
void vendor_data_confirm(nwk_enum_t Status, uint8_t PairingRef,
		profile_id_t ProfileId,
		uint8_t Handle
		);

static void nwk_ch_agility_indication(uint8_t LogicalChannel);
static void nlme_unpair_indication(uint8_t PairingRef);
static void zrc_cmd_indication(uint8_t PairingRef, uint8_t nsduLength,
		uint8_t *nsdu,
		uint8_t RxLinkQuality, uint8_t RxFlags);

#ifdef ZRC_CMD_DISCOVERY
static void zrc_cmd_disc_indication(uint8_t PairingRef);

#endif

static void vendor_data_ind(uint8_t PairingRef, uint16_t VendorId,
		uint8_t nsduLength, uint8_t *nsdu, uint8_t RxLinkQuality,
		uint8_t RxFlags);

static zrc_indication_callback_t zrc_ind;
static nwk_indication_callback_t nwk_ind;

static void app_alert(void);

/* === IMPLEMENTATION ====================================================== */

/**
 * @brief Main function of the Terminal Target application
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

	zrc_ind.vendor_data_ind_cb = vendor_data_ind;

#ifdef ZRC_CMD_DISCOVERY
	zrc_ind.zrc_cmd_disc_indication_cb =  zrc_cmd_disc_indication;
#endif
	zrc_ind.zrc_cmd_indication_cb = zrc_cmd_indication;

	register_zrc_indication_callback(&zrc_ind);

	nwk_ind.nwk_ch_agility_indication_cb = nwk_ch_agility_indication;
	nwk_ind.nlme_unpair_indication_cb = nlme_unpair_indication;
	register_nwk_indication_callback(&nwk_ind);

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

	sw_timer_get_id(&led_timer);

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
 * @brief Handle keyboard input
 *
 * @param input_char       character typed from input device.
 */
static void handle_input(uint8_t input_char)
{
	/* We allow user input if we are either in IDLE state ot POWER_SAVE
	 * state
	 * In case of POWER_SAVE state, we allow only reset & disabling
	 * POWER_SAVE req*/
	if (((node_status != IDLE) && (node_status != POWER_SAVE)) ||
			((node_status == POWER_SAVE) &&
			(!((input_char == 'Y') || (input_char == 'R') ||
			(input_char == 'A') || (input_char == 'W') ||
			(input_char == 0x0D))))) {
		printf(
				"Node is in power save mode.Press (R) to Reset/Press (Y) to Disable power save mode.\r\n");
		return;
	}

	switch (input_char) {
	case 'Y':
		if (node_status == POWER_SAVE) {
			printf(
					"Leaving standby (power save mode). Press Enter to return to main menu.\r\n ");
			nlme_rx_enable_request(RX_DURATION_INFINITY,
					(FUNC_PTR)nlme_rx_enable_confirm
					);
			node_status = IDLE;
		} else {
			printf(
					"Entering standby (power save mode). Press Enter to return to main menu.\r\n ");
			nlme_rx_enable_request(nwkcMinActivePeriod,
					(FUNC_PTR)nlme_rx_enable_confirm
					);
			node_status = POWER_SAVE;
		}

		break;

	case 'R':
		printf("Reset node - \r\n");
		node_status = RESETTING;
		ch_ag_enabled = false;
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

	case 'P':
		printf("Push button pairing -\r\n");
		node_status = PUSH_BUTTON_PAIRING;
		sw_timer_start(led_timer,
				500000,
				SW_TIMEOUT_RELATIVE,
				(FUNC_PTR)led_handling,
				NULL);
		LED_On(LED_NWK_SETUP);
		{
			dev_type_t RecDevTypeList[DEVICE_TYPE_LIST_SIZE];
			profile_id_t RecProfileIdList[PROFILE_ID_LIST_SIZE];

			RecDevTypeList[0] = (dev_type_t)SUPPORTED_DEV_TYPE_0;
			RecProfileIdList[0] = SUPPORTED_PROFILE_ID_0;

			pbp_rec_pair_request(APP_CAPABILITIES, RecDevTypeList,
					RecProfileIdList,
					(FUNC_PTR)pbp_rec_pair_confirm
					);
		}
		break;

	case 'A':
		printf("All-in-one start; wait until done.\r\n");
		node_status = ALL_IN_ONE_START;
		printf("\tReset node - ");
		ch_ag_enabled = false;
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

	case 'T':
		previous_node_status = node_status;
		node_status = PRINTING_PAIRING_TABLE;
		print_pairing_table(true, NULL, 0);
		break;

	case 'U':
		node_status = UNPAIRING;
		print_unpair_submenu();
		break;

	case 'C':
		if (ch_ag_enabled) {
			ch_ag_enabled = false;
			nwk_ch_agility_request(AG_STOP,
					(FUNC_PTR)nwk_ch_agility_confirm
					);
			printf(" - Channel agility is stopped ...\r\n");
		} else {
			ch_ag_enabled = true;
			node_status = CH_AGILITY_EXECUTION;
			printf(" - Channel agility is started ...\r\n");
			nwk_ch_agility_request(AG_PERIODIC,
					(FUNC_PTR)nwk_ch_agility_confirm
					);
		}

		break;

	case 'O':
		/* Start getting the NIBs value */
		node_status = GETTING_CH_AG_NIBS;
		nlme_get_request(nwkPrivateChAgEdThreshold, 0,
				(FUNC_PTR)nlme_get_confirm
				);
		break;

	case 'B':
		print_ch_change_submenu();
		break;

	case 'D':
		print_vendor_data_submenu(BATTERY_STATUS_REQ);
		break;

	case 'V':
		print_vendor_data_submenu(FW_VERSION_REQ);
		break;

	case 'Z':
		print_vendor_data_submenu(ALIVE_REQ);
		break;

	default:
		print_main_menu();
		break;
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
	printf("(P) : Push button pairing (step 3)\r\n");
	printf("(A) : All-in-one start (Reset, start, pairing)\r\n");
	printf("(W) : Warm start - reset(0)\r\n");
	printf("(T) : Print pairing table\r\n");
	printf("(U) : Unpair\r\n");
	printf("(C) : Channel agility (periodic mode) - enable/disable\r\n");
	printf("(O) : Channel agility configuration\r\n");
	printf("(B) : Base channel change\r\n");
	printf("(Y) : Power save mode - enable/disable\r\n");
	printf("(D) : Send remote battery status request\r\n");
	printf("(V) : Send remote firmware version request\r\n");
	printf("(Z) : Send alive request\r\n");
	printf(">\r\n");
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

	case POWER_SAVE:
		printf("power save mode");
		break;

	case PUSH_BUTTON_PAIRING:
		printf("push button pairing");
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

	case UNPAIRING:
		printf("unpairing");
		break;

	case CH_AGILITY_EXECUTION:
		printf("channel agility scan");
		break;

	default:
		printf("unknown status");
		break;
	}
	printf("  No.of paired devices: %d", number_of_paired_dev);
	printf("\r\n");
	nlme_get_request(nwkBaseChannel, 0, (FUNC_PTR)nlme_get_confirm);
	printf("Channel agility (periodic mode) ");
	if (ch_ag_enabled) {
		printf("enabled\r\n");
	} else {
		printf("disabled\r\n");
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
#ifdef BIG_ENDIAN
		printf("Source network address: 0x%.2X%.2X",
				(uint8_t)(table->SourceNetworkAddress),
				(uint8_t)((table->SourceNetworkAddress) >> 8));
#else
		printf("Source network address: 0x%.2X%.2X",
				(uint8_t)((table->SourceNetworkAddress) >> 8),
				(uint8_t)(table->SourceNetworkAddress));
#endif
		printf(", destination logical channel: %d\r\n",
				table->DestinationLogicalChannel);
		printf("Destination IEEE address: 0x");
		memcpy(addr, &table->DestinationIEEEaddress, 8);
		for (i = 0; i < 8; i++) {
			printf("%.2X", addr[7 - i]);
		}
		printf("\r\n");
#ifdef BIG_ENDIAN
		printf("Destination PAN Id: 0x%.2X%.2X",
				(uint8_t)(table->DestinationPANidentifier),
				(uint8_t)((table->DestinationPANidentifier) >>
				8));
#else
		printf("Destination PAN Id: 0x%.2X%.2X",
				(uint8_t)((table->DestinationPANidentifier) >> 8),
				(uint8_t)(table->DestinationPANidentifier));
#endif
#ifdef BIG_ENDIAN
		printf(", destination network address: 0x%.2X%.2X\r\n",
				(uint8_t)(table->DestinationNetworkAddress),
				(uint8_t)((table->DestinationNetworkAddress) >>
				8));
#else
		printf(", destination network address: 0x%.2X%.2X\r\n",
				(uint8_t)((table->DestinationNetworkAddress) >>
				8),
				(uint8_t)(table->DestinationNetworkAddress));
#endif
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
 * @brief Print the head lines of the application to the terminal program
 */
static void print_app_header(void)
{
	printf("******************************************************\r\n");
	printf("Atmel RF4Control Terminal Target Application\r\n");
	printf("Configuration: ");
	printf("Target (e.g. TV), ");

	printf(BOARD_NAME);
}

/**
 * @brief Print the unpairing submenu to the terminal program
 */
static void print_unpair_submenu(void)
{
	char input_char;

	printf("Which device should be unpaired? Pairing Ref = \r\n");
	input_char = (char)sio2host_getchar();

	if ((input_char >= '0') && (input_char <= '9')) {
		uint8_t pair_ref = input_char - 0x30;
		nlme_unpair_request(pair_ref,
				(FUNC_PTR)nlme_unpair_confirm
				);
	} else {
		node_status = IDLE;
		printf("Unknown paring reference\r\n");
		printf("> Press Enter to return to main menu:\r\n ");
	}
}

/**
 * @brief Handles the rc command indications at terminal target.
 *
 * @param PairingRef       Pairing reference
 * @param nsduLength       Length of the payload.
 * @param nsdu             Actual payload
 * @param RxLinkQuality    Link quality of received packet.
 * @param RxFlags          Rx Flags.
 */
static void zrc_cmd_indication(uint8_t PairingRef, uint8_t nsduLength,
		uint8_t *nsdu,
		uint8_t RxLinkQuality, uint8_t RxFlags)
{
	zrc_cmd_frm_t *zrc_frm;

	if (node_status == POWER_SAVE) {
		printf("Leaving power save mode.\r\n");
		nlme_rx_enable_request(RX_DURATION_INFINITY,
				(FUNC_PTR)nlme_rx_enable_confirm
				);
		node_status = IDLE;
	}

	/* Switch LED on indicating data reception */
	LED_On(LED_DATA);
	sw_timer_start(led_timer, 250000, SW_TIMEOUT_RELATIVE,
			(FUNC_PTR)led_handling, NULL);

	/* Check with frame control field which kind of data is indicated */
	zrc_frm = (zrc_cmd_frm_t *)nsdu;
	switch (zrc_frm->fcf) {
	case USER_CONTROL_PRESSED:
	{
		printf("Rx: ");
		printf("%s", zrc_print_rc_cmd_text(zrc_frm->rc_cmd));
		printf(" Press (0x%.2X), ", zrc_frm->rc_cmd);
		printf("from %d, LQI = 0x%.2X\r\n", PairingRef, RxLinkQuality);
	}
	break;

	case USER_CONTROL_REPEATED:
		printf("Rx: ");
		zrc_print_rc_cmd_text(zrc_frm->rc_cmd);
		printf(" Repeat (0x%.2X), ", zrc_frm->rc_cmd);
		printf("from %d, LQI = 0x%.2X\r\n", PairingRef, RxLinkQuality);
		break;

	case USER_CONTROL_RELEASED:
		printf("Rx: ");
		zrc_print_rc_cmd_text(zrc_frm->rc_cmd);
		printf(" Release (0x%.2X), ", zrc_frm->rc_cmd);
		printf("from %d, LQI = 0x%.2X\r\n", PairingRef, RxLinkQuality);
		break;

	default:
		break;
	}

	/* Keep compiler happy */
	nsduLength = nsduLength;
	RxFlags = RxFlags;
}

/**
 * @brief Prints the status of push button pairing and if status is success,
 *        then send the cmd discovery to controller.
 *
 * @param Status           nwk status
 * @param PairingRef       Pairing Ref of the new entry.
 */
static void pbp_rec_pair_confirm(nwk_enum_t Status, uint8_t PairingRef)
{
	node_status = IDLE;
	printf(
			"Push button pairing completed\r\nStatus - %s (0x%02X), Pairing Ref 0x%02X\r\n",
			get_status_text((nwk_enum_t)Status), Status,
			PairingRef);

	if (Status == NWK_SUCCESS) {
		if (duplicate_pair_entry == false) {
			number_of_paired_dev++;
		}

#ifdef ZRC_CMD_DISCOVERY
		printf("Sending ZRC command discovery request\r\n");
		node_status = CMD_DISCOVERING;
		zrc_cmd_disc_request(PairingRef,
				(FUNC_PTR)zrc_cmd_disc_confirm
				);
#endif
	}
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
		uint16_t OrgVendorId, uint8_t OrgVendorString[7],
		uint8_t OrgUserString[15], uint8_t KeyExTransferCount)
{
	duplicate_pair_entry = false;
	if (Status == NWK_DUPLICATE_PAIRING) {
		duplicate_pair_entry = true;
	}

	/* Keep compiler happy */
	SrcIEEEAddr = SrcIEEEAddr;
	OrgVendorId = OrgVendorId;
	OrgVendorString[0] = OrgVendorString[0];
	OrgUserString[0] = OrgUserString[0];
	KeyExTransferCount = KeyExTransferCount;

	return true;
}

/**
 * @brief The command discovery confirm for the previous command discovery
 * request.
 *        request command was receiced.
 *
 * @param Status           nwk status
 * @param PairingRef       Pairing Ref for destination.
 * @param SupportedCmd     Supported commands by destination node.
 */
#ifdef ZRC_CMD_DISCOVERY
static void zrc_cmd_disc_confirm(nwk_enum_t Status, uint8_t PairingRef,
		uint8_t *SupportedCmd)
{
	printf("ZRC command discovery confirm, Status = %s (0x%02X)\r\n",
			get_status_text((nwk_enum_t)Status), Status);

	node_status = IDLE;

	/* Keep compiler happy */
	Status = Status;
	PairingRef = PairingRef;
	SupportedCmd = SupportedCmd;
}

#endif

/**
 * @brief The command discovery indication callback indicates that a command
 * discovery
 *        request command was receiced.
 *
 * @param PairingRef          Pairing Ref for the source.
 */
#ifdef ZRC_CMD_DISCOVERY
static void zrc_cmd_disc_indication(uint8_t PairingRef)
{
	/* Send back the response */
	uint8_t cec_cmds[32];

	PGM_READ_BLOCK(cec_cmds, supported_cec_cmds, 32);
	zrc_cmd_disc_response(PairingRef, cec_cmds);
}

#endif

/**
 * @brief Notify the application of the status of its request for the value of a
 *        NIB attribute
 *
 * @param Status              nwk status
 * @param NIBAttribute        NIBAttribute
 * @param NIBAttributeIndex   NIBAttributeIndex
 * @param NIBAttributeValue   Value of the NIB attribute.
 */
static void nlme_get_confirm(nwk_enum_t Status, nib_attribute_t NIBAttribute,
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
						"> Press Enter to return to main menu: \r\n");
				node_status = previous_node_status;
			}

#else
			printf("> Press Enter to return to main menu: \r\n");
			node_status = previous_node_status;
#endif
			break;

		case nwkPrivateChAgEdThreshold:
			memcpy(&nwk_Private_ChAgEdThreshold, NIBAttributeValue,
					1);
			nlme_get_request(nwkPrivateChAgScanInterval, 0,
					(FUNC_PTR)nlme_get_confirm
					);
			break;

		case nwkPrivateChAgScanInterval:
			memcpy(&nwk_Private_ChAgScanInterval, NIBAttributeValue,
					sizeof(uint32_t));
			nlme_get_request(nwkScanDuration, 0,
					(FUNC_PTR)nlme_get_confirm
					);
			break;

		case nwkScanDuration:
			memcpy(&nwk_ScanDuration, NIBAttributeValue, 1);
			node_status = IDLE;
			print_sub_mode_ch_ag_setup();
			break;

		case nwkBaseChannel:
			channel = *((uint8_t *)NIBAttributeValue);
			/* printf("channel %u\r\n",channel); */
			break;

		default:
			break;
		}
	}
}

/**
 * @brief Notify the application of the status of its request to reset the NWK
 *        layer.
 *
 * @param Status              nwk status
 */
static void nlme_reset_confirm(nwk_enum_t Status)
{
	printf("Node reset completed - %s (0x%.2X)\r\n",
			get_status_text((nwk_enum_t)Status), Status);
	number_of_paired_dev = 0;

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
		printf("\tStart RF4CE network layer - \r\n");
		nlme_start_request(
				(FUNC_PTR)nlme_start_confirm
				);
	}
}

/**
 * @brief Notify the application of the status of its request to enable or
 *        disable the receiver.
 *
 * @param Status              nwk status
 */
static void nlme_rx_enable_confirm(nwk_enum_t Status)
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
 * @brief Notify the application of the status of its request to to change the
 *        value of a NIB attribute.
 *
 * @param Status              nwk status
 * @param NIBAttribute        NIBAttribute
 * @param NIBAttributeIndex   NIBAttributeIndex
 */
static void nlme_set_confirm(nwk_enum_t Status, nib_attribute_t NIBAttribute,
		uint8_t NIBAttributeIndex)
{
	if (Status != NWK_SUCCESS) {
		printf("Set confirm status - %s (0x%.2X)\r\n",
				get_status_text((nwk_enum_t)Status), Status);
		return;
	}

	if (NIBAttribute == nwkBaseChannel) {
		printf(
				"Base channel changed - press Enter to return to main menu.\r\n");
		node_status = IDLE;
		return;
	}

	/* Keep compiler happy */
	NIBAttributeIndex = NIBAttributeIndex;
}

/**
 * @brief Notify the application of the status of its request to start a
 * network.
 *
 * The NLME-START.confirm primitive allows the NLME to notify the application of
 * the status of its request to start a network.
 *
 * @param Status       nwk status
 */
static void nlme_start_confirm(nwk_enum_t Status)
{
	printf("Node start completed - %s (0x%.2X)\r\n",
			get_status_text((nwk_enum_t)Status), Status);
	if (node_status == STARTING) {
		printf("> Press Enter to return to main menu: \r\n");
		node_status = IDLE;
	}

	if (node_status == ALL_IN_ONE_START) {
		printf("\tPush button pairing.\r\n");
		printf(
				"\tPress SEL key then keeping SEL pressed press any FUNC key.\r\n");
		printf(
				"\tThis starts the push button pairing at the remote controller.\r\n");
		sw_timer_start(led_timer,
				500000,
				SW_TIMEOUT_RELATIVE,
				(FUNC_PTR)led_handling,
				NULL);
		LED_On(LED_NWK_SETUP);

		dev_type_t RecDevTypeList[DEVICE_TYPE_LIST_SIZE];
		profile_id_t RecProfileIdList[PROFILE_ID_LIST_SIZE];

		RecDevTypeList[0] = (dev_type_t)SUPPORTED_DEV_TYPE_0;
		RecProfileIdList[0] = SUPPORTED_PROFILE_ID_0;

		pbp_rec_pair_request(APP_CAPABILITIES, RecDevTypeList,
				RecProfileIdList,
				(FUNC_PTR)pbp_rec_pair_confirm
				);
	}
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
	/* Keep compiler happy */
	PairingRef = PairingRef;
}

/**
 * @brief Notify the application for the previous unpair request.
 *
 * The NLME-UNPAIR.confirm primitive allows the NLME to notify the application
 * of
 * the status of its request to remove a pair with another device.
 *
 * @param Status           nwk status
 * @param PairingRef       Pairing Ref for which entry is removed from pairing
 * table.
 */
static void nlme_unpair_confirm(uint8_t Status, uint8_t PairingRef)
{
	if (node_status == UNPAIRING) {
		printf("Unpairing completed - %s(0x%.2X)\r\n",
				get_status_text((nwk_enum_t)Status), Status);
		printf("> Press Enter to return to main menu: \r\n");
		node_status = IDLE;
		if (Status != NWK_NO_PAIRING) {
			number_of_paired_dev--;
		}
	}

	/* Keep compiler happy */
	Status = Status;
	PairingRef = PairingRef;
}

/**
 * @brief Confirms the previous channel agility request, i.e.
 * nwk_ch_agility_req()
 *
 * @param Status           nwk status
 * @param ChannelChanged   whether channel is changed.
 * @param LogicalChannel   changed logical channel.
 */
static void nwk_ch_agility_confirm(nwk_enum_t Status, bool ChannelChanged,
		uint8_t LogicalChannel)
{
	printf("Channel agility confirm: ");
	if (ch_ag_enabled) {
		printf("enabled\r\n");
	} else {
		printf("disabled\r\n");
	}

	printf("Status = %s (0x%.2X), channel ",
			get_status_text((nwk_enum_t)Status), Status);
	if (!ChannelChanged) {
		printf("not ");
	}

	printf("changed, current channel %d\r\n", LogicalChannel);
	printf("> Press Enter to return to main menu: \r\n");
	node_status = IDLE;
}

/**
 * @brief Prints the new channel after channel agility took place.
 *
 * @param LogicalChannel  changed logical channel.
 */
static void nwk_ch_agility_indication(uint8_t LogicalChannel)
{
	printf("Channel agility - new channel: %d\r\n", LogicalChannel);
}

/**
 * @brief Prints the sub menu for the base channel change.
 */
static void print_ch_change_submenu(void)
{
	char input_char[3] = {0, 0, 0};
	uint8_t channel;
	uint8_t i;
	uint8_t input;

#if (RF_BAND == BAND_2400)
	printf("Enter new base channel (15, 20, or 25): \r\n");
#else
	printf("Enter new base channel (1, 4, or 7): \r\n");
#endif

	for (i = 0; i < 3; i++) {
		input = (char)sio2host_getchar();
		if (isdigit(input)) {
			input_char[i] = input;
		} else {
			break;
		}
	}
	channel = atol(input_char);

#if (RF_BAND == BAND_2400)
	if ((channel == 15) || (channel == 20) || (channel == 25))
#else
	if ((channel == 1) || (channel == 4) || (channel == 7))
#endif
	{
		nlme_set_request(nwkBaseChannel, 0, &channel,
				(FUNC_PTR)nlme_set_confirm
				);

		node_status = BASE_CHANNEL_CHANGE;
	} else {
		printf(
				"Entered channel is not valid - press Enter to return to main menu.\r\n ");
		return;
	}
}

/**
 * @brief LED handling including timer control .
 */
static void led_handling(void *callback_parameter)
{
	switch (node_status) {
	case PUSH_BUTTON_PAIRING:
	case ALL_IN_ONE_START:
		sw_timer_start(led_timer,
				500000,
				SW_TIMEOUT_RELATIVE,
				(FUNC_PTR)led_handling,
				NULL);
		LED_Toggle(LED_NWK_SETUP);
		break;

	default:
		sw_timer_stop(led_timer);
		LED_Off(LED_DATA);
		LED_Off(LED_NWK_SETUP);
		break;
	}

	/* Keep compiler happy */
	callback_parameter = callback_parameter;
}

/**
 * @brief Prints the sub menu for the channel agility.
 */
static void print_sub_mode_ch_ag_setup(void)
{
	int8_t input_char;
	uint8_t input;

	printf("\n");
	printf(
			"Configuration of channel agility, select a configration parameter\r\n");
	printf("(Scan interval > 4*scan duration)\r\n\r\n");
	printf("(F) : Noise threshold, current value: %d\r\n",
			nwk_Private_ChAgEdThreshold);
	printf("(G) : Scan interval, current value: 0x%.8lX\r\n",
			nwk_Private_ChAgScanInterval);
	printf("(E) : Scan duration, current value: %d\r\n", nwk_ScanDuration);
	printf(">\r\n");
	/* Check for incoming characters from terminal program. */
	while (1) {
		input_char = sio2host_getchar_nowait();
		if (input_char != -1) {
			break;
		}
	}

	switch (toupper(input_char)) {
	case 'F': /* Noise Threshold */
	{
		char input_char2[3] = {0, 0, 0};
		uint8_t threshold;
		printf(
				"Enter new noise threshold (Valid Range:0 = -91dBm to 84 = -35dBm).\r\n");
		printf("Default: 10 = -80 dBm, new value: \r\n");
		for (uint8_t i = 0; i < 2; i++) {
			input = (char)sio2host_getchar();
			if (isdigit(input)) {
				input_char2[i] = input;
			} else if (input == 0x0D) {
				break;
			} else {
				printf("Invalid value. \r\n\r\n");
				printf(
						"> Press Enter to return to main menu:\r\n ");
				return;
			}
		}
		threshold = atol(input_char2);
		if (threshold <= 84) {
			nlme_set_request(nwkPrivateChAgEdThreshold, 0,
					&threshold,
					(FUNC_PTR)nlme_set_confirm
					);
		} else {
			printf("Invalid threshold value. \r\n\r\n");
			printf("> Press Enter to return to main menu:\r\n ");
			return;
		}
	}
	break;

	case 'G':   /* Scan interval */
	{
		char input_char2[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
		char input_char3[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
		uint32_t threshold = 0;
		printf(
				"Enter new scan interval (0x00000000 ... 0xFFFFFFFF), enter 32-bit value \r\n");
		printf(
				"Default: 0x00393870 symbols = 60 sec, new value: 0x \r\n");
		for (uint8_t i = 0; i < 8; i++) {
			input = (char)sio2host_getchar();
			input = (uint8_t)toupper(input);
			if (((input >= '0') && (input <= '9')) ||
					((input >= 'A') && (input <= 'F'))) {
				input_char2[i] = input;
			} else {
				if (input == 0x0D) {
					break;
				} else {
					printf("Invalid value. \r\n\r\n");
					printf(
							"> Press Enter to return to main menu:\r\n ");
					return;
				}
			}
		}

		for (uint8_t i = 7; i > 0; i--) {
			if (input_char2[i] != 0x00) {
				memcpy(&input_char3[7 - i], input_char2, i + 1);
				break;
			}
		}
		for (uint8_t i = 0; i < 8; i++) {
			if (input_char3[i] != 0x00) {
				if ((input_char3[i] >= '0') &&
						(input_char3[i] <=
						'9')) {
					input_char3[i] -= '0';
				} else {
					input_char3[i] -= 55;
				}

				threshold += (uint32_t)input_char3[i] <<
						((7 - i) * 4);
			}
		}
		nlme_set_request(nwkPrivateChAgScanInterval, 0,
				(uint8_t *)&threshold,
				(FUNC_PTR)nlme_set_confirm
				);
	}
	break;

	case 'E':
	{
		char input_char2[3] = {0, 0, 0};
		uint8_t scan_dur;
		printf(
				"Enter new scan duration (0 = 30 msec, 6 = 1 sec, 14 = 14 min).\r\n");
		printf("Default: 6 = 1 sec, new value: \r\n");
		for (uint8_t i = 0; i < 2; i++) {
			input = (char)sio2host_getchar();
			if (isdigit(input)) {
				input_char2[i] = input;
			} else if (input == 0x0D) {
				break;
			} else {
				printf("Invalid value. \r\n\r\n");
				printf(
						"> Press Enter to return to main menu:\r\n ");
				return;
			}
		}
		scan_dur = atol(input_char2);
		if ((scan_dur >= 0) && (scan_dur <= 14)) {
			nlme_set_request(nwkScanDuration, 0, &scan_dur,
					(FUNC_PTR)nlme_set_confirm
					);
		} else {
			printf("Invalid value. \r\n\r\n");
			printf("> Press Enter to return to main menu:\r\n ");
			return;
		}
	}
	break;

	default:
		printf("Invalid input\r\n");
	}
	printf("\r\n\r\n");
	printf("> Press Enter to return to main menu:\r\n ");
}

/**
 * @brief This function is used to print the vendor data submenu on the
 * hyperterminal.
 *
 * @param Vcmd Vendor command id to be requested.
 *
 */
static void print_vendor_data_submenu(vendor_cmd_id_t Vcmd)
{
	char input_char;

	printf("Which device should be asked? Pairing Ref = \r\n");
	input_char = (char)sio2host_getchar();
	printf("\r\n");

	if ((input_char >= '0') && (input_char <= '9')) {
		uint8_t PairingRef = input_char - 0x30;

		uint16_t VendorId = NWKC_VENDOR_IDENTIFIER;
		profile_id_t ProfileId = PROFILE_ID_ZRC;
		uint8_t nsdu = (uint8_t)Vcmd;
		vendor_data_request(PairingRef, ProfileId,
				VendorId, 1, &nsdu,
				TXO_UNICAST | TXO_DST_ADDR_IEEE | TXO_ACK_REQ | TXO_SEC_REQ | TXO_MULTI_CH | TXO_CH_NOT_SPEC |
				TXO_VEND_SPEC);
	} else {
		node_status = IDLE;
		printf("Unknown paring reference\r\n\r\n");
		printf("> Press Enter to return to main menu: \r\n");
	}
}

/**
 * @brief Post processing of the vendor data response.
 *
 * @param PairingRef       Pairing reference
 * @param VendorId         Vendor ID.
 * @param nsduLength       Length of the payload.
 * @param nsdu             Actual payload
 * @param RxLinkQuality    Link quality of received packet.
 * @param RxFlags          Rx Flags.
 */
static void vendor_data_ind(uint8_t PairingRef, uint16_t VendorId,
		uint8_t nsduLength, uint8_t *nsdu, uint8_t RxLinkQuality,
		uint8_t RxFlags)
{
	uint16_t v_id = PGM_READ_WORD(&VendorIdentifier);
	if (v_id == VendorId) {
		if (node_status == POWER_SAVE) {
			printf("Leaving power save mode.\r\n");
			nlme_rx_enable_request(RX_DURATION_INFINITY,
					(FUNC_PTR)nlme_rx_enable_confirm
					);
			node_status = IDLE;
		}

		printf("Vendor data received from pairing ref %d - ",
				PairingRef);
		switch (nsdu[0]) {
		case BATTERY_STATUS_RESP:
		{
			uint16_t volt = (uint16_t)nsdu[1] |
					((uint16_t)nsdu[2] << 8);

			printf("battery status ");
			printf("%d.", volt / 1000);
			printf("%d V\r\n", volt % 1000);
		}
		break;

		case ALIVE_RESP:
			printf("Remote controller is alive\r\n");
			break;

		case FW_VERSION_RESP:
			printf("Firmware version response: %d.%d.%d\r\n",
					nsdu[1], nsdu[2], nsdu[3]);
			break;

		case RX_ON_RESP:
			printf("RX on response\r\n");
			break;

		default:
			printf("unknown command: 0x");
			for (uint8_t i = 0; i < nsduLength; i++) {
				printf("%.02X ", nsdu[i]);
			}
			printf("\r\n");
			break;
		}
	}

	/* Keep compiler happy */
	PairingRef = PairingRef;
	nsduLength = nsduLength;
	RxLinkQuality = RxLinkQuality;
	RxFlags = RxFlags;
}

/**
 * @brief Post processing of the vendor data confirm.
 *
 * @param status     nwk status (constants defined by nwk layer)
 * @param PairingRef Pairing reference
 * @param Handle     Network Layer retries
 *
 */
void vendor_data_confirm(nwk_enum_t Status, uint8_t PairingRef,
		profile_id_t ProfileId,
		uint8_t Handle
		)
{
	printf(
			"Vendor data confirm,  Status = %s (0x%.2X), pairing reference %d\r\n",
			get_status_text((nwk_enum_t)Status), Status,
			PairingRef);
	/* Keeps compiler Happy */
	Handle = Handle;
	ProfileId = ProfileId;
}

/**
 * @brief This function is used to print the status in text on the
 * hyperterminal.
 *
 * @param status    nwk status (constants defined by nwk layer)
 *
 * @return         Text string corresponding to status.
 */
static char *get_status_text(nwk_enum_t status)
{
	char *text;

	switch (status) {
	case NWK_SUCCESS:
		text = "NWK_SUCCESS";
		break;

	case NWK_NO_ORG_CAPACITY:
		text = "NWK_NO_ORG_CAPACITY";
		break;

	case NWK_NO_REC_CAPACITY:
		text = "NWK_NO_REC_CAPACITY";
		break;

	case NWK_NO_PAIRING:
		text = "NWK_NO_PAIRING";
		break;

	case NWK_NO_RESPONSE:
		text = "NWK_NO_RESPONSE";
		break;

	case NWK_NOT_PERMITTED:
		text = "NWK_NOT_PERMITTED";
		break;

	case NWK_DUPLICATE_PAIRING:
		text = "NWK_DUPLICATE_PAIRING";
		break;

	case NWK_FRAME_COUNTER_EXPIRED:
		text = "NWK_FRAME_COUNTER_EXPIRED";
		break;

	case NWK_DISCOVERY_ERROR:
		text = "NWK_DISCOVERY_ERROR";
		break;

	case NWK_DISCOVERY_TIMEOUT:
		text = "NWK_DISCOVERY_TIMEOUT";
		break;

	case NWK_SECURITY_TIMEOUT:
		text = "NWK_SECURITY_TIMEOUT";
		break;

	case NWK_SECURITY_FAILURE:
		text = "NWK_SECURITY_FAILURE";
		break;

	case NWK_INVALID_PARAMETER:
		text = "NWK_INVALID_PARAMETER";
		break;

	case NWK_UNSUPPORTED_ATTRIBUTE:
		text = "NWK_UNSUPPORTED_ATTRIBUTE";
		break;

	case NWK_INVALID_INDEX:
		text = "NWK_INVALID_INDEX";
		break;

	case NWK_MAC_CHANNEL_ACCESS_FAILURE:
		text = "NWK_MAC_CHANNEL_ACCESS_FAILURE";
		break;

	case NWK_MAC_DISABLE_TRX_FAILURE:
		text = "NWK_MAC_DISABLE_TRX_FAILURE";
		break;

	case NWK_MAC_INVALID_GTS:
		text = "NWK_MAC_INVALID_GTS";
		break;

	case NWK_MAC_INVALID_HANDLE:
		text = "NWK_MAC_INVALID_HANDLE";
		break;

	case NWK_MAC_NO_ACK:
		text = "NWK_MAC_NO_ACK";
		break;

	case NWK_MAC_NO_BEACON:
		text = "NWK_MAC_NO_BEACON";
		break;

	case NWK_MAC_NO_DATA:
		text = "NWK_MAC_NO_DATA";
		break;

	case NWK_MAC_NO_SHORT_ADDRESS:
		text = "NWK_MAC_NO_SHORT_ADDRESS";
		break;

	case NWK_MAC_OUT_OF_CAP:
		text = "NWK_MAC_OUT_OF_CAP";
		break;

	case NWK_MAC_PAN_ID_CONFLICT:
		text = "NWK_MAC_PAN_ID_CONFLICT";
		break;

	case NWK_MAC_REALIGNMENT:
		text = "NWK_MAC_REALIGNMENT";
		break;

	case NWK_MAC_TRANSACTION_EXPIRED:
		text = "NWK_MAC_TRANSACTION_EXPIRED";
		break;

	case NWK_MAC_TRANSACTION_OVERFLOW:
		text = "NWK_MAC_TRANSACTION_OVERFLOW";
		break;

	case NWK_MAC_TX_ACTIVE:
		text = "NWK_MAC_TX_ACTIVE";
		break;

	case NWK_MAC_INVALID_ADDRESS:
		text = "NWK_MAC_INVALID_ADDRESS";
		break;

	default:
		text = "reserved or unknown";
		break;
	}
	return text;
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
