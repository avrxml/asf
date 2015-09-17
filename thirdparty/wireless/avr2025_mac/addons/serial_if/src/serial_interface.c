/**
 * @file serial_interface.c
 *
 * @brief Handles Serial Interface Functionalities
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
 */

/*
 * Copyright (c) 2013-2015 Atmel Corporation. All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */
#include <asf.h>
#include "conf_board.h"
#include "avr2025_mac.h"
#include "serial_interface.h"
#include "sio2host.h"

/**
 * This is the receive buffer of the UART.
 */
static uint8_t sio_rx_buf[SIO_RX_BUF_SIZE];

/**
 * This is the transmit buffer of the UART.
 */
static uint8_t sio_tx_buf[SIO_BUF_COUNT][SIO_TX_BUF_SIZE];

/**
 * This pointer points to the next free element inside the
 * receive buffer of the UART.
 */
static uint8_t *sio_rx_ptr;

/**
 * This is the receiver state of the UART. (\ref UART_RX_STATE_SOT,
 *   \ref UART_RX_STATE_LENGTH, \ref UART_RX_STATE_DATA, or
 *   \ref UART_RX_STATE_EOT)
 */
static volatile uint8_t sio_rx_state;

/**
 * This is the length of the message should be received.
 */
static uint8_t sio_rx_length;

/**
 * This is the length of the message should be transmitted.
 */
static uint8_t data[SIO_RX_BUF_SIZE];
static uint8_t data_length = 0;
static uint8_t rx_index = 0;
static uint8_t head = 0, buf_count = 0;

#if (defined MAC_SECURITY_ZIP || defined MAC_SECURITY_2006)
#include "tal_internal.h"
#include "mac_internal.h"
#include "tal.h"
extern mac_sec_pib_t mac_sec_pib;
uint8_t key_source_8[8] = {0};
uint8_t key_source_4[4] = {0};
#endif

#if (defined BEACON_SUPPORT) || (defined ENABLE_TSTAMP)
/* extern uint32_t tal_rx_timestamp; */
#endif  /* #if (defined BEACON_SUPPORT) || (defined ENABLE_TSTAMP) */

/*
 * \brief This function does the initialization of the Serial handler state
 * Machine.
 */
void serial_interface_init(void)
{
	sio_rx_state = UART_RX_STATE_SOT;
	sio2host_init();
}

/**
 * \brief get the new buffer for next transmission through serial
 *
 * \return unsigned integer pointer to buf
 */
static uint8_t *get_next_tx_buffer(void)
{
	if (buf_count != SIO_BUF_COUNT) {
		uint8_t *buf;
		uint8_t tail;

		tail = (head + buf_count) % SIO_BUF_COUNT;
		buf = (uint8_t *)(&sio_tx_buf[tail]);
		buf_count++;
		/* Add message start character */
		*buf++ = SOT;
		return buf;
	}

	return NULL;
}

/*
 * Callback function usr_mcps_data_conf
 *
 * @param msduHandle  Handle of MSDU handed over to MAC earlier
 * @param status      Result for requested data transmission request
 * @param Timestamp   The time, in symbols, at which the data were transmitted
 *                    (only if timestamping is enabled).
 *
 */

#if defined(ENABLE_TSTAMP)
void usr_mcps_data_conf(uint8_t msduHandle,
		uint8_t status,
		uint32_t Timestamp)
#else
void usr_mcps_data_conf(uint8_t msduHandle,
		uint8_t status)
#endif  /* ENABLE_TSTAMP */
{
	uint8_t *msg_buf;

	msg_buf = get_next_tx_buffer();
	*msg_buf++ = MCPS_DATA_CON_LEN + MAC_PID_LEN;
	*msg_buf++ = MAC_PID;
	*msg_buf++ = MCPS_DATA_CONFIRM;
	*msg_buf++ = msduHandle;
	*msg_buf++ = status;
#if (defined ENABLE_TSTAMP)
	*msg_buf++ = Timestamp;
	*msg_buf++ = Timestamp >> 8;
	*msg_buf++ = Timestamp >> 16;
	*msg_buf++ = Timestamp >> 24;
#endif  /* ENABLE_TSTAMP */
	*msg_buf = EOT;
}

/*
 * @brief Callback function usr_mcps_data_ind
 *
 * @param SrcAddrSpec      Pointer to source address specification
 * @param DstAddrSpec      Pointer to destination address specification
 * @param msduLength       Number of octets contained in MSDU
 * @param msdu             Pointer to MSDU
 * @param mpduLinkQuality  LQI measured during reception of the MPDU
 * @param DSN              DSN of the received data frame.
 * @param Timestamp        The time, in symbols, at which the data were received
 *                         (only if timestamping is enabled).
 */
#if defined(MAC_SECURITY_ZIP) || defined(__DOXYGEN__)
void usr_mcps_data_ind(wpan_addr_spec_t *SrcAddrSpec,
		wpan_addr_spec_t *DstAddrSpec,
		uint8_t msduLength,
		uint8_t *msdu,
		uint8_t mpduLinkQuality,
		uint8_t DSN,
    #if defined(ENABLE_TSTAMP) || defined(__DOXYGEN__)
		uint32_t Timestamp,
    #endif  /* ENABLE_TSTAMP */
		uint8_t SecurityLevel,
		uint8_t KeyIdMode,
		uint8_t KeyIndex)
#else   /* No MAC_SECURITY */
void usr_mcps_data_ind(wpan_addr_spec_t *SrcAddrSpec,
		wpan_addr_spec_t *DstAddrSpec,
		uint8_t msduLength,
		uint8_t *msdu,
		uint8_t mpduLinkQuality,
    #ifdef ENABLE_TSTAMP
		uint8_t DSN,
		uint32_t Timestamp)
    #else
		uint8_t DSN)
    #endif  /* ENABLE_TSTAMP */
#endif  /* MAC_SECURITY */
{
	uint8_t *msg_buf;
	/* Pointer to size element - the content is written later. */
	uint8_t *msg_ptr_to_size;

	msg_buf = get_next_tx_buffer();
	/* Store pointer to size element. */
	msg_ptr_to_size = msg_buf;

	*msg_buf++ = MCPS_DATA_IND_LEN + MAC_PID_LEN;
	*msg_buf++ = MAC_PID;
	*msg_buf++ = MCPS_DATA_INDICATION;

	*msg_buf++ = SrcAddrSpec->AddrMode;

#ifdef TEST_HARNESS_BIG_ENDIAN
	*msg_buf++ = SrcAddrSpec->PANId >> 8;
	*msg_buf++ = SrcAddrSpec->PANId;
#else
	*msg_buf++ = SrcAddrSpec->PANId;
	*msg_buf++ = SrcAddrSpec->PANId >> 8;
#endif
	memcpy(msg_buf, &(SrcAddrSpec->Addr), 8);
	msg_buf += 8;
	*msg_buf++ = DstAddrSpec->AddrMode;

#ifdef TEST_HARNESS_BIG_ENDIAN
	*msg_buf++ = DstAddrSpec->PANId >> 8;
	*msg_buf++ = DstAddrSpec->PANId;
#else
	*msg_buf++ = DstAddrSpec->PANId;
	*msg_buf++ = DstAddrSpec->PANId >> 8;
#endif
	memcpy(msg_buf, &(DstAddrSpec->Addr), 8);
	msg_buf += 8;
	*msg_buf++ = mpduLinkQuality;
	*msg_buf++ = DSN;
    #ifdef ENABLE_TSTAMP
	*msg_buf++ = Timestamp;
	*msg_buf++ = Timestamp >> 8;
	*msg_buf++ = Timestamp >> 16;
	*msg_buf++ = Timestamp >> 24;
    #endif  /* ENABLE_TSTAMP */
#if (defined MAC_SECURITY_ZIP) || (defined MAC_SECURITY_2006)
	/* Add size of SecurityLevel parameter. */
	*msg_ptr_to_size += 3;

	*msg_buf++ = SecurityLevel,
	*msg_buf++ = KeyIdMode;
	*msg_buf++ = KeyIndex;
#endif  /* MAC_SECURITY_ZIP */
	*msg_buf++ = msduLength;

	/*
	 * The second length of the MSDU is required since the test harness
	 * requires the length as first element of each variable list.
	 */
	*msg_buf++ = msduLength;
	memcpy(msg_buf, msdu, msduLength);
	msg_buf += msduLength;
	*msg_ptr_to_size += msduLength;
	*msg_buf = EOT;
}

