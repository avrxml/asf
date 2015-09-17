/*******************************************************************************
*   $FILE:  eeprom_access.h
*   Atmel Corporation:  http://www.atmel.com \n
*   Support: http://www.atmel.com/design-support/
******************************************************************************/

/*  License
 * Copyright (c) 2010-2015 Atmel Corporation. All rights reserved.
*
*   Redistribution and use in source and binary forms, with or without
*   modification, are permitted provided that the following conditions are met:
*
*   1. Redistributions of source code must retain the above copyright notice,
*   this list of conditions and the following disclaimer.
*
*   2. Redistributions in binary form must reproduce the above copyright notice,
*   this list of conditions and the following disclaimer in the documentation
*   and/or other materials provided with the distribution.
*
*   3. The name of ATMEL may not be used to endorse or promote products derived
*   from this software without specific prior written permission.
*
*   THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
*   WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
*   MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY AND
*   SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT,
*   INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
*   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
*   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
*   ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
*   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
*   THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef EEPROM_ACCESS_H
#define EEPROM_ACCESS_H

/*----------------------------------------------------------------------------
nested include files
----------------------------------------------------------------------------*/
#include "touch_api.h"

#if defined ( __ICCAVR__ )
   #include <ioavr.h>
   #if !defined(__XMEGA_CORE__)   // Caution
      #define EEPROM_START_ADDRESS 0x0000u /* User Eeprom address */
   #else /* !defined(__ATxmega128A1_H) */
      #define EEPROM_START_ADDRESS 0x1000u /* User Eeprom address */
      #define _ATXMEGA_DEVICE_ 1u
   #endif /* !defined(__ATxmega128A1_H) */
#elif defined( __GNUC__ ) /* defined ( __ICCAVR__ ) */
   #include <avr/io.h>
   #if !defined(__AVR_XMEGA__)
      #define EEPROM_START_ADDRESS 0x0000u /* User Eeprom address */
   #else /* !defined(__AVR_ATxmega128A1__) */
      #define EEPROM_START_ADDRESS 0x1000u /* User Eeprom address */
      #define _ATXMEGA_DEVICE_ 1u
   #endif /* !defined(__AVR_ATxmega128A1__) */
#endif /* defined( __GNUC__ ) */

/*----------------------------------------------------------------------------
manifest constants
----------------------------------------------------------------------------*/
/* Block start addresses in EEPROM */
#define EEPROM_LIB_CONFIG_START_ADDRESS  (EEPROM_START_ADDRESS + 2u)

/*==============================================================================
Name    : write_global_settings_to_eeprom
--------------------------------------------------------------------------------
Purpose : Save global parameter settings to EEPROM
Input   :
Output  :
Notes   :
==============================================================================*/
void write_global_settings_to_eeprom(void);

/*==============================================================================
Name    : write_sensor_settings_to_eeprom
--------------------------------------------------------------------------------
Purpose : Save sensor parameter settings to EEPROM
Input   :
Output  :
Notes   :
==============================================================================*/
void write_sensor_settings_to_eeprom(void);

#ifdef _QMATRIX_
/*==============================================================================
Name    : write_burst_lenghts_to_eeprom
--------------------------------------------------------------------------------
Purpose : Save burst length settings to EEPROM
Input   :
Output  :
Notes   :
==============================================================================*/
void write_burst_lenghts_to_eeprom(void);
#endif

/*==============================================================================
Name    : read_info_from_eeprom
--------------------------------------------------------------------------------
Purpose : Read Info block from EEPROM
Input   :
Output  :
Notes   :
==============================================================================*/
uint16_t read_info_from_eeprom( void );

/*==============================================================================
Name    : write_info_to_eeprom
--------------------------------------------------------------------------------
Purpose : Save information from library and sign in the EEPROM
Input   :
Output  :
Notes   :
==============================================================================*/
void write_info_to_eeprom( uint16_t lib_version );

/*==============================================================================
Name    : read_settings_from_eeprom
--------------------------------------------------------------------------------
Purpose : Will read settings from EEPROM
Input   :
Output  :
Notes   : Data is stored in 'eeprom_gv_block[]' in the format shown above
==============================================================================*/
void read_settings_from_eeprom( void );

#endif /* EEPROM_ACCESS_H */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-EOF-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
