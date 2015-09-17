/**
 * @file mac_data_extract_mhr.c
 *
 * @brief Implements common functionality for MAC
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
#include "mac_internal.h"

/* === Macros =============================================================== */

/* === Globals ============================================================= */

/* === Prototypes ========================================================== */

/* === Implementation ====================================================== */

/*
 * @brief Helper function to extract the complete address information
 *        of the received frame
 *
 * @param frame_ptr Pointer to first octet of Addressing fields of received
 * frame
 *        (See IEEE 802.15.4-2006 Figure 41)
 *
 * @return bool Length of Addressing fields
 */
uint8_t mac_extract_mhr_addr_info(uint8_t *frame_ptr)
{
	uint16_t fcf = mac_parse_data.fcf;
	uint8_t src_addr_mode = (fcf >> FCF_SOURCE_ADDR_OFFSET) & FCF_ADDR_MASK;
	uint8_t dst_addr_mode = (fcf >> FCF_DEST_ADDR_OFFSET) & FCF_ADDR_MASK;
	bool intra_pan = fcf & FCF_PAN_ID_COMPRESSION;
	uint8_t addr_field_len = 0;

	if (dst_addr_mode != 0) {
		mac_parse_data.dest_panid = convert_byte_array_to_16_bit(
				frame_ptr);
		frame_ptr += PAN_ID_LEN;
		addr_field_len += PAN_ID_LEN;

		if (FCF_SHORT_ADDR == dst_addr_mode) {
			/*
			 * First initialize the complete long address with zero,
			 * since
			 * later only 16 bit are actually written.
			 */
			mac_parse_data.dest_addr.long_address = 0;
			mac_parse_data.dest_addr.short_address
				= convert_byte_array_to_16_bit(frame_ptr);
			frame_ptr += SHORT_ADDR_LEN;
			addr_field_len += SHORT_ADDR_LEN;
		} else if (FCF_LONG_ADDR == dst_addr_mode) {
			mac_parse_data.dest_addr.long_address
				= convert_byte_array_to_64_bit(frame_ptr);
			frame_ptr += EXT_ADDR_LEN;
			addr_field_len += EXT_ADDR_LEN;
		}
	}

	if (src_addr_mode != 0) {
		if (!intra_pan) {
			/*
			 * Source PAN ID is present in the frame only if the
			 * intra-PAN bit
			 * is zero and src_addr_mode is non zero.
			 */
			mac_parse_data.src_panid = convert_byte_array_to_16_bit(
					frame_ptr);
			frame_ptr += PAN_ID_LEN;
			addr_field_len += PAN_ID_LEN;
		} else {
			/*
			 * The received frame does not contain a source PAN ID,
			 * hence
			 * source PAN ID is updated with the destination PAN ID.
			 */
			mac_parse_data.src_panid = mac_parse_data.dest_panid;
		}

		/* The source address is updated. */
		if (FCF_SHORT_ADDR == src_addr_mode) {
			/*
			 * First initialize the complete long address with zero,
			 * since
			 * later only 16 bit are actually written.
			 */
			mac_parse_data.src_addr.long_address = 0;
			mac_parse_data.src_addr.short_address
				= convert_byte_array_to_16_bit(frame_ptr);
			frame_ptr += SHORT_ADDR_LEN;

			addr_field_len += SHORT_ADDR_LEN;
		} else if (FCF_LONG_ADDR == src_addr_mode) {
			mac_parse_data.src_addr.long_address
				= convert_byte_array_to_64_bit(frame_ptr);
			frame_ptr += EXT_ADDR_LEN;
			addr_field_len += EXT_ADDR_LEN;
		}
	}

	/*
	 * The length of the Addressing Field is known, so the length of the
	 * MAC payload can be calcluated.
	 * The actual MAC payload length is calculated from
	 * the length of the mpdu minus 2 octets FCS, minus 1 octet sequence
	 * number, minus the length of the addressing fields, minus 2 octet FCS.
	 */
	mac_parse_data.mac_payload_length = mac_parse_data.mpdu_length -
			FCF_LEN -
			SEQ_NUM_LEN -
			addr_field_len -
			FCS_LEN;

	mac_parse_data.src_addr_mode = src_addr_mode;
	mac_parse_data.dest_addr_mode = dst_addr_mode;

	return (addr_field_len);
}

/* EOF */
