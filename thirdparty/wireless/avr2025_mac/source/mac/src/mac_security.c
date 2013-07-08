/**
 * @file mac_security.c
 *
 * @brief Handles MAC security
 *
 * This file handles MAC security according to IEEE 802.15.4-2006.
 *
 * Copyright (c) 2013 Atmel Corporation. All rights reserved.
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
 * Copyright (c) 2013, Atmel Corporation All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

/* === Includes ============================================================ */

#if (defined MAC_SECURITY_ZIP) || (defined MAC_SECURITY_2006)
#include <compiler.h>
#include <string.h>
#include "tal.h"
#include "ieee_const.h"
#include "stack_config.h"
#include "mac_internal.h"
#include "stb.h"
#include "mac_security.h"

/* === Macros =============================================================== */

/* Security Control Field: Security Level mask */
#define SEC_CTRL_SEC_LVL_MASK           (0x07)

/* Security Control Field: Key Identifier mask */
#define SEC_CTRL_KEY_ID_MASK            (0x03)

/* Security Control Field: Key Identifier Field position */
#define SEC_CTRL_KEY_ID_FIELD_POS       (3)

/* === Globals ============================================================= */

/* === Prototypes ========================================================== */

static uint8_t get_key_id_field_len(uint8_t key_id_mode);

static uint8_t get_mic_length(uint8_t security_level);

static inline uint8_t sec_additional_len(mcps_data_req_t *pmdr);

static inline retval_t parse_aux_sec_header(parse_t *mac_parse_data,
		uint8_t *mac_payload);

static inline retval_t unsecure_frame(parse_t *mac_parse_data, uint8_t *mpdu,
		uint8_t *mac_payload,
		uint8_t *payload_index);

static inline retval_t outgoing_key_retrieval(mcps_data_req_t *pmdr,
		uint8_t **key);

static inline retval_t incoming_sec_material_retrieval(parse_t *mac_parse_data,
		uint8_t **key,
		uint8_t **addr);

static void create_nonce(uint8_t *ieee_addr, uint8_t *frame_cnt,
		uint8_t security_level, uint8_t *nonce);

/* === Implementation ====================================================== */

/* --- Helper Functions ---------------------------------------------------- */

static inline uint8_t sec_additional_len(mcps_data_req_t *pmdr)
{
	uint8_t len;

	/* Auxiliary security header length */
	len  = 1 /* security ctrl field */
			+ 4 /* frame counter length */
			+ get_key_id_field_len(pmdr->KeyIdMode);

	/* MIC length */
	len += get_mic_length(pmdr->SecurityLevel);

	return len;
}

/*
 * @brief Gets the length of the Key Identifier field
 *
 * This function returns the length of the Key Identifier field
 * within the Auxiliary Security Header of a secured frame based
 * on the Key Identifier Mode.
 *
 * @param key_id_mode Key Identifier Mode
 *
 * @return Length of Key Identifier field in octets.
 * @ToDo Check if lookup table implementation needs less footprint
 */
static uint8_t get_key_id_field_len(uint8_t key_id_mode)
{
	uint8_t len_field = 0;

	switch (key_id_mode) {
	case 1:
		len_field = 1; break;

	case 2:
		len_field = 5; break;

	case 3:
		len_field = 9; break;

	case 0:
	default:
		break;
	}

	return (len_field);
}

/**
 * @brief Calculates the length of the MIC
 *
 * This function returns the length of the MIC depending on the given security
 *level
 *
 * @param security_level Security Level of current frame
 *
 * @return Length of MIC in octets.
 * @ToDo Check if lookup table implementation needs less footprint
 */
static uint8_t get_mic_length(uint8_t security_level)
{
	uint8_t mic_len = 0;

	switch (security_level) {
	case 1:
	case 5:
		mic_len = 4; break;

	case 2:
	case 6:
		mic_len = 8; break;

	case 3:
	case 7:
		mic_len = 16; break;

	case 0:
	case 4:
	default:
		break;
	}

	return mic_len;
}

