/**
 * @file mac_misc.c
 *
 * @brief This file implements miscellaneous MAC sublayer components.
 *
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

/*
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

/* === Includes ============================================================ */
#include <compiler.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "return_val.h"
#include "pal.h"
#include "bmm.h"
#include "qmm.h"
#include "tal.h"
#include "ieee_const.h"
#include "mac_msg_const.h"
#include "mac_api.h"
#include "mac_msg_types.h"
#include "mac_data_structures.h"
#include "stack_config.h"
#include "mac_internal.h"
#include "mac.h"
#include "mac_config.h"
#include "mac_build_config.h"
#if ((defined MAC_SECURITY_ZIP)  || (defined MAC_SECURITY_2006))
#include "mac_security.h"
#endif  /* (MAC_SECURITY_ZIP || MAC_SECURITY_2006) */
#ifdef STB_ON_SAL
#include "stb.h"
#endif

/* === Macros =============================================================== */

/* === Globals ============================================================= */

#if (NUMBER_OF_MAC_TIMERS > 0)
#ifdef BEACON_SUPPORT
uint8_t T_Beacon_Tracking_Period;
uint8_t T_Superframe;
uint8_t T_Missed_Beacon;
    #if (MAC_START_REQUEST_CONFIRM == 1)
uint8_t T_Beacon;
uint8_t T_Beacon_Preparation;
    #endif /* (MAC_START_REQUEST_CONFIRM == 1) */
#ifdef GTS_SUPPORT
uint8_t T_CAP;
#endif /* GTS_SUPPORT */
#endif  /* BEACON_SUPPORT / No BEACON_SUPPORT */

#if (MAC_INDIRECT_DATA_BASIC == 1)
uint8_t T_Poll_Wait_Time;
    #if (MAC_INDIRECT_DATA_FFD == 1)
uint8_t T_Data_Persistence;
    #endif  /* (MAC_INDIRECT_DATA_FFD == 1) */
#endif  /* (MAC_INDIRECT_DATA_BASIC == 1) */

#if (MAC_SCAN_SUPPORT == 1)
uint8_t T_Scan_Duration;
#endif  /* MAC_SCAN_SUPPORT */

#if (MAC_RX_ENABLE_SUPPORT == 1)
uint8_t T_Rx_Enable;
#endif  /* MAC_RX_ENABLE_SUPPORT */
#endif /* (NUMBER_OF_MAC_TIMERS != 0) */

/* === Prototypes ========================================================== */

static void do_init_pib(void);
static void flush_queues(void);
static uint8_t mac_reset(uint8_t init_pib);
static void mac_soft_reset(uint8_t init_pib);
static void reset_globals(void);
static void send_reset_conf(buffer_t *buf_ptr, uint8_t status);

/* === Implementation ====================================================== */

/*
 * @brief Initializes the MAC global variables
 */
static void reset_globals(void)
{
	mac_busy = false;
	mac_state = MAC_IDLE;
	mac_radio_sleep_state = RADIO_AWAKE;
	mac_scan_state = MAC_SCAN_IDLE;
	mac_sync_state = MAC_SYNC_NEVER;
	mac_poll_state = MAC_POLL_IDLE;
#ifdef BEACON_SUPPORT
	mac_final_cap_slot = FINAL_CAP_SLOT_DEFAULT;
	mac_bc_data_indicated = false;
#endif  /* BEACON_SUPPORT */
	mac_last_dsn = 0;
	memset((uint8_t *)&mac_last_src_addr, 0xFF, sizeof(mac_last_src_addr));
	/* mac_last_src_addr = 0xFFFFFFFFFFFFFFFFULL; */
	mac_rx_enabled = false;
#ifdef GTS_SUPPORT
	reset_gts_globals();
#endif /* GTS_SUPPORT */
}

/**
 * @brief Initializes the MAC sublayer
 *
 * @return MAC_SUCCESS  if TAL is intialized successfully else FAILURE
 */