#if (MAC_PURGE_REQUEST_CONFIRM == 1)

/*
 * Callback function that must be implemented by application (NHLE) for MAC
 * service
 * MCPS-PURGE.confirm.
 *
 * @param msduHandle           Handle (id) of MSDU to be purged.
 * @param status               Result of requested purge operation.
 *
 * @return void
 */
void usr_mcps_purge_conf(uint8_t msduHandle,
		uint8_t status)
{
	uint8_t *msg_buf;

	msg_buf = get_next_tx_buffer();
	*msg_buf++ = MCPS_PURGE_CONF_LEN + MAC_PID_LEN;
	*msg_buf++ = MAC_PID;
	*msg_buf++ = MCPS_PURGE_CONFIRM;
	*msg_buf++ = msduHandle;
	*msg_buf++ = status;
	*msg_buf = EOT;
}

#endif /* MAC_PURGE_REQUEST_CONFIRM */

#if (MAC_ASSOCIATION_REQUEST_CONFIRM == 1) || defined(DOXYGEN)

/*
 * Callback function usr_mlme_associate_conf.
 *
 * @param AssocShortAddress    Short address allocated by the coordinator.
 * @param status               Result of requested association operation.
 *
 * @return void
 *
 */
void usr_mlme_associate_conf(uint16_t AssocShortAddress,
		uint8_t status)
{
	uint8_t *msg_buf;

	msg_buf = get_next_tx_buffer();
	*msg_buf++ = MLME_ASSOCIATE_CONF_LEN + MAC_PID_LEN;
	*msg_buf++ = MAC_PID;
	*msg_buf++ = MLME_ASSOCIATE_CONFIRM;

#ifdef TEST_HARNESS_BIG_ENDIAN
	*msg_buf++ = AssocShortAddress >> 8;
	*msg_buf++ = AssocShortAddress;
#else
	*msg_buf++ = AssocShortAddress;
	*msg_buf++ = AssocShortAddress >> 8;
#endif
	*msg_buf++ = status;
	*msg_buf = EOT;
}

#endif  /* (MAC_ASSOCIATION_REQUEST_CONFIRM == 1) */

#if (MAC_ASSOCIATION_INDICATION_RESPONSE == 1)

/*
 * @brief Callback function usr_mlme_associate_ind
 *
 * @param DeviceAddress         Extended address of device requesting
 * association
 * @param CapabilityInformation Capabilities of device requesting association
 */
void usr_mlme_associate_ind(uint64_t DeviceAddress,
		uint8_t CapabilityInformation)
{
	uint8_t *msg_buf;

	msg_buf = get_next_tx_buffer();
	*msg_buf++ = MLME_ASSOCIATE_IND_LEN + MAC_PID_LEN;
	*msg_buf++ = MAC_PID;
	*msg_buf++ = MLME_ASSOCIATE_INDICATION;
	memcpy(msg_buf, &DeviceAddress, 8);
	msg_buf += 8;
	*msg_buf++ = CapabilityInformation;
	*msg_buf = EOT;
}

#endif  /* (MAC_ASSOCIATION_INDICATION_RESPONSE == 1) */

#if (MAC_BEACON_NOTIFY_INDICATION == 1)

/*
 * Callback function usr_mlme_beacon_notify_ind
 *
 * @param BSN            Beacon sequence number.
 * @param PANDescriptor  Pointer to PAN descriptor for received beacon.
 * @param PendAddrSpec   Pending address specification in received beacon.
 * @param AddrList       List of addresses of devices the coordinator has
 * pending data.
 * @param sduLength      Length of beacon payload.
 * @param sdu            Pointer to beacon payload.
 *
 * @return void
 *
 */
void usr_mlme_beacon_notify_ind(uint8_t BSN,
		wpan_pandescriptor_t *PANDescriptor,
		uint8_t PendAddrSpec,
		uint8_t *AddrList,
		uint8_t sduLength,
		uint8_t *sdu)
{
	uint8_t *msg_buf;
	/* Pointer to size element - the content is written later. */
	uint8_t *msg_ptr_to_size;
	uint8_t short_addr_list = WPAN_NUM_SHORT_ADDR_PENDING(PendAddrSpec) *
			sizeof(uint16_t);
	uint8_t ext_addr_list = WPAN_NUM_EXTENDED_ADDR_PENDING(PendAddrSpec) *
			sizeof(uint64_t);

	msg_buf = get_next_tx_buffer();
	/* Store pointer to size element. */
	msg_ptr_to_size = msg_buf;

	*msg_buf++ = MLME_BEACON_NOTIFY_IND_LEN + MAC_PID_LEN;
	*msg_buf++ = MAC_PID;
	*msg_buf++ = MLME_BEACON_NOTIFY_INDICATION;
	*msg_buf++ = BSN;

	/*
	 * Since the wpan_desc_ptr_t might have various sizes depending
	 * on the platform type (8 bit vs. 32 bit), but the test
	 * harness requires strict 8 bit oriented structure layouts,
	 * the wpan descriptor cannot be copied via memcpy here.
	 */
	*msg_buf++ = PANDescriptor->CoordAddrSpec.AddrMode;

#ifdef TEST_HARNESS_BIG_ENDIAN
	*msg_buf++ = PANDescriptor->CoordAddrSpec.PANId >> 8;
	*msg_buf++ = PANDescriptor->CoordAddrSpec.PANId;
#else
	*msg_buf++ = PANDescriptor->CoordAddrSpec.PANId;
	*msg_buf++ = PANDescriptor->CoordAddrSpec.PANId >> 8;
#endif
	memcpy(msg_buf, &(PANDescriptor->CoordAddrSpec.Addr), 8);
	msg_buf += 8;

	*msg_buf++ = PANDescriptor->LogicalChannel;
	*msg_buf++ = PANDescriptor->ChannelPage;
	*msg_buf++ = PANDescriptor->SuperframeSpec;
	*msg_buf++ = PANDescriptor->SuperframeSpec >> 8;
	*msg_buf++ = PANDescriptor->GTSPermit;
	*msg_buf++ = PANDescriptor->LinkQuality;
#ifdef ENABLE_TSTAMP
	*msg_buf++ = PANDescriptor->TimeStamp;
	*msg_buf++ = PANDescriptor->TimeStamp >> 8;
	*msg_buf++ = PANDescriptor->TimeStamp >> 16;
	*msg_buf++ = PANDescriptor->TimeStamp >> 24;
#endif  /* ENABLE_TSTAMP */

	*msg_ptr_to_size += WPAN_DESCRIPTOR_LEN;

	*msg_buf++ = PendAddrSpec;
	*msg_ptr_to_size += 1;

	/* Variable data portion */
	*msg_buf++ = sduLength + short_addr_list + ext_addr_list;
	*msg_ptr_to_size += 1;

	/* Sizes of short address list and extended address list */
	memcpy(msg_buf, AddrList, short_addr_list);
	msg_buf += short_addr_list;
	*msg_ptr_to_size += short_addr_list;
	memcpy(msg_buf,
			(uint8_t *)AddrList + short_addr_list,
			ext_addr_list);
	msg_buf += ext_addr_list;
	*msg_ptr_to_size += ext_addr_list;

	/* SDU */
	memcpy(msg_buf, sdu, sduLength);
	msg_buf += sduLength;
	*msg_ptr_to_size += sduLength;

	*msg_buf = EOT;
}

