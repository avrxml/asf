/**
 * @file mac_start.c
 *
 * @brief This file implements the MLME-START.request
 * (MAC layer management entity) entry points.
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

/* === Macros =============================================================== */

/* === Globals ============================================================== */

#if (MAC_START_REQUEST_CONFIRM == 1) || defined(__DOXYGEN__)

#ifdef MAC_SECURITY_BEACON
mlme_start_req_t msr_params;    /* Intermediate start parameters */
#else
static mlme_start_req_t msr_params;    /* Intermediate start parameters */
#endif

/* === Prototypes =========================================================== */

/* === Implementation ======================================================= */

/*
 * @brief Internal function to generate confirmation
 * for MLME_START.request
 *
 * @param start_buf_ptr Pointer to MLME_START.request
 * @param start_req_status Status of the MLME_START.request
 */
static void gen_mlme_start_conf(buffer_t *start_buf_ptr,
		uint8_t start_req_status)
{
	/* Use the same start request buffer for start confirm */
	mlme_start_conf_t *msc = (mlme_start_conf_t *)BMM_BUFFER_POINTER(
			start_buf_ptr);

	msc->cmdcode = MLME_START_CONFIRM;
	msc->status = start_req_status;

	/* The confirmation message is appended to the MAC-NHLE queue */
	qmm_queue_append(&mac_nhle_q, start_buf_ptr);
}

#ifndef REDUCED_PARAM_CHECK

/*
 * @brief Checks the parameters of a MLME_START.request
 *
 * This function validates the parameters of the MLME_START.request.
 *
 * @param msg Pointer to the MLME_START.request message which holds the
 * start parameters
 *
 * @return true if parameters are valid, false otherwise
 */
static bool check_start_parameter(mlme_start_req_t *msg)
{
	bool param_validation_status = false;

#ifndef BEACON_SUPPORT

	/*
	 * In a build without beacon support the beacon order and/or
	 * superframe order needs to be 15, since only a nonbeacon-enabled
	 * network
	 * can be created.
	 * The following code is valid for starting a new network as well as for
	 * for a coordinator realignment attempt.
	 */
	if ((msg->BeaconOrder != NON_BEACON_NWK) ||
			(msg->SuperframeOrder != NON_BEACON_NWK)) {
		/* This is not allowed in a build without beacon support. */
		return param_validation_status;
	}
#endif

	/*
	 * Value of beacon order has to be less than or equal to 15 (Non beacon
	 * Network). The value of superframe has to be either less than equal to
	 * beacon order.
	 */
	if ((msg->BeaconOrder <= NON_BEACON_NWK) &&
			((msg->SuperframeOrder <= msg->BeaconOrder) ||
			(NON_BEACON_NWK == msg->SuperframeOrder))) {
		/*
		 * Coordinator realignment command can only be given by a
		 * PAN coordinator or cordinator (which is associated to a
		 * PAN coordinator).
		 */
		if ((msg->CoordRealignment) &&
				((MAC_ASSOCIATED == mac_state) ||
				(MAC_IDLE == mac_state))
				) {
			/*
			 * We are neigher the requested to be the PAN
			 * Coordinator,
			 * nor are we associated, so Realignment is not allowed
			 * at
			 * this stage.
			 */
			param_validation_status = false;
		} else {
			param_validation_status = true;
		}
	}

	return param_validation_status;
}

#endif  /* REDUCED_PARAM_CHECK */

/**
 * @brief The MLME-START.request primitive makes a request for the device to
 * start using a new superframe configuration
 *
 * @param m Pointer to MLME_START.request message issued by the NHLE
 */