static void create_nonce(uint8_t *ieee_addr, uint8_t *frame_cnt,
		uint8_t security_level, uint8_t *nonce)
{
	/*
	 * Create Nonce - Attentation: byte order is inverse in comparison to
	 *RF4CE
	 * RF4CE: Little endian
	 */
	uint8_t *ptr;
	uint8_t *nonce_ptr = &nonce[1];

	ptr = ieee_addr;
	ptr += 7;
	for (uint8_t i = 0; i < 8; i++) {
		*nonce_ptr++ = *ptr--;
	}
	ptr = frame_cnt;
	ptr += 3;
	for (uint8_t i = 0; i < 4; i++) {
		*nonce_ptr++ = *ptr--;
	}
	*nonce_ptr = security_level; /* Only security level NOT security control
	                              * field */
}

/* --- Outgoing Security --------------------------------------------------- */

/*
 * @brief Generates Auxiliary Security Header fields
 *
 * This function generates the required fields of the
 * Auxiliary Security Header of a secured frame based
 * on the actual security parameter.
 *
 * @param tx_frame Frame information structure of current frame
 * @param security_level Security Level of current frame
 * @param key_id_mode Key Identifier Mode of current frame
 * @param key_source Key Source of current frame
 * @param key_index Key Index of current frame
 *
 * @return Status of extraction of Auxiliary Security Header fields
 */
retval_t mac_build_aux_sec_header(uint8_t **frame_ptr, mcps_data_req_t *pmdr,
		uint8_t *frame_len)
{
	uint8_t sec_info_len = sec_additional_len(pmdr);
	*frame_ptr = *frame_ptr - sec_info_len;
	/* Add security field length to original frame length */
	*frame_len += sec_info_len;

	/* Create Auxiliary security header. */
	uint8_t *sec_msdu_ptr = *frame_ptr;
	/* Fill in Security Control Field. */
	*sec_msdu_ptr = (pmdr->SecurityLevel & SEC_CTRL_SEC_LVL_MASK) |
			((pmdr->KeyIdMode &
			SEC_CTRL_KEY_ID_MASK) << SEC_CTRL_KEY_ID_FIELD_POS);
	sec_msdu_ptr++;

	/* Fill in Frame Counter. */
	if (mac_sec_pib.FrameCounter == 0xFFFFFFFF) {
		return MAC_COUNTER_ERROR;
	} else {
		memcpy(sec_msdu_ptr, &mac_sec_pib.FrameCounter, 4);
		sec_msdu_ptr += 4;
	}

	/*
	 * Key Source and Key Index subfields are only present
	 * if KeyIdMode is not zero.
	 */
	if (pmdr->KeyIdMode == 1) {
		/* TODO */
		/* See 802.15.4-2006 section 7.5.8.2.1 h) 4) */
		/* Fix for ZIP */
		*sec_msdu_ptr = pmdr->KeyIndex;
	} else if (pmdr->KeyIdMode > 1) {
		return MAC_UNSUPPORTED_SECURITY;
	}

	return MAC_SUCCESS;
}

/*
 * @brief Secures MAC frame
 *
 * This function secures the given MAC frame.
 *
 * @param tx_frame Frame information structure of current frame
 * @param security_level Security Level of current frame
 * @param key_id_mode Key Identifier Mode
 *
 * @return retval_t MAC_SUCCESS or MAC_UNSUPPORTED_SECURITY
 */
