/**
 * \file *********************************************************************
 *
 * \brief Memory map for QMatrix file
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
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
 */

/*
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

/*============================================================================*/

#ifndef MEMORY_H
#define MEMORY_H

/*----------------------------------------------------------------------------
 *  nested include files
 *
 *
 *----------------------------------------------------------------------------*/
#if defined(__ICCAVR__)
#include <ioavr.h>
#include <intrinsics.h>
#include "inavr.h"
#elif defined(__GNUC__)
#include <avr/io.h>
#include <avr/interrupt.h>
#endif

#include <stdint.h>

/*----------------------------------------------------------------------------
 *  manifest constants
 *
 *
 *----------------------------------------------------------------------------*/

/* Memory Map */

/* Identification Info */
/* ! @ingroup RApp-API */
/* ! @{ */
#define CHIPID    0x55u /* HP */
#define SWVER     0xAAu /* Revision x.x */

#define NUM_STATUS_BYTES   5u
#define FIRST_STATUS_BYTE  2u
#define NO_MATCH           0xFF
#define MATCH              0x00

/*----------------------------------------------------------------------------
 *  type definitions
 *
 *
 *----------------------------------------------------------------------------*/

/* definition of memory map body */
typedef struct tag_memory_map_body_t {
	uint8_t id;                       /* Address  0 - hard-coded chip id */
	uint8_t major_minor;              /* Address  1 - hard-coded chip
	                                   * version */
	uint8_t sensor_status[2];            /* Address  3-4 - Key status */
	uint8_t wheel_position;           /* Address  5 - Wheel Position */
	uint8_t prox_status;              /* Address  6 - Proximity Sensor
	                                   * Status */
	uint8_t accelero_status;           /* Address  7 - Acclerometer Status
	                                   **/
} memory_map_body_t;

/* union allowing memory map to be addressed by field name, or as an array */
typedef union tag_memory_map_t {
	memory_map_body_t body;
	uint8_t array[sizeof(memory_map_body_t)];
} memory_map_t;
/* ! @} */

/*----------------------------------------------------------------------------
 *  macros
 *
 *
 *----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 *  extern variables
 *
 *
 *----------------------------------------------------------------------------*/

extern memory_map_t memory_map;  /* application memory map */
extern uint8_t sent_status_out[NUM_STATUS_BYTES];

extern uint8_t comms_match(void);

#endif /* MEMORY_H */

/*----------------------------------------------------------------------------
 *  End of file $RCSfile: memory_map.h $
 *
 *
 *----------------------------------------------------------------------------*/
