/**
 * @file private_const.h
 *
 * @brief This header holds all Atmel private constants and attribute
 * identifiers
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
 */

/*
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

/* Prevent double inclusion */
#ifndef PRIVATE_CONST_H
#define PRIVATE_CONST_H

/* === Includes ============================================================= */

/* === Macros =============================================================== */

/*
 * Private test PIB attributes, will only be implemented (in the MAC
 * layer) if TEST_HARNESS > 0.
 */

/**
 * Private MAC PIB attribute to disable ACK sending.
 *
 * Value 0 implements normal ACK sending.  Value 255 disables ACK
 * sending completely.  Any other value will arrange for the
 * respective number of ACKs from being sent.
 */
#define macPrivateDisableACK           (0xF1)

/**
 * Private TAL PIB attribute to generate a CCA Channel Access Failure.
 *
 * Value 0 implements normal CCA behaviour.
 * Value 1 leads to CCA Channel Access Failure.
 * Any other value will also implement normal CCA behaviour.
 */
#define macPrivateTransactionOverflow   (0xF2)

/**
 * Private MAC PIB attribute to suppress the initiation of a Data Frame after
 * Association Request and expiration of aResponseWaitTime.
 *
 * Value 0   implements normal transmission of the Data Frame to the coordinator
 *           after aResponseWaitTime has expired.
 * Value > 0 does not suppress the transmission of the Data Frame to the
 *           coordinator after aResponseWaitTime has expired.
 */
#define macPrivateNoDataAfterAssocReq   (0xF3)

/**
 * Private MAC PIB attribute to generate a frame with an illegale frame type.
 *
 * Value 1 implements normal data frame transmission.
 * Value <> 1 does generate a data frame with the specified frame type.
 * Value 0 generates a Data frame with Frame type Beacon
 * Value 2 generates a Data frame with Frame type Ack
 * Value 3 generates a Data frame with Frame type MAC Command
 * Values 4 - 7 generate a Data frame with Frame type Reserved
 */
#define macPrivateIllegalFrameType      (0xF4)

/**
 * Private MAC PIB attribute to generate a CCA Channel Access Failure.
 *
 * Value 0 implements normal CCA behaviour.
 * Value 1 leads to CCA Channel Access Failure.
 * Any other value will also implement normal CCA behaviour.
 */
#define macPrivateCCAFailure            (0xF5)

/**
 * Private MAC PIB attribute to read internal MAC state.
 *
 * Read only.
 */
#define macPrivateMACState              (0xF6)

/**
 * Private MAC PIB attribute to pretend virtual Beacon-enabled PANs.
 *
 * Value 0 implements standard behaviour.
 * Value > 0 creates n different Beacon-enabled PANs by rotating the PAN-ID.
 */
#define macPrivateVirtualPANs           (0xF7)

/**
 * Private MAC PIB attribute to read internal MAC Sync state.
 *
 * Read only.
 */
#define macPrivateMACSyncState          (0xF8)

/* === Types ================================================================ */

/* === Externals ============================================================ */

/* === Prototypes =========================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* PRIVATE_CONST_H */
/* EOF */