retval_t mac_secure(frame_info_t *frame, uint8_t *mac_payload_ptr,
		mcps_data_req_t *pmdr)
{
	uint8_t *key;

	/* Retrieval of the key */
	retval_t key_status = outgoing_key_retrieval(pmdr, &key);
	if (key_status != MAC_SUCCESS) {
		return key_status;
	}

	uint8_t m = get_mic_length(pmdr->SecurityLevel);

	/* Encrypt payload data */
	uint8_t nonce[AES_BLOCKSIZE];
	uint8_t enc_data[aMaxPHYPacketSize - FCS_LEN + AES_BLOCKSIZE];
	/* Extra AES Bloc size at end required for MIC generation. */

	create_nonce((uint8_t *)&tal_pib.IeeeAddress,
			(uint8_t *)&mac_sec_pib.FrameCounter,
			pmdr->SecurityLevel,
			nonce);

	/*
	 * Create string a
	 * Here: MHR | Aux sec header = MHR | SecLvl | FrameCounter
	 * = FCF (2) | SeqNo (1) | AddrFields
	 */
	uint8_t hdr_len = mac_payload_ptr - &frame->mpdu[1] - m;
	memcpy(enc_data, &frame->mpdu[1], hdr_len);

	switch (frame->mpdu[1] & 0x03) { /* FCF */
	case FCF_FRAMETYPE_DATA:                  /* (0x01) */
		/* Append payload (string m) */
		memcpy(&enc_data[hdr_len], mac_payload_ptr, pmdr->msduLength);
		{
			uint8_t *current_key;

			/* Shall the real key be used or rather a test key? */
			current_key = key;

			stb_ccm_secure(enc_data, /* plaintext header (string a)
			                          *and payload concatenated */
					nonce,
					current_key, /*security_key */
					hdr_len, /* plaintext header length */
					pmdr->msduLength, /* Length of payload
			                                   *to be encrypted */
					pmdr->SecurityLevel, /* security level
			                                      **/
					AES_DIR_ENCRYPT);
		}

		mac_sec_pib.FrameCounter++;

		/* Replace original payload by secured payload */
		memcpy(mac_payload_ptr - m, &enc_data[hdr_len],
				pmdr->msduLength + m);
		break;

	default:
		return MAC_UNSUPPORTED_SECURITY;
	}

	return MAC_SUCCESS;
}

/* 7.5.8.2.2 */
static inline retval_t outgoing_key_retrieval(mcps_data_req_t *pmdr,
		uint8_t **key)
{
	uint8_t key_lookup_data_size;
	uint8_t lookup_data[9];

	if (pmdr->KeyIdMode == 0x00) { /* implicit key identification */
		switch (pmdr->DstAddrMode) {
		/* 1) is not applicabled for ZIP */
		/* 2) is not applicabled for ZIP */

		case FCF_SHORT_ADDR:
			/* lookup_data: Dest PANId right-concatenated */
			lookup_data[1] = (uint8_t)pmdr->DstPANId; /* LSB */
			lookup_data[0] = (uint8_t)(pmdr->DstPANId >> 8); /* MSB */
			/* 2-octet dest addr right-concatenated */
			lookup_data[3] = (uint8_t)pmdr->DstAddr; /* LSB */
			lookup_data[2] = (uint8_t)(pmdr->DstAddr >> 8); /* MSB */
			lookup_data[4] = 0x00; /* single octet 0x00 */
			key_lookup_data_size = 0; /* key lookup size = 5 -> key
			                           * lookup data size = 0 */
			break;

		case FCF_LONG_ADDR:
			/* lookup_data: 8-octet dest addr right-concatenated */
			memcpy(&lookup_data[0], &pmdr->DstAddr, 8);
			lookup_data[8] = 0x00; /* single octet 0x00 */
			key_lookup_data_size = 1; /* key lookup size = 9 -> key
			                           * lookup data size = 1 */
			break;

		default:
			return MAC_UNSUPPORTED_SECURITY; /* applies for ZIP only */
		}
	} else if (pmdr->KeyIdMode == 0x01) { /* explicit key identification */
		/*
		 * lookup_data:
		 * 8-octet of macDefaultKeySource right-concatenated with
		 * single octet: Key Index parameter = tx_frame->key_id[0]
		 */
		for (uint8_t i = 0; i < 8; i++) {
			lookup_data[i] = mac_sec_pib.DefaultKeySource[i];
		}
		lookup_data[8] = pmdr->KeyIndex; /* tx_frame->key_id[0]; */
		key_lookup_data_size = 1; /* key lookup size = 9 -> key lookup
		                           * data size = 1 */
	}

#ifdef MAC_SECURITY_2006
	else if (key_id_mode == 0x02) {
		/*
		 * lookup_data: 4-octet of KeySource with is
		 *tx_frame->key_id[0...3]
		 * right-concatenated with
		 * single octet: Key Index parameter = tx_frame->key_id[4]
		 */
		for (uint8_t i = 0; i < 4; i++) {
			lookup_data[i] = tx_frame->key_id[i - 1];
		}
		lookup_data[5] = tx_frame->key_id[4];
		key_lookup_data_size = 0; /* key lookup size = 5 -> key lookup
		                           * data size = 0 */
	}
#endif  /* MAC_SECURITY_2006 */
	else {
		/* other than 0x00 and 0x01 are NOT used for ZIP */
		return MAC_UNSUPPORTED_SECURITY; /* applies for ZIP only */
	}

	/* Get key from KeyDescriptor as 7.5.8.2.5 */
	for (uint8_t i = 0; i < mac_sec_pib.KeyTableEntries; i++) {
		for (uint8_t k =
				0;
				k <
				mac_sec_pib.KeyTable[i].KeyIdLookupListEntries;
				k++) {
			if (mac_sec_pib.KeyTable[i].KeyIdLookupList[k].
					LookupDataSize ==
					key_lookup_data_size) {
				int16_t compare;
				uint8_t len;

				switch (key_lookup_data_size) {
				case 0:
					len = 5; break;

				case 1:
					len = 9; break;

				default:
					return MAC_UNSUPPORTED_SECURITY;
				}
				compare = memcmp(
						mac_sec_pib.KeyTable[i].KeyIdLookupList[
							k].LookupData,
						lookup_data,
						len);
				if (compare == 0x00) {
					*key = mac_sec_pib.KeyTable[i].Key;
					return MAC_SUCCESS;
				}
			}
		}
	}

	return MAC_UNAVAILABLE_KEY;
}