#endif  /* (MAC_BEACON_NOTIFY_INDICATION == 1) */

#if ((MAC_ORPHAN_INDICATION_RESPONSE == 1) || \
	(MAC_ASSOCIATION_INDICATION_RESPONSE == 1))

/*
 * @brief Callback function usr_mlme_comm_status_ind
 *
 * @param SrcAddrSpec      Pointer to source address specification
 * @param DstAddrSpec      Pointer to destination address specification
 * @param status           Result for related response operation
 */
void usr_mlme_comm_status_ind(wpan_addr_spec_t *SrcAddrSpec,
		wpan_addr_spec_t *DstAddrSpec,
		uint8_t status)
{
	uint8_t *msg_buf;

	msg_buf = get_next_tx_buffer();

	*msg_buf++ = MLME_COMM_STATUS_IND_LEN + MAC_PID_LEN;
	*msg_buf++ = MAC_PID;
	*msg_buf++ = MLME_COMM_STATUS_INDICATION;

	/* Do not change order of the elements */
#ifdef TEST_HARNESS_BIG_ENDIAN
	*msg_buf++ = SrcAddrSpec->PANId >> 8;
	*msg_buf++ = SrcAddrSpec->PANId;
#else
	*msg_buf++ = SrcAddrSpec->PANId;
	*msg_buf++ = SrcAddrSpec->PANId >> 8;
#endif
	*msg_buf++ = SrcAddrSpec->AddrMode;
	memcpy(msg_buf, &(SrcAddrSpec->Addr), 8);
	msg_buf += 8;
	*msg_buf++ = DstAddrSpec->AddrMode;
	memcpy(msg_buf, &(DstAddrSpec->Addr), 8);

	msg_buf += 8;
	*msg_buf++ = status;
	*msg_buf = EOT;
}

#endif  /* ((MAC_ORPHAN_INDICATION_RESPONSE == 1) ||
         *(MAC_ASSOCIATION_INDICATION_RESPONSE == 1)) */

#if (MAC_DISASSOCIATION_BASIC_SUPPORT == 1)

/*
 * Callback function usr_mlme_disassociate_conf
 *
 * @param status             Result of requested disassociate operation.
 * @param DeviceAddrSpec     Pointer to wpan_addr_spec_t structure for device
 *                           that has either requested disassociation or been
 *                           instructed to disassociate by its coordinator.
 *
 * @return void
 */
void usr_mlme_disassociate_conf(uint8_t status,
		wpan_addr_spec_t *DeviceAddrSpec)
{
	uint8_t *msg_buf;

	msg_buf = get_next_tx_buffer();

	*msg_buf++ = MLME_DISASSOCIATE_CONF_LEN + MAC_PID_LEN;
	*msg_buf++ = MAC_PID;
	*msg_buf++ = MLME_DISASSOCIATE_CONFIRM;

	*msg_buf++ = status;

	*msg_buf++ = DeviceAddrSpec->AddrMode;
#ifdef TEST_HARNESS_BIG_ENDIAN
	*msg_buf++ = DeviceAddrSpec->PANId >> 8;
	*msg_buf++ = DeviceAddrSpec->PANId;
#else
	*msg_buf++ = DeviceAddrSpec->PANId;
	*msg_buf++ = DeviceAddrSpec->PANId >> 8;
#endif
	memcpy(msg_buf, &(DeviceAddrSpec->Addr), 8);
	msg_buf += 8;
	*msg_buf = EOT;
}

#endif /* (MAC_DISASSOCIATION_BASIC_SUPPORT == 1) */

#if (MAC_DISASSOCIATION_BASIC_SUPPORT == 1)

/*
 * Callback function usr_mlme_disassociate_ind
 *
 * @param DeviceAddress        Extended address of device which initiated the
 *                             disassociation request.
 * @param DisassociateReason   Reason for the disassociation. Valid values:
 *                           - @ref WPAN_DISASSOC_BYPARENT,
 *                           - @ref WPAN_DISASSOC_BYCHILD.
 *
 * @return void
 */
void usr_mlme_disassociate_ind(uint64_t DeviceAddress,
		uint8_t DisassociateReason)
{
	uint8_t *msg_buf;

	msg_buf = get_next_tx_buffer();
	*msg_buf++ = MLME_DISASSOCIATE_IND_LEN + MAC_PID_LEN;
	*msg_buf++ = MAC_PID;
	*msg_buf++ = MLME_DISASSOCIATE_INDICATION;
	memcpy(msg_buf, &DeviceAddress, 8);
	msg_buf += 8;
	*msg_buf++ = DisassociateReason;
	*msg_buf = EOT;
}

#endif  /* (MAC_DISASSOCIATION_BASIC_SUPPORT == 1) */

#if (MAC_GET_SUPPORT == 1)

/*
 * Callback function usr_mlme_get_conf
 *
 * @param status            Result of requested PIB attribute get operation.
 * @param PIBAttribute      Retrieved PIB attribute.
 * @param PIBAttributeIndex Index of the PIB attribute to be read.
 * @param PIBAttributeValue Pointer to data containing retrieved PIB attribute,
 *
 * @return void
 */
void usr_mlme_get_conf(uint8_t status,
		uint8_t PIBAttribute,
#if ((defined MAC_SECURITY_ZIP)  || (defined MAC_SECURITY_2006))
		uint8_t PIBAttributeIndex,
#endif  /* ((defined MAC_SECURITY_ZIP)  || (defined MAC_SECURITY_2006)) */
		void *PIBAttributeValue)
{
	uint8_t *msg_buf;
	uint8_t pib_attribute_octet_no;
	/* Pointer to size element - the content is written later. */
	uint8_t *msg_ptr_to_size;

	msg_buf = get_next_tx_buffer();
	/* Store pointer to size element. */
	msg_ptr_to_size = msg_buf;

	*msg_buf++ = MLME_GET_CONF_LEN + MAC_PID_LEN;
	*msg_buf++ = MAC_PID;
	*msg_buf++ = MLME_GET_CONFIRM;
	*msg_buf++ = status;
	*msg_buf++ = PIBAttribute;

#if ((defined MAC_SECURITY_ZIP)  || (defined MAC_SECURITY_2006))
	*msg_buf++ = PIBAttributeIndex;
#endif

	/* Get size of PIB attribute to be set */
	pib_attribute_octet_no = mac_get_pib_attribute_size(PIBAttribute);
	*msg_buf++ = pib_attribute_octet_no;

	/* Attribute value */

#ifdef TEST_HARNESS_BIG_ENDIAN
	if ((macMaxFrameTotalWaitTime == PIBAttribute) ||
			(macResponseWaitTime == PIBAttribute) ||
			(macTransactionPersistenceTime == PIBAttribute) ||
			(macBeaconTxTime == PIBAttribute)
#ifdef MAC_SECURITY_ZIP
			|| (macFrameCounter == PIBAttribute)
#endif
			) {
		memcpy_be(msg_buf, PIBAttributeValue, pib_attribute_octet_no);
	} else {
		memcpy(msg_buf, PIBAttributeValue, pib_attribute_octet_no);
	}

#else
	memcpy(msg_buf, PIBAttributeValue, pib_attribute_octet_no);
#endif

	msg_buf += pib_attribute_octet_no;
	*msg_ptr_to_size += pib_attribute_octet_no;
	*msg_buf = EOT;
}

#endif  /* (MAC_GET_SUPPORT == 1) */

#if (MAC_ORPHAN_INDICATION_RESPONSE == 1)

/*
 * Callback function usr_mlme_orphan_ind
 *
 * @param OrphanAddress     Address of orphaned device.
 *
 * @return void
 *
 */
