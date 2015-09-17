/**
 * @file main.c
 *
 * @brief Single button controller application
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
 */

/**
 * \mainpage
 * \section preface Preface
 * This is the reference manual for ZRC Single button controller demo
 * application.
 * \section main_files Application Files
 * - main.c                     Application main file.
 * - vendor_data.c               Vendor Specific API functions
 * \section intro Application Introduction
 *  Single Button Controller is the RF4CE demo application which can be used in
 * the ZRC target - controller setup
 *  This will support push button pairing procedure and zrc commands i.e Sending
 * the button events to the remote terminal target over the air.
 *
 *	Application supports cold reset and warm reset. While powering on the
 * device, if the Select key is  pressed then it does cold reset.
 *  Otherwise it does warm reset i.e retreiving the network information base
 * from NVM.
 *
 *  If the Select key is pressed on power on, it does the push pairing procedure
 * following the cold reset.
 *
 *  The Application will use the ZRC Commands to send the key press event to
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

/* === INCLUDES ============================================================ */

#include <asf.h>
#include "conf_board.h"
#include "common_sw_timer.h"
#include "keyboard.h"
#include "app_config.h"
#include "rf4ce.h"
#include "zrc.h"
#include "pb_pairing.h"
#include "vendor_data.h"
#include "nwk_config.h"

#ifdef SLEEP_ENABLE
#include "conf_interrupt.h"
#include "sleep_mgr.h"
#endif

/* === TYPES =============================================================== */
typedef enum node_status_tag {
	IDLE,
	WARM_START,
	COLD_START,
	TRANSMITTING,
} SHORTENUM node_status_t;

/* === MACROS ============================================================== */

#define DEBOUNCE_TIME_US    200000
#define TX_OPTIONS  (TXO_UNICAST | TXO_DST_ADDR_NET | \
	TXO_ACK_REQ | TXO_SEC_REQ | TXO_MULTI_CH | \
	TXO_CH_NOT_SPEC | TXO_VEND_NOT_SPEC)

/* === GLOBALS ============================================================= */

FLASH_DECLARE(uint16_t VendorIdentifier) = (uint16_t)NWKC_VENDOR_IDENTIFIER;
FLASH_DECLARE(uint8_t vendor_string[7]) = NWKC_VENDOR_STRING;
FLASH_DECLARE(uint8_t app_user_string[15]) = APP_USER_STRING;
FLASH_DECLARE(uint8_t supported_cec_cmds[32]) = SUPPORTED_CEC_CMDS;

static node_status_t node_status;
static uint8_t pairing_ref = 0xFF;

/* === PROTOTYPES ========================================================== */

static void app_task(void);
static key_state_t key_state_read(key_id_t key_no);
static void indicate_fault_behavior(void);

#ifdef ZRC_CMD_DISCOVERY
static void start_cmd_disc_cb(void *callback_parameter);

#endif

static void zrc_cmd_confirm(nwk_enum_t Status, uint8_t PairingRef,
		cec_code_t RcCmd);
static void zrc_cmd_disc_confirm(nwk_enum_t Status, uint8_t PairingRef,
		uint8_t *SupportedCmd);
static void nlme_reset_confirm(nwk_enum_t Status);
void nlme_rx_enable_confirm(nwk_enum_t Status);
static void pbp_org_pair_confirm(nwk_enum_t Status, uint8_t PairingRef);
static void nlme_start_confirm(nwk_enum_t Status);

#ifdef SLEEP_ENABLE
static void enter_sleep(void);

#endif

#ifdef ZRC_CMD_DISCOVERY
static void zrc_cmd_disc_indication(uint8_t PairingRef);

#endif

#ifdef VENDOR_DATA
extern void vendor_data_ind(uint8_t PairingRef, uint16_t VendorId,
		uint8_t nsduLength, uint8_t *nsdu, uint8_t RxLinkQuality,
		uint8_t RxFlags);

#endif
zrc_indication_callback_t zrc_ind;
uint8_t app_timer;

/* === IMPLEMENTATION ====================================================== */

/**
 * @brief Main function of the Single Button Controller application
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

#ifdef SLEEP_ENABLE

	/* Configure the wakeup source to wakeup the MCU, when it is in sleep
	 * mode */
	config_wakeup_source();
#endif
	sw_timer_init();

	/* Initialize all layers */
	if (nwk_init() != NWK_SUCCESS) {
		/* something went wrong during initialization */
		while (1) {
			indicate_fault_behavior();
		}
	}

	cpu_irq_enable();
	sw_timer_get_id(&app_timer);

#ifdef VENDOR_DATA
	zrc_ind.vendor_data_ind_cb = vendor_data_ind;
#endif
#ifdef ZRC_CMD_DISCOVERY
	zrc_ind.zrc_cmd_disc_indication_cb =  zrc_cmd_disc_indication;