/* --- Incoming Security --------------------------------------------------- */

/**
 * @brief Handles unsecuring of MAC frame
 *
 * This function handles the complete unsecuring of a MAC frame.
 * This includes the extraction of the Auxiliary Security Header and
 * the actual frame decryption.
 *
 * @param[in] tx_frame Frame information structure of current frame
 * @param[in] security_level Security Level of current frame
 * @param[in] key_id_mode Key Identifier Mode of current frame
 * @param[in] key_source Key Source of current frame
 * @param[in] key_index Key Index of current frame
 * @param[out] cur_data_frame_len Current length of frame before applying
 *security
 *
 * @return Status of extraction of Auxiliary Security Header fields
 */
retval_t mac_unsecure(parse_t *mac_parse_data, uint8_t *mpdu,
		uint8_t *mac_payload, uint8_t *payload_index)
{
	retval_t status;
	status = parse_aux_sec_header(mac_parse_data, mac_payload);
	if (status != MAC_SUCCESS) {
		return status;
	}

	status
		= unsecure_frame(mac_parse_data, mpdu, mac_payload,
			payload_index);
	return status;
}

/*
 * @brief Generates Auxiliary Security Header fields
 *
 * This function extracts the actual security parameters
 * from the Auxiliary Security Header of a received secured frame.
 *
 * @param[in] mac_parse_data Parsed information of the received frame
 * @param[out] security_level Extracted Security Level of received frame
 * @param[out] key_id_mode Extracted Key Identifier Mode of received frame
 * @param[out] key_source Extracted Key Source of received frame
 * @param[out] key_index Extracted Key Index of received frame
 *
 * @return Status of generation of Auxiliary Security Header fields
 */