void usr_mlme_orphan_ind(uint64_t OrphanAddress)
{
	uint8_t *msg_buf;

	msg_buf = get_next_tx_buffer();
	*msg_buf++ = MLME_ORPHAN_IND_LEN + MAC_PID_LEN;
	*msg_buf++ = MAC_PID;
	*msg_buf++ = MLME_ORPHAN_INDICATION;
	memcpy(msg_buf, &OrphanAddress, 8);
	msg_buf += 8;
	*msg_buf = EOT;
}

#endif  /* (MAC_ORPHAN_INDICATION_RESPONSE == 1) */

#if (MAC_INDIRECT_DATA_BASIC == 1)

/*
 * Callback function that must be implemented by application (NHLE) for MAC
 * service
 * MLME-POLL.confirm.
 *
 * @param status           Result of requested poll operation.
 *
 * @return void
 *
 */
void usr_mlme_poll_conf(uint8_t status)
{
	uint8_t *msg_buf;

	msg_buf = get_next_tx_buffer();
	*msg_buf++ = MLME_POLL_CONF_LEN + MAC_PID_LEN;
	*msg_buf++ = MAC_PID;
	*msg_buf++ = MLME_POLL_CONFIRM;
	*msg_buf++ = status;
	*msg_buf = EOT;
}

#endif  /* (MAC_INDIRECT_DATA_BASIC == 1) */

/*
 * @brief Callback function usr_mlme_reset_conf
 *
 * @param status Result of the reset procedure
 */
void usr_mlme_reset_conf(uint8_t status)
{
	uint8_t *msg_buf;

	msg_buf = get_next_tx_buffer();
	*msg_buf++ = MLME_RESET_CONF_LEN + MAC_PID_LEN;
	*msg_buf++ = MAC_PID;
	*msg_buf++ = MLME_RESET_CONFIRM;
	*msg_buf++ = status;
	*msg_buf = EOT;
}

#if (defined MAC_SECURITY_ZIP || defined MAC_SECURITY_2006)

/**
 * @brief Handle fake frame interrupt
 *
 * This function handles transceiver fake frames and
 * uploads the frames from the trx.
 */
static inline void handle_fake_frame_irq(uint8_t *fake_frame)
{
	uint8_t ed_value;
	/* Actual frame length of received frame. */
	uint8_t phy_frame_len;
	/* Extended frame length appended by LQI and ED. */
	uint8_t ext_frame_length;
	frame_info_t *receive_frame;
	uint8_t *frame_ptr;

	receive_frame = (frame_info_t *)BMM_BUFFER_POINTER(tal_rx_buffer);

	/* Get ED value; needed to normalize LQI. */
	ed_value = trx_reg_read(RG_PHY_ED_LEVEL);

	/* Get frame length from transceiver. */
	/* trx_frame_read(&phy_frame_len, LENGTH_FIELD_LEN); */
	phy_frame_len = 0x2C;

	/*
	 * The PHY header is also included in the frame (length field), hence
	 * the frame length
	 * is incremented.
	 * In addition to that, the LQI and ED value are uploaded, too.
	 */
	ext_frame_length = phy_frame_len + LENGTH_FIELD_LEN + LQI_LEN +
			ED_VAL_LEN;

	/* Update payload pointer to store received frame. */
	frame_ptr = (uint8_t *)receive_frame + LARGE_BUFFER_SIZE -
			ext_frame_length;

	/*
	 * Note: The following code is different from single chip
	 * transceivers, since reading the frame via SPI contains the length
	 * field
	 * in the first octet.
	 */

	/* trx_frame_read(frame_ptr, LENGTH_FIELD_LEN + phy_frame_len +
	 * LQI_LEN); */
	memcpy(frame_ptr, fake_frame, LENGTH_FIELD_LEN + phy_frame_len +
			LQI_LEN);

	receive_frame->mpdu = frame_ptr;
	/* Add ED value at the end of the frame buffer. */
	receive_frame->mpdu[phy_frame_len + LQI_LEN + ED_VAL_LEN] = ed_value;

#if (defined BEACON_SUPPORT) || (defined ENABLE_TSTAMP)

	/*
	 * Store the timestamp.
	 * The time stamping is only required for beaconing networks
	 * or if time stamping is explicitly enabled.
	 */
	/* receive_frame->time_stamp = tal_rx_timestamp; */
#endif  /* #if (defined BEACON_SUPPORT) || (defined ENABLE_TSTAMP) */

	/* Append received frame to incoming_frame_queue and get new rx buffer.
	**/
	qmm_queue_append(&tal_incoming_frame_queue, tal_rx_buffer);

	/* The previous buffer is eaten up and a new buffer is not assigned yet.
	**/
	tal_rx_buffer = bmm_buffer_alloc(LARGE_BUFFER_SIZE);
}

#endif

#if (MAC_RX_ENABLE_SUPPORT == 1)

/*
 * Callback function usr_mlme_rx_enable_conf
 *
 * @param status           Result of requested receiver enable operation.
 *
 * @return void
 */
void usr_mlme_rx_enable_conf(uint8_t status)
{
	uint8_t *msg_buf;

	msg_buf = get_next_tx_buffer();
	*msg_buf++ = MLME_RX_ENABLE_CONF_LEN + MAC_PID_LEN;
	*msg_buf++ = MAC_PID;
	*msg_buf++ = MLME_RX_ENABLE_CONFIRM;
	*msg_buf++ = status;
	*msg_buf = EOT;
}

#endif  /* (MAC_RX_ENABLE_SUPPORT == 1) */

#if ((MAC_SCAN_ED_REQUEST_CONFIRM == 1)      ||	\
	(MAC_SCAN_ACTIVE_REQUEST_CONFIRM == 1)  || \
	(MAC_SCAN_PASSIVE_REQUEST_CONFIRM == 1) || \
	(MAC_SCAN_ORPHAN_REQUEST_CONFIRM == 1))

/*
 * @brief Callback function usr_mlme_scan_conf
 *
 * @param status            Result of requested scan operation
 * @param ScanType          Type of scan performed
 * @param ChannelPage       Channel page on which the scan was performed
 * @param UnscannedChannels Bitmap of unscanned channels
 * @param ResultListSize    Number of elements in ResultList
 * @param ResultList        Pointer to array of scan results
 */