void mlme_start_request(uint8_t *m)
{
	mlme_start_req_t *msg
		= (mlme_start_req_t *)BMM_BUFFER_POINTER((buffer_t *)m);

	/*
	 * The MLME_START.request parameters are copied into a global variable
	 * structure, which is used by check_start_parameter() function.
	 */
	memcpy(&msr_params, msg, sizeof(msr_params));

	if (BROADCAST == tal_pib.ShortAddress) {
		/*
		 * The device is void of short address. This device cannot start
		 * a
		 * network, hence a confirmation is given back.
		 */
		gen_mlme_start_conf((buffer_t *)m, MAC_NO_SHORT_ADDRESS);
		return;
	}

#ifndef REDUCED_PARAM_CHECK
	if (!check_start_parameter(msg)) {
		/*
		 * The MLME_START.request parameters are invalid, hence
		 * confirmation
		 * is given to NHLE.
		 */
		gen_mlme_start_conf((buffer_t *)m, MAC_INVALID_PARAMETER);
	} else
#endif  /* REDUCED_PARAM_CHECK */
	{
		/*
		 * All the start parameters are valid, hence MLME_START.request
		 * can
		 * proceed.
		 */
		set_tal_pib_internal(mac_i_pan_coordinator,
				(void *)&(msg->PANCoordinator));

		if (msr_params.CoordRealignment) {
			/* First inform our devices of the configuration change
			**/
			if (!mac_tx_coord_realignment_command(
					COORDINATORREALIGNMENT,
					(buffer_t *)m,
					msr_params.PANId,
					msr_params.LogicalChannel,
					msr_params.ChannelPage)) {
				/*
				 * The coordinator realignment command was
				 * unsuccessful,
				 * hence the confiramtion is given to NHLE.
				 */
				gen_mlme_start_conf((buffer_t *)m,
						MAC_INVALID_PARAMETER);
			}
		} else {
			/* This is a normal MLME_START.request. */
			retval_t channel_set_status, channel_page_set_status;

			/* The new PIBs are set at the TAL. */
			set_tal_pib_internal(macBeaconOrder,
					(void *)&(msg->BeaconOrder));

			/* If macBeaconOrder is equal to 15, set also
			 * macSuperframeOrder to 15. */
			if (msg->BeaconOrder == NON_BEACON_NWK) {
				msg->SuperframeOrder = NON_BEACON_NWK;
			}

			set_tal_pib_internal(macSuperframeOrder,
					(void *)&(msg->SuperframeOrder));

#ifdef BEACON_SUPPORT

			/*
			 * Symbol times are calculated according to the new BO
			 * and SO
			 * values.
			 */
			if (tal_pib.BeaconOrder < NON_BEACON_NWK) {
				set_tal_pib_internal(macBattLifeExt,
						(void *)&(msr_params.
						BatteryLifeExtension));
			}
#endif /* BEACON_SUPPORT */

			/* Wake up radio first */
			mac_trx_wakeup();

			/* MLME_START.request parameters other than BO and SO
			 * are set at TAL */
			set_tal_pib_internal(macPANId,
					(void *)&(msr_params.PANId));

			channel_page_set_status
				= set_tal_pib_internal(phyCurrentPage,
					(void *)&(msr_params.ChannelPage));

			channel_set_status
				= set_tal_pib_internal(phyCurrentChannel,
					(void *)&(msr_params.LogicalChannel));

			set_tal_pib_internal(mac_i_pan_coordinator,
					(void *)&(msr_params.PANCoordinator));

			if ((MAC_SUCCESS == channel_page_set_status) &&
					(MAC_SUCCESS == channel_set_status) &&
					(PHY_RX_ON == tal_rx_enable(PHY_RX_ON))
					) {
				if (msr_params.PANCoordinator) {
					mac_state = MAC_PAN_COORD_STARTED;
				} else {
					mac_state = MAC_COORDINATOR;
				}

				gen_mlme_start_conf((buffer_t *)m, MAC_SUCCESS);

#ifdef BEACON_SUPPORT

				/*
				 * In case we have a beaconing network, the
				 * beacon timer needs
				 * to be started now.
				 */
				if (tal_pib.BeaconOrder != NON_BEACON_NWK) {
					mac_start_beacon_timer();
				}
#endif  /* BEACON_SUPPORT */
			} else {
				/* Start of network failed. */
				gen_mlme_start_conf((buffer_t *)m,
						MAC_INVALID_PARAMETER);
			}

			/* Set radio to sleep if allowed */
			mac_sleep_trans();
		}
	}
}