retval_t mac_init(void)
{
	#ifdef GTS_DEBUG
	struct port_config config_port_pin;
	config_port_pin.direction = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(DEBUG_PIN1, &config_port_pin);
	port_pin_set_config(DEBUG_PIN2, &config_port_pin);
	port_pin_set_config(DEBUG_PIN3, &config_port_pin);
	port_pin_set_config(DEBUG_PIN4, &config_port_pin);
	port_pin_set_config(DEBUG_PIN5, &config_port_pin);
	port_pin_set_config(DEBUG_PIN6, &config_port_pin);
	port_pin_set_config(DEBUG_PIN7, &config_port_pin);
	port_pin_set_config(DEBUG_PIN8, &config_port_pin);
	port_pin_set_config(DEBUG_PIN9, &config_port_pin);
	port_pin_set_config(DEBUG_PIN10, &config_port_pin);
	port_pin_set_config(DEBUG_PIN11, &config_port_pin);
	port_pin_set_config(DEBUG_PIN12, &config_port_pin);
	port_pin_set_config(DEBUG_PIN13, &config_port_pin);
	port_pin_set_config(DEBUG_PIN14, &config_port_pin);
	port_pin_set_config(DEBUG_PIN15, &config_port_pin);

	/*ioport_configure_pin(DEBUG_PIN1, IOPORT_DIR_OUTPUT |
	 *  IOPORT_INIT_LOW);
	 *  ioport_configure_pin(DEBUG_PIN2, IOPORT_DIR_OUTPUT |
	 *  IOPORT_INIT_LOW);
	 *  ioport_configure_pin(DEBUG_PIN3, IOPORT_DIR_OUTPUT |
	 *  IOPORT_INIT_LOW);
	 *  ioport_configure_pin(DEBUG_PIN4, IOPORT_DIR_OUTPUT |
	 *  IOPORT_INIT_LOW);*/
	#endif

	/* Initialize TAL */
	if (tal_init() != MAC_SUCCESS) {
		return FAILURE;
	}

#ifdef STB_ON_SAL
	stb_init();
#endif

#ifdef ENABLE_RTB
	/* Initialize RTB */
	if (rtb_init() != RTB_SUCCESS) {
		return FAILURE;
	}

#endif  /* ENABLE_RTB */

	/* Calibrate MCU's RC oscillator */
	if (!pal_calibrate_rc_osc()) {
		return FAILURE;
	}

	if (MAC_SUCCESS != mac_timers_init()) {
		return FAILURE;
	}

	mac_soft_reset(true);

	/* Set radio to sleep if allowed */
	mac_sleep_trans();

	/* Initialize the queues */
#ifdef ENABLE_QUEUE_CAPACITY
	qmm_queue_init(&nhle_mac_q, NHLE_MAC_QUEUE_CAPACITY);
	qmm_queue_init(&tal_mac_q, TAL_MAC_QUEUE_CAPACITY);
    #if (MAC_INDIRECT_DATA_FFD == 1)
	qmm_queue_init(&indirect_data_q, INDIRECT_DATA_QUEUE_CAPACITY);
    #endif /* (MAC_INDIRECT_DATA_FFD == 1) */
    #if (MAC_START_REQUEST_CONFIRM == 1)
    #ifdef BEACON_SUPPORT
	qmm_queue_init(&broadcast_q, BROADCAST_QUEUE_CAPACITY);
    #endif  /* BEACON_SUPPORT */
    #endif /* (MAC_START_REQUEST_CONFIRM == 1) */
#else
	qmm_queue_init(&nhle_mac_q);
	qmm_queue_init(&tal_mac_q);
    #if (MAC_INDIRECT_DATA_FFD == 1)
	qmm_queue_init(&indirect_data_q);
    #endif /* (MAC_INDIRECT_DATA_FFD == 1) */
    #if (MAC_START_REQUEST_CONFIRM == 1)
    #ifdef BEACON_SUPPORT
	qmm_queue_init(&broadcast_q);
    #endif  /* BEACON_SUPPORT */
    #endif /* (MAC_START_REQUEST_CONFIRM == 1) */
#endif  /* ENABLE_QUEUE_CAPACITY */

	return MAC_SUCCESS;
}

/**
 * @brief Resets the MAC helper variables and transition to idle state
 *
 * This function sets the MAC to idle state and resets
 * MAC helper variables
 */
