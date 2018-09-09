/**
 * @file usr_mlme_get_conf.c
 *
 * @brief This file contains user call back function for MLME-GET.confirm.
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
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
 */

/*
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

/* === Includes ============================================================= */

#include <stdint.h>
#include <stdbool.h>
#include "mac_api.h"

#if (MAC_GET_SUPPORT == 1)

/* === Macros ============================================================== */

/* === Globals ============================================================= */

/* === Prototypes ========================================================== */

/* === Implementation ====================================================== */
#if (defined __GNUC__)
void __attribute__((weak)) usr_mlme_get_conf(uint8_t status,
		uint8_t PIBAttribute,
#ifdef MAC_SECURITY_ZIP
		uint8_t PIBAttributeIndex,
#endif  /* MAC_SECURITY_ZIP */
		void *PIBAttributeValue)
#else
void usr_mlme_get_conf(uint8_t status,
		uint8_t PIBAttribute,
#ifdef MAC_SECURITY_ZIP
		uint8_t PIBAttributeIndex,
#endif  /* MAC_SECURITY_ZIP */
		void *PIBAttributeValue)
#endif
{
	/* Keep compiler happy. */
	status = status;
	PIBAttribute = PIBAttribute;
#ifdef MAC_SECURITY_ZIP
	PIBAttributeIndex = PIBAttributeIndex;
#endif  /* MAC_SECURITY_ZIP */
	PIBAttributeValue = PIBAttributeValue;
}

#endif  /* (MAC_GET_SUPPORT == 1) */

/* EOF */
