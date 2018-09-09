/**
 * \file *********************************************************************
 *
 * \brief memory map for QTouch
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

#ifdef AVR477_QTOUCH
/*============================================================================*/

/*----------------------------------------------------------------------------
 *  compiler information
 *
 *
 *----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 *  include files
 *
 *
 *----------------------------------------------------------------------------*/

#include "memorymap.h"

/*----------------------------------------------------------------------------
 *  manifest constants
 *
 *
 *----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 *  type definitions
 *
 *
 *----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 *  prototypes
 *
 *
 *----------------------------------------------------------------------------*/

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

/*----------------------------------------------------------------------------
 *  global variables
 *
 *
 *----------------------------------------------------------------------------*/

/* application memory map */
memory_map_t memory_map;

uint8_t sent_status_out[NUM_STATUS_BYTES];

/*----------------------------------------------------------------------------
 *  static variables
 *
 *
 *----------------------------------------------------------------------------*/

/**
 * @brief Compares last sent values again current values of pertinent registers
 * @ingroup  QTouch
 * determine match based on key status registers, general status and
 *  slider position
 *
 * @return returns a NO_MATCH if registers don't match, MATCH if they do
 */
uint8_t comms_match(void)
{
	uint8_t i;

	for (i = 0u; i < NUM_STATUS_BYTES; i++) {
		if (sent_status_out[i] !=
				memory_map.array[(FIRST_STATUS_BYTE + i)]) {
			/* return no match if status regs don't match */
			return(NO_MATCH);
		}
	} /* Loop */
	return(MATCH); /* all values match */
} /* End of comms_match() */

#endif

/*----------------------------------------------------------------------------
 *  End of file $RCSfile: MemoryMap.c $
 *
 *
 *----------------------------------------------------------------------------*/