void mac_idle_trans(void)
{
	/* Wake up radio first */
	mac_trx_wakeup();

	{
		uint16_t default_shortaddress = macShortAddress_def;
		uint16_t default_panid = macPANId_def;
#if (_DEBUG_ > 0)
		retval_t set_status =
#endif
		set_tal_pib_internal(macShortAddress,
				(void *)&default_shortaddress);
#if (_DEBUG_ > 0)
		Assert(MAC_SUCCESS == set_status);

		set_status =
#endif
		set_tal_pib_internal(macPANId, (void *)&default_panid);

#if (_DEBUG_ > 0)
		Assert(MAC_SUCCESS == set_status);
		set_status = set_status;
#endif
	}

	mac_soft_reset(true);

	/* Set radio to sleep if allowed */
	mac_sleep_trans();
}

/*
 * @brief Initializes MAC PIBs
 *
 * This function initializes all MAC PIBs to their defaults as stated by
 * 802.15.4.
 */
static void do_init_pib(void)
{
#if (MAC_ASSOCIATION_REQUEST_CONFIRM == 1)
	mac_pib.mac_AssociatedPANCoord = macAssociatedPANCoord_def;
#endif /* (MAC_ASSOCIATION_REQUEST_CONFIRM == 1) */
#if ((MAC_INDIRECT_DATA_BASIC == 1) || defined(BEACON_SUPPORT))
	mac_pib.mac_MaxFrameTotalWaitTime = macMaxFrameTotalWaitTime_def;
#endif  /* ((MAC_INDIRECT_DATA_BASIC == 1) || defined(BEACON_SUPPORT)) */
	mac_pib.mac_ResponseWaitTime = macResponseWaitTime_def;
	mac_pib.mac_SecurityEnabled = macSecurityEnabled_def;

#if (MAC_ASSOCIATION_INDICATION_RESPONSE == 1)
	mac_pib.mac_AssociationPermit = macAssociationPermit_def;
#endif /* (MAC_ASSOCIATION_INDICATION_RESPONSE == 1) */

#if (MAC_START_REQUEST_CONFIRM == 1)
	mac_pib.mac_BeaconPayloadLength = macBeaconPayloadLength_def;
	mac_pib.mac_BSN = (uint8_t)rand();
#endif  /* (MAC_START_REQUEST_CONFIRM == 1) */

#ifdef GTS_SUPPORT
	mac_pib.mac_GTSPermit = macGTSPermit_def;
#endif /* GTS_SUPPORT */

#if (MAC_INDIRECT_DATA_FFD == 1)
	mac_pib.mac_TransactionPersistenceTime
		= macTransactionPersistenceTime_def;
#endif /* (MAC_INDIRECT_DATA_FFD == 1) */

	mac_pib.mac_AutoRequest = macAutoRequest_def;
	mac_pib.mac_BattLifeExtPeriods = macBattLifeExtPeriods_def;
	memset((uint8_t *)&mac_pib.mac_CoordExtendedAddress, 0,
			sizeof(mac_pib.mac_CoordExtendedAddress));
	/* mac_pib.mac_CoordExtendedAddress = (uint64_t)CLEAR_ADDR_64; */
	mac_pib.mac_CoordShortAddress = macCoordShortAddress_def;
	mac_pib.mac_DSN = (uint8_t)rand();
	mac_pib.mac_RxOnWhenIdle = macRxOnWhenIdle_def;

#if ((defined MAC_SECURITY_ZIP)  || (defined MAC_SECURITY_2006))
	/* TODO: Create a specific function for security PIB initialization? */
	mac_sec_pib.KeyTableEntries = macKeyTableEntries_def;
	mac_sec_pib.DeviceTableEntries = macDeviceTable_def;
	mac_sec_pib.SecurityLevelTableEntries = macSecurityLevelTable_def;
	mac_sec_pib.FrameCounter = macFrameCounter_def;
#endif  /* (MAC_SECURITY_ZIP || MAC_SECURITY_2006) */

#ifdef TEST_HARNESS
	mac_pib.privateIllegalFrameType = 1;
	mac_pib.privateNoDataAfterAssocReq = 0;
	mac_pib.privateVirtualPANs = 0;
#endif /* TEST_HARNESS */
}

