/**
 * \file *********************************************************************
 *
 * \brief memory map for QTouch
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
