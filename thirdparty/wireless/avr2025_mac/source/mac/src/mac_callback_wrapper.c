/**
 *
 * @file mac_callback_wrapper.c
 *
 * @brief Wrapper code for MAC callback functions.
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
#include "mac_build_config.h"

/* === Macros ============================================================== */

/* === Globals ============================================================= */

/* === Prototypes ========================================================== */

/* === Implementation ====================================================== */

/**
 * @brief Wrapper function for messages of type mcps_data_ind_t
 *
 * This function is a callback for mcps data indication
 *
 * @param m Pointer to message structure
 */
void mcps_data_ind(uint8_t *m)
{
	mcps_data_ind_t *pmsg;
	wpan_addr_spec_t src_addr;
	wpan_addr_spec_t dst_addr;

	/* Get the buffer body from buffer header */
	pmsg = (mcps_data_ind_t *)BMM_BUFFER_POINTER(((buffer_t *)m));

	/* Source address spec */
	src_addr.AddrMode = pmsg->SrcAddrMode;
	src_addr.PANId = pmsg->SrcPANId;
	src_addr.Addr.long_address = pmsg->SrcAddr;
	ADDR_COPY_DST_SRC_64(src_addr.Addr.long_address, pmsg->SrcAddr);

	/* Destination address spec */
	dst_addr.AddrMode = pmsg->DstAddrMode;
	dst_addr.PANId = pmsg->DstPANId;
	ADDR_COPY_DST_SRC_64(dst_addr.Addr.long_address, pmsg->DstAddr);

	/* Callback function */
#if ((defined MAC_SECURITY_ZIP)  || (defined MAC_SECURITY_2006))
	usr_mcps_data_ind(&src_addr,
			&dst_addr,
			pmsg->msduLength,
			pmsg->msdu,
			pmsg->mpduLinkQuality,
			pmsg->DSN,
    #ifdef ENABLE_TSTAMP
			pmsg->Timestamp,
    #endif  /* ENABLE_TSTAMP */
			pmsg->SecurityLevel,
			pmsg->KeyIdMode,
			pmsg->KeyIndex);
#else   /* No MAC_SECURITY */
	usr_mcps_data_ind(&src_addr,
			&dst_addr,
			pmsg->msduLength,
			pmsg->msdu,
			pmsg->mpduLinkQuality,
    #ifdef ENABLE_TSTAMP
			pmsg->DSN,
			pmsg->Timestamp);
    #else
			pmsg->DSN);
    #endif  /* ENABLE_TSTAMP */
#endif  /* MAC_SECURITY */

	/* Free the buffer */
	bmm_buffer_free((buffer_t *)m);
}

/**
 * @brief Wrapper function for messages of type mcps_data_conf_t
 *
 * This function is a callback for mcps data confirm.
 *
 * @param m Pointer to message structure
 */
void mcps_data_conf(uint8_t *m)
{
	mcps_data_conf_t *pmsg;

	/* Get the buffer body from buffer header */
	pmsg = (mcps_data_conf_t *)BMM_BUFFER_POINTER(((buffer_t *)m));

#ifdef ENABLE_TSTAMP
	usr_mcps_data_conf(pmsg->msduHandle, pmsg->status, pmsg->Timestamp);
#else
	usr_mcps_data_conf(pmsg->msduHandle, pmsg->status);
#endif  /* ENABLE_TSTAMP */

	/* Free the buffer */
	bmm_buffer_free((buffer_t *)m);
}

#if ((MAC_PURGE_REQUEST_CONFIRM == 1) && (MAC_INDIRECT_DATA_BASIC == 1))

/**
 * @brief Wrapper function for messages of type mcps_purge_conf_t
 *
 * This function is a callback for mcps purge confirm.
 *
 * @param m Pointer to message structure
 */
void mcps_purge_conf(uint8_t *m)
{
	mcps_purge_conf_t *pmsg;

	/* Get the buffer body from buffer header */
	pmsg = (mcps_purge_conf_t *)BMM_BUFFER_POINTER(((buffer_t *)m));

	usr_mcps_purge_conf(pmsg->msduHandle, pmsg->status);

	/* Free the buffer */
	bmm_buffer_free((buffer_t *)m);
}

#endif /* ((MAC_PURGE_REQUEST_CONFIRM == 1) && (MAC_INDIRECT_DATA_BASIC == 1))
       **/

#if (MAC_ASSOCIATION_REQUEST_CONFIRM == 1)

/**
 * @brief Wrapper function for messages of type mlme_associate_conf_t
 *
 * This function is a callback for mlme associate confirm.
 *
 * @param m Pointer to message structure
 */