static inline retval_t parse_aux_sec_header(parse_t *mac_parse_data,
		uint8_t *mac_payload)
{
	memcpy(&mac_parse_data->sec_ctrl, &mac_payload[0], 1);
	memcpy(&mac_parse_data->frame_cnt, &mac_payload[1], 4);
	mac_parse_data->key_id_len = get_key_id_field_len(
			mac_parse_data->sec_ctrl.key_id_mode);

	if (mac_parse_data->sec_ctrl.key_id_mode != 0) {
		if (mac_parse_data->key_id_len > 1) {
			/* Key Id Len must not be larger than 1 for ZIP.
			 * @Todo: Handle other modes */
			return MAC_UNSUPPORTED_SECURITY;
		}

		mac_parse_data->key_id[0] = mac_payload[5]; /* =
		                                             * mac_parse_data->key_id_len
		                                             * + 4;    // 4 =
		                                             * frame counter len */
	}

	/* See 802.15.4-2006 section 7.5.8.2.3 b) */
	if ((mac_parse_data->fcf & FCF_SECURITY_ENABLED) &&
			(!(mac_parse_data->fcf & FCF_FRAME_VERSION_2006))
			) {
		return MAC_UNSUPPORTED_LEGACY;
	}

	return MAC_SUCCESS;
}

/**
 * @brief Unsecures MAC frame
 *
 * This function unsecures the given MAC frame.
 *
 * @param mac_parse_data Frame information structure of current frame
 * @param security_level Security Level of current frame
 *
 * @return retval_t MAC_SUCCESS, MAC_UNSUPPORTED_SECURITY or MAC_SECURITY_ERROR
 */
static inline retval_t unsecure_frame(parse_t *mac_parse_data, uint8_t *mpdu,
		uint8_t *mac_payload,
		uint8_t *payload_index)
{
	/* Encrypt payload data */
	uint8_t nonce[AES_BLOCKSIZE]; /* AES_BLOCKSIZE 16 */
	uint8_t *src_ieee_addr;

	/* Incoming key retrieval */
	uint8_t *key;

	retval_t status = incoming_sec_material_retrieval(mac_parse_data, &key,
			&src_ieee_addr);
	if (status != MAC_SUCCESS) {
		return status;
	}

	/*
	 * Create Nonce - Attentation: byte order is inverse in comparison to
	 *RF4CE
	 * RF4CE: Little endian
	 */
	uint8_t *addr_ptr;
	uint8_t *current_key; /* Pointer to actually used key */

	/* Standard way of extracting Src IEEE address. */
	addr_ptr = src_ieee_addr;

	uint8_t m = get_mic_length(mac_parse_data->sec_ctrl.sec_level);

	create_nonce(addr_ptr, (uint8_t *)&mac_parse_data->frame_cnt,
			mac_parse_data->sec_ctrl.sec_level, nonce);

	switch (mac_parse_data->frame_type) {
	case FCF_FRAMETYPE_DATA:                  /* (0x01) */
	{
		/* Shall the real key be used or rather a test key? */
		current_key = key;

		uint8_t sec_hdr_len = 5 + mac_parse_data->key_id_len; /* 5 = sec
			                                               * ctrl +
			                                               * frame
			                                               * counter */
		uint8_t mhr_len = mac_payload - mpdu + sec_hdr_len;
		uint8_t encryp_payload_len = mac_parse_data->mpdu_length -
				mhr_len - m - 2;                                         /*
			                                                                  * 2
			                                                                  * =
			                                                                  * CRC */

		if (stb_ccm_secure(mpdu, /* plaintext header (string a) and
			                  *payload concatenated */
				nonce,
				current_key, /* security_key */
				mhr_len, /* plaintext header length */
				encryp_payload_len, /* Length of payload to be
			                             *encrypted */
				mac_parse_data->sec_ctrl.sec_level, /* security
			                                             *level */
				AES_DIR_DECRYPT)
				== STB_CCM_OK) {
			/* Adjust payload by secured payload */
			*payload_index = sec_hdr_len;
			/* Substract the MIC length from the payload length. */
			mac_parse_data->mac_payload_length = encryp_payload_len;
			return MAC_SUCCESS;
		} else {
			return MAC_SECURITY_ERROR;
		}

		/* break; not reachabled */
	}

	default:
		return MAC_UNSUPPORTED_SECURITY;
	}
}