void usr_mlme_scan_conf(uint8_t status,
		uint8_t ScanType,
		uint8_t ChannelPage,
		uint32_t UnscannedChannels,
		uint8_t ResultListSize,
		void *ResultList)
{
	uint8_t *msg_buf;
	/* Pointer to size element - the content is written later. */
	uint8_t *msg_ptr_to_size;

	msg_buf = get_next_tx_buffer();

	/* Store pointer to size element. */
	msg_ptr_to_size = msg_buf;

	*msg_buf++ = MLME_SCAN_CONF_LEN + MAC_PID_LEN;
	*msg_buf++ = MAC_PID;
	*msg_buf++ = MLME_SCAN_CONFIRM;

	*msg_buf++ = status;
	*msg_buf++ = ScanType;
	*msg_buf++ = ChannelPage;
	*msg_buf++ = UnscannedChannels;
	*msg_buf++ = UnscannedChannels >> 8;
	*msg_buf++ = UnscannedChannels >> 16;
	*msg_buf++ = UnscannedChannels >> 24;
	*msg_buf++ = ResultListSize;

	if ((MLME_SCAN_TYPE_ACTIVE == ScanType) ||
			(MLME_SCAN_TYPE_PASSIVE == ScanType)) {
		wpan_pandescriptor_t *wpan_desc_ptr
			= (wpan_pandescriptor_t *)ResultList;

		if (0 == ResultListSize) {
			*msg_buf++ = 0;
		} else {
			*msg_buf++ = ResultListSize * WPAN_DESCRIPTOR_LEN;

			for (uint8_t i = 0; i < ResultListSize; i++) {
				/*
				 * Since the wpan_desc_ptr_t might have various
				 * sizes depending
				 * on the platform type (8 bit vs. 32 bit), but
				 * the test
				 * harness requires strict 8 bit oriented
				 * structure layouts,
				 * the wpan descriptor cannot be copied via
				 * memcpy here.
				 */
				*msg_buf++
					= wpan_desc_ptr->CoordAddrSpec.AddrMode;
#ifdef TEST_HARNESS_BIG_ENDIAN
				*msg_buf++
					= wpan_desc_ptr->CoordAddrSpec.PANId >>
						8;
				*msg_buf++ = wpan_desc_ptr->CoordAddrSpec.PANId;
#else
				*msg_buf++ = wpan_desc_ptr->CoordAddrSpec.PANId;
				*msg_buf++
					= wpan_desc_ptr->CoordAddrSpec.PANId >>
						8;
#endif
				memcpy(msg_buf,
						&(wpan_desc_ptr->CoordAddrSpec.
						Addr),
						8);
				msg_buf += 8;

				*msg_buf++ = wpan_desc_ptr->LogicalChannel;
				*msg_buf++ = wpan_desc_ptr->ChannelPage;
				*msg_buf++ = wpan_desc_ptr->SuperframeSpec;
				*msg_buf++ = wpan_desc_ptr->SuperframeSpec >> 8;
				*msg_buf++ = wpan_desc_ptr->GTSPermit;
				*msg_buf++ = wpan_desc_ptr->LinkQuality;
#ifdef ENABLE_TSTAMP
				*msg_buf++ = wpan_desc_ptr->TimeStamp;
				*msg_buf++ = wpan_desc_ptr->TimeStamp >> 8;
				*msg_buf++ = wpan_desc_ptr->TimeStamp >> 16;
				*msg_buf++ = wpan_desc_ptr->TimeStamp >> 24;
#endif  /* ENABLE_TSTAMP */

				wpan_desc_ptr++;
				*msg_ptr_to_size += WPAN_DESCRIPTOR_LEN;
			}
		}
	} else if (MLME_SCAN_TYPE_ED == ScanType) {
		if (0 == ResultListSize) {
			*msg_buf++ = 0;
		} else {
			*msg_buf++ = ResultListSize;
			memcpy(msg_buf, (uint8_t *)ResultList, ResultListSize);
			msg_buf += ResultListSize;
			*msg_ptr_to_size += ResultListSize;
		}
	} else { /* Orphan Scan */
		*msg_buf++ = ResultListSize;
		*msg_buf++ = 0; /* no variable part */
		*msg_ptr_to_size += 1;
	}

	*msg_buf = EOT;
}

#endif

/*
 * @brief Callback function usr_mlme_set_conf
 *
 * @param status        Result of requested PIB attribute set operation
 * @param PIBAttribute  Updated PIB attribute
 */
#if ((defined MAC_SECURITY_ZIP)  || (defined MAC_SECURITY_2006))
void usr_mlme_set_conf(uint8_t status,
		uint8_t PIBAttribute,
		uint8_t PIBAttributeIndex)

#else
void usr_mlme_set_conf(uint8_t status,
		uint8_t PIBAttribute)
#endif  /* (MAC_SECURITY_ZIP || MAC_SECURITY_2006) */
{
	uint8_t *msg_buf;

	msg_buf = get_next_tx_buffer();
	*msg_buf++ = MLME_SET_CONF_LEN + MAC_PID_LEN;
	*msg_buf++ = MAC_PID;
	*msg_buf++ = MLME_SET_CONFIRM;
	*msg_buf++ = status;
	*msg_buf++ = PIBAttribute;
#if ((defined MAC_SECURITY_ZIP)  || (defined MAC_SECURITY_2006))
	*msg_buf++ = PIBAttributeIndex;
#endif
	*msg_buf = EOT;
}

#if (MAC_START_REQUEST_CONFIRM == 1)

/*
 * @brief Callback function usr_mlme_start_conf
 *
 * @param status        Result of requested start operation
 */
void usr_mlme_start_conf(uint8_t status)
{
	uint8_t *msg_buf;

	msg_buf = get_next_tx_buffer();

	*msg_buf++ = MLME_START_CONF_LEN + MAC_PID_LEN;
	*msg_buf++ = MAC_PID;
	*msg_buf++ = MLME_START_CONFIRM;
	*msg_buf++ = status;

	*msg_buf = EOT;
}

#endif  /* (MAC_START_REQUEST_CONFIRM == 1) */

/*
 * Callback function usr_mlme_sync_loss_ind
 *
 * @param LossReason     Reason for synchronization loss.
 * @param PANId          The PAN identifier with which the device lost
 *                       synchronization or to which it was realigned.
 * @param LogicalChannel The logical channel on which the device lost
 *                       synchronization or to which it was realigned.
 * @param ChannelPage    The channel page on which the device lost
 *                       synchronization or to which it was realigned.
 *
 * @return void
 *
 */
void usr_mlme_sync_loss_ind(uint8_t LossReason,
		uint16_t PANId,
		uint8_t LogicalChannel,
		uint8_t ChannelPage)
{
	uint8_t *msg_buf;

	msg_buf = get_next_tx_buffer();
	*msg_buf++ = MLME_SYNC_LOSS_IND_LEN + MAC_PID_LEN;
	*msg_buf++ = MAC_PID;
	*msg_buf++ = MLME_SYNC_LOSS_INDICATION;
	*msg_buf++ = LossReason;
#ifdef TEST_HARNESS_BIG_ENDIAN
	*msg_buf++ = PANId >> 8;
	*msg_buf++ = PANId;
#else
	*msg_buf++ = PANId;
	*msg_buf++ = PANId >> 8;
#endif

	*msg_buf++ = LogicalChannel;
	*msg_buf++ = ChannelPage;
	*msg_buf = EOT;
}

#ifdef GTS_SUPPORT
#if (MAC_GTS_REQUEST == 1)
void usr_mlme_gts_conf(gts_char_t GtsChar, uint8_t status)
{
	uint8_t *msg_buf;

	msg_buf = get_next_tx_buffer();
	*msg_buf++ = MLME_GTS_CONF_LEN + MAC_PID_LEN;
	*msg_buf++ = MAC_PID;
	*msg_buf++ = MLME_GTS_CONFIRM;
	*msg_buf++ = *((uint8_t *)&GtsChar);
	*msg_buf++ = status;
	*msg_buf = EOT;
}

void usr_mlme_gts_ind(uint16_t DeviceAddr, gts_char_t GtsChar)
{
	uint8_t *msg_buf;

	msg_buf = get_next_tx_buffer();
	*msg_buf++ = MLME_GTS_IND_LEN + MAC_PID_LEN;
	*msg_buf++ = MAC_PID;
	*msg_buf++ = MLME_GTS_INDICATION;
#ifdef TEST_HARNESS_BIG_ENDIAN
	*msg_buf++ = DeviceAddr >> 8;
	*msg_buf++ = DeviceAddr;
#else
	*msg_buf++ = DeviceAddr;
	*msg_buf++ = DeviceAddr >> 8;
#endif
	*msg_buf++ = *((uint8_t *)&GtsChar);
	*msg_buf = EOT;
}

#endif /* (MAC_GTS_REQUEST == 1) */
#endif /* GTS_SUPPORT */

/**
 * \brief Parses the Received Data in the Buffer and Process the Commands
 * accordingly.
 */