/**
 * @brief Resets the MAC layer
 *
 * The MLME-RESET.request primitive allows the next higher layer to request
 * that the MLME performs a reset operation.
 *
 * @param m Pointer to the MLME_RESET.request given by the NHLE
 */
void mlme_reset_request(uint8_t *m)
{
	mlme_reset_req_t *mrr
		= (mlme_reset_req_t *)BMM_BUFFER_POINTER((buffer_t *)m);

	/* Wakeup the radio */
	mac_trx_wakeup();

	/* Start MAC reset functionality */
	uint8_t status = mac_reset(mrr->SetDefaultPIB);

	/* Set radio to sleep if allowed */
	mac_sleep_trans();

	/*
	 * As this is a mlme_reset request, all the requests, data (whether
	 * direct
	 * or indirect), incoming frames are removed from the queues
	 */
	flush_queues();

	send_reset_conf((buffer_t *)m, status);
} /* mlme_reset_request() */

/*
 * @brief Internal MAC reset function
 *
 * This function resets the MAC variables, stops all running timers and
 * initializes the PIBs.
 *
 * @param init_pib Boolean indicates whether PIB attributes shall be
 * initialized or not.
 *
 * @return Success or failure status
 */
static uint8_t mac_reset(uint8_t init_pib)
{
	uint8_t status = MAC_DISABLE_TRX_FAILURE;

	/*
	 * Transceiver is going to stop giving out clock for some time
	 * so change to internal clock
	 */
	pal_timer_source_select(TMR_CLK_SRC_DURING_TRX_SLEEP);

	/* Reset TAL */
	status = tal_reset(init_pib);

	/* Transceiver is now giving out clock, switch back to external clock */
	pal_timer_source_select(TMR_CLK_SRC_DURING_TRX_AWAKE);

	mac_soft_reset(init_pib);

	return status;
}

/*
 * @brief Internal MAC soft reset function
 *
 * This function resets the MAC variables, stops all running timers and
 * initializes the PIBs.
 *
 * @param init_pib Boolean indicates whether PIB attributes shall be
 * initialized or not.
 */
static void mac_soft_reset(uint8_t init_pib)
{
	reset_globals();

	/* Set trx to PHY_TRX_OFF */
	tal_rx_enable(PHY_TRX_OFF);

	ENTER_CRITICAL_REGION();
	mac_timers_stop();
	LEAVE_CRITICAL_REGION();

	if (init_pib) {
		do_init_pib();
	}
}

/**
 * @brief Puts the radio to sleep if this is allowed
 */
