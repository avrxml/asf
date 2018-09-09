/**
 * \file *********************************************************************
 *
 * \brief Memory map for QMatrix file
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
