/**
 * @file stb.h
 *
 * @brief Declarations for security tool box
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
 * Copyright (c) 2013-2015 Atmel Corporation. All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

/* Prevent double inclusion */
#ifndef STB_H
#define STB_H

#if defined(STB_ON_SAL) || defined(STB_ARMCRYPTO) || defined(__DOXYGEN__)

/* === Includes =========================================================== */

#if defined(STB_ON_SAL)
#include "sal.h"
#elif defined (STB_ARMCRYPTO)
#include "stb_armcrypto.h"
#else
#error "Unknown STB type"
#endif
#include "stb_generic.h"
#include "stb_internal.h"

/* === Macros ============================================================= */

/**
 * \defgroup group_stb  Security toolbox
 * The STB (Security Toolbox) is a high level security abstraction layer
 * providing an easy-
 * to-use  crypto  API for direct application access.It is placed on top of the
 * SAL and abstracts and implements
 * transceiver or MCU dependent functionality that encrypts or decrypts frames
 * usimg CCM according to 802.15.4/Zigbee.
 */

/**
 * \ingroup group_stb
 * \defgroup group_sal
 * SAL Security abstraction layer
 * The SAL (Security Abstraction Layer) provides an API that allows access to
 * low level
 * AES engine functions abstraction to encrypt and  decrypt frames.
 *
 */

/*
 * The following defines are taken from the ZigBee specification and can be
 * to implement ZigBee security.
 */

/**
 * Length of ZigBee Frame Counter in octets.
 */
#define FRM_COUNTER_LEN                 (4)

/**
 * Position of Security Control octet in Auxiliary Security Header.
 */
#define MSDU_POS_SEC_CTRL               (1)

/**
 * Position of Frame Counter octet in Auxiliary Security Header.
 */
#define MSDU_POS_FRM_CNTR               (2)

/**
 * Position of first Source Addess octet in Auxiliary Security Header.
 */
#define MSDU_POS_SRC_ADDR               (6)

/**
 * Position of Key Sequence Number octet in Auxiliary Security Header.
 */
#define MSDU_POS_KEY_SEQ_NO             (14)

/**
 * Position of Security Control octet in Nonce.
 */
#define NONCE_POS_SEC_CTRL              (13)

/*
 * Definition of Security Control Field:
 *
 * Bit 0-2: Security Level = 6 (MIC length 8 bytes, encryption)
 * Bit 3-4: Key identifier - A Network key
 * Bit 5:   Extended Nonce enabled
 * Bit 6-7: Reserved
 */

/**
 * Position of Security Level bits in Security Control Field.
 */
#define SEC_CTRL_POS_SEC_LEVEL          (0)

/**
 * Position of Key identifier bits in Security Control Field.
 */
#define SEC_CTRL_POS_KEY_ID             (3)

/**
 * Position ofExtended Nonce enabled bit in Security Control Field.
 */
#define SEC_CTRL_POS_EXT_NONCE          (5)

/*
 * Definition of Key Identifier Sub-field of Security Control Field:
 *
 * 0x00: A Data key
 * 0x01: A Network key
 * 0x01: A Key-transport key
 * 0x01: A Key-load key
 */

/**
 * Key Identifier Sub-field of Security Control Field indicates Data key.
 */
#define KEY_ID_DATA_KEY                 (0x00)

/**
 * Key Identifier Sub-field of Security Control Field indicates Network key.
 */
#define KEY_ID_NWK_KEY                  (0x01)

/**
 * Key Identifier Sub-field of Security Control Field indicates Key-transport
 * key.
 */
#define KEY_ID_KEY_TRANSPORT_KEY        (0x02)

/**
 * Key Identifier Sub-field of Security Control Field indicates Key-load key.
 */
#define KEY_ID_KEY_LOAD_KEY             (0x03)

/**
 * Maximum length of ZigBee Auxiliary Frame Header.
 */

/*
 * Format:
 * Securiy Control: 0/1 octets
 * Frame Counter: 4 octets
 * Source Address 0/8 octets
 * Key Sequence Number: 0/1 octet
 */
#define AUX_HDR_LEN                     (MSDU_POS_KEY_SEQ_NO)

/* Set radio to SLEEP, if it has been in SLEEP before sal_aes_restart()
**/
#define TRX_SLEEP() \
	{ \
		if (prev_trx_status == TRX_SLEEP) { \
			tal_trx_sleep(SLEEP_MODE_1); \
		} \
	} \
        /* === Types
         *============================================================== */

/* === Externals ========================================================== */

/* === Prototypes ========================================================= */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief STB Restart
 *
 * This function re-starts the STB after power down.
 *
 * @ingroup group_StbApi
 */
void stb_restart(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* #ifdef STB_ON_SAL */

#endif /* STB_H */
/* EOF */