#endif

	register_zrc_indication_callback(&zrc_ind);

	key_state_t key_state = key_state_read(SELECT_KEY);

	if (key_state == KEY_PRESSED) {
		/* Force push button pairing */
		/* Cold start */
		LED_On(LED0);
		node_status = COLD_START;
		nlme_reset_request(true,
				(FUNC_PTR)nlme_reset_confirm
				);
	} else {
		/* Warm start */
		node_status = WARM_START;
		nlme_reset_request(false,
				(FUNC_PTR)nlme_reset_confirm
				);
	}

#ifdef SLEEP_ENABLE
	sm_init();
#endif

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
static void nlme_reset_confirm(nwk_enum_t Status)
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
				(FUNC_PTR)nlme_start_confirm
				);
	} else { /* warm start */
		pairing_ref = 0;
		/* Set power save mode */
#ifdef ENABLE_PWR_SAVE_MODE
		nlme_rx_enable_request(nwkcMinActivePeriod,
				(FUNC_PTR)nlme_rx_enable_confirm
				);
#else
		nlme_rx_enable_request(RX_DURATION_OFF,
				(FUNC_PTR)nlme_rx_enable_confirm
				);
#endif
	}
}

/*
 * The NLME-START.confirm primitive allows the NLME to notify the application of
 * the status of its request to start a network.
 */
static void nlme_start_confirm(nwk_enum_t Status)
{
	if (Status != NWK_SUCCESS) {
		while (1) {
			indicate_fault_behavior();
		}
	}

	LED_On(LED0);

	dev_type_t OrgDevTypeList[1];
	profile_id_t OrgProfileIdList[1];
	profile_id_t DiscProfileIdList[1];

	OrgDevTypeList[0] = DEV_TYPE_REMOTE_CONTROL;
	OrgProfileIdList[0] = PROFILE_ID_ZRC;
	DiscProfileIdList[0] = PROFILE_ID_ZRC;

	pbp_org_pair_request(APP_CAPABILITIES, OrgDevTypeList, OrgProfileIdList,
			DEV_TYPE_WILDCARD, NUM_SUPPORTED_PROFILES,
			DiscProfileIdList,
			(FUNC_PTR)pbp_org_pair_confirm
			);
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
static void pbp_org_pair_confirm(nwk_enum_t Status, uint8_t PairingRef)
{
	if (Status != NWK_SUCCESS) {
		indicate_fault_behavior();
	}

	pairing_ref = PairingRef;

#ifdef ZRC_CMD_DISCOVERY
	/* Start timer to send the cmd discovery request */
	sw_timer_start(app_timer,
			aplcMinTargetBlackoutPeriod_us,
			SW_TIMEOUT_RELATIVE,
			(FUNC_PTR)start_cmd_disc_cb,
			NULL);
#else
	/* Set power save mode */
#ifdef ENABLE_PWR_SAVE_MODE
	nlme_rx_enable_request(nwkcMinActivePeriod,
			(FUNC_PTR)nlme_rx_enable_confirm);
#else
	nlme_rx_enable_request(RX_DURATION_OFF,
			(FUNC_PTR)nlme_rx_enable_confirm);
#endif
#endif
}

/*
 * Callback funtion indication that the timer expired for sending the command
 * discovery request.
 */
#ifdef ZRC_CMD_DISCOVERY
static void start_cmd_disc_cb(void *callback_parameter)
{
	zrc_cmd_disc_request(pairing_ref,
			(FUNC_PTR)zrc_cmd_disc_confirm
			);

	/* Keep compiler happy */
	callback_parameter = callback_parameter;
}

#endif

/*
 * The command discovery confirm callback provides information about the command
 * discovery reqest.
 */
#ifdef ZRC_CMD_DISCOVERY
static void zrc_cmd_disc_confirm(nwk_enum_t Status, uint8_t PairingRef,
		uint8_t *SupportedCmd)
{
	/* Enable transceiver Power Save Mode */
#ifdef ENABLE_PWR_SAVE_MODE
	nlme_rx_enable_request(nwkcMinActivePeriod,
			(FUNC_PTR)nlme_rx_enable_confirm
			);
#else
	nlme_rx_enable_request(RX_DURATION_OFF,
			(FUNC_PTR)nlme_rx_enable_confirm
			);
#endif

	node_status = IDLE;

	/* Keep compiler happy */
	Status = Status;
	PairingRef = PairingRef;
	SupportedCmd = SupportedCmd;
}

#endif

/*
 * The command discovery indication callback indicates that a command discovery
 * request command was receiced.
 */
#ifdef ZRC_CMD_DISCOVERY
static void zrc_cmd_disc_indication(uint8_t PairingRef)
{
	/* Send back the response */
	uint8_t cec_cmds[32];
	PGM_READ_BLOCK(cec_cmds, supported_cec_cmds, 32);
	zrc_cmd_disc_response(PairingRef, cec_cmds);
	node_status = IDLE;
}

#endif

/*  */

/*
 * The NLME-RX-ENABLE.confirm primitive reports the results of the attempt to
 * enable or disable the receiver.
 */
void nlme_rx_enable_confirm(nwk_enum_t Status)
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
		delay_ms(1000);
		LED_Off(LED0);
	} else if (node_status == WARM_START) {
		node_status = IDLE;

		/* LED handling */
		LED_On(LED0 );
		delay_ms(250);
		LED_Off(LED0);
	}
}

