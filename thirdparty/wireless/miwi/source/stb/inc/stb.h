/**
* \file  stb.h
*
* \brief Declarations for security tool box
*
* Copyright (c) 2018 Microchip Technology Inc. and its subsidiaries. 
*
* \asf_license_start
*
* \page License
*
* Subject to your compliance with these terms, you may use Microchip
* software and any derivatives exclusively with Microchip products. 
* It is your responsibility to comply with third party license terms applicable 
* to your use of third party software (including open source software) that 
* may accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, 
* WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, 
* INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, 
* AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE 
* LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL 
* LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE 
* SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE 
* POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT 
* ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY 
* RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY, 
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*
* \asf_license_stop
*
*/
/*
* Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
#include "phy.h"

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
			PHY_Sleep(); \
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
