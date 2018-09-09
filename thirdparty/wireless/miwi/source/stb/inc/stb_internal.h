/**
* \file  stb_internal.h
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

/* Prevent double inclusion */
#ifndef STB_INTERNAL_H
#define STB_INTERNAL_H

#ifdef STB_ON_SAL

/* === Includes =========================================================== */

/* === Macros ============================================================= */

/* === Types ============================================================== */

/* === Externals ========================================================== */

/* === Prototypes ========================================================= */

#ifdef __cplusplus
extern "C" {
#endif

void compute_mic(uint8_t *buffer,
		uint8_t *mic,
		uint8_t *nonce,
		uint8_t hdr_len,
		uint8_t pld_len);

void encrypt_pldmic(uint8_t *buffer,
		uint8_t *nonce,
		uint8_t mic_len,
		uint8_t pld_len);

void encrypt_with_padding(uint8_t *start, uint8_t buflen);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* #ifdef STB_ON_SAL */

#endif /* STB_INTERNAL_H */
/* EOF */