/**
 * @brief Application task and state machine
 */
static void app_task(void)
{
	static uint32_t current_time;
	static uint32_t previous_button_time;
	switch (node_status) {
	case IDLE:
	{
		static key_state_t key_state;
		key_state = key_state_read(SELECT_KEY);
		if (key_state == KEY_PRESSED) {
			/* Check time to previous transmission. */
			current_time = sw_timer_get_time();
			if ((current_time - previous_button_time) <
					DEBOUNCE_TIME_US) {
				return;
			} else {
				/* Store current time */
				previous_button_time = current_time;
			}

			LED_On(LED0);
			uint8_t cmd = POWER_TOGGLE_FUNCTION; /* 0x6b */
			if (zrc_cmd_request(pairing_ref, 0x0000,
					USER_CONTROL_PRESSED,
					1, &cmd, TX_OPTIONS,
					(FUNC_PTR)zrc_cmd_confirm
					)) {
				node_status = TRANSMITTING;
			}
		}

#ifdef SLEEP_ENABLE
		else {
			if (nwk_ready_to_sleep() || (nwk_stack_idle())) {
				/* Enter Sleep will enable the wakeup source,
				 * also config the timers and sleep modes
				 * handling */
				enter_sleep();

				/* MCU wakes up from the sleep and continues in
				 * normal mode */
			}
		}
#endif
	}
	break;

	default:
		break;
	}
}

/*  */

/**
 * @brief ZRC command confirm
 *
 * The ZRC comand confirm callback provides information about the status of the
 * last command request/transmission.
 *
 * @param Status        NWK_SUCCESS if transmission was successful
 * @param PairingRef    Pairing reference
 * @param RcCmd         Sent RC command
 */
static void zrc_cmd_confirm(nwk_enum_t Status, uint8_t PairingRef,
		cec_code_t RcCmd)
{
	node_status = IDLE;

	if (Status == NWK_SUCCESS) {
		LED_Off(LED0);
	} else {
		indicate_fault_behavior();
	}

	/* Keep compiler happy. */
	PairingRef = PairingRef;
	RcCmd = RcCmd;
}

/*  */

/* --- Helper functions ---------------------------------------------------- */

/**
 * @brief Indicating malfunction
 */
static void indicate_fault_behavior(void)
{
	uint8_t i;

	for (i = 0; i < 10; i++) {
		LED_On(LED0);
		delay_ms(200);
		LED_Off(LED0);
		delay_ms(200);
	}
}

/**
 * @brief Vendor-spefic callback; handles reaction to incoming alive request
 */
void vendor_app_alive_req(void)
{
	LED_On(LED0);
	delay_ms(500);
	LED_Off(LED0);
}

/**
 * @brief Read key_state
 *
 * @param key_no Key to be read.
 */
static key_state_t key_state_read(key_id_t key_no)
{
	key_state_t key_val = KEY_RELEASED;

	if (SELECT_KEY == key_no) {
		#if SAMD20 || SAMD21 || SAMR21
		if (!port_pin_get_input_level(BUTTON_0_PIN)) {
			key_val = KEY_PRESSED;
		}

		#else
		if (!ioport_get_pin_level(GPIO_PUSH_BUTTON_0)) {
			key_val = KEY_PRESSED;
		}
		#endif
	}

	return key_val;
}

#ifdef SLEEP_ENABLE

/**
 * @brief enter_sleep This function will put the MCU to Lowest possible sleep
 * mode
 *            This function will get the current running timer id expiry
 * duration. This
 *            will be used to run the other timer(sleep mode timers during MCU
 * sleep)
 *	       wakeup source also used to wakeup the controllers from sleep
 * mode. The
 *	       timer drift values will be written into the software timer once
 * the MCU wake from
 *		sleep by timer expiry or due to wakeup sources.
 * @param key_no Key to be read.
 */

static void enter_sleep(void)
{
	uint32_t sleep_duration;

	/* This function will get the next expiry timer duration */
	sleep_duration = sw_timer_next_timer_expiry_duration();

	if (sleep_duration > MIN_SLEEP_TIME) {
		/* Enable the wakeup source, before MCU goes to sleep */
		enable_wakeup_source();

		/* Set MCU to sleep */
		sm_sleep((sleep_duration -
				MIN_SLEEP_TIME) / SLEEP_MGR_TIMER_RES);

		/* handle the wakeup, config the clock set the interrupts etc */
		wakeup_handle();

		/* sw timer  - add the sleep offset time duration */
		sw_timer_run_residual_time(MIN_SLEEP_TIME);
	}
}

#endif
/* EOF */
