/**
 * @file mac_security.h
 *
 * @brief Declares MAC security related functions, globals, and macros.
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
#ifndef MAC_SECURITY_H
#define MAC_SECURITY_H

#if ((defined MAC_SECURITY_ZIP)  || (defined MAC_SECURITY_2006))

/* === Includes ============================================================= */

#include "mac_api.h"

/* === Macros =============================================================== */

/**
 * \addtogroup group_mac_def
 * @{
 */

/**
 * Default value for PIB macKeyTableEntries
 */
#define macKeyTableEntries_def              (0)

/**
 * Default value for PIB macDeviceTableEntries
 */
#define macDeviceTable_def                  (0)

/**
 * Default value for PIB macSecurityLevelTableEntries
 */
#define macSecurityLevelTable_def           (0)

/**
 * Default value for PIB macFrameCounter
 */
#define macFrameCounter_def                 (0x00000000)

/**
 * Default value for PIB macDefaultKeySource
 */
#define macDefaultKeySource_def             {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, \
					     0xFF, 0xFF}

/**
 * Default value for KeyIdLookupListEntries
 */
#define KeyIdLookupListEntries_def          (0)

/**
 * Default value for KeyDeviceListEntries
 */
#define KeyDeviceListEntries_def            (0)

/**
 * Default value for KeyUsageListEntries
 */
#define KeyUsageListEntries_def             (0)
/* ! @} */
/* === Externals ============================================================ */

extern mac_sec_pib_t mac_sec_pib;

/* === Prototypes =========================================================== */
/* Finding the Key Identifier Length field */
uint8_t get_key_id_field_len(uint8_t key_id_mode);

bool build_sec_mcps_data_frame(mcps_data_req_t *mpdr, frame_info_t *mframe);

#ifdef __cplusplus
extern "C" {
#endif
/*@{*/

/*@}*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /* (MAC_SECURITY_ZIP || MAC_SECURITY_2006) */

#endif /* MAC_SECURITY_H */
/* EOF */