static inline retval_t incoming_sec_material_retrieval(parse_t *mac_parse_data,
		uint8_t **key,
		uint8_t **addr)
{
	/* @ToDo: Check a holy bunch of other stuff, see 7.5.8.2.3*/

	uint8_t lookup_data_size;
	uint8_t lookup_data[9];

	if (mac_parse_data->sec_ctrl.key_id_mode == 0x00) { /* implicit */
		switch (mac_parse_data->src_addr_mode) {
		/* 1) is not applicabled for ZIP */
		/* 2) is not applicabled for ZIP */

		case FCF_SHORT_ADDR:
			/* PAN Id: TAL handles PAN ID compression issue */
			lookup_data[1] = (uint8_t)mac_parse_data->src_panid; /*
			                                                      * LSB */
			lookup_data[0]
				= (uint8_t)(mac_parse_data->src_panid >>
					8);                                 /*
			                                                     * MSB */
			/* 2-octet dest addr right-concatenated */
			lookup_data[3]
				= (uint8_t)mac_parse_data->src_addr.
					short_address;                            /*
			                                                           * LSB */
			lookup_data[2]
				= (uint8_t)(mac_parse_data->src_addr.
					short_address
					>> 8);                                           /*
			                                                                  * MSB */
			lookup_data[4] = 0x00; /* single octet 0x00 */
			lookup_data_size = 0; /* key lookup size = 5 -> key
			                       * lookup data size = 0 */
			break;

		case FCF_LONG_ADDR:
			/* lookup_data: 8-octet dest addr right-concatenated */
			memcpy(&lookup_data[0],
					&mac_parse_data->src_addr.long_address,
					8);
			lookup_data[8] = 0x00; /* single octet 0x00 */
			lookup_data_size = 1; /* key lookup size = 9 -> key
			                       * lookup data size = 1 */
			break;

		default:
			return MAC_UNSUPPORTED_SECURITY;
		}
	} else if (mac_parse_data->sec_ctrl.key_id_mode == 0x01) {
		/* lookup_data: 8-octet of macDefaultKeySource
		 * right-concatenated */
		/* single octet: Key Index parameter */
		for (uint8_t i = 0; i < 8; i++) {
			lookup_data[i] = mac_sec_pib.DefaultKeySource[i];
		}
		lookup_data[8] = mac_parse_data->key_id[0];
		lookup_data_size = 1; /* key lookup size = 9 -> key lookup data
		                       * size = 1 */
	} else {
		return MAC_UNSUPPORTED_SECURITY; /* not used by ZIP */
	}

	/* Get key from KeyDescriptor as 7.5.8.2.5 */
	for (uint8_t i = 0; i < mac_sec_pib.KeyTableEntries; i++) {
		for (uint8_t k =
				0;
				k <
				mac_sec_pib.KeyTable[i].KeyIdLookupListEntries;
				k++) {
			if (mac_sec_pib.KeyTable[i].KeyIdLookupList[k].
					LookupDataSize == lookup_data_size) {
				int16_t compare;
				uint8_t len;

				switch (lookup_data_size) {
				case 0:
					len = 5; break;

				case 1:
					len = 9; break;

				default:
					return MAC_UNSUPPORTED_SECURITY;
				}
				compare = memcmp(
						mac_sec_pib.KeyTable[i].KeyIdLookupList[
							k].LookupData,
						lookup_data,
						len);
				if (compare == 0x00) {
					*key = mac_sec_pib.KeyTable[i].Key;
					*addr
						= (uint8_t *)&mac_sec_pib.
							DeviceTable
							[i].DeviceDescriptor[i].
							ExtAddress;
					return MAC_SUCCESS;
				}
			}
		}
	}
	/* No key has been found. * / */
	return MAC_UNAVAILABLE_KEY;
}

#endif  /* (defined MAC_SECURITY_ZIP) || (defined MAC_SECURITY_2006) */

/* EOF */