void mlme_associate_conf(uint8_t *m)
{
	mlme_associate_conf_t *pmsg;

	/* Get the buffer body from buffer header */
	pmsg = (mlme_associate_conf_t *)BMM_BUFFER_POINTER(((buffer_t *)m));

	usr_mlme_associate_conf(pmsg->AssocShortAddress, pmsg->status);

	/* Free the buffer */
	bmm_buffer_free((buffer_t *)m);
}

#endif /* (MAC_ASSOCIATION_REQUEST_CONFIRM == 1) */

#if (MAC_ASSOCIATION_INDICATION_RESPONSE == 1)

/**
 * @brief Wrapper function for messages of type mlme_associate_ind_t
 *
 * This function is a callback for mlme associate indication.
 *
 * @param m Pointer to message structure
 */
void mlme_associate_ind(uint8_t *m)
{
	mlme_associate_ind_t *pmsg;

	/* Get the buffer body from buffer header */
	pmsg = (mlme_associate_ind_t *)BMM_BUFFER_POINTER((buffer_t *)m);

#if  SAMD || SAMR21 || SAM4L || SAM4S || SAM4E
	uint64_t device_addr_temp = 0;
	memcpy((uint8_t *)&device_addr_temp, (uint8_t *)&pmsg->DeviceAddress,
			sizeof(device_addr_temp));
	usr_mlme_associate_ind(device_addr_temp, pmsg->CapabilityInformation);
#else
	usr_mlme_associate_ind(pmsg->DeviceAddress,
			pmsg->CapabilityInformation);
#endif

	/* Free the buffer */
	bmm_buffer_free((buffer_t *)m);
}

#endif /* (MAC_ASSOCIATION_INDICATION_RESPONSE == 1) */

#if (MAC_BEACON_NOTIFY_INDICATION == 1)

/**
 * @brief Wrapper function for messages of type mlme_beacon_notify_ind_t
 *
 * This function is a callback for mlme beacon notify indication.
 *
 * @param m Pointer to message structure
 */
void mlme_beacon_notify_ind(uint8_t *m)
{
	mlme_beacon_notify_ind_t *pmsg;

	/* Get the buffer body from buffer header */
	pmsg = (mlme_beacon_notify_ind_t *)BMM_BUFFER_POINTER(((buffer_t *)m));

	/* Callback function */
	usr_mlme_beacon_notify_ind(pmsg->BSN,           /* BSN */
			&(pmsg->PANDescriptor),         /* PANDescriptor */
			pmsg->PendAddrSpec,             /* PendAddrSpec */
			pmsg->AddrList,                 /* AddrList */
			pmsg->sduLength,                /* sduLength */
			pmsg->sdu);                     /* sdu */

	/* Free the buffer */
	bmm_buffer_free((buffer_t *)m);
}

#endif /* (MAC_BEACON_NOTIFY_INDICATION == 1) */

#if (MAC_COMM_STATUS_INDICATION == 1)

/**
 * @brief Wrapper function for messages of type mlme_comm_status_ind_t
 *
 * This function is a callback for mlme comm status indication.
 *
 * @param m Pointer to message structure
 */
void mlme_comm_status_ind(uint8_t *m)
{
	mlme_comm_status_ind_t *pmsg;
	wpan_addr_spec_t src_addr;
	wpan_addr_spec_t dst_addr;

	/* Get the buffer body from buffer header */
	pmsg = (mlme_comm_status_ind_t *)BMM_BUFFER_POINTER(((buffer_t *)m));

	/* Source address spec */
	src_addr.PANId = pmsg->PANId;
	src_addr.AddrMode = pmsg->SrcAddrMode;
	ADDR_COPY_DST_SRC_64(src_addr.Addr.long_address, pmsg->SrcAddr);

	/* Destintion address spec */
	dst_addr.PANId = pmsg->PANId;
	dst_addr.AddrMode = pmsg->DstAddrMode;
	ADDR_COPY_DST_SRC_64(dst_addr.Addr.long_address, pmsg->DstAddr);

	/* Callback function */
	usr_mlme_comm_status_ind(&src_addr,
			&dst_addr,
			pmsg->status);

	/* Free the buffer */
	bmm_buffer_free((buffer_t *)m);
}

#endif /* (MAC_COMM_STATUS_INDICATION == 1) */

#if (MAC_DISASSOCIATION_BASIC_SUPPORT == 1)

/**
 * @brief Wrapper function for messages of type mlme_disassociate_conf_t
 *
 * This function is a callback for mlme disassociate confirm.
 *
 * @param m Pointer to message structure
 */
void mlme_disassociate_conf(uint8_t *m)
{
	mlme_disassociate_conf_t *pmsg;
	wpan_addr_spec_t device_addr;

	/* Get the buffer body from buffer header */
	pmsg = (mlme_disassociate_conf_t *)BMM_BUFFER_POINTER(((buffer_t *)m));

	/* Device address spec */
	device_addr.AddrMode = pmsg->DeviceAddrMode;
	device_addr.PANId = pmsg->DevicePANId;
	ADDR_COPY_DST_SRC_64(device_addr.Addr.long_address,
			pmsg->DeviceAddress);

	usr_mlme_disassociate_conf(pmsg->status,
			&device_addr);

	/* Free the buffer */
	bmm_buffer_free((buffer_t *)m);
}

