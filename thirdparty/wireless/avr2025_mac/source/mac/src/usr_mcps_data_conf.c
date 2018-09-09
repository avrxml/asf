/**
 * @file usr_mcps_data_conf.c
 *
 * @brief This file contains user call back function for MCPS-DATA.confirm.
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

/* === Macros ============================================================== */

/* === Globals ============================================================= */

/* === Prototypes ========================================================== */

/* === Implementation ====================================================== */
#if (defined __GNUC__)
void __attribute__((weak))
#ifdef ENABLE_TSTAMP
usr_mcps_data_conf(uint8_t msduHandle, uint8_t status, uint32_t Timestamp)
#else
usr_mcps_data_conf(uint8_t msduHandle, uint8_t status)
#endif  /* ENABLE_TSTAMP */
#else
#ifdef ENABLE_TSTAMP
void usr_mcps_data_conf(uint8_t msduHandle, uint8_t status, uint32_t Timestamp)
#else
void usr_mcps_data_conf(uint8_t msduHandle, uint8_t status)
#endif  /* ENABLE_TSTAMP */
#endif
{
	/* Keep compiler happy. */
	msduHandle = msduHandle;
	status = status;
#ifdef ENABLE_TSTAMP
	Timestamp = Timestamp;
#endif  /* ENABLE_TSTAMP */
}

/* EOF */
