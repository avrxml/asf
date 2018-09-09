/**
 * \file sysEncrypt.c
 *
 * \brief System encryption routines implementation
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
 *
 *
 */

/*
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

/*- Includes ---------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include "sysEncrypt.h"
#include "sysConfig.h"
#include "phy.h"

#ifdef NWK_ENABLE_SECURITY

/*- Implementations --------------------------------------------------------*/

#if SYS_SECURITY_MODE == 1

/*************************************************************************//**
*****************************************************************************/
static void xtea(uint32_t text[2], uint32_t const key[4])
{
	uint32_t t0 = text[0];
	uint32_t t1 = text[1];
	uint32_t sum = 0;
	uint32_t delta = 0x9e3779b9;

	for (uint8_t i = 0; i < 32; i++) {
		t0 += (((t1 << 4) ^ (t1 >> 5)) + t1) ^ (sum + key[sum & 3]);
		sum += delta;
		t1
			+= (((t0 <<
				4) ^
				(t0 >> 5)) + t0) ^ (sum + key[(sum >> 11) & 3]);
	}
	text[0] = t0;
	text[1] = t1;
}

#endif

/*************************************************************************//**
*****************************************************************************/
void SYS_EncryptReq(uint32_t *text, uint32_t *key)
{
#if SYS_SECURITY_MODE == 0
	PHY_EncryptReq((uint8_t *)text, (uint8_t *)key);

#elif SYS_SECURITY_MODE == 1
	xtea(&text[0], key);
	text[2] ^= text[0];
	text[3] ^= text[1];
	xtea(&text[2], key);
#endif

	SYS_EncryptConf();
}

#endif /* NWK_ENABLE_SECURITY */