#endif  /* (MAC_DISASSOCIATION_BASIC_SUPPORT == 1) */

#if (MAC_DISASSOCIATION_BASIC_SUPPORT == 1)

/**
 * @brief Wrapper function for messages of type mlme_disassociate_ind_t
 *
 * This function is a callback for mlme disassociate indication.
 *
 * @param m Pointer to message structure
 */
void mlme_disassociate_ind(uint8_t *m)
{
	mlme_disassociate_ind_t *pmsg;

	/* Get the buffer body from buffer header */
	pmsg = (mlme_disassociate_ind_t *)BMM_BUFFER_POINTER(((buffer_t *)m));

	usr_mlme_disassociate_ind(pmsg->DeviceAddress,
			pmsg->DisassociateReason);

	/* Free the buffer */
	bmm_buffer_free((buffer_t *)m);
}

#endif /* (MAC_DISASSOCIATION_BASIC_SUPPORT == 1) */

#if (MAC_GET_SUPPORT == 1)

/**
 * @brief Wrapper function for messages of type mlme_get_conf_t
 *
 * This function is a callback for mlme get confirm.
 *
 * @param m Pointer to message structure
 */
void mlme_get_conf(uint8_t *m)
{
	mlme_get_conf_t *pmsg;

	/* Get the buffer body from buffer header */
	pmsg = (mlme_get_conf_t *)BMM_BUFFER_POINTER(((buffer_t *)m));

	/* Callback function */
	usr_mlme_get_conf(pmsg->status,
			pmsg->PIBAttribute,
#if ((defined MAC_SECURITY_ZIP)  || (defined MAC_SECURITY_2006))
			pmsg->PIBAttributeIndex,
#endif  /* (MAC_SECURITY_ZIP || MAC_SECURITY_2006) */
			&pmsg->PIBAttributeValue);

	/* Free the buffer */
	bmm_buffer_free((buffer_t *)m);
}

#endif  /* (MAC_GET_SUPPORT == 1) */

#if (MAC_ORPHAN_INDICATION_RESPONSE == 1)

/**
 * @brief Wrapper function for messages of type mlme_orphan_ind_t
 *
 * This function is a callback for mlme orphan indication.
 *
 * @param m Pointer to message structure
 */
void mlme_orphan_ind(uint8_t *m)
{
	mlme_orphan_ind_t *pmsg;

	/* Get the buffer body from buffer header */
	pmsg = (mlme_orphan_ind_t *)BMM_BUFFER_POINTER(((buffer_t *)m));

	/* Callback function */
	usr_mlme_orphan_ind(pmsg->OrphanAddress);

	/* Free the buffer */
	bmm_buffer_free((buffer_t *)m);
}

#endif /* (MAC_ORPHAN_INDICATION_RESPONSE == 1) */

#if (MAC_INDIRECT_DATA_BASIC == 1)

/**
 * @brief Wrapper function for messages of type mlme_poll_conf_t
 *
 * This function is a callback for mlme poll confirm.
 *
 * @param m Pointer to message structure
 */
void mlme_poll_conf(uint8_t *m)
{
	mlme_poll_conf_t *pmsg;

	/* Get the buffer body from buffer header */
	pmsg = (mlme_poll_conf_t *)BMM_BUFFER_POINTER(((buffer_t *)m));

	usr_mlme_poll_conf(pmsg->status);

	/* Free the buffer */
	bmm_buffer_free((buffer_t *)m);
}

#endif /* (MAC_INDIRECT_DATA_BASIC == 1) */

/**
 * @brief Wrapper function for messages of type mlme_reset_conf_t
 *
 * This function is a callback for mlme reset confirm.
 *
 * @param m Pointer to message structure
 */
void mlme_reset_conf(uint8_t *m)
{
	mlme_reset_conf_t *pmsg;

	/* Get the buffer body from buffer header */
	pmsg = (mlme_reset_conf_t *)BMM_BUFFER_POINTER(((buffer_t *)m));

	usr_mlme_reset_conf(pmsg->status);

	/* Free the buffer */
	bmm_buffer_free((buffer_t *)m);
}

#if (MAC_RX_ENABLE_SUPPORT == 1)

/**
 * @brief Wrapper function for messages of type mlme_rx_enable_conf_t
 *
 * This function is a callback for mlme rx enable confirm.
 *
 * @param m Pointer to message structure
 */