void mac_sleep_trans(void)
{
	/* Go to sleep? */
#ifdef BEACON_SUPPORT
	if ((NON_BEACON_NWK > tal_pib.BeaconOrder && MAC_INACTIVE ==
			mac_superframe_state && (!mac_rx_enabled)) ||
			(NON_BEACON_NWK == tal_pib.BeaconOrder &&
			(!mac_pib.mac_RxOnWhenIdle) && (!mac_rx_enabled))) {
#else /* BEACON_SUPPORT */
	if ((!mac_pib.mac_RxOnWhenIdle) && (!mac_rx_enabled)) {
#endif /* BEACON_SUPPORT */
#if (MAC_SYNC_REQUEST == 1)

		/*
		 * In case we are currently synced with our parent, and the
		 * tracking beacon timer it NOT running (i.e. the timer is
		 * expired and not started again yet), we need to stay awake,
		 * until we receive a new beacon frame form our parent.
		 */
		if ((MAC_SYNC_NEVER != mac_sync_state) &&
				(!pal_is_timer_running(T_Beacon_Tracking_Period)))
		{
			/* Stays awake */
		} else {
			mac_trx_init_sleep();
		}

#else
		mac_trx_init_sleep();
#endif /* (MAC_SYNC_REQUEST == 1) */
	}
}

/*
 * @brief Flushes all queues
 */
static void flush_queues(void)
{
	/* Flush NHLE MAC queue */
	qmm_queue_flush(&nhle_mac_q);

	/* Flush TAL_MAC queue */
	qmm_queue_flush(&tal_mac_q);

#if (HIGHEST_STACK_LAYER == MAC)
	/* Flush MAC-NHLE queue */
	qmm_queue_flush(&mac_nhle_q);
#endif

#ifdef GTS_SUPPORT
	/* Flush MAC GTS queue */
	flush_gts_queues();
#endif /* GTS_SUPPORT */

#if (MAC_INDIRECT_DATA_FFD == 1)
	/* Flush MAC indirect queue */
	qmm_queue_flush(&indirect_data_q);
#endif /* (MAC_INDIRECT_DATA_FFD == 1) */

#if (MAC_START_REQUEST_CONFIRM == 1)
#ifdef BEACON_SUPPORT
	/* Flush MAC broadcast queue */
	qmm_queue_flush(&broadcast_q);
#endif  /* BEACON_SUPPORT */
#endif /* (MAC_START_REQUEST_CONFIRM == 1) */
}

/*
 * @brief Sends mlme reset confirm
 *
 * @param m Buffer for reset confirm
 * @param status Status of MAC reset operation
 */
static void send_reset_conf(buffer_t *buf_ptr, uint8_t status)
{
	mlme_reset_conf_t *mrc;

	mrc = (mlme_reset_conf_t *)BMM_BUFFER_POINTER(buf_ptr);

	mrc->status = status;
	mrc->cmdcode = MLME_RESET_CONFIRM;

	/* Append the mlme reset confirm to the MAC-NHLE queue */
	qmm_queue_append(&mac_nhle_q, buf_ptr);
}

#if (MAC_COMM_STATUS_INDICATION == 1)

/**
 * @brief Creates a Communication Status Indication message to the upper layer
 *
 * @param status Status of the last operation
 * @param buf_ptr Buffer for Communication Status Indication to the NHLE
 */
void mac_mlme_comm_status(uint8_t status,
		buffer_t *buf_ptr)
{
	/*
	 * The pointer to the destination address (received as one of the
	 * function
	 * paramters) points to a location in buf_ptr.
	 * As the same buffer is used to generate the comm status
	 * indication, it is typecasted to the 'mlme_comm_status_ind_t'. This
	 * may
	 * result in loosing destination address (which is still a part of this
	 * buffer), hence the destination address is backed up in a stack
	 * variable.
	 */
	frame_info_t *frame_ptr = (frame_info_t *)BMM_BUFFER_POINTER(buf_ptr);
	uint64_t destination_address;
	memcpy(&destination_address,
			&frame_ptr->mpdu[PL_POS_DST_ADDR_START],
			sizeof(uint64_t));
	mlme_comm_status_ind_t *csi
		= (mlme_comm_status_ind_t *)BMM_BUFFER_POINTER(buf_ptr);

	csi->cmdcode = MLME_COMM_STATUS_INDICATION;

	csi->PANId = tal_pib.PANId;

	csi->SrcAddrMode = FCF_LONG_ADDR;

	/* Initialize the source address */
	csi->SrcAddr = tal_pib.IeeeAddress;

	csi->DstAddrMode = FCF_LONG_ADDR;

	/* Initialize the destination address */
	csi->DstAddr = destination_address;

	csi->status = status;

	qmm_queue_append(&mac_nhle_q, buf_ptr);
}

#endif /* MAC_COMM_STATUS_INDICATION */

/**
 * @brief MAC function to put the radio to sleep mode
 */
void mac_trx_init_sleep(void)
{
	/* If the radio is not sleeping, it is put to sleep */
	if (RADIO_AWAKE == mac_radio_sleep_state) {
		pal_timer_source_select(TMR_CLK_SRC_DURING_TRX_SLEEP);

#ifdef ENABLE_DEEP_SLEEP
		if (MAC_SUCCESS == tal_trx_sleep(DEEP_SLEEP_MODE))
#else
		if (MAC_SUCCESS == tal_trx_sleep(SLEEP_MODE_1))
#endif
		{
			mac_radio_sleep_state = RADIO_SLEEPING;
		} else {
			pal_timer_source_select(TMR_CLK_SRC_DURING_TRX_AWAKE);
		}
	}
}

/**
 * @brief MAC function to wake-up the radio from sleep state
 */
void mac_trx_wakeup(void)
{
	/* If the radio is sleeping, it is woken-up */
	if (RADIO_SLEEPING == mac_radio_sleep_state) {
		if (FAILURE != tal_trx_wakeup()) {
			pal_timer_source_select(TMR_CLK_SRC_DURING_TRX_AWAKE);

			mac_radio_sleep_state = RADIO_AWAKE;
		}
	}
}

retval_t mac_timers_init(void)
{
#if (NUMBER_OF_MAC_TIMERS > 0)
#ifdef BEACON_SUPPORT
	if (MAC_SUCCESS != pal_timer_get_id(&T_Beacon_Tracking_Period)) {
		return FAILURE;
	}

	if (MAC_SUCCESS != pal_timer_get_id(&T_Superframe)) {
		return FAILURE;
	}

	if (MAC_SUCCESS != pal_timer_get_id(&T_Missed_Beacon)) {
		return FAILURE;
	}

    #if (MAC_START_REQUEST_CONFIRM == 1)
	if (MAC_SUCCESS != pal_timer_get_id(&T_Beacon)) {
		return FAILURE;
	}

	if (MAC_SUCCESS != pal_timer_get_id(&T_Beacon_Preparation)) {
		return FAILURE;
	}

    #endif /* (MAC_START_REQUEST_CONFIRM == 1) */
#ifdef GTS_SUPPORT
	if (MAC_SUCCESS != pal_timer_get_id(&T_CAP)) {
		return FAILURE;
	}
#endif /* GTS_SUPPORT */
#endif  /* BEACON_SUPPORT / No BEACON_SUPPORT */

#if (MAC_INDIRECT_DATA_BASIC == 1)
	if (MAC_SUCCESS != pal_timer_get_id(&T_Poll_Wait_Time)) {
		return FAILURE;
	}

    #if (MAC_INDIRECT_DATA_FFD == 1)
	if (MAC_SUCCESS != pal_timer_get_id(&T_Data_Persistence)) {
		return FAILURE;
	}
    #endif  /* (MAC_INDIRECT_DATA_FFD == 1) */
#endif  /* (MAC_INDIRECT_DATA_BASIC == 1) */

#if (MAC_SCAN_SUPPORT == 1)
	if (MAC_SUCCESS != pal_timer_get_id(&T_Scan_Duration)) {
		return FAILURE;
	}

#endif  /* MAC_SCAN_SUPPORT */

#if (MAC_RX_ENABLE_SUPPORT == 1)
	if (MAC_SUCCESS != pal_timer_get_id(&T_Rx_Enable)) {
		return FAILURE;
	}
#endif  /* MAC_RX_ENABLE_SUPPORT */
#endif /* (NUMBER_OF_MAC_TIMERS != 0) */
	return MAC_SUCCESS;
}

retval_t mac_timers_stop(void)
{
#if (NUMBER_OF_MAC_TIMERS > 0)
#ifdef BEACON_SUPPORT
	pal_timer_stop(T_Beacon_Tracking_Period);
	pal_timer_stop(T_Superframe);
	pal_timer_stop(T_Missed_Beacon);
    #if (MAC_START_REQUEST_CONFIRM == 1)
	pal_timer_stop(T_Beacon);
	pal_timer_stop(T_Beacon_Preparation);
    #endif /* (MAC_START_REQUEST_CONFIRM == 1) */
#ifdef GTS_SUPPORT
	pal_timer_stop(T_CAP);
#endif /* GTS_SUPPORT */
#endif  /* BEACON_SUPPORT / No BEACON_SUPPORT */

#if (MAC_INDIRECT_DATA_BASIC == 1)
	pal_timer_stop(T_Poll_Wait_Time);
    #if (MAC_INDIRECT_DATA_FFD == 1)
	pal_timer_stop(T_Data_Persistence);
    #endif  /* (MAC_INDIRECT_DATA_FFD == 1) */
#endif  /* (MAC_INDIRECT_DATA_BASIC == 1) */

#if (MAC_SCAN_SUPPORT == 1)
	pal_timer_stop(T_Scan_Duration);
#endif  /* MAC_SCAN_SUPPORT */

#if (MAC_RX_ENABLE_SUPPORT == 1)
	pal_timer_stop(T_Rx_Enable);
#endif  /* MAC_RX_ENABLE_SUPPORT */
#endif /* (NUMBER_OF_MAC_TIMERS != 0) */
	return MAC_SUCCESS;
}

/* EOF */
