/**
 * \file
 *
 * \brief Service Specific Convergence Sublayer for IEC-432
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
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

#ifndef SSCS432_H_INCLUDE
#define SSCS432_H_INCLUDE

#include "compiler.h"

/* @cond 0 */
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/* @endcond */

/**
 * \ingroup prime_group
 * \defgroup sscs_prime_group PRIME Service Specific Convergence Sublayer for
 * IEC-432
 *
 * This module provides configuration and utils for the Convergence layer,
 * in particular the IEC 61334-4-32 Service Specific Convergence Sublayer.
 *
 * @{
 */

/* \name SSCS432 results */
/* @{ */
#define SSCS432_OK                  0
#define SSCS432_ERROR               (-1)
/* @} */

/* MAC address length */
#define MAC_LENGTH                  6

/* Address base node */
#define BASE_NODE_432_ADDRESS       0

/* Length serial number */
#define LEN_432_SERIAL_NUMBER       16

/* \name L_SAPs as defined in Iberdrola's Companion standard: */

/* \note  4 associations within 1 logical device (mngt logical device): */
/* \note - public client (client id 16) */
/* \note - management client (client id 1) */
/* \note - reading client (client id 2) */
/* \note - update firmware client (client id 3) */
/* @{ */
#define PUBLIC_CLIENT_ID            16
#define MANAGEMENT_CLIENT_ID        1
#define READING_CLIENT_ID           2
#define UPDATE_FIRMWARE_CLIENT_ID   3
/* Default L_SAPs. Use these values by default */
#define CL_DEFAULT_BASE_SAP         MANAGEMENT_CLIENT_ID
#define CL_DEFAULT_SERVICE_SAP      PUBLIC_CLIENT_ID
/* Use this to update firmware: */
#define CL_UPDATE_FIRMWARE_SSAP     UPDATE_FIRMWARE_CLIENT_ID
/* @} */

/* \name Convergence Layer Commands for the service node */
/* @{ */
#define CL_432_ESTABLISH_CONFIRM        1
#define CL_432_RELEASE_CONFIRM          2
#define CL_432_ESTABLISH_REQUEST        11
#define CL_432_RELEASE_REQUEST          12
/* @} */

/* \name Convergence Layer Commands for the base node */
/* @{ */
#define CL_432_JOIN_INDICATE            3
#define CL_432_LEAVE_INDICATE           4
/* @} */

/* \name Definitions to serialize 432 (USI) */
/* @{ */
#define PROTOCOL_432_PRIME              0x1A
/* Buffer432 size + header Size */
#define SSCS432_DATA_SIZE               1024 + 11
#define DATA_TYPE                       0
#define CTRL_TYPE                       1

#define CMD_432_PROTOCOL_MSK            0x1F
#define TYPE_432_PROTOCOL_MSK           0x20
#define TYPE_432_PROTOCOL_SHIFT         5

#define GET_TYPE_432_PROTOCOL(A)        (((A)&TYPE_432_PROTOCOL_MSK) >> TYPE_432_PROTOCOL_SHIFT)
#define SET_TYPE_432_PROTOCOL(A)        (((A) << TYPE_432_PROTOCOL_SHIFT) & TYPE_432_PROTOCOL_MSK)

#define CMD_432_PROTOCOL(A)             ((A)&CMD_432_PROTOCOL_MSK)
/* @} */

typedef struct {
	uint8_t cmd;
	uint16_t address;
	uint8_t lenSerialNumber;
	uint8_t serialNumber[LEN_432_SERIAL_NUMBER];
	uint8_t macAddress[MAC_LENGTH];
	uint8_t arq;
} sscs432_NotBase_t;

typedef struct {
	uint8_t cmd;
	uint16_t address;
	uint8_t lenSerialNumber;
	uint8_t serialNumber[LEN_432_SERIAL_NUMBER];
	uint16_t baseAddress;
	uint8_t arq;
} sscs432_NotService_t;

/* \name SSCS432 interface */
/* @{ */
void sscs432_init(void);
void sscs432_start(void);
void sscs432_process(void);
void sscs432_upd1ms(void);
Bool sscs432_poll_joins_leaves(sscs432_NotBase_t *ptrNot);
int8_t sscs432_establish_request(uint8_t *prSerialNumber, uint8_t lenSerialNumber);
int8_t sscs432_release_request(uint16_t destinationAddress);
Bool sscs432_poll_establish_release(sscs432_NotService_t *ptrNot);

/* @} */

/* @} */

/* / @cond 0 */
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/* / @endcond */

#endif