void mlme_rx_enable_conf(uint8_t *m)
{
	mlme_rx_enable_conf_t *pmsg;

	/* Get the buffer body from buffer header */
	pmsg = (mlme_rx_enable_conf_t *)BMM_BUFFER_POINTER(((buffer_t *)m));

	usr_mlme_rx_enable_conf(pmsg->status);

	/* Free the buffer */
	bmm_buffer_free((buffer_t *)m);
}

#endif /* (MAC_RX_ENABLE_SUPPORT == 1) */

#if (MAC_SCAN_SUPPORT == 1)

/**
 * @brief Wrapper function for messages of type mlme_scan_conf_t
 *
 * This function is a callback for mlme scan confirm.
 *
 * @param m Pointer to message structure
 */
void mlme_scan_conf(uint8_t *m)
{
	mlme_scan_conf_t *pmsg;

	/* Get the buffer body from buffer header */
	pmsg = (mlme_scan_conf_t *)BMM_BUFFER_POINTER(((buffer_t *)m));

	/* Callback */
	usr_mlme_scan_conf(pmsg->status,
			pmsg->ScanType,
			pmsg->ChannelPage,
			pmsg->UnscannedChannels,
			pmsg->ResultListSize,
			&pmsg->scan_result_list);

	/* Free the buffer */
	bmm_buffer_free((buffer_t *)m);
}

#endif /* (MAC_SCAN_SUPPORT == 1) */

/**
 * @brief Wrapper function for messages of type mlme_set_conf_t
 *
 * This function is a callback for mlme set confirm.
 *
 * @param m Pointer to message structure
 */
void mlme_set_conf(uint8_t *m)
{
	mlme_set_conf_t *pmsg;

	/* Get the buffer body from buffer header */
	pmsg = (mlme_set_conf_t *)BMM_BUFFER_POINTER(((buffer_t *)m));

#if ((defined MAC_SECURITY_ZIP)  || (defined MAC_SECURITY_2006))
	usr_mlme_set_conf(pmsg->status, pmsg->PIBAttribute,
			pmsg->PIBAttributeIndex);
#else
	usr_mlme_set_conf(pmsg->status, pmsg->PIBAttribute);
#endif  /* (MAC_SECURITY_ZIP || MAC_SECURITY_2006) */

	/* Free the buffer */
	bmm_buffer_free((buffer_t *)m);
}

#if (MAC_START_REQUEST_CONFIRM == 1)

/**
 * @brief Wrapper function for messages of type mlme_start_conf_t
 *
 * This function is a callback for mlme start confirm.
 *
 * @param m Pointer to message structure
 */
void mlme_start_conf(uint8_t *m)
{
	mlme_start_conf_t *pmsg;

	/* Get the buffer body from buffer header */
	pmsg = (mlme_start_conf_t *)BMM_BUFFER_POINTER(((buffer_t *)m));

	usr_mlme_start_conf(pmsg->status);

	/* Free the buffer */
	bmm_buffer_free((buffer_t *)m);
}

#endif /* (MAC_START_REQUEST_CONFIRM == 1) */

#if (MAC_SYNC_LOSS_INDICATION == 1)

/**
 * @brief Wrapper function for messages of type mlme_sync_loss_ind_t
 *
 * This function is a callback for mlme sync loss indication.
 *
 * @param m Pointer to message structure
 */
void mlme_sync_loss_ind(uint8_t *m)
{
	mlme_sync_loss_ind_t *pmsg;

	/* Get the buffer body from buffer header */
	pmsg = (mlme_sync_loss_ind_t *)BMM_BUFFER_POINTER(((buffer_t *)m));

	usr_mlme_sync_loss_ind(pmsg->LossReason,
			pmsg->PANId,
			pmsg->LogicalChannel,
			pmsg->ChannelPage);

	/* Uses static buffer for sync loss indication and it is not freed */
}

#endif /* (MAC_SYNC_LOSS_INDICATION == 1) */

#ifdef GTS_SUPPORT
void mlme_gts_conf(uint8_t *m)
{
	mlme_gts_conf_t *pmsg;

	/* Get the buffer body from buffer header */
	pmsg = (mlme_gts_conf_t *)BMM_BUFFER_POINTER(((buffer_t *)m));

	usr_mlme_gts_conf(pmsg->GtsChar, pmsg->status);

	/* Free the buffer */
	bmm_buffer_free((buffer_t *)m);
}

void mlme_gts_ind(uint8_t *m)
{
	mlme_gts_ind_t *pmsg;

	/* Get the buffer body from buffer header */
	pmsg = (mlme_gts_ind_t *)BMM_BUFFER_POINTER(((buffer_t *)m));

	usr_mlme_gts_ind(pmsg->DeviceAddr, pmsg->GtsChar);

	/* Free the buffer */
	bmm_buffer_free((buffer_t *)m);
}

#endif /* GTS_SUPPORT */
/* EOF */