/**
 * @brief Continues handling of MLME_START.request (Coordinator realignment)
 * command
 *
 * This function is called once the coordinator realignment command is
 * sent out to continue the handling of the MLME_START.request command.
 *
 * @param tx_status Status of the coordinator realignment command
 *                  transmission
 * @param buf_ptr Buffer for start confirmation
 */
void mac_coord_realignment_command_tx_success(uint8_t tx_status,
		buffer_t *buf_ptr)
{
	uint8_t conf_status = MAC_INVALID_PARAMETER;
	retval_t channel_set_status, channel_page_set_status;

	if (MAC_SUCCESS == tx_status) {
		/* The parameters of the existing PAN are updated. */
		channel_page_set_status
			= set_tal_pib_internal(phyCurrentPage,
				(void *)&(msr_params.ChannelPage));

#if (_DEBUG_ > 0)
		Assert(MAC_SUCCESS == channel_page_set_status);
#endif

		channel_set_status
			= set_tal_pib_internal(phyCurrentChannel,
				(void *)&msr_params.LogicalChannel);

#if (_DEBUG_ > 0)
		Assert(MAC_SUCCESS == channel_set_status);
#endif
		if ((MAC_SUCCESS == channel_set_status) &&
				(MAC_SUCCESS == channel_page_set_status)) {
			conf_status = MAC_SUCCESS;

			set_tal_pib_internal(macPANId,
					(void *)&(msr_params.PANId));

#ifdef BEACON_SUPPORT

			/*
			 * Store current beacon order in order to be able to
			 * detect
			 * switching from nonbeacon to beacon network.
			 */
			uint8_t cur_beacon_order = tal_pib.BeaconOrder;

			set_tal_pib_internal(macBeaconOrder,
					(void *)&(msr_params.BeaconOrder));
			set_tal_pib_internal(macSuperframeOrder,
					(void *)&(msr_params.SuperframeOrder));

			/*
			 * New symbol times for beacon time (in sysbols) and
			 * inactive time are
			 * calculated according to the new superframe
			 * configuration.
			 */
			if (msr_params.BeaconOrder < NON_BEACON_NWK) {
				set_tal_pib_internal(macBattLifeExt,
						(void *)&(msr_params.
						BatteryLifeExtension));
			}

			if ((NON_BEACON_NWK < cur_beacon_order) &&
					(msr_params.BeaconOrder ==
					NON_BEACON_NWK)) {
				/*
				 * This is a transition from a beacon enabled
				 * network to
				 * a nonbeacon enabled network.
				 * In this case the broadcast data queue will
				 * never be served.
				 *
				 * Therefore the broadcast queue needs to be
				 * emptied.
				 * The standard does not define what to do now.
				 * The current implementation will try to send
				 * all pending broadcast
				 * data frames immediately, thus giving the
				 * receiving nodes a chance
				 * receive them.
				 */
				while (broadcast_q.size > 0) {
					mac_tx_pending_bc_data();
				}
			}

			if ((NON_BEACON_NWK == cur_beacon_order) &&
					(msr_params.BeaconOrder <
					NON_BEACON_NWK)) {
				/*
				 * This is a transition from a nonbeacon enabled
				 * network to
				 * a beacon enabled network, hence the beacon
				 * timer will be
				 * started.
				 */
				mac_start_beacon_timer();
			}
#endif  /* BEACON_SUPPORT */
		}
	}

	gen_mlme_start_conf(buf_ptr, conf_status);

	/* Set radio to sleep if allowed */
	mac_sleep_trans();
} /* mac_coord_realignment_command_tx_success() */

#endif /* (MAC_START_REQUEST_CONFIRM == 1) || defined(__DOXYGEN__) */

/* EOF */