static void handle_incoming_msg(void)
{
	bool ret_val;
	switch (sio_rx_buf[1]) { /* Protocol Identifier type*/
	case MAC_PID:
	{
		switch (sio_rx_buf[2]) { /* message type */
		case MCPS_DATA_REQUEST:
		{
			/* Order of reception:
			 * With MAC_SECURITY_ZIP:
			 *     size;
			 *                         ProtocolId;
			 *     cmdCode;
			 *     SrcAddrMode;
			 *     DstAddrMode;
			 *     DstPANId;
			 *     DstAddr;
			 *     msduHandle;
			 *     TxOptions;
			 *     SecurityLevel;
			 *     KeyIdMode;
			 *     KeyIndex;
			 *     msduLength;
			 *     msdu*;
			 *
			 *                          Without MAC_SECURITY:
			 *                          size;
			 *                              ProtocolId;
			 *                          cmdCode;
			 *                          SrcAddrMode;
			 *                          DstAddrMode;
			 *                          DstPANId;
			 *                          DstAddr;
			 *                          msduHandle;
			 *                          TxOptions;
			 *                          msduLength;
			 *                          msdu*;
			 */

			uint8_t src_addr_mode = sio_rx_buf[3];
			wpan_addr_spec_t dst_addr;

			dst_addr.AddrMode = sio_rx_buf[4];
			dst_addr.PANId
				= ((uint16_t)sio_rx_buf[5] |
					((uint16_t)sio_rx_buf[6]
					<< 8));
			memcpy(&dst_addr.Addr.long_address, &sio_rx_buf[7],
					sizeof(uint64_t));
			uint8_t msdu_handle = sio_rx_buf[15];
			uint8_t tx_options = sio_rx_buf[16];

#if (defined MAC_SECURITY_ZIP || defined MAC_SECURITY_2006)
			uint8_t security_level = sio_rx_buf[17];
			uint8_t key_id_mode = sio_rx_buf[18];
			uint8_t *key_source = NULL;

			if (key_id_mode == 3) {
				memcpy(key_source_8,
						&dst_addr.Addr.long_address, 8);
				key_source = key_source_8;
			} else if (key_id_mode == 2) {
				key_source_4[0] = (uint8_t)dst_addr.PANId;
				key_source_4[1]
					= (uint8_t)(dst_addr.PANId >> 8);

				/*                    key_source_4[2] =
				 * (uint8_t)(mac_sec_pib.PANCoordShortAddress);
				 **/

				/*                    key_source_4[3] =
				 * (uint8_t)(mac_sec_pib.PANCoordShortAddress >>
				 * 8); */
				key_source_4[2]
					= (uint8_t)(mac_pib.
						mac_CoordShortAddress);
				key_source_4[3]
					= (uint8_t)(mac_pib.
						mac_CoordShortAddress
						>> 8);
				key_source = key_source_4;
			}

			uint8_t key_index = sio_rx_buf[19];

			uint8_t msdu_length = sio_rx_buf[20];
			uint8_t *msdu = &sio_rx_buf[22];

			ret_val = wpan_mcps_data_req(src_addr_mode,
					&dst_addr,
					msdu_length,
					msdu,
					msdu_handle,
					tx_options,
					security_level,
					key_source,
					key_id_mode,
					key_index);
#else   /* No MAC_SECURITY */
			uint8_t msdu_length = sio_rx_buf[17];
			uint8_t *msdu = &sio_rx_buf[19];

			/*
			 * bool wpan_mcps_data_req(uint8_t SrcAddrMode,
			 *                       wpan_addr_spec_t *DstAddrSpec,
			 *                       uint8_t msduLength,
			 *                       uint8_t *msdu,
			 *                       uint8_t msduHandle,
			 *                       uint8_t TxOptions);
			 */
			ret_val = wpan_mcps_data_req(src_addr_mode,
					&dst_addr,
					msdu_length,
					msdu,
					msdu_handle,
					tx_options);
#endif  /* MAC_SECURITY */

			/*
			 * No Assert for MCPS-DATA request, since a too large
			 * payload
			 * will also return true, but this is correct behavior
			 * in the
			 * test case then.
			 */
		}
		break;

#if ((MAC_PURGE_REQUEST_CONFIRM == 1) && (MAC_INDIRECT_DATA_BASIC == 1))
		case MCPS_PURGE_REQUEST:
		{
			/* Order of reception:
			 *     size;
			 *     cmdCode;
			 *     msduHandle uint8_t;
			 */

			/*
			 * bool wpan_mcps_purge_req(const uint8_t msduHandle);
			 */

			ret_val = wpan_mcps_purge_req(sio_rx_buf[3]);

			if (ret_val == false) {
				Assert(
						"Test harness: Purge Request not successful" ==
						0);
			}
		}
		break;
#endif  /* ((MAC_PURGE_REQUEST_CONFIRM == 1) && (MAC_INDIRECT_DATA_BASIC == 1))
				**/

#if (MAC_ASSOCIATION_REQUEST_CONFIRM == 1)
		case MLME_ASSOCIATE_REQUEST:
		{
			/* Order of reception:
			 * size;
			 * ProtocolId,
			 * cmdCode;
			 * LogicalChannel uint8_t;
			 * ChannelPage uint8_t;
			 * CoordAddrMode uint8_t;
			 * CoordPANId uint16_t;
			 * CoordAddress address_field_t;
			 * CapabilityInformation uint8_t;
			 */

			/*
			 * bool wpan_mlme_associate_req(uint8_t LogicalChannel,
			 *                            uint8_t ChannelPage,
			 *                            wpan_addr_spec_t
			 * *CoordAddrSpec,
			 *                            uint8_t
			 * CapabilityInformation);
			 */

			wpan_addr_spec_t coord_addr;

			coord_addr.AddrMode = sio_rx_buf[5];
			coord_addr.PANId
				= ((uint16_t)sio_rx_buf[6] |
					((uint16_t)sio_rx_buf[7]
					<< 8));
			memcpy(&coord_addr.Addr.long_address, &sio_rx_buf[8],
					sizeof(uint64_t));
			ret_val = wpan_mlme_associate_req(sio_rx_buf[3],
					sio_rx_buf[4],
					&coord_addr,
					sio_rx_buf[16]);
			if (ret_val == false) {
				Assert(
						"Test harness: Associate Request not successful" ==
						0);
			}
		}
		break;
#endif  /* (MAC_ASSOCIATION_REQUEST_CONFIRM == 1) */

#if (MAC_ASSOCIATION_INDICATION_RESPONSE == 1)
		case MLME_ASSOCIATE_RESPONSE:
		{
			/* Order of reception:
			 * size;
			 * ProtocolId,
			 * cmdCode;
			 * DeviceAddress uint64_t;
			 * AssocShortAddress uint16_t;
			 * status uint8_t;
			 */

			/*
			 * bool wpan_mlme_associate_resp(uint64_t DeviceAddress,
			 *                             uint16_t
			 * AssocShortAddress,
			 *                             uint8_t status);
			 */

			ret_val
				= wpan_mlme_associate_resp(((uint64_t)sio_rx_buf
					[
						3] |
					((uint64_t)sio_rx_buf[4]
					<< 8) |
					(uint64_t)sio_rx_buf[5] << 16 |
					((uint64_t)sio_rx_buf[6]
					<< 24) |
					(uint64_t)sio_rx_buf[7] << 32 |
					((uint64_t)sio_rx_buf[8]
					<< 40) |
					(uint64_t)sio_rx_buf[9] << 48 |
					((uint64_t)sio_rx_buf[10
					] << 56)),
					(uint16_t)sio_rx_buf[11] |
					((uint16_t)sio_rx_buf[12
					] << 8),
					sio_rx_buf[13]);
			if (ret_val == false) {
				Assert(
						"Test harness: Associate Reqsponse not successful" ==
						0);
			}
		}
		break;
#endif  /* (MAC_ASSOCIATION_INDICATION_RESPONSE == 1) */

		case MLME_DISASSOCIATE_REQUEST:
		{
			/* Order of reception:
			 * size;
			 * ProtocolId,
			 * cmdCode;
			 * DeviceAddrMode uint8_t;
			 * DevicePANId uint16_t;
			 * DeviceAddress uint64_t;
			 * DisassociateReason uint8_t;
			 * TxIndirect uint8_t;
			 */

			/*
			 * bool wpan_mlme_disassociate_req(wpan_addr_spec_t
			 * *DeviceAddrSpec,
			 *                               uint8_t
			 * DisassociateReason,
			 *                               bool TxIndirect);
			 */

			wpan_addr_spec_t dev_addr;

			dev_addr.AddrMode = sio_rx_buf[3];
			dev_addr.PANId
				= ((uint16_t)sio_rx_buf[4] |
					((uint16_t)sio_rx_buf[5]
					<< 8));
#ifdef TEST_HARNESS_BIG_ENDIAN
			if (FCF_SHORT_ADDR == dev_addr.AddrMode) {
				memcpy(&dev_addr.Addr.short_address,
						&sio_rx_buf[6],
						sizeof(uint16_t));
			} else {
				memcpy(&dev_addr.Addr.long_address,
						&sio_rx_buf[6],
						sizeof(uint64_t));
			}

#else
			memcpy(&dev_addr.Addr.long_address, &sio_rx_buf[6],
					sizeof(uint64_t));
#endif

			ret_val = wpan_mlme_disassociate_req(&dev_addr,
					sio_rx_buf[14],
					sio_rx_buf[15]);
			if (ret_val == false) {
				Assert(
						"Test harness: Disassociate Request not successful" ==
						0);
			}
		}
		break;

		case MLME_GET_REQUEST:

			/* Order of reception:
			 * size;
			 * ProtocolId,
			 * cmdCode;
			 * PIBAttribute uint8_t;
			 */

			/*
			 * bool wpan_mlme_get_req(uint8_t PIBAttribute);
			 */

#if ((defined MAC_SECURITY_ZIP)  || (defined MAC_SECURITY_2006))
			ret_val
				= wpan_mlme_get_req(sio_rx_buf[3],
					sio_rx_buf[4]);
#else
			ret_val = wpan_mlme_get_req(sio_rx_buf[3]);
#endif  /* (MAC_SECURITY_ZIP || MAC_SECURITY_2006) */

			if (ret_val == false) {
				Assert(
						"Test harness: Get Request not successful" ==
						0);
			}

			break;

#if (MAC_ORPHAN_INDICATION_RESPONSE == 1)
		case MLME_ORPHAN_RESPONSE:
		{
			/* Order of reception:
			 * size;
			 * ProtocolId,
			 * cmdCode;
			 * OrphanAddress uint64_t;
			 * ShortAddress uint16_t;
			 * AssociatedMember uint8_t;
			 */

			/*
			 * bool wpan_mlme_orphan_resp(uint64_t OrphanAddress,
			 *                          uint16_t ShortAddress,
			 *                          bool );
			 */

			ret_val
				= wpan_mlme_orphan_resp(((uint64_t)sio_rx_buf[3]
					|
					((uint64_t)sio_rx_buf[4]
					<< 8) |
					(uint64_t)sio_rx_buf[5] << 16 |
					((uint64_t)sio_rx_buf[6]
					<< 24) |
					(uint64_t)sio_rx_buf[7] << 32 |
					((uint64_t)sio_rx_buf[8]
					<< 40) |
					(uint64_t)sio_rx_buf[9] << 48 |
					((uint64_t)sio_rx_buf[10
					] << 56)),
					/* ShortAddress */
					(uint16_t)sio_rx_buf[11] |
					((uint16_t)sio_rx_buf[12
					] << 8),
					/* AssociatedMember */
					sio_rx_buf[13]);

			if (ret_val == false) {
				Assert(
						"Test harness: Orphan Response not successful" ==
						0);
			}
		}
		break;
#endif  /* (MAC_ORPHAN_INDICATION_RESPONSE == 1) */

		case MLME_POLL_REQUEST:
		{
			/* Order of reception:
			 * size;
			 * ProtocolId,
			 * cmdCode;
			 * CoordAddrMode uint8_t;
			 * CoordPANId uint16_t;
			 * CoordAddress uint64_t;
			 */

			/*
			 * bool wpan_mlme_poll_req(wpan_addr_spec_t
			 * *CoordAddrSpec);
			 */

			wpan_addr_spec_t coord_addr;

			coord_addr.AddrMode = sio_rx_buf[3];
			coord_addr.PANId
				= ((uint16_t)sio_rx_buf[4] |
					((uint16_t)sio_rx_buf[5]
					<< 8));
#ifdef TEST_HARNESS_BIG_ENDIAN
			if (FCF_SHORT_ADDR == coord_addr.AddrMode) {
				memcpy(&(coord_addr.Addr.short_address),
						&sio_rx_buf[6],
						sizeof(uint16_t));
			} else {
				memcpy(&(coord_addr.Addr.long_address),
						&sio_rx_buf[6],
						sizeof(uint64_t));
			}

#else
			memcpy(&coord_addr.Addr.long_address, &sio_rx_buf[6],
					sizeof(uint64_t));
#endif

			ret_val = wpan_mlme_poll_req(&coord_addr);

			if (ret_val == false) {
				Assert(
						"Test harness: Poll Request not successful" ==
						0);
			}
		}
		break;

		case MLME_RESET_REQUEST:

			/* Order of reception:
			 * size;
			 * ProtocolId,
			 * cmdCode;
			 * SetDefaultPIB;
			 */
			/* bool mlme_reset_request(bool SetDefaultPIB); */
			ret_val = wpan_mlme_reset_req(sio_rx_buf[3]);

			if (ret_val == false) {
				Assert(
						"Test harness: Reset Request not successful" ==
						0);
			}

			break;

		case MLME_RX_ENABLE_REQUEST:
		{
			/* Order of reception:
			 * size;
			 * ProtocolId,
			 * cmdCode;
			 * DeferPermit uint8_t;
			 * RxOnTime uint32_t;
			 * RxOnDuration uint32_t;
			 */

			/*
			 * bool wpan_mlme_rx_enable_req(bool DeferPermit,
			 *                            uint32_t RxOnTime,
			 *                            uint32_t RxOnDuration);
			 */

			ret_val = wpan_mlme_rx_enable_req(sio_rx_buf[3],
					/* RxOnTime */
					((uint32_t)sio_rx_buf[4] |
					((uint32_t)sio_rx_buf[5]
					<< 8) |
					(uint32_t)sio_rx_buf[6] << 16 |
					((uint32_t)sio_rx_buf[7]
					<< 24)
					),
					/* RxOnDuration */
					((uint32_t)sio_rx_buf[8] |
					((uint32_t)sio_rx_buf[9]
					<< 8) |
					(uint32_t)sio_rx_buf[10] << 16 |
					((uint32_t)sio_rx_buf[11
					] << 24)
					)
					);
			if (ret_val == false) {
				Assert(
						"Test harness: Rx_Enable Request not successful" ==
						0);
			}
		}
		break;

		case MLME_SCAN_REQUEST:
		{
			/* Order of reception:
			 * size;
			 * ProtocolId,
			 * cmdCode;
			 * ScanType;
			 * uint32_t ScanChannels;
			 * uint8_t ScanDuration;
			 * uint8_t ChannelPage;
			 */

			/*
			 * bool wpan_mlme_scan_req(uint8_t ScanType,
			 *                       uint32_t ScanChannels,
			 *                       uint8_t ScanDuration,
			 *                       uint8_t ChannelPage);
			 */
			uint32_t scan_channels = ((uint32_t)sio_rx_buf[4] |
					((uint32_t)sio_rx_buf[5] << 8) |
					((uint32_t)sio_rx_buf[6] << 16) |
					((uint32_t)sio_rx_buf[7] << 24)
					);

			ret_val = wpan_mlme_scan_req(sio_rx_buf[3],          /*
					                                      *
					                                      *
					                                      *
					                                      *ScanType
					                                      **/
					scan_channels,                       /*
					                                      *
					                                      *
					                                      *
					                                      *ScanChannels
					                                      **/
					sio_rx_buf[8],                       /*
					                                      *
					                                      *
					                                      *
					                                      *ScanDuration
					                                      **/
					sio_rx_buf[9]);                      /*
					                                      *
					                                      *
					                                      *
					                                      *ChannelPage
					                                      **/
			if (ret_val == false) {
				Assert(
						"Test harness: Scan Request not successful" ==
						0);
			}
		}
		break;

		case MLME_SET_REQUEST:

			/* Order of reception:
			 * size;
			 * ProtocolId,
			 * cmdCode;
			 * PIBAttribute;
			 * PIBAttributeValue;
			 */

			/*
			 * bool wpan_mlme_set_req(uint8_t PIBAttribute,
			 *                      uint8_t *PIBAttributeValue);
			 */
#if (defined MAC_SECURITY_ZIP) || (defined MAC_SECURITY_2006)
			ret_val = wpan_mlme_set_req(sio_rx_buf[3],
					sio_rx_buf[4],
					&sio_rx_buf[6]);

#else
			ret_val = wpan_mlme_set_req(sio_rx_buf[3],
					&sio_rx_buf[5]);
#endif  /* (MAC_SECURITY_ZIP || MAC_SECURITY_2006)  */

			if (ret_val == false) {
				Assert(
						"Test harness: Set Request not successful" ==
						0);
			}

			break;

#if (MAC_START_REQUEST_CONFIRM == 1)
		case MLME_START_REQUEST:

			/* Order of reception:
			 * size;
			 * ProtocolId,
			 * cmdCode;
			 * PANId uint16_t;
			 * LogicalChannel uint8_t;
			 * ChannelPage uint8_t;
			 * BeaconOrder uint8_t;
			 * SuperframeOrder uint8_t;
			 * PANCoordinator uint8_t;
			 * BatteryLifeExtension uint8_t;
			 * CoordRealignment uint8_t;
			 */

			/*
			 * bool wpan_mlme_start_req(uint16_t PANId,
			 *                        uint8_t LogicalChannel,
			 *                        uint8_t ChannelPage,
			 *                        uint8_t BeaconOrder,
			 *                        uint8_t SuperframeOrder,
			 *                        bool PANCoordinator,
			 *                        bool BatteryLifeExtension,
			 *                        bool CoordRealignment);
			 */
			ret_val = wpan_mlme_start_req(
					(uint16_t)sio_rx_buf[3] |
					((uint16_t)sio_rx_buf[4] << 8),
					sio_rx_buf[5],
					sio_rx_buf[6],
					sio_rx_buf[7],
					sio_rx_buf[8],
					sio_rx_buf[9],
					sio_rx_buf[10],
					sio_rx_buf[11]);
			if (ret_val == false) {
				Assert(
						"Test harness: Start Request not successful" ==
						0);
			}
			break;
#endif  /* (MAC_START_REQUEST_CONFIRM == 1) */

#if (MAC_SYNC_REQUEST == 1)
		case MLME_SYNC_REQUEST:

			/* Order of reception:
			 * size;
			 * ProtocolId,
			 * cmdCode;
			 * LogicalChannel uint8_t;
			 * ChannelPage uint8_t;
			 * TrackBeacon uint8_t;
			 */

			/*
			 * bool wpan_mlme_sync_req(uint8_t LogicalChannel,
			 *                       uint8_t ChannelPage,
			 *                       bool TrackBeacon);
			 */
			ret_val = wpan_mlme_sync_req(sio_rx_buf[3],
					sio_rx_buf[4],
					sio_rx_buf[5]);
			if (ret_val == false) {
				Assert(
						"Test harness: Sync Request not successful" ==
						0);
			}
			break;
#endif /* (MAC_SYNC_REQUEST == 1) */

#if (defined MAC_SECURITY_ZIP || defined MAC_SECURITY_2006)
		case MAC_INC_FRAME:
			/* mac_process_tal_data_ind(&sio_rx_buf[3]); */

			handle_fake_frame_irq(&sio_rx_buf[4]);
			break;
#endif

#ifdef GTS_SUPPORT
#if (MAC_GTS_REQUEST == 1)
		case MLME_GTS_REQUEST:
		{
			uint16_t DevAddr = ((uint16_t)sio_rx_buf[3] |
					((uint16_t)sio_rx_buf[4] << 8));

			/* Order of reception:
			 * size;
			 * ProtocolId,
			 * cmdCode;
			 *
			 */

			/*
			 * bool wpan_mlme_sync_req(uint8_t LogicalChannel,
			 *                       uint8_t ChannelPage,
			 *                       bool TrackBeacon);
			 */
			ret_val
				= wpan_mlme_gts_req(DevAddr,
					*((gts_char_t *)&sio_rx_buf[
						5]));
			if (ret_val == false) {
				Assert(
						"Test harness: GTS Request not successful" ==
						0);
			}

			break;
		}
#endif /* (MAC_GTS_REQUEST == 1) */
#endif /* GTS_SUPPORT */
		default:
			Assert("???" == 0);
			break;
		}
	}
	break;

	default:
		Assert("???" == 0);
		break;
	}
}

