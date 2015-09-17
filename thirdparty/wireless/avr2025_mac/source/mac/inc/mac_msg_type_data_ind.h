/**
 * @file mac_msg_type_data_ind.h
 *
 * @brief This file defines all message structures for the MAC.
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

/* Prevent double inclusion */
#ifndef MAC_MSG_TYPE_DATA_IND_H
#define MAC_MSG_TYPE_DATA_IND_H

/* === Includes ============================================================= */

#include "mac_msg_const.h"
#include "return_val.h"

/* === Macros =============================================================== */

/* === Types ================================================================ */

/**
 * \addtogroup group_mac_ds
 * @{
 */

/**
 * @brief This is the MCPS-DATA.indication message structure.
 */

/* In orer to support 32Bit MCUs the MCPS-DATA.indication structure
 * is packed, i.e.,for ARM(GCC & IAR) and AVR32(GCC)
 */
#if ((defined __ICCARM__) || (defined __GNUARM__) || (defined __GNUAVR32__))
/* #pragma pack(1) */
#endif /* __ICCARM__, __GNUARM__ , __GNUAVR32__*/
__PACK__DATA__
typedef struct mcps_data_ind_tag {
	/**< This identifies the message as \ref MCPS_DATA_INDICATION */
	enum msg_code cmdcode;

	/**
	 * The source addressing mode for this primitive corresponding to the
	 * received MPDU. This value can take one of the following values:
	 * 0 x 00 = no address (addressing fields omitted). 0 x 01 = reserved.
	 *  0 x 02 = 16 bit short address. 0 x 03 = 64 bit extended address.
	 */
	uint8_t SrcAddrMode;

	/**
	 * The 16 bit PAN identifier of the entity from which the MSDU was
	 * received.
	 */
	uint16_t SrcPANId;

	/**
	 * The individual device address of the entity from which the
	 * MSDU was received.
	 */
	uint64_t SrcAddr;

	/**
	 * The destination addressing mode for this primitive corresponding to
	 * the
	 * received MPDU. This value can take one of the following values:
	 * 0 x 00 = no address (addressing fields omitted). 0 x 01 = reserved.
	 * 0 x 02 = 16 bit short device address. 0 x 03 = 64 bit extended
	 * device address.
	 */
	uint8_t DstAddrMode;

	/**
	 * The 16 bit PAN identifier of the entity to which the MSDU is
	 * being transferred.
	 */
	uint16_t DstPANId;

	/**
	 * The individual device address of the entity to which the MSDU is
	 * being transferred.
	 */
	uint64_t DstAddr;

	/**
	 * LQI value measured during reception of the MPDU. Lower values
	 * represent lower LQI (see 6.7.8).
	 */
	uint8_t mpduLinkQuality;

	/**
	 * The DSN of the received data frame.
	 */
	uint8_t DSN;
#ifdef ENABLE_TSTAMP

	/**
	 * Optional. The time, in symbols, at which the data were received
	 * (see 7.5.4.1).
	 * The symbol boundary is described by macSyncSymbolOffset (see Table 86
	 * in 7.4.1).
	 * This is a 24-bit value, and the precision of this value shall be a
	 * minimum of 20 bits, with the lowest 4 bits being the least
	 * significant.
	 */
	uint32_t Timestamp;
#endif  /* ENABLE_TSTAMP */
#if ((defined MAC_SECURITY_ZIP)  || (defined MAC_SECURITY_2006))

	/**
	 * The security level purportedly used by the received data frame.
	 */
	uint8_t SecurityLevel;

	/**
	 * The mode used to identify the key purportedly used by
	 * the originator of the received frame.
	 */
	uint8_t KeyIdMode;

	/**
	 * The index of the key purportedly used by the originator
	 * of the received frame.
	 */
	uint8_t KeyIndex;
#endif  /* ((defined MAC_SECURITY_ZIP)  || (defined MAC_SECURITY_2006)) */

	/**
	 * The number of octets contained in the MSDU being indicated by the
	 * MAC sublayer entity.
	 */
	uint8_t msduLength;

	/**
	 * The set of octets forming the MSDU being indicated by the
	 * MAC sublayer entity.
	 */
	uint8_t *msdu;
} mcps_data_ind_t;
__PACK__RST_DATA__
/*End packing for ARM(GCC & IAR) and AVR32(GCC) */
#if ((defined __ICCARM__) || (defined __GNUARM__) || (defined __GNUAVR32__))
/* #pragma pack() */
#endif /* __ICCARM__, __GNUARM__ , __GNUAVR32__*/
/* ! @} */
/* === Externals ============================================================ */

/* === Prototypes =========================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* MAC_MSG_TYPE_DATA_IND_H */
/* EOF */
