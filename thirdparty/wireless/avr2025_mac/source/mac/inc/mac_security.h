/**
 * @file mac_security.h
 *
 * @brief Declares MAC security related functions, globals, and macros.
 *
 * Copyright (c) 2013-2018 Microchip Technology Inc. and its subsidiaries.
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
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
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
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
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