/**
 * @brief Process data received from SIO
 */
static inline void process_incoming_sio_data(void)
{
	switch (sio_rx_state) {
	case UART_RX_STATE_SOT:
		sio_rx_ptr = sio_rx_buf;
		if (SOT == data[rx_index]) {
			sio_rx_state = UART_RX_STATE_LENGTH;
		}

		break;

	case UART_RX_STATE_LENGTH:
		sio_rx_length = data[rx_index];
		if (sio_rx_length) {
			sio_rx_state = UART_RX_STATE_DATA;
			*sio_rx_ptr = sio_rx_length;
			sio_rx_ptr++;
		} else {
			/* NULL message */
			sio_rx_ptr = sio_rx_buf;
			sio_rx_state = UART_RX_STATE_SOT;
		}

		break;

	case UART_RX_STATE_DATA:
		*sio_rx_ptr = data[rx_index];
		sio_rx_ptr++;
		sio_rx_length--;
		if (!sio_rx_length) {
			sio_rx_state = UART_RX_STATE_EOT;
		}

		break;

	case UART_RX_STATE_EOT:
		if (EOT == data[rx_index]) {
			/* Message received successfully */
			handle_incoming_msg();
		}

		/* Make rx buffer ready for next reception before handling
		 * received data. */
		sio_rx_ptr = sio_rx_buf;
		sio_rx_state = UART_RX_STATE_SOT;
		break;

	default:
		sio_rx_ptr = sio_rx_buf;
		sio_rx_state = UART_RX_STATE_SOT;
		break;
	}
}

/*
 * @brief Function to handle the state machine serial data exchange.
 */
void serial_data_handler(void)
{
	/* Rx processing */
	if (data_length == 0) {
		/* No data to process, read the stream IO */
		rx_index = 0;
		data_length = sio2host_rx(data, SIO_RX_BUF_SIZE); /* @ToDo 20 ?,
		                                                   * different
		                                                   * values for
		                                                   * USB and
		                                                   * UART ? */
	} else { /* Data has been received, process the data */
		 /* Process each single byte */
		process_incoming_sio_data();
		data_length--;
		rx_index++;
	}

	/* Tx processing */
	if (buf_count != 0) {
		if (sio2host_tx(sio_tx_buf[head],
				(sio_tx_buf[head][1] + 3)) != 0) {
			head++;
			head %= SIO_BUF_COUNT;
			buf_count--;
		} else {
			/* @ToDo what happens if none or only a part of the
			 * bytes could be transmitted? */
		}
	}
}
